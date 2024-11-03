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

 * $Logfile: /DescentIII/Main/ui/UISlider.cpp $
 * $Revision: 10 $
 * $Date: 4/14/99 1:53a $
 * $Author: Jeff $
 *
 *	Slider code
 *
 * $Log: /DescentIII/Main/ui/UISlider.cpp $
 *
 * 10    4/14/99 1:53a Jeff
 * fixed case mismatched #includes
 *
 * 9     12/02/98 11:45a Samir
 * small fixes to slider control.
 *
 * 8     10/19/98 6:30p Jeff
 * changes made for detail variables.  Put in preset values.  Preset
 * options.  Removed terrain_cast from detail.  Put new callbacks in
 * UIListBox and UISlider
 *
 * 7     9/01/98 4:33p Samir
 * improved mouse handling of sliders.
 *
 * 6     3/10/98 7:25p Samir
 * Allow customization of visual slider.
 *
 * 5     3/04/98 12:08p Samir
 * Slider fixed.
 *
 * 4     1/30/98 7:05p Samir
 * Each gadget now has their own coordinate system.
 *
 * 3     1/18/98 4:24p Samir
 * Fixed up scrolling sliders.
 *
 * 2     1/13/98 4:28p Samir
 * Added sliders.
 *
 * 1     1/09/98 12:00p Samir
 * Initial revision
 *
 * $NoKeywords: $
 */

#include "UIlib.h"
#include "pserror.h"

//	Construction and destruction

UISlider::UISlider() {}

UISlider::~UISlider() {}

#define SLIDER_BUTTON_WIDTH (switch_item->width() ? switch_item->width() : 16)
#define SLIDER_BUTTON_HEIGHT (switch_item->height() ? switch_item->height() : 16)

void UISlider::Create(UIWindow *parent, int id, int x, int y, int w, int h, int flags) {
  ASSERT(w > 0);

  m_Pos = 0;
  m_Range = 1;
  selectchange_fn = NULL;
  selectchange_id_fn = NULL;
  m_callbackptr = NULL;

  switch_item = UIPrimativeItem(GR_DARKGRAY).CopyUIItem();
  slider_item = UIPrimativeItem(GR_DARKGRAY).CopyUIItem();

  UIGadget::Create(parent, id, x, y, w, h, flags);
}

//	sets visual characteristics of slider.
void UISlider::SetSliderSwitchItem(UIItem *item) {
  if (switch_item)
    delete switch_item;

  if (item) {
    switch_item = item->CopyUIItem();
  } else
    switch_item = UIPrimativeItem(GR_DARKGRAY).CopyUIItem();
}

void UISlider::SetSliderItem(UIItem *item) {
  if (slider_item)
    delete slider_item;

  if (item) {
    slider_item = item->CopyUIItem();
  } else
    slider_item = UIPrimativeItem(GR_DARKGRAY).CopyUIItem();
}

//	settings

void UISlider::SetRange(int range) {
  ASSERT(range > 0);

  m_Range = range;
  m_IntervalWidth = ((float)m_PixelWidth) / ((float)m_Range);
}

//	called from outside gadget hierarchy.

// override: called when resized or before drawing.
void UISlider::OnFormat() {
  if (slider_item->width() == 0)
    m_W = GetSavedW();
  else
    m_W = slider_item->width();

  if (switch_item->height() == 0)
    m_H = GetSavedH();
  else
    m_H = switch_item->height();

  m_PixelWidth = m_W - SLIDER_BUTTON_WIDTH;
  m_IntervalWidth = ((float)m_PixelWidth) / ((float)m_Range);
  switch_w = SLIDER_BUTTON_WIDTH;
  switch_h = SLIDER_BUTTON_HEIGHT;

  UIGadget::OnFormat();
}

// behavior when gadget is being destroyed.
void UISlider::OnDestroy() {
  if (slider_item)
    delete slider_item;
  if (switch_item)
    delete switch_item;
}

// behavior when gadget is being drawn.
void UISlider::OnDraw() {
  //	draw the slider region and the slider switch
  switch_x = (int)((float)m_Pos * (float)m_IntervalWidth);
  switch_y = 0;

  if (slider_item->width())
    slider_item->draw(0, switch_y + (SLIDER_BUTTON_HEIGHT / 2) - slider_item->height() / 2);
  else
    slider_item->draw(0, switch_y + (SLIDER_BUTTON_HEIGHT / 2) - 1, m_W, 3);

  if (switch_item->width())
    switch_item->draw(switch_x, switch_y);
  else
    switch_item->draw(switch_x, switch_y, SLIDER_BUTTON_WIDTH, SLIDER_BUTTON_HEIGHT);
}

//	behavior when key is pressed.
void UISlider::OnKeyDown(int key) {
  if (key == KEY_LEFT && m_Pos > 0) {
    m_Pos--;
    OnSelect();
  } else if (key == KEY_RIGHT && m_Pos < m_Range) {
    m_Pos++;
    OnSelect();
  }
}

// behavior when key is released.
void UISlider::OnKeyUp(int key) {}

// behavior when mouse button is pressed.
void UISlider::OnMouseBtnDown(int btn) {
  if (btn == UILMSEBTN) {
    int x, new_pos; // within gadget.
                    //	determine where in slider the new switch should go.
    LOCK_FOCUS(this);
    x = SCREEN_TO_GAD_X(this, UI_input.mx);
    if (x < 0)
      x = 0;
    if (x > m_W)
      x = m_W;
    new_pos = (int)(((float)x) / ((float)m_IntervalWidth));
    if (new_pos < 0)
      new_pos = 0;
    else if (new_pos >= m_Range)
      new_pos = m_Range - 1;
    if (new_pos != m_Pos) {
      //	mprintf(0, "x=%d, interval=%d, pos=%d\n", x, m_IntervalWidth, new_pos);
      OnSelect();
    }
    m_Pos = new_pos;
  }
}

// behavior when mouse button is released.
void UISlider::OnMouseBtnUp(int btn) {
  if (btn == UILMSEBTN)
    UNLOCK_FOCUS(this);
}

void UISlider::OnSelect() {
  if (selectchange_fn) {
    (*selectchange_fn)(m_Pos);
  }

  if (selectchange_id_fn) {
    (*selectchange_id_fn)(GetID(), m_callbackptr);
  }

  UIGadget::OnSelect();
}

void UISlider::SetSelectChangeCallback(void (*fn)(int)) { selectchange_fn = fn; }

void UISlider::SetSelectChangeCallback(void (*fn)(int, void *), void *ptr) {
  selectchange_id_fn = fn;
  m_callbackptr = ptr;
}
