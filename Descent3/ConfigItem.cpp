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

 * $Logfile: /DescentIII/main/ConfigItem.cpp $
 * $Revision: 19 $
 * $Date: 4/26/99 9:11p $
 * $Author: Matt $
 *
 * Contains the implementation for ConfigItem [config dialog UI objects]
 *
 * $Log: /DescentIII/main/ConfigItem.cpp $
 *
 * 19    4/26/99 9:11p Matt
 * Use small font instead of old UI font.
 *
 * 18    4/14/99 2:50a Jeff
 * fixed some case mismatched #includes
 *
 * 17    10/23/98 11:09a Jeff
 * fixed percent sign
 *
 * 16    10/20/98 12:14p Jeff
 * added color to the UIGroup
 *
 * 15    10/19/98 6:30p Jeff
 * changes made for detail variables.  Put in preset values.  Preset
 * options.  Removed terrain_cast from detail.  Put new callbacks in
 * UIListBox and UISlider
 *
 * 14    10/16/98 1:42p Jeff
 * added CIF_USEGROUP and general dialog look
 *
 * 13    10/07/98 2:54p Jeff
 * General UI fixes and additions
 *
 * 12    10/06/98 5:34p Jeff
 * various UI changes/improvements
 *
 * 11    9/23/98 3:07p Jeff
 * updated the colors and various other items of config and UI
 *
 * 10    9/08/98 11:40a Jeff
 * fixed radio button bug
 *
 * 9     9/04/98 3:52p Jeff
 * changes made from UI meeting
 *
 * 8     9/02/98 2:54p Jeff
 * added defines for text colors to be used throughout the game...fixed up
 * buddy menu too
 *
 * 7     6/16/98 10:38a Jeff
 * localization, strings pulled out to stringtable.h and d3.str
 *
 * 6     6/11/98 12:08p Jeff
 * fixed memory leak in CIT_CHECKBOX
 *
 * 5     6/05/98 5:59p Jeff
 * Added a CIT_CHECKBOX style
 *
 * 4     5/23/98 6:33p Jeff
 * Added some flags for the slider, and fixed it up a bit
 *
 * 3     5/22/98 5:02p Jeff
 * Increased functionality
 *
 * 2     5/21/98 10:35p Jeff
 * Initial creation
 *
 * $NoKeywords: $
 */

#include "ConfigItem.h"
#include "ddio.h"
#include "newui.h"
#include "application.h"
#include "descent.h"
#include "mono.h"
#include "ddio.h"
#include "gamefont.h"
#include "multi_ui.h"
#include "cfile.h"
#include "mem.h"
#include "game.h"
#include "stringtable.h"

#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "stringtable.h"

#define UI_CHECKBOX_OFF_CHAR 28
#define UI_CHECKBOX_ON_CHAR 29

ConfigItem::ConfigItem() {
  m_fInitial = 0;
  m_X = m_Y = m_W = m_H = 0;
  curr_x = curr_y = 0;
  m_fCurrentValue = 0;
  m_iFlags = 0;
  m_iType = 0;
  m_iID = NULL;
  m_iNumIDs = 0;
  m_iItems = 0;
  m_iValueType = 0;
  m_hWnd = NULL;
  m_tiList = NULL;
  m_tiCount = 0;
  m_hsList = NULL;
  m_hsCount = 0;
  m_sList = NULL;
  m_sCount = 0;
  m_rbList = NULL;
  m_rbCount = 0;
  m_bList = NULL;
  m_bCount = 0;
  m_lbList = NULL;
  m_lbCount = 0;
  m_fMin = 0;
  m_fMax = 100;
  m_iRange = 100;
  m_fUnit = 1;
  m_auxX = 0;
  m_bAlive = false;
  m_labeltext = NULL;
  m_fCallback = NULL;
  m_iCallback = NULL;
  m_bCallback = NULL;
}

ConfigItem::~ConfigItem() {
  if (m_bAlive)
    Destroy();
}

