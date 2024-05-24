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

 * $Logfile: /DescentIII/Main/ui/UIListBox.cpp $
 * $Revision: 42 $
 * $Date: 4/30/99 6:49p $
 * $Author: Samir $
 *
 *	Listbox code
 *
 * $Log: /DescentIII/Main/ui/UIListBox.cpp $
 *
 * 42    4/30/99 6:49p Samir
 * fixed bug when scroll down with keyboard in last visible item of
 * listbox.
 *
 * 41    4/14/99 1:53a Jeff
 * fixed case mismatched #includes
 *
 * 40    3/22/99 1:55p Jeff
 * fixed more crashes
 *
 * 39    3/22/99 12:47p Jeff
 * oops...fixed some listbox crashes
 *
 * 38    3/19/99 9:21p Jeff
 * fixed some listbox sorting issues
 *
 * 37    2/24/99 1:14p Jeff
 * fixed sorting bug when you have 2 identical items
 *
 * 36    2/21/99 6:36p Samir
 * focusing changes and key input changes to ui.,
 *
 * 35    2/04/99 12:10p Jeff
 * fixed SelectItem
 *
 * 34    1/27/99 10:26p Jeff
 * fixed sorting bug, very odd it hasn't cropped up before, only happened
 * when allocating a new buffer of indices
 *
 * 32    10/19/98 6:30p Jeff
 * changes made for detail variables.  Put in preset values.  Preset
 * options.  Removed terrain_cast from detail.  Put new callbacks in
 * UIListBox and UISlider
 *
 * 31    10/06/98 8:05p Samir
 * added page up and page down!
 *
 * 30    10/06/98 7:30p Samir
 * scrolling listbox is cleaner.
 *
 * 29    9/30/98 4:33p Samir
 * listbox broke for keyboard for now.
 *
 * 28    9/22/98 3:57p Samir
 * listbox prob fixed again.
 *
 * 27    9/10/98 6:32p Samir
 * changed some visual characteristics of listboxes.
 *
 * 26    9/02/98 6:50p Samir
 * fixed some double clicking issues for listboxes.
 *
 * 25    8/31/98 12:37p Samir
 * SetSelectedIndex modifies visible index as well, and dblclick works
 * again.
 *
 * 24    8/25/98 5:27p Samir
 * new focusing rules for keyboard.
 *
 * 23    8/24/98 3:12p Samir
 * fixed text clipping
 *
 * 22    8/11/98 11:38a Samir
 * fixed listbox probs yet again. arrows on side this time.
 *
 * 21    7/29/98 3:26p Samir
 * fixed some listbox mouse problems.
 *
 * 20    7/28/98 7:13p Samir
 * reset index when removing all items.
 *
 * 19    7/15/98 2:26p Samir
 * may have fixed something with arrows.
 *
 * 18    6/22/98 2:15p Samir
 * implemented border flag.
 *
 * 17    6/05/98 5:36p Samir
 * cleaner keyboard inteface for listboxes.
 *
 * 16    4/29/98 1:04p Jeff
 * fixed up the UIListBox a bit (color control and scroll buttons)
 *
 * 15    4/29/98 11:55a Samir
 * allow user to change color of selected listbox item.
 *
 * 14    4/13/98 7:02p Samir
 * made some things protected.
 *
 * 13    3/16/98 11:43a Brent
 * (Jeff) fixed UIListBox so it doesn't draw arrows if nothing in the box
 *
 * 12    3/13/98 5:19p Jeff
 * added scroll button code
 *
 * 11    3/09/98 5:08p Samir
 * fixed bug.
 *
 * 10    3/09/98 5:05p Samir
 * Added callback to selection change function.
 *
 * 9     3/09/98 3:43p Samir
 * Single clicks should select an item in a listbox.
 *
 * 8     2/24/98 3:05p Samir
 * don't set listindex when selecting.
 *
 * 7     2/15/98 7:07p Samir
 * Listbox now supports up and down arrow keys.
 *
 * 6     2/13/98 6:36p Samir
 * Double click to select.
 *
 * 5     2/10/98 4:56p Samir
 * Listbox with alphas and a selection bar.
 *
 * 4     2/10/98 3:45p Samir
 * Fixed bug in drawing items over the boundaries of the listbox.
 *
 * 3     1/30/98 7:07p Samir
 * Simple box containing items.  Must be handled by a parent.
 *
 * 2     1/13/98 4:28p Samir
 * Fixed typo.
 *
 * 1     1/09/98 12:00p Samir
 * Initial revision
 *
 * $NoKeywords: $
 */

