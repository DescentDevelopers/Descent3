// TriggerListCombo.cpp : implementation file
//

#include "stdafx.h"

#ifndef NEWEDITOR
#include "editor.h"
#else
#include "neweditor.h"
#endif

#include "TriggerListCombo.h"
#include "trigger.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTriggerListCombo

CTriggerListCombo::CTriggerListCombo()
{
}

CTriggerListCombo::~CTriggerListCombo()
{
}


BEGIN_MESSAGE_MAP(CTriggerListCombo, CComboBox)
	//{{AFX_MSG_MAP(CTriggerListCombo)
	ON_CONTROL_REFLECT(CBN_SETFOCUS, OnSetfocus)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTriggerListCombo message handlers

void CTriggerListCombo::Init(int selected) 
{
	//Clear the combo box
	ResetContent();

	//Add the "none" sound
	AddString("<none>");
	SetItemData(0,-1);
	SetCurSel(0);		//default
	
	//Now add all the sounsd
	for (int i=0;i<Num_triggers;i++) {
		int index = AddString(Triggers[i].name);
		SetItemData(index,i);
		if (selected == i)
			SetCurSel(index);
	}
}

void CTriggerListCombo::SetSelected(int selected) 
{
	//Now add all the sounsd
	int count = GetCount();
	for (int i=0;i<count;i++) {
		if (selected == (int) GetItemData(i))
			SetCurSel(i);
	}
}

int CTriggerListCombo::GetSelected() 
{
	return (int) GetItemData(GetCurSel());
}


void CTriggerListCombo::OnSetfocus() 
{
	//Update list in case the items have changed
	Init(GetSelected());
}
