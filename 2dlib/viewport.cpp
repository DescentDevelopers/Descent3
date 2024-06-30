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

 * $Logfile: /DescentIII/Main/2dlib/viewport.cpp $
 * $Revision: 7 $
 * $Date: 12/19/97 5:23p $
 * $Author: Samir $
 *
 * 	this class manages a specific viewing region of either a screen or bitmap.
 *	functions such as font setting and bitmap bltting are supported
 *	under this class.
 *
 *	this class also spawns a pen object which effectively locks the screen or
 *	bitmap associated with it.
 *
 * $Log: /DescentIII/Main/2dlib/viewport.cpp $
 *
 * 7     12/19/97 5:23p Samir
 * Cleaned up a lot of code to call renderer code.
 *
 * 6     12/19/97 12:32p Samir
 * Support alpha for text.  Stubbed out clipping support.
 *
 * 5     11/18/97 12:32p Samir
 * Took out useless lines.
 *
 * 4     11/14/97 12:05p Samir
 * don't free font, just init it.
 *
 * 20    6/13/97 1:17p Samir
 * Fucked up last checkin
 *
 * 19    6/13/97 1:00p Samir
 * lock when called will always lock, so if the lock fails, then it's not
 * viewport's job to fix it.
 *
 * 18    6/12/97 6:29p Samir
 * DDGR v2.0 Changes in 2d system implemented.
 *
 * 17    6/06/97 12:18p Samir
 * OpenGL HACKS to call renderer for some drawing functions.  Must fix.
 *
 * 16    5/15/97 3:47p Samir
 * Fixed lock_clear.
 *
 * 15    5/13/97 6:36p Samir
 * added lock_clear functions.
 *
 * 14    4/01/97 4:37p Samir
 * Simply better circle functions using viewport line and pixel functions.
 * made an hline function that clips and a setpixel function that clips.
 *
 * 13    3/28/97 3:07p Samir
 * Clear function now takes a color
 *
 * 12    3/28/97 2:50p Samir
 * Fixed clip_rect fuckup.
 *
 * 11    2/18/97 1:04p Samir
 * Changed prototype for setpixel
 *
 * 10    2/18/97 11:38a Samir
 * Added getpixel and setpixel to grViewport. NOTE: you must lock the
 * viewport when doing these ops.
 *
 * 9     2/04/97 4:56p Samir
 * added 'hack' in viewport lock.  see the function to see what I mean.
 *
 * 8     2/04/97 12:50p Samir
 * Viewports now lock on CLIP_LEFT, CLIP_TOP
 *
 * 7     2/04/97 11:14a Samir
 * Improved clipping system by adding a separate clipport based off of the
 * viewport's rectangle in it's parent surface
 *
 * 6     2/03/97 7:56p Samir
 *
 * 5     1/31/97 11:02a Samir
 * Adjusted clear function to use x,y,width,height parms.
 *
 * 4     1/30/97 6:28p Samir
 * Stupid me, left some dummy text causing errors.
 *
 * 3     1/30/97 6:01p Samir
 * The partition of memory and os surfaces as well as the change in
 * ddgr_surface structure and all related changes.
 *
 * $NoKeywords: $
 */

#include <algorithm>

#include "gr.h"
#include "lib2d.h"
#include "renderer.h"

#include "pserror.h"

#define CLIP_LEFT (vp_InitLeft + vp_Left)
#define CLIP_TOP (vp_InitTop + vp_Top)
#define CLIP_RIGHT (vp_InitLeft + vp_Right)
#define CLIP_BOTTOM (vp_InitTop + vp_Bottom)

int grViewport::vp_Locked = 0;
grViewport *grViewport::vp_Current_VP = NULL;

//	---------------------------------------------------------------------------
//	grViewport constructor and destructor
//	---------------------------------------------------------------------------

grViewport::grViewport(grScreen *scr_parent) {
  ASSERT(scr_parent != NULL);

  sf_Parent = scr_parent;    // screen is a child of grSurface, so we
                             // can do this.
  vp_InitLeft = vp_Left = 0; // define bounds = entire screen
  vp_InitTop = vp_Top = 0;
  vp_InitRight = vp_Right = sf_Parent->width() - 1;
  vp_InitBottom = vp_Bottom = sf_Parent->height() - 1;

  //	initialize font object
  //	setup text settings.
  char_Props.col[0] = GR_WHITE;
  char_Props.col[1] = GR_WHITE;
  char_Props.col[2] = GR_WHITE;
  char_Props.col[3] = GR_WHITE;
  char_Props.alpha = 255;

  text_Spacing = 1;
  text_Line_spacing = 1;
  text_Font.init(DEFAULT_FONT);
}