#include "UIlib.h"

#include <stdlib.h>
#include <string.h>

#define SCROLL_BUTTON_GAP 4

//	Construction and destruction

UIListBox::UIListBox() {
  m_NumItems = 0;
  m_SelectedIndex = 0;
  m_Index = 0;
  m_ItemList = NULL;
  m_Virt2Real = NULL;
  m_Real2Virt = NULL;
  m_MouseState = 0;
  selectchange_fn = NULL;
  selectchange_id_fn = NULL;
  m_callbackptr = NULL;
  m_TextOffX = 4;
  m_TextOffY = 4;
}

UIListBox::~UIListBox() {}

void UIListBox::Create(UIWindow *parent, int id, int x, int y, int w, int h, int flags) {
  m_NumItems = 0;
  m_ItemList = NULL;
  m_Virt2Real = NULL;
  m_Real2Virt = NULL;
  m_SelectedIndex = 0;
  m_Index = 0;
  m_MouseState = 0;
  m_ClickTime = 0;
  selectchange_fn = NULL;
  selectchange_id_fn = NULL;
  m_callbackptr = NULL;
  m_ShowDown = false;
  m_ShowUp = false;
  m_SelectColor = GR_WHITE;
  m_HiliteColor = GR_DARKGRAY;
  m_NumVisibleItems = 0;
  m_Alpha = 192;

  UIGadget::Create(parent, id, x, y, w, h, flags);
  m_CX = 1;
  m_CY = 1;
  m_CX2 = m_W - 1;
  m_CY2 = m_H - 1;
}

//	settings

