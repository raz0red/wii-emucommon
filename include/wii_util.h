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

#ifndef WII_UTIL_H
#define WII_UTIL_H

#include <gccore.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DIR_SEP_CHAR '/'
#define DIR_SEP_STR "/"

typedef struct RGBA {
    unsigned char R;
    unsigned char G;
    unsigned char B;
    unsigned char A;
} RGBA;

extern void Util_chomp(char* s);
extern void Util_trim(char* s);
extern int Util_sscandec(const char* s);
extern u32 Util_sscandec_u32(const char* s);
extern char* Util_strlcpy(char* dest, const char* src, size_t size);
extern int Util_fileexists(char* filename);
extern void Util_splitpath(const char* path, char* dir_part, char* file_part);
extern void Util_getextension(char* filename, char* ext);
extern int Util_hextodec(const char* hex);
extern void Util_hextorgba(const char* hex, RGBA* rgba);
extern void Util_dectohex(int dec, char* hex, int fill);
extern void Util_rgbatohex(RGBA* rgba, char* hex);
extern void Util_ansitoUTF8(unsigned char* in, unsigned char* out);

/**
 * Converts the RGBA to an integer value
 *
 * @param   rgba The RGBA
 * @param   includeAlpha Whether to include alpha in the value
 * @return  The RGBA as an integer value
 */
u32 Util_rgbatovalue(RGBA* rgba, BOOL includeAlpha);

#ifdef __cplusplus
}
#endif

#endif
