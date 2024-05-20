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

// ScriptSelect.cpp : implementation file
//

#include "stdafx.h"
#include "editor.h"

#include "ScriptSelect.h"
#include "AddScriptDialog.h"
#include "ScriptStudio.h"

#include "pserror.h"
#include "d3x.h"
#include "ObjCScript.h"
#include "ScriptWizard.h"
#include "ScriptParmDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CScriptSelect dialog


CScriptSelect::CScriptSelect(uint16_t scr_type, int custom_type, CWnd* pParent /*=NULL*/)
	: CDialog(CScriptSelect::IDD, pParent)
{
	//{{AFX_DATA_INIT(CScriptSelect)
	m_ScriptName = _T("");
	//}}AFX_DATA_INIT

	m_ScriptCode = NULL;
	m_CurScriptSel = -1;
	m_ScriptType = (uint16_t)scr_type;
	m_NumParms = 0;
	m_CustomType = custom_type;
}


void CScriptSelect::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CScriptSelect)
	DDX_LBString(pDX, IDC_SCRIPTSEL_LISTBOX, m_ScriptName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CScriptSelect, CDialog)
	//{{AFX_MSG_MAP(CScriptSelect)
	ON_LBN_DBLCLK(IDC_SCRIPTSEL_LISTBOX, OnDblclkScriptselListbox)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_PARAMETERS, OnParameters)
	ON_LBN_SELCHANGE(IDC_SCRIPTSEL_LISTBOX, OnSelchangeScriptselListbox)
	ON_BN_CLICKED(IDC_SCRIPTSEL_ADD, OnScriptselAdd)
	ON_BN_CLICKED(IDC_EDITSCRIPT, OnEditScript)
	ON_BN_CLICKED(IDC_SCRTYPE, OnScrtype)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScriptSelect message handlers