// adds an item to the list, no sorting
void UIListBox::AddItem(const UIItem *item) {
  UIItem **temp;
  int i;
  int *tempvirt2real, *tempreal2virt;
  int real_index = -1;

  if (m_Flags & UILB_NOSORT) {
    // insert without sort

    if ((m_NumItems % LISTBOX_BUFFER_SIZE) == 0) {
      //	dynamically allocated memory for listbox, expand as needed.
      temp = new UIItem *[m_NumItems + LISTBOX_BUFFER_SIZE];
      tempvirt2real = new int[m_NumItems + LISTBOX_BUFFER_SIZE];
      tempreal2virt = new int[m_NumItems + LISTBOX_BUFFER_SIZE];

      if (m_ItemList) {
        for (i = 0; i < m_NumItems; i++) {
          temp[i] = m_ItemList[i];
          tempvirt2real[i] = m_Virt2Real[i];
          tempreal2virt[i] = m_Real2Virt[i];
        }

        delete[] m_ItemList;
        delete[] m_Virt2Real;
        delete[] m_Real2Virt;
      }
      m_ItemList = temp;
      m_Virt2Real = tempvirt2real;
      m_Real2Virt = tempreal2virt;
    }

    real_index = m_NumItems;

  } else {
    // only text item listboxes!

    // insert with sorting
    if ((m_NumItems % LISTBOX_BUFFER_SIZE) == 0) {
      //	dynamically allocated memory for listbox, expand as needed.
      temp = new UIItem *[m_NumItems + LISTBOX_BUFFER_SIZE];
      tempvirt2real = new int[m_NumItems + LISTBOX_BUFFER_SIZE];
      tempreal2virt = new int[m_NumItems + LISTBOX_BUFFER_SIZE];
      int oidx = 0, cidx = 0;
      int pidx = 0;
      real_index = -1;
      int result;

      if (m_ItemList) {
        while (oidx <= m_NumItems) {
          if (oidx == m_NumItems) {
            if (real_index == -1) {
              real_index = oidx;
            } else {
              temp[cidx] = m_ItemList[pidx];
              tempreal2virt[cidx] = m_Real2Virt[pidx];
            }
            oidx++;
            continue;
          }

          if (m_Flags & UILB_CASESENSITIVE)
            result = strcmp(((UITextItem *)m_ItemList[cidx])->GetBuffer(), ((UITextItem *)item)->GetBuffer());
          else
            result = stricmp(((UITextItem *)m_ItemList[cidx])->GetBuffer(), ((UITextItem *)item)->GetBuffer());
          if (result < 0) {
            // copy old
            temp[cidx] = m_ItemList[pidx];
            tempreal2virt[cidx] = m_Real2Virt[pidx];
            cidx++;
            pidx++;
          } else if (result >= 0) {
            // see if we need to insert, if not just keep copying
            if (real_index == -1) {
              real_index = cidx;
              cidx++;
            } else {
              temp[cidx] = m_ItemList[pidx];
              tempreal2virt[cidx] = m_Real2Virt[pidx];
              cidx++;
              pidx++;
            }
          }
          oidx++;
        }
        ASSERT(real_index != -1);

        // now we need to fix m_Virt2Real to represent the new structure,
        // all values>=real_index must be incremented
        for (oidx = 0; oidx <= m_NumItems; oidx++) {
          if (oidx < m_NumItems) {
            tempvirt2real[oidx] = m_Virt2Real[oidx];

            // if(oidx!=real_index){
            if (tempvirt2real[oidx] >= real_index)
              tempvirt2real[oidx]++;
          }
        }

        delete[] m_ItemList;
        delete[] m_Virt2Real;
        delete[] m_Real2Virt;
      } else {
        real_index = 0;
      }

      m_ItemList = temp;
      m_Virt2Real = tempvirt2real;
      m_Real2Virt = tempreal2virt;
    } else {
      // find where to insert, without allocating memory
      int oidx = 0, cidx = 0;
      real_index = -1;
      int result;

      int old_r2v;
      UIItem *old_item = NULL;

      while (oidx <= m_NumItems) {

        if (oidx == m_NumItems) {
          // we're past the slot of the old buffer
          if (real_index == -1) {
            // we haven't set the real index yet, so it has to go here
            real_index = m_NumItems;
          } else {
            // the real index has been set, we're still pulling back
            m_Real2Virt[cidx] = old_r2v;
            m_ItemList[cidx] = old_item;
          }
          oidx++;
          continue;
        } else {
          if (m_Flags & UILB_CASESENSITIVE)
            result = strcmp(((UITextItem *)m_ItemList[cidx])->GetBuffer(), ((UITextItem *)item)->GetBuffer());
          else
            result = stricmp(((UITextItem *)m_ItemList[cidx])->GetBuffer(), ((UITextItem *)item)->GetBuffer());
        }

        if (result < 0) {
          // we haven't gotten to the string yet, keep as they are
          cidx++;
        } else if (result >= 0) {
          // see if we need to insert, if not just keep copying
          if (real_index == -1) {
            // we need to insert here
            real_index = cidx;
            old_item = m_ItemList[cidx];
            old_r2v = m_Real2Virt[cidx];
            cidx++;
          } else {
            // old_item and old_r2v should be set!
            ASSERT(old_item);
            UIItem *oi;
            int or2v;

            or2v = m_Real2Virt[cidx];
            oi = m_ItemList[cidx];

            m_Real2Virt[cidx] = old_r2v;
            m_ItemList[cidx] = old_item;
            old_r2v = or2v;
            old_item = oi;

            cidx++;
          }
        }

        // continue on
        oidx++;
      }

      ASSERT(real_index != -1);

      // now we need to fix m_Virt2Real to represent the new structure,
      // all values>=real_index must be incremented
      cidx = 0;
      for (oidx = 0; oidx <= m_NumItems; oidx++) {
        if (oidx != real_index) {
          if (m_Virt2Real[cidx] >= real_index)
            m_Virt2Real[cidx]++;
          cidx++;
        }
      }
    }
  }

  //	copy item passed to list.
  m_ItemList[real_index] = (UIItem *)item;
  m_Virt2Real[m_NumItems] = real_index;
  m_Real2Virt[real_index] = m_NumItems;
  m_NumItems++;

  if (real_index <= m_SelectedIndex) {
    // adjust selected index
    if (m_SelectedIndex < m_NumItems - 1)
      m_SelectedIndex++;
  }
}

