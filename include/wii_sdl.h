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

#ifndef WII_SDL_H
#define WII_SDL_H

#include <SDL.h>
#include <SDL_ttf.h>

#include <gctypes.h>

#ifdef __cplusplus
extern "C" {
#endif

/** The Wii surface */
extern SDL_Surface* back_surface;
/** The Blit surface */
extern SDL_Surface* blit_surface;

// Fonts
extern TTF_Font *sdl_font_18;
extern TTF_Font *sdl_font_14;
extern TTF_Font *sdl_font_13;
extern TTF_Font *sdl_font_12;

// Colors
extern SDL_Color SDL_COLOR_WHITE;
extern SDL_Color SDL_COLOR_BLACK;
extern SDL_Color SDL_COLOR_RED;

/**
 * Initializes the SDL
 * 
 * @return  Non-zero if was successful
 */
int wii_sdl_init();

/**
 * Maps the specified color into the back surface.
 *
 * @param   R The red value
 * @param   G The green value
 * @param   B The blue value
 * @return  The index of the specified color
 */
uint wii_sdl_rgb(u8 R, u8 G, u8 B);

/**
 * Renders the current back surface
 */
void wii_sdl_flip();

/**
 * Renders black to both blit and back surfaces
 */
void wii_sdl_black_screen();

/*
 * Blacks the back surface
 */
void wii_sdl_black_back_surface();

/**
 * Renders the blit surface to the Wii surface.
 *
 * @param scale  The scale to render the surface (1 or 2)
 */
void wii_sdl_put_image_normal(int scale);

/**
 * Renders text to the Wii surface
 *
 * @param   font The font to render with
 * @param   text The text to display
 * @param   destRect The location to render the text
 * @param   colorFG The foreground color (optional)
 * @param   colorBG The background color (optional)
 */
void wii_sdl_render_text(TTF_Font* font,
                         const char* text,
                         SDL_Rect* destRect,
                         SDL_Color* colorFG,
                         SDL_Color* colorBG);

/**
 * Determines the size of the text displayed with the specified font
 *
 * @param   font The font
 * @param   text The text
 * @param   w The width (return value)
 * @param   h The height (return value)
 */
void wii_sdl_get_text_size(TTF_Font* font, const char* text, int* w, int* h);

/**
 * Renders a filled rectangle to the back (Wii) surface
 *
 * @param   surface The surface to render to
 * @param   x The x location
 * @param   y The y location
 * @param   w The width
 * @param   h The height
 * @param   color The color
 */
void wii_sdl_fill_rectangle(SDL_Surface* surface,
                            int x,
                            int y,
                            int w,
                            int h,
                            u32 color);

/**
 * Renders a rectangle to the back (Wii) surface
 *
 * @param   surface The surface to render to
 * @param   x The x location
 * @param   y The y location
 * @param   w The width
 * @param   h The height
 * @param   border The border color
 * @param   exor Whether to exclusive or the rectangle's lines
 */
void wii_sdl_draw_rectangle(SDL_Surface* surface,
                            int x,
                            int y,
                            int w,
                            int h,
                            u32 border,
                            BOOL exor);

/**
 * Frees the SDL resources
 */
void wii_sdl_free_resources();

/** Locks the render mutex */
void LOCK_RENDER_MUTEX();

/** Unlocks the render mutex */
extern void UNLOCK_RENDER_MUTEX();

//
// Methods to be implemented by application
//

/**
 * Initializes the SDL
 * 
 * @return  Non-zero if successful
 */
int wii_sdl_handle_init();

#ifdef __cplusplus
}
#endif

#endif
