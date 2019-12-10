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

#include "wii_app.h"
#include "wii_config.h"
#include "wii_util.h"

/** The config file */
static char config_file[WII_MAX_PATH] = "";

/**
 * Returns the path to the config file
 *
 * @return  The path to the config file
 */
static char* get_config_file_path() {
    if (config_file[0] == '\0') {
        snprintf(config_file, WII_MAX_PATH, "%s%s", wii_get_fs_prefix(),
                 wii_get_config_file_path());
    }

    return config_file;
}

/**
 * Read the configuration file
 *
 * @return  Whether we read the configuration file successfully
 */
BOOL wii_read_config() {
    char buff[512];

    FILE* fp;
    fp = fopen(get_config_file_path(), "r");
    if (fp == NULL) {
        return FALSE;
    }

    while (fgets(buff, sizeof(buff), fp)) {
        char* ptr;
        Util_chomp(buff);
        ptr = strchr(buff, '=');
        if (ptr != NULL) {
            *ptr++ = '\0';
            Util_trim(buff);
            Util_trim(ptr);

            // Read the value
            wii_config_handle_read_value(buff, ptr);
        }
    }

    fclose(fp);

    return TRUE;
}

/**
 * Write the configuration file
 *
 * @return  Whether we wrote the configuration file successfully
 */
BOOL wii_write_config() {
    FILE* fp;
    fp = fopen(get_config_file_path(), "w");
    if (fp == NULL) {
        return FALSE;
    }

    // Write the configuration file
    wii_config_handle_write_config(fp);

    fclose(fp);

    return TRUE;
}