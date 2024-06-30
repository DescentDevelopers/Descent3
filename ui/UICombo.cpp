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

 * $Logfile: /DescentIII/Main/ui/UICombo.cpp $
 * $Revision: 3 $
 * $Date: 4/14/99 1:52a $
 * $Author: Jeff $
 *
 *
 *
 * $Log: /DescentIII/Main/ui/UICombo.cpp $
 *
 * 3     4/14/99 1:52a Jeff
 * fixed case mismatched #includes
 *
 * 2     3/03/99 2:45p Samir
 * use arrows up and down for combo boxes.
 *
 * 1     9/02/98 6:49p Samir
 * New combo box
 *
 * $NoKeywords: $
 */

#include "UIlib.h"

//	----------------------------------------------------------------------------
//	UIComboBox
//		A listbox may contain text items, BUT only one will be visible
//		Allows the user to scroll through them too.

UIComboBox::UIComboBox() {
  m_ItemList = NULL;
  m_nItems = 0;
  m_Index = 0;
  m_ShowUp = false;
  m_ShowDown = false;
  selectchange_fn = NULL;
}

UIComboBox::~UIComboBox() {}

//	creates a combo box
void UIComboBox::Create(UIWindow *parent, int id, int x, int y, int w, int h, int flags) {
  m_ItemList = NULL;
  m_nItems = 0;
  m_Index = 0;
  m_ShowUp = false;
  m_ShowDown = false;
  selectchange_fn = NULL;
  m_scrolltimer = 0.0f;
  m_scrollstage = 0;

  UIGadget::Create(parent, id, x, y, w, h, flags);

  m_CX = 1;
  m_CY = 1;
  m_CX2 = m_W - 1;
  m_CY2 = m_H - 1;
}

void UIComboBox::AddItem(const UIItem *item) {
  int i;

  if ((m_nItems % LISTBOX_BUFFER_SIZE) == 0) {
    //	dynamically allocated memory for listbox, expand as needed.
    UIItem **temp = new UIItem *[m_nItems + LISTBOX_BUFFER_SIZE];
    if (m_ItemList) {
      for (i = 0; i < m_nItems; i++)
        temp[i] = m_ItemList[i];
      delete[] m_ItemList;
    }
    m_ItemList = temp;
  }

  //	copy item passed to list.
  m_ItemList[m_nItems++] = (UIItem *)item;
}

// removes an item from the list.
void UIComboBox::RemoveItem(const UIItem *item) {
  int i, j;

  for (i = 0; i < m_nItems; i++) {
    if (item == m_ItemList[i]) {
      for (j = i; j < (m_nItems - 1); j++)
        m_ItemList[j] = m_ItemList[j + 1];
      m_nItems--;
      break;
    }
  }
}

// empties combobox
void UIComboBox::RemoveAll() { m_Index = m_nItems = 0; }

// selects the given item in the list.
void UIComboBox::SelectItem(const UIItem *item) {
  int i;

  for (i = 0; i < m_nItems; i++) {
    if (item == m_ItemList[i]) {
      SetSelectedIndex(i);
      break;
    }
  }
}

// returns the item at listbox index given.
UIItem *UIComboBox::GetItem(int index) const { return (index < m_nItems) ? m_ItemList[index] : NULL; }

// sets the selected index.
void UIComboBox::SetSelectedIndex(int index) {
  if (index < m_nItems) {
    m_Index = index;
    if (selectchange_fn)
      (*selectchange_fn)(index);
  }
}

void UIComboBox::SetSelectChangeCallback(void (*fn)(int)) { selectchange_fn = fn; }

//	called from outside gadget hierarchy.

void UIComboBox::OnDestroy() {
  UIGadget::OnDestroy();

  if (m_ItemList)
    delete[] m_ItemList;

  m_ItemList = NULL;
}

// behavior when gadget is being drawn.
void UIComboBox::OnDraw() {
  UITextItem uarrow;
  UITextItem darrow;
  char arrowbuf[2];

  if (m_Flags & UIF_BORDER)
    ui_DrawBox(GR_WHITE, 0, 0, m_W, m_H);

  //	draw all items in box that can be visible.
  int x = m_CX;
  int y = m_CY;

  if (!m_nItems)
    return;

  snprintf(arrowbuf, sizeof(arrowbuf), "%c", UI_UP_ARROW_CHAR);
  uarrow = UITextItem(arrowbuf, m_ItemList[m_Index]->get_color(), 255);
  snprintf(arrowbuf, sizeof(arrowbuf), "%c", UI_DOWN_ARROW_CHAR);
  darrow = UITextItem(arrowbuf, m_ItemList[m_Index]->get_color(), 255);

  ui_SetTextClip(m_CX, m_CY, m_CX2, m_CY2);

  // reset these
  m_ShowUp = m_ShowDown = false;
  m_ArrY = y;
  m_ArrH = uarrow.height();

  if (m_Index > 0) {
    m_UpArrX = m_CX;
    m_UpArrW = uarrow.width();
    uarrow.draw(m_UpArrX, m_ArrY);
    m_ShowUp = true;
  }
  if (m_Index < (m_nItems - 1)) {
    m_DownArrX = m_CX2 - darrow.width();
    m_DownArrW = darrow.width();
    darrow.draw(m_DownArrX, m_ArrY);
    m_ShowDown = true;
  }

  // draw current item.
  x = m_CX + uarrow.width() + 2;
  ui_SetTextClip(x, m_CY, m_CX2 - darrow.width() - 2, m_CY2);
  m_ItemList[m_Index]->draw(x, y);

  ui_ResetTextClip();
}

//	behavior when key is pressed.
void UIComboBox::OnKeyDown(int key) {
  if (key == KEY_UP) {
    if (m_Index > 0) {
      UIComboBox::SetSelectedIndex(m_Index - 1);
    }
  } else if (key == KEY_DOWN) {
    if (m_Index < (m_nItems - 1)) {
      UIComboBox::SetSelectedIndex(m_Index + 1);
    }
  }
}

// behavior when mouse button is pressed.
void UIComboBox::OnMouseBtnDown(int btn) {
  if (btn == UILMSEBTN) {
    int mx = SCREEN_TO_GAD_X(this, UI_input.mx);
    int my = SCREEN_TO_GAD_Y(this, UI_input.my);

    // determine whether to accept mouse input at this time.
    if (m_scrollstage == 0) {
      m_scrollstage = 1;
    } else if (m_scrollstage == 1) {
      if ((m_scrolltimer - UI_TIME()) < KEYDOWN_FIRST_DELAY)
        return;
      m_scrollstage = 2;
    } else if (m_scrollstage == 2) {
      if ((m_scrolltimer - UI_TIME()) < KEYDOWN_REPEAT_DELAY)
        return;
    }
    m_scrolltimer = UI_TIME();

    // do mouse processing.
    if (m_ShowUp && PT_IN_RECT(mx, my, m_UpArrX, m_ArrY, m_UpArrW, m_ArrH)) {
      if (m_Index > 0)
        UIComboBox::SetSelectedIndex(m_Index - 1);
    } else if (m_ShowDown && PT_IN_RECT(mx, my, m_DownArrX, m_ArrY, m_DownArrW, m_ArrH)) {
      if (m_Index < (m_nItems - 1))
        UIComboBox::SetSelectedIndex(m_Index + 1);
    }
  }
}

// behavior when mouse button is released.
void UIComboBox::OnMouseBtnUp(int btn) {
  if (btn == UILMSEBTN) {
    m_scrollstage = 0;
  }
}

void UIComboBox::OnSelect() { UIGadget::OnSelect(); }