BOOL CScriptSelect::OnInitDialog() 
{
	CListBox *scrlistbox = (CListBox *)GetDlgItem(IDC_SCRIPTSEL_LISTBOX);

	CDialog::OnInitDialog();

//	update names of scripts into listbox
	m_ScriptCode = D3XReallocProgram(NULL, 0, 0, 0);
	UpdateScriptListbox();
	if (m_ScriptName.IsEmpty()) 
		scrlistbox->SelectString(-1, "null");
	else
		scrlistbox->SelectString(-1, m_ScriptName);
	OnSelchangeScriptselListbox() ;

//	update script list selector
	CButton *btn = (CButton *)GetDlgItem(IDC_SCRTYPE);

	if (m_CustomType & DEFAULT_SCRIPT_MASK) {
		btn->SetWindowText("Custom List");
	}
	else if (m_CustomType & CUSTOM_SCRIPT_MASK) {
		btn->SetWindowText("Default List");
	}
	else {
		btn->EnableWindow(FALSE);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CScriptSelect::OnDestroy() 
{
	CDialog::OnDestroy();
	
	if (m_ScriptCode) D3XFreeProgram(m_ScriptCode);	
}


//////////////////////////////////////////////////////////////////////////////
//	CONTROL HANDLERS

void CScriptSelect::OnOK() 
{
//	DDX takes care of returning the script name selected

	CDialog::OnOK();
}
																								 

void CScriptSelect::OnDblclkScriptselListbox() 
{
	OnOK();
}


//@@void CScriptSelect::OnScriptselScriptWiz() 
//@@{
//@@	CListBox *scrlistbox = (CListBox *)GetDlgItem(IDC_SCRIPTSEL_LISTBOX);
//@@	CString str;
//@@	int sel;
//@@
//@@	sel = scrlistbox->GetCurSel();
//@@	if (sel != LB_ERR) {
//@@		scrlistbox->GetText(sel, str);
//@@	}
//@@
//@@	CScriptWizard dlg;
//@@	dlg.DoModal();
//@@	UpdateScriptListbox();
//@@
//@@	if (sel != LB_ERR) 
//@@		scrlistbox->SelectString(-1, str);
//@@}


void CScriptSelect::OnSelchangeScriptselListbox() 
{
	CListBox *scrlistbox = (CListBox *)GetDlgItem(IDC_SCRIPTSEL_LISTBOX);
	CButton *btn = (CButton *)GetDlgItem(IDC_PARAMETERS);
	CButton *btn2 = (CButton *)GetDlgItem(IDC_EDITSCRIPT);
	CString str;
	int sel, id;

	sel = scrlistbox->GetCurSel();
	if (sel != LB_ERR) {
		scrlistbox->GetText(sel, str);
		id = FindScriptIDFromName(str);
		if (id > -1) {
			btn2->EnableWindow(TRUE);
			m_NumParms = Script_names[id].nparms;
			if (Script_names[id].nparms) {
				btn->EnableWindow(TRUE);
				return;
			}
		}
		else {
			m_NumParms = 0;
			btn2->EnableWindow(FALSE);
		}
	}
	btn->EnableWindow(FALSE);
}


void CScriptSelect::OnParameters() 
{
//	search list of script names for match with listbox.   Then use this index
//	to look in the m_SciptCode program to see if that script has parameters
	CListBox *scrlistbox = (CListBox *)GetDlgItem(IDC_SCRIPTSEL_LISTBOX);
	CString str;
	int sel, id;

	sel = scrlistbox->GetCurSel();
	if (sel != LB_ERR) {
		scrlistbox->GetText(sel, str);
		id = FindScriptIDFromName(str);
		if (id > -1) {
			CScriptParmDialog dlg;

		//	grab parameter information.
			SETUP_SCRIPTPARM_DIALOG(&dlg, Script_names[id].parameters, m_Parm);
		
			if (dlg.DoModal() == IDOK) {
				RETURN_SCRIPTPARM_DIALOG(m_Parm, &dlg, Script_names[id].paramtypes);
			}
		}
		else {
			Int3();
		}
	}
}


//////////////////////////////////////////////////////////////////////////////
//	INTERNAL FUNCTIONS

void CScriptSelect::UpdateScriptListbox()
{
	CListBox *scrlistbox = (CListBox *)GetDlgItem(IDC_SCRIPTSEL_LISTBOX);
	int i;

	GenerateScriptListFromAllFiles(m_CustomType);

//	Add script names to list box
	scrlistbox->ResetContent();
	for (i = 0; i < MAX_SCRIPTS; i++)
	{
		if (Script_names[i].used)
			if (m_ScriptType == 0xffff || Script_names[i].type == m_ScriptType) 
				if ((Script_names[i].iscustom && (m_CustomType & CUSTOM_SCRIPT_MASK)) || 
					(!Script_names[i].iscustom && (m_CustomType & DEFAULT_SCRIPT_MASK)))
					scrlistbox->AddString(Script_names[i].name);
	}

	scrlistbox->AddString("null");

//	make sure we reselect the current string.
	if (scrlistbox->GetCount() > 0) 
		scrlistbox->SetCurSel(0);
}




void CScriptSelect::OnScriptselAdd() 
{
	CAddScriptDialog dlg(this);

//	in this case, load the script source file level.scr and add to it.
	if (dlg.DoModal()==IDOK) {
		char *source;

		source = LoadScript(LEVEL_SCRIPT_NAME);
		if (!source) {
			OutrageMessageBox("Unable to find script for level. Something is wrong!");
			return;
		}
		source = AddScriptBlockToScript(source, (LPCSTR)dlg.m_Name, (LPCSTR)dlg.m_TypeName);
		SaveScript(LEVEL_SCRIPT_NAME, source);
		FreeScript(source);
		EditScript(LEVEL_SCRIPT_NAME, dlg.m_Name);
	}
}


void CScriptSelect::OnEditScript() 
{
	CListBox *scrlistbox = (CListBox *)GetDlgItem(IDC_SCRIPTSEL_LISTBOX);
	CString str;
	int sel, id;

	sel = scrlistbox->GetCurSel();
	if (sel != LB_ERR) {
		scrlistbox->GetText(sel, str);
		id = FindScriptIDFromName(str);
		if (id > -1) 
			EditScript(Script_names[id].iscustom ? LEVEL_SCRIPT_NAME : DEFAULT_SCRIPT_NAME, str);
	}
}


void CScriptSelect::EditScript(const char *filename, const char *scrname)
{
	if (InvokeScriptStudio(filename, scrname)) 
		UpdateScriptListbox();
}


void CScriptSelect::OnScrtype() 
{
	CListBox *scrlistbox = (CListBox *)GetDlgItem(IDC_SCRIPTSEL_LISTBOX);
	CButton *btn = (CButton *)GetDlgItem(IDC_SCRTYPE);

	if (m_CustomType & DEFAULT_SCRIPT_MASK) {
		m_CustomType = CUSTOM_SCRIPT_MASK;
		btn->SetWindowText("Default List");
	}
	else if (m_CustomType & CUSTOM_SCRIPT_MASK) {
		m_CustomType = DEFAULT_SCRIPT_MASK;
		btn->SetWindowText("Custom List");
	}

	UpdateScriptListbox();

	if (m_ScriptName.IsEmpty()) 
		scrlistbox->SelectString(-1, "null");
	else
		scrlistbox->SelectString(-1, m_ScriptName);
}
