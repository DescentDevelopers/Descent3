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

 * $Logfile: /DescentIII/Main/2dlib/surface.cpp $
 * $Revision: 7 $
 * $Date: 11/30/98 5:50p $
 * $Author: Jason $
 *
 * Master Surface class
 *
 * $Log: /DescentIII/Main/2dlib/surface.cpp $
 *
 * 7     11/30/98 5:50p Jason
 * added 4444 bitmap support
 *
 * 6     4/23/98 6:38p Jason
 * made bitmaps use 1555 format
 *
 * 5     12/19/97 2:26p Jason
 * more fixes for 2d/3d intergration
 *
 * 4     12/19/97 12:32p Samir
 * lock now calls renderer functions.
 *
 * 3     11/11/97 1:24p Samir
 * Fixed problem with clearing using the renderer.
 *
 * 2     10/15/97 5:20p Jason
 * did a HUGE overhaul of the bitmap system
 *
 * 24    6/12/97 6:29p Samir
 * DDGR v2.0 Changes in 2d system implemented.
 *
 * 23    6/06/97 12:18p Samir
 * OpenGL HACKS to call renderer for some drawing functions.  Must fix.
 *
 * 22    5/19/97 5:10p Jason
 * changes for our new abstracted renderer code
 *
 * 21    4/30/97 3:15p Jason
 * changes to support both 8bit and 16bit rendering in software
 *
 * 20    3/28/97 3:07p Samir
 * Clear function now takes a color
 *
 * 19    3/27/97 11:11a Samir
 * Moved code from ddgr library to 2dlib because it was ANSI compliant and
 * should work on all systems
 *
 * 18    2/28/97 2:51p Samir
 * took out __cdecl in atexit function.
 *
 * 17    2/17/97 2:19p Jason
 * made blit function scale instead of clip
 *
 * 16    2/11/97 3:19p Jason
 * added displaying of 8bit palettized graphics on 16bit surfaces
 *
 * 15    2/04/97 12:49p Samir
 * Added lock(x,y) for those special cases.
 *
 * 14    2/03/97 3:01p Samir
 * Used one memory surface global to grSurface for scratchpad blitting.
 *
 * 13    1/30/97 6:01p Samir
 * The partition of memory and os surfaces as well as the change in
 * ddgr_surface structure and all related changes.
 *
 * $NoKeywords: $
 */

#include <algorithm>
#include <cstdlib>
#include <cstring>

#include "bitmap.h"
#include "lib2d.h"
#include "pserror.h"
#include "renderer.h"
#include "texture.h"

static inline unsigned XLAT_RGB_TO_16(ddgr_color c) {
  uint8_t r, g, b;
  r = (uint8_t)((c & 0x00ff0000) >> 16);
  g = (uint8_t)((c & 0x0000ff00) >> 8);
  b = (uint8_t)(c & 0x000000ff);

  return (((r >> 3) << 10) + ((g >> 3) << 5) + (b >> 3));
}

ddgr_surface_node *grSurface::surf_list = nullptr;     // the surface list
ddgr_surface_node *grSurface::surf_list_cur = nullptr; // current node on list.
ddgr_surface *grSurface::scratch_mem_surf = nullptr;

void grSurface::init_system() {
  //	allocate our scratch memory surface used in uniblt
  if (!grSurface::scratch_mem_surf) {
    grSurface::scratch_mem_surf = new ddgr_surface;
    memset(grSurface::scratch_mem_surf, 0, sizeof(ddgr_surface));
  }

  atexit(grSurface::close_system);
}

void grSurface::close_system() {
  //	cleanup our scratch memory surface for blt
  if (grSurface::scratch_mem_surf) {
    if (grSurface::scratch_mem_surf->obj != nullptr)
      gr_mem_surf_Destroy(grSurface::scratch_mem_surf);

    delete grSurface::scratch_mem_surf;
    grSurface::scratch_mem_surf = nullptr;
  }
}

//	---------------------------------------------------------------------------
//	grSurface constructor and destructor
//	---------------------------------------------------------------------------

grSurface::grSurface() {
  surf_Locked = 0;
  m_SurfInit = 0;
  memset(&ddsfObj, 0, sizeof(ddgr_surface));
}