// removes an item from the list.
void UIListBox::RemoveItem(const UIItem *item) {
  int i, j, found = -1;

  for (i = 0; i < m_NumItems; i++) {
    if (item == m_ItemList[i]) {
      found = i;
      break;
    }
  }
  if (found == -1) {
    mprintf(0, "UIListBox:: Didn't find item to remove!\n");
    return;
  }

  // found is the real index to remove
  // go through virt2real and subtract one from values of found or higher
  // go through real2virt and move everything from index found on down, and subtract
  //	1 from those with value of found or higher
  int real_index = found;              // index to remove
  int virt_index = m_Real2Virt[found]; // index to remove
  int real_thresh = virt_index;
  int virt_thresh = real_index;

  for (j = 0; j < (m_NumItems - 1); j++) {
    if (j >= real_index) {
      m_ItemList[j] = m_ItemList[j + 1];
      m_Real2Virt[j] = m_Real2Virt[j + 1];
    }

    if (j >= virt_index) {
      m_Virt2Real[j] = m_Virt2Real[j + 1];
    }

    if (m_Real2Virt[j] >= real_thresh)
      m_Real2Virt[j]--;

    if (m_Virt2Real[j] >= virt_thresh)
      m_Virt2Real[j]--;
  }
  m_NumItems--;

  if (real_index <= m_SelectedIndex) {
    // adjust selected index
    if (m_SelectedIndex > 0)
      m_SelectedIndex--;
  }
}

//	empties listbox
void UIListBox::RemoveAll() {
  m_Index = m_SelectedIndex = m_NumItems = 0;
  if (m_ItemList)
    delete[] m_ItemList;
  if (m_Virt2Real)
    delete[] m_Virt2Real;
  if (m_Real2Virt)
    delete[] m_Real2Virt;
  m_ItemList = NULL;
  m_Virt2Real = NULL;
  m_Real2Virt = NULL;
}

// selects the given item in the list.
void UIListBox::SelectItem(const UIItem *item) {
  int i, found = 0;

  for (i = 0; i < m_NumItems; i++) {
    if (item == m_ItemList[i]) {
      SetSelectedIndex(m_Real2Virt[i]);
      found = 1;
      break;
    }
  }
  if (!found) {
    mprintf(0, "UIListBox::SelectItem item not found!\n");
  }
}

// returns the item at listbox index given.
UIItem *UIListBox::GetItem(int index) const {
  if (m_NumItems == 0)
    return NULL;

  if (index >= 0 && index < m_NumItems) {
    int real_index = m_Virt2Real[index];
    ASSERT(real_index >= 0 && real_index < m_NumItems);
    return m_ItemList[real_index];
  } else
    return NULL;
}

// returns the current listbox index
int UIListBox::GetListIndex() const {
  if (m_NumItems == 0)
    return 0;

  if (m_Index < 0)
    return m_Index;

  if (!m_Real2Virt)
    return 0;

  int virt_index = m_Real2Virt[m_Index];
  ASSERT(virt_index >= 0 && virt_index < m_NumItems);

  return virt_index;
}

// sets the index of a listbox
void UIListBox::SetListIndex(int index) {
  if (m_NumItems == 0)
    return;

  if (index >= 0 && index < m_NumItems) {
    int real_index = m_Virt2Real[index];
    ASSERT(real_index >= 0 && real_index < m_NumItems);
    m_Index = real_index;
  }
}

// returns which item index is selected
int UIListBox::GetSelectedIndex() const {
  if (m_NumItems == 0)
    return 0;

  if (m_SelectedIndex < 0)
    return m_SelectedIndex;

  if (!m_Real2Virt)
    return 0;

  int virt_index = m_Real2Virt[m_SelectedIndex];
  ASSERT(virt_index >= 0 && virt_index < m_NumItems);
  return virt_index;
}

// sets the selected index.
void UIListBox::SetSelectedIndex(int index) {
  if (m_NumItems == 0)
    return;

  if (index >= 0 && index < m_NumItems) {
    int real_index = m_Virt2Real[index];
    ASSERT(real_index >= 0 && real_index < m_NumItems);

    int i, y = m_TextOffY;
    m_SelectedIndex = real_index;
    for (i = 0; i < m_NumItems; i++) {
      y += m_ItemList[i]->height() + 2;
      if (y > (m_H - m_TextOffY))
        break;
    }

    m_Index = m_SelectedIndex - i;
    if (m_Index < 0)
      m_Index = 0;

    if (selectchange_fn)
      (*selectchange_fn)(index);
    if (selectchange_id_fn)
      (*selectchange_id_fn)(GetID(), m_callbackptr);
  }
}