bool ConfigItem::Create(NewUIGameWindow *parentwnd, int type, int flags, int x, int y, int w, int h, char *label,
                        int item_x) {
  if (m_bAlive)
    return false;
  if (!parentwnd)
    return false;
  switch (type) {
  case CIT_HOTSPOTLIST:
  case CIT_SLIDER:
  case CIT_ONOFFBUTTON:
  case CIT_LISTBOX:
  case CIT_YESNOBUTTON:
  case CIT_RADIOBUTTON:
  case CIT_CHECKBOX:
    m_iType = type;
    break;
  default:
    mprintf(0, "Bad config item type in Create\n");
    return false;
  }
  if (!label)
    return false;

  m_hWnd = parentwnd;
  m_iFlags = flags;

  if ((m_iFlags & CIF_USEGROUP) && (type != CIT_RADIOBUTTON)) {
    // only radiobutton items support the CIF_USEGROUP
    mprintf(0, "CONFIGITEM: Only CIT_RADIOBUTTON supports CIF_USEGROUP flag\n");
    m_iFlags &= ~CIF_USEGROUP;
    Int3();
  }

  if (type == CIT_SLIDER) { // slider's need to have the label centered vertically
    UITextItem item{label, UICOL_TEXT_NORMAL};
    m_tLabel.Create(parentwnd, &item, x, y + 7, UIF_FIT);
  } else {
    if (!(m_iFlags & CIF_USEGROUP)) {
      // create a regular label
      UITextItem item{label, UICOL_TEXT_NORMAL};
      m_tLabel.Create(parentwnd, &item, x, y, UIF_FIT);
    } else {
      // save the label text
      m_labeltext = mem_strdup(label);
    }
  }

  m_auxX = x;
  m_auxY = y;
  m_X = item_x;
  m_Y = y;
  m_W = w;
  m_H = h;
  return true;
}

bool ConfigItem::Create(NewUIGameWindow *parentwnd, int type, char *label, int x, int y, int w, int h, int initialvalue,
                        int flags, int item_x) {
  if (!Create(parentwnd, type, flags, x, y, w, h, label, item_x))
    return false;

  m_iValueType = CIV_INT;
  m_iInitial = m_iCurrentValue = initialvalue;

  m_bAlive = true;
  return true;
}

bool ConfigItem::Create(NewUIGameWindow *parentwnd, int type, char *label, int x, int y, int w, int h,
                        bool initialvalue, int flags, int item_x) {
  if (!Create(parentwnd, type, flags, x, y, w, h, label, item_x))
    return false;

  m_iValueType = CIV_BOOL;
  m_bInitial = m_bCurrentValue = initialvalue;

  m_bAlive = true;
  return true;
}

bool ConfigItem::Create(NewUIGameWindow *parentwnd, int type, char *label, int x, int y, int w, int h,
                        float initialvalue, int flags, int item_x) {
  if (!Create(parentwnd, type, flags, x, y, w, h, label, item_x))
    return false;

  m_iValueType = CIV_FLOAT;
  m_fInitial = m_fCurrentValue = initialvalue;

  m_bAlive = true;
  return true;
}

void ConfigItem::Destroy(void) {
  if (!m_bAlive)
    return;

  int i;

  if (m_labeltext) {
    mem_free(m_labeltext);
    m_labeltext = NULL;
  }

  if ((m_tiCount) && (m_tiList)) {
    for (i = 0; i < m_tiCount; i++)
      if (m_tiList[i])
        delete m_tiList[i];
    mem_free(m_tiList);
  }

  if ((m_hsCount) && (m_hsList)) {
    for (i = 0; i < m_hsCount; i++)
      if (m_hsList[i])
        delete m_hsList[i];
    mem_free(m_hsList);
  }

  if ((m_sCount) && (m_sList)) {
    for (i = 0; i < m_sCount; i++)
      if (m_sList[i])
        delete m_sList[i];
    mem_free(m_sList);
  }

  if ((m_rbCount) && (m_rbList)) {
    for (i = 0; i < m_rbCount; i++)
      if (m_rbList[i])
        delete m_rbList[i];
    mem_free(m_rbList);
  }

  if ((m_bCount) && (m_bList)) {
    for (i = 0; i < m_bCount; i++)
      if (m_bList[i])
        delete m_bList[i];
    mem_free(m_bList);
  }

  if ((m_lbCount) && (m_lbList)) {
    for (i = 0; i < m_lbCount; i++)
      if (m_lbList[i])
        delete m_lbList[i];
    mem_free(m_lbList);
  }

  if ((m_iNumIDs) && (m_iID))
    mem_free(m_iID);

  m_bAlive = false;
}

bool ConfigItem::GetCurrentValue(int *currvalue) {
  if (!m_bAlive)
    return false;
  if (m_iValueType != CIV_INT)
    return false;
  if (m_iType == CIT_LISTBOX) // we need to get the current index since it only gets updated on a double click
    m_iCurrentValue = m_lbList[0]->GetSelectedIndex();
  *currvalue = m_iCurrentValue;
  return true;
}

bool ConfigItem::GetCurrentValue(bool *currvalue) {
  if (!m_bAlive)
    return false;
  if (m_iValueType != CIV_BOOL)
    return false;
  *currvalue = m_bCurrentValue;
  return true;
}

bool ConfigItem::GetCurrentValue(float *currvalue) {
  if (!m_bAlive)
    return false;
  if (m_iValueType != CIV_FLOAT)
    return false;
  *currvalue = m_fCurrentValue;
  return true;
}

