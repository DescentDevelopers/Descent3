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

 * $Logfile: /DescentIII/Main/D3Launch/KeyboardTab.cpp $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:56:51 $
 * $Author: kevinb $
 *
 * 
 *
 * $Log: not supported by cvs2svn $
 * 
 * 11    6/09/99 2:46p Nate
 * Minor changes for different builds
 * 
 * 10    5/23/99 1:21a Nate
 * Made FindFast Disabling default to "always"
 * 
 * 9     5/22/99 1:39a Nate
 * Added Find Fast support
 * 
 * 8     5/07/99 11:24a Nate
 * Added support for a launcher config file
 * 
 * 7     5/05/99 11:38p Nate
 * Disabled Language selection for US release builds
 * 
 * 6     2/24/99 1:46p Nate
 * Added multi-language support
 * 
 * 5     10/13/98 3:03p Nate
 * More fixes and changes.
 * 
 * 4     9/22/98 3:33p Nate
 * Added conditional compiling to help system (can toggle between HTML and
 * standard windows help)
 * 
 * 3     9/21/98 5:40p Nate
 * Incorporated the new HTML help system
 * 
 * 2     8/17/98 10:35a Nate
 * Added Keyboard Type page to Setup
 *
 * $NoKeywords: $
 */

// KeyboardTab.cpp : implementation file
//

#include "stdafx.h"
#include "afxpriv.h"
#include "afxext.h"
#include "d3launch.h"
#include "KeyboardTab.h"
#include "OS_Config.h"

#include "LaunchNames.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define KEYBOARD_US			"United States"
#define KEYBOARD_FRENCH		"French"
#define KEYBOARD_GERMAN     "German"

/////////////////////////////////////////////////////////////////////////////
// CKeyboardTab property page

IMPLEMENT_DYNCREATE(CKeyboardTab, CPropertyPage)

CKeyboardTab::CKeyboardTab() : CPropertyPage(CKeyboardTab::IDD)
{
	//{{AFX_DATA_INIT(CKeyboardTab)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CKeyboardTab::~CKeyboardTab()
{
}

void CKeyboardTab::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CKeyboardTab)
	DDX_Control(pDX, IDC_FINDFAST_CHECK, m_FindFastCheck);
	DDX_Control(pDX, IDC_LANGUAGE_COMBO, m_LanguageCombo);
	DDX_Control(pDX, IDC_COMMANDLINE_EDIT, m_CommandLineEdit);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CKeyboardTab, CPropertyPage)
	//{{AFX_MSG_MAP(CKeyboardTab)
	ON_MESSAGE(WM_COMMANDHELP,OnCommandHelp)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CKeyboardTab message handlers

BOOL CKeyboardTab::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// Setup the language selection list
	int lang_id;

	AddLangToList(IDS_LANG_ENGLISH,LANGUAGE_ENGLISH);
#ifdef USE_MULTI_LANGUAGES
	AddLangToList(IDS_LANG_FRENCH,LANGUAGE_FRENCH);
	AddLangToList(IDS_LANG_GERMAN,LANGUAGE_GERMAN);
#ifdef ENABLE_ITALIAN
	AddLangToList(IDS_LANG_ITALIAN,LANGUAGE_ITALIAN);
#endif
	AddLangToList(IDS_LANG_SPANISH,LANGUAGE_SPANISH);
	lang_id = os_config_read_uint(szSectionName, "LanguageType", LANGUAGE_ENGLISH);	
#else
	lang_id = LANGUAGE_ENGLISH;
