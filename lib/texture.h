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

--- HISTORICAL COMMENTS FOLLOW ---

 * $Logfile: /DescentIII/Main/lib/texture.h $
 * $Revision: 9 $
 * $Date: 5/05/98 3:02p $
 * $Author: Jason $
 *
 * Header for texture mapper library
 *
 * $Log: /DescentIII/Main/lib/texture.h $
 *
 * 9     5/05/98 3:02p Jason
 * attempting to add different screen resolutions
 *
 * 8     1/14/98 5:49p Jeff
 * tex_LFBLock stuff added (samir.)
 *
 * 7     12/22/97 7:32p Samir
 * Took out include to gr.h
 *
 * 6     12/19/97 5:22p Samir
 * Added more drawing primatives for software.
 *
 * 5     12/19/97 12:20p Jason
 * changes for better 2d/3d system integration
 *
 * 4     10/15/97 5:20p Jason
 * did a HUGE overhaul of the bitmap system
 *
 * 3     9/16/97 4:09p Jason
 * implemented software zbuffer
 *
 * 2     9/09/97 12:34p Jason
 * made software transparency work correctly with new alpha type
 *
 * 2     6/24/97 7:45p Matt
 * Removed include of manage.h
 *
 * 1     6/23/97 9:25p Samir
 * added because source safe sucks
 *
 * 40    5/22/97 11:59a Jason
 * add a ScaleBitmap function to the abstracted render library
 *
 * 39    5/19/97 5:10p Jason
 * changes for our new abstracted renderer code
 *
 * 38    5/12/97 11:41a Jason
 * made game work (default) to 16bit no mip maps mode
 * Saves us alot of memory
 *
 * 37    5/01/97 4:37p Jason
 * added alpha blending (slow)
 *
 * 36    4/30/97 3:15p Jason
 * changes to support both 8bit and 16bit rendering in software
 *
 * 35    4/11/97 4:20p Jason
 * implemented a general fog system
 *
 * 34    3/25/97 2:33p Jason
 * added Software_renderer byte
 *
 * 33    3/20/97 11:54a Jason
 * changes to support special triangle drawer
 *
 * 32    3/14/97 4:33p Jason
 * added tex_SetMipState function
 *
 * 31    3/13/97 6:19p Jason
 * implemented tex_GetRenderType function
 *
 * 30    2/26/97 6:00p Matt
 * Renamed 3d lib structs for D3 naming convention
 *
 * $NoKeywords: $
 */

#ifndef TEXTURE_H
#define TEXTURE_H

#include "3d.h"
#include "pstypes.h"
#include "renderer.h"

#define MAX_TEXTURE_SHADES 32

struct texs_texpoint {
  int screenx, screeny;
  float flscreenx, flscreeny;

  fix ucoord, vcoord;
  float flucoord, flvcoord, flzcoord;

  fix monolight;
  fix redlight, greenlight, bluelight;
};

// For the following enum table: LIN=Linear, PER=Perspective, NON=No Transparency, LIT = Shaded,
//								 COLORED=Colored lighting, TRANS=Transparency
// ALPHA=Alpha renderer

#define MAX_RENDER_TYPES 15

extern void (*Texture_functions[])(g3Point *, int);

// Our shade tables
extern uint8_t TexShadeTable8[MAX_TEXTURE_SHADES][256];
extern uint32_t TexShadeTable16[MAX_TEXTURE_SHADES][256];
extern uint8_t TexRevShadeTable8[MAX_TEXTURE_SHADES][256];
extern uint32_t TexRevShadeTable16[MAX_TEXTURE_SHADES][256];

int tex_Init();

// These are the different routines for drawing the various types of effects on a texture
void tex_PerUnlitTexturedQuad(g3Point *, int);
void tex_PerLitTexturedQuad(g3Point *, int);
void tex_UnlitTexturedQuad(g3Point *, int);
void tex_LitTexturedQuad(g3Point *, int);
void tex_ColoredTexturedQuad(g3Point *, int);
void tex_PerColoredTexturedQuad(g3Point *, int);
void tex_UnlitTransQuad(g3Point *, int);
void tex_LitTransQuad(g3Point *, int);
void tex_PerUnlitTransQuad(g3Point *, int);
void tex_PerLitTransQuad(g3Point *, int);
void tex_LinFlatShade(g3Point *, int);
void tex_PerFlatShade(g3Point *, int);
void tex_LitTexturedTriangle(g3Point *, int);
void tex_FlatPolyTriangle(g3Point *, int);
void tex_SetFlatColor(ddgr_color color);
void tex_UnlitTexturedQuad16(g3Point *p, int nv);
void tex_LitTexturedQuad16(g3Point *p, int nv);
void tex_PerUnlitTexturedQuad16(g3Point *p, int nv);
void tex_PerLitTexturedQuad16(g3Point *p, int nv);
void tex_LitTexturedTriangle16(g3Point *p, int nv);
void tex_LinFlatShade16(g3Point *p, int nv);
void tex_PerFlatShade16(g3Point *p, int nv);
void tex_AlphaTexturedQuad(g3Point *p, int nv);

// These functions get the extremes of a polygon
void tex_GetVertexOrdering(texs_texpoint *t, int nv, int *vlt, int *vlb, int *vrt, int *vrb, int *bottom_y_ind);
int tex_PrevIndex(int val, int modulus);
int tex_NextIndex(int val, int modulus);

// Builds the 1/n table
void tex_BuildRecipTable();

// Draws a solid color polygon.  "color" is a color in 5-6-5 format.
void tex_DrawFlatPolygon(g3Point **pv, int nv);

// Given a handle to a texture and nv point vertices, does a fan algorithm to draw them all
// Uses the function set by tex_SetRenderType
void tex_DrawPointList(int handle, g3Point **p, int nv);

// Tells the software renderer whether or not to use mipping
void tex_SetMipState(int);

// Sets the fog state to TRUE or FALSE
void tex_SetFogState(int on);

// Sets the a lighting state for the software renderer
void tex_SetLighting(light_state);

// Sets a color model (see renderer.h for valid models)
void tex_SetColorModel(color_model);

// Sets a texturing type (see renderer.h)
void tex_SetTextureType(texture_type);

// Sets the near and far plane of fog
void tex_SetFogBorders(float fog_near, float fog_far);

// Sets the alpha type
void tex_SetAlphaType(int8_t);

// ZBuffer variables
void tex_SetZBufferState(int state);
void tex_ClearZBuffer();
void tex_StartFrame(int x1, int y1, int x2, int y2);
void tex_EndFrame();

// Sets the software renderer to render to a particular place
void tex_SetSoftwareParameters(float, int, int, int, uint8_t *);

// Fills in projection variables
void tex_GetProjectionParameters(int *width, int *height);

// Returns the aspect ratio of the physical screen
float tex_GetAspectRatio();

//	software line renderer.
void tex_DrawLine(int x1, int y1, int x2, int y2);

//	fill rect supported.
void tex_FillRect(ddgr_color color, int x1, int y1, int x2, int y2);

//	set pixel
void tex_SetPixel(ddgr_color color, int x, int y);

//	get pixel
ddgr_color tex_GetPixel(int x, int y);

//	fillcircle
void tex_FillCircle(ddgr_color col, int x, int y, int r);

//	draw circle
void tex_DrawCircle(int x, int y, int r);

//	lfb stuff
void tex_GetLFBLock(renderer_lfb *lfb);

void tex_ReleaseLFBLock(renderer_lfb *lfb);

void tex_GetRenderState(rendering_state *rs);

#endif