bool ConfigItem::SetCurrentValue(int currvalue, bool call_back) {
  if (!m_bAlive)
    return false;
  if (m_iValueType != CIV_INT)
    return false;

  float f_cv = (float)currvalue;
  int i_cv = currvalue;

  // do some error checking
  switch (m_iType) {
  case CIT_HOTSPOTLIST:
  case CIT_RADIOBUTTON:
    if (i_cv < 0)
      i_cv = 0;
    if (i_cv >= m_iNumIDs)
      i_cv = m_iNumIDs - 1;
    break;
  case CIT_SLIDER:
    if (f_cv < m_fMin)
      f_cv = m_fMin;
    if (f_cv >= m_fMax)
      f_cv = m_fMax;
    i_cv = (int)(((float)(f_cv - m_fMin)) / m_fUnit);
    break;
  case CIT_LISTBOX:
    // handles error checking in itself
    break;
  case CIT_CHECKBOX:
  case CIT_ONOFFBUTTON:
  case CIT_YESNOBUTTON:
    Int3();
    break;
  }

  switch (m_iType) {
  case CIT_HOTSPOTLIST:
  case CIT_RADIOBUTTON:
    m_rbList[i_cv]->Activate();
    break;
  case CIT_SLIDER:
    m_sList[0]->SetPos(i_cv);
    UpdateSlider(0, call_back);
    break;
  case CIT_LISTBOX:
    m_lbList[0]->SetSelectedIndex(i_cv);
    break;
  case CIT_CHECKBOX:
  case CIT_ONOFFBUTTON:
  case CIT_YESNOBUTTON:
    Int3();
    break;
  }

  // set the value than update
  m_iCurrentValue = currvalue;

  // Update(int result);

  return true;
}

bool ConfigItem::SetCurrentValue(bool currvalue, bool call_back) {
  if (!m_bAlive)
    return false;
  if (m_iValueType != CIV_BOOL)
    return false;

  // do some error checking
  switch (m_iType) {
  case CIT_HOTSPOTLIST:
  case CIT_RADIOBUTTON:
  case CIT_SLIDER:
  case CIT_LISTBOX:
    Int3();
    break;
  case CIT_CHECKBOX:
  case CIT_ONOFFBUTTON:
  case CIT_YESNOBUTTON:
    break;
  }

  switch (m_iType) {
  case CIT_HOTSPOTLIST:
  case CIT_RADIOBUTTON:
  case CIT_SLIDER:
  case CIT_LISTBOX:
    Int3();
    break;
  case CIT_CHECKBOX:
    if (currvalue)
      m_hsList[0]->SetStates(m_tiList[2], m_tiList[3]); // Checked
    else
      m_hsList[0]->SetStates(m_tiList[0], m_tiList[1]); // Unchecked
    break;
  case CIT_ONOFFBUTTON:
  case CIT_YESNOBUTTON:
    if (currvalue)
      m_bList[0]->SetTitle(m_tiList[1]); // Yes
    else
      m_bList[0]->SetTitle(m_tiList[0]); // No
    break;
  }

  // set the value than update
  m_bCurrentValue = currvalue;

  // Update(int result);

  return true;
}

bool ConfigItem::SetCurrentValue(float currvalue, bool call_back) {
  if (!m_bAlive)
    return false;
  if (m_iValueType != CIV_FLOAT)
    return false;

  // do some error checking
  switch (m_iType) {
  case CIT_SLIDER:
    if (currvalue < m_fMin)
      currvalue = m_fMin;
    if (currvalue >= m_fMax)
      currvalue = m_fMax;
    currvalue = ((currvalue - m_fMin) / m_fUnit);
    break;
  case CIT_LISTBOX:
  case CIT_HOTSPOTLIST:
  case CIT_RADIOBUTTON:
  case CIT_CHECKBOX:
  case CIT_ONOFFBUTTON:
  case CIT_YESNOBUTTON:
    Int3();
    break;
  }

  switch (m_iType) {
  case CIT_SLIDER:
    m_sList[0]->SetPos((int)currvalue);
    UpdateSlider(0, call_back);
    break;
  case CIT_HOTSPOTLIST:
  case CIT_RADIOBUTTON:
  case CIT_LISTBOX:
  case CIT_CHECKBOX:
  case CIT_ONOFFBUTTON:
  case CIT_YESNOBUTTON:
    Int3();
    break;
  }

  // Update(int result);

  // set the value than update
  m_fCurrentValue = currvalue;

  return true;
}

bool ConfigItem::GetInitialValue(int *initvalue) {
  if (!m_bAlive)
    return false;
  if (m_iValueType != CIV_INT)
    return false;
  *initvalue = m_iInitial;
  return true;
}

bool ConfigItem::GetInitialValue(bool *initvalue) {
  if (!m_bAlive)
    return false;
  if (m_iValueType != CIV_BOOL)
    return false;
  *initvalue = m_bInitial;
  return true;
}

