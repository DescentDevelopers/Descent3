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

 * $Logfile: /DescentIII/Main/ui/UIDraw.cpp $
 * $Revision: 16 $
 * $Date: 4/14/99 1:52a $
 * $Author: Jeff $
 *
 *	UI Library internal code.
 *
 * $Log: /DescentIII/Main/ui/UIDraw.cpp $
 *
 * 16    4/14/99 1:52a Jeff
 * fixed case mismatched #includes
 *
 * 15    10/22/98 2:43p Samir
 * ui_DrawStiring uses grtext_Puts.
 *
 * 14    10/12/98 9:28a Jason
 * fixed some rendering bugs
 *
 * 13    9/30/98 4:32p Samir
 * alpha is set to UICharAlpha when drawing a string.
 *
 * 12    9/08/98 10:27a Samir
 * added function to get text height.
 *
 * 11    5/07/98 2:59p Samir
 * modified ui_StartDraw.
 *
 * 10    3/18/98 6:25p Samir
 * Don't turn Z-buffering back on.
 *
 * 9     3/10/98 1:08p Samir
 * Fixed ui_DrawBitmap to always draw properly by setting states.
 *
 * 8     3/10/98 12:49p Samir
 * Added text saturation support.
 *
 * 7     2/26/98 4:56p Samir
 * Fixed some line drawing probs.
 *
 * 6     2/10/98 4:56p Samir
 * Fixed bug in ui_DrawRect.
 *
 * 5     1/30/98 7:06p Samir
 * Added text clipping.
 *
 * 4     1/18/98 4:22p Samir
 * Implemented new UIItem system.
 *
 * 3     1/05/98 4:36p Samir
 * Moved centering and fittext control flags to UIGadget class.
 *
 * 2     1/02/98 12:42p Samir
 * Added ui_GetTextWidth.
 *
 * 1     12/30/97 4:36p Samir
 * Initial revision
 *
 * $NoKeywords: $
 */

#include "UIlib.h"
#include "bitmap.h"
#include "ddvid.h"
#include "renderer.h"
#include "grtext.h"
#include "3d.h"

//////////////////////////////////////////////////////////////////////////////
//	VARIABLES
static int m_UIDrawLeft;
static int m_UIDrawTop;
static int m_UIDrawRight;
static int m_UIDrawBottom;
static ubyte m_UIDrawAlpha, m_UICharAlpha;
static int m_UITextFlags = 0;

//////////////////////////////////////////////////////////////////////////////
//	FUNCTIONS

//////////////////////////////////////////////////////////////////////////////
//	DRAWING PRIMATIVE FUNCTIONS

void ui_StartDraw(int left, int top, int right, int bottom) {
  m_UIDrawLeft = left;
  m_UIDrawTop = top;
  m_UIDrawRight = right;
  m_UIDrawBottom = bottom;
  m_UIDrawAlpha = m_UICharAlpha = 255;

  rend_StartFrame(m_UIDrawLeft, m_UIDrawTop, m_UIDrawRight, m_UIDrawBottom);
  grtext_SetParameters(0, 0, right - left + 1, bottom - top + 1);
}

void ui_EndDraw() {
  grtext_Flush();
  rend_EndFrame();
}

//	sets text clipping within the startdraw region
void ui_SetTextClip(int left, int top, int right, int bottom) { grtext_SetParameters(left, top, right, bottom); }

void ui_ResetTextClip() { grtext_SetParameters(m_UIDrawLeft, m_UIDrawTop, m_UIDrawRight, m_UIDrawBottom); }

//	draw primatives
void ui_DrawSetAlpha(ubyte alpha) { m_UIDrawAlpha = alpha; }

void ui_DrawLine(ddgr_color color, int x1, int y1, int x2, int y2) {
  if (color == TRANSPARENT_COLOR32)
    return;

  rend_SetFlatColor(color);
  rend_DrawLine(x1, y1, x2, y2);
}

void ui_DrawBox(ddgr_color color, int l, int t, int r, int b) {
  if (color == TRANSPARENT_COLOR32)
    return;
  rend_SetFlatColor(color);
  rend_DrawLine(l, t, r - 1, t);
  rend_DrawLine(r - 1, t, r - 1, b - 1);
  rend_DrawLine(l, b - 1, r - 1, b - 1);
  rend_DrawLine(l, t, l, b - 1);
}

void ui_DrawLTBox(ddgr_color lt_col, ddgr_color rb_col, int l, int t, int r, int b) {
  if (lt_col != TRANSPARENT_COLOR32) {
    rend_SetFlatColor(lt_col);
    rend_DrawLine(l, t, r - 1, t);
    rend_DrawLine(l, t, l, b - 1);
  }
  if (rb_col != TRANSPARENT_COLOR32) {
    rend_SetFlatColor(rb_col);
    rend_DrawLine(l, b - 1, r - 1, b - 1);
    rend_DrawLine(r - 1, t + 1, r - 1, b - 1);
  }
}

