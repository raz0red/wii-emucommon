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

#include "FreeTypeGX.h"

#include "pngu.h"

#include "wii_app.h"
#include "wii_gx.h"
#include "wii_sdl.h"

extern Mtx gx_view;
extern "C" {
void WII_SetRenderCallback( void (*cb)(void) );
void WII_SetPreRenderCallback( void (*cb)(void) );
void WII_SetRenderScreen( BOOL render );
}

static gx_imagedata* getimagedata( IMGCTX ctx );

/** Render callback state information */
typedef struct callbackstate {
    void (*rendercallback)(void);
    BOOL renderscreen;
    void (*precallback)(void);
} callbackstate;

// The callback state stack
static callbackstate cbstate_stack[16];
// The head of the callback state stack
static s8 cbstate_head = -1;

/**
 * Pushes the specified callback. It will be set as the active callback.
 *
 * @param   rendercallback The callback to activate
 * @param   renderscreen Whether we want to render the SDL screen (main surface)
 * @param   precallback The pre render callback
 */
void wii_gx_push_callback(void (*rendercallback)(void),
                          BOOL renderscreen,
                          void (*precallback)(void)) {
    cbstate_stack[++cbstate_head].rendercallback = rendercallback;
    cbstate_stack[cbstate_head].renderscreen = renderscreen;
    cbstate_stack[cbstate_head].precallback = precallback;

    WII_SetRenderCallback(rendercallback);
    WII_SetRenderScreen(renderscreen);
    WII_SetPreRenderCallback(precallback);
}

/**
 * Pops the active callback. The previous callback (if it exists) will be
 * restored.
 */
void wii_gx_pop_callback() {
    if (cbstate_head <= 0) {
        return;
    }

    cbstate_stack[cbstate_head].precallback = NULL;
    cbstate_stack[cbstate_head].rendercallback = NULL;
    cbstate_stack[cbstate_head--].renderscreen = FALSE;

    if (cbstate_head >= 0) {
        WII_SetRenderCallback(cbstate_stack[cbstate_head].rendercallback);
        WII_SetRenderScreen(cbstate_stack[cbstate_head].renderscreen);
        WII_SetPreRenderCallback(cbstate_stack[cbstate_head].precallback);
    }
}

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
                          BOOL filled) {
    u8 fmt;
    long n;
    int i;
    int x2 = x + width;
    int y2 = y - height;
    if (!filled) {
        x += 1;
        y -= 1;
    }
    guVector v[] = {
        {x, y, 0}, {x2, y, 0}, {x2, y2, 0}, {x, y2, 0}, {x, y + 1, 0}};

    if (!filled) {
        fmt = GX_LINESTRIP;
        n = 5;
    } else {
        fmt = GX_TRIANGLEFAN;
        n = 4;
    }

    GX_Begin(fmt, GX_VTXFMT0, n);
    for (i = 0; i < n; i++) {
        GX_Position3s16(v[i].x, v[i].y, v[i].z);
        GX_Color4u8(color.r, color.g, color.b, color.a);
    }
    GX_End();
}

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
                     uint16_t textStyle) {
    FT_DrawText(x, y, pixelSize, (char*)text, color, textStyle);
}

/**
 * Returns the width of the specified text
 *
 * @param   pixelSize The pixel size
 * @param   text The text
 * @return  The width of the specified text
 */
uint16_t wii_gx_gettextwidth(FT_UInt pixelSize, char* text) {
    return FT_GetWidth(pixelSize, text);
}

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
 * @param   alpha Alpha channel
 */