bool ConfigItem::GetInitialValue(float *initvalue) {
  if (!m_bAlive)
    return false;
  if (m_iValueType != CIV_FLOAT)
    return false;
  *initvalue = m_fInitial;
  return true;
}

void ConfigItem::Update(int result) {
  if (!m_bAlive)
    return;
  if (!m_iID)
    return;

  // first see if this is one of our items
  int index = -1;
  for (int i = 0; i < m_iNumIDs; i++) {
    if (m_iID[i] == result) {
      index = i;
      break;
    }
  }

  if (index == -1) // it isn't one of ours
    return;

  // now determine which type of update to call
  switch (m_iType) {
  case CIT_HOTSPOTLIST:
    UpdateHotSpotList(index);
    break;
  case CIT_SLIDER:
    UpdateSlider(index);
    break;
  case CIT_ONOFFBUTTON:
    UpdateOnOffButton(index);
    break;
  case CIT_LISTBOX:
    UpdateListBox(index);
    break;
  case CIT_YESNOBUTTON:
    UpdateYesNoButton(index);
    break;
  case CIT_RADIOBUTTON:
    UpdateRadioButton(index);
    break;
  case CIT_CHECKBOX:
    UpdateCheckBox(index);
    break;
  default:
    mprintf(0, "Bad ConfigItem Type in Update\n");
  }
}

void ConfigItem::UpdateHotSpotList(int index) {
  ASSERT((index >= 0) && (index < m_rbCount));
  m_iCurrentValue = index;
  m_rbList[m_iCurrentValue]->Activate();
  if (m_iCallback)
    (*m_iCallback)(m_iCurrentValue);
}

void ConfigItem::UpdateSlider(int index, bool call_callback) {
  ASSERT((index >= 0) && (index < m_sCount));
  // determine which type of variables int or float
  char temp[10];

  switch (m_iValueType) {
  case CIV_INT:
    m_iCurrentValue = (int)(((float)m_sList[index]->GetPos()) * m_fUnit + m_fMin + 0.5f);
    if (m_iCurrentValue > m_fMax)
      m_iCurrentValue = m_fMax;
    snprintf(temp, sizeof(temp), "%d", m_iCurrentValue);
    if (m_iFlags & CIF_PERCENT)
      strncat(temp, "%", sizeof(temp) - strlen(temp) - 1);
    if (m_iCallback && call_callback)
      (*m_iCallback)(m_iCurrentValue);
    break;
  case CIV_FLOAT:
    m_fCurrentValue = ((float)m_sList[index]->GetPos()) * m_fUnit + m_fMin;
    if (m_fCurrentValue > m_fMax)
      m_fCurrentValue = m_fMax;
    if (m_iFlags & CIF_NODECIMAL)
      snprintf(temp, sizeof(temp), "%.0f", m_fCurrentValue);
    else
      snprintf(temp, sizeof(temp), "%.2f", m_fCurrentValue);
    if (m_iFlags & CIF_PERCENT)
      strncat(temp, "%", sizeof(temp) - strlen(temp) - 1);
    if (m_fCallback && call_callback)
      (*m_fCallback)(m_fCurrentValue);
    break;
  default:
    Int3(); // Get Jeff
  }
  UITextItem item{temp, UICOL_TEXT_NORMAL};
  m_tLabel2.SetTitle(&item);
}

void ConfigItem::UpdateOnOffButton(int index) {
  ASSERT((index >= 0) && (index < m_bCount));
  m_bCurrentValue = !m_bCurrentValue;
  if (m_bCurrentValue)
    m_bList[index]->SetTitle(m_tiList[1]); // On
  else
    m_bList[index]->SetTitle(m_tiList[0]); // Off
  if (m_bCallback)
    (*m_bCallback)(m_bCurrentValue);
}

void ConfigItem::UpdateListBox(int index) {
  ASSERT((index >= 0) && (index < m_lbCount));
  m_iCurrentValue = m_lbList[index]->GetSelectedIndex();
  if (m_iCallback)
    (*m_iCallback)(m_iCurrentValue);
}

void ConfigItem::UpdateYesNoButton(int index) {
  ASSERT((index >= 0) && (index < m_bCount));
  m_bCurrentValue = !m_bCurrentValue;
  if (m_bCurrentValue)
    m_bList[index]->SetTitle(m_tiList[1]); // Yes
  else
    m_bList[index]->SetTitle(m_tiList[0]); // No
  if (m_bCallback)
    (*m_bCallback)(m_bCurrentValue);
}

void ConfigItem::UpdateCheckBox(int index) {
  ASSERT((index >= 0) && (index < m_hsCount));
  m_bCurrentValue = !m_bCurrentValue;
  if (m_bCurrentValue)
    m_hsList[index]->SetStates(m_tiList[2], m_tiList[3]); // Checked
  else
    m_hsList[index]->SetStates(m_tiList[0], m_tiList[1]); // Unchecked

  if (m_bCallback)
    (*m_bCallback)(m_bCurrentValue);
}

