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

/*
 * $Logfile: /DescentIII/Main/bitmap/tga.cpp $
 * $Revision: 28 $
 * $Date: 9/06/01 10:25a $
 * $Author: Matt $
 *
 * Functions to read TGA files
 *
 * $Log: /DescentIII/Main/bitmap/tga.cpp $
 *
 * 28    9/06/01 10:25a Matt
 * Took out annoying Int3
 *
 * 27    2/01/00 2:37a Jason
 * Fixed problem with bad tga files
 *
 * 26    10/21/99 9:28p Jeff
 * B.A. Macintosh code merge
 *
 * 25    4/14/99 1:07a Jeff
 * fixed case mismatched #includes
 *
 * 24    2/16/99 12:42p Kevin
 * Improvements to the paging data progress indicator
 *
 * 23    2/09/99 7:01p Kevin
 * First work for new and improved progress screen while loading a level.
 * Note that this is a hack at this point, while I get the details worked
 * out, then I'll make it cleaner.
 *
 * 22    1/14/99 2:26p Jason
 * made data tracking more reliable
 *
 * 21    12/18/98 3:16p Jason
 * added specularity to mine walls
 *
 * 20    10/20/98 1:04p Jason
 * fixed import problem
 *
 * 19    10/19/98 4:22p Jason
 * more fixes for Beta5
 *
 * 18    10/09/98 3:32p Kevin
 * New memory library
 *
 * 17    10/08/98 4:23p Kevin
 * Changed code to comply with memory library usage. Always use mem_malloc
 * , mem_free and mem_strdup
 *
 * 16    8/14/98 2:55p Jason
 * fixed reading of 24 bit TGA files
 *
 * 15    5/24/98 1:52a Jeff
 * no changes, checked out and checked back in so people would recompile
 * it, because it is now to be compiled optimized
 *
 * 14    4/23/98 6:38p Jason
 * made bitmaps use 1555 format
 *
 * 13    3/31/98 3:48p Jason
 * added memory lib
 *
 * 12    2/12/98 1:32p Jason
 * got mipmapping working
 *
 * 11    1/12/98 3:00p Keneta
 * Changed assert to Int3 (Jason on MattL's machine)
 *
 * 10    12/31/97 11:11a Samir
 * FROM JASON: Added the ability to read NEW_COMPRESS_MIPPED
 *
 * 9     12/22/97 7:34p Samir
 * Removed instances of gr.h include.  Replaced with grdefs.h
 *
 * 8     12/22/97 3:25p Keneta
 * fixed bug with new texture paging
 *
 * 7     12/19/97 3:36p Jason
 * bug fixes for bitmap paging stuff
 *
 * 6     12/19/97 2:46p Jason
 * implemented bitmap paging routines
 *
 * 5     11/03/97 6:21p Jason
 * fixed some problems with animating textures being remapped improperly
 *
 * 4     10/16/97 4:55p Jason
 * fixed stupid off-by-one bug
 *
 * 3     10/15/97 5:20p Jason
 * did a HUGE overhaul of the bitmap system
 *
 * 2     8/29/97 1:22p Jason
 * added tga screenshots
 *
 * 13    5/29/97 6:21p Jason
 * made tga's better handle pixmap info
 *
 * 12    5/19/97 5:10p Jason
 * changes for our new abstracted renderer code
 *
 * 11    4/30/97 3:15p Jason
 * changes to support both 8bit and 16bit rendering in software
 *
 * 10    3/07/97 4:03p Samir
 * Took out malloc.h for ANSI compliance.
 *
 * 9     3/07/97 1:02p Jason
 * added the ability to read 24 bit tgas as well
 *
 * 8     3/03/97 6:20p Matt
 * Changed cfile functions to use D3 naming convention
 *
 * $NoKeywords: $
 */

#include "cfile.h"
#include "pserror.h"
#include "pstypes.h"
#include "bitmap.h"
#include "mono.h"
#include "grdefs.h"
#include "texture.h"
#include "byteswap.h"
#include <string.h>
#include "mem.h"
#include "gamesequence.h"

#include <stdlib.h>

static char *Tga_file_data = NULL;
static int Fake_pos = 0;
static int Bad_tga = 0;
static int Fake_file_size = 0;

static inline char tga_read_byte();
static inline int tga_read_int();
static inline short tga_read_short();
static ushort bm_tga_translate_pixel(int pixel, int format);
static int bm_tga_read_outrage_compressed16(CFILE *infile, int n, int num_mips, int type);

