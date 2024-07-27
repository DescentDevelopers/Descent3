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

 * $Logfile: /DescentIII/Main/2dlib/pentext.cpp $
 * $Revision: 8 $
 * $Date: 11/01/98 1:57a $
 * $Author: Jeff $
 *
 * handles text drawing caps for viewports
 *
 * $Log: /DescentIII/Main/2dlib/pentext.cpp $
 *
 * 8     11/01/98 1:57a Jeff
 * converted the vsprintf calls to use the Pvsprintf, which is a safe
 * vsprintf, no buffer overflows allowed
 *
 * 7     12/19/97 12:31p Samir
 * Better renderer support.
 *
 * 6     12/12/97 6:40p Samir
 * Some viewport font functions.
 *
 * 5     11/17/97 3:46p Matt
 * Made the tab character move the current text location to the next even
 * multiple of the width of a space
 *
 * 4     11/16/97 5:06p Matt
 * Fixed puts() to deal correctly with multiple blank likes in a string.
 *
 * 3     11/14/97 5:37p Matt
 * Fixed small bug w/ printing multi-line strings
 *
 * 2     8/15/97 6:34p Samir
 * Fixed bug when passing an empty string to puts.
 *
 * 13    3/04/97 12:06p Samir
 * fixed bug in draw_clip_text_line.
 *
 * 12    2/13/97 3:44p Samir
 * get_text_line_width works correctly with special color text formatting.
 *
 * 11    2/13/97 2:24p Samir
 * Added mid-string color-coding text.  Note that when
 * done with text operation, the original color is restored.
 *
 * 10    2/06/97 6:33p Samir
 * Untested clipping of text.
 *
 * 9     2/06/97 12:44p Samir
 * Made change to draw_char call based off change of prototype.
 *
 * 8     2/05/97 10:35a Samir
 * Fixed text drawing at wrong location problem
 *
 * 7     2/04/97 11:14a Samir
 * Improved clipping system by adding a separate clipport based off of the
 * viewport's rectangle in it's parent surface
 *
 * 6     2/03/97 3:00p Samir
 * Fixed x updating problem in draw_text_line
 *
 * 5     1/30/97 7:01p Samir
 * updated font stuff
 *
 * $NoKeywords: $
 */

#include <cstdio>
#include <cstdarg>
#include <cstring>

#include "gr.h"
#include "mono.h"
#include "renderer.h"

#define CLIP_LEFT (vp_InitLeft + vp_Left)
#define CLIP_TOP (vp_InitTop + vp_Top)
#define CLIP_RIGHT (vp_InitLeft + vp_Right)
#define CLIP_BOTTOM (vp_InitTop + vp_Bottom)

#define STR_BUF_SIZE 512
static char Str_buf[STR_BUF_SIZE];      // String buffer for variable arguments
static char Draw_str_buf[STR_BUF_SIZE]; // used to buffer words in a string

//	---------------------------------------------------------------------------
//	Viewport font and text funcitons
//	---------------------------------------------------------------------------

void grViewport::set_font(const char *fontname) { text_Font.init(fontname); }

void grViewport::get_font(char *fontname, int size) {
  ASSERT(size > (int)strlen(text_Font.get_name()));
  strcpy(fontname, text_Font.get_name());
}

//	---------------------------------------------------------------------------
//	Standard text functions
//	---------------------------------------------------------------------------

int grViewport::printf(grTextAlign align, int x, int y, char *fmt, ...) {
  std::va_list arglist;
  int len;

  va_start(arglist, fmt);
  len = std::vsnprintf(Str_buf, STR_BUF_SIZE, fmt, arglist);
  va_end(arglist);
  if (len < 0)
    return 0;

  return grViewport::puts(align, x, y, Str_buf);
}

int grViewport::printf(int x, int y, char *fmt, ...) {
  std::va_list arglist;
  int len;

  va_start(arglist, fmt);
  len = std::vsnprintf(Str_buf, STR_BUF_SIZE, fmt, arglist);
  va_end(arglist);
  if (len < 0)
    return 0;

  return grViewport::puts(grTextUnjustified, x, y, Str_buf);
}