void UIListBox::SetInternalSelectedIndex(int index) {
  if (m_NumItems == 0)
    return;

  if (index >= 0 && index < m_NumItems) {
    int real_index = m_Virt2Real[index];
    ASSERT(real_index >= 0 && real_index < m_NumItems);
    m_SelectedIndex = real_index;

    if (selectchange_fn)
      (*selectchange_fn)(index);
    if (selectchange_id_fn)
      (*selectchange_id_fn)(GetID(), m_callbackptr);
  }
}

void UIListBox::SetSelectChangeCallback(void (*fn)(int)) { selectchange_fn = fn; }

void UIListBox::SetSelectChangeCallback(void (*fn)(int, void *), void *ptr) {
  selectchange_id_fn = fn;
  m_callbackptr = ptr;
}

//	called from outside gadget hierarchy.

void UIListBox::OnDestroy() {
  UIGadget::OnDestroy();

  if (m_ItemList)
    delete[] m_ItemList;
  if (m_Real2Virt)
    delete[] m_Real2Virt;
  if (m_Virt2Real)
    delete[] m_Virt2Real;

  m_ItemList = NULL;
  m_Real2Virt = NULL;
  m_Virt2Real = NULL;
}

// behavior when gadget is being drawn.
void UIListBox::OnDraw() {
  bool use_scroll = !(m_Flags & UILB_NOSCROLL);
  bool auto_select = ((m_Flags & UILB_AUTOSELECT) > 0);
  UITextItem uarrow;
  UITextItem darrow;
  char arrowbuf[2];

  if (m_Flags & UIF_BORDER)
    ui_DrawBox(GR_WHITE, 0, 0, m_W, m_H);

  ui_SetTextClip(m_CX, m_CY, m_CX2, m_CY2);

  //	draw all items in box that can be visible.
  int x, y, i;
  uint8_t alpha;

  x = m_TextOffX;
  y = m_TextOffY;

  if (m_NumItems) {
    snprintf(arrowbuf, sizeof(arrowbuf), "%c", UI_UP_ARROW_CHAR);
    uarrow = UITextItem(arrowbuf, m_ItemList[m_Index]->get_color(), 255);
    snprintf(arrowbuf, sizeof(arrowbuf), "%c", UI_DOWN_ARROW_CHAR);
    darrow = UITextItem(arrowbuf, m_ItemList[m_Index]->get_color(), 255);

    // reset these
    m_ShowUp = m_ShowDown = false;

    if (use_scroll) {
      m_UpArrowY0 = y;
      m_UpArrowY1 = y + uarrow.height();
      if (m_Index > 0)
        uarrow.draw(m_TextOffX, m_UpArrowY0, IsDisabled() ? uiDrawFaded : uiDrawNormal);
      m_ShowUp = true;
      m_ArrowWidth = uarrow.width() + 2;
      x = x + m_ArrowWidth;
    } else
      m_ArrowWidth = 0;
  }

  alpha = m_Alpha;

  for (i = m_Index; i < m_NumItems; i++) {
    // check to see if scroll arrow down needs to be drawn.
    //		if (m_SelectedIndex == i) {
    //			ui_DrawSetAlpha((alpha/4)+1);
    //			ui_DrawRect(m_HiliteColor, x,y,m_W-m_TextOffX,y+m_ItemList[i]->height()+2);
    //		}

    m_ItemList[i]->set_alpha((uint8_t)alpha);
    if (m_SelectedIndex == i) {
      ddgr_color old_color = m_ItemList[i]->get_color();
      m_ItemList[i]->set_color(m_SelectColor);
      m_ItemList[i]->draw(x + 1, y + 1, IsDisabled() ? uiDrawFaded : uiDrawNormal);
      if (!IsDisabled())
        m_ItemList[i]->draw(x + 1, y + 1, uiDrawAlphaSaturate);
      m_ItemList[i]->set_color(old_color);
    } else {
      m_ItemList[i]->draw(x + 1, y + 1, IsDisabled() ? uiDrawFaded : uiDrawNormal);
    }

    if (use_scroll && (i < m_NumItems)) {
      if ((y + m_ItemList[i]->height()) > (m_H - m_TextOffY - darrow.height() + 1)) {
        if (i < (m_NumItems - 1)) {
          m_DownArrowY0 = m_H - m_TextOffY - darrow.height() + 1;
          m_DownArrowY1 = m_H - m_TextOffY;
          darrow.draw(m_TextOffX, m_DownArrowY0, IsDisabled() ? uiDrawFaded : uiDrawNormal);
          m_ShowDown = true;
        }
        break;
      }
    } else if ((y + m_ItemList[i]->height() + 2) > (m_H - m_TextOffY)) {
      break;
    }

    y += m_ItemList[i]->height() + 2;
  }

  if (m_NumItems) {
    if (use_scroll && auto_select) {
      if (m_SelectedIndex < m_Index)
        UIListBox::SetSelectedIndex(m_Real2Virt[m_Index]);
      if (m_SelectedIndex > i)
        UIListBox::SetSelectedIndex(m_Real2Virt[i]);
    }
  }

  m_NumVisibleItems = i - m_Index;

  ui_ResetTextClip();
}

