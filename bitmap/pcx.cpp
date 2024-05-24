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

 * $Logfile: /DescentIII/Main/bitmap/pcx.cpp $
 * $Revision: 8 $
 * $Date: 3/30/99 12:36a $
 * $Author: Jeff $
 *
 * Code to read PCX files
 *
 * $Log: /DescentIII/Main/bitmap/pcx.cpp $
 *
 * 8     3/30/99 12:36a Jeff
 * added support for 24bit pcx's
 *
 * 7     2/16/99 5:50p Jason
 * made pcx stuff only work with 8 bit files
 *
 * 6     10/20/98 1:41a Jeff
 * fixed a mem_malloc/free bug
 *
 * 5     10/08/98 4:23p Kevin
 * Changed code to comply with memory library usage. Always use mem_malloc
 * , mem_free and mem_strdup
 *
 * 4     4/23/98 6:38p Jason
 * made bitmaps use 1555 format
 *
 * 3     12/22/97 7:34p Samir
 * Removed instances of gr.h include.  Replaced with grdefs.h
 *
 * 2     10/15/97 5:20p Jason
 * did a HUGE overhaul of the bitmap system
 *
 * 4     3/03/97 6:20p Matt
 * Changed cfile functions to use D3 naming convention
 *
 * $NoKeywords: $
 */

#include "mem.h"
#include "bitmap.h"
#include "pserror.h"
#include "pstypes.h"
#include "grdefs.h"
#include <stdlib.h>

// load an 8bit pcx image
static int bm_pcx_8bit_alloc_file(CFILE *infile);
// load a 24bit pcx image
static int bm_pcx_24bit_alloc_file(CFILE *infile);

// Loads a pcx file and converts it to 16 bit.  Returns bitmap handle or -1 on error
int bm_pcx_alloc_file(CFILE *infile) {
  uint8_t temp[128];

  cf_ReadBytes(temp, 128, infile);
  cfseek(infile, 0, SEEK_SET);

  switch (temp[65]) {
  case 1:
    // one plane, sounds like an 8bit pcx
    return bm_pcx_8bit_alloc_file(infile);
    break;
  case 3:
    // three planes, sounds like a 24bit pcx
    return bm_pcx_24bit_alloc_file(infile);
    break;
  }

  return -1; // Must be 8 bit only
}

// load an 8bit pcx image
int bm_pcx_8bit_alloc_file(CFILE *infile) {
  int xmin, ymin, xmax, ymax;
  int width, height;
  int total, run = 0;
  int i, t;
  int src_bm;
  uint8_t pred[256], pgreen[256], pblue[256];
  uint8_t buf;
  uint8_t temp[128];

  cf_ReadBytes(temp, 4, infile);

  if (temp[3] != 8)
    return -1; // nope...bail

  xmin = cf_ReadShort(infile);
  ymin = cf_ReadShort(infile);
  xmax = cf_ReadShort(infile);
  ymax = cf_ReadShort(infile);

  cf_ReadBytes(temp, 116, infile);

  if (temp[65 - 12] != 1)
    return -1; // Must be 8 bit only

  width = 1 + xmax - xmin;
  height = 1 + ymax - ymin;
  total = width * height;

  uint8_t *rawdata = (uint8_t *)mem_malloc(width * height);
  if (!rawdata)
    return -1; // no memory!

  run = 0;
  while (run < total) {
    buf = cf_ReadByte(infile);
    if (buf >= 192) {
      uint8_t tb;
      tb = cf_ReadByte(infile);
      for (i = 0; i < (buf - 192); i++, run++)
        rawdata[run] = tb;
    } else {
      rawdata[run] = buf;
      run++;
    }
  }

  cf_ReadByte(infile); // ignore pad

  // Read palette

  for (i = 0; i < 256; i++) {
    pred[i] = cf_ReadByte(infile);
    pgreen[i] = cf_ReadByte(infile);
    pblue[i] = cf_ReadByte(infile);

    pred[i] >>= 3;
    pgreen[i] >>= 3;
    pblue[i] >>= 3;
  }

  src_bm = bm_AllocBitmap(width, height, 0);
  if (src_bm < 0)
    return -1; // probably out of memory

  uint16_t *data = bm_data(src_bm, 0);

  for (i = 0; i < height; i++) {
    for (t = 0; t < width; t++) {
      uint16_t pixel;
      uint8_t c = rawdata[i * width + t];

      int r = pred[c];
      int g = pgreen[c];
      int b = pblue[c];

      pixel = OPAQUE_FLAG | (r << 10) | (g << 5) | b;
      if (c == 0)
        pixel = NEW_TRANSPARENT_COLOR;

      data[i * width + t] = pixel;
    }
  }
  mem_free(rawdata);

  return src_bm; // success!
}

