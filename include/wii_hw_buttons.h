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

#ifndef WII_HW_BUTTONS_H
#define WII_HW_BUTTONS_H

#include <gctypes.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Hardware buttons (reset, power, etc.) */
extern u8 wii_hw_button;

/**
 * Callback for the reset button on the Wii.
 */
void wii_reset_pressed(u32 irq, void* ctx);

/**
 * Callback for the power button on the Wii.
 */
void wii_power_pressed();

/**
 * Callback for the power button on the Wiimote.
 *
 * @param   chan The Wiimote that pressed the button
 */
void wii_mote_power_pressed(s32 chan);

/**
 * Registers the hardware button callbacks
 */
void wii_register_hw_buttons();

#ifdef __cplusplus
}
#endif

#endif
