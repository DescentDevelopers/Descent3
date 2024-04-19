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

CRoomListCombo::CRoomListCombo()
{
}

CRoomListCombo::~CRoomListCombo()
{
}


BEGIN_MESSAGE_MAP(CRoomListCombo, CComboBox)
	//{{AFX_MSG_MAP(CRoomListCombo)
	ON_CONTROL_REFLECT(CBN_SETFOCUS, OnSetfocus)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRoomListCombo message handlers


void CRoomListCombo::Init(int selected) 
{
	//Clear the combo box
	ResetContent();

	//Add the "none" sound
	AddString("<none>");
	SetItemData(0,-1);
	SetCurSel(0);		//default
	
	//Now add all the sounsd
	for (int i=0;i<=Highest_room_index;i++) {
		if (Rooms[i].used && Rooms[i].name) {
			int index = AddString(Rooms[i].name);
			SetItemData(index,i);
			if (selected == i)
				SetCurSel(index);
		}
	}
}

void CRoomListCombo::SetSelected(int selected) 
{
	//Now add all the sounsd
	int count = GetCount();
	for (int i=0;i<count;i++) {
		if (selected == (int) GetItemData(i))
			SetCurSel(i);
	}
}

int CRoomListCombo::GetSelected() 
{
	return (int) GetItemData(GetCurSel());
}


void CRoomListCombo::OnSetfocus() 
{
	//Update list in case the items have changed
	Init(GetSelected());
}
