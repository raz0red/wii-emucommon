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

#ifndef WII_CONFIG_H
#define WII_CONFIG_H

#include <gctypes.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Read the configuration file
 *
 * @return  Whether the configuration file was read successfully
 */
BOOL wii_read_config();

/**
 * Write the configuration file
 *
 * return   Whether the configuration file was written successfully
 */
BOOL wii_write_config();

//
// Methods to be implemented by application
//

/**
 * Handles reading a particular configuration value
 *
 * @param   name The name of the config value
 * @param   value The config value
 */
void wii_config_handle_read_value(char* name, char* value);

/**
 * Handles the writing of the configuration file
 *
 * @param   fp The file pointer
 */
void wii_config_handle_write_config(FILE* fp);

/**
 * Returns the location of the config file
 * 
 * @return  The location of the config file
 */
const char* wii_get_config_file_path();

#ifdef __cplusplus
}
#endif

#endif
