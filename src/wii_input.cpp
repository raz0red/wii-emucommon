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

#include <math.h>

#include "wii_input.h"

#define PI 3.14159265f

/**
 * Returns the analog value for the joystick (-128.0 to 128.0)
 *
 * @param   exp The expansion (Nunchuk or Classic)
 * @param   isX Whether to get the X or Y axis value
 * @param   isRjs Whether we are reading for the right joystick
 * @return  The analog value for the joystick (-128.0 to 128.0)
 */
float wii_exp_analog_val(const expansion_t* exp, BOOL isX, BOOL isRjs) {
    return wii_exp_analog_val_range(exp, isX, isRjs, 128.0f);
}

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
                               float range) {
    float mag = 0.0;
    float ang = 0.0;

    if (exp->type == WPAD_EXP_CLASSIC) {
        if (isRjs) {
            mag = exp->classic.rjs.mag;
            ang = exp->classic.rjs.ang;
        } else {
            mag = exp->classic.ljs.mag;
            ang = exp->classic.ljs.ang;
        }
    } else if (exp->type == WPAD_EXP_NUNCHUK) {
        mag = exp->nunchuk.js.mag;
        ang = exp->nunchuk.js.ang;
    } else {
        return 0.0;
    }

    if (mag > 1.0)
        mag = 1.0;
    else if (mag < -1.0)
        mag = -1.0;
    double val =
        (isX ? mag * sin(PI * ang / 180.0f) : mag * cos(PI * ang / 180.0f));

    if (isnan(val)) {
        return 0.0f;
    }

    return val * range;
}

/**
 * Whether the right digital pad is pressed
 *
 * @param   wmHorizontal Whether the Wiimote is horizontal or vertical
 * @param   classic Whether to check the classic controller
 * @param   held The current held state
 * @return  Whether the right digital pad is pressed
 */
BOOL wii_digital_right(BOOL wmHorizontal, BOOL classic, u32 held) {
    return (held & ((wmHorizontal ? WPAD_BUTTON_DOWN : WPAD_BUTTON_RIGHT) |
                    (classic ? WPAD_CLASSIC_BUTTON_RIGHT : 0)));
}

/**
 * Whether the left digital pad is pressed
 *
 * @param   wmHorizontal Whether the Wiimote is horizontal or vertical
 * @param   classic Whether to check the classic controller
 * @param   held The current held state
 * @return  Whether the left digital pad is pressed
 */
BOOL wii_digital_left(BOOL wmHorizontal, BOOL classic, u32 held) {
    return (held & ((wmHorizontal ? WPAD_BUTTON_UP : WPAD_BUTTON_LEFT) |
                    (classic ? WPAD_CLASSIC_BUTTON_LEFT : 0)));
}

/**
 * Whether the up digital pad is pressed
 *
 * @param   wmHorizontal Whether the Wiimote is horizontal or vertical
 * @param   classic Whether to check the classic controller
 * @param   held The current held state
 * @return  Whether the up digital pad is pressed
 */
BOOL wii_digital_up(BOOL wmHorizontal, BOOL classic, u32 held) {
    return (held & ((wmHorizontal ? WPAD_BUTTON_RIGHT : WPAD_BUTTON_UP) |
                    (classic ? WPAD_CLASSIC_BUTTON_UP : 0)));
}

/**
 * Whether the down digital pad is pressed
 *
 * @param   wmHorizontal Whether the Wiimote is horizontal or vertical
 * @param   classic Whether to check the classic controller
 * @param   held The current held state
 * @return  Whether the down digital pad is pressed
 */
BOOL wii_digital_down(BOOL wmHorizontal, BOOL classic, u32 held) {
    return (held & ((wmHorizontal ? WPAD_BUTTON_LEFT : WPAD_BUTTON_DOWN) |
                    (classic ? WPAD_CLASSIC_BUTTON_DOWN : 0)));
}

/**
 * Waits until no buttons are pressed
 *
 * @param   joys The number of joysticks
 */
void wii_wait_until_no_buttons(int joys) {
    while (1) {
        WPAD_ScanPads();
        PAD_ScanPads();
        if (!wii_is_any_button_held(joys)) {
            break;
        }
        VIDEO_WaitVSync();
    }
}

/**
 * Checks to see if any buttons are held
 *
 * @param   joys The number of joysticks to check
 * @return  Whether any of the buttons are held
 */
BOOL wii_is_any_button_held(int joys) {
    int i;
    for (i = 0; i < joys; i++) {
        if (WPAD_ButtonsHeld(i) || PAD_ButtonsHeld(i)) {
            return TRUE;
        }
    }

    return FALSE;
}