grSurface::grSurface(int w, int h, int bpp, unsigned type, unsigned flags, const char *name) {
  surf_Locked = 0;
  m_SurfInit = 0;

  grSurface::create(w, h, bpp, type, flags, name);
}

grSurface::~grSurface() { grSurface::free(); }

void grSurface::create(int w, int h, int bpp, unsigned type, unsigned flags, const char *name) {
  bool grerr;

  if (m_SurfInit)
    grSurface::free();

  //	create new surface: set up surface structure
  memset(&ddsfObj, 0, sizeof(ddgr_surface));
  ddsfObj.w = w;
  ddsfObj.h = h;
  ddsfObj.bpp = bpp;
  ddsfObj.type = (uint16_t)type;
  ddsfObj.flags = (uint16_t)flags;

  if (name)
    strncpy(ddsfObj.name, name, 15);

  switch (type) {
  case SURFTYPE_VIDEOSCREEN:
    grerr = ddgr_surf_InitVideo(&ddsfObj);
    break;
  case SURFTYPE_GENERIC:
    grerr = ddgr_surf_Create(&ddsfObj);
    break;
  case SURFTYPE_MEMORY:
    grerr = (bool)gr_mem_surf_Create(&ddsfObj);
    break;
  default:
    Int3(); // Invalid!!
  }
  if (!grerr)
    Int3();

  add_to_list(&ddsfObj);

  m_SurfInit = 1;
}

void grSurface::free() {
  /*	Freeing surface is same regardless of whether it was loaded from a bitmap handle or
          created ourselves
  */
  if (!m_SurfInit)
    return;
  ASSERT(!surf_Locked);

  switch (ddsfObj.type) {
  case SURFTYPE_VIDEOSCREEN:
    ddgr_surf_CloseVideo(&ddsfObj);
    break;
  case SURFTYPE_GENERIC:
    ddgr_surf_Destroy(&ddsfObj);
    break;
  case SURFTYPE_MEMORY:
    gr_mem_surf_Destroy(&ddsfObj);
    break;
  default:
    Int3(); // This really shouldn't happen.  bad type!
  }

  remove_from_list(&ddsfObj);

  m_SurfInit = 0;
}

void grSurface::load(int handle) {
  /*	just load the bitmap from the handle info */
  ASSERT(m_SurfInit);

  grSurface::load((char *)bm_data(handle, 0), bm_w(handle, 0), bm_h(handle, 0), bm_bpp(handle), bm_format(handle));
}

void grSurface::load(char *data, int w, int h, int bpp, int format) {
  //	copy defined bitmap given as arguments to this surface, with correct bpp translation.
  int rowsize;
  ASSERT(m_SurfInit);

  /*	perform lock, then copy data to this surface, then unlock */
  grSurface::lock(&rowsize);

  switch (ddsfObj.bpp) {
  case BPP_16:
    if (bpp == BPP_16)
      xlat16_16(data, w, h, format);
    if (bpp == BPP_24)
      xlat24_16(data, w, h);
    if (bpp == BPP_32)
      xlat32_16(data, w, h);
    break;

  case BPP_24:
    if (bpp == BPP_16)
      xlat16_24(data, w, h);
    break;

  default:
    Int3(); // NOT SUPPORTED YET
  }

  grSurface::unlock();
}

//	loads 8bit palettized data onto this surface.
void grSurface::load(int handle, char *pal) {
  ASSERT(m_SurfInit);

  grSurface::load((char *)bm_data(handle, 0), bm_w(handle, 0), bm_h(handle, 0), pal);
}

//	loads 8bit palettized data onto this surface.
void grSurface::load(char *data, int w, int h, char *pal) {
  int rowsize;
  ASSERT(m_SurfInit);

  /*	perform lock, then copy data to this surface, then unlock */
  grSurface::lock(&rowsize);

  switch (ddsfObj.bpp) {
  case BPP_16:
    // TODO: This is probably software renderer only?
    xlat16_16(data, w, h);
  default:
    Int3(); // NOT SUPPORTED YET
  }

  grSurface::unlock();
}

