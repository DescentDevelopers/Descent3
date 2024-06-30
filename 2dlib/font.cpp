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

 * $Logfile: /DescentIII/Main/2dlib/font.cpp $
 * $Revision: 15 $
 * $Date: 4/17/99 6:15p $
 * $Author: Samir $
 *
 * will load and manage a font's capabilities.
 *
 * $Log: /DescentIII/Main/2dlib/font.cpp $
 *
 * 15    4/17/99 6:15p Samir
 * replaced gr.h with grdefs.h and fixed resulting compile bugs.
 *
 * 14    10/13/98 10:31a Sean
 * Fixed a mem_free (free()) conflict
 *
 * 13    4/23/98 6:38p Jason
 * made bitmaps use 1555 format
 *
 * 12    3/31/98 3:48p Jason
 * added memory lib
 *
 * 11    12/19/97 12:31p Samir
 * load font registration
 *
 * 10    12/12/97 6:40p Samir
 * Some viewport font functions.
 *
 * 9     11/16/97 6:53p Samir
 * Reinstated instance count for fonts.
 *
 * 8     11/16/97 4:41p Matt
 * Commented-out an assert.
 *
 * 7     11/14/97 12:06p Samir
 * When you use a font for the first time, load it.  Never free font until
 * game closes.
 *
 * 6     10/15/97 5:20p Jason
 * did a HUGE overhaul of the bitmap system
 *
 * 5     9/26/97 2:53p Samir
 * Delete old m_CharSurf if creating a new one.
 *
 * 4     9/19/97 5:37p Samir
 * Software font coloring works.
 *
 * 3     8/07/97 3:15p Samir
 * May have fixed horizontal left clipping problem.
 *
 * 22    6/25/97 10:10a Samir
 * Removed unused code causing warning.
 *
 * 21    6/24/97 7:42p Matt
 * Included bitmap.h instead of font.h
 *
 * 20    6/16/97 4:45p Samir
 * Fixed transparency probs
 *
 * 19    6/16/97 3:01p Samir
 * Changed font system to work better with renderer.  Fonts are rendered
 * on bitmaps of 128x128 at load time.
 *
 * 18    6/12/97 6:27p Samir
 * DDGR v2.0 (GR v1.1) Changes to ddgr system reflected in 2d system
 *
 * 17    5/14/97 8:07p Samir
 * some pathname problem which screwed up Mac.
 *
 * 16    3/07/97 4:01p Samir
 * Took out malloc.h for ANSI compliance.
 *
 * 15    3/03/97 6:20p Matt
 * Changed cfile functions to use D3 naming convention
 *
 * 14    2/28/97 3:17 PM Jeremy
 * changed a call of strcmpi to stricmp
 *
 * 13    2/28/97 2:57p Samir
 * Took out __cdecl from atexit
 *
 * 12    2/10/97 12:14p Samir
 * Allow multiple font loading per viewport.
 *
 * 11    2/06/97 6:33p Samir
 * Added clipping character functions for font.
 *
 * 10    2/06/97 12:43p Samir
 * Mono fonts work.
 *
 * 9     2/05/97 12:19p Samir
 * Stopped redundant increases in font reference count for same font
 * object.
 *
 * 8     2/05/97 10:35a Samir
 * Fixed text drawing at wrong location problem
 *
 * $NoKeywords: $
 */

#include "pserror.h"
#include "renderer.h"
#include "gr.h"
#include "mono.h"
#include "cfile.h"
#include "bitmap.h"
#include "mem.h"

#include <string.h>
#include <stdlib.h>

#define FT_COLOR 1
#define FT_PROPORTIONAL 2
#define FT_KERNED 4
#define FT_GRADIENT 8

#define GRFONT_SURFACE_WIDTH 128
#define GRFONT_SURFACE_HEIGHT 128

//	----------------------------------------------------------------------------
//	Macros for file io.
//	----------------------------------------------------------------------------

typedef CFILE *FONTFILE;

static inline int READ_FONT_INT(FONTFILE ffile);
static inline int16_t READ_FONT_SHORT(FONTFILE ffile);
static inline uint8_t READ_FONT_BYTE(FONTFILE ffile);
static inline int READ_FONT_DATA(FONTFILE ffile, void *buf, int size, int nelem);
static inline FONTFILE OPEN_FONT(char *filename, bool &success);
static inline void CLOSE_FONT(FONTFILE ffile);

