//---------------------------------------------------------------------------//
//                                                                           //
//  wii-emucommon:                                                           //
//  Wii emulator common code                                                 //
//                                                                           //
//  [github.com/raz0red/wii-emucommon]                                       //
//                                                                           //
//---------------------------------------------------------------------------//
//                                                                           //
//  Copyright (C) 2019 raz0red                                               //
//                                                                           //
//  This program is free software; you can redistribute it and/or            //
//  modify it under the terms of the GNU General Public License              //
//  as published by the Free Software Foundation; either version 2           //
//  of the License, or (at your option) any later version.                   //
//                                                                           //
//  This program is distributed in the hope that it will be useful,          //
//  but WITHOUT ANY WARRANTY; without even the implied warranty of           //
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            //
//  GNU General Public License for more details.                             //
//                                                                           //
//  You should have received a copy of the GNU General Public License        //
//  along with this program; if not, write to the Free Software              //
//  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA            //
//  02110-1301, USA.                                                         //
//---------------------------------------------------------------------------//

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <wiiuse/wpad.h>

#include "fileop.h"
#include "wii_input.h"
#include "wii_main.h"
#include "wii_hw_buttons.h"
#include "wii_app.h"

#include "gettext.h"

#ifdef WII_NETTRACE
#include <network.h>
#include "net_print.h"  
#endif

/** The base application path */
static char app_path[WII_MAX_PATH] = "";

/** The initial rom to execute (Wiiflow) */
char wii_initial_rom[WII_MAX_PATH] = "";

/** The most recent status message */
char wii_status_message[WII_MENU_BUFF_SIZE] = "";

/** The status message display count down */
u32 wii_status_message_count = 0;

/** Whether we are installed on a USB drive */
BOOL wii_is_usb = FALSE;

/**
 * Updates the specified result string with the location of the file relative to
 * the application root directory.
 *
 * @param   file The file name
 * @param   result The buffer to store the result in
 */
void wii_get_app_relative(const char* file, char* result) {
    snprintf(result, WII_MAX_PATH, "%s%s", app_path, file);
}

/**
 * Returns the base application path
 *
 * @return  The base application path
 */
char* wii_get_app_path() {
    return app_path;
}

/**
 * Attempts to find the "data drive" (contains the roms, config files, etc.).
 *
 * @param preferredFilePath A path containing the drive prefix that is preferred
 */
static BOOL find_and_mount_data_drive(const char* preferredFilePath) {
    DIR* dir;
    BOOL success = FALSE;
    BOOL checkUsb = strncasecmp("usb", preferredFilePath, 3) == 0;
    char dataDirPath[WII_MAX_PATH] = "";
    const char* dataDir = wii_get_data_path();  // Dir w/o file system prefix

    if (!dataDir) {
        // No data directory is required by the application
        success = TRUE;
    } else {
        for (int i = 0; !success && i < 2; i++) {
            // Generate path
            snprintf(dataDirPath, sizeof(dataDirPath), "%s:%s",
                     (checkUsb ? "usb" : "sd"), dataDir);
            // Mount drive
            if (ChangeInterface(dataDirPath, FS_RETRY_COUNT)) {
                // Attempt to find the data path
                if ((dir = opendir(dataDirPath)) != NULL) {
                    closedir(dir);
#ifdef WII_NETTRACE
                    net_print_string(__FILE__, __LINE__,
                                     "Found data path: %s\n", dataDirPath);
#endif
                    // Mark whether the data is on the usb or sd
                    wii_is_usb = checkUsb;
                    // Exit the check loop
                    success = TRUE;
                }
            }
            checkUsb = !checkUsb;
        }
    }

    return success;
}

/**
 * Processes the application arguments
 *
 * @param   argc The count of main arguments
 * @param   argv The array of argument values
 * @return  Whether the processing was successful
 */