//	---------------------------------------------------------------------------
//	blt functions:: clear
//	---------------------------------------------------------------------------

void grSurface::clear(ddgr_color col) { grSurface::clear(0, 0, ddsfObj.w, ddsfObj.h, col); }

void grSurface::clear(int l, int t, int w, int h, ddgr_color col) {
  ASSERT(m_SurfInit);
  ASSERT(!surf_Locked);

  switch (ddsfObj.type) {
  case SURFTYPE_VIDEOSCREEN:
  case SURFTYPE_GENERIC:
    //	HACK!!!!!!!  I MUST CHANGE THIS
    //	---------------------------------------------------------------------------
    if (ddsfObj.flags & SURFFLAG_RENDERER) {
      rend_FillRect(col, l, t, l + w, t + h);
      return;
    }
    //	---------------------------------------------------------------------------
    ddgr_surf_Clear(&ddsfObj, col, l, t, w, h);
    break;
  case SURFTYPE_MEMORY:
    gr_mem_surf_Clear(&ddsfObj, col, l, t, w, h);
    break;
  default:
    Int3();
  }
}

//	---------------------------------------------------------------------------
//	blt functions:
//		these functions must support memory->os, os->memory, os->os, mem->mem blts.
//		note that (os) blts and (video) blts use the os library.
//	---------------------------------------------------------------------------

void grSurface::uniblt(int dx, int dy, grSurface *ssf, int sx, int sy, int sw, int sh) {
  int rowsize;

  ASSERT(m_SurfInit);

  //	mode 11b = dd_to_dd, 01b = mem_to_dd, 10b = dd_to_mem, 00b = mem_to_mem
  int mode = 0;
  ddgr_surface *dsfs = &ddsfObj, *ssfs = &ssf->ddsfObj;
  bool err;

  //	setup blitting mode
  if (ssfs->type == SURFTYPE_GENERIC || ssfs->type == SURFTYPE_VIDEOSCREEN)
    mode = mode | 0x1;
  if (dsfs->type == SURFTYPE_GENERIC || dsfs->type == SURFTYPE_VIDEOSCREEN)
    mode = mode | 0x2;

  /*	case 1: os->os
                  just invoke os blitter
  */
  if (mode == 0x3) {
    ASSERT(!surf_Locked);
    err = ddgr_surf_Blt(dsfs, dx, dy, ssfs, sx, sy, sw, sh);
    return;
  }

  /*	case 2: mem->mem
                  just lock both surfaces and do a memory blt.
  */
  if (mode == 0) {
    err = gr_mem_surf_Blt(dsfs, dx, dy, ssfs, sx, sy, sw, sh);
    return;
  }

  /*	case 3: os->mem
                  This requires locking both surfaces and performing a memory blt.
                  This also means tricking the memory blitter into thinking that the OS surface
                  is a memory surface
  */
  if (mode == 0x1) {
    char *data = ssf->lock(&rowsize);

    grSurface::scratch_mem_surf->w = ssfs->w;
    grSurface::scratch_mem_surf->h = ssfs->h;
    grSurface::scratch_mem_surf->bpp = ssfs->bpp;
    grSurface::scratch_mem_surf->type = SURFTYPE_MEMORY;
    grSurface::scratch_mem_surf->flags = ssfs->flags;

    gr_mem_surf_Init(grSurface::scratch_mem_surf, data, rowsize);
    err = gr_mem_surf_Blt(dsfs, dx, dy, grSurface::scratch_mem_surf, sx, sy, sw, sh);

    ssf->unlock();
    return;
  }

  /*	case 4: mem->os
                  This requires locking both surfaces and performing a memory blt.
                  This also means tricking the memory blitter into thinking that the OS surface
                  is a memory surface
  */
  if (mode == 0x2) {
    grSurface::lock(&rowsize);

    grSurface::scratch_mem_surf->w = dsfs->w;
    grSurface::scratch_mem_surf->h = dsfs->h;
    grSurface::scratch_mem_surf->bpp = dsfs->bpp;
    grSurface::scratch_mem_surf->type = SURFTYPE_MEMORY;
    grSurface::scratch_mem_surf->flags = dsfs->flags;

    gr_mem_surf_Init(grSurface::scratch_mem_surf, m_DataPtr, m_DataRowsize);
    err = gr_mem_surf_Blt(grSurface::scratch_mem_surf, dx, dy, ssfs, sx, sy, sw, sh);

    grSurface::unlock();
    return;
  }
}

