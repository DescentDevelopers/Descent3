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

 * $Logfile: /DescentIII/Main/ui/UIStatic.cpp $
 * $Revision: 12 $
 * $Date: 4/14/99 1:53a $
 * $Author: Jeff $
 *
 *	Static text
 *
 * $Log: /DescentIII/Main/ui/UIStatic.cpp $
 *
 * 12    4/14/99 1:53a Jeff
 * fixed case mismatched #includes
 *
 * 11    1/02/99 6:46p Matt
 * Fixed compile warning
 *
 * 10    12/30/98 12:16p Kevin
 * Auto Mission Download system
 *
 * 9     10/01/98 12:57p Samir
 * fixed hotspots so their width and height touch text, no outer border.
 *
 * 8     8/31/98 12:37p Samir
 * disable all static controls.
 *
 * 7     6/05/98 5:36p Samir
 * disable static items by default, so they don't mess up keyboard focus.
 *
 * 6     2/13/98 6:34p Samir
 * Added UIStatic class.
 *
 * 5     1/30/98 7:07p Samir
 * Changes to gadget system.
 *
 * 4     1/18/98 4:22p Samir
 * Implemented new UIItem system.
 *
 * 3     1/08/98 12:18p Samir
 * Static control size is always calculated.
 *
 * 2     1/05/98 4:36p Samir
 * Moved centering and fittext control flags to UIGadget class.
 *
 * 1     1/02/98 12:49p Samir
 * Initial revision.
 *
 *
 * $NoKeywords: $
 */

#include <cstring>

#include "UIlib.h"

//	----------------------------------------------------------------------------
//	UIText
//		This is a simple text.

void UIText::Create(UIWindow *parent, UITextItem *text, int x, int y, int flags) {
  UIStatic::Create(parent, text, x, y, 1, 1, flags);
}

//	inheritable operations
// override: called when resized or before drawing.
void UIText::OnFormat() {
  m_W = m_Title->width();
  m_H = m_Title->height();

  UIGadget::OnFormat();
}

void UIStatic::Create(UIWindow *parent, UIItem *item, int x, int y, int w, int h, int flags) {
  m_Background = NULL;
  if (item)
    m_Title = item->CopyUIItem();
  else
    m_Title = NULL;
  UIGadget::Create(parent, -1, x, y, w, h, flags);
  UIGadget::Disable();
}

void UIStatic::SetBackground(UIPrimativeItem *item) {
  if (m_Background)
    delete m_Background;
  m_Background = NULL;
  if (item)
    m_Background = (UIPrimativeItem *)item->CopyUIItem();
}

void UIStatic::SetTitle(UIItem *item) {
  if (m_Title)
    delete m_Title;
  m_Title = NULL;
  if (item)
    m_Title = item->CopyUIItem();
}

void UIStatic::OnDraw() {
  if (m_Flags & UIF_BORDER)
    ui_DrawBox(GR_WHITE, 0, 0, m_W, m_H);

  if (m_Background)
    m_Background->draw(0, 0, m_W, m_H);
  if (m_Title) {
    //	int y = (m_H/2 - m_Title->height()/2);
    //	int x = (m_W/2 - m_Title->width()/2);
    m_Title->draw(0, 0);
  }
}

void UIStatic::OnFormat() {
  if (m_Flags & UIF_FIT) {
    if (m_Title) {
      m_W = m_Title->width();
      m_H = m_Title->height();
    } else {
      m_W = 10;
      m_H = 10;
    }
  }

  UIGadget::OnFormat();
}

// called when gadget is being nuked.
void UIStatic::OnDestroy() {
  if (m_Title)
    delete m_Title;
  if (m_Background)
    delete m_Background;

  m_Title = NULL;
  m_Background = NULL;
}

//	----------------------------------------------------------------------------
//	UIProgress
//		This is a simple text.

void UIProgress::Create(UIWindow *parent, int x, int y, int w, int h, int flags) {
  m_W = w;
  m_H = h;
  m_ProgressWidth = 0;
  UIStatic::Create(parent, NULL, x, y, w, h, flags);
}

//	inheritable operations
// override: called when resized or before drawing.
void UIProgress::OnFormat() { UIGadget::OnFormat(); }

void UIProgress::Update(float progress) {
  m_Progress = progress;
  m_ProgressWidth = ((int)((m_W - 2) * m_Progress));
}

void UIProgress::OnDraw() {
  // Take the x & y & w/h & progress to draw the progress bar
  ui_DrawBox(GR_WHITE, 0, 0, m_W, m_H);
  ui_DrawBox(GR_WHITE, 1, 1, m_W - 1, m_H - 1);
  ui_DrawRect(GR_LIGHTGRAY, 2, 2, m_ProgressWidth, m_H - 2);
}