int grViewport::puts(int x, int y, char *str) { return grViewport::puts(grTextUnjustified, x, y, str); }

//	Ultimately all text output string functions call this monolith which
//	calls the ddgr interface.

int grViewport::puts(grTextAlign align, int x, int y, char *str) {
  int cur_x, cur_y, gx, gy;
  char *line, *save_pos;
  tCharProperties old_props;

  //	setup rendering of text.
  rend_SetTextureType(TT_LINEAR);
  rend_SetOverlayType(OT_NONE);
  rend_SetFiltering(0);
  rend_SetLighting(LS_GOURAUD);
  rend_SetAlphaType(ATF_TEXTURE + ATF_CONSTANT);
  rend_SetColorModel(CM_RGB);
  rend_SetZBufferState(0);
  rend_SetAlphaValue(char_Props.alpha);

  get_char_properties(&old_props);

  cur_x = x;
  cur_y = y;

  line = str;

  do {
    // Look for newline, and if found, put in a 0 to terminate the line
    save_pos = strchr(line, '\n');
    if (save_pos)    // found an newline
      *save_pos = 0; // terminate the line

    int line_width;
    int clipped;

    line_width = get_text_line_width(line);

    if (align == grTextCentered)
      cur_x = (((vp_Right - vp_Left) + 1) / 2) - (line_width / 2);
    else if (align == grTextLeft)
      cur_x = 0;

    gy = global_y(cur_y);
    gx = global_x(cur_x); // Clip function needs global coords.

    clipped = 0;
    if ((CLIP_TOP > (gy + text_Font.height())) || (CLIP_BOTTOM < gy))
      clipped = 2;
    else if ((CLIP_LEFT > (gx + line_width)) || (CLIP_RIGHT < gx))
      clipped = 2;

    if (clipped != 2) {
      if (CLIP_LEFT > gx || CLIP_RIGHT < (gx + line_width))
        clipped = 1;
      if (CLIP_TOP > gy || CLIP_BOTTOM < (gy + text_Font.height()))
        clipped = 1;

      if (clipped == 0)
        draw_text_line(gx, gy, line);
      else if (clipped == 1)
        draw_text_line_clip(gx, gy, line);
    }
    cur_y += (get_text_line_spacing() + text_Font.height());
    cur_x = x;

    // replace the newline, if there was one
    if (save_pos)
      *save_pos++ = '\n';

    line = save_pos;

  } while (line);

  set_char_properties(GR_VPCP_ALL, &old_props);

  rend_SetFiltering(1);
  rend_SetZBufferState(1);

  return strlen(str);
}

//	---------------------------------------------------------------------------
//	Private functions
//	---------------------------------------------------------------------------

//	get_text_line_width()
//		returns length in pixels of text line using current text spacing settings

int grViewport::get_text_line_width(char *str) {
  int line_width = 0;
  int rgb_define_mode = 0;

  for (int i = 0; i < (int)strlen(str); i++) {
    int width;

    // note that if we hit the GR_COLOR_CHAR then the next three values should
    //	not count when defining the width of the line.
    if (rgb_define_mode == 3)
      rgb_define_mode = 0;
    else if (str[i] == GR_COLOR_CHAR)
      rgb_define_mode = 1;
    if (!rgb_define_mode) {
      if (str[i] == '\t') { // tab char
        int space_width;
        text_Font.get_char_info(' ', &space_width);
        space_width += text_Spacing;
        line_width = (line_width + space_width - 1) / space_width * space_width;
      } else {
        text_Font.get_char_info((int)str[i], &width);
        line_width += (width + get_text_spacing());
      }
    } else
      rgb_define_mode++;
  }

  return line_width - get_text_spacing();
}

/*	These internal routines just draw a line of text.  depending on the function
        we clip or don't clip
*/

