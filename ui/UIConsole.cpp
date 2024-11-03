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

 * $Logfile: /DescentIII/Main/ui/UIConsole.cpp $
 * $Revision: 8 $
 * $Date: 8/11/99 10:50a $
 * $Author: Samir $
 *
 *	Console system
 *
 * $Log: /DescentIII/Main/ui/UIConsole.cpp $
 *
 * 8     8/11/99 10:50a Samir
 * possible fix to console bug with really wacky formatted strings
 * exceeding rowsize.
 *
 * 7     4/14/99 1:52a Jeff
 * fixed case mismatched #includes
 *
 * 6     11/03/98 7:05p Samir
 * added word wrapping console.
 *
 * 5     10/13/98 8:21p Samir
 * disable input to console gadget.
 *
 * 4     6/23/98 5:05p Samir
 * console supports color per line ONLY.
 *
 * 3     6/22/98 2:15p Samir
 * added UIConsoleGadget.
 *
 * 2     2/02/98 7:35p Samir
 * Full scrolling for console windows.
 *
 * 1     1/30/98 3:26p Samir
 * UI Console initial revision.
 *
 * $NoKeywords: $
 */

#include <cstdarg>
#include <cstring>

#include "UIlib.h"
#include "grtext.h"
#include "mem.h"
#include "pserror.h"
#include "textaux.h"

#define CONSOLE_LINE_FILLER 128

//	UIConsoleGadget should display a simple console gadget inside a window

//	int m_ConsoleFont;							// standard font for text in console
//	char *m_ConsoleBuffer;						// text buffer of console.

UIConsoleGadget::UIConsoleGadget() {}

UIConsoleGadget::~UIConsoleGadget() {}

//	font = console font, NOT GADGET FONT.   This font is used to proportion window
void UIConsoleGadget::Create(UIWindow *parent, int id, int x, int y, int font, int cols, int rows, int flags) {
  int cw, ch, w, h;
  int i;

  grtext_SetFont(font);
  cw = grtext_GetLineWidth("A");
  ch = grfont_GetHeight(font) + 1;

  w = cw * (cols + 3);
  h = ch * (rows + 3);

  UIGadget::Create(parent, id, x, y, w, h, flags);

  //	initialize console info.
  m_ConsoleFont = font;
  m_Rows = rows;
  m_Cols = cols;
  m_VisRows = m_Rows;
  m_VisRowStart = 0;
  m_CurRow = 0;
  m_CurCol = 0;
  m_OffX = cw + cw / 2;
  m_OffY = ch + ch / 2;
  m_Rowsize = m_Cols + CONSOLE_LINE_FILLER + 1;
  m_LineIndex = 0;

  grtext_Reset();

  // extra 32 bytes per row for color coding.
  m_ConsoleBuffer = (char *)mem_malloc(m_Rows * m_Rowsize);
  m_PutsBufLen = 512;
  m_PutsBuffer = mem_rmalloc<char>(m_PutsBufLen);

  m_ColorRows = new ddgr_color[m_Rows];
  memset(m_ConsoleBuffer, 0, m_Rows * m_Rowsize);
  for (i = 0; i < m_Rows; i++)
    m_ColorRows[i] = GR_GREEN;

  // lock out any input to this gadget.
  UIGadget::Disable();
}

void UIConsoleGadget::OnDestroy() {
  if (m_ColorRows)
    delete[] m_ColorRows;
  if (m_ConsoleBuffer)
    mem_free(m_ConsoleBuffer);
  if (m_PutsBuffer)
    mem_free(m_PutsBuffer);

  UIGadget::OnDestroy();
}

//	functions
void UIConsoleGadget::puts(ddgr_color col, const char *str) {
  int len = strlen(str);
  uint8_t r = GR_COLOR_RED(col), g = GR_COLOR_GREEN(col), b = GR_COLOR_BLUE(col);
  char *linebuf;

  if (len >= m_PutsBufLen) {
    mem_free(m_PutsBuffer);
    m_PutsBufLen = len + 1;
    m_PutsBuffer = mem_rmalloc<char>(m_PutsBufLen);
  }

  textaux_WordWrap(str, m_PutsBuffer, m_W - 2 * m_OffX, m_ConsoleFont);
  len = strlen(m_PutsBuffer);

  m_LineColor = col;
  if (m_CurCol == 0)
    m_ColorRows[m_CurRow] = m_LineColor;

  if ((m_LineIndex + 5) >= m_Rowsize) {
    UIConsoleGadget::PutChar('\n'); // simulate newline to reset line index.

    // force return character.  note that this isn't a newline, but
    // the above PutChar should've done this anyways.  this is redundant code but since this bug didn't
    // happen often and is hard to reproduce, i'm not sure the aboce PutChar would really work, but the below lines
    // should definitely prevent a crash though may overwrite some text in the console window.  but the
    // newline should work and not mess up the display.
    if (m_LineIndex != 0) { // Above PutChar should've done this anyways. useless redundant code.
      m_CurCol = 0;         // doesn't really slow anything down.
      m_LineIndex = 0;
    }
  }

  //		Error("Console line overflow in UIConsoleGadget line index (%d).\n", m_LineIndex);
  linebuf = m_ConsoleBuffer + (m_Rowsize * m_CurRow);
  linebuf[m_LineIndex++] = GR_COLOR_CHAR;
  linebuf[m_LineIndex++] = (r == 0 ? 1 : r);
  linebuf[m_LineIndex++] = (g == 0 ? 1 : g);
  linebuf[m_LineIndex++] = (b == 0 ? 1 : b);

  for (int i = 0; i < len; i++)
    UIConsoleGadget::PutChar(m_PutsBuffer[i]);
}