grViewport::grViewport(grSurface *sf_parent) {
  ASSERT(sf_parent != NULL);

  sf_Parent = sf_parent;

  vp_InitLeft = vp_Left = 0; // define bounds = entire bitmap
  vp_InitTop = vp_Top = 0;
  vp_InitRight = vp_Right = sf_Parent->width() - 1;
  vp_InitBottom = vp_Bottom = sf_Parent->height() - 1;

  //	initialize font object
  //	setup text settings.
  char_Props.col[0] = GR_WHITE;
  char_Props.col[1] = GR_WHITE;
  char_Props.col[2] = GR_WHITE;
  char_Props.col[3] = GR_WHITE;
  char_Props.alpha = 255;

  text_Spacing = 1;
  text_Line_spacing = 1;
  text_Font.init(DEFAULT_FONT);
}

grViewport::~grViewport() {
  ASSERT(!vp_Locked); // Viewport shouldn't be locked here!

  //	text_Font.free();
}

void grViewport::set_clipping_rect(int left, int top, int right, int bottom) {
  ASSERT(!vp_Locked); // YOU CANT DO THIS WHILE LOCKED!

  //	these are local to the viewport.
  //	vp_Left = left;
  //	vp_Top = top;
  //	vp_Right = right;
  //	vp_Bottom = bottom;
}

//	these functions set and restore the state of the viewport (font, colors, etc.)
void grViewport::get_state(tVPState *state) {
  state->font = text_Font;
  state->text_spacing = text_Spacing;
  state->text_line_spacing = text_Line_spacing;
  get_char_properties(&state->chprops);
}

void grViewport::set_state(const tVPState *state) {
  set_char_properties(GR_VPCP_ALL, &state->chprops);
  text_Font = state->font;
  text_Spacing = state->text_spacing;
  text_Line_spacing = state->text_line_spacing;
}

//	---------------------------------------------------------------------------
//	font functions
//	---------------------------------------------------------------------------

//	---------------------------------------------------------------------------
//	bltting functions
//	---------------------------------------------------------------------------

void grViewport::clear(ddgr_color col) // clear with background color
{
  sf_Parent->clear(CLIP_LEFT, CLIP_TOP, (vp_Right - vp_Left) + 1, (vp_Bottom - vp_Top) + 1, col);
}

void grViewport::blt(int dx, int dy, grSurface *ssf) {
  int x1, y1, x2, y2, sx, sy;

  //	perform rectangular clipping
  x1 = global_x(dx);
  y1 = global_y(dy);
  x2 = x1 + ssf->width() - 1;
  y2 = y1 + ssf->height() - 1;

  if (x1 < CLIP_LEFT)
    sx = CLIP_LEFT - x1;
  else
    sx = 0;
  if (y1 < CLIP_TOP)
    sy = CLIP_TOP - y1;
  else
    sy = 0;

  if (clip_rect(x1, y1, x2, y2) == 2)
    return;

  sf_Parent->blt(x1, y1, ssf, sx, sy, x2 - x1 + 1, y2 - y1 + 1);
}

void grViewport::blt(int dx, int dy, grSurface *ssf, int sx, int sy, int sw, int sh) {
  int x1, y1, x2, y2;

  x1 = global_x(dx);
  y1 = global_y(dy);
  x2 = x1 + sw - 1;
  y2 = y1 + sh - 1;
  if (clip_rect(x1, y1, x2, y2) == 2)
    return;

  sf_Parent->blt(x1, y1, ssf, sx, sy, x2 - x1 + 1, y2 - y1 + 1);
}

//	---------------------------------------------------------------------------
//	general lock creation functions
//	---------------------------------------------------------------------------

