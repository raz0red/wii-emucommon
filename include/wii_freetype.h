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

#ifndef WII_FREETYPE_H
#define WII_FREETYPE_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initializes the FreeType library
 *
 * @return  If an error occurred (non-zero)
 */
int wii_ft_init();

/**
 * Sets the font size
 *
 * @param   pixelsize The font size
 * @return  If an error occurred (non-zero)
 */
int wii_ft_set_fontsize(int pixelsize);

/**
 * Sets the font color
 *
 * @param   r Red
 * @param   g Green
 * @param   b Blue
 */
void wii_ft_set_fontcolor(u8 r, u8 g, u8 b);

/**
 * Draws the specified text
 *
 * @param   xfb The framebuffer
 * @param   x The x location
 *          (-1 auto center, -2 left of center, -3 right of center)
 * @param   y The y location
 * @param   text The text to draw
 */
void wii_ft_drawtext(u32* xfb, int x, int y, char* text);

/**
 * Calculates the width of the specified text
 *
 * @param   text The text
 * @return  The width of the text
 */
int wii_ft_get_textwidth(char* text);

#ifdef __cplusplus
}
#endif

#endif

