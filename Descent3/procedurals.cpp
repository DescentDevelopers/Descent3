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

 * $Logfile: /DescentIII/main/procedurals.cpp $
 * $Revision: 34 $
 * $Date: 7/28/99 3:46p $
 * $Author: Kevin $
 *
 * Procedurals
 *
 * $Log: /DescentIII/main/procedurals.cpp $
 *
 * 34    7/28/99 3:46p Kevin
 * Mac!
 *
 * 33    5/10/99 10:23p Ardussi
 * changes to compile on Mac
 *
 * 32    4/22/99 8:29p Kevin
 * made psrand.h come after stdlib.h
 *
 * 31    4/21/99 11:05a Kevin
 * new ps_rand and ps_srand to replace rand & srand
 *
 * $NoKeywords: $
 */
#include "procedurals.h"
#include "bitmap.h"
#include "gr.h"
#include "gametexture.h"
#include "vclip.h"
#include "game.h"
#include "texture.h"
#include "mem.h"
#include "ddio.h"
#include "config.h"
#include <stdlib.h>
#include <math.h>
#include <memory.h>
#include "psrand.h"

#include <algorithm>

#define BRIGHT_COLOR 254
#define PROC_SIZE 128
#define TABSIZE 256
#define TABMASK (TABSIZE - 1)
#define PERM(x) perm[(x) & TABMASK]
#define INDEX(ix, iy) PERM((ix) + PERM((iy)))
#define RANDNBR ((prand()) / (float)D3_RAND_MAX)
#define LERP(t, x0, x1) ((x0) + (t) * ((x1) - (x0)))
#define SMOOTHSTEP(x) ((x))
uint8_t perm[TABSIZE];
static float Noise_table[TABSIZE * 3];
dynamic_proc_element DynamicProcElements[MAX_PROC_ELEMENTS];
ushort DefaultProcPalette[256];
int Num_proc_elements = 0;
static short Proc_free_list[MAX_PROC_ELEMENTS];
ushort ProcFadeTable[32768];
#define NUM_WATER_SHADES 64
ushort WaterProcTableHi[NUM_WATER_SHADES][256];
ubyte WaterProcTableLo[NUM_WATER_SHADES][256];
const char *ProcNames[] = {"None",     "Line Lightning", "Sphere lightning", "Straight", "Rising Embers", "Random Embers",
                     "Spinners", "Roamers",        "Fountain",         "Cone",     "Fall Right",    "Fall Left",
                     "END"};