//	behavior when key is pressed.
void UIListBox::OnKeyDown(int key) {
  UIGadget *gadget = this;

  if (key == KEY_UP) {
    if (m_SelectedIndex > 0) {
      if (m_SelectedIndex == m_Index)
        m_Index--;
      m_SelectedIndex--;

      UIListBox::SetInternalSelectedIndex((m_Real2Virt) ? m_Real2Virt[m_SelectedIndex] : 0);
    }
  } else if (key == KEY_DOWN) {
    if (m_SelectedIndex < (m_NumItems - 1)) {
      if (m_SelectedIndex >= (m_Index + m_NumVisibleItems))
        m_Index++;
      m_SelectedIndex++;
      UIListBox::SetInternalSelectedIndex((m_Real2Virt) ? m_Real2Virt[m_SelectedIndex] : 0);
    }
  } else if (key == KEY_PAGEDOWN) {
    if (m_Index < (m_NumItems - m_NumVisibleItems))
      m_Index = m_Index + m_NumVisibleItems;
    if (m_Index > (m_NumItems - m_NumVisibleItems))
      m_Index = m_NumItems - m_NumVisibleItems;
    m_SelectedIndex = m_Index;
    UIListBox::SetInternalSelectedIndex((m_Real2Virt) ? m_Real2Virt[m_SelectedIndex] : 0);
  } else if (key == KEY_PAGEUP) {
    m_Index = m_Index - m_NumVisibleItems;
    if (m_Index < 0)
      m_Index = 0;
    m_SelectedIndex = m_Index;

    UIListBox::SetInternalSelectedIndex((m_Real2Virt) ? m_Real2Virt[m_SelectedIndex] : 0);
  }
}

// behavior when key is released.
void UIListBox::OnKeyUp(int key) {}

