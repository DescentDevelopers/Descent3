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

#ifndef POSTRENDER_H
#define POSTRENDER_H

#define MAX_POSTRENDERS 3000

#define PRT_OBJECT 0
#define PRT_VISEFFECT 1
#define PRT_WALL 2

struct postrender_struct {
  uint8_t type; // See types above
  union {
    int16_t objnum;
    int16_t visnum;
    int16_t facenum;
  };

  int16_t roomnum;
  float z;
};

extern int Num_postrenders;
extern postrender_struct Postrender_list[];

void ResetPostrenderList();

// Renders all the objects/viseffects/walls we have in our postrender list
void PostRender(int);

void DrawPostrenderFace(int roomnum, int facenum, bool change_z = true);

#endif
