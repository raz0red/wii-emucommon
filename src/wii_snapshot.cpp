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

#include "wii_app.h"
#include "wii_snapshot.h"
#include "wii_util.h"

/**
 * Deletes the snapshot for the current rom
 */
void wii_delete_snapshot() {
    if (wii_last_rom != NULL) {
        char filename[WII_MAX_PATH];
        wii_snapshot_handle_get_name(wii_last_rom, filename);

        int status = remove(filename);

        if (!status) {
            wii_menu_reset_indexes();
            wii_menu_move(wii_menu_stack[wii_menu_stack_head], 1);
        }

        wii_set_status_message(
            !status ? "Successly deleted saved state."
                    : "An error occurred attempting to delete saved state.");
    }
}

/**
 * Saves the current games state to the specified save file
 *
 * @param   savefile The name of the save file to write state to. If this value
 *          is NULL, the default save name for the last rom is used.
 * @param   status_update Whether to set the status message
 */
void wii_save_snapshot(const char* savefile, BOOL status_update) {
    bool succeeded = false;

    char filename[WII_MAX_PATH];
    filename[0] = '\0';

    if (savefile != NULL) {
        Util_strlcpy(filename, savefile, WII_MAX_PATH);
    } else if (wii_last_rom != NULL) {
        wii_snapshot_handle_get_name(wii_last_rom, filename);
    }

    if (strlen(filename) != 0) {
        // For some reason sometimes the save file seems to use a corrupted name
        // even though the string we are passing it looks correct. This may be
        // a libogc issue.... So, we retry until it works (or fail after 5
        // times)
        int i;
        for (i = 0; i < 5; i++) {
            remove(filename);
            succeeded = wii_snapshot_handle_save(filename);

            if (wii_check_snapshot(filename) == 0) {
                break;
            } else {
                succeeded = FALSE;
            }
        }
    }

    if (status_update) {
        wii_set_status_message(
            succeeded ? "Successly saved state."
                      : "An error occurred attempting to save state.");
    }
}

/**
 * Determines whether the specified snapshot is valid
 *
 * @return  0  = valid
 *          -1 = does not exist
 *          -2 = not a valid save (wrong size)
 */
int wii_check_snapshot(const char* savefile) {
    struct stat statbuf;
    int result = stat(savefile, &statbuf);
    return result < 0 ? -1 : 0;
}
