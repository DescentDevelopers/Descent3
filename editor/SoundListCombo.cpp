// SoundListCombo.cpp : implementation file
//

#include "stdafx.h"
#include "editor.h"
#include "SoundListCombo.h"
#include "soundload.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSoundListCombo

CSoundListCombo::CSoundListCombo()
{
}

CSoundListCombo::~CSoundListCombo()
{
}


BEGIN_MESSAGE_MAP(CSoundListCombo, CComboBox)
	//{{AFX_MSG_MAP(CSoundListCombo)
	ON_CONTROL_REFLECT(CBN_SETFOCUS, OnSetfocus)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSoundListCombo message handlers

void CSoundListCombo::Init(int selected) 
{
	//Clear the combo box
	ResetContent();

	//Add the "none" sound
	AddString("<none>");
	SetItemData(0,-1);
	SetCurSel(0);		//default
	
	//Now add all the sounsd
	for (int i=0;i<MAX_SOUNDS;i++) {
		if (Sounds[i].used) {
			int index;

			index = AddString(Sounds[i].name);
			SetItemData(index,i);
			if (selected == i)
				SetCurSel(index);
		}
	}
}

void CSoundListCombo::SetSelected(int selected) 
{
	//Now add all the sounsd
	int count = GetCount();
	for (int i=0;i<count;i++) {
		if (selected == (int) GetItemData(i))
			SetCurSel(i);
	}
}

int CSoundListCombo::GetSelected() 
{
	return (int) GetItemData(GetCurSel());
}

#include "mono.h"

void CSoundListCombo::OnSetfocus() 
{
	//Update list in case the items have changed
	Init(GetSelected());
}
