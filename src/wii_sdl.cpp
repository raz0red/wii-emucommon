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

#include <gctypes.h>

#include "wii_app.h"
#include "wii_sdl.h"

#ifdef WII_NETTRACE
#include <network.h>
#include "net_print.h"
#endif

/** The Wii surface */
SDL_Surface* back_surface = NULL;
/** The BLIT surface */
SDL_Surface* blit_surface = NULL;
// The render mutex
static SDL_mutex* render_mutex = NULL;
/** mutext lock count */
static int mutex_lock_count = 0;

// Fonts
TTF_Font* sdl_font_18 = NULL;
TTF_Font* sdl_font_14 = NULL;
TTF_Font* sdl_font_13 = NULL;
TTF_Font* sdl_font_12 = NULL;

// Colors
SDL_Color SDL_COLOR_WHITE = {255, 255, 255, 0};
SDL_Color SDL_COLOR_BLACK = {0, 0, 0, 0};
SDL_Color SDL_COLOR_RED = {255, 0, 0, 0};

/** Locks the render mutex */
void LOCK_RENDER_MUTEX() {
    if (render_mutex != NULL) {
        SDL_mutexP(render_mutex);
        mutex_lock_count++;
#ifdef WII_NETTRACE
        // net_print_string( NULL, 0, "RenderMutexLock:%d\n", mutex_lock_count
        // );
#endif
    }
}

/** Unlocks the render mutex */
void UNLOCK_RENDER_MUTEX() {
    if (render_mutex != NULL) {
        --mutex_lock_count;
#ifdef WII_NETTRACE
        // net_print_string( NULL, 0, "RenderMutexUnlock:%d\n", mutex_lock_count
        // );
#endif
        if (!mutex_lock_count) {
#ifdef WII_NETTRACE
            // net_print_string( NULL, 0, "MutexUnlocked\n" );
#endif
            SDL_mutexV(render_mutex);
        }
    }
}

/**
 * Maps the specified color into the back surface.
 *
 * @return  The index of the specified color
 */
uint wii_sdl_rgb(u8 R, u8 G, u8 B) {
    return SDL_MapRGB(back_surface->format, R, G, B);
}

/**
 * Renders black to both blit and back surfaces
 */
void wii_sdl_black_screen() {
    SDL_FillRect(blit_surface, NULL,
                 SDL_MapRGB(blit_surface->format, 0x0, 0x0, 0x0));
    SDL_Flip(blit_surface);
    wii_sdl_black_back_surface();
}

/**
 * Blacks the back surface
 */
void wii_sdl_black_back_surface() {
    SDL_FillRect(back_surface, NULL,
                 SDL_MapRGB(back_surface->format, 0x0, 0x0, 0x0));
    SDL_Flip(back_surface);
}

/**
 * Renders to the Wii surface.
 *
 * @param   scale The scale to render the surface (1 or 2)
 */
void wii_sdl_put_image_normal(int scale) {
#if 0
    // 16bpp+
    SDL_Rect srcRect = {0, 0, blit_surface->w, blit_surface->h};
    SDL_Rect destRect = {(WII_WIDTH - blit_surface->w) / 2,
                         (WII_HEIGHT - blit_surface->h) / 2, 0, 0};

    SDL_BlitSurface(blit_surface, &srcRect, back_surface, &destRect);
#else
    // 8bpp
    int offsetx = ((WII_WIDTH - (blit_surface->w * scale)) / 2);
    int offsety = ((WII_HEIGHT - (blit_surface->h * scale)) / 2);

    int src = 0, dst = 0, start = 0, x = 0, y = 0, i = 0;
    u8* backpixels = (u8*)back_surface->pixels;
    u8* blitpixels = (u8*)blit_surface->pixels;
    int startoffset = 0;
    for (y = 0; y < blit_surface->h; y++) {
        for (i = 0; i < scale; i++) {
            start = startoffset + (y * blit_surface->w);
            src = 0;
            dst = ((((y * scale) + i) + offsety) * WII_WIDTH) + offsetx;
            for (x = 0; x < blit_surface->w; x++) {
                int j;
                for (j = 0; j < scale; j++) {
                    backpixels[dst++] = blitpixels[start + src];
                }
                src++;
            }
        }
    }
#endif
}

/**
 * Determines the size of the text displayed with the specified font
 *
 * @param   font The font
 * @param   text The text
 * @param   w The width (return value)
 * @param   h The height (return value)
 */