inline char tga_read_byte() {
  // Check for bad file
  if (Fake_pos + 1 > Fake_file_size) {
    Bad_tga = 1;
    return 0;
  }

  return Tga_file_data[Fake_pos++];
}
inline int tga_read_int() {
  int i;

  // Check for bad file
  if (Fake_pos + 4 > Fake_file_size) {
    Bad_tga = 1;
    return 0;
  }

  i = *(int *)(Tga_file_data + Fake_pos);
  Fake_pos += 4;

  return INTEL_INT(i);
}
inline short tga_read_short() {
  short i;

  // Check for bad file
  if (Fake_pos + 2 > Fake_file_size) {
    Bad_tga = 1;
    return 0;
  }

  i = *(short *)(Tga_file_data + Fake_pos);
  Fake_pos += 2;

  return INTEL_SHORT(i);
}

ushort bm_tga_translate_pixel(int pixel, int format) {
  int red = ((pixel >> 16) & 0xFF);
  int green = ((pixel >> 8) & 0xFF);
  int blue = ((pixel) & 0xFF);
  int alpha = ((pixel >> 24) & 0xFF);
  ushort newpix;

  if (format == BITMAP_FORMAT_4444) {
    int newred = red >> 4;
    int newgreen = green >> 4;
    int newblue = blue >> 4;
    int newalpha = alpha >> 4;
    newpix = (newalpha << 12) | (newred << 8) | (newgreen << 4) | (newblue);
  } else {
    int newred = red >> 3;
    int newgreen = green >> 3;
    int newblue = blue >> 3;

    newpix = OPAQUE_FLAG | (newred << 10) | (newgreen << 5) | (newblue);

    if (alpha == 0)
      newpix = NEW_TRANSPARENT_COLOR;
  }

  return newpix;
}

int bm_tga_read_outrage_compressed16(CFILE *infile, int n, int num_mips, int type) {
  ushort *dest_data;
  ushort pixel;
  int width, height;
  int m;

  for (m = 0; m < num_mips; m++) {

    width = bm_w(n, m);
    height = bm_h(n, m);

    int total = height * width;
    int count = 0;

    dest_data = (ushort *)bm_data(n, m);

    while (count != total) {
      ASSERT(count < total);

      ubyte command = tga_read_byte();

      if (Bad_tga)
        return 0;

      if (command == 0) // next pixel is raw
      {
        pixel = tga_read_short();

        if (Bad_tga)
          return 0;

        if (type != OUTRAGE_1555_COMPRESSED_MIPPED && type != OUTRAGE_4444_COMPRESSED_MIPPED) {
          if (pixel == 0x07e0)
            pixel = NEW_TRANSPARENT_COLOR;
          else {
            int r = ((pixel & 0xF800) >> 11) << 3;
            int g = ((pixel & 0x07e0) >> 5) << 2;
            int b = (pixel & 0x001f) << 3;

            pixel = OPAQUE_FLAG | GR_RGB16(r, g, b);
          }
        }

        int i = count / width;
        int t = count % width;
        dest_data[i * width + t] = pixel;
        count++;
      } else if (command >= 2 && command <= 250) // next pixel is run of pixels
      {
        pixel = tga_read_short();

        if (Bad_tga)
          return 0;

        if (type != OUTRAGE_1555_COMPRESSED_MIPPED && type != OUTRAGE_4444_COMPRESSED_MIPPED) {
          if (pixel == 0x07e0)
            pixel = NEW_TRANSPARENT_COLOR;
          else {
            int r = ((pixel & 0xF800) >> 11) << 3;
            int g = ((pixel & 0x07e0) >> 5) << 2;
            int b = (pixel & 0x001f) << 3;

            pixel = OPAQUE_FLAG | GR_RGB16(r, g, b);
          }
        }

        for (int k = 0; k < command; k++) {
          int i = count / width;
          int t = count % width;
          dest_data[i * width + t] = pixel;
          count++;
        }
      } else
        Int3(); // bad compression run
    }
  }

  // DAJ added to fill out the mip maps down to the 1x1 size (memory is already there)
  // does not average since we are only a pixel or two in size
  if (num_mips > 1) {
    GameBitmaps[n].mip_levels = bm_miplevels(n);
    for (m = num_mips; m < bm_miplevels(n); m++) {
      width = bm_w(n, m);
      height = bm_h(n, m);

      ushort w_prev = bm_w(n, m - 1);
      ushort *dst = bm_data(n, m);
      ushort *src = bm_data(n, m - 1);

      for (int h_inc = 0; h_inc < height; h_inc++) {
        for (int w_inc = 0; w_inc < width; w_inc++) {
          dst[h_inc * width + w_inc] = src[2 * (h_inc * w_prev + w_inc)];
        }
      }
    }
  }

  return 1;
}