void ConfigItem::UpdateRadioButton(int index) {
  ASSERT((index >= 0) && (index < m_rbCount));
  m_rbList[index]->Activate();
  m_iCurrentValue = index;

  if (m_iCallback)
    (*m_iCallback)(m_iCurrentValue);
}

void ConfigItem::Add(int count, ...) {
  if (!m_bAlive)
    return;

  ASSERT(m_tiCount == 0);

  va_list marker;
  va_start(marker, count);
  int i;
  UITextItem *uitemp;

  switch (m_iType) {
  case CIT_HOTSPOTLIST:
    ASSERT(count > 0);
    ASSERT(m_rbCount == 0);
    // we need count text items
    // we need count radiobuttons
    // we need count IDs
    m_tiCount = count;
    m_tiList = (UITextItem **)mem_malloc(sizeof(UITextItem *) * m_tiCount);
    m_rbCount = count;
    m_rbList = (UIRadioButton **)mem_malloc(sizeof(UIRadioButton *) * m_rbCount);
    m_iNumIDs = count;
    m_iID = (int *)mem_malloc(sizeof(int) * m_iNumIDs);
    // make sure mallocs are ok
    ASSERT((m_tiList) && (m_rbList) && (m_iID));
    // initialize variables
    // adjust the curr x/y for the list
    curr_x = 0;
    curr_y = 0;
    for (i = 0; i < m_iNumIDs; i++) {
      // get unique ID
      m_iID[i] = GetUniqueID();
      // allocate text item
      m_tiList[i] = new UITextItem;
      // make sure allocation ok
      ASSERT(m_tiList[i]);
      uitemp = va_arg(marker, UITextItem *);
      (*m_tiList[i]) = *uitemp;
      (*m_tiList[i]).set_alpha(255);
      // allocate radio button
      m_rbList[i] = new UIRadioButton;
      // make sure allocation ok
      ASSERT(m_rbList[i]);
      if (i)
        m_rbList[i]->Create(m_hWnd, m_rbList[i - 1], m_iID[i], m_tiList[i], m_X + curr_x, m_Y + curr_y,
                            m_tiList[i]->width(), m_tiList[i]->height(), UIF_FIT | UIRB_NOBUTTON);
      else
        m_rbList[i]->Create(m_hWnd, NULL, m_iID[i], m_tiList[i], m_X + curr_x, m_Y + curr_y, m_tiList[i]->width(),
                            m_tiList[i]->height(), UIF_FIT | UIRB_NOBUTTON);

      (*m_tiList[i]).set_color(UICOL_HOTSPOT_HI);
      m_rbList[i]->SetStateItem(UI_BTS_ACTIVATED, m_tiList[i]);
      (*m_tiList[i]).set_color(UICOL_HOTSPOT_LO);
      m_rbList[i]->SetStateItem(UI_BTS_INACTIVE, m_tiList[i]);
      (*m_tiList[i]).set_color(UICOL_HOTSPOT_HI);
      m_rbList[i]->SetStateItem(UI_BTS_HILITE, m_tiList[i]);

      // adjust curr x/y
      curr_x += (m_rbList[i]->W() + HOTSPOTLIST_GAP);
    }
    // Initialize Config Item to initial value
    ASSERT(m_iValueType == CIV_INT);
    ASSERT((m_iInitial >= 0) && (m_iInitial < m_iNumIDs));
    m_rbList[m_iInitial]->Activate();
    break;
  case CIT_SLIDER: {
    // only 1 slider per config item
    ASSERT(m_sCount == 0);
    // we need 1 slider
    // we need 1 ID
    m_sCount = 1;
    m_sList = (NewUISlider **)mem_malloc(sizeof(NewUISlider *) * 1);
    m_iNumIDs = 1;
    m_iID = (int *)mem_malloc(sizeof(int) * 1);
    // make sure mallocs are ok
    ASSERT((m_sList) && (m_iID));
    // get unique ID
    m_iID[0] = GetUniqueID();
    // allocate slider
    m_sList[0] = new NewUISlider;
    // make sure allocation ok
    ASSERT(m_sList[0]);
    m_sList[0]->Create(m_hWnd, m_iID[0], m_X, m_Y, UIF_FIT);
    char temp[10];
    switch (m_iValueType) {
    case CIV_INT:
      snprintf(temp, sizeof(temp), "%d", m_iInitial);
      if (m_iFlags & CIF_PERCENT)
        strncat(temp, "%", sizeof(temp) - strlen(temp) - 1);
      break;
    case CIV_FLOAT:
      if (m_iFlags & CIF_NODECIMAL)
        snprintf(temp, sizeof(temp), "%.0f", m_fInitial);
      else
        snprintf(temp, sizeof(temp), "%.2f", m_fInitial);
      if (m_iFlags & CIF_PERCENT)
        strncat(temp, "%", sizeof(temp) - strlen(temp) - 1);
      break;
    }
    UITextItem item{temp, UICOL_TEXT_NORMAL};
    m_tLabel2.Create(m_hWnd, &item, m_X + 155, m_Y + 7, UIF_FIT);
    m_sList[0]->SetSelectChangeCallback(CISliderCallback, this);
    break;
  }
  case CIT_ONOFFBUTTON:
    // only 1 on/off button per config item
    ASSERT(m_bCount == 0);
    // we need 2 text items ("On" and "Off")
    // we need 1 button
    // we need 1 ID
    m_bCount = 1;
    m_bList = (NewUIButton **)mem_malloc(sizeof(NewUIButton *) * 1);
    m_tiCount = 2;
    m_tiList = (UITextItem **)mem_malloc(sizeof(UITextItem *) * 2);
    m_iNumIDs = 1;
    m_iID = (int *)mem_malloc(sizeof(int) * 1);
    // make sure mallocs mallocs are ok
    ASSERT((m_tiList) && (m_bList) && (m_iID));
    // adjust the curr x/y for the button
    curr_x = 0;
    curr_y = 0;
    // get unique ID
    m_iID[0] = GetUniqueID();
    // allocate text items
    m_tiList[0] = new UITextItem(TXT_OFF, UICOL_BUTTON_LO);
    m_tiList[1] = new UITextItem(TXT_ON, UICOL_BUTTON_HI);
    // make sure allocation ok
    ASSERT((m_tiList[0]) && (m_tiList[1]));
    // allocate buttonCreate
    m_bList[0] = new NewUIButton;
    // make sure allocation ok
    ASSERT(m_bList[0]);
    m_bList[0]->Create(m_hWnd, m_iID[0], m_tiList[0], m_X + curr_x, m_Y + curr_y, m_tiList[0]->width(),
                       m_tiList[0]->height(), UIF_FIT);
    // Initialize Config Item to initial value
    ASSERT(m_iValueType == CIV_BOOL);
    if (m_bInitial)
      m_bList[0]->SetTitle(m_tiList[1]); // On
    else
      m_bList[0]->SetTitle(m_tiList[0]); // Off
    break;
  case CIT_LISTBOX:
    ASSERT(count > 0);
    ASSERT(m_lbCount == 0);
    // we need count text items (one for each item in list box)
    // we need 1 listbox
    // we need 1 ID
    m_lbCount = 1;
    m_lbList = (NewUIListBox **)mem_malloc(sizeof(NewUIListBox *) * 1);
    m_tiCount = count;
    m_tiList = (UITextItem **)mem_malloc(sizeof(UITextItem *) * m_tiCount);
    m_iNumIDs = 1;
    m_iID = (int *)mem_malloc(sizeof(int) * 1);
    // make sure mallocs ok
    ASSERT((m_lbList) && (m_tiList) && (m_iID));
    // adjust the curr x/y for the listbox
    curr_x = 0;
    curr_y = m_tLabel.H() + LISTBOX_LABELGAP;
    // get unique ID
    m_iID[0] = GetUniqueID();
    // allocate list box
    m_lbList[0] = new NewUIListBox;
    // check allocation
    ASSERT(m_lbList[0]);
    m_lbList[0]->Create(m_hWnd, m_iID[0], m_X + curr_x, m_Y + curr_y, m_W, m_H, UIF_FIT);
    // allocate text items and put them in
    for (i = 0; i < m_tiCount; i++) {
      // create the text item
      m_tiList[i] = new UITextItem;
      // make sure allocation ok
      ASSERT(m_tiList[i]);
      (*m_tiList[i]) = (*(va_arg(marker, UITextItem *)));
      // add textitem to listbox
      m_lbList[0]->AddItem(m_tiList[i]);
    }
    // Initialize Config Item to initial value
    ASSERT(m_iValueType == CIV_INT);
    ASSERT((m_iInitial >= 0) && (m_iInitial < count));
    m_lbList[0]->SetSelectChangeCallback(CIListBoxCallback, this);
    m_lbList[0]->SetSelectedIndex(m_iInitial);
    break;
  case CIT_YESNOBUTTON:
    // only 1 yes/no button per config item
    ASSERT(m_bCount == 0);
    // we need 2 text items ("Yes" and "No")
    // we need 1 button
    // we need 1 ID
    m_bCount = 1;
    m_bList = (NewUIButton **)mem_malloc(sizeof(NewUIButton *) * 1);
    m_tiCount = 2;
    m_tiList = (UITextItem **)mem_malloc(sizeof(UITextItem *) * 2);
    m_iNumIDs = 1;
    m_iID = (int *)mem_malloc(sizeof(int) * 1);
    // make sure mallocs mallocs are ok
    ASSERT((m_tiList) && (m_bList) && (m_iID));
    // adjust the curr x/y for the button
    curr_x = 0;
    curr_y = 0;
    // get unique ID
    m_iID[0] = GetUniqueID();
    // allocate text items
    m_tiList[0] = new UITextItem(TXT_NO, UICOL_BUTTON_LO);
    m_tiList[1] = new UITextItem(TXT_YES, UICOL_BUTTON_HI);
    // make sure allocation ok
    ASSERT((m_tiList[0]) && (m_tiList[1]));
    // allocate buttonCreate
    m_bList[0] = new NewUIButton;
    // make sure allocation ok
    ASSERT(m_bList[0]);
    m_bList[0]->Create(m_hWnd, m_iID[0], m_tiList[0], m_X + curr_x, m_Y + curr_y, m_tiList[0]->width(),
                       m_tiList[0]->height(), UIF_FIT);
    // Initialize Config Item to initial value
    ASSERT(m_iValueType == CIV_BOOL);
    if (m_bInitial)
      m_bList[0]->SetTitle(m_tiList[1]); // Yes
    else
      m_bList[0]->SetTitle(m_tiList[0]); // No
    break;
  case CIT_CHECKBOX:
    // only 1 checkbox per config item
    ASSERT(m_bCount == 0);
    // we need 4 text items (a checked and unchecked box hightlited and not)
    // we need 1 hotspot
    // we need 1 ID
    m_hsCount = 1;
    m_hsList = (UIHotspot **)mem_malloc(sizeof(UIHotspot *) * 1);
    m_tiCount = 4;
    m_tiList = (UITextItem **)mem_malloc(sizeof(UITextItem *) * 4);
    m_iNumIDs = 1;
    m_iID = (int *)mem_malloc(sizeof(int) * 1);
    // make sure mallocs are ok
    ASSERT((m_hsList) && (m_tiList) && (m_iID));
    // adjust the curr x/y for the checkbox
    curr_x = 0;
    curr_y = 0;
    // get unique ID
    m_iID[0] = GetUniqueID();
    // allocate text items
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "%c %s", UI_CHECKBOX_OFF_CHAR, ((UITextItem *)m_tLabel.GetItem())->GetBuffer());
    m_tiList[0] = new UITextItem(buffer, UICOL_HOTSPOT_LO);
    m_tiList[1] = new UITextItem(buffer, UICOL_HOTSPOT_HI);
    snprintf(buffer, sizeof(buffer), "%c %s", UI_CHECKBOX_ON_CHAR, ((UITextItem *)m_tLabel.GetItem())->GetBuffer());
    m_tiList[2] = new UITextItem(buffer, UICOL_HOTSPOT_LO);
    m_tiList[3] = new UITextItem(buffer, UICOL_HOTSPOT_HI);
    // make sure allocation ok
    ASSERT((m_tiList[0]) && (m_tiList[1]));
    // allocate hot spots
    m_hsList[0] = new UIHotspot;
    // make sure allocation ok
    ASSERT(m_hsList[0]);
    m_hsList[0]->Create(m_hWnd, m_iID[0], -1, m_tiList[0], m_tiList[1], m_X + curr_x, m_Y + curr_y,
                        m_tiList[0]->width(), m_tiList[0]->height(), UIF_FIT);
    // Initialize Config Item to initial value
    ASSERT(m_iValueType == CIV_BOOL);
    if (m_bInitial)
      m_hsList[0]->SetStates(m_tiList[2], m_tiList[3]);
    else
      m_hsList[0]->SetStates(m_tiList[0], m_tiList[1]);
    // remove the title so it's not drawn
    m_tLabel.Destroy();
    break;
  case CIT_RADIOBUTTON:
    ASSERT(count > 0);
    ASSERT(m_rbCount == 0);
    // we need count text items
    // we need count radio buttons
    // we need count IDs
    m_tiCount = count;
    m_tiList = (UITextItem **)mem_malloc(sizeof(UITextItem *) * m_tiCount);
    m_rbCount = count;
    m_rbList = (UIRadioButton **)mem_malloc(sizeof(UIRadioButton *) * m_rbCount);
    m_iNumIDs = count;
    m_iID = (int *)mem_malloc(sizeof(int) * m_iNumIDs);
    // make sure mallocs are ok
    ASSERT((m_tiList) && (m_rbList) && (m_iID));
    // adjust the curr x/y for the list of radio buttons
    curr_x = 0;

    int offset;
    bool use_group;
    use_group = (bool)((m_iFlags & CIF_USEGROUP) != 0);

    if (m_X) {
      curr_y = 0;
      offset = 8;
    } else {
      m_X = m_auxX;
      curr_y = RADIOBUTTON_LABELGAP + 10;

      if (use_group) {
        offset = 5;
      } else {
        offset = 0;
      }
    }

    int start_y, w, max_width;

    start_y = curr_y;
    max_width = 0;

    for (i = 0; i < count; i++) {
      // get unique ID
      m_iID[i] = GetUniqueID();
      // allocate text items
      m_tiList[i] = new UITextItem;
      // make sure allocation ok
      ASSERT(m_tiList[i]);
      (*m_tiList[i]) = (*(va_arg(marker, UITextItem *)));

      m_tiList[i]->set_color(UICOL_HOTSPOT_LO);

      // m_tiList[i]->set_alpha(UIALPHA_HOTSPOT_LO);
      // allocate radio button
      m_rbList[i] = new UIRadioButton;
      ASSERT(m_rbList[i]);
      if (i)
        m_rbList[i]->Create(m_hWnd, m_rbList[i - 1], m_iID[i], m_tiList[i], m_X + curr_x + offset, m_Y + curr_y, m_W,
                            m_H, UIF_FIT);
      else
        m_rbList[i]->Create(m_hWnd, NULL, m_iID[i], m_tiList[i], m_X + curr_x + offset, m_Y + curr_y, m_W, m_H,
                            UIF_FIT);

      // store the max width
      w = m_rbList[i]->W();
      if (w > max_width)
        max_width = w;

      // adjust curr x/y
      curr_y += (m_tiList[i]->height() + RADIOBUTTON_GAP);
    }

    if (m_iFlags & CIF_USEGROUP && m_labeltext) {
      // create the group box now

      // we may need to update the width depending on width of title
      grtext_SetFont(SMALL_FONT);
      int w = grtext_GetTextLineWidth(m_labeltext);
      if (w > max_width)
        max_width = w;

      m_H = curr_y - start_y + 15; // generate real height
      m_W = max_width + 20;        // generate real width

      // Create the group box
      m_gGroup.Create(m_hWnd, m_labeltext, m_auxX, m_Y, m_W, m_H, UICOL_TEXT_NORMAL, UICOL_HOTSPOT_LO);
    }

    // Initialize Config Item to initial value
    ASSERT(m_iValueType == CIV_INT);
    ASSERT((m_iInitial >= 0) && (m_iInitial < m_iNumIDs));
    m_rbList[m_iInitial]->Activate();
    break;
  default:
    mprintf(0, "Bad ConfigItem Type in Add\n");
  }
  va_end(marker);
}
void ConfigItem::SetRange(int min, int max, int range) {
  if (!m_bAlive)
    return;
  ASSERT(m_iType == CIT_SLIDER);
  ASSERT(m_iValueType == CIV_INT);
  ASSERT(max > min);
  ASSERT((m_iCurrentValue >= min) && (m_iCurrentValue <= max));

  m_fMin = min;
  m_fMax = max;
  m_sList[0]->SetRange(range + 1);
  m_fUnit = ((float)max - min) / ((float)range);

  // figure out how far in is the position
  m_sList[0]->SetPos((int)(((float)(m_iInitial - min)) / m_fUnit));
}

