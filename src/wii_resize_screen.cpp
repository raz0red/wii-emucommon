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

#include "wii_app.h"
#include "wii_gx.h"
#include "wii_hw_buttons.h"
#include "wii_input.h"
#include "wii_resize_screen.h"
#include "wii_sdl.h"
#include "gettext.h"

extern "C" {
void WII_SetRenderCallback(void (*cb)(void));
void WII_ChangeSquare(int xscale, int yscale, int xshift, int yshift);
void WII_VideoStart();
void WII_VideoStop();
}

// TODO: Fix this so there isn't a dependency on coleco
void wii_get_screen_size(int inX, int inY, int* x, int* y);

/** SDL GX View */
extern Mtx gx_view;

/** Whether the aspect ratio is locked */
static BOOL arlocked = TRUE;
/** The current aspect ratio */
static float aratio = 0.0f;
/** The x increment */
static float xinc = 0.0f;
/** The y increment */
static float yinc = 0.0f;
/** The screen size */
static resize_info* resizeinfo = NULL;
/** The current screen size index */
static int currsize = -1;
/** The current width */
float currentX = 0;
/** The current height */
float currentY = 0;


#define DELAY_FRAMES 6
#define DELAY_STEP 1
#define DELAY_MIN 0

/**
 * Returns the index of the screen size corresponding to the specified width and
 * height
 *
 * @param   rinfo The resize info
 * @param   w The width
 * @param   h The height
 * @return  The index of the screen size corresponding to the specified width
 *          and height
 */
int wii_resize_screen_find_size(resize_info* rinfo, int w, int h) {
    for (int i = 0; i < rinfo->size_count; i++) {
        const screen_size* cur = &(rinfo->sizes[i]);
        if (cur->w == w && cur->h == h) {
            return i;
        }
    }

    return -1;
}

/**
 * Resets the aspect ratio
 *
 * @param   currentX The current X value
 * @param   currentY The current Y value
 */
static void reset_aspect_ratio(float currentX, float currentY) {
    if (arlocked) {
        aratio = currentX / currentY;
        xinc = aratio;
    } else {
        xinc = 1.0f;
    }

    yinc = 1.0f;
}

/**
 * GX render callback
 */
void wii_resize_render_callback() {
    GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
    GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT);
    GX_SetVtxDesc(GX_VA_TEX0, GX_NONE);

    Mtx m;   // model matrix.
    Mtx mv;  // modelview matrix.

    guMtxIdentity(m);
    guMtxTransApply(m, m, 0, 0, -100);
    guMtxConcat(gx_view, m, mv);
    GX_LoadPosMtxImm(mv, GX_PNMTX0);

    GXColor white = (GXColor){0xff, 0xff, 0xff, 0xff};

#if 0
    wii_gx_drawrectangle(-231, 71, 462, 142, white, FALSE);
