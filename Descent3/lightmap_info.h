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

 * $Logfile: /DescentIII/Main/lightmap_info.h $
 * $Revision: 25 $
 * $Date: 3/20/00 12:12p $
 * $Author: Matt $
 *
 *
 *
 * $Log: /DescentIII/Main/lightmap_info.h $
 *
 * 25    3/20/00 12:12p Matt
 * Merge of Duane's post-1.3 changes.
 * Lower MAX_LIGHT_INFOS for Mac (Mac only)
 *
 * 24    10/21/99 6:42p Matt
 * Mac merge
 *
 */

#ifndef LIGHTMAP_INFO_H
#define LIGHTMAP_INFO_H
// Lightmap info header

#include "pstypes.h"
#include "pserror.h"
#include "vecmat.h"

#define BAD_LMI_INDEX 65535

// What this lightmap is used for:
#define LMI_ROOM 0
#define LMI_ROOM_OBJECT 1
#define LMI_TERRAIN 2
#define LMI_TERRAIN_OBJECT 3
#define LMI_DYNAMIC 4
#define LMI_EXTERNAL_ROOM 5
#define LMI_EXTERNAL_ROOM_OBJECT 6

struct lightmap_info {
  uint8_t xspacing, yspacing;
  uint16_t lm_handle;
  vector upper_left, normal;
  uint8_t width, height, x1, y1;
  uint8_t used;

  uint16_t dynamic;
  int16_t spec_map;

  uint8_t type; // see LMI_types above
};

extern lightmap_info *LightmapInfo;
extern int Num_of_lightmap_info;
extern int Num_lightmap_infos_read;

#define MAX_LIGHTMAP_INFOS (65534)

// Sets all the lightmaps to unused
void InitLightmapInfo(int nummaps = 0);

// Allocs a lightmap of w x h size
// Returns lightmap handle if successful, -1 if otherwise
int AllocLightmapInfo(int w, int h, int type, bool alloc_lightmap = true);

// Given a handle, frees the lightmap memory and flags this lightmap as unused
void FreeLightmapInfo(int handle);

// Gets the width of this lightmap_info handle
int lmi_w(int handle);

// Gets the height of this lightmap_info handle
int lmi_h(int handle);

// Softens the edges of lightmaps so there are fewer artifaces
void ShadeLightmapInfoEdges(int type);
void BlurLightmapInfos(int type);

#endif
