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

// TODO: This is missing a good way of overriding base behavior (like, you know, method overrides...)

#include "DDAccess.h"
#include "pstypes.h"
#include "pserror.h"
#include "mono.h"
#include "3d.h"
#include "renderer.h"
#include "ddvid.h"
#include "ddio.h"
#include "application.h"
#include "bitmap.h"
#include "lightmap.h"
#include "rend_opengl.h"
#include "grdefs.h"
#include "mem.h"
#include "rtperformance.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "HardwareInternal.h"

// FIXME: Unused
// The font characteristics
static float rend_FontRed[4], rend_FontBlue[4], rend_FontGreen[4], rend_FontAlpha[4];
char Renderer_error_message[256] = "Generic renderer error";

// The following values are referenced by the game code, but only
// UseMultitexture is actually referenced and used in this code - and
// even that can probably go away and we just assume there is more than
// one texture unit (which, since I believe OpenGL 1.2 is a requirement)
bool UseHardware = true;
bool NoLightmaps = false;
bool StateLimited = false;
bool UseMultitexture = false;
bool UseWBuffer = false;

// General renderer states
ubyte Renderer_initted = 0;

// Generic GPU data
rendering_state gpu_state;
renderer_preferred_state gpu_preferred_state = {0, 1, 1.5};

int gpu_last_frame_polys_drawn = 0;
int gpu_last_frame_verts_processed = 0;
int gpu_last_uploaded = 0;

float gpu_Alpha_factor = 1.0f;

// Retrieves an error message
const char *rend_GetErrorMessage() { return Renderer_error_message; }

// Sets an error message
void rend_SetErrorMessage(const char *str) {
  ASSERT(strlen(str) < 256);
  strcpy(Renderer_error_message, str);
}

// FIXME: Unused
// Sets the argb characteristics of the font characters.  color1 is the upper left and proceeds clockwise
void rend_SetCharacterParameters(ddgr_color color1, ddgr_color color2, ddgr_color color3, ddgr_color color4) {
  rend_FontRed[0] = (float)(GR_COLOR_RED(color1) / 255.0f);
  rend_FontRed[1] = (float)(GR_COLOR_RED(color2) / 255.0f);
  rend_FontRed[2] = (float)(GR_COLOR_RED(color3) / 255.0f);
  rend_FontRed[3] = (float)(GR_COLOR_RED(color4) / 255.0f);
  rend_FontGreen[0] = (float)(GR_COLOR_GREEN(color1) / 255.0f);
  rend_FontGreen[1] = (float)(GR_COLOR_GREEN(color2) / 255.0f);
  rend_FontGreen[2] = (float)(GR_COLOR_GREEN(color3) / 255.0f);
  rend_FontGreen[3] = (float)(GR_COLOR_GREEN(color4) / 255.0f);
  rend_FontBlue[0] = (float)(GR_COLOR_BLUE(color1) / 255.0f);
  rend_FontBlue[1] = (float)(GR_COLOR_BLUE(color2) / 255.0f);
  rend_FontBlue[2] = (float)(GR_COLOR_BLUE(color3) / 255.0f);
  rend_FontBlue[3] = (float)(GR_COLOR_BLUE(color4) / 255.0f);
  rend_FontAlpha[0] = (color1 >> 24) / 255.0f;
  rend_FontAlpha[1] = (color2 >> 24) / 255.0f;
  rend_FontAlpha[2] = (color3 >> 24) / 255.0f;
  rend_FontAlpha[3] = (color4 >> 24) / 255.0f;
}

// Sets the overall alpha scale factor (all alpha values are scaled by this value)
// usefull for motion blur effect
void rend_SetAlphaFactor(float val) {
  if (val < 0.0f)
    val = 0.0f;
  if (val > 1.0f)
    val = 1.0f;
  gpu_Alpha_factor = val;
}

// Returns the current Alpha factor
float rend_GetAlphaFactor(void) { return gpu_Alpha_factor; }

// Preuploads a texture to the video card
void rend_PreUploadTextureToCard(int handle, int map_type) {}

// Frees an uploaded texture from the video card
void rend_FreePreUploadedTexture(int handle, int map_type) {}

// Returns 1 if there is mid video memory, 2 if there is low vid memory, or 0 if there is large vid memory
int rend_LowVidMem(void) { return 0; }

// Returns 1 if the renderer supports bumpmapping
int rend_SupportsBumpmapping(void) { return 0; }

// Sets a bumpmap to be rendered, or turns off bumpmapping altogether
void rend_SetBumpmapReadyState(int state, int map) {}

// returns rendering statistics for the frame
void rend_GetStatistics(tRendererStats *stats) {
  if (Renderer_initted) {
    stats->poly_count = gpu_last_frame_polys_drawn;
    stats->vert_count = gpu_last_frame_verts_processed;
    stats->texture_uploads = gpu_last_uploaded;
  } else {
    memset(stats, 0, sizeof(tRendererStats));
  }
}
