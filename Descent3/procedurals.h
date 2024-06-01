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

#ifndef PROCEDURALS_H
#define PROCEDURALS_H

#include "pstypes.h"
#include "pserror.h"
#include "fix.h"

#define MAX_PROC_ELEMENTS 8000

// Fire procedurals
#define PROC_NONE 0
#define PROC_LINE_LIGHTNING 1
#define PROC_SPHERE_LIGHTNING 2
#define PROC_STRAIGHT 3
#define PROC_RISING_EMBER 4
#define PROC_RANDOM_EMBERS 5
#define PROC_SPINNERS 6
#define PROC_ROAMERS 7
#define PROC_FOUNTAIN 8
#define PROC_CONE 9
#define PROC_FALL_RIGHT 10
#define PROC_FALL_LEFT 11

#define PROC_WATER_NONE 0
#define PROC_WATER_HEIGHT_BLOB 1
#define PROC_WATER_SINE_BLOB 2
#define PROC_WATER_RAINDROPS 3
#define PROC_WATER_BLOBDROPS 4

struct dynamic_proc_element {
  uint8_t type;
  fix dx, dy;
  uint8_t frames_left;
  uint8_t speed;
  uint8_t color;

  uint8_t size;

  fix x1, y1;

  int16_t prev, next;
};

extern dynamic_proc_element DynamicProcElements[];
extern const char *ProcNames[], *WaterProcNames[];

extern uint16_t DefaultProcPalette[];

// Goes through our array and clears the slots out
void InitProcedurals();

// Fades and entire texture one step closer to black
void FadeProcTexture(int tex_handle);

// Draws lightning into a bitmap
void AddProcLightning(int x1, int y1, int x2, int y2);

// link the procedural into the list for its texture
void ProcElementLink(int index, int texnum);

// Unlinks a proc_element from a texture
void ProcElementUnlink(int index, int texnum);

// Clears all the procedurals elements attached to this texture
void ClearAllProceduralsFromTexture(int texnum);

// Does a procedural for this texture
void EvaluateProcedural(int texnum);

// Returns the next free procelement
int ProcElementAllocate();

// Frees up a procelement for use
int ProcElementFree(int index);

// Given an array of r,g,b values, generates a 16bit palette table for those colors
void GeneratePaletteForProcedural(uint8_t *r, uint8_t *g, uint8_t *b, uint16_t *pal);

#endif
