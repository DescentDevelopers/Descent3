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

#ifndef SPECIAL_FACE_H
#define SPECIAL_FACE_H

#include "pstypes.h"
#include "pserror.h"
#include "vecmat.h"

#define BAD_SPECIAL_FACE_INDEX -1
#ifdef MACINTOSH
#define MAX_SPECIAL_FACES 5000
#else
#define MAX_SPECIAL_FACES 13000 // made large enough for Josh's Mercenary level 3
#endif

// What this special face is used for:
#define SFT_SPECULAR 1

typedef struct {
  vector bright_center;
  ushort bright_color;
} specular_instance;

#define SFF_SPEC_OBJECT 1
#define SFF_SPEC_SMOOTH 2

typedef struct {
  ubyte type; // See types (above)
  ubyte num;  // Number of instances
  ubyte used;
  ubyte flags;

  specular_instance *spec_instance;

  vector *vertnorms;

} special_face;

extern special_face SpecialFaces[];
extern int Num_of_special_faces;

// Sets all the special faces to unused
void InitSpecialFaces();

// Returns an index into the special faces array
int AllocSpecialFace(int type, int num, bool vertnorms = false, int num_vertnorms = 0);

// Given a handle, frees the special face
void FreeSpecialFace(int handle);

#endif
