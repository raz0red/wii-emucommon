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

#include "wii_video.h"

/**
 * Converts RGB to Y1CbY2Cr format
 *
 * @param   r1 Red
 * @param   g1 Green
 * @param   b1 Blue
 * @return  Color in Y1CbY2Cr format
 */
u32 wii_video_rgb_to_y1cby2cr(u8 r1, u8 g1, u8 b1) {
    int y1, cb1, cr1, y2, cb2, cr2, cb, cr;
    u8 r2, g2, b2;

    r2 = r1;
    g2 = g1;
    b2 = b1;

    y1 = (299 * r1 + 587 * g1 + 114 * b1) / 1000;
    cb1 = (-16874 * r1 - 33126 * g1 + 50000 * b1 + 12800000) / 100000;
    cr1 = (50000 * r1 - 41869 * g1 - 8131 * b1 + 12800000) / 100000;

    y2 = (299 * r2 + 587 * g2 + 114 * b2) / 1000;
    cb2 = (-16874 * r2 - 33126 * g2 + 50000 * b2 + 12800000) / 100000;
    cr2 = (50000 * r2 - 41869 * g2 - 8131 * b2 + 12800000) / 100000;

    cb = (cb1 + cb2) >> 1;
    cr = (cr1 + cr2) >> 1;

    return ((y1 << 24) | (cb << 16) | (y2 << 8) | cr);
}

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
                         u8 b) {
    int i;

    u32 offset = (y * 320) + (startx >> 1);
    u32 color = wii_video_rgb_to_y1cby2cr(r, g, b);
    int width = (endx - startx) >> 1;

    for (i = 0; i < width; i++) {
        xfb[offset++] = color;
    }
}

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
                          u16 y) {
    u16 pix = 0, row = 0, col = 0;
    u16 offset = y * 320;

    x >>= 1;

    for (row = 0; row < props->imgHeight; row++) {
        for (col = 0; col < (props->imgWidth >> 1); col++) {
            framebuff[offset + col + x] = source[pix++];
        }
        offset += 320;
    }
}