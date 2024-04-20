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

// DallasUserTypesDlg.cpp : implementation file
//

#include "stdafx.h"
#include "cfile.h"

#ifdef NEWEDITOR
#include "NewEditor.h"
#else
#include "editor.h"
#endif

#include "DallasMainDlg.h"
#include "DallasGenericPromptDlg.h"
#include "DallasUserTypesDlg.h"

CDallasMainDlg *GetDallasDialogPtr(void);


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CString last_type_selected;

/////////////////////////////////////////////////////////////////////////////
// CDallasUserTypesDlg dialog


CDallasUserTypesDlg::CDallasUserTypesDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDallasUserTypesDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDallasUserTypesDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDallasUserTypesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDallasUserTypesDlg)
	DDX_Control(pDX, IDC_VALUES_LIST, m_ValuesListBox);
	DDX_Control(pDX, IDC_UTYPE_COMBO, m_UserTypeCombo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDallasUserTypesDlg, CDialog)
	//{{AFX_MSG_MAP(CDallasUserTypesDlg)
	ON_BN_CLICKED(IDC_ADD_BUTTON, OnAddButton)
	ON_BN_CLICKED(IDC_CHANGE_BUTTON, OnChangeButton)
	ON_BN_CLICKED(IDC_DELETE_BUTTON, OnDeleteButton)
	ON_CBN_SELCHANGE(IDC_UTYPE_COMBO, OnSelchangeUtypeCombo)
	ON_LBN_DBLCLK(IDC_VALUES_LIST, OnDblclkValuesList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDallasUserTypesDlg message handlers

BOOL CDallasUserTypesDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CDallasMainDlg *m_DallasModelessDlgPtr;
	m_DallasModelessDlgPtr = GetDallasDialogPtr();

	// If Dallas is up, fill in the user type and value boxes
	if(m_DallasModelessDlgPtr!=NULL) {
		m_DallasModelessDlgPtr->FillUserTypeBox(&m_UserTypeCombo);

		if(!last_type_selected.IsEmpty()) {
			m_UserTypeCombo.SelectString(-1,last_type_selected.GetBuffer(0));
		}

		OnSelchangeUtypeCombo();
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDallasUserTypesDlg::OnAddButton() 
{
	CDallasGenericPromptDlg dlg;
	int index;
	CString name;

	CDallasMainDlg *m_DallasModelessDlgPtr;
	m_DallasModelessDlgPtr = GetDallasDialogPtr();
	
	if(m_DallasModelessDlgPtr==NULL) return;

	// Get the name of the user type
	index=m_UserTypeCombo.GetCurSel();
	if(index==CB_ERR) return;

	m_UserTypeCombo.GetLBText(index,name);
	if(name.IsEmpty()) return;

	// Display the prompt dialog
	dlg.m_DialogTitle="Value Name Prompt";
	dlg.m_PromptText="Enter a name for the new value:";
	dlg.m_PromptData="";
	dlg.m_MaxDataLength=MAX_MESSAGE_NAME_LEN;

	if(dlg.DoModal()==IDCANCEL) return;

	// Check if the message name is valid
	char *valname=dlg.m_PromptData.GetBuffer(0);
	if(strlen(valname)==0) return;
	for(unsigned int j=0;j<strlen(valname);j++)
		if(!isalnum(valname[j]) && valname[j]!='_' && valname[j]!=' ') {
			MessageBox("That name is invalid!\n\nA value name may only contain letters and numbers","Invalid Name Error",MB_OK|MB_ICONEXCLAMATION);
			return;
		}

	// Check if the message name already exists in the list
	int pos;
	if(m_DallasModelessDlgPtr->GetEnumValue(name.GetBuffer(0),valname,pos)) {
		MessageBox("That value name is already in use!\n\nYou must enter a UNIQUE name.","Invalid Value Name",MB_OK|MB_ICONEXCLAMATION);
		return;
	}

	// Add the value to the enum database
	pos=m_DallasModelessDlgPtr->AddUserTypeValue(name.GetBuffer(0),valname);
	if(pos==0) return;
	if(pos==-1) {
		MessageBox("The value could not be added.\n\nThe list may be full.","Value Not Added",MB_OK|MB_ICONEXCLAMATION);
		return;
	}

	// Add the value to the list
	m_ValuesListBox.InsertString(pos-1,valname);
}

void CDallasUserTypesDlg::OnChangeButton() 
{
	CDallasGenericPromptDlg dlg;
	int index;
	CString type_name, value_name;
	
	CDallasMainDlg *m_DallasModelessDlgPtr;
	m_DallasModelessDlgPtr = GetDallasDialogPtr();

	if(m_DallasModelessDlgPtr==NULL) return;

	// Get the name of the user type
	index=m_UserTypeCombo.GetCurSel();
	if(index==CB_ERR) return;

	m_UserTypeCombo.GetLBText(index,type_name);
	if(type_name.IsEmpty()) return;

	// Get the name of the selected value
	index=m_ValuesListBox.GetCurSel();
	if(index==LB_ERR) return;

	m_ValuesListBox.GetText(index,value_name);
	if(value_name.IsEmpty()) return;

	// Display the prompt dialog
	dlg.m_DialogTitle="Value Name Prompt";
	dlg.m_PromptText="Enter a new name for this value:";
	dlg.m_PromptData=value_name;
	dlg.m_MaxDataLength=MAX_MESSAGE_NAME_LEN;

	if(dlg.DoModal()==IDCANCEL) return;

	// Check if the value name is valid
	char *valname=dlg.m_PromptData.GetBuffer(0);
	if(strlen(valname)==0) return;
	for(unsigned int j=0;j<strlen(valname);j++)
		if(!isalnum(valname[j]) && valname[j]!='_' && valname[j]!=' ') {
			MessageBox("That name is invalid!\n\nA value name may only contain letters and numbers","Invalid Name Error",MB_OK|MB_ICONEXCLAMATION);
			return;
		}

	// Check if the value name already exists in the list
	int pos;
	if(m_DallasModelessDlgPtr->GetEnumValue(type_name.GetBuffer(0),valname,pos) && strcmp(valname,value_name.GetBuffer(0))!=0) {
		MessageBox("That value name is already in use!\n\nYou must enter a UNIQUE name.","Invalid Value Name",MB_OK|MB_ICONEXCLAMATION);
		return;
	}

	// Change the name of the value
	if(m_DallasModelessDlgPtr->ChangeValueName(type_name.GetBuffer(0),value_name.GetBuffer(0),valname)) {
		m_ValuesListBox.DeleteString(index);
		m_ValuesListBox.InsertString(index,valname);
		m_ValuesListBox.SetCurSel(index);
	}
}

void CDallasUserTypesDlg::OnDeleteButton() 
{
	int index;
	CString type_name, value_name;

	CDallasMainDlg *m_DallasModelessDlgPtr;
	m_DallasModelessDlgPtr = GetDallasDialogPtr();

	if(m_DallasModelessDlgPtr==NULL) return;

	// Get the name of the user type
	index=m_UserTypeCombo.GetCurSel();
	if(index==CB_ERR) return;

	m_UserTypeCombo.GetLBText(index,type_name);
	if(type_name.IsEmpty()) return;

	// Get the name of the selected value
	index=m_ValuesListBox.GetCurSel();
	if(index==LB_ERR) return;

	m_ValuesListBox.GetText(index,value_name);
	if(value_name.IsEmpty()) return;

	if(m_DallasModelessDlgPtr->DeleteUserTypeValue(type_name.GetBuffer(0),value_name.GetBuffer(0))) {
		m_ValuesListBox.DeleteString(index);
	}
}

void CDallasUserTypesDlg::OnOK() 
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
}

void CDallasUserTypesDlg::OnSelchangeUtypeCombo() 
{
	int index;
	CString name;

	CDallasMainDlg *m_DallasModelessDlgPtr;
	m_DallasModelessDlgPtr = GetDallasDialogPtr();

	index=m_UserTypeCombo.GetCurSel();
	if(index==CB_ERR) return;

	m_UserTypeCombo.GetLBText(index,name);
	if(name.IsEmpty()) return;

	last_type_selected=name;

	// If Dallas is up, fill in the user type and value boxes
	if(m_DallasModelessDlgPtr!=NULL) {
		m_DallasModelessDlgPtr->FillValuesBox(&m_ValuesListBox,name.GetBuffer(0));
	}	
}

void CDallasUserTypesDlg::OnDblclkValuesList() 
{
	OnChangeButton();
}