void grSurface::blt(int dx, int dy, grSurface *ssf) {
  ASSERT(m_SurfInit);
  grSurface::uniblt(dx, dy, ssf, 0, 0, ssf->width(), ssf->height());
}

void grSurface::blt(int dx, int dy, grSurface *ssf, int sx, int sy, int sw, int sh) {
  ASSERT(m_SurfInit);
  grSurface::uniblt(dx, dy, ssf, sx, sy, sw, sh);
}

/*	Helper functions
                replace_color, simply replaces one color in the surface with another
*/
void grSurface::replace_color(ddgr_color sc, ddgr_color dc) {
  int rowsize;
  ASSERT(m_SurfInit);

  grSurface::lock(&rowsize);

  switch (ddsfObj.bpp) {
  case BPP_16: {
    uint16_t *data = (uint16_t *)m_DataPtr;
    int rowsize_w = m_DataRowsize / 2;
    uint16_t scc = XLAT_RGB_TO_16(sc), dcc = XLAT_RGB_TO_16(dc);

    for (int y = 0; y < ddsfObj.h; y++) {
      for (int x = 0; x < ddsfObj.w; x++)
        if (data[x] == scc)
          data[x] = dcc;

      data += rowsize_w;
    }
    break;
  }

  case BPP_32:
  default:
    Int3();
  }

  grSurface::unlock();
}

//	---------------------------------------------------------------------------
//	lock and unlock functions
//		these handle memory and os locks
//	---------------------------------------------------------------------------

char *grSurface::lock(int *rowsize) {
  bool grerr;

  ASSERT(m_SurfInit);

  switch (ddsfObj.type) {
  case SURFTYPE_VIDEOSCREEN:
  case SURFTYPE_GENERIC:
    grerr = ddgr_surf_Lock(&ddsfObj, (void **)&m_OrigDataPtr, &m_DataRowsize);
    break;
  case SURFTYPE_MEMORY:
    grerr = gr_mem_surf_Lock(&ddsfObj, (void **)&m_OrigDataPtr, &m_DataRowsize);
    break;
  default:
    Int3(); // invalid type!!
  }

  m_DataPtr = m_OrigDataPtr;
  *rowsize = m_DataRowsize;

  if (!grerr)
    return nullptr;

  surf_Locked++; // increment lock counter for this surface

  rend_SetSoftwareParameters(ddgr_GetAspectRatio(), ddsfObj.w, ddsfObj.h, m_DataRowsize, (uint8_t *)m_OrigDataPtr);

  return m_DataPtr;
}

/* do lock based off of x,y offsets */
char *grSurface::lock(int x, int y, int *rowsize) {
  bool grerr = false;

  ASSERT(m_SurfInit);

  switch (ddsfObj.type) {
  case SURFTYPE_VIDEOSCREEN:
  case SURFTYPE_GENERIC:
    grerr = ddgr_surf_Lock(&ddsfObj, (void **)&m_OrigDataPtr, &m_DataRowsize);
    break;
  case SURFTYPE_MEMORY:
    grerr = gr_mem_surf_Lock(&ddsfObj, (void **)&m_OrigDataPtr, &m_DataRowsize);
    break;
  default:
    Int3(); // invalid type!!
  }

  *rowsize = m_DataRowsize;
  if (!grerr)
    return nullptr;

  /*	calculate the adjusted data pointer based off of x,y,bpp */
  char *data = m_OrigDataPtr;

  data += (y * m_DataRowsize);

  switch (ddsfObj.bpp) {
  case BPP_16:
    data += (x * 2);
    break;
  case BPP_32:
    data += (x * 4);
    break;
  default:
    Int3();
  }

  m_DataPtr = data;

  surf_Locked++; // increment lock counter for this surface

  rend_SetSoftwareParameters(ddgr_GetAspectRatio(), ddsfObj.w, ddsfObj.h, m_DataRowsize, (uint8_t *)m_OrigDataPtr);

  return m_DataPtr;
}

