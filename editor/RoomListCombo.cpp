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
 */

// RoomListCombo.cpp : implementation file
//

#include "stdafx.h"

#ifndef NEWEDITOR
#include "editor.h"
#else
#include "neweditor.h"
#endif

#include "RoomListCombo.h"
#include "room.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRoomListCombo

CRoomListCombo::CRoomListCombo() {}

CRoomListCombo::~CRoomListCombo() {}

BEGIN_MESSAGE_MAP(CRoomListCombo, CComboBox)
//{{AFX_MSG_MAP(CRoomListCombo)
ON_CONTROL_REFLECT(CBN_SETFOCUS, OnSetfocus)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRoomListCombo message handlers

void CRoomListCombo::Init(int selected) {
  // Clear the combo box
  ResetContent();

  // Add the "none" sound
  AddString("<none>");
  SetItemData(0, -1);
  SetCurSel(0); // default

  // Now add all the sounsd
  for (int i = 0; i <= Highest_room_index; i++) {
    if (Rooms[i].used && Rooms[i].name) {
      int index = AddString(Rooms[i].name);
      SetItemData(index, i);
      if (selected == i)
        SetCurSel(index);
    }
  }
}

void CRoomListCombo::SetSelected(int selected) {
  // Now add all the sounsd
  int count = GetCount();
  for (int i = 0; i < count; i++) {
    if (selected == (int)GetItemData(i))
      SetCurSel(i);
  }
}

int CRoomListCombo::GetSelected() { return (int)GetItemData(GetCurSel()); }

void CRoomListCombo::OnSetfocus() {
  // Update list in case the items have changed
  Init(GetSelected());
}