#define BITS_TO_BYTES(_c) (((_c) + 7) >> 3)
#define BITS_TO_SHORTS(_c) (((_c) + 15) >> 4)

inline int READ_FONT_INT(FONTFILE ffile) { return cf_ReadInt(ffile); }

inline int16_t READ_FONT_SHORT(FONTFILE ffile) { return cf_ReadShort(ffile); }

inline uint8_t READ_FONT_BYTE(FONTFILE ffile) { return (uint8_t)cf_ReadByte(ffile); }

inline int READ_FONT_DATA(FONTFILE ffile, void *buf, int size, int nelem) {
  int i;

  i = cf_ReadBytes((uint8_t *)buf, size * nelem, ffile);

  ASSERT(i == (size * nelem));

  return i;
}

inline FONTFILE OPEN_FONT(char *filename, bool &success) {
  FONTFILE fp;
  int file_id;

  success = false;
  fp = (FONTFILE)cfopen(filename, "rb");
  if (!fp)
    return NULL;

  file_id = READ_FONT_INT(fp);
  if (file_id != 0xfeedbaba)
    return (FONTFILE)(-1);

  success = true;
  return fp;
}

inline void CLOSE_FONT(FONTFILE ffile) { cfclose(ffile); }

//	----------------------------------------------------------------------------
//	static variables and functions
//	----------------------------------------------------------------------------

gr_font_record grFont::m_FontList[MAX_FONTS];
grMemorySurface *grFont::m_CharSurf = NULL;

void grFont::init_system() {
  for (int i = 0; i < MAX_FONTS; i++) {
    grFont::m_FontList[i].references = 0;
    grFont::m_FontList[i].filename[0] = 0;
  }

  if (grFont::m_CharSurf) {
    delete grFont::m_CharSurf;
    grFont::m_CharSurf = NULL;
  }

  if (!grFont::m_CharSurf) {
    grFont::m_CharSurf = new grMemorySurface;
    grFont::m_CharSurf->init(0, 0, BPP_16, NULL, 0, 0, "char_surf");
  }

  atexit(grFont::close_system);
}

void grFont::close_system() {
  if (grFont::m_CharSurf) {
    delete grFont::m_CharSurf;
    grFont::m_CharSurf = NULL;
  }

  // we should free any fonts currently allocated here.
  mprintf(0, "Freeing cached fonts...");
  for (int i = 0; i < MAX_FONTS; i++) {
    gr_font_record *ft;
    if (grFont::m_FontList[i].references) {
      ft = &grFont::m_FontList[i];
      grFont::free_font(ft);
      grFont::m_FontList[i].references = 0;
    }
  }
}

int grFont::register_font(char *fontname, char *filename) {
  /* look for a slot with no fontname and place fontname in it */
  ASSERT(fontname != NULL);

  int i;

  for (i = 0; i < MAX_FONTS; i++) {
    if (grFont::m_FontList[i].filename[0] == 0) {
      strcpy(grFont::m_FontList[i].filename, filename);
      strcpy(grFont::m_FontList[i].name, fontname);
      grFont::load(m_FontList[i].filename, i);
      grFont::m_FontList[i].references = 1;
      break;
    }
  }

  if (i == MAX_FONTS)
    i = -1;

  return i;
}

//	----------------------------------------------------------------------------
//	constructor and destructor of font object
//	----------------------------------------------------------------------------

grFont::grFont() { m_FontHandle = -1; }

grFont::~grFont() {
  if (m_FontHandle > -1)
    grFont::free();
}

//	----------------------------------------------------------------------------
//	high level font initialization and cleanup routines
//	----------------------------------------------------------------------------

void grFont::init(const char *fontname) {
  int i, slot = -1;

  //	Look for this font in the list, check if it has any references, and load it if
  //	there are none.
  if (fontname == NULL) {
    slot = 0;
  } else {
    for (i = 0; i < MAX_FONTS; i++) {
      if (stricmp(grFont::m_FontList[i].name, fontname) == 0) {
        slot = i;

        if (grFont::m_FontList[i].references == 0) {
          break;
        } else {
          m_FontHandle = slot;
          return;
        }
      }
    }

    ASSERT(slot != -1);

    //	clear up surface and bitmap lists
    grFont::m_FontList[slot].ch_w = NULL;
    grFont::m_FontList[slot].ch_h = NULL;
    grFont::m_FontList[slot].ch_u = NULL;
    grFont::m_FontList[slot].ch_v = NULL;

    for (i = 0; i < MAX_FONT_BITMAPS; i++) {
      grFont::m_FontList[slot].bmps[i] = -1;
      grFont::m_FontList[slot].surfs[i] = NULL;
    }

    //	We must load this font from the 'hogfile'
    grFont::load(m_FontList[slot].filename, slot);

    grFont::m_FontList[slot].references++;
  }

  m_FontHandle = slot;
}