void ConfigItem::SetRange(float min, float max, int range) {
  if (!m_bAlive)
    return;
  ASSERT(m_iType == CIT_SLIDER);
  ASSERT(m_iValueType == CIV_FLOAT);
  ASSERT(max > min);
  ASSERT((m_fCurrentValue >= min) && (m_fCurrentValue <= max));

  m_fMin = min;
  m_fMax = max;
  m_sList[0]->SetRange(range + 1);
  m_fUnit = (max - min) / ((float)range);

  // figure out the real positions
  m_sList[0]->SetPos((int)((m_fInitial - min) / m_fUnit));
}

void ConfigItem::SetCallback(void (*callback)(float)) {
  if (!m_bAlive)
    return;
  if (m_iValueType != CIV_FLOAT)
    return;
  m_fCallback = callback;
}

void ConfigItem::SetCallback(void (*callback)(int)) {
  if (!m_bAlive)
    return;
  if (m_iValueType != CIV_INT)
    return;
  m_iCallback = callback;
}

void ConfigItem::SetCallback(void (*callback)(bool)) {
  if (!m_bAlive)
    return;
  if (m_iValueType != CIV_BOOL)
    return;
  m_bCallback = callback;
}

int GetUniqueID(void) {
  static int count = 0xB0;
  count++;
  return count;
}

void CIListBoxCallback(int ID, void *ptr) {}

void CISliderCallback(int ID, void *ptr) {
  ConfigItem *ci = (ConfigItem *)ptr;
  if (!ci)
    return;
  ci->Update(ID);
}
