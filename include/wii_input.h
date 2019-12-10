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

#ifndef WII_INPUT_H
#define WII_INPUT_H

#include <wiiuse/wpad.h>

#ifdef __cplusplus
extern "C" {
#endif

// UI Navigation, Standard Buttons
#define WII_BUTTON_LEFT ( WPAD_BUTTON_LEFT )
#define WII_CLASSIC_BUTTON_LEFT ( WPAD_CLASSIC_BUTTON_LEFT )
#define GC_BUTTON_LEFT ( PAD_BUTTON_LEFT )
#define WII_BUTTON_RIGHT ( WPAD_BUTTON_RIGHT | WPAD_CLASSIC_BUTTON_RIGHT )
#define GC_BUTTON_RIGHT ( PAD_BUTTON_RIGHT )
#define WII_BUTTON_UP ( WPAD_BUTTON_UP  )
#define WII_CLASSIC_BUTTON_UP ( WPAD_CLASSIC_BUTTON_UP  )
#define GC_BUTTON_UP ( PAD_BUTTON_UP )
#define WII_BUTTON_DOWN ( WPAD_BUTTON_DOWN | WPAD_CLASSIC_BUTTON_DOWN  )
#define GC_BUTTON_DOWN ( PAD_BUTTON_DOWN )
#define WII_BUTTON_ENTER ( WPAD_BUTTON_A | WPAD_BUTTON_2 )
#define WII_CLASSIC_BUTTON_ENTER ( WPAD_CLASSIC_BUTTON_A )
#define WII_NUNCHUK_BUTTON_ENTER ( WPAD_NUNCHUK_BUTTON_C )
#define GC_BUTTON_ENTER ( PAD_BUTTON_A )
#define WII_BUTTON_ESC ( WPAD_BUTTON_B | WPAD_BUTTON_1 )
#define WII_CLASSIC_BUTTON_ESC ( WPAD_CLASSIC_BUTTON_B )
#define WII_NUNCHUK_BUTTON_ESC ( WPAD_NUNCHUK_BUTTON_Z )
#define GC_BUTTON_ESC ( PAD_BUTTON_B )
#define WII_BUTTON_HOME ( WPAD_BUTTON_HOME | WPAD_CLASSIC_BUTTON_HOME )
#define GC_BUTTON_HOME ( PAD_TRIGGER_Z )

// General
#define WII_BUTTON_A WII_BUTTON_ENTER
#define WII_CLASSIC_BUTTON_A WII_CLASSIC_BUTTON_ENTER
#define WII_NUNCHUK_BUTTON_A WII_NUNCHUK_BUTTON_ENTER
#define GC_BUTTON_A GC_BUTTON_ENTER
#define WII_BUTTON_B WII_BUTTON_ESC
#define WII_CLASSIC_BUTTON_B WII_CLASSIC_BUTTON_ESC
#define WII_NUNCHUK_BUTTON_B WII_NUNCHUK_BUTTON_ESC
#define GC_BUTTON_B GC_BUTTON_ESC

/**
 * Returns the analog value for the joystick (-128.0 to 128.0)
 *
 * @param   exp The expansion (Nunchuk or Classic)
 * @param   isX Whether to get the X or Y axis value
 * @param   isRjs Whether we are reading for the right joystick
 * @return  The analog value for the joystick (-128.0 to 128.0)
 */
float wii_exp_analog_val(const expansion_t* exp, BOOL isX, BOOL isRjs);

/**
 * Returns the analog value for the joystick
 *
 * @param   exp The expansion (Nunchuk or Classic)
 * @param   isX Whether to get the X or Y axis value
 * @param   isRjs Whether we are reading for the right joystick
 * @param   range The analog range (+/-)
 * @return  The analog value for the joystick
 */
float wii_exp_analog_val_range(const expansion_t* exp,
                               BOOL isX,
                               BOOL isRjs,
                               float range);

inline BOOL wii_analog_right(float expX, s8 gcX) {
    return expX > 60 || gcX > 46;
}
inline BOOL wii_analog_left(float expX, s8 gcX) {
    return expX < -60 || gcX < -46;
}
inline BOOL wii_analog_up(float expY, s8 gcY) {
    return expY > 70 || gcY > 54;
}
inline BOOL wii_analog_down(float expY, s8 gcY) {
    return expY < -70 || gcY < -54;
}

/**
 * Whether the right digital pad is pressed
 *
 * @param   wmHorizontal Whether the Wiimote is horizontal or vertical
 * @param   classic Whether to check the classic controller
 * @param   held The current held state
 * @return  Whether the right digital pad is pressed
 */
BOOL wii_digital_right(BOOL wmHorizontal, BOOL classic, u32 held);

/**
 * Whether the left digital pad is pressed
 *
 * @param   wmHorizontal Whether the Wiimote is horizontal or vertical
 * @param   classic Whether to check the classic controller
 * @param   held The current held state
 * @return  Whether the left digital pad is pressed
 */
BOOL wii_digital_left(BOOL wmHorizontal, BOOL classic, u32 held);

/**
 * Whether the up digital pad is pressed
 *
 * @param   wmHorizontal Whether the Wiimote is horizontal or vertical
 * @param   classic Whether to check the classic controller
 * @param   held The current held state
 * @return  Whether the up digital pad is pressed
 */
BOOL wii_digital_up(BOOL wmHorizontal, BOOL classic, u32 held);

/**
 * Whether the down digital pad is pressed
 *
 * @param   wmHorizontal Whether the Wiimote is horizontal or vertical
 * @param   classic Whether to check the classic controller
 * @param   held The current held state
 * @return  Whether the down digital pad is pressed
 */
BOOL wii_digital_down(BOOL wmHorizontal, BOOL classic, u32 held);

/**
 * Waits until no buttons are pressed
 *
 * @param   joys The number of joysticks
 */
void wii_wait_until_no_buttons(int joys);

/**
 * Checks to see if any buttons are held
 *
 * @param   joys The number of joysticks to check
 * @return  Whether any of the buttons are held
 */
BOOL wii_is_any_button_held(int joys);

#ifdef __cplusplus
}
#endif

#endif