// load a 24bit pcx image
int bm_pcx_24bit_alloc_file(CFILE *infile) {
  int xmin, ymin, xmax, ymax;
  int width, height;
  int total, run = 0;
  int i, t;
  int src_bm;
  uint8_t buf;
  uint8_t temp[128];

  cf_ReadBytes(temp, 4, infile);

  if (temp[1] < 5) {
    // need at least version 5.0f
    mprintf(0, "PCXLoad: PCX Not version 5.0 or greater\n");
    return -1;
  }

  if (temp[3] != 8) {
    // need 8 bits per pixel
    mprintf(0, "PCXLoad: PCX Not 8 bpp\n");
    return -1; // nope...bail
  }

  xmin = cf_ReadShort(infile);
  ymin = cf_ReadShort(infile);
  xmax = cf_ReadShort(infile);
  ymax = cf_ReadShort(infile);

  cf_ReadBytes(temp, 116, infile);

#define PCXHEADER_OFFSET 12

  if (temp[65 - PCXHEADER_OFFSET] != 3) {
    // Must have 3 planes
    mprintf(0, "PCXLoad: PCX Not 3 Planes for 24bit encoding\n");
    return -1;
  }

  width = 1 + xmax - xmin;
  height = 1 + ymax - ymin;

  // Determine BytesPerLine
  cfseek(infile, 66, SEEK_SET);
  int BytesPerLine = cf_ReadShort(infile);
  cfseek(infile, 128, SEEK_SET);

  // scanline length
  total = 3 * BytesPerLine;

  uint8_t *rawdata = (uint8_t *)mem_malloc(total * height);
  if (!rawdata) {
    mprintf(0, "PCXLoad: Out of memory\n");
    return -1; // no memory!
  }

  // Load in the data
  // the data is divided into scanlines
  // the first scanline is line 0's red
  // the second scanline is line 0's green
  // the third scanline is line 0's blue
  // the fourth scanline is line 1's red
  // etc.
  int data_index = 0;
  for (int line = 0; line < height; line++) {
    // Read red scanline
    run = 0;
    while (run < BytesPerLine) {
      buf = cf_ReadByte(infile);
      if (buf >= 192) {
        uint8_t tb;
        tb = cf_ReadByte(infile);
        for (i = 0; i < (buf - 192); i++, run++, data_index++)
          rawdata[data_index] = tb;
      } else {
        rawdata[data_index] = buf;
        run++;
        data_index++;
      }
    }

    // Read green scanline
    run = 0;
    while (run < BytesPerLine) {
      buf = cf_ReadByte(infile);
      if (buf >= 192) {
        uint8_t tb;
        tb = cf_ReadByte(infile);
        for (i = 0; i < (buf - 192); i++, run++, data_index++)
          rawdata[data_index] = tb;
      } else {
        rawdata[data_index] = buf;
        run++;
        data_index++;
      }
    }

    // Read blue scanline
    run = 0;
    while (run < BytesPerLine) {
      buf = cf_ReadByte(infile);
      if (buf >= 192) {
        uint8_t tb;
        tb = cf_ReadByte(infile);
        for (i = 0; i < (buf - 192); i++, run++, data_index++)
          rawdata[data_index] = tb;
      } else {
        rawdata[data_index] = buf;
        run++;
        data_index++;
      }
    }
  }

  src_bm = bm_AllocBitmap(width, height, 0);
  if (src_bm < 0)
    return -1; // probably out of memory

  uint16_t *data = bm_data(src_bm, 0);

  for (i = 0; i < height; i++) {
    for (t = 0; t < width; t++) {
      int r, g, b;
      uint16_t pixel;

      r = rawdata[(i * total) + (0 * BytesPerLine) + t];
      g = rawdata[(i * total) + (1 * BytesPerLine) + t];
      b = rawdata[(i * total) + (2 * BytesPerLine) + t];

      pixel = OPAQUE_FLAG | GR_RGB16(r, g, b);

      data[i * width + t] = pixel;
    }
  }
  mem_free(rawdata);

  return src_bm; // success!
}
