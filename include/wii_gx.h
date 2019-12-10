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

#ifndef WII_GX_H
#define WII_GX_H

#include <gccore.h>

#include "FreeTypeGX.h"

#ifdef __cplusplus
extern "C" {
#endif

#define GX_X(x) (-320 + (x))
#define GX_Y(y) (240 - (y))

/*
 * Structure for loading images
 */
typedef struct gx_imagedata {
    u8* data;
    int width;
    int height;
} gx_imagedata;

/**
 * Pushes the specified callback. It will be set as the active callback.
 *
 * @param   rendercallback The callback to activate
 * @param   renderscreen Whether to render the SDL screen (main surface)
 * @param   precallback The pre-render callback
 */
void wii_gx_push_callback(void (*rendercallback)(void),
                          BOOL renderscreen,
                          void (*precallback)(void));

/**
 * Pops the active callback. The previous callback (if it exists) will be
 * restored.
 */
void wii_gx_pop_callback();

/**
 * Draws a rectangle at the specified position
 *
 * @param   x The x position
 * @param   y The y position
 * @param   width The width of the rectangle
 * @param   height The height of the rectangle
 * @param   color The color of the rectangle
 * @param   filled Whether the rectangle is filled
 */
void wii_gx_drawrectangle(int x,
                          int y,
                          int width,
                          int height,
                          GXColor color,
                          BOOL filled);

/**
 * Draws text at the specified position
 *
 * @param   x The x position
 * @param   y The y position
 * @param   pixelSize The pixel size
 * @param   color The color of the text
 * @param   textStyle The style(s) for the text (FreeTypeGX)
 */
void wii_gx_drawtext(int16_t x,
                     int16_t y,
                     FT_UInt pixelSize,
                     const char* text,
                     GXColor color,
                     uint16_t textStyle);

/**
 * Returns the width of the specified text
 *
 * @param   pixelSize The pixel size
 * @param   text The text
 * @return  The width of the specified text
 */
uint16_t wii_gx_gettextwidth(FT_UInt pixelSize, char* text);

/**
 * Draws the image at the specified position
 *
 * @param   xpos The x position
 * @param   ypos The y position
 * @param   width The image width
 * @param   height The image height
 * @param   data The image data
 * @param   degress The rotation degrees
 * @param   scaleX How much to scale the X
 * @param   scaleY How much to scale the Y
 * @param   alpha The alpha amount
 */
void wii_gx_drawimage(int xpos,
                      int ypos,
                      u16 width,
                      u16 height,
                      u8 data[],
                      f32 degrees,
                      f32 scaleX,
                      f32 scaleY,
                      u8 alpha);

/**
 * Loads and returns the data for the image at the specified path
 *
 * @param   imgpath The path to the image
 * @return  The data for the loaded image
 */
gx_imagedata* wii_gx_loadimage(char* imgpath);

/**
 * Loads image data for the specified image buffer
 *
 * @param   buff The image buffer
 * @return  The data for the loaded image
 */
gx_imagedata* wii_gx_loadimagefrombuff(const u8* buff);

/**
 * Frees the specified image data information
 *
 * @param   data The image data to release
 */
void wii_gx_freeimage(gx_imagedata* imgdata);

#ifdef __cplusplus
}
#endif

#endif
