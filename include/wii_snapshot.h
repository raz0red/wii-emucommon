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

#ifndef WII_SNAPSHOT_H
#define WII_SNAPSHOT_H

#include <gctypes.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Deletes the snapshot for the current rom
 */
void wii_delete_snapshot();

/**
 * Saves the current games state to the specified save file
 *
 * @param   savefile The name of the save file to write state to. If this value
 *          is NULL, the default save name for the last rom is used.
 * @param   status_update Whether to update the status message
 */
void wii_save_snapshot(const char* savefile, BOOL status_update);

/**
 * Determines whether the specified snapshot is valid
 *
 * @return  0  = valid
 *          -1 = does not exist
 *          -2 = not a valid save (wrong size)
 */
int wii_check_snapshot(const char* savefile);

//
// Methods to be implemented by application
//

/**
 * Determines the save name for the specified rom file
 *
 * @param   romfile The name of the rom file
 * @param   buffer The buffer to write the save name to
 */
void wii_snapshot_handle_get_name(const char* romfile, char* buffer);

/**
 * Saves with the specified save name
 *
 * @param   filename The name of the save file
 * @return  Whether the save was successful
 */
BOOL wii_snapshot_handle_save(char* filename);

#ifdef __cplusplus
}
#endif

#endif
