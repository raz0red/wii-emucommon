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

/** The most recent status message */
char wii_status_message[WII_MENU_BUFF_SIZE] = "";

/** The status message display count down */
u32 wii_status_message_count = 0;

/** Whether we are installed on a USB drive */
BOOL wii_is_usb = FALSE;

/**
 * Updates the specified result string with the location of the file relative to
 * the appliation root directory.
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
 * Determines and stores the base application path
 *
 * @param   argc The count of main arguments
 * @param   argv The array of argument values
 */
void wii_set_app_path(int argc, char* argv[]) {
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

    if (strncasecmp("usb", app_path, 3) == 0) {
        wii_is_usb = TRUE;
    }
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