// Reads a 8bit bitmap into memory
// Note: Even if we're are not using an 8bit renderer this routine still needs
// to be called due to the fact that we need to 'move' our position in the file
// We also need to extract the palette number

/*int bm_tga_read_outrage_compressed8 (CFILE *infile,int n)
{

        ubyte pal_num=tga_read_byte ();
        ubyte num_mips=tga_read_byte ();

        ASSERT (pal_num>=0 && pal_num<=MAX_BITMAP_PALETTES);
        if (num_mips>1)
                GameBitmaps[n].mipmapped=1;

        GameBitmaps[n].palette_num=pal_num;

        for (int m=0;m<num_mips;m++)
        {
                int width=bm_w(n,m);
                int height=bm_h(n,m);

                int total=height * width;
                int count=0;
                ubyte *data;

                if (Renderer_type==RENDERER_SOFTWARE_8BIT)
                        data=bm_data8 (n,m);
                else
                        data=NULL;

                while (count!=total)
                {
                        ASSERT (count<total);

                        ubyte command=tga_read_byte ();
                        if (command==0)	// next pixel is raw
                        {
                                ubyte pixel=tga_read_byte ();
                                int i=count/width;
                                int t=count%width;
                                if (Renderer_type==RENDERER_SOFTWARE_8BIT)
                                        data[i*256+t]=pixel;
                                count++;
                        }
                        else if (command>=2 && command<=250)	// next pixel is run of pixels
                        {
                                ubyte pixel=tga_read_byte ();
                                for (int k=0;k<command;k++)
                                {
                                        int i=count/width;
                                        int t=count%width;
                                        if (Renderer_type==RENDERER_SOFTWARE_8BIT)
                                                data[i*256+t]=pixel;
                                        count++;
                                }
                        }
                        else Int3();		// bad compression run
                }
        }
        return 1;
}*/