void grSurface::unlock() {
  bool grerr;

  ASSERT(m_SurfInit);
  ASSERT(surf_Locked);

  switch (ddsfObj.type) {
  case SURFTYPE_VIDEOSCREEN:
  case SURFTYPE_GENERIC:
    grerr = ddgr_surf_Unlock(&ddsfObj, m_OrigDataPtr);
    break;
  case SURFTYPE_MEMORY:
    grerr = gr_mem_surf_Unlock(&ddsfObj, m_OrigDataPtr);
    break;
  default:
    Int3(); // invalid type!!
  }

  surf_Locked--;
}

//	---------------------------------------------------------------------------
//	miscellaneous
//	---------------------------------------------------------------------------

void grSurface::attach_to_window(unsigned handle) {
  ASSERT(m_SurfInit);
  ddgr_surf_AttachHandle(&ddsfObj, handle);
}

//	Private functions
//		conversion_xlat
//	---------------------------------------------------------------------------

void grSurface::xlat8_16(char *data, int w, int h, char *pal) {
  /*	copy from 8bit source bitmap to destination surface just created and
          locked
  */
  uint16_t *dptr;
  char *sptr;
  int row, col;
  int rowsize_w;
  int height, width;
  uint8_t *upal = (uint8_t *)pal;

  dptr = (uint16_t *)m_DataPtr;
  sptr = (char *)data;
  rowsize_w = m_DataRowsize / 2;

  height = std::min(h, ddsfObj.h);
  width = std::min(w, ddsfObj.w);

  for (row = 0; row < height; row++) {
    for (col = 0; col < width; col++) {
      uint8_t spix = sptr[col];
      if (spix != 0) {
        int r = upal[spix * 3] >> 3;
        int g = upal[spix * 3 + 1] >> 2;
        int b = upal[spix * 3 + 2] >> 3;
        uint16_t destpix = (r << 11) | (g << 5) | b;

        dptr[col] = destpix;
      } else
        dptr[col] = 0;
    }
    sptr += 256;
    dptr += rowsize_w;
  }
}

void grSurface::xlat16_16(char *data, int w, int h, int format) {
  /*	16-bit copy from source bitmap to destination surface just created and
          locked
          This function performs scaling if the source width and height don't match
          that of the destinations - JL
  */
  uint16_t *sptr, *dptr;
  int row, col;
  int rowsize_w;
  int height, width;
  fix xstep = IntToFix(w) / ddsfObj.w;
  fix ystep = IntToFix(h) / ddsfObj.h;
  fix fu = 0, fv = 0;

  dptr = (uint16_t *)m_DataPtr;
  sptr = (uint16_t *)data;
  rowsize_w = m_DataRowsize / 2;

  height = ddsfObj.h;
  width = ddsfObj.w;

  // height = SET_MIN(h, ddsfObj.h);
  // width = SET_MIN(w, ddsfObj.w);

  for (row = 0; row < height; row++, fv += ystep, fu = 0) {
    for (col = 0; col < width; col++, fu += xstep) {
      int newpix;

      if (format == BITMAP_FORMAT_1555)
        newpix = sptr[(FixToInt(fv) * w) + FixToInt(fu)];
      else {
        int temp_pix = sptr[(FixToInt(fv) * w) + FixToInt(fu)];
        int r = (temp_pix >> 8) & 0xf;
        int g = (temp_pix >> 4) & 0xf;
        int b = (temp_pix) & 0xf;

        r <<= 4;
        g <<= 4;
        b <<= 4;

        newpix = OPAQUE_FLAG | GR_RGB16(r, g, b);
      }

      dptr[col] = newpix;
    }

    dptr += rowsize_w;
  }
}

