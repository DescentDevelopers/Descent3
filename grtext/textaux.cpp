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

 * $Logfile: /DescentIII/Main/grtext/textaux.cpp $
 * $Revision: 6 $
 * $Date: 5/19/99 10:00p $
 * $Author: Jeff $
 *
 * Auxillary Text functions (helper function, not necessarily belonging to grtext)
 *
 * $Log: /DescentIII/Main/grtext/textaux.cpp $
 *
 * 6     5/19/99 10:00p Jeff
 * fixed word-wrap not to wrap words that are bigger than the line
 *
 * 5     11/03/98 7:04p Samir
 * made Grtext_spacing global so word wrapper could access it.
 *
 * 4     11/03/98 3:12p Jeff
 * fixed wordwrap so it doesn't modify source buffer
 *
 * 3     7/14/98 2:47p Jeff
 * added textaux_ClipString
 *
 * 2     7/14/98 11:53a Samir
 * moved textaux to its own library again.
 *
 * 1     7/13/98 4:41p Samir
 *
 * 2     7/11/98 9:14p Jeff
 * initial creation (moved from TelCom)
 *
 * $NoKeywords: $
 */

#include <cstdio>
#include <cstring>

#include "grtext.h"

//	textaux_WordWrap
//
//		Given a buffer of text, and an empty buffer of same size
//	it will break the source buffer up into lines (separated by /n) of size width or smaller (in pixels).
//	All /n within the source buffer are preserved.  Source buffer is also not changed.
void textaux_WordWrap(const char *src, char *dest, int width, int font) {
  // make sure we are kosher with the values past in
  if ((!src) || (!dest))
    return;
  if (src[0] == '\0')
    return;

  char *last_word;  // points to the character after the last complete word
  char *start_line; // points to the starting char of the line
  // char *destpos;		//points to the current position of the dest buffer
  int curr_width;    // current width
  int index;         // index into source string
  bool done = false; // if we are done adjusting for word-wrap

  // copy the source buffer into the destination buffer, where all work will go
  strcpy(dest, src);

  // initialize vars
  last_word = start_line = dest;
  curr_width = 0;
  index = 0;

  int num_words_on_line = 0;

  while (!done) {
    curr_width = 0;
    index = 0;
    num_words_on_line = 0;

    while (curr_width <= width || num_words_on_line == 0) {
      switch (start_line[index]) {
      case '\0':
        // we are done with everything
        done = true;
      case '\n':
        // we are done with this line
        curr_width = width + 1;
      case ' ':
        // update last_word
        last_word = &start_line[index];
        num_words_on_line++;
        break;
      }
      if ((start_line[index] != '\0') && (start_line[index] != '\n'))
        curr_width += (grfont_GetCharWidth(font, start_line[index]) + Grtext_spacing);
      index++;
    }
    // put a newline char
    if (!done) {
      last_word[0] = '\n';
    }

    // copy from start_line->last_word into the dest buffer
    start_line += (last_word - start_line + 1);
    last_word = start_line;
  }
}

// textaux_CopyTextLine
//
//	This function goes hand-in-hand with textaux_WordWrap().  Given a buffer of data it will fill in
//	the dest buffer until it hits a /n or /0.  It returns a pointer to the start position of the next line,
//  or NULL if it's done with the buffer (it hit a /0).
const char *textaux_CopyTextLine(const char *src, char *dest) {
  // make sure src and dest are allocated
  if (!src) {
    if (dest)
      dest[0] = '\0';
    return nullptr;
  }
  if (!dest)
    return nullptr;
  // see if we are at the end of the src
  if (src[0] == '\0') {
    dest[0] = '\0';
    return nullptr;
  }
  int i;
  i = 0;
  // find the end
  while ((src[i] != '\n') && (src[i] != '\0'))
    i++;
  if (src[i] == '\0') {
    // no more lines left after this
    strncpy(dest, src, i);
    dest[i] = '\0';
    return nullptr;
  } else {
    // we hit a newline char
    strncpy(dest, src, i);
    dest[i] = '\0';
    i++;
    return &src[i];
  }
}

//	textaux_ClipString
//	   Given a width (in pixels), and a string, this function will truncate the string
//	to at most width pixels.  If the end parameter is not 0, then that char is attached to
//  the end of the string if it has to clip(the char's width is taken into consideration).
//  It is based off the current font.  if horizont_ratio is given it is used to correct for
//  possible different sized hud fonts.  For instance, if this string is going to be printed on
//  the hud, then you should always pass (DEFAULT_HUD_WIDTH/((float)*Game_window_w)) as the
//  horizont_ratio paramter.
void textaux_ClipString(int width, char *string, float horizont_ratio, char end) {
  if (!string)
    return;

  float ratio = horizont_ratio;

  int string_length = strlen(string);
  char arrow_string[2];
  int arrow_length = 0;

  if (end) {
    snprintf(arrow_string, sizeof(arrow_string), "%c", end);
    arrow_length = (int)(ratio * ((float)grtext_GetTextLineWidth(arrow_string)));
  }

  if (width < arrow_length) {
    // Nothing can fit
    strcpy(string, "");
    return;
  }

  // adjust width based on the arrow
  width -= arrow_length;

  int size = 0;
  char save = string[0];
  while (size < string_length) {
    string[size] = '\0';
    if ((((float)grtext_GetTextLineWidth(string)) * ratio) >= width) {
      // We have to clip
      size--;
      if (end) {
        string[size] = end;
        string[size + 1] = '\0';
      }
      return;
    }
    // replace the char and move to the next
    string[size] = save;
    size++;
    save = string[size];
  }
  // The string didn't need to be clipped
}