// Loads a tga or ogf file into a bitmap...returns handle to bm or -1 on error
int bm_tga_alloc_file(CFILE *infile, char *name, int format) {
  ubyte image_id_len, color_map_type, image_type, pixsize, descriptor;
  ubyte upside_down = 0;
  ushort width, height;
  unsigned int pixel;
  int i, t, n, data8bit = 0, savepos;
  int mipped = 0;
  int num_mips = 1;
  int read_ok = 1;

  image_id_len = cf_ReadByte(infile);
  color_map_type = cf_ReadByte(infile);
  image_type = cf_ReadByte(infile);

  if (color_map_type != 0 ||
      (image_type != 10 && image_type != 2 && image_type != OUTRAGE_TGA_TYPE && image_type != OUTRAGE_COMPRESSED_OGF &&
       image_type != OUTRAGE_COMPRESSED_MIPPED && image_type != OUTRAGE_NEW_COMPRESSED_MIPPED &&
       image_type != OUTRAGE_1555_COMPRESSED_MIPPED && image_type != OUTRAGE_4444_COMPRESSED_MIPPED)) {
    mprintf((0, "bm_tga: Can't read this type of TGA.\n"));
    return -1;
  }

  if (image_type == OUTRAGE_COMPRESSED_OGF_8BIT)
    data8bit = 1;

  if (image_type == OUTRAGE_4444_COMPRESSED_MIPPED || image_type == OUTRAGE_1555_COMPRESSED_MIPPED ||
      image_type == OUTRAGE_NEW_COMPRESSED_MIPPED || image_type == OUTRAGE_TGA_TYPE ||
      image_type == OUTRAGE_COMPRESSED_MIPPED || image_type == OUTRAGE_COMPRESSED_OGF ||
      image_type == OUTRAGE_COMPRESSED_OGF_8BIT) {
    if (image_type == OUTRAGE_4444_COMPRESSED_MIPPED || image_type == OUTRAGE_NEW_COMPRESSED_MIPPED ||
        image_type == OUTRAGE_1555_COMPRESSED_MIPPED) {
      cf_ReadString(name, BITMAP_NAME_LEN - 1, infile);
    } else {
      for (i = 0; i < BITMAP_NAME_LEN; i++)
        name[i] = cf_ReadByte(infile);
    }
    if (image_type == OUTRAGE_4444_COMPRESSED_MIPPED || image_type == OUTRAGE_1555_COMPRESSED_MIPPED ||
        image_type == OUTRAGE_COMPRESSED_MIPPED || image_type == OUTRAGE_NEW_COMPRESSED_MIPPED)
      num_mips = cf_ReadByte(infile);
    else
      num_mips = 1;

    if (num_mips > 1)
      mipped = 1;
  }

  for (i = 0; i < 9; i++) // ingore next 9 bytes
    cf_ReadByte(infile);

  width = cf_ReadShort(infile);
  height = cf_ReadShort(infile);
  pixsize = cf_ReadByte(infile);

  if (pixsize != 32 && pixsize != 24) {
    mprintf((0, "bm_tga: This file has a pixsize of field of %d, it should be 32. ", pixsize));
    return -1;
  }

  descriptor = cf_ReadByte(infile);
  if (((descriptor & 0x0F) != 8) && ((descriptor & 0x0F) != 0)) {
    mprintf((0, "bm_tga: Descriptor field & 0x0F must be 8 or 0, but this is %d.", descriptor & 0x0F));
    return -1;
  }

  for (i = 0; i < image_id_len; i++)
    cf_ReadByte(infile);

  n = bm_AllocBitmap(width, height, mipped * ((width * height * 2) / 3));

  if (format == BITMAP_FORMAT_4444 || image_type == OUTRAGE_4444_COMPRESSED_MIPPED)
    GameBitmaps[n].format = BITMAP_FORMAT_4444;

  // Copy the name
  strcpy(GameBitmaps[n].name, name);

  if (mipped)
    GameBitmaps[n].flags |= BF_MIPMAPPED;

  if (n < 0) {
    mprintf((0, "bm_tga: Failed to allocate memory.\n"));
    Int3();
    return -1;
  }

  upside_down = (descriptor & 0x20) >> 5;
  upside_down = 1 - upside_down;

  // Load the actual bitmap data in, converting it from 32 bit to 16 bit, and replacing
  // that pesky transparency color without our replacement

  if (image_type == 10 || image_type == 2) {
    if (image_type == 10) // compressed tga
    {
      int total = 0;

      while (total < (height * width)) {
        ubyte command = cf_ReadByte(infile);
        ubyte len = (command & 127) + 1;

        if (command & 128) // rle chunk
        {
          if (pixsize == 32)
            pixel = cf_ReadInt(infile);
          else {
            int r, g, b;
            r = cf_ReadByte(infile);
            g = cf_ReadByte(infile);
            b = cf_ReadByte(infile);
            pixel = (255 << 24) | (r << 16) | (g << 8) | b;
          }

          ushort newpix = bm_tga_translate_pixel(pixel, format);

          for (int k = 0; k < len; k++, total++) {
            i = total / width;
            t = total % width;

            if (upside_down)
              GameBitmaps[n].data16[((height - 1) - i) * width + t] = newpix;
            else
              GameBitmaps[n].data16[i * width + t] = newpix;
          }
        } else // raw chunk
        {
          for (int k = 0; k < len; k++, total++) {
            if (pixsize == 32)
              pixel = cf_ReadInt(infile);
            else {
              int r, g, b;
              b = (ubyte)cf_ReadByte(infile);
              g = (ubyte)cf_ReadByte(infile);
              r = (ubyte)cf_ReadByte(infile);
              pixel = (255 << 24) | (r << 16) | (g << 8) | b;
            }
            ushort newpix = bm_tga_translate_pixel(pixel, format);

            i = total / width;
            t = total % width;

            if (upside_down)
              GameBitmaps[n].data16[((height - 1) - i) * width + t] = newpix;
            else
              GameBitmaps[n].data16[i * width + t] = newpix;
          }
        }
      }
    } else // uncompressed TGA
    {
      for (i = 0; i < height; i++) {
        for (t = 0; t < width; t++) {
          if (pixsize == 32)
            pixel = cf_ReadInt(infile);
          else {
            int r, g, b;
            b = (ubyte)cf_ReadByte(infile);
            g = (ubyte)cf_ReadByte(infile);
            r = (ubyte)cf_ReadByte(infile);
            pixel = (255 << 24) | (r << 16) | (g << 8) | b;
          }

          ushort newpix = bm_tga_translate_pixel(pixel, format);

          if (upside_down)
            GameBitmaps[n].data16[((height - 1) - i) * width + t] = newpix;
          else
            GameBitmaps[n].data16[i * width + t] = newpix;
        }
      }
    }
  } else if (image_type == OUTRAGE_4444_COMPRESSED_MIPPED || image_type == OUTRAGE_1555_COMPRESSED_MIPPED ||
             image_type == OUTRAGE_NEW_COMPRESSED_MIPPED || image_type == OUTRAGE_COMPRESSED_MIPPED ||
             image_type == OUTRAGE_COMPRESSED_OGF || image_type == OUTRAGE_COMPRESSED_OGF_8BIT) // COMPRESSED OGF
  {
    // read this ogf in all at once (much faster)

    savepos = cftell(infile);
    cfseek(infile, 0, SEEK_END);
    int lastpos = cftell(infile);
    int numleft = lastpos - savepos;

    cfseek(infile, savepos, SEEK_SET);

    Tga_file_data = (char *)mem_malloc(numleft);
    ASSERT(Tga_file_data != NULL);
    Fake_pos = 0;
    Bad_tga = 0;
    Fake_file_size = numleft;

    cf_ReadBytes((ubyte *)Tga_file_data, numleft, infile);

    read_ok = bm_tga_read_outrage_compressed16(infile, n, num_mips, image_type);
  }

  else
    Int3(); // Get Jason

  if (image_type == OUTRAGE_COMPRESSED_OGF_8BIT)
    Int3();
  // bm_tga_read_outrage_compressed8 (infile,n);

  if (Tga_file_data != NULL) {
    mem_free(Tga_file_data);
    Tga_file_data = NULL;
    cfseek(infile, savepos + Fake_pos, SEEK_SET);
  }

  if (!read_ok)
    return -1;
  else
    return (n);
}