const char *WaterProcNames[] = {"None", "Height blob", "Sine Blob", "Random Raindrops", "Random Blobdrops", "END"};
static ubyte *ProcDestData;
int pholdrand = 1;
static inline int prand() { return (((pholdrand = pholdrand * 214013L + 2531011L) >> 16) & 0x7fff); }
// Given an array of r,g,b values, generates a 16bit palette table for those colors
void GeneratePaletteForProcedural(ubyte *r, ubyte *g, ubyte *b, ushort *pal) {
  int i;
  float fr, fg, fb;
  for (i = 0; i < 256; i++) {
    int ir = r[i];
    int ig = g[i];
    int ib = b[i];
    fr = (float)ir / 255.0;
    fg = (float)ig / 255.0;
    fb = (float)ib / 255.0;
    ir = (fr * 31.0);
    ig = (fg * 31.0);
    ib = (fb * 31.0);
    pal[i] = OPAQUE_FLAG | (ir << 10) | (ig << 5) | (ib);
  }
}
// Inits the noise table for procedurals
void InitNoise() {
  float *table = Noise_table;
  float z, r, theta;
  int i;
  for (i = 0; i < TABSIZE; i++) {
    perm[i] = prand() % TABMASK;
    z = 1.0f - 2.0f * RANDNBR;

    /* r is radius of x,y circle */
    r = sqrtf(1 - z * z);
    /* theta is angle in (x,y) */
    theta = 2.0f * 3.14f * RANDNBR;
    *table++ = r * cosf(theta);
    *table++ = r * sinf(theta);
    *table++ = z;
  }
}
// Gets a lattice point for our noise
float GradLattice(int ix, int iy, float fx, float fy) {
  float *g = &Noise_table[INDEX(ix, iy) * 2];
  return g[0] * fx + g[1] * fy;
}
float GradNoise(float x, float y) {
  int ix, iy;
  float fx0, fx1, fy0, fy1;
  float wx, wy;
  float vx0, vx1, vy0, vy1;
  ix = (int)floor(x);
  fx0 = x - ix;
  fx1 = fx0 - 1;
  wx = SMOOTHSTEP(fx0);
  iy = (int)floor(y);
  fy0 = y - iy;
  fy1 = fy0 - 1;
  wy = SMOOTHSTEP(fy0);
  vx0 = GradLattice(ix, iy, fx0, fy0);
  vx1 = GradLattice(ix + 1, iy, fx1, fy0);
  vy0 = LERP(wx, vx0, vx1);
  vx0 = GradLattice(ix, iy + 1, fx0, fy1);
  vx1 = GradLattice(ix + 1, iy + 1, fx1, fy1);
  vy1 = LERP(wx, vx0, vx1);

  return LERP(wy, vy0, vy1);
}
extern ubyte EasterEgg;
int Easter_egg_handle = -1;
// Goes through our array and clears the slots out
void InitProcedurals() {
  int t;
  int i;

  // Load easter egg bitmap
  Easter_egg_handle = bm_AllocLoadFileBitmap("FreakyEye.ogf", 0);
  if (Easter_egg_handle == -1) {
    mprintf((0, "Failed to load easter egg!\n"));
  }
  for (i = 0; i < MAX_PROC_ELEMENTS; i++) {
    DynamicProcElements[i].type = PROC_NONE;
    Proc_free_list[i] = i;
  }

  Num_proc_elements = 0;

  // Init our fade table
  for (i = 0; i < 32768; i++) {
    int r = (i >> 10) & 0x1f;
    int g = (i >> 5) & 0x1f;
    int b = (i) & 0x1f;
    r = std::max(0, r - 1);
    g = std::max(0, g - 1);
    b = std::max(0, b - 1);
    ProcFadeTable[i] = OPAQUE_FLAG | (r << 10) | (g << 5) | b;
  }
  // Init our palette
  for (i = 0; i < 128; i++) {
    float fr = (float)i / 127.0;
    int ib = fr * 31.0;
    int ig = fr * 16.0;
    DefaultProcPalette[i] = OPAQUE_FLAG | (ig << 5) | (ib);
  }
  // Do lower part of table
  for (i = 0; i < (NUM_WATER_SHADES); i++) {
    float norm = ((float)i / (float)(NUM_WATER_SHADES - 1));
    float lo_norm = std::min((norm / .5) * 1.0, 1.0);
    float hi_norm = std::max(((norm - .5) / .5) * 1.0, 0.0);

    for (int rcount = 0; rcount < 32; rcount++) {
      for (int gcount = 0; gcount < 4; gcount++) {
        int r = rcount;

        int index = (rcount * 4) + gcount;

        float fr = r;

        r = std::min(fr * lo_norm + (31.0 * hi_norm), 31.0);

        WaterProcTableHi[i][index] = OPAQUE_FLAG | (r << 10);
      }
    }
    for (int bcount = 0; bcount < 32; bcount++) {
      for (int gcount = 0; gcount < 8; gcount++) {
        int b = bcount;
        int index = gcount * 32 + bcount;

        float fb = b;
        b = std::min(fb * lo_norm + (31.0 * hi_norm), 31.0);

        WaterProcTableLo[i][index] = b;
      }
    }

    int gcount;
    for (gcount = 0; gcount < 8; gcount++) {
      float fg = gcount;
      int g = std::min((fg * lo_norm) + (7.0 * hi_norm), 7.0);
      for (t = 0; t < 32; t++) {
        int index = gcount * 32 + t;
        WaterProcTableLo[i][gcount * 32 + t] |= (g << 5);
      }
    }
    for (gcount = 0; gcount < 4; gcount++) {
      float fg = gcount * 8;
      int g = std::min((fg * lo_norm) + (24.0 * hi_norm), 24.0);

      for (t = 0; t < 32; t++) {
        int index = t * 4 + gcount;

        WaterProcTableHi[i][t * 4 + gcount] |= (g << 5);
      }
    }
  }
  /*for (i=0;i<NUM_WATER_SHADES;i++)
  {
          int r=(WaterProcTableHi[i][127]>>10) & 0x1f;
          int g=((WaterProcTableHi[i][127]>>5) & 0x1f);
          g+=((WaterProcTableLo[i][255]>>5) & 0x1f);
          int b=(WaterProcTableLo[i][255]) & 0x1f;
          mprintf ((0,"index=%d r=%d g=%d b=%d\n",i,r,g,b));
  }*/
  // Init our default palette
  for (i = 0; i < 128; i++) {
    float norm = (float)i / 127.0;
    int ir = (norm * 31.0);
    int ig = 16 + (norm * 15.0);
    DefaultProcPalette[i + 128] = OPAQUE_FLAG | (ir << 10) | (ig << 5) | (0x1f);
  }
  InitNoise();
}
// Returns the next free procelement
int ProcElementAllocate() {
  if (Num_proc_elements == MAX_PROC_ELEMENTS) {
    // mprintf ((0,"Couldn't allocate proc element!\n"));
    return -1;
  }

  int n = Proc_free_list[Num_proc_elements++];
  ASSERT(DynamicProcElements[n].type == PROC_NONE); // Get Jason

  DynamicProcElements[n].next = -1;
  DynamicProcElements[n].prev = -1;

  return n;
}
// Frees up a procelement for use
int ProcElementFree(int index) {
  ASSERT(index >= 0 && index <= MAX_PROC_ELEMENTS);
  Proc_free_list[--Num_proc_elements] = index;
  DynamicProcElements[index].type = PROC_NONE;
  return 1;
}
// Draws a line into the passed in bitmap
void DrawProceduralLine(int x1, int y1, int x2, int y2, ubyte color) {
  int DY, DX;
  int i, x, y, yinc = 1, xinc = 1, error_term;

  ubyte *surface_ptr = ProcDestData;

  int xmask = PROC_SIZE - 1;
  int ymask = PROC_SIZE - 1;

  // Check to see if our x coords are reversed
  if (x1 > x2) {
    SWAP(x1, x2);
    SWAP(y1, y2);
  }
  DX = x2 - x1;
  DY = y2 - y1;
  if (DX < 0) {
    xinc = -1;
    DX = -DX;
  }
  if (DY < 0) {
    yinc = -1;
    DY = -DY;
  }
  if (DX >= DY) // X is greater than y
  {
    error_term = 0;
    x = x1 & xmask;
    y = y1 & ymask;
    surface_ptr += y * PROC_SIZE;
    for (i = 0; i < DX; i++) {

      *(surface_ptr + x) = color;
      x += xinc;
      x &= xmask;
      error_term += DY;
      if (error_term >= DX) {
        y += yinc;
        y &= ymask;
        surface_ptr = ProcDestData + (y * PROC_SIZE);
        error_term -= DX;
      }
    }
  } else {
    error_term = 0;
    x = x1 & xmask;
    y = y1 & ymask;
    surface_ptr += y * PROC_SIZE;

    for (i = 0; i < DY; i++) {
      *(surface_ptr + x) = color;
      y += yinc;
      y &= ymask;
      error_term += DX;
      surface_ptr = ProcDestData + (y * PROC_SIZE);
      if (error_term >= DY) {
        x += xinc;
        x &= xmask;
        error_term -= DY;
      }
    }
  }
}
// Fades an entire bitmap one step closer to black
void FadeProcTexture(int tex_handle) {
  int total = PROC_SIZE * PROC_SIZE;
  ubyte *src_data = ProcDestData;
  int fadeval;
  fadeval = 255 - GameTextures[tex_handle].procedural->heat;
  fadeval >>= 3;
  fadeval++;

  for (int i = 0; i < total; i++, src_data++) {
    int pix = *src_data;
    if (pix) {
      if (pix - fadeval <= 0)
        *src_data = 0;
      else
        *src_data = pix - fadeval;
    }
  }
}
// Heats an entire bitmap
void HeatProcTexture(int tex_handle) {
  int total = PROC_SIZE * PROC_SIZE;
  ubyte *src_data = ProcDestData;
  ubyte val;
  ubyte heat = GameTextures[tex_handle].procedural->heat;
  val = 255 - heat;

  for (int i = 0; i < total; i++, src_data++) {
    int pix = *src_data;
    if (pix < heat && pix >= val)
      (*src_data)++;
  }
}
// Fades and entire bitmap one step closer to black
void BlendProcTexture(int tex_handle) {
  int total = PROC_SIZE * PROC_SIZE;
  ubyte *src_data = (ubyte *)GameTextures[tex_handle].procedural->proc1;
  ubyte *dest_data = (ubyte *)GameTextures[tex_handle].procedural->proc2;
  ubyte *start_data = src_data;
  for (int i = 0; i < PROC_SIZE; i++) {
    ubyte *downrow;
    ubyte *start_row = src_data;
    // Get row underneath
    if (i != PROC_SIZE - 1)
      downrow = src_data + PROC_SIZE;
    else
      downrow = start_data;
    for (int t = 0; t < PROC_SIZE; t++, src_data++, dest_data++, downrow++) {
      // Get center
      total = *src_data;
      // Get right
      if (t != PROC_SIZE - 1)
        total += *(src_data + 1);
      else
        total += *(start_row);
      // Get left
      if (t)
        total += *(src_data - 1);
      else
        total += *(start_row + PROC_SIZE - 1);

      // Get below
      total += *(downrow);
      total >>= 2;
      *dest_data = total;
    }
  }
}
// Draws lightning into a bitmap
void AddProcLightning(int x1, int y1, int x2, int y2, ubyte color, static_proc_element *proc) {
  float dx = x2 - x1;
  float dy = y2 - y1;

  float mag = sqrt((dx * dx) + (dy * dy));
  if (mag < 1)
    return;
  int num_segments = mag / 8;
  dx /= mag;
  dy /= mag;
  float cur_x = x1;
  float cur_y = y1;
  float rot_x = dy;
  float rot_y = dx;
  float from_x = cur_x;
  float from_y = cur_y;
  for (int i = 0; i < num_segments; i++, cur_x += (dx * 8), cur_y += (dy * 8)) {
    float to_x = cur_x + (dx * 8);
    float to_y = cur_y + (dy * 8);
    if (i != num_segments - 1) {
      to_x += (rot_x) * ((1 + proc->speed) * (((prand() % 200) - 100.0) / 18.0));
      to_y += (rot_y) * ((1 + proc->speed) * (((prand() % 200) - 100.0) / 18.0));
    }

    DrawProceduralLine(from_x, from_y, to_x, to_y, color);

    from_x = to_x;
    from_y = to_y;
  }
}
// link the procedural into the list for its texture
void ProcElementLink(int index, int texnum) {
  dynamic_proc_element *proc = &DynamicProcElements[index];
  proc->next = GameTextures[texnum].procedural->dynamic_proc_elements;
  GameTextures[texnum].procedural->dynamic_proc_elements = index;
  ASSERT(proc->next != index);

  proc->prev = -1;
  if (proc->next != -1)
    DynamicProcElements[proc->next].prev = index;
}
// Unlinks a proc_element from a texture
void ProcElementUnlink(int index, int texnum) {
  dynamic_proc_element *proc = &DynamicProcElements[index];

  ASSERT(index != -1);
  if (proc->prev == -1)
    GameTextures[texnum].procedural->dynamic_proc_elements = proc->next;
  else
    DynamicProcElements[proc->prev].next = proc->next;
  if (proc->next != -1)
    DynamicProcElements[proc->next].prev = proc->prev;
  ProcElementFree(index);
}
// Clears all the procedurals elements attached to this texture
void ClearAllProceduralsFromTexture(int texnum) {
  int proc_num = GameTextures[texnum].procedural->dynamic_proc_elements;

  while (proc_num != -1) {
    int temp = DynamicProcElements[proc_num].next;
    ProcElementUnlink(proc_num, texnum);
    proc_num = temp;
  }
  GameTextures[texnum].procedural->num_static_elements = 0;
}
// Adds one point to a bitmap
void AddProcPoint(int x, int y, ubyte color) {
  x &= (PROC_SIZE - 1);
  y &= (PROC_SIZE - 1);

  int index = y * PROC_SIZE + x;
  ProcDestData[index] = color;
}
// Adds one point to a bitmap
static inline void PlaceProcPoint(int x, int y, ubyte color) {
  x &= (PROC_SIZE - 1);
  y &= (PROC_SIZE - 1);
  ProcDestData[y * PROC_SIZE + x] = color;
}
// Adds a sphere lightning bolt
void AddProcSphereLightning(int handle, static_proc_element *proc) {
  int procnum = proc - GameTextures[handle].procedural->static_proc_elements;

  if (Detail_settings.Procedurals_enabled) {
    if (proc->frequency != 0 && ((FrameCount) % proc->frequency) != 0)
      return;
  }
  float norm = ((float)proc->size / 255.0);
  int len = norm * (float)PROC_SIZE;
  len /= 2;
  int dir = prand() * 2;
  int dest_x = proc->x1 + (FixCos(dir) * (float)len);
  int dest_y = proc->y1 + (FixSin(dir) * (float)len);
  AddProcLightning(proc->x1, proc->y1, dest_x, dest_y, BRIGHT_COLOR, proc);
}
void AddProcRisingEmber(int handle, static_proc_element *proc) {
  if (proc->frequency == 0 || (FrameCount % proc->frequency) == 0) {
    // Add a new fizzle
    int num = prand() % 7;
    for (int i = 0; i < num; i++) {
      int index = ProcElementAllocate();
      if (index > -1) {
        ProcElementLink(index, handle);
        DynamicProcElements[index].type = proc->type;

        DynamicProcElements[index].x1 = IntToFix((int)proc->x1);
        DynamicProcElements[index].y1 = IntToFix((int)proc->y1);
        DynamicProcElements[index].color = BRIGHT_COLOR;
        DynamicProcElements[index].speed = proc->speed;
        DynamicProcElements[index].frames_left = (prand() % 10) + 15;
      }
    }
  }
}
void DoDynamicRisingEmber(int handle, dynamic_proc_element *proc) {
  PlaceProcPoint(FixToInt(proc->x1), FixToInt(proc->y1), proc->color);
  proc->frames_left--;
  if (proc->frames_left < 1)
    ProcElementUnlink(proc - DynamicProcElements, handle);
  else {
    proc->color--;
    if (proc->color == 0)
      ProcElementUnlink(proc - DynamicProcElements, handle);
    else {
      int speed_adjust = 1 + ((proc->speed / 255.0) * 2.0);
      proc->dx = FloatToFix(((prand() % 3) - 1) * speed_adjust);
      proc->dy = FloatToFix(((prand() % 3) - 2) * speed_adjust);
      proc->x1 += proc->dx;
      proc->y1 += proc->dy;
    }
  }
}
void AddProcSpinners(int handle, static_proc_element *proc) {
  if (proc->frequency == 0 || (FrameCount % proc->frequency) == 0) {

    int proc_num = (proc - GameTextures[handle].procedural->static_proc_elements) * 60;
    int speed_adjust = 1 + ((proc->speed / 255.0) * 5.0);
    int cur_angle = (((FrameCount + proc_num) * speed_adjust) % 64) * 1024;

    int index = ProcElementAllocate();
    if (index > -1) {
      ProcElementLink(index, handle);
      DynamicProcElements[index].type = proc->type;

      DynamicProcElements[index].dx = FloatToFix(FixCos(cur_angle));
      DynamicProcElements[index].dy = FloatToFix(FixSin(cur_angle));
      DynamicProcElements[index].x1 = FloatToFix(proc->x1 + (DynamicProcElements[index].dx * FloatToFix(proc->size)));
      DynamicProcElements[index].y1 = FloatToFix(proc->y1 + (DynamicProcElements[index].dy * FloatToFix(proc->size)));
      DynamicProcElements[index].color = BRIGHT_COLOR;
      DynamicProcElements[index].speed = proc->speed;
      DynamicProcElements[index].frames_left = (prand() % 10) + 15;
    }
  }
}
void DoDynamicSpinners(int handle, dynamic_proc_element *proc) {
  PlaceProcPoint(FixToInt(proc->x1), FixToInt(proc->y1), proc->color);
  proc->frames_left--;
  if (proc->frames_left < 1)
    ProcElementUnlink(proc - DynamicProcElements, handle);
  else {
    proc->color--;
    if (proc->color == 0)
      ProcElementUnlink(proc - DynamicProcElements, handle);
    else {
      proc->x1 += proc->dx;
      proc->y1 += proc->dy;
    }
  }
}
void AddProcRandomEmber(int handle, static_proc_element *proc) {
  if (proc->frequency == 0 || (FrameCount % proc->frequency) == 0) {
    // Add a new fizzle
    int num = (prand() % 4) + 1;
    for (int i = 0; i < num; i++) {
      int index = ProcElementAllocate();
      if (index > -1) {
        ProcElementLink(index, handle);
        DynamicProcElements[index].type = proc->type;

        DynamicProcElements[index].x1 = IntToFix(proc->x1);
        DynamicProcElements[index].y1 = IntToFix(proc->y1);
        DynamicProcElements[index].color = BRIGHT_COLOR;
        DynamicProcElements[index].speed = proc->speed;
        DynamicProcElements[index].frames_left = (prand() % 10) + 15;
      }
    }
  }
}
void DoDynamicRandomEmber(int handle, dynamic_proc_element *proc) {
  PlaceProcPoint(FixToInt(proc->x1), FixToInt(proc->y1), proc->color);
  proc->frames_left--;
  if (proc->frames_left < 1)
    ProcElementUnlink(proc - DynamicProcElements, handle);
  else {
    proc->color--;
    if (proc->color == 0)
      ProcElementUnlink(proc - DynamicProcElements, handle);
    else {
      int speed_adjust = 1 + ((proc->speed / 255.0) * 2.0);
      proc->dx = FloatToFix(((prand() % 3) - 1) * speed_adjust);
      proc->dy = FloatToFix(((prand() % 3) - 1) * speed_adjust);
      proc->x1 += proc->dx;
      proc->y1 += proc->dy;
    }
  }
}
void AddProcRoamers(int handle, static_proc_element *proc) {
  proc->x1 += (ps_rand() % 5) - 2;
  proc->y1 += (ps_rand() % 5) - 2;
  if (proc->frequency == 0 || (FrameCount % proc->frequency) == 0) {
    // Add a new fizzle
    int num = (prand() % 4) + 1;
    for (int i = 0; i < num; i++) {
      int index = ProcElementAllocate();
      if (index > -1) {
        ProcElementLink(index, handle);
        DynamicProcElements[index].type = proc->type;

        DynamicProcElements[index].x1 = IntToFix(proc->x1);
        DynamicProcElements[index].y1 = IntToFix(proc->y1);
        DynamicProcElements[index].color = BRIGHT_COLOR;
        DynamicProcElements[index].speed = proc->speed;
        DynamicProcElements[index].frames_left = (prand() % 10) + 15;
      }
    }
  }
}
void DoDynamicRoamers(int handle, dynamic_proc_element *proc) {
  PlaceProcPoint(FixToInt(proc->x1), FixToInt(proc->y1), proc->color);
  proc->frames_left--;
  if (proc->frames_left < 1)
    ProcElementUnlink(proc - DynamicProcElements, handle);
  else {
    proc->color--;
    if (proc->color == 0)
      ProcElementUnlink(proc - DynamicProcElements, handle);
    else {
      int speed_adjust = 1 + ((proc->speed / 255.0) * 2.0);
      proc->dx = FloatToFix(((prand() % 3) - 1) * speed_adjust);
      proc->dy = FloatToFix(((prand() % 3) - 1) * speed_adjust);
      proc->x1 += proc->dx;
      proc->y1 += proc->dy;
    }
  }
}
void AddProcFountain(int handle, static_proc_element *proc) {
  if (proc->frequency == 0 || (FrameCount % proc->frequency) == 0) {
    // Add a new fizzle
    int num = (prand() % 4) + 1;
    for (int i = 0; i < num; i++) {
      int index = ProcElementAllocate();
      if (index > -1) {
        ProcElementLink(index, handle);
        DynamicProcElements[index].type = proc->type;

        DynamicProcElements[index].x1 = IntToFix(proc->x1);
        DynamicProcElements[index].y1 = IntToFix(proc->y1);
        DynamicProcElements[index].color = BRIGHT_COLOR;
        DynamicProcElements[index].speed = proc->speed;
        DynamicProcElements[index].dx = FloatToFix(((prand() % 100) - 50) / 200.0);
        if ((prand() % 10) == 0) {
          DynamicProcElements[index].dy = FloatToFix(-((prand() % 100) / 300.0));
          DynamicProcElements[index].frames_left = (prand() % 6) + 3;
        } else {
          DynamicProcElements[index].dy = FloatToFix((prand() % 100) / 50.0);
          DynamicProcElements[index].frames_left = (prand() % 10) + 15;
        }
      }
    }
  }
}
void DoDynamicFountain(int handle, dynamic_proc_element *proc) {
  PlaceProcPoint(FixToInt(proc->x1), FixToInt(proc->y1), proc->color);
  proc->frames_left--;
  if (proc->frames_left < 1)
    ProcElementUnlink(proc - DynamicProcElements, handle);
  else {
    proc->color--;
    if (proc->color == 0)
      ProcElementUnlink(proc - DynamicProcElements, handle);
    else {
      proc->x1 += proc->dx;
      proc->y1 += proc->dy;
    }
  }
}
void AddProcCone(int handle, static_proc_element *proc) {
  if (proc->frequency == 0 || (FrameCount % proc->frequency) == 0) {
    // Add a new fizzle
    int num = (prand() % 4) + 1;
    for (int i = 0; i < num; i++) {
      int index = ProcElementAllocate();
      if (index > -1) {
        ProcElementLink(index, handle);
        DynamicProcElements[index].type = proc->type;

        DynamicProcElements[index].x1 = IntToFix(proc->x1);
        DynamicProcElements[index].y1 = IntToFix(proc->y1);

        DynamicProcElements[index].color = BRIGHT_COLOR;
        DynamicProcElements[index].speed = proc->speed;
        DynamicProcElements[index].dx = FloatToFix(((prand() % 100) - 50) / 80.0);
        if ((prand() % 10) == 0) {
          DynamicProcElements[index].dy = FloatToFix(-((prand() % 100) / 300.0));
          DynamicProcElements[index].frames_left = (prand() % 6) + 3;
        } else {
          DynamicProcElements[index].dy = IntToFix(1);
          DynamicProcElements[index].frames_left = (prand() % 10) + 15;
        }
      }
    }
  }
}
void DoDynamicCone(int handle, dynamic_proc_element *proc) {
  PlaceProcPoint(FixToInt(proc->x1), FixToInt(proc->y1), proc->color);
  proc->frames_left--;
  if (proc->frames_left < 1)
    ProcElementUnlink(proc - DynamicProcElements, handle);
  else {
    proc->color--;
    if (proc->color == 0)
      ProcElementUnlink(proc - DynamicProcElements, handle);
    else {
      proc->x1 += proc->dx;
      proc->y1 += proc->dy;
    }
  }
}
void AddProcFallRight(int handle, static_proc_element *proc) {
  if (proc->frequency == 0 || (FrameCount % proc->frequency) == 0) {
    // Add a new fizzle
    int num = (prand() % 2) + 1;
    for (int i = 0; i < num; i++) {
      int index = ProcElementAllocate();
      if (index > -1) {
        ProcElementLink(index, handle);
        DynamicProcElements[index].type = proc->type;

        DynamicProcElements[index].x1 = IntToFix(proc->x1 + ((ps_rand() % 5) - 2));
        DynamicProcElements[index].y1 = IntToFix(proc->y1 + ((ps_rand() % 5) - 2));
        DynamicProcElements[index].color = BRIGHT_COLOR;
        DynamicProcElements[index].dx = IntToFix(1);
        DynamicProcElements[index].dy = FloatToFix((-(prand() % 100)) / 300.0);
        DynamicProcElements[index].frames_left = (prand() % 15) + 25;
      }
    }
  }
}
void DoDynamicFallRight(int handle, dynamic_proc_element *proc) {
  PlaceProcPoint(FixToInt(proc->x1), FixToInt(proc->y1), proc->color);
  proc->frames_left--;
  if (proc->frames_left < 1)
    ProcElementUnlink(proc - DynamicProcElements, handle);
  else {
    proc->color--;
    if (proc->color == 0)
      ProcElementUnlink(proc - DynamicProcElements, handle);
    else {
      if (FixToInt(proc->dx) > 0)
        proc->dx -= FloatToFix(((ps_rand() % 100) / 2000.0));
      if (FixToInt(proc->dy) < 2)
        proc->dy += FloatToFix(((ps_rand() % 100) / 1000.0));
      proc->x1 += proc->dx;
      proc->y1 += proc->dy;
    }
  }
}
void AddProcFallLeft(int handle, static_proc_element *proc) {
  if (proc->frequency == 0 || (FrameCount % proc->frequency) == 0) {
    // Add a new fizzle
    int num = (prand() % 2) + 1;
    for (int i = 0; i < num; i++) {
      int index = ProcElementAllocate();
      if (index > -1) {
        ProcElementLink(index, handle);
        DynamicProcElements[index].type = proc->type;

        DynamicProcElements[index].x1 = IntToFix(proc->x1 + ((ps_rand() % 5) - 2));
        DynamicProcElements[index].y1 = IntToFix(proc->y1 + ((ps_rand() % 5) - 2));
        DynamicProcElements[index].color = BRIGHT_COLOR;
        DynamicProcElements[index].dx = IntToFix(-1);
        DynamicProcElements[index].dy = FloatToFix((-(prand() % 100)) / 300.0);
        DynamicProcElements[index].frames_left = (prand() % 15) + 25;
      }
    }
  }
}
void DoDynamicFallLeft(int handle, dynamic_proc_element *proc) {
  PlaceProcPoint(FixToInt(proc->x1), FixToInt(proc->y1), proc->color);
  proc->frames_left--;
  if (proc->frames_left < 1)
    ProcElementUnlink(proc - DynamicProcElements, handle);
  else {
    proc->color--;
    if (proc->color == 0)
      ProcElementUnlink(proc - DynamicProcElements, handle);
    else {
      if (FixToInt(proc->dx) < 0)
        proc->dx += FloatToFix(((ps_rand() % 100) / 2000.0));
      if (FixToInt(proc->dy) < 2)
        proc->dy += FloatToFix(((ps_rand() % 100) / 1000.0));
      proc->x1 += proc->dx;
      proc->y1 += proc->dy;
    }
  }
}
void CalcWater2(int handle, int density) {
  int newh;
  int count = PROC_SIZE + 1;
  proc_struct *procedural = GameTextures[handle].procedural;

  short *newptr = (short *)procedural->proc2;
  short *oldptr = (short *)procedural->proc1;

  int x, y;

  // Do main block
  for (y = 1; y < (PROC_SIZE - 1); y++, count += 2) {
    for (x = 1; x < PROC_SIZE - 1; x++, count++) {
      newh = ((oldptr[count + PROC_SIZE] + oldptr[count - PROC_SIZE] + oldptr[count + 1] + oldptr[count - 1] +
               oldptr[count - PROC_SIZE - 1] + oldptr[count - PROC_SIZE + 1] + oldptr[count + PROC_SIZE - 1] +
               oldptr[count + PROC_SIZE + 1]) >>
              2) -
             newptr[count];
      newptr[count] = newh - (newh >> density);
    }
  }
  int up, down, left, right;
  count = 0;
  for (y = 0; y < PROC_SIZE; y++) {
    if (y == 0)
      up = -((PROC_SIZE - 1) * PROC_SIZE);
    else
      up = PROC_SIZE;
    if (y == PROC_SIZE - 1)
      down = -((PROC_SIZE - 1) * PROC_SIZE);
    else
      down = PROC_SIZE;
    for (x = 0; x < PROC_SIZE; x++, count++) {
      if ((y != 0 && y != PROC_SIZE - 1) && x != 0 && x != PROC_SIZE - 1) {
        if (x == 1) {
          x = PROC_SIZE - 2;
          count += (PROC_SIZE - 3);
        }
        continue;
      }
      if (x == 0)
        left = -(PROC_SIZE - 1);
      else
        left = 1;

      if (x == PROC_SIZE - 1)
        right = -(PROC_SIZE - 1);
      else
        right = 1;
      newh = oldptr[count + down];
      newh += oldptr[count - up];
      newh += oldptr[count + right];
      newh += oldptr[count - left];
      newh += oldptr[count - up - left];
      newh += oldptr[count - up + right];
      newh += oldptr[count + down - left];
      newh += oldptr[count + down + right];
      newh >>= 2;
      newh -= newptr[count];
      newptr[count] = newh - (newh >> density);
    }
  }
}
void CalcWater(int handle, int density) {
  int newh;
  int count = PROC_SIZE + 1;
  proc_struct *procedural = GameTextures[handle].procedural;

  short *newptr = (short *)procedural->proc2;
  short *oldptr = (short *)procedural->proc1;

  int x, y;

  // Do main block
  for (y = 1; y < (PROC_SIZE - 1); y++, count += 2) {
    for (x = 1; x < PROC_SIZE - 1; x++, count++) {
      newh = ((oldptr[count + PROC_SIZE] + oldptr[count - PROC_SIZE] + oldptr[count + 1] + oldptr[count - 1]) >> 1) -
             newptr[count];
      newptr[count] = newh - (newh >> density);
    }
  }
  int up, down, left, right;
  count = 0;
  for (y = 0; y < PROC_SIZE; y++) {
    if (y == 0)
      up = -((PROC_SIZE - 1) * PROC_SIZE);
    else
      up = PROC_SIZE;
    if (y == PROC_SIZE - 1)
      down = -((PROC_SIZE - 1) * PROC_SIZE);
    else
      down = PROC_SIZE;
    for (x = 0; x < PROC_SIZE; x++, count++) {
      if ((y != 0 && y != PROC_SIZE - 1) && x != 0 && x != PROC_SIZE - 1) {
        if (x == 1) {
          x = PROC_SIZE - 2;
          count += (PROC_SIZE - 3);
        }
        continue;
      }
      if (x == 0)
        left = -(PROC_SIZE - 1);
      else
        left = 1;

      if (x == PROC_SIZE - 1)
        right = -(PROC_SIZE - 1);
      else
        right = 1;
      newh = oldptr[count + down];
      newh += oldptr[count - up];
      newh += oldptr[count + right];
      newh += oldptr[count - left];
      newh >>= 1;
      newh -= newptr[count];
      newptr[count] = newh - (newh >> density);
    }
  }
}
void DrawWaterNoLight(int handle) {
  int dx, dy;
  int x, y;
  int offset = 0;
  proc_struct *procedural = GameTextures[handle].procedural;
  int dest_bitmap = procedural->procedural_bitmap;
  ushort *dest_data = (ushort *)bm_data(dest_bitmap, 0);
  ushort *src_data = (ushort *)bm_data(GameTextures[handle].bm_handle, 0);
  short *ptr = (short *)procedural->proc1;
  for (y = 0; y < PROC_SIZE; y++) {
    for (x = 0; x < PROC_SIZE; x++, offset++) {
      if (x == PROC_SIZE - 1)
        dx = ptr[offset] - ptr[offset - (PROC_SIZE - 1)];
      else
        dx = ptr[offset] - ptr[offset + 1];
      if (y == PROC_SIZE - 1)
        dy = ptr[offset] - ptr[offset - ((PROC_SIZE - 1) * PROC_SIZE)];
      else
        dy = ptr[offset] - ptr[offset + PROC_SIZE];
      if (dy < 0)
        dy = 0;
      if (dx < 0)
        dx = 0;

      int yoffset = (y + (dy >> 3)) % PROC_SIZE;
      int xoffset = (x + (dx >> 3)) % PROC_SIZE;
      dest_data[offset] = src_data[yoffset * PROC_SIZE + xoffset];
    }
  }
}
void DrawWaterWithLight(int handle, int lightval) {
  int dx, dy;
  int x, y;
  ushort c;
  int offset = 0;
  proc_struct *procedural = GameTextures[handle].procedural;
  int dest_bitmap = procedural->procedural_bitmap;
  ushort *dest_data = (ushort *)bm_data(dest_bitmap, 0);
  ushort *src_data = (ushort *)bm_data(GameTextures[handle].bm_handle, 0);
  short *ptr = (short *)procedural->proc1;
  for (y = 0; y < PROC_SIZE; y++) {
    int ychange, ychange2;
    if (y == PROC_SIZE - 1) {
      ychange = PROC_SIZE;
      ychange2 = ((PROC_SIZE - 1) * PROC_SIZE);
    } else if (y == 0) {
      ychange = -((PROC_SIZE - 1) * PROC_SIZE);
      ychange2 = -PROC_SIZE;
    } else {
      ychange = PROC_SIZE;
      ychange2 = -PROC_SIZE;
    }

    for (x = 0; x < PROC_SIZE; x++, offset++) {
      if (x == PROC_SIZE - 1)
        dx = ptr[offset - 1] - ptr[offset - (PROC_SIZE - 1)];
      else if (x == 0)
        dx = ptr[offset + (PROC_SIZE - 1)] - ptr[offset + 1];
      else
        dx = ptr[offset - 1] - ptr[offset + 1];
      dy = ptr[offset - ychange] - ptr[offset - ychange2];

      int yoffset = (y + (dy >> 3));
      int xoffset = (x + (dx >> 3));
      yoffset &= (PROC_SIZE - 1);
      xoffset &= (PROC_SIZE - 1);
      ASSERT(yoffset >= 0);
      ASSERT(xoffset >= 0);
      c = src_data[yoffset * PROC_SIZE + xoffset];
      int l = (NUM_WATER_SHADES / 2) - (dx >> lightval);
      if (l > (NUM_WATER_SHADES - 1))
        l = NUM_WATER_SHADES - 1;
      if (l < 0)
        l = 0;
      c &= ~OPAQUE_FLAG;

      c = WaterProcTableHi[l][c >> 8] + WaterProcTableLo[l][c & 0xFF];

      dest_data[offset] = c;
    }
  }
}
void AddProcHeightBlob(static_proc_element *proc, int handle) {
  int rquad;
  int cx, cy, cyq;
  int left, top, right, bottom;
  short *dest_data = (short *)GameTextures[handle].procedural->proc1;
  int x = proc->x1;
  int y = proc->y1;
  int procnum = proc - GameTextures[handle].procedural->static_proc_elements;
  if (proc->frequency && ((FrameCount + procnum) % proc->frequency))
    return;
  int radius = proc->size;
  int height = proc->speed;
  rquad = radius * radius;

  left = -radius;
  right = radius;
  top = -radius;
  bottom = radius;
  // Perform edge clipping...
  if (x - radius < 1)
    left -= (x - radius - 1);
  if (y - radius < 1)
    top -= (y - radius - 1);
  if (x + radius > PROC_SIZE - 1)
    right -= (x + radius - PROC_SIZE + 1);
  if (y + radius > PROC_SIZE - 1)
    bottom -= (y + radius - PROC_SIZE + 1);
  for (cy = top; cy < bottom; cy++) {
    cyq = cy * cy;
    for (cx = left; cx < right; cx++) {
      if (cx * cx + cyq < rquad)
        dest_data[PROC_SIZE * (cy + y) + (cx + x)] += height;
    }
  }
}
void AddProcRaindrops(static_proc_element *proc, int handle) {
  int procnum = proc - GameTextures[handle].procedural->static_proc_elements;
  if (proc->frequency && ((FrameCount + procnum) % proc->frequency))
    return;
  ubyte proc_frequency = proc->frequency;
  ubyte proc_size = proc->size;
  ubyte proc_speed = proc->speed;
  ubyte x1 = proc->x1;
  ubyte y1 = proc->y1;
  proc->frequency = 0;
  proc->size = (ps_rand() % 3) + 1;
  proc->speed = std::max(0, proc_speed + ((ps_rand() % 10) - 5));
  proc->x1 = x1 + (ps_rand() % (proc_size * 2)) - (proc_size);
  proc->y1 = y1 + (ps_rand() % (proc_size * 2)) - (proc_size);
  AddProcHeightBlob(proc, handle);
  proc->x1 = x1;
  proc->y1 = y1;
  proc->size = proc_size;
  proc->speed = proc_speed;
  proc->frequency = proc_frequency;
}
void AddProcBlobdrops(static_proc_element *proc, int handle) {
  int procnum = proc - GameTextures[handle].procedural->static_proc_elements;
  if (proc->frequency && ((FrameCount + procnum) % proc->frequency))
    return;
  ubyte proc_frequency = proc->frequency;
  ubyte proc_size = proc->size;
  ubyte proc_speed = proc->speed;
  ubyte x1 = proc->x1;
  ubyte y1 = proc->y1;
  proc->frequency = 0;
  proc->size = (ps_rand() % 6) + 4;
  proc->speed = std::max(0, proc_speed + ((ps_rand() % 50) - 25));
  proc->x1 = x1 + (ps_rand() % (proc_size * 2)) - (proc_size);
  proc->y1 = y1 + (ps_rand() % (proc_size * 2)) - (proc_size);
  AddProcHeightBlob(proc, handle);
  proc->x1 = x1;
  proc->y1 = y1;
  proc->size = proc_size;
  proc->speed = proc_speed;
  proc->frequency = proc_frequency;
}
void AddProcSineBlob(static_proc_element *proc, int handle) {
  int procnum = proc - GameTextures[handle].procedural->static_proc_elements;
  if (proc->frequency && ((FrameCount + procnum) % proc->frequency))
    return;
  int x = proc->x1;
  int y = proc->y1;
  int cx, cy;
  int left, top, right, bottom;
  int square, dist;
  int radius = proc->size;
  int radsquare = radius * radius;
  float length = (1024.0 / (float)radius) * (1024.0 / (float)radius);
  short *dest_data = (short *)GameTextures[handle].procedural->proc1;
  int height = proc->speed;
  radsquare = (radius * radius);
  left = -radius;
  right = radius;
  top = -radius;
  bottom = radius;
  // Perform edge clipping...
  if (x - radius < 1)
    left -= (x - radius - 1);
  if (y - radius < 1)
    top -= (y - radius - 1);
  if (x + radius > PROC_SIZE - 1)
    right -= (x + radius - PROC_SIZE + 1);
  if (y + radius > PROC_SIZE - 1)
    bottom -= (y + radius - PROC_SIZE + 1);
  for (cy = top; cy < bottom; cy++) {
    for (cx = left; cx < right; cx++) {
      square = cy * cy + cx * cx;
      if (square < radsquare) {
        dist = sqrt(square * length);
        int addval = (FixCos(dist % 65536) * (float)height);
        addval /= 8;
        dest_data[PROC_SIZE * (cy + y) + cx + x] += addval;
      }
    }
  }
}
void AllocateMemoryForWaterProcedural(int handle) {
  proc_struct *procedural = GameTextures[handle].procedural;
  int w = PROC_SIZE;
  int h = PROC_SIZE;
  if (procedural->proc1)
    mem_free(procedural->proc1);
  if (procedural->proc2)
    mem_free(procedural->proc2);
  GameTextures[handle].procedural->proc1 = (ushort *)mem_malloc(w * h * 2);
  ASSERT(GameTextures[handle].procedural->proc1);
  memset(GameTextures[handle].procedural->proc1, 0, w * h * 2);

  GameTextures[handle].procedural->proc2 = (ushort *)mem_malloc(w * h * 2);
  ASSERT(GameTextures[handle].procedural->proc2);
  memset(GameTextures[handle].procedural->proc2, 0, w * h * 2);
  procedural->memory_type = PROC_MEMORY_TYPE_WATER;
}
// Does a water effect for this texture
void EvaluateWaterProcedural(int handle) {
  proc_struct *procedural = GameTextures[handle].procedural;
  if (procedural->memory_type != PROC_MEMORY_TYPE_WATER)
    AllocateMemoryForWaterProcedural(handle);

  int dest_bitmap = procedural->procedural_bitmap;
  for (int i = 0; i < procedural->num_static_elements; i++) {
    static_proc_element *proc = &procedural->static_proc_elements[i];
    switch (proc->type) {
    case PROC_WATER_HEIGHT_BLOB:
      AddProcHeightBlob(proc, handle);
      break;
    case PROC_WATER_SINE_BLOB:
      AddProcSineBlob(proc, handle);
      break;
    case PROC_WATER_RAINDROPS:
      AddProcRaindrops(proc, handle);
      break;
    case PROC_WATER_BLOBDROPS:
      AddProcBlobdrops(proc, handle);
      break;
    default:
      break;
    }
  }
  if (EasterEgg) {
    if (Easter_egg_handle != -1) {
      ushort *src_data = bm_data(Easter_egg_handle, 0);
      short *dest_data = (short *)GameTextures[handle].procedural->proc1;
      int sw = bm_w(Easter_egg_handle, 0);
      int sh = bm_w(Easter_egg_handle, 0);
      int x1 = (PROC_SIZE / 2) - (sw / 2);
      int y1 = (PROC_SIZE / 2) - (sh / 2);
      // Make sure size is valid
      if (sw <= PROC_SIZE && sh <= PROC_SIZE) {
        int i, t;
        for (i = 0; i < sh; i++) {
          for (t = 0; t < sw; t++) {
            if (src_data[i * sw + t] & OPAQUE_FLAG)
              dest_data[((y1 + i) * PROC_SIZE) + t + x1] += 200;
          }
        }
      }
    }
    EasterEgg = 0;
  }
  // Calculate the water on the current texture
  if (!procedural->light)
    DrawWaterNoLight(handle);
  else
    DrawWaterWithLight(handle, procedural->light - 1);
  int thickness = procedural->thickness;
  if (procedural->osc_time > 0) {
    int start = procedural->osc_value;
    int end = procedural->thickness;
    if (start > end) {
      int t = start;
      start = end;
      end = t;
    }
    int diff = end - start;
    if (diff > 0) {
      float gametime = timer_GetTime();
      float frametime = procedural->osc_time / (diff);
      float cur_frametime = gametime / frametime;
      int int_frame = cur_frametime;

      int_frame %= ((diff) * 2);
      if (int_frame >= diff)
        int_frame = (diff - 1) - (int_frame % diff);
      else
        int_frame %= diff;

      thickness = start + int_frame;
    }
  }
  CalcWater(handle, thickness);
  // Swap for next time
  short *temp = (short *)procedural->proc1;
  procedural->proc1 = procedural->proc2;
  procedural->proc2 = temp;
}
void AllocateMemoryForFireProcedural(int handle) {
  proc_struct *procedural = GameTextures[handle].procedural;
  int w = PROC_SIZE;
  int h = PROC_SIZE;
  if (procedural->proc1)
    mem_free(procedural->proc1);
  if (procedural->proc2)
    mem_free(procedural->proc2);
  GameTextures[handle].procedural->proc1 = (ubyte *)mem_malloc(w * h * 1);
  ASSERT(GameTextures[handle].procedural->proc1);
  memset(GameTextures[handle].procedural->proc1, 0, w * h * 1);

  GameTextures[handle].procedural->proc2 = (ubyte *)mem_malloc(w * h * 1);
  ASSERT(GameTextures[handle].procedural->proc2);
  memset(GameTextures[handle].procedural->proc2, 0, w * h * 1);
  procedural->memory_type = PROC_MEMORY_TYPE_FIRE;
}
void EvaluateFireProcedural(int handle) {
  proc_struct *procedural = GameTextures[handle].procedural;

  int dest_bitmap = procedural->procedural_bitmap;
  if (procedural->memory_type != PROC_MEMORY_TYPE_FIRE) {
    AllocateMemoryForFireProcedural(handle);
  }
  ProcDestData = (ubyte *)procedural->proc1;
  // fade and the current texture
  FadeProcTexture(handle);
  // HeatProcTexture (handle);
  // Do the static procedurals first
  int i;
  for (i = 0; i < procedural->num_static_elements; i++) {
    static_proc_element *proc = &procedural->static_proc_elements[i];
    switch (proc->type) {
    case PROC_LINE_LIGHTNING:
      AddProcLightning(proc->x1, proc->y1, proc->x2, proc->y2, BRIGHT_COLOR, proc);
      break;
    case PROC_SPHERE_LIGHTNING:
      AddProcSphereLightning(handle, proc);
      break;
    case PROC_RISING_EMBER:
      AddProcRisingEmber(handle, proc);
      break;
    case PROC_RANDOM_EMBERS:
      AddProcRandomEmber(handle, proc);
      break;
    case PROC_SPINNERS:
      AddProcSpinners(handle, proc);
      break;
    case PROC_ROAMERS:
      AddProcRoamers(handle, proc);
      break;
    case PROC_FOUNTAIN:
      AddProcFountain(handle, proc);
      break;
    case PROC_CONE:
      AddProcCone(handle, proc);
      break;
    case PROC_FALL_RIGHT:
      AddProcFallRight(handle, proc);
      break;
    case PROC_FALL_LEFT:
      AddProcFallLeft(handle, proc);
      break;
    default:
      break;
    }
  }
  int proc_num = procedural->dynamic_proc_elements;
  while (proc_num != -1) {
    dynamic_proc_element *proc = &DynamicProcElements[proc_num];

    switch (proc->type) {
    case PROC_RISING_EMBER:
      DoDynamicRisingEmber(handle, proc);
      break;
    case PROC_RANDOM_EMBERS:
      DoDynamicRandomEmber(handle, proc);
      break;
    case PROC_SPINNERS:
      DoDynamicSpinners(handle, proc);
      break;
    case PROC_ROAMERS:
      DoDynamicRoamers(handle, proc);
      break;
    case PROC_FOUNTAIN:
      DoDynamicFountain(handle, proc);
      break;
    case PROC_CONE:
      DoDynamicCone(handle, proc);
      break;
    case PROC_FALL_RIGHT:
      DoDynamicFallRight(handle, proc);
      break;
    case PROC_FALL_LEFT:
      DoDynamicFallLeft(handle, proc);
      break;
    default:
      break;
    }
    proc_num = DynamicProcElements[proc_num].next;
  }

  // blend the current texture
  BlendProcTexture(handle);
  ProcDestData = (ubyte *)procedural->proc2;
  int total = PROC_SIZE * PROC_SIZE;
  ushort *data = bm_data(dest_bitmap, 0);
  ushort *pal = procedural->palette;
  for (i = 0; i < total; i++, data++, ProcDestData++) {
    *data = pal[*ProcDestData];
  }
  // Swap for next time
  ubyte *temp = (ubyte *)procedural->proc1;
  procedural->proc1 = procedural->proc2;
  procedural->proc2 = temp;
}
// Does a procedural for this texture
void EvaluateProcedural(int handle) {
  proc_struct *procedural = GameTextures[handle].procedural;

  int dest_bitmap = procedural->procedural_bitmap;
  if (bm_w(dest_bitmap, 0) != PROC_SIZE) {
    mprintf((0, "Couldn't evaluate procedural because its not %d x %d!\n", PROC_SIZE, PROC_SIZE));
    return;
  }
  if (GameTextures[handle].flags & TF_WATER_PROCEDURAL)
    EvaluateWaterProcedural(handle);
  else
    EvaluateFireProcedural(handle);
}
