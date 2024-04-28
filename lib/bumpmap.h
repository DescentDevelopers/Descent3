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

#ifndef BUMPMAP_H
#define BUMPMAP_H

#include "pstypes.h"

#define MAX_BUMPMAPS 500
#define BAD_BUMP_INDEX 65535

#define BUMPF_USED 1
#define BUMPF_CHANGED 2

typedef struct {
  ushort *data;     // 8bit data
  short cache_slot; // for the renderers use
  ubyte width, height;
  ubyte flags, pad;
} bms_bumpmap;

extern bms_bumpmap GameBumpmaps[MAX_BUMPMAPS];

// Sets all the bumpmaps to unused
void bump_InitBumpmaps();

void bump_ShutdownBumpmaps(void);

// Allocs a bumpmap of BUMP_WIDTH * BUMP_HEIGHT size
// Returns bumpmap handle if successful, -1 if otherwise
int bump_AllocBumpmap(int w, int h);

// Given a handle, frees the bumpmap memory and flags this bumpmap as unused
void bump_FreeBumpmap(int handle);

// returns a bumpmaps data else NULL if something is wrong
ushort *bump_data(int handle);

// returns width or height of the passed in bumpmap
ubyte bump_w(int handle);
ubyte bump_h(int handle);

#endif