void wii_gx_drawimage(int xpos,
                      int ypos,
                      u16 width,
                      u16 height,
                      u8 data[],
                      f32 degrees,
                      f32 scaleX,
                      f32 scaleY,
                      u8 alpha) {
    if (data == NULL)
        return;

    GXTexObj texObj;

    GX_InitTexObj(&texObj, data, width, height, GX_TF_RGBA8, GX_CLAMP, GX_CLAMP,
                  GX_FALSE);
    // GX_InitTexObjLOD(&texObj,GX_NEAR,GX_NEAR_MIP_NEAR,0.0,10.0,0.0,GX_FALSE,GX_FALSE,GX_ANISO_1);
    GX_LoadTexObj(&texObj, GX_TEXMAP0);
    GX_InvalidateTexAll();

    GX_SetTevOp(GX_TEVSTAGE0, GX_MODULATE);
    GX_SetVtxDesc(GX_VA_TEX0, GX_DIRECT);

    Mtx m, m1, m2, mv;

    int x2 = xpos + width;
    int y2 = ypos - height;

    guMtxIdentity(m1);
    guMtxScaleApply(m1, m1, scaleX, scaleY, 1.0);
    guVector axis = (guVector){0, 0, 1};
    guMtxRotAxisDeg(m2, &axis, degrees);
    guMtxConcat(m2, m1, m);

    guMtxTransApply(m, m, 0, 0, -100);
    guMtxConcat(gx_view, m, mv);
    GX_LoadPosMtxImm(mv, GX_PNMTX0);

    GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
    GX_Position3s16(xpos, ypos, 0);
    GX_Color4u8(0xFF, 0xFF, 0xFF, alpha);
    GX_TexCoord2f32(0, 0);

    GX_Position3s16(x2, ypos, 0);
    GX_Color4u8(0xFF, 0xFF, 0xFF, alpha);
    GX_TexCoord2f32(1, 0);

    GX_Position3s16(x2, y2, 0);
    GX_Color4u8(0xFF, 0xFF, 0xFF, alpha);
    GX_TexCoord2f32(1, 1);

    GX_Position3s16(xpos, y2, 0);
    GX_Color4u8(0xFF, 0xFF, 0xFF, alpha);
    GX_TexCoord2f32(0, 1);
    GX_End();

    GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
    GX_SetVtxDesc(GX_VA_TEX0, GX_NONE);
}

/**
 * Loads and returns the data for the image at the specified path
 *
 * @param   imgpath The path to the image
 * @return  The data for the loaded image
 */
gx_imagedata* wii_gx_loadimage(char* imgpath) {
    if (imgpath) {
        IMGCTX ctx = PNGU_SelectImageFromDevice(imgpath);
        return getimagedata(ctx);
    }

    return NULL;
}

/**
 * Loads image data for the specified image buffer
 *
 * @param   buff The image buffer
 * @return  The data for the loaded image
 */
gx_imagedata* wii_gx_loadimagefrombuff(const u8* buff) {
    if (buff) {
        IMGCTX ctx = PNGU_SelectImageFromBuffer(buff);
        return getimagedata(ctx);
    }

    return NULL;
}

/**
 * Loads image data for the specified image context
 *
 * @param   ctx The image context
 * @return  The image data for the specified context
 */
static gx_imagedata* getimagedata(IMGCTX ctx) {
    if (!ctx)
        return NULL;

    gx_imagedata imgdata;
    memset(&imgdata, 0, sizeof(gx_imagedata));
    PNGUPROP imgProp;

    int res = PNGU_GetImageProperties(ctx, &imgProp);
    if (res == PNGU_OK) {
        int len = imgProp.imgWidth * imgProp.imgHeight * 4;
        if (len % 32)
            len += (32 - len % 32);
        imgdata.data = (u8*)memalign(32, len);
        if (imgdata.data) {
            res = PNGU_DecodeTo4x4RGBA8(ctx, imgProp.imgWidth,
                                        imgProp.imgHeight, imgdata.data, 255);

            if (res == PNGU_OK) {
                imgdata.width = imgProp.imgWidth;
                imgdata.height = imgProp.imgHeight;
                DCFlushRange(imgdata.data, len);
            } else {
                free(imgdata.data);
                imgdata.data = NULL;
            }
        }
    }

    PNGU_ReleaseImageContext(ctx);

    gx_imagedata* ret = NULL;
    if (imgdata.data) {
        ret = (gx_imagedata*)malloc(sizeof(gx_imagedata));
        *ret = imgdata;
    }

    return ret;
}

/**
 * Frees the specified image data information
 *
 * @param   data The image data to release
 */
void wii_gx_freeimage(gx_imagedata* imgdata) {
    if (imgdata) {
        free(imgdata->data);
        imgdata->data = NULL;
        free(imgdata);
    }
}
