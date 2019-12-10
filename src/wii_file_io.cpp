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

#include <gccore.h>

#include <fat.h>
#include <sdcard/wiisd_io.h>
#include <ogc/usbstorage.h>
#include <SDL.h>

#ifdef WII_NETTRACE
#include <network.h>
#include "net_print.h"  
#endif

#include "wii_app.h"

/** Is the file system mounted? */
static BOOL mounted = FALSE;

/**
 * Unmounts the file system
 */
void wii_unmount() {
    if (mounted) {
        if (wii_is_usb) {
            fatUnmount("usb:/");
            __io_usbstorage.shutdown();
        } else {
            fatUnmount("sd:/");
            __io_wiisd.shutdown();
        }

        mounted = FALSE;
    }
}

/**
 * Mounts the file system
 *
 * @return  Whether we mounted the file system successfully
 */
BOOL wii_mount() {
    if (!mounted) {
        if (wii_is_usb) {
            if (!__io_usbstorage.startup() ||
                !fatMountSimple("usb", &__io_usbstorage)) {
                return FALSE;
            }
        } else {
            if (!__io_wiisd.startup() || !fatMountSimple("sd", &__io_wiisd)) {
                return FALSE;
            }
        }

        chdir(wii_get_app_path());

        mounted = TRUE;
    }

    return TRUE;
}