void ui_DrawRect(ddgr_color color, int l, int t, int r, int b) {
  if (color == TRANSPARENT_COLOR32)
    return;

  g3Point *pntlist[4], points[4];
  int i;

  // Set our four corners to cover the screen
  points[0].p3_sx = l;
  points[0].p3_sy = t;
  points[1].p3_sx = r;
  points[1].p3_sy = t;
  points[2].p3_sx = r;
  points[2].p3_sy = b;
  points[3].p3_sx = l;
  points[3].p3_sy = b;

  for (i = 0; i < 4; i++) {
    points[i].p3_z = 0;
    points[i].p3_flags = PF_PROJECTED;
    pntlist[i] = &points[i];
  }

  rend_SetZBufferState(0);
  rend_SetTextureType(TT_FLAT);
  rend_SetAlphaType(AT_CONSTANT);
  rend_SetLighting(LS_NONE);
  rend_SetFlatColor(color);
  rend_SetAlphaValue(m_UIDrawAlpha);
  rend_DrawPolygon2D(0, pntlist, 4);
}

//	draws all textures derived from ui_bm_handle.
void ui_DrawBitmap(UIBitmapItem *bi, int x, int y, ubyte alpha) {
  if (bi->is_chunked()) {
    chunked_bitmap *chunk = bi->get_chunked_bitmap();
    rend_DrawChunkedBitmap(chunk, x, y, alpha);
  } else {
    int bmp_w = bm_w(bi->get_bitmap(), 0);
    int bmp_h = bm_h(bi->get_bitmap(), 0);
    float u1, v1;

    if ((x + bmp_w) > m_UIDrawRight) {
      u1 = ((float)(m_UIDrawRight - x)) / ((float)bmp_w);
      bmp_w = m_UIDrawRight - x;
    } else
      u1 = 1.0f;

    if ((y + bmp_h) > m_UIDrawBottom) {
      v1 = ((float)(m_UIDrawBottom - y)) / ((float)bmp_h);
      bmp_h = m_UIDrawBottom - y;
    } else
      v1 = 1.0f;

    rend_SetOverlayType(OT_NONE);
    rend_SetLighting(LS_NONE);
    rend_SetColorModel(CM_MONO);
    rend_SetZBufferState(0);
    rend_SetAlphaType(ATF_CONSTANT + ATF_TEXTURE);
    rend_SetAlphaValue(alpha);
    rend_SetWrapType(WT_CLAMP);

    rend_DrawScaledBitmap(x, y, x + bmp_w, y + bmp_h, bi->get_bitmap(), 0, 0, u1, v1);
  }
}

int ui_GetBitmapWidth(UIBitmapItem *bi) {
  if (bi->is_chunked()) {
    chunked_bitmap *chunk = bi->get_chunked_bitmap();
    return chunk->pw;
  } else {
    return bm_w(bi->get_bitmap(), 0);
  }
}

int ui_GetBitmapHeight(UIBitmapItem *bi) {
  if (bi->is_chunked()) {
    chunked_bitmap *chunk = bi->get_chunked_bitmap();
    return chunk->ph;
  } else {
    return bm_h(bi->get_bitmap(), 0);
  }
}

//	sets font for text drawing.
void ui_DrawSetFont(int handle) { grtext_SetFont(handle); }

int ui_DrawGetFont() { return grtext_GetFont(); }

int ui_GetFontHeight() { return grfont_GetHeight(grtext_GetFont()); }

void ui_SetCharAlpha(ubyte alpha) { m_UICharAlpha = alpha; }

int ui_GetTextWidth(const char *text) { return grtext_GetLineWidth((char *)text); }

int ui_GetTextHeight(const char *text) { return grtext_GetHeight(text); }

void ui_DrawSetTextType(int type) {
  switch (type) {
  case UI_TEXTTYPE_SHADOW:
    m_UITextFlags = GRTEXTFLAG_SHADOW;
    break;
  case UI_TEXTTYPE_SATURATE:
    m_UITextFlags = GRTEXTFLAG_SATURATE;
    break;
  default:
    m_UITextFlags = 0;
  }
}

//	draws a line of text.
void ui_DrawString(ddgr_color col, int x, int y, const char *str) {
  grtext_SetFlags(m_UITextFlags);
  grtext_SetAlpha(m_UICharAlpha);
  grtext_SetColor(col);
  grtext_Puts(x, y, str);
  grtext_Flush();
}

void ui_DrawCenteredString(ddgr_color col, int xoff, int y, const char *str) {
  grtext_SetFlags(m_UITextFlags);
  grtext_SetAlpha(m_UICharAlpha);
  grtext_SetColor(col);
  grtext_CenteredPrintf(xoff, y, str);
  grtext_Flush();
}