void grFont::free() {
  if (m_FontHandle == -1)
    return;

  return;

  //!! Removed the following assert because there are bugs in the reference count system. Matt, 11-15-97.
  //!!	ASSERT(grFont::m_FontList[m_FontHandle].references > 0);
  //@@
  //	free font if there was only one reference left.
  //@@	if (grFont::m_FontList[m_FontHandle].references == 1) {
  //@@		gr_font_record *ft;
  //@@
  //@@	// free up all memory allocated to this font.
  //@@		ft = &grFont::m_FontList[m_FontHandle];
  //@@
  //@@ 		grFont::free_font(ft);
  //@@	}
  //@@
  //@@	grFont::m_FontList[m_FontHandle].references--;
  m_FontHandle = -1;
}

void grFont::free_font(gr_font_record *ft) {
  int i;

  //	delete font surface info.
  if (ft->ch_wf)
    delete[] ft->ch_wf;
  if (ft->ch_hf)
    delete[] ft->ch_hf;
  if (ft->ch_uf)
    delete[] ft->ch_uf;
  if (ft->ch_vf)
    delete[] ft->ch_vf;
  if (ft->ch_w)
    delete[] ft->ch_w;
  if (ft->ch_h)
    delete[] ft->ch_h;
  if (ft->ch_u)
    delete[] ft->ch_u;
  if (ft->ch_v)
    delete[] ft->ch_v;
  if (ft->ch_surf)
    delete[] ft->ch_surf;

  for (i = 0; i < MAX_FONT_BITMAPS; i++) {
    if (ft->bmps[i] != -1)
      bm_FreeBitmap(ft->bmps[i]);
    if (ft->surfs[i])
      delete ft->surfs[i];
  }

  //	delete font file info.
  if (ft->font.flags & FT_PROPORTIONAL) {
    delete[] ft->font.char_widths;
  }

  ::mem_free(ft->font.raw_data);
  ::mem_free(ft->font.char_data);
}

void grFont::load(char *filename, int slot) {
  FONTFILE ff;
  gr_font_file_record fnt, *ft;
  char fontname[32];
  int num_char;
  int i;

  ft = &fnt;

  bool success;
  ff = OPEN_FONT(filename, success);
  if (!ff) {
    Error("Unable to open font in file %s.\n", filename);
  } else if (!success) {
    Error("Illegal font file: %s.\n", filename);
  }

  mprintf(0, "%s font.\n", grFont::m_FontList[slot].name);

  ft->width = READ_FONT_SHORT(ff);
  ft->height = READ_FONT_SHORT(ff);
  ft->flags = READ_FONT_SHORT(ff);
  ft->baseline = READ_FONT_SHORT(ff);
  ft->min_ascii = READ_FONT_BYTE(ff);
  ft->max_ascii = READ_FONT_BYTE(ff);
  READ_FONT_DATA(ff, fontname, 32, 1);

  mprintf(0, "  <ht %d>::<min %d>::<max %d>::<base %d>", ft->height, ft->min_ascii, ft->max_ascii, ft->baseline);

  num_char = ft->max_ascii - ft->min_ascii + 1;

  //	Read in all widths
  if (ft->flags & FT_PROPORTIONAL) {
    ft->char_widths = new int16_t[num_char];
    for (i = 0; i < num_char; i++)
      ft->char_widths[i] = READ_FONT_SHORT(ff);
    mprintf(0, "::proportional");
  } else {
    ft->char_widths = NULL;
  }

  //	Read in pixel data.
  //	for color fonts, read in byte count and then the data,
  //		generate character data pointer table
  //	for mono fonts, read in byte count, then the data, convert to bits and store
  //		generate character data pointer table
  int bytesize = READ_FONT_INT(ff);

  ft->raw_data = (uint8_t *)mem_malloc(bytesize);
  ft->char_data = (uint8_t **)mem_malloc(num_char * sizeof(uint8_t *));

  READ_FONT_DATA(ff, ft->raw_data, bytesize, 1);

  if (ft->flags & FT_COLOR) {
    int off = 0;
    mprintf(0, "::color");
    for (i = 0; i < num_char; i++) {
      ft->char_data[i] = ft->raw_data + off;
      if (ft->flags & FT_PROPORTIONAL)
        off += (ft->char_widths[i] * ft->height * BITS_TO_BYTES(BPP_16));
      else
        off += (ft->width * ft->height * BITS_TO_BYTES(BPP_16));
    }
  } else { // Monochrome
    uint8_t *ptr = ft->raw_data;
    mprintf(0, "::mono");
    for (i = 0; i < num_char; i++) {
      ft->char_data[i] = ptr;
      if (ft->flags & FT_PROPORTIONAL)
        ptr += BITS_TO_BYTES(ft->char_widths[i]) * ft->height;
      else
        ptr += BITS_TO_BYTES(ft->width) * ft->height;
    }
  }

  //	Read in kerning data
  ft->kern_data = NULL;

  //	Then read in
  CLOSE_FONT(ff);

  mprintf(0, "\n");

  grFont::m_FontList[slot].font = fnt;

  //	draw font to bitmaps, load into surfaces too.
  grFont::translate_to_surfaces(slot);
}