void grViewport::draw_text_line_clip(int x, int y, char *str) {
  int ch_y, ch_x, ch_w, ch_h;   // what part of the character to draw
  int i, cur_x, draw_x, draw_y; // where to draw character section
  int h;

  /*	by clipping, we should first determine what our vertical clipping is.  then
          go through each character in the line and determine what is totally clipped,
          partially clipped and by how much, and not clipped at all and draw accordingly
  */
  h = text_Font.height();
  ch_y = 0;
  ch_h = h;
  draw_y = y;
  //	determine each character bitmap y and height to blt.
  if (CLIP_TOP >= y) {
    ch_y = CLIP_TOP - y;
    draw_y = CLIP_TOP;
  }
  if (CLIP_BOTTOM < (y + text_Font.height())) {
    ch_h = CLIP_BOTTOM - y;
  }
  ch_h = ch_h - ch_y; // do this to clip both top and bottom

  cur_x = x;
  for (i = 0; i < (int)strlen(str); i++) {
    uint8_t ch;
    int w;
    ch = (uint8_t)str[i];
    text_Font.get_char_info((int)ch, &w);

    if (ch == GR_COLOR_CHAR) {
      if ((i + 3) >= (int)strlen(str))
        Int3(); // This shouldn't happen!  bad string!
      set_text_color(GR_RGB(str[i + 1], str[i + 2], str[i + 3]));
      i += 4;
      if (i >= (int)strlen(str))
        Int3(); // This shouldn't happen too.
      ch = (uint8_t)str[i];
    } else if (ch == '\t') { // tab char
      int space_width;
      text_Font.get_char_info(' ', &space_width);
      space_width += text_Spacing;
      cur_x = (cur_x + space_width - 1) / space_width * space_width;
    }

    //	do horizontal clipping
    if (((cur_x + w) < CLIP_LEFT) || (cur_x > CLIP_RIGHT)) {
      cur_x += (text_Spacing + w);
    } else {
      ch_x = 0;
      ch_w = w;
      draw_x = cur_x;
      if (CLIP_LEFT > cur_x) {
        ch_x = CLIP_LEFT - cur_x;
        draw_x = CLIP_LEFT;
      }
      if (CLIP_RIGHT < (cur_x + w)) {
        ch_w = CLIP_RIGHT - cur_x;
      }
      ch_w = ch_w - ch_x;

      if (ch_x == 0 && ch_w == w && ch_y == 0 && ch_h == h)
        cur_x = text_Font.draw_char(sf_Parent, draw_x, draw_y, ch, &char_Props);
      else
        cur_x = text_Font.draw_char(sf_Parent, draw_x, draw_y, ch, ch_x, ch_y, ch_w, ch_h, &char_Props);

      cur_x += text_Spacing;
    }
  }
}

void grViewport::draw_text_line(int x, int y, char *str) {
  unsigned i, cur_x;

  /*	perform string drawing without viewport clipping.
          supports bitmap fonts or color(alpha) mapped fonts.
  */
  cur_x = x;
  for (i = 0; i < (int)strlen(str); i++) {
    uint8_t ch;

    ch = (uint8_t)str[i];

    if (ch == GR_COLOR_CHAR) {
      if ((i + 3) >= (int)strlen(str))
        Int3(); // This shouldn't happen!  bad string!
      set_text_color(GR_RGB(str[i + 1], str[i + 2], str[i + 3]));
      i += 4;
      if (i >= (int)strlen(str))
        Int3(); // This shouldn't happen too.
      ch = (uint8_t)str[i];
    } else if (ch == '\t') { // tab char
      int space_width;
      text_Font.get_char_info(' ', &space_width);
      space_width += text_Spacing;
      cur_x = (cur_x + space_width - 1) / space_width * space_width;
    } else {
      cur_x = text_Font.draw_char(sf_Parent, cur_x, y, ch, &char_Props);
      cur_x += text_Spacing;
    }
  }
}