#endif
    wii_gx_drawrectangle(-230, 80, 460, 160, (GXColor){0x0, 0x0, 0x0, 0xcc},
                         TRUE);

    int fontsize = 14;
    int spacing = 16;
    int largespacing = 28;
    int y = 60 /*68 - largespacing*/;

    u16 right = (FTGX_ALIGN_TOP | FTGX_JUSTIFY_RIGHT);
    u16 left = (FTGX_ALIGN_TOP | FTGX_JUSTIFY_LEFT);

    char buffer[512] = "";
    snprintf(buffer, sizeof(buffer), "%s :", gettextmsg("D-pad/Analog"));
    wii_gx_drawtext(0, y, fontsize, buffer, white, right);
    snprintf(buffer, sizeof(buffer), " %s", gettextmsg("Resize screen"));
    wii_gx_drawtext(0, y, fontsize, buffer, white, left);
    y -= spacing;

    snprintf(buffer, sizeof(buffer), "%s :", gettextmsg("A/2 button"));
    wii_gx_drawtext(0, y, fontsize, buffer, white, right);
    snprintf(buffer, sizeof(buffer), " %s", gettextmsg("Accept changes"));
    wii_gx_drawtext(0, y, fontsize, buffer, white, left);
    y -= spacing;

    snprintf(buffer, sizeof(buffer), "%s :", gettextmsg("B/1 button"));
    wii_gx_drawtext(0, y, fontsize, buffer, white, right);
    snprintf(buffer, sizeof(buffer), " %s", gettextmsg("Cancel changes"));
    wii_gx_drawtext(0, y, fontsize, buffer, white, left);
    y -= largespacing;

    snprintf(buffer, sizeof(buffer), "%s :", gettextmsg("Minus/LTrigger"));
    wii_gx_drawtext(0, y, fontsize, buffer, white, right);
    snprintf(buffer, sizeof(buffer), " %s", gettextmsg("Toggle A/R lock"));
    wii_gx_drawtext(0, y, fontsize, buffer, white, left);
    y -= spacing;

    snprintf(buffer, sizeof(buffer), "%s :", gettextmsg("Plus/RTrigger"));
    wii_gx_drawtext(0, y, fontsize, buffer, white, right);
    snprintf(buffer, sizeof(buffer), " %s",
             gettextmsg(resizeinfo->size_count == 1 ? "Reset to defaults"
                                                    : "Toggle screen sizes"));
    wii_gx_drawtext(0, y, fontsize, buffer, white, left);
    y -= largespacing;

    snprintf(buffer, sizeof(buffer), "%s :", gettextmsg("Screen size"));
    wii_gx_drawtext(0, y, fontsize, buffer, white, right);

    int curr_index =
        wii_resize_screen_find_size(resizeinfo, currentX, currentY);
    snprintf(
        buffer, sizeof(buffer), " %s",
        gettextmsg((curr_index == -1 ? "Custom"
                                     : resizeinfo->sizes[curr_index].name)));
    wii_gx_drawtext(0, y, fontsize, buffer, white, left);
    y -= spacing;

    snprintf(buffer, sizeof(buffer), "%s :", gettextmsg("Aspect ratio"));
    wii_gx_drawtext(0, y, fontsize, buffer, white, right);
    snprintf(
        buffer, sizeof(buffer), " %s",
        gettextmsg(arlocked ? gettextmsg("Locked") : gettextmsg("Unlocked")));
    wii_gx_drawtext(0, y, fontsize, buffer, white, left);
}

/**
 * Displays the resize user interface
 *
 * @param   rinfo Information for the resize operation
 */