// Pages in bitmap index n.  Returns 1 if successful, 0 if not
int bm_page_in_file(int n) {
  ubyte image_id_len, color_map_type, image_type, pixsize, descriptor;
  ubyte upside_down = 0;
  ushort width, height;
  int i, data8bit = 0, savepos;
  int mipped = 0, file_mipped = 0;
  int num_mips = 1;
  char name[BITMAP_NAME_LEN];
  CFILE *infile;

  ASSERT((GameBitmaps[n].flags & BF_NOT_RESIDENT));

  infile = (CFILE *)cfopen(GameBitmaps[n].name, "rb");
  if (!infile) {
    mprintf((0, "Couldn't page in bitmap %s!\n", GameBitmaps[n].name));
    return 0;
  }
  // Used for progress bar when loading the level
  paged_in_count += cfilelength(infile);
  paged_in_num++;
  image_id_len = cf_ReadByte(infile);
  color_map_type = cf_ReadByte(infile);
  image_type = cf_ReadByte(infile);

  if (color_map_type != 0 ||
      (image_type != 10 && image_type != 2 && image_type != OUTRAGE_TGA_TYPE && image_type != OUTRAGE_COMPRESSED_OGF &&
       image_type != OUTRAGE_COMPRESSED_MIPPED && image_type != OUTRAGE_NEW_COMPRESSED_MIPPED &&
       image_type != OUTRAGE_1555_COMPRESSED_MIPPED && image_type != OUTRAGE_4444_COMPRESSED_MIPPED)) {
    mprintf((0, "bm_tga: Can't read this type of TGA.\n"));
    return -1;
  }

  if (image_type == OUTRAGE_COMPRESSED_OGF_8BIT)
    data8bit = 1;

  if (image_type == OUTRAGE_4444_COMPRESSED_MIPPED || image_type == OUTRAGE_1555_COMPRESSED_MIPPED ||
      image_type == OUTRAGE_NEW_COMPRESSED_MIPPED || image_type == OUTRAGE_TGA_TYPE ||
      image_type == OUTRAGE_COMPRESSED_MIPPED || image_type == OUTRAGE_COMPRESSED_OGF ||
      image_type == OUTRAGE_COMPRESSED_OGF_8BIT) {
    if (image_type == OUTRAGE_4444_COMPRESSED_MIPPED || image_type == OUTRAGE_1555_COMPRESSED_MIPPED ||
        image_type == OUTRAGE_NEW_COMPRESSED_MIPPED) {
      cf_ReadString(name, BITMAP_NAME_LEN - 1, infile);
    } else {
      for (i = 0; i < BITMAP_NAME_LEN; i++)
        name[i] = cf_ReadByte(infile);
    }
    if (image_type == OUTRAGE_4444_COMPRESSED_MIPPED || image_type == OUTRAGE_1555_COMPRESSED_MIPPED ||
        image_type == OUTRAGE_COMPRESSED_MIPPED || image_type == OUTRAGE_NEW_COMPRESSED_MIPPED)
      num_mips = cf_ReadByte(infile);
    else
      num_mips = 1;

    if (num_mips > 1)
      file_mipped = 1;
  }

  for (i = 0; i < 9; i++) // ingore next 9 bytes
    cf_ReadByte(infile);

  width = cf_ReadShort(infile);
  height = cf_ReadShort(infile);
  pixsize = cf_ReadByte(infile);

  if (pixsize != 32 && pixsize != 24) {
    mprintf((0, "bm_tga: This file has a pixsize of field of %d, it should be 32. ", pixsize));
    return 0;
  }

  descriptor = cf_ReadByte(infile);
  if (((descriptor & 0x0F) != 8) && ((descriptor & 0x0F) != 0)) {
    mprintf((0, "bm_tga: Descriptor field & 0x0F must be 8 or 0, but this is %d.", descriptor & 0x0F));
    return 0;
  }

  for (i = 0; i < image_id_len; i++)
    cf_ReadByte(infile);

  if ((GameBitmaps[n].flags & BF_WANTS_MIP) || file_mipped)
    mipped = 1;

  int size = (width * height * 2) + (mipped * ((width * height * 2) / 3)) + 2;
  GameBitmaps[n].data16 = (ushort *)mem_malloc(size);
  if (!GameBitmaps[n].data16) {
    mprintf((0, "Out of memory in bm_page_in_file!\n"));
    return 0;
  }

  Bitmap_memory_used += size;

  if ((GameBitmaps[n].flags & BF_WANTS_4444) || image_type == OUTRAGE_4444_COMPRESSED_MIPPED)
    GameBitmaps[n].format = BITMAP_FORMAT_4444;
  else
    GameBitmaps[n].format = BITMAP_FORMAT_STANDARD;

  GameBitmaps[n].width = width;
  GameBitmaps[n].height = height;
  GameBitmaps[n].flags &= ~BF_NOT_RESIDENT;

  // Copy the name
  //	if ((stricmp(GameBitmaps[n].name,name)))
  //			Int3(); //Get Jason!

  strcpy(GameBitmaps[n].name, name);

  mprintf((0, "Paging in bitmap %s!\n", GameBitmaps[n].name));

  if (file_mipped)
    GameBitmaps[n].flags |= BF_MIPMAPPED;

  upside_down = (descriptor & 0x20) >> 5;
  upside_down = 1 - upside_down;

  // Load the actual bitmap data in, converting it from 32 bit to 16 bit, and replacing
  // that pesky transparency color without our replacement
  if (image_type == OUTRAGE_4444_COMPRESSED_MIPPED || image_type == OUTRAGE_1555_COMPRESSED_MIPPED ||
      image_type == OUTRAGE_NEW_COMPRESSED_MIPPED || image_type == OUTRAGE_COMPRESSED_MIPPED ||
      image_type == OUTRAGE_COMPRESSED_OGF || image_type == OUTRAGE_COMPRESSED_OGF_8BIT) // COMPRESSED OGF
  {
    // read this ogf in all at once (much faster)

    savepos = cftell(infile);
    cfseek(infile, 0, SEEK_END);
    int lastpos = cftell(infile);
    int numleft = lastpos - savepos;

    cfseek(infile, savepos, SEEK_SET);

    Tga_file_data = (char *)mem_malloc(numleft);
    ASSERT(Tga_file_data != NULL);
    Fake_pos = 0;
    Bad_tga = 0;
    Fake_file_size = numleft;

    cf_ReadBytes((ubyte *)Tga_file_data, numleft, infile);

    bm_tga_read_outrage_compressed16(infile, n, num_mips, image_type);
  }

  else
    Int3(); // Get Jason

  if (image_type == OUTRAGE_COMPRESSED_OGF_8BIT)
    Int3();
  // bm_tga_read_outrage_compressed8 (infile,n);

  if (Tga_file_data != NULL) {
    mem_free(Tga_file_data);
    Tga_file_data = NULL;
    cfseek(infile, savepos + Fake_pos, SEEK_SET);
  }

  cfclose(infile);

  if ((GameBitmaps[n].flags & BF_WANTS_MIP) && !file_mipped) {
    bm_GenerateMipMaps(n);
  }

  return 1;
}