void wii_sdl_get_text_size(TTF_Font* font, const char* text, int* w, int* h) {
    TTF_SizeText(font, text, w, h);
}

/**
 * Renders text to the Wii surface
 *
 * @param   font The font to render with
 * @param   text The text to display
 * @param   destRect The location to render the text
 * @param   colorFG The forground color (optional)
 * @param   colorBG The background color (optional)
 */
void wii_sdl_render_text(TTF_Font* font,
                         const char* text,
                         SDL_Rect* destRect,
                         SDL_Color* colorFG,
                         SDL_Color* colorBG) {
    SDL_Surface* sText = NULL;

    if (colorBG != NULL) {
        sText = TTF_RenderText_Shaded(
            font, text, (colorFG != NULL ? *colorFG : SDL_COLOR_WHITE),
            SDL_COLOR_BLACK);
    } else {
        sText = TTF_RenderText_Solid(
            font, text, (colorFG != NULL ? *colorFG : SDL_COLOR_WHITE));
    }

    if (sText == NULL) {
        return;
    }

    SDL_Rect srcRect = {0, 0, destRect->w, destRect->h};
    SDL_Rect* srcRectPtr = NULL;
    if (destRect->w > 0 && destRect->h > 0) {
        srcRectPtr = &srcRect;
    }

    SDL_Surface* sTextDF = SDL_DisplayFormat(sText);
    SDL_FreeSurface(sText);
    SDL_BlitSurface(sTextDF, srcRectPtr, back_surface, destRect);
    SDL_FreeSurface(sTextDF);
}

/**
 * Returns a pointer to the specified x,y location in the Wii surface
 *
 * @param   surface The surface
 * @param   x The x location
 * @param   y The y location
 * @return  A pointer to the specified x,y location in the Wii surface
 */
static void* wii_sdl_get_vram_addr(SDL_Surface* surface, uint x, uint y) {
    return ((u8*)surface->pixels) + surface->offset +
           (x * surface->format->BytesPerPixel) + (y * surface->pitch);
}

/**
 * Renders a rectangle to the back (Wii) surface
 *
 * @param   surface The surface to render to
 * @param   x The x location
 * @param   y The y location
 * @param   w The width
 * @param   hThe height
 * @param   border The border color
 * @param   xor Exclusive or
 */