//	translates a font to surfaces
void grFont::translate_to_surfaces(int slot) {
  gr_font_file_record *fntfile;
  gr_font_record *fnt;

  fnt = &grFont::m_FontList[slot];
  fntfile = &grFont::m_FontList[slot].font;

  //	start creating font surfaces, map these surfaces onto bitmaps created via bitmap library
  //	this is needed for the renderer library.
  //	create a 128x128 surface first.
  //	draw each character into surface until we need to create another
  //	surface.
  uint8_t u = 0, v = 0, w;
  int ch, num_ch;
  uint8_t surf_index = 0;

  num_ch = fntfile->max_ascii - fntfile->min_ascii + 1;

  //	initialize memory
  fnt->ch_w = new uint8_t[num_ch];
  fnt->ch_h = new uint8_t[num_ch];
  fnt->ch_u = new uint8_t[num_ch];
  fnt->ch_v = new uint8_t[num_ch];
  fnt->ch_surf = new uint8_t[num_ch];
  fnt->ch_uf = new float[num_ch];
  fnt->ch_vf = new float[num_ch];
  fnt->ch_wf = new float[num_ch];
  fnt->ch_hf = new float[num_ch];

  fnt->bmps[surf_index] = bm_AllocBitmap(GRFONT_SURFACE_WIDTH, GRFONT_SURFACE_HEIGHT, 0);
  if (fnt->bmps[surf_index] == -1)
    Error("grFont::translate_to_surfaces <Bitmap allocation error>");
  fnt->surfs[surf_index] = new grMemorySurface;
  fnt->surfs[surf_index]->init(GRFONT_SURFACE_WIDTH, GRFONT_SURFACE_HEIGHT, BPP_16,
                               (char *)bm_data(fnt->bmps[surf_index], 0), GRFONT_SURFACE_WIDTH * BITS_TO_BYTES(BPP_16),
                               SURFFLAG_COLORKEY);
  fnt->surfs[surf_index]->clear(TRANSPARENT_COLOR32);
  surf_index++;

  for (ch = 0; ch < num_ch; ch++) {
    if (fntfile->flags & FT_PROPORTIONAL)
      w = (int)fntfile->char_widths[ch];
    else
      w = (int)fntfile->width;

    // blt each character
    if (fntfile->flags & FT_COLOR) {
      m_CharSurf->init(w, fntfile->height, BPP_16, (char *)fntfile->char_data[ch], w * BITS_TO_BYTES(BPP_16),
                       SURFFLAG_COLORKEY);

      fnt->surfs[surf_index - 1]->blt(u, v, m_CharSurf);
    } else { //	font monochrome, convert bits to pixels
      grFont::translate_mono_char(fnt->surfs[surf_index - 1], u, v, ch, fntfile, w);
    }

    fnt->ch_h[ch] = fntfile->height;
    fnt->ch_w[ch] = w;
    fnt->ch_u[ch] = u;
    fnt->ch_v[ch] = v;
    fnt->ch_surf[ch] = surf_index - 1;
    fnt->ch_hf[ch] = ((float)fntfile->height) / ((float)GRFONT_SURFACE_HEIGHT);
    fnt->ch_wf[ch] = ((float)w) / ((float)GRFONT_SURFACE_WIDTH);
    fnt->ch_uf[ch] = ((float)u) / ((float)GRFONT_SURFACE_WIDTH);
    fnt->ch_vf[ch] = ((float)v) / ((float)GRFONT_SURFACE_HEIGHT);

    //	check to adjust uv's if we are outside surface.
    u += w;
    if ((u + w) > GRFONT_SURFACE_WIDTH) {
      u = 0;
      v += fntfile->height;
      if ((v + fntfile->height) > GRFONT_SURFACE_HEIGHT) {
        if (surf_index == MAX_FONT_BITMAPS)
          Int3();
        fnt->bmps[surf_index] = bm_AllocBitmap(GRFONT_SURFACE_WIDTH, GRFONT_SURFACE_HEIGHT, 0);
        if (fnt->bmps[surf_index] == -1)
          Error("grFont::translate_to_surfaces <Bitmap allocation error>");
        fnt->surfs[surf_index] = new grMemorySurface;
        fnt->surfs[surf_index]->init(GRFONT_SURFACE_WIDTH, GRFONT_SURFACE_HEIGHT, BPP_16,
                                     (char *)bm_data(fnt->bmps[surf_index], 0),
                                     GRFONT_SURFACE_WIDTH * BITS_TO_BYTES(BPP_16), SURFFLAG_COLORKEY);
        fnt->surfs[surf_index]->clear(TRANSPARENT_COLOR32);
        surf_index++;
        v = 0;
      }
    }
  }
}

