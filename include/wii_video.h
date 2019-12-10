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

#ifndef WII_VIDEO_H
#define WII_VIDEO_H

#include <gctypes.h>

#include "pngu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Converts RGB to Y1CbY2Cr format
 *
 * @param   r1 Red
 * @param   g1 Green
 * @param   b1 Blue
 * @return  Color in Y1CbY2Cr format
 */
u32 wii_video_rgb_to_y1cby2cr(u8 r1, u8 g1, u8 b1);

/**
 * Draws a line
 *
 * @param   xfb The frame buffer
 * @param   startx The starting x position
 * @param   endx The ending x position
 * @param   y The y location
 * @param   r Red
 * @param   g Green
 * @param   b Blue
 */
void wii_video_draw_line(u32* xfb,
                         int startx,
                         int endx,
                         int y,
                         u8 r,
                         u8 g,
                         u8 b);

/**
 * Draws the image specified to the frame buffer
 *
 * @param   props The image properties
 * @param   source The bytes of the image
 * @param   framebuff The destination frame buffer
 * @param   x The x location to display the image in the buffer
 * @param   y The y location to disploay the image in the buffer
 */
void wii_video_draw_image(PNGUPROP* props,
                          u32* source,
                          u32* framebuff,
                          u16 x,
                          u16 y);

#ifdef __cplusplus
}
#endif                          

#endif
