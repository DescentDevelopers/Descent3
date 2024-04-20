/*
* Descent 3 
* Copyright (C) 2024 Parallax Software
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#define MVE_GFX_VESA_640_400 0x100
#define MVE_GFX_VESA_640_200 0x8100
#define MVE_GFX_VESA_640_480 0x101
#define MVE_GFX_VESA_640_240 0x8101
#define MVE_GFX_VESA_800_600 0x103
#define MVE_GFX_VESA_800_300 0x8103
#define MVE_GFX_VESA_1024_768 0x105
#define MVE_GFX_VESA_1024_384 0x8105
#define MVE_GFX_VESA_1280_1024 0x107
#define MVE_GFX_VESA_1280_512 0x8107

#define MVE_GFX_VESA_320_200_HI 0x10d /* 1:5:5:5 Hi Color modes */
#define MVE_GFX_VESA_640_480_HI 0x110
#define MVE_GFX_VESA_800_600_HI 0x113
#define MVE_GFX_VESA_1024_768_HI 0x116
#define MVE_GFX_VESA_1280_1024_HI 0x119

/* Some hacked vesa modes */
#define MVE_GFX_VESA_640_350 0x0F00 /* 640x350 */
#define MVE_GFX_VESA_640_175 0x8F00 /* 640x175 */
#define MVE_GFX_VESA_640_160 0x0F01 /* 640x160 */

#define MVE_GFX_VESA_CURRENT 0 /* Use current VESA mode */

#define MVE_GFX_AUTO (-1)         /* Choose mode based on movie's preference */
#define MVE_GFX_VGA 0x13          /* 320x200 */
#define MVE_GFX_VGA_CURRENT (-2)  /* 320x200 (mode already set) */
#define MVE_GFX_VGA_MEDRES (-3)   /* 248x264 out of 320x350 */
#define MVE_GFX_VGA_HIRES (-4)    /* 224x288 out of 360x480 */
#define MVE_GFX_VGA_LORES (-5)    /* 288x224 out of 320x240 */
#define MVE_GFX_VESA_320_480 (-6) /* 320x480 */