//	translate mono font data to the surface
void grFont::translate_mono_char(grSurface *sf, int x, int y, int index, gr_font_file_record *ft, int width) {
  int row, col; // byte width of char
  int rowsize;
  uint8_t bit_mask = 0, byte;
  uint8_t *fp;

  fp = ft->char_data[index];

  switch (sf->bpp()) {
  case BPP_16: {
    /*	draw one-bit one color. */
    uint16_t *dest_ptr;
    uint16_t col_w = GR_COLOR_TO_16(GR_RGB(255, 255, 255));
    int rowsize_w;

    dest_ptr = (uint16_t *)sf->lock(&rowsize);
    rowsize_w = sf->rowsize() / 2;
    dest_ptr += (y * rowsize_w) + x;

    for (row = 0; row < ft->height; row++) {
      bit_mask = 0;
      for (col = 0; col < width; col++) {
        if (bit_mask == 0) {
          byte = *fp++;
          bit_mask = 0x80;
        }

        if (byte & bit_mask)
          dest_ptr[col] = col_w;
        bit_mask >>= 1;
      }
      dest_ptr += rowsize_w;
    }

    sf->unlock();
    break;
  }

  case BPP_32:
  default:
    Int3(); // NOT SUPPORTED YET
  }
}

//	----------------------------------------------------------------------------
//	draw_char function
//	give a viewport x,y and char
//	----------------------------------------------------------------------------

int grFont::draw_char(grSurface *sf, int x, int y, int ch, tCharProperties *chprop) {
  gr_font_record *ft;
  int next_x = 0, width, index;

  ASSERT(m_FontHandle > -1);
  if ((ch < min_ascii()) || (ch > max_ascii()))
    return (next_x + 1);

  //	save current x and get this font
  ft = &grFont::m_FontList[m_FontHandle];
  next_x = x;
  index = ch - ft->font.min_ascii;

  //	draw either a color bitmap or mono font.
  if (ft->font.flags & FT_PROPORTIONAL) {
    width = (int)ft->font.char_widths[index];
  } else {
    width = (int)ft->font.width;
  }

  //	perform blt.
  if (sf->get_flags() & SURFFLAG_RENDERER) {
    rend_SetCharacterParameters(chprop->col[0], chprop->col[1], chprop->col[2], chprop->col[3]);
    rend_DrawFontCharacter(ft->bmps[ft->ch_surf[index]], x, y, x + ft->ch_w[index] - 1, y + ft->ch_h[index] - 1,
                           ft->ch_uf[index], ft->ch_vf[index], ft->ch_wf[index], ft->ch_hf[index]);
  } else if (ft->font.flags & FT_COLOR) {
    sf->blt(x, y, ft->surfs[ft->ch_surf[index]], ft->ch_u[index], ft->ch_v[index], ft->ch_w[index], ft->ch_h[index]);
  } else {
    charblt16(sf, chprop->col[0], x, y, ft->surfs[ft->ch_surf[index]], ft->ch_u[index], ft->ch_v[index],
              ft->ch_w[index], ft->ch_h[index]);
  }

  //	adjust next x value with kerning and return it.
  next_x += width;

  return next_x;
}