grSurface *grViewport::lock() {
  int rowsize;

  ASSERT(!vp_Locked);

  sf_Parent->lock(CLIP_LEFT, CLIP_TOP, &rowsize);
  vp_Locked = 1;

  if (vp_Locked) {
    pen_Obj.sf = &sf_Parent->ddsfObj;
    pen_Obj.rowsize = sf_Parent->rowsize();
    pen_Obj.rgbcolor = GR_RGB(255, 255, 255);

    //	SUPERHACK.  Pen functions don't use the adjusted pointer of grSurface, but use the
    //	base data pointer provided by locking the entire surface at (0,0).  Pen functions
    //	use global coordinates, so they can calculate the adjusted pointer themselves.  This
    //	makes global_x and global_y consisitent with the surface base pointer.  This problem
    //	was caused by the clipping rect being a virtual viewport within a viewport to make
    //	the 3d system work better.
    pen_Obj.data = sf_Parent->m_OrigDataPtr;
  }

  grViewport::vp_Current_VP = this;

  return sf_Parent;
}

void grViewport::unlock() {
  ASSERT(vp_Locked);

  sf_Parent->unlock();
  vp_Locked = 0;

  grViewport::vp_Current_VP = NULL;

  pen_Obj.data = NULL;
  pen_Obj.rowsize = 0;
  pen_Obj.sf = NULL;
}

//	---------------------------------------------------------------------------
//	miscellaneous viewport functions
//	---------------------------------------------------------------------------

void grViewport::setpixel(ddgr_color col, int x, int y) {
  ASSERT(vp_Locked);

  rend_SetPixel(col, x, y);
}

void grViewport::setpixelc(ddgr_color col, int x, int y) {
  ASSERT(vp_Locked);

  if (x < CLIP_LEFT || x > CLIP_RIGHT || y < CLIP_TOP || y > CLIP_BOTTOM)
    return;

  rend_SetPixel(col, x, y);
}

ddgr_color grViewport::getpixel(int x, int y) {
  ASSERT(vp_Locked);

  return rend_GetPixel(x, y);
}

ddgr_color grViewport::getpixelc(int x, int y) {
  ASSERT(vp_Locked);

  if (x < CLIP_LEFT || x > CLIP_RIGHT || y < CLIP_TOP || y > CLIP_BOTTOM)
    return 0;

  return rend_GetPixel(x, y);
}

void grViewport::lock_clear(ddgr_color col) {
  int l = CLIP_LEFT, t = CLIP_TOP, r = CLIP_RIGHT, b = CLIP_BOTTOM;
  ddgr_surface *memsurf = sf_Parent->scratch_mem_surf;

  ASSERT(vp_Locked);

  if (sf_Parent->ddsfObj.flags & SURFFLAG_RENDERER) {
    rend_FillRect(col, l, t, r, b);
  } else {
    memsurf->bpp = pen_Obj.sf->bpp;

    gr_mem_surf_Init(memsurf, pen_Obj.data, pen_Obj.rowsize);
    gr_mem_surf_Clear(memsurf, col, l, t, r - l + 1, b - t + 1);
  }
}

void grViewport::lock_clear(ddgr_color col, int l, int t, int r, int b) {
  ddgr_surface *memsurf = sf_Parent->scratch_mem_surf;

  ASSERT(vp_Locked);

  l = global_x(l);
  t = global_y(t);
  r = global_x(r);
  b = global_y(b);

  clip_rect(l, t, r, b);

  if (sf_Parent->ddsfObj.flags & SURFFLAG_RENDERER) {
    rend_FillRect(col, l, t, r, b);
  } else {
    memsurf->bpp = pen_Obj.sf->bpp;
    gr_mem_surf_Init(memsurf, pen_Obj.data, pen_Obj.rowsize);
    gr_mem_surf_Clear(memsurf, col, l, t, r - l + 1, b - t + 1);
  }
}

//	---------------------------------------------------------------------------
//	private functions
//	---------------------------------------------------------------------------

//	grPen::clip_rect
//		returns 2 if totally clipped
//		returns 1 if partially clipped
//		returns 0 if not clipped.

int grViewport::clip_rect(int &l, int &t, int &r, int &b) {
  int clipped = 0;

  if (l > r)
    std::swap(l, r);
  if (t > b)
    std::swap(t, b);

  if (l > CLIP_RIGHT || t > CLIP_BOTTOM || r < CLIP_LEFT || b < CLIP_TOP)
    return 2;

  if (l < CLIP_LEFT) {
    l = CLIP_LEFT;
    clipped = 1;
  }
  if (t < CLIP_TOP) {
    t = CLIP_TOP;
    clipped = 1;
  }
  if (r > CLIP_RIGHT) {
    r = CLIP_RIGHT;
    clipped = 1;
  }
  if (b > CLIP_BOTTOM) {
    b = CLIP_BOTTOM;
    clipped = 1;
  }

  return clipped;
}
