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

#ifndef WII_RESIZE_SCREEN_H
#define WII_RESIZE_SCREEN_H

#include <SDL.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Information about the resize operation
 */
typedef struct resize_info {
    float defaultX;
    float defaultY;
    float currentX;
    float currentY;
} resize_info;

/**
 * Displays the resize user interface
 *
 * @param   rinfo Information for the resize operation
 */
void wii_resize_screen_gui(resize_info* rinfo);

/**
 * Draws a border around the surface that is to be scaled.
 *
 * @param   surface The surface to scale
 * @param   startY The Y offset into the surface to scale
 * @param   height The height to scale
 */
void wii_resize_screen_draw_border(SDL_Surface* surface,
                                   int startY,
                                   int height);

#ifdef __cplusplus
}
#endif

#endif
