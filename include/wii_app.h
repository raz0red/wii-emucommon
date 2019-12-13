//---------------------------------------------------------------------------//
//                                                                           //
//  wii-emucommon:                                                           //
//  Wii emulator common code                                                 //
//                                                                           //
//  [github.com/raz0red/wiicolem]                                            //
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

#ifndef WII_APP_H
#define WII_APP_H

#include "wii_main.h"

#ifdef __cplusplus
extern "C" {
#endif

/** The most recent status message */
extern char wii_status_message[WII_MENU_BUFF_SIZE];

/** The initial rom to execute (Wiiflow) */
extern char wii_initial_rom[WII_MAX_PATH];

/** The status message display count down */
extern u32 wii_status_message_count;

/** Whether the application is installed on a USB drive */
extern BOOL wii_is_usb;

/**
 * Updates the specified result string with the location of the file relative to
 * the application root directory.
 *
 * @param   file The file name
 * @param   result The buffer to store the result in
 */
void wii_get_app_relative(const char* file, char* result);

/**
 * Returns the base application path
 *
 * @param   return The base application path
 */
char* wii_get_app_path();

/**
 * Processes the application arguments
 *
 * @param   argc The count of main arguments
 * @param   argv The array of argument values
 * @return  Whether the processing was successful
 */
BOOL wii_process_app_args(int argc, char* argv[]);

/**
 * Pause and wait for input. Usually used when debugging.
 */
void wii_pause();

/**
 * Checks whether a button was pressed
 *
 * @return  1 if a button was pressed, -1 if home/hadware was pressed
 */
int wii_check_button_pressed();

/**
 * Writes the specified string to the VT display
 */
void wii_write_vt(char* buffer);

/**
 * Initializes the console output to the specified frame buffer. This allows us
 * to swap between console output buffers.
 *
 * @param   fb The frame buffer to associate with the console
 */
void wii_console_init(void* fb);

/**
 * Stores a status message for display in the footer of the menu
 *
 * @param   message The message to display in the footer of the menu
 */
void wii_set_status_message(const char* message);

/**
 * Returns the file system prefix
 *
 * @return  The file system prefix
 */
const char* wii_get_fs_prefix();

/**
 * Returns the base directory for the application
 * 
 * @return  The base directory for the application
 */
const char* wii_get_app_base_dir();

#ifdef __cplusplus
}
#endif

#endif