BOOL wii_process_app_args(int argc, char* argv[]) {
    char message[255] = "";
    BOOL success = TRUE;

    // Handle application path
    snprintf(app_path, WII_MAX_PATH, "%s", wii_get_app_base_dir());
    if ((argc > 0) && (argv[0] != NULL) &&
        (strchr(argv[0], ':') != NULL))  // To support wiiload
    {
#ifdef WII_NETTRACE
        net_print_string(__FILE__, __LINE__, "Found drive prefix\n");
#endif
        char temp_path[WII_MAX_PATH];
        snprintf(temp_path, WII_MAX_PATH, "%s", argv[0]);

        char* loc;

        // Remove the file name
        loc = strrchr(temp_path, '/');
        if (loc != NULL) {
            *loc = '\0';
        }

        // Convert the prefix to lowercase (consistent with our mapped drive)
        for (loc = temp_path; *loc && *loc != ':'; ++loc) {
            *loc = tolower(*loc);
        }

        snprintf(app_path, WII_MAX_PATH, "%s/", temp_path);
    } else {
#ifdef WII_NETTRACE
        net_print_string(__FILE__, __LINE__, "No drive prefix.");
#endif
    }

    // Handle initial rom (Wiiflow)
    if (argc > 1 && argv[1] != NULL) {
        const char* path = argv[1];
        int pathLen = strlen(path);
        if (pathLen > 0) {
            // Was name passed separately?
            // Check to see if it has a value with a length and does not start
            // with usb: or sd:
            const char* name =
                (argc > 2 && argv[2] != NULL && strlen(argv[2]) > 0 &&
                 strncasecmp(argv[2], "usb:", 4) &&
                 strncasecmp(argv[2], "sd:", 3))
                    ? argv[2]
                    : NULL;
            snprintf(wii_initial_rom, WII_MAX_PATH, "%s%s%s", path,
                     name && path[pathLen - 1] != '/' ? "/" : "",
                     name ? name : "");
#ifdef WII_NETTRACE
            net_print_string(__FILE__, __LINE__, "Wiiflow initial rom: %s\n",
                             wii_initial_rom);
#endif
        }
    }

    // Attempt to mount application drive
    if (!ChangeInterface(app_path, FS_RETRY_COUNT)) {
        snprintf(message, sizeof(message),
                 "Unable to mount file system for application: %s", app_path);
        wii_display_console_message_and_pause(message);
        success = FALSE;
    }

    if (success) {
        // Attempt to find the data drive and mount it
        success = find_and_mount_data_drive(
            wii_initial_rom[0] != '\0' ? wii_initial_rom : app_path);

        if (!success) {
            snprintf(message, sizeof(message),
                     "Unable to find and mount data drive");
            wii_display_console_message_and_pause(message);
            success = FALSE;
        }
    }

    return success;
}

/**
 * Returns the file system prefix
 *
 * @return  The file system prefix
 */
const char* wii_get_fs_prefix() {
    return (const char*)(wii_is_usb ? "usb:" : "sd:");
}

/**
 * Pause and wait for input. Usually used when debugging.
 */
void wii_pause() {
    int done = 0;
    while (!done) {
        if (wii_check_button_pressed()) {
            done = 1;
        }
    }
}

/**
 * Checks whether a button was pressed
 *
 * @return  1 if a button was pressed, -1 if home/hadware was pressed
 */
int wii_check_button_pressed() {
    WPAD_ScanPads();
    PAD_ScanPads();

    expansion_t exp;
    WPAD_Expansion(0, &exp);
    bool isClassic = (exp.type == WPAD_EXP_CLASSIC);

    u32 pressed = WPAD_ButtonsDown(0);
    u32 held = WPAD_ButtonsHeld(0);
    u32 gcPressed = PAD_ButtonsDown(0);
    u32 gcHeld = PAD_ButtonsHeld(0);

    if ((pressed & WII_BUTTON_HOME) || (gcPressed & GC_BUTTON_HOME) ||
        (wii_hw_button)) {
        return -1;
    }

    return (
        (held &
         (WPAD_BUTTON_A | WPAD_BUTTON_B | WPAD_BUTTON_1 | WPAD_BUTTON_2 |
          WPAD_BUTTON_PLUS | WPAD_BUTTON_MINUS |
          (isClassic ? (WPAD_CLASSIC_BUTTON_PLUS | WPAD_CLASSIC_BUTTON_MINUS |
                        WPAD_CLASSIC_BUTTON_X | WPAD_CLASSIC_BUTTON_A |
                        WPAD_CLASSIC_BUTTON_Y | WPAD_CLASSIC_BUTTON_B)
                     : (WPAD_NUNCHUK_BUTTON_Z | WPAD_NUNCHUK_BUTTON_C)))) ||
        (gcHeld & (PAD_BUTTON_START | PAD_BUTTON_A | PAD_BUTTON_B |
                   PAD_BUTTON_X | PAD_BUTTON_Y)));
}

/**
 * Writes the specified string to the VT display
 *
 * @param   buffer The buffer to write to the display
 */
void wii_write_vt(char* buffer) {
    printf(buffer);
}

/**
 * Initializes the console output to the specified frame buffer. This allows us
 * to swap between console output buffers.
 *
 * @param   fb The frame buffer to associate with the console
 */
void wii_console_init(void* fb) {
    console_init(fb, 20, 20, vmode->fbWidth, vmode->xfbHeight,
                 vmode->fbWidth * VI_DISPLAY_PIX_SZ);
}

/**
 * Stores a status message for display in the footer of the menu
 *
 * @param message   The status message
 */
void wii_set_status_message(const char* message) {
    // LOCK_RENDER_MUTEX();
    wii_status_message_count = (wii_is_pal ? 50 : 60) * 5;  // 5 seconds
    snprintf(wii_status_message, sizeof(wii_status_message), "%s",
             gettextmsg(message));
    // UNLOCK_RENDER_MUTEX();
}