//	Inheritable operations
void UIConsoleGadget::OnDraw() {
  int x, y, cy, index;
  char *linebuf;

  //	draw frame
  if (m_Flags & UIF_BORDER) {
    ui_DrawBox(GR_GREEN, 1, 1, m_W - 1, m_H - 1);
    ui_DrawBox(GR_RGB(64, 255, 64), 3, 3, m_W - 3, m_H - 3);
  }

  // draw text
  ui_SetCharAlpha(255);
  ui_DrawSetFont(m_ConsoleFont);

  x = m_OffX;
  y = m_OffY;

  index = 0;

  for (cy = m_VisRowStart; cy < (m_VisRowStart + m_VisRows); cy++) {
    linebuf = m_ConsoleBuffer + (m_Rowsize * cy);
    ui_DrawString(m_ColorRows[cy], x, y, linebuf);
    y = y + ui_GetFontHeight() + 1;
  }
}

//	OnUserProcess
//		Takes care of scrolling of text and any keyboard input.

void UIConsoleGadget::OnUserProcess() {}

//	Internal functions

void UIConsoleGadget::PutChar(int ch) {
  char *linebuf;

  ASSERT(m_CurRow < m_Rows);
  //	ASSERT(m_CurCol < m_Cols);

  if (ch == '\n') {
    m_CurCol = 0;
    m_LineIndex = 0;
    m_CurRow++;
    if (m_CurRow < (m_VisRowStart + m_VisRows))
      m_ColorRows[m_CurRow] = m_LineColor;
  } else {
    linebuf = m_ConsoleBuffer + (m_CurRow * m_Rowsize);
    linebuf[m_LineIndex] = (char)ch;
    linebuf[m_LineIndex + 1] = 0;

    //	adjust current character position.
    m_LineIndex++;
    m_CurCol++;
    if (m_LineIndex >= m_Rowsize) {
      m_LineIndex = 0;
      m_CurCol = 0;
      m_CurRow++;
      if (m_CurRow < (m_VisRowStart + m_VisRows))
        m_ColorRows[m_CurRow] = m_LineColor;
    }
  }
  if (m_CurRow >= (m_VisRowStart + m_VisRows)) {
    // PUT SCROLL CODE HERE
    UIConsoleGadget::Scroll();
    m_ColorRows[m_CurRow] = m_LineColor;
  }
}

void UIConsoleGadget::Scroll() {
  char *linebuf, *linebuf2;
  int row;

  //	 adjust visible row start if visible region is in buffer
  //	 other wise scroll all text.
  if (m_VisRowStart < (m_Rows - m_VisRows)) {
    m_VisRowStart++;
  } else {
    for (row = 1; row < m_Rows; row++) {
      linebuf = (m_ConsoleBuffer + (m_Rowsize)*row);
      linebuf2 = (m_ConsoleBuffer + (m_Rowsize) * (row - 1));
      memcpy(linebuf2, linebuf, m_Rowsize);
      memset(linebuf, 0, m_Rowsize);
      m_ColorRows[row - 1] = m_ColorRows[row];
    }
    m_CurRow--;
  }
}

//	UIConsole should display a simple console window where anyone can dump text into

//	int m_ConsoleFont;							// standard font for text in console
//	char *m_ConsoleBuffer;						// text buffer of console.

UIConsole::UIConsole() {}

UIConsole::~UIConsole() {}

//	font = console font, NOT GADGET FONT.   This font is used to proportion window
void UIConsole::Create(int x, int y, int font, int cols, int rows) {
  UIWindow::Create(x, y, 10, 10);
  UIPrimativeItem item{GR_BLACK};
  UIWindow::SetBackItem(&item);

  m_Console.Create(this, -1, 0, 0, font, cols, rows);

  m_W = m_Console.W();
  m_H = m_Console.H();
}

void UIConsole::Destroy() { UIWindow::Destroy(); }

//	functions
void UIConsole::puts(const char *str) { m_Console.puts(GR_GREEN, str); }

void UIConsole::puts(ddgr_color col, const char *str) { m_Console.puts(col, str); }

//	Inheritable operations
void UIConsole::OnDraw() {

  //	draw frame
  UIWindow::OnDraw();
}