int grFont::draw_char(grSurface *sf, int x, int y, int ch, int sx, int sy, int sw, int sh, tCharProperties *chprop) {
  gr_font_record *ft;
  int next_x = 0, width, index;

  ASSERT(m_FontHandle > -1);
  if ((ch < min_ascii()) || (ch > max_ascii()))
    return (next_x + 1);

  //	save current x and get this font
  ft = &grFont::m_FontList[m_FontHandle];
  next_x = x;
  index = ch - ft->font.min_ascii;

  //	draw either a color bitmap or mono font.
  if (ft->font.flags & FT_PROPORTIONAL) {
    width = (int)ft->font.char_widths[index];
  } else {
    width = (int)ft->font.width;
  }

  ASSERT(sx + sw <= width);
  ASSERT(sy + sh <= ft->font.height);

  //	peform blt instead.
  //	perform blt.
  if (sf->get_flags() & SURFFLAG_RENDERER) {
    rend_SetCharacterParameters(chprop->col[0], chprop->col[1], chprop->col[2], chprop->col[3]);

    rend_DrawFontCharacter(ft->bmps[ft->ch_surf[index]], x, y, x + sw - 1, y + sh - 1,
                           ft->ch_uf[index] + (((float)sx) / ((float)GRFONT_SURFACE_WIDTH)),
                           ft->ch_vf[index] + (((float)sy) / ((float)GRFONT_SURFACE_HEIGHT)),
                           ((float)sw) / ((float)GRFONT_SURFACE_WIDTH), ((float)sh) / ((float)GRFONT_SURFACE_HEIGHT));
  } else if (ft->font.flags & FT_COLOR) {
    sf->blt(x, y, ft->surfs[ft->ch_surf[index]], ft->ch_u[index] + sx, ft->ch_v[index] + sy, sw, sh);
  } else {
    charblt16(sf, chprop->col[0], x, y, ft->surfs[ft->ch_surf[index]], ft->ch_u[index] + sx, ft->ch_v[index] + sy, sw,
              sh);
  }

  //	adjust next x value with kerning and return it.
  next_x += sw;

  return next_x;
}

//	----------------------------------------------------------------------------
//	accessor functions
//	----------------------------------------------------------------------------

int grFont::get_char_info(int ch, int *width) {
  gr_font_file_record *ft;

  ASSERT(m_FontHandle > -1);

  ft = &grFont::m_FontList[m_FontHandle].font;

  if (ch < ft->min_ascii || ch > ft->max_ascii)
    *width = 0;
  else if (ft->flags & FT_PROPORTIONAL)
    *width = ft->char_widths[ch - ft->min_ascii];
  else
    *width = ft->width;

  return 0;
}

uint8_t *grFont::get_kern_info(uint8_t c1, uint8_t c2) {
  //	gr_font_file_record *ft;
  //	uint8_t *p;

  //	p = ft->kern_data;
  //	ft = &grFont::m_FontList[m_FontHandle].font;
  //	c2 = c2 - ft->min_ascii;
  //	c1 = c1 - ft->min_ascii;

  //	while (*p!=255)
  //	{
  //		if (p[0]==c1 && p[1]==c2) return p;
  //		else p+=3;
  //	}

  return NULL;
}

void grFont::charblt16(grSurface *dsf, ddgr_color col, int dx, int dy, grSurface *ssf, int sx, int sy, int sw, int sh) {
  uint16_t *dbits;
  uint16_t *sbits;
  int srowsize_w, drowsize_w, row, coln;
  uint16_t scol = GR_COLOR_TO_16(col);

  dbits = (uint16_t *)dsf->lock(&drowsize_w);
  if (dbits) {
    sbits = (uint16_t *)ssf->lock(&srowsize_w);
    if (sbits) {
      srowsize_w >>= 1; // rowsize in shorts
      drowsize_w >>= 1;
      dbits = dbits + (dy * drowsize_w) + dx;
      sbits = sbits + (sy * srowsize_w) + sx;

      for (row = 0; row < sh; row++) {
        for (coln = 0; coln < sw; coln++) {
          if (sbits[coln] & OPAQUE_FLAG16)
            dbits[coln] = scol;
        }
        sbits += srowsize_w;
        dbits += drowsize_w;
      }

      ssf->unlock();
    }
    dsf->unlock();
  }
}