void grSurface::xlat16_24(char *data, int w, int h) {
  uint16_t *sptr;
  char *dptr;
  int scol, dcol, row;
  int height, width;

  dptr = (char *)m_DataPtr;
  sptr = (uint16_t *)data;
  height = std::min(h, ddsfObj.h);
  width = std::min(w, ddsfObj.w);

  for (row = 0; row < height; row++) {
    dcol = 0;
    for (scol = 0; scol < width; scol++) {
      uint16_t pix;
      ddgr_color new_color;
      char r, g, b;
      pix = sptr[scol];

      new_color = GR_16_TO_COLOR(pix);

      r = GR_COLOR_RED(new_color);
      g = GR_COLOR_GREEN(new_color);
      b = GR_COLOR_BLUE(new_color);

      dptr[dcol++] = r;
      dptr[dcol++] = g;
      dptr[dcol++] = b;
    }
    sptr += w;
    dptr += m_DataRowsize;
  }
}

void grSurface::xlat24_16(char *data, int w, int h) {
  char *sptr;
  uint16_t *dptr;
  int scol, dcol, row;
  int rowsize_w, height, width;

  dptr = (uint16_t *)m_DataPtr;
  sptr = (char *)data;
  rowsize_w = m_DataRowsize / 2;
  height = std::min(h, ddsfObj.h);
  width = std::min(w, ddsfObj.w);

  for (row = 0; row < height; row++) {
    scol = 0;
    for (dcol = 0; dcol < width; dcol++) {
      uint16_t pix;
      char r, g, b;
      r = sptr[scol++];
      g = sptr[scol++];
      b = sptr[scol++];
      pix = ((uint16_t)(r >> 3) << 11) + ((uint16_t)(g >> 2) << 5) + ((uint16_t)(b >> 3));
      dptr[dcol] = pix;
    }
    sptr += (w * 3);
    dptr += m_DataRowsize;
  }
}

void grSurface::xlat32_16(char *data, int w, int h) {
  unsigned *sptr;
  uint16_t *dptr;
  int col, row;
  int rowsize_w, height, width;

  ASSERT((w % 4) == 0);

  dptr = (uint16_t *)m_DataPtr;
  sptr = (unsigned *)data;
  rowsize_w = m_DataRowsize / 2;
  height = std::min(h, ddsfObj.h);
  width = std::min(w, ddsfObj.w);

  for (row = 0; row < height; row++) {
    for (col = 0; col < width; col++) {
      unsigned pix;
      uint16_t spix;

      pix = sptr[col];
      spix = (uint16_t)(pix & 0x000000f8) >> 3;
      spix |= (uint16_t)(pix & 0x0000fc00) >> 5;
      spix |= (uint16_t)(pix & 0x00f80000) >> 8;
      dptr[col] = spix;
    }
    sptr += w;
    dptr += rowsize_w;
  }
}

//-----------------------------------------------------------------------------
//	surface list functions

void grSurface::add_to_list(ddgr_surface *sf) {
  ddgr_surface_node *node;

  ASSERT(sf != nullptr);

  if (!grSurface::surf_list_cur) {
    //	first surface on list
    node = new ddgr_surface_node;
    node->sf = sf;
    node->prev = nullptr;
    grSurface::surf_list = grSurface::surf_list_cur = node;
  } else {
    //	next surface on list.
    node = new ddgr_surface_node;
    node->sf = sf;
    node->prev = grSurface::surf_list_cur;
    grSurface::surf_list_cur = node;
  }

  //	mprintf(0, "Allocated surface %s [%d].\n", sf->name, DDData.surfs_alloc);
}

void grSurface::remove_from_list(ddgr_surface *sf) {
  ddgr_surface_node *node, *next_node;

  ASSERT(grSurface::surf_list_cur);
  ASSERT(sf != nullptr);

  //	node is current node.  next node is node after it on the list.  since we are
  //	going from end to beginning, this may confuse you.
  node = grSurface::surf_list_cur;
  next_node = nullptr; // grSurface::surf_list_cur should be at end of list, so no next node

  while (node) {
    if (node->sf == sf) {
      if (next_node) { // delete from within list.
        next_node->prev = node->prev;
      } else { // delete from end of list.  set list tail to deleted node prev.
        grSurface::surf_list_cur = node->prev;
      }
      delete node;

      node = nullptr; // break out of loop
    } else {
      //	go down one node.
      next_node = node;
      node = node->prev;
    }
  }

  //	mprintf(0, "Freed surface %s [%d].\n", sf->name, DDData.surfs_alloc);
}