void wii_sdl_draw_rectangle(SDL_Surface* surface,
                            int x,
                            int y,
                            int w,
                            int h,
                            u32 border,
                            BOOL exor) {
    if (x < 0) {
        w += x;
        x = 0;
    }
    if (y < 0) {
        h += y;
        y = 0;
    }
    if ((x + w) > surface->w)
        w = surface->w - x;
    if ((y + h) > surface->h)
        h = surface->h - y;
    if (w <= 0 || h <= 0)
        return;

    BOOL is8 = surface->format->BytesPerPixel == 1;
    BOOL is16 = surface->format->BytesPerPixel == 2;

    int xo, yo;
    if (exor) {
        for (xo = x + 1; xo < (x + w - 1); xo++) {
            if (is8)
                *((u8*)wii_sdl_get_vram_addr(surface, xo, y)) ^= border;
            else if (is16)
                *((u16*)wii_sdl_get_vram_addr(surface, xo, y)) ^= border;
            else
                *((u32*)wii_sdl_get_vram_addr(surface, xo, y)) ^= border;
            if (h > 1) {
                if (is8)
                    *((u8*)wii_sdl_get_vram_addr(surface, xo, y + h - 1)) ^= border;
                else if (is16)
                    *((u16*)wii_sdl_get_vram_addr(surface, xo, y + h - 1)) ^=
                        border;
                else
                    *((u32*)wii_sdl_get_vram_addr(surface, xo, y + h - 1)) ^=
                        border;
            }
        }
        for (yo = y; yo < (y + h); yo++) {
            if (is8)
                *((u8*)wii_sdl_get_vram_addr(surface, x, yo)) ^= border;
            else if (is16)
                *((u16*)wii_sdl_get_vram_addr(surface, x, yo)) ^= border;
            else
                *((u32*)wii_sdl_get_vram_addr(surface, x, yo)) ^= border;
            if (w > 1) {
                if (is8)
                    *((u8*)wii_sdl_get_vram_addr(surface, x + w - 1, yo)) ^= border;
                else if (is16)
                    *((u16*)wii_sdl_get_vram_addr(surface, x + w - 1, yo)) ^=
                        border;
                else
                    *((u32*)wii_sdl_get_vram_addr(surface, x + w - 1, yo)) ^=
                        border;
            }
        }
    } else {
        for (xo = x + 1; xo < (x + w - 1); xo++) {
            if (is8)
                *((u8*)wii_sdl_get_vram_addr(surface, xo, y)) = border;
            else if (is16)
                *((u16*)wii_sdl_get_vram_addr(surface, xo, y)) = border;
            else
                *((u32*)wii_sdl_get_vram_addr(surface, xo, y)) = border;
            if (h > 1) {
                if (is8)
                    *((u8*)wii_sdl_get_vram_addr(surface, xo, y + h - 1)) = border;
                else if (is16)
                    *((u16*)wii_sdl_get_vram_addr(surface, xo, y + h - 1)) = border;
                else
                    *((u32*)wii_sdl_get_vram_addr(surface, xo, y + h - 1)) = border;
            }
        }
        for (yo = y; yo < (y + h); yo++) {
            if (is8)
                *((u8*)wii_sdl_get_vram_addr(surface, x, yo)) = border;
            else if (is16)
                *((u16*)wii_sdl_get_vram_addr(surface, x, yo)) = border;
            else
                *((u32*)wii_sdl_get_vram_addr(surface, x, yo)) = border;
            if (w > 1) {
                if (is8)
                    *((u8*)wii_sdl_get_vram_addr(surface, x + w - 1, yo)) = border;
                else if (is16)
                    *((u16*)wii_sdl_get_vram_addr(surface, x + w - 1, yo)) = border;
                else
                    *((u32*)wii_sdl_get_vram_addr(surface, x + w - 1, yo)) = border;
            }
        }
    }
}

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
                            u32 color) {
    if (x < 0) {
        w += x;
        x = 0;
    }
    if (y < 0) {
        h += y;
        y = 0;
    }
    if ((x + w) > surface->w)
        w = surface->w - x;
    if ((y + h) > surface->h)
        h = surface->h - y;
    if (w <= 0 || h <= 0)
        return;

    BOOL is8 = surface->format->BytesPerPixel == 1;
    int xo, yo;
    for (xo = x; xo < w + x; xo++) {
        for (yo = y; yo < h + y; yo++) {
            if (is8) {
                *((u8*)wii_sdl_get_vram_addr(surface, xo, yo)) = color;
            } else {
                *((u16*)wii_sdl_get_vram_addr(surface, xo, yo)) = color;
            }
        }
    }
}

/**
 * Initializes the SDL
 */
int wii_sdl_init() {
    render_mutex = SDL_CreateMutex();

    // App initialization of the SDL
    wii_sdl_handle_init();

    // Don't show the cursor
    SDL_ShowCursor(SDL_DISABLE);

#if 0
    // True type fonts
    TTF_Init();

    SDL_RWops* rw = SDL_RWFromMem((u8*)font_ttf, font_ttf_size);
    sdl_font_18 = TTF_OpenFontRW(rw, 1, 18);
    rw = SDL_RWFromMem((u8*)font_ttf, font_ttf_size);
    sdl_font_14 = TTF_OpenFontRW(rw, 1, 14);
    rw = SDL_RWFromMem((u8*)font_ttf, font_ttf_size);
    sdl_font_13 = TTF_OpenFontRW(rw, 1, 13);
    rw = SDL_RWFromMem((u8*)font_ttf, font_ttf_size);
    sdl_font_12 = TTF_OpenFontRW(rw, 1, 12);
#endif

    return 1;
}

/**
 * Renders the current back surface
 */
void wii_sdl_flip() {
    SDL_Flip(back_surface);
}

/**
 * Frees the SDL resources
 */
void wii_sdl_free_resources() {
    if (back_surface != NULL) {
        SDL_FreeSurface(back_surface);
    }
    if (blit_surface != NULL) {
        SDL_FreeSurface(blit_surface);
    }
#if 0
    if (sdl_font_18 != NULL) {
        TTF_CloseFont(sdl_font_18);
    }
    if (sdl_font_14 != NULL) {
        TTF_CloseFont(sdl_font_14);
    }
    if (sdl_font_13 != NULL) {
        TTF_CloseFont(sdl_font_13);
    }
    if (sdl_font_12 != NULL) {
        TTF_CloseFont(sdl_font_12);
    }
#endif
}