#endif

	SelectLangInList(lang_id);

	// Disable the language selection box
	if(!LanguageSelectionEnabled) {
		m_LanguageCombo.EnableWindow(FALSE);
	}

	// Setup the keyboard type
	char *keyboard_type = KEYBOARD_US;
	keyboard_type = os_config_read_string(szSectionName, "KeyboardType", "");

	if (!stricmp(keyboard_type, KEYBOARD_FRENCH))
		((CButton *) GetDlgItem(IDC_FRENCH_KEYBOARD))->SetCheck(1);
	else if (!stricmp(keyboard_type, KEYBOARD_GERMAN))
		((CButton *) GetDlgItem(IDC_GERMAN_KEYBOARD))->SetCheck(1);
	else {
		((CButton *) GetDlgItem(IDC_US_KEYBOARD))->SetCheck(1);
	}

	// Set the FindFast checkbox
	if(os_config_read_uint(szSectionName, "FindFastDisable",1))
		m_FindFastCheck.SetCheck(1);
	else
		m_FindFastCheck.SetCheck(0);

	// Read the command line
	char *command_line;
	command_line = os_config_read_string(szSectionName, "CmdLineOptions", "");
	m_CommandLineEdit.SetWindowText(command_line);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CKeyboardTab::OnOK() 
{
	// Get the selected language
	int prev_lang_id, lang_id;
	lang_id=GetSelectedLangFromList();
	if(lang_id<0) lang_id=LANGUAGE_ENGLISH;
	
	prev_lang_id=os_config_read_uint(szSectionName, "LanguageType", LANGUAGE_ENGLISH);
	if(prev_lang_id!=lang_id) NewLanguageSelected=TRUE;
	os_config_write_uint(szSectionName, "LanguageType", lang_id);

	// Get the keyboard type
	char *keyboard_type = KEYBOARD_US;

	if ( ((CButton *) GetDlgItem(IDC_FRENCH_KEYBOARD))->GetCheck() == 1 )
		keyboard_type = KEYBOARD_FRENCH;
	else if ( ((CButton *) GetDlgItem(IDC_GERMAN_KEYBOARD))->GetCheck() == 1 )
		keyboard_type = KEYBOARD_GERMAN;

	os_config_write_string(szSectionName, "KeyboardType", keyboard_type);

	// save the findfast checkbox setting
	if(m_FindFastCheck.GetCheck())
		os_config_write_uint(szSectionName, "FindFastDisable",1);
	else
		os_config_write_uint(szSectionName, "FindFastDisable",0);

	// write the command line
	CString command_line;
	m_CommandLineEdit.GetWindowText(command_line);
	os_config_write_string(szSectionName, "CmdLineOptions", command_line.GetBuffer(0));

	CPropertyPage::OnOK();
}

BOOL CKeyboardTab::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	// TODO: Add your message handler code here and/or call default
#ifdef USE_HTML_HELP_SYSTEM
	CWaitCursor wc;
	help_launch(KEYBOARDTAB_HELP);
	return 1;
#else
	return CPropertyPage::OnHelpInfo(pHelpInfo);
#endif
}

// Display the html help file
afx_msg LRESULT CKeyboardTab::OnCommandHelp(WPARAM wParam, LPARAM lParam)
{
#ifdef USE_HTML_HELP_SYSTEM
	help_launch(KEYBOARDTAB_HELP);
	return 1;
#else
	return CPropertyPage::OnCommandHelp(wParam,lParam);
#endif
}


bool CKeyboardTab::AddLangToList(int stringID, int langID)
{
	int index;
	CString lang_string;

	if(!lang_string.LoadString(stringID)) return FALSE;

	index=m_LanguageCombo.AddString(lang_string.GetBuffer(0));
	if(index<0) return FALSE;

	if(m_LanguageCombo.SetItemData(index,langID)==CB_ERR) return FALSE;

	return TRUE;
}


bool CKeyboardTab::SelectLangInList(int langID)
{
	int index, num_items;

	num_items=m_LanguageCombo.GetCount();
	for(index=0;index<num_items;index++) {
		int data;
		data=m_LanguageCombo.GetItemData(index);
		if(data!=CB_ERR && data==langID) {
			m_LanguageCombo.SetCurSel(index);
			return TRUE;
		}
	}

	return FALSE;
}


int CKeyboardTab::GetSelectedLangFromList(void)
{
	int index, data;

	index=m_LanguageCombo.GetCurSel();
	if(index<0) return(-1);

	data=m_LanguageCombo.GetItemData(index);
	if(data==CB_ERR) return(-1);

	return(data);
}