void wii_resize_screen_gui(resize_info* rinfo) {

    resizeinfo = rinfo;

    currentX = rinfo->currentX;
    currentY = rinfo->currentY;
    currsize = wii_resize_screen_find_size(rinfo, currentX, currentY);

    arlocked = TRUE;

    reset_aspect_ratio(currentX, currentY);

    int x, y;
    wii_get_screen_size(currentX, currentY, &x, &y);
    WII_ChangeSquare(x, y, 0, 0);

    wii_gx_push_callback(&wii_resize_render_callback, TRUE, NULL);

    // Allows for incremental speed when scaling
    // (Scales faster the longer the directional pad is held)
    s16 delay_frames = -1;
    s16 delay_factor = -1;

    BOOL loop = TRUE;
    while (loop && !wii_hw_button) {
        int x, y;
        wii_get_screen_size(currentX, currentY, &x, &y);
        WII_ChangeSquare(x, y, 0, 0);

        // Scan the Wii and Gamecube controllers
        WPAD_ScanPads();
        PAD_ScanPads();

        // Check the state of the controllers
        u32 down = WPAD_ButtonsDown(0);
        u32 held = WPAD_ButtonsHeld(0);
        u32 gcDown = PAD_ButtonsDown(0);
        u32 gcHeld = PAD_ButtonsHeld(0);

        // Analog controls
        expansion_t exp;
        WPAD_Expansion(0, &exp);
        float expX = wii_exp_analog_val(&exp, TRUE, FALSE);
        float expY = wii_exp_analog_val(&exp, FALSE, FALSE);
        s8 gcX = PAD_StickX(0);
        s8 gcY = PAD_StickY(0);

        // Classic or Nunchuck?
        bool isClassic = (exp.type == WPAD_EXP_CLASSIC);

        if (((held &
              (WII_BUTTON_LEFT | WII_BUTTON_RIGHT | WII_BUTTON_DOWN |
               WII_BUTTON_UP |
               (isClassic ? (WII_CLASSIC_BUTTON_LEFT | WII_CLASSIC_BUTTON_UP)
                          : 0))) == 0) &&
            ((gcHeld & (GC_BUTTON_LEFT | GC_BUTTON_RIGHT | GC_BUTTON_DOWN |
                        GC_BUTTON_UP)) == 0) &&
            (!wii_analog_left(expX, gcX) && !wii_analog_right(expX, gcX) &&
             !wii_analog_up(expY, gcY) && !wii_analog_down(expY, gcY))) {
            delay_frames = -1;
            delay_factor = -1;
        } else {
            if (delay_frames < 0) {
                if (wii_digital_left(!wii_mote_menu_vertical, isClassic,
                                     held) ||
                    (gcHeld & GC_BUTTON_LEFT) || wii_analog_left(expX, gcX)) {
                    // Left
                    if (arlocked) {
                        currentX -= xinc;
                        currentY -= yinc;
                    } else {
                        currentX -= xinc;
                    }
                } else if (wii_digital_right(!wii_mote_menu_vertical, isClassic,
                                             held) ||
                           (gcHeld & GC_BUTTON_RIGHT) ||
                           wii_analog_right(expX, gcX)) {
                    // Right
                    if (arlocked) {
                        currentX += xinc;
                        currentY += yinc;
                    } else {
                        currentX += xinc;
                    }
                } else if (wii_digital_down(!wii_mote_menu_vertical, isClassic,
                                            held) ||
                           (gcHeld & GC_BUTTON_DOWN) ||
                           wii_analog_down(expY, gcY)) {
                    // Down
                    if (arlocked) {
                        currentY += yinc;
                        currentX += xinc;
                    } else {
                        currentY += yinc;
                    }
                } else if (wii_digital_up(!wii_mote_menu_vertical, isClassic,
                                          held) ||
                           (gcHeld & GC_BUTTON_UP) ||
                           wii_analog_up(expY, gcY)) {
                    // Up
                    if (arlocked) {
                        currentY -= yinc;
                        currentX -= xinc;
                    } else {
                        currentY -= yinc;
                    }
                }

                delay_frames = DELAY_FRAMES - (DELAY_STEP * ++delay_factor);

                if (delay_frames < DELAY_MIN) {
                    delay_frames = DELAY_MIN;
                }
            } else {
                delay_frames--;
            }
        }

        if (currentY < 0 || currentX < 0) {
            if (arlocked || currentX < 0) {
                currentX += xinc;
            }
            if (arlocked || currentY < 0) {
                currentY += yinc;
            }
        }

        // Confirm (A)
        if ((down & (WII_BUTTON_A | (isClassic ? WII_CLASSIC_BUTTON_A
                                               : WII_NUNCHUK_BUTTON_A))) ||
            (gcDown & GC_BUTTON_A)) {
            rinfo->currentX = currentX;
            rinfo->currentY = currentY;
            loop = FALSE;
        }
        // Cancel (B or Home)
        if ((down & (WII_BUTTON_B |
                     (isClassic ? WII_CLASSIC_BUTTON_B : WII_NUNCHUK_BUTTON_B) |
                     WII_BUTTON_HOME)) ||
            (gcDown & (GC_BUTTON_B | GC_BUTTON_HOME))) {
            loop = FALSE;
        }
        // Toggle aspect ratio (Plus or LTrigger/GC)
        if ((down & (WPAD_BUTTON_MINUS | WPAD_CLASSIC_BUTTON_MINUS |
                     WPAD_CLASSIC_BUTTON_FULL_L)) ||
            (gcDown & PAD_TRIGGER_L)) {
            arlocked = !arlocked;
            reset_aspect_ratio(currentX, currentY);
        }
        // Reset to defaults (Minus or RTrigger/GC)
        if ((down & (WPAD_BUTTON_PLUS | WPAD_CLASSIC_BUTTON_PLUS |
                     WPAD_CLASSIC_BUTTON_FULL_R)) ||
            (gcDown & PAD_TRIGGER_R)) {
            currsize++;
            if (resizeinfo->size_count <= currsize) {
                currsize = 0;
            }
            const screen_size* size = &(resizeinfo->sizes[currsize]);
            currentX = size->w;
            currentY = size->h;
            reset_aspect_ratio(currentX, currentY);
        }

        VIDEO_WaitVSync();
    }
    wii_gx_pop_callback();
}

/**
 * Draws a border around the surface that is to be scaled.
 *
 * @param   surface The surface to scale
 * @param   startY The Y offset into the surface to scale
 * @param   height The height to scale
 */
void wii_resize_screen_draw_border(SDL_Surface* surface,
                                   int startY,
                                   int height) {
    wii_sdl_draw_rectangle(surface, 0, startY, surface->w, height,
                           wii_sdl_rgb(0xff, 0xff, 0xff), FALSE);

    wii_sdl_draw_rectangle(surface, 1, 1 + startY, surface->w - 2, height - 2,
                           wii_sdl_rgb(0, 0, 0), FALSE);
}
