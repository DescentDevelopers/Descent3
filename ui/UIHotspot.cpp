/*
 * $Logfile: /DescentIII/Main/ui/UIHotspot.cpp $
 * $Revision: 16 $
 * $Date: 4/26/99 9:11p $
 * $Author: Matt $
 *
 * Hotspot code.
 *
 * $Log: /DescentIII/Main/ui/UIHotspot.cpp $
 *
 * 16    4/26/99 9:11p Matt
 * Took out saturation used by old newui.
 *
 * 15    4/24/99 8:23p Samir
 * set hotspot hotkey after gadget is created.
 *
 * 14    4/14/99 1:53a Jeff
 * fixed case mismatched #includes
 *
 * 13    10/13/98 3:47p Samir
 * hotspots now handle spacebats
 *
 * 12    10/01/98 12:57p Samir
 * fixed hotspots so their width and height touch text, no outer border.
 *
 * 11    9/30/98 4:33p Samir
 * disabled gadgets display dim text.
 *
 * 10    6/05/98 5:58p Jeff
 * Added a way to change the items for the states of a HotSpot
 *
 * 9     5/01/98 6:23p Samir
 * saturate hotspots when active.
 *
 * 8     3/09/98 3:43p Samir
 * Hotspot selected when mouse button goes up.
 *
 * 7     1/30/98 7:05p Samir
 * Each gadget now has their own coordinate system.
 *
 * 6     1/23/98 5:34p Samir
 * Allow for nonpersistant UIItems.
 *
 * 5     1/18/98 4:22p Samir
 * Implemented new UIItem system.
 *
 * 4     1/05/98 4:36p Samir
 * Moved centering and fittext control flags to UIGadget class.
 *
 * 3     1/02/98 2:13p Samir
 * Set alpha to full for item with focus.
 *
 * 2     1/02/98 12:47p Samir
 * Added selection and bettering centering?
 *
 * 1     12/30/97 4:36p Samir
 * Initial revision
 *
 * $NoKeywords: $
 */

#include "UIlib.h"

//	----------------------------------------------------------------------------
//	UIHotspot
//		This is simply a region within the parent window that when clicked on
//		or key pressed, does something.

UIHotspot::UIHotspot() {
  m_ItemOff = NULL;
  m_ItemOn = NULL;
}

UIHotspot::~UIHotspot() {}

//	optional key, when pressed, triggers hotspot.
void UIHotspot::Create(UIWindow *wnd, int id, int key, UIItem *itemoff, UIItem *itemon, int x, int y, int w, int h,
                       int flags) {
  ASSERT(itemoff);
  ASSERT(itemon);

  if (itemoff)
    m_ItemOff = itemoff->CopyUIItem();
  if (itemon)
    m_ItemOn = itemon->CopyUIItem();

  m_ItemCur = m_ItemOff;

  UIGadget::Create(wnd, id, x, y, w, h, flags);

  if (key) {
    UIGadget::SetHotkey(key);
  }
}

void UIHotspot::SetStates(UIItem *itemoff, UIItem *itemon) {
  ASSERT(itemoff);
  ASSERT(itemon);

  // determine which state it currently set, so we can set it on the new state
  bool currstate = false;

  if (m_ItemOn == m_ItemCur)
    currstate = true;

  if (m_ItemOff)
    delete m_ItemOff;
  if (m_ItemOn)
    delete m_ItemOn;

  m_ItemOff = itemoff->CopyUIItem();
  m_ItemOn = itemon->CopyUIItem();

  if (currstate)
    m_ItemCur = m_ItemOn;
  else
    m_ItemCur = m_ItemOff;
}

// override: behavior when gadget is destroyed.
void UIHotspot::OnDestroy() {
  UIGadget::OnDestroy();

  if (m_ItemOff)
    delete m_ItemOff;
  if (m_ItemOn)
    delete m_ItemOn;

  m_ItemOff = NULL;
  m_ItemOn = NULL;
}

// behavior when key is pressed.
void UIHotspot::OnKeyDown(int key) {
  if (key == KEY_SPACEBAR) {
    OnSelect();
  }
}

// behavior when mouse button is pressed.
void UIHotspot::OnMouseBtnDown(int btn) {
  if (btn == UILMSEBTN) {
    LOCK_FOCUS(this);
  }
}

// override: behavior when mouse button is released.
void UIHotspot::OnMouseBtnUp(int btn) {
  if (btn == UILMSEBTN) {
    if (HasFocus()) {
      if (PT_IN_GADGET(UIGadget::m_Wnd, this, UI_input.mx, UI_input.my)) {
        OnSelect();
      }
    }
    UNLOCK_FOCUS(this);
  }
}

// override: behavior when gadget loses input focus.
void UIHotspot::OnLostFocus() { m_ItemCur = m_ItemOff; }

// override: behavior when gadget gains input focus.
void UIHotspot::OnGainFocus() { m_ItemCur = m_ItemOn; }

// override: called when resized or before drawing.
void UIHotspot::OnFormat() {
  if (m_Flags & UIF_FIT) {
    m_W = m_ItemCur->width();
    m_H = m_ItemCur->height();
  }

  UIGadget::OnFormat();
}

// behavior when gadget is being drawn.
void UIHotspot::OnDraw() {
  //	draw simple background.
  if (m_ItemCur) {
    int x = (m_W / 2 - m_ItemCur->width() / 2);
    int y = (m_H / 2 - m_ItemCur->height() / 2);
    if (IsDisabled())
      m_ItemCur->draw(x, y, uiDrawFaded);
    else {
      m_ItemCur->draw(x, y);
    }
  }
}
