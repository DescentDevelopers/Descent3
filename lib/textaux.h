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

 * $Logfile: /DescentIII/Main/lib/textaux.h $
 * $Revision: 4 $
 * $Date: 11/03/98 7:04p $
 * $Author: Samir $
 *
 * Auxillary Text functions (helper function, not necessarily belonging to grtext)
 *
 * $Log: /DescentIII/Main/lib/textaux.h $
 *
 * 4     11/03/98 7:04p Samir
 * made Grtext_spacing global so word wrapper could access it.
 *
 * 3     7/14/98 2:47p Jeff
 * added textaux_ClipString
 *
 * 2     7/14/98 11:53a Samir
 * moved textaux to it's own libtary again.
 *
 * 1     7/13/98 4:41p Samir
 *
 * 2     7/11/98 9:14p Jeff
 * initial creation (moved from TelCom)
 *
 * $NoKeywords: $
 */

#ifndef TEXTAUX_H
#define TEXTAUX_H

//	textaux_WordWrap
//
//		Given a buffer of text, and an empty buffer of same size
//	it will break the source buffer up into lines (seperated by /n) of size width or smaller (in pixels).
//	All /n within the source buffer are preserved.  Source buffer is also not changed.
void textaux_WordWrap(const char *src, char *dest, int width, int font);

// textaux_CopyTextLine
//
//	This function goes hand-in-hand with textaux_WordWrap().  Given a buffer of data it will fill in
//	the dest buffer until it hits a /n or /0.  It returns a pointer to the start position of the next line,
//  or NULL if it's done with the buffer (it hit a /0).
//
//	Example of use:
//
//	char *nextline;
//	char linebuffer[256];
//	int y = INITIAL_Y;
//	int x = INITIAL_X;
//
//	nextline = textaux_CopyTextLine(buffer,linebuffer);
//
//	while((nextline)&&((y+font_height)<max_height) )
//	{
//		grtext_Printf(x,y,linebuffer);
//		y += font_height;
//		nextline = CopyTextLine(nextline,linebuffer);
//	}
//	grtext_Printf(x,y,linebuffer);
const char *textaux_CopyTextLine(const char *src, char *dest);

//	textaux_ClipString
//	   Given a width (in pixels), and a string, this function will truncate the string
//	to at most width pixels.  If the end parameter is not 0, then that char is attached to
//  the end of the string if it has to clip(the char's width is taken into consideration).
//  It is based off the current font.  if horizont_ratio is given it is used to correct for
//  possible different sized hud fonts.  For instance, if this string is going to be printed on
//  the hud, then you should always pass (DEFAULT_HUD_WIDTH/((float)*Game_window_w)) as the
//  horizont_ratio paramter.
void textaux_ClipString(int width, char *string, float horizont_ratio = 1, char end = 0);

#endif
