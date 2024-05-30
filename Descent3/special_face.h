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
#define MAX_SPECIAL_FACES 13000 // made large enough for Josh's Mercenary level 3

// What this special face is used for:
#define SFT_SPECULAR 1

struct specular_instance {
  vector bright_center;
  uint16_t bright_color;
};

#define SFF_SPEC_OBJECT 1
#define SFF_SPEC_SMOOTH 2

struct special_face {
  uint8_t type; // See types (above)
  uint8_t num;  // Number of instances
  uint8_t used;
  uint8_t flags;

  specular_instance *spec_instance;

  vector *vertnorms;

};

extern special_face SpecialFaces[];
extern int Num_of_special_faces;

// Sets all the special faces to unused
void InitSpecialFaces();

// Returns an index into the special faces array
int AllocSpecialFace(int type, int num, bool vertnorms = false, int num_vertnorms = 0);

// Given a handle, frees the special face
void FreeSpecialFace(int handle);

#endif