// behavior when mouse button is pressed.
void UIListBox::OnMouseBtnDown(int btn) {
  // One day, this system needs a complete rewrite, for all this stuff is a HACK!!!!
  if (btn == UILMSEBTN) {
    if (m_MouseState == 1 || m_MouseState == 2) { // single click pass or double clicking
                                                  // handle first slow delay scrolling (like keyboard input)
      if ((m_ClickTime + KEYDOWN_FIRST_DELAY) > UI_TIME()) {
        //	mprintf(0, "old=%.3f  cur=%.3f\n", m_ClickTime, UI_TIME());
        return;
      } else
        m_MouseState = 3;
    } else if (m_MouseState == 3) {
      // handle repeat scrolling (note this is if the user has scrolled once already.
      if ((m_ClickTime + KEYDOWN_REPEAT_DELAY) > UI_TIME()) {
        //	mprintf(0, "old=%.3f  cur=%.3f\n", m_ClickTime, UI_TIME());
        return;
      }
    } else if (m_MouseState == 0) {
      m_MouseState = 1;
      m_MouseX = SCREEN_TO_GAD_X(this, UI_input.mx);
      m_MouseY = SCREEN_TO_GAD_Y(this, UI_input.my);

      MseCheckSelect(m_MouseX, m_MouseY);

      if (UI_TIME() < (m_ClickTime + UI_DBLCLICK_DELAY)) {
        if (abs(m_LastMseX - m_MouseX) < UI_DBLCLICK_MSEDELTA && abs(m_LastMseY - m_MouseY) < UI_DBLCLICK_MSEDELTA) {

          if (MseCheckSelect(m_MouseX, m_MouseY))
            OnSelect();
          m_MouseState = 2;
        }
      }
      LOCK_FOCUS(this);
    }

    m_ClickTime = UI_TIME(); // use for scroll timing, this should be reset on button release
    // mprintf(0, "new=%.3f\n", m_ClickTime);

    // code here should allow for continuous scrolling on mouse down.
    // we need to add timing so scrolling isn't do damn quick.
    m_LastMseX = m_MouseX;
    m_LastMseY = m_MouseY;

    if (m_ShowUp && m_MouseY >= m_UpArrowY0 && m_MouseY <= m_UpArrowY1) {
      if (m_MouseX < (m_TextOffX + m_ArrowWidth)) {
        if (m_Index > 0)
          m_Index--;
      }
    } else if (m_ShowDown && m_MouseY >= m_DownArrowY0 && m_MouseY <= m_DownArrowY1) {
      if (m_MouseX < (m_TextOffX + m_ArrowWidth)) {
        if (m_Index < (m_NumItems - m_NumVisibleItems))
          m_Index++;
      }
    }
  }
}

// behavior when mouse button is released.
void UIListBox::OnMouseBtnUp(int btn) {
  if (btn == UILMSEBTN) {
    if (m_MouseState == 0)
      return;
    if (m_MouseState == 2) // double clicked.
      m_ClickTime = -2.0f;
    else if (m_MouseState == 1 || m_MouseState == 3) // single clicked or prolonged single click.
      m_ClickTime = UI_TIME();
    m_MouseState = 0;
    UNLOCK_FOCUS(this);
  }
}

void UIListBox::OnSelect() { UIGadget::OnSelect(); }

void UIListBox::SetSelectedColor(ddgr_color col) { m_SelectColor = col; }

void UIListBox::SetHiliteColor(ddgr_color col) { m_HiliteColor = col; }

// override: behavior when gadget loses input focus.
void UIListBox::OnLostFocus() {
  m_Alpha = 192;

  UIGadget::OnLostFocus();
}

// override: behavior when gadget gains input focus.
void UIListBox::OnGainFocus() {
  m_Alpha = 255;
  UIGadget::OnGainFocus();
}

// override: behavior when gadget is processed
void UIListBox::OnUserProcess() {
  //	if mouse is outside listbox, unlock the focus on this listbox.
  //	if (this->IsLocked()) {
  //		if (!m_Wnd->HasKeyFocus() && !PT_IN_GADGET(UIGadget::m_Wnd, this, UI_input.mx, UI_input.my)) {
  //			UNLOCK_FOCUS(this);
  //			this->LostFocus();
  //		}
  //	}
}

// check if x and y selection hits a selected item.
bool UIListBox::MseCheckSelect(int x, int y) {
  int i, ty;
  bool use_scroll = !(m_Flags & UILB_NOSCROLL);

  if (!m_NumItems)
    return false;

  if (m_ShowUp)
    ty = m_UpArrowY0;
  else
    ty = m_TextOffY;

  for (i = m_Index; i < m_NumItems; i++) {
    //	check if mouse down event occurred within this item.
    if (y >= ty && y < (ty + m_ItemList[i]->height() + 2)) {
      if (x > (m_ArrowWidth + m_TextOffX)) {
        UIListBox::SetInternalSelectedIndex((m_Real2Virt) ? m_Real2Virt[i] : 0);
        return true;
      }
    }

    if (m_ShowDown && (ty + m_ItemList[i]->height()) > m_DownArrowY1)
      return false;
    else if ((ty + m_ItemList[i]->height()) > (m_H - m_TextOffY))
      return false;

    ty += m_ItemList[i]->height() + 2;
  }

  return false;
}
