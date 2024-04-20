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

/*
 * $Logfile: /DescentIII/Main/editor/TableFileFilterAddDlg.cpp $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:57:39 $
 * $Author: kevinb $
 *
 * 
 *
 * $Log: not supported by cvs2svn $
 * 
 * 5     10/09/98 11:07a Nate
 * Fixed it up a bit, made it save the last selected type
 * 
 * 4     10/08/98 10:48p Nate
 * 
 * 3     10/08/98 10:30a Nate
 * Initial version
 *
 * $NoKeywords: $
 */

// TableFileFilterAddDlg.cpp : implementation file
//

#include "stdafx.h"
#include "editor.h"
#include "TableFileFilterAddDlg.h"
#include "manage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Global to save user's last type choice
CString last_type="";

/////////////////////////////////////////////////////////////////////////////
// CTableFileFilterAddDlg dialog


CTableFileFilterAddDlg::CTableFileFilterAddDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTableFileFilterAddDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTableFileFilterAddDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CTableFileFilterAddDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTableFileFilterAddDlg)
	DDX_Control(pDX, IDC_TABLEFILEFILTER_ADDTYPE_COMBO, m_TypeCombo);
	DDX_Control(pDX, IDC_TABLEFILEFILTER_ADDNAME_EDIT, m_NameEdit);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTableFileFilterAddDlg, CDialog)
	//{{AFX_MSG_MAP(CTableFileFilterAddDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTableFileFilterAddDlg message handlers

BOOL CTableFileFilterAddDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_TypeCombo.ResetContent();
	//m_TypeCombo.AddString("Unknown");
	m_TypeCombo.AddString("Texture");
	m_TypeCombo.AddString("Weapon");
	//m_TypeCombo.AddString("Robot");
	//m_TypeCombo.AddString("Powerup");
	m_TypeCombo.AddString("Door");
	m_TypeCombo.AddString("Ship");
	m_TypeCombo.AddString("Sound");
	//m_TypeCombo.AddString("Megacell");
	//m_TypeCombo.AddString("Gamefile");
	m_TypeCombo.AddString("Generic");

	if(last_type.IsEmpty())
		m_TypeCombo.SelectString(-1,"Generic");
	else
		m_TypeCombo.SelectString(-1,last_type.GetBuffer(0));

	m_NameEdit.LimitText(PAGENAME_LEN-1);
	m_NameEdit.SetWindowText("");

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTableFileFilterAddDlg::OnOK() 
{
	// TODO: Add extra validation here
	CString type_name;

	m_NameEdit.GetWindowText(page_name);

	int index=m_TypeCombo.GetCurSel();
	m_TypeCombo.GetLBText(index,type_name);
	last_type=type_name;

	if(type_name=="Unknown")
		page_type=PAGETYPE_UNKNOWN;
	else if(type_name=="Texture")
		page_type=PAGETYPE_TEXTURE;
	else if(type_name=="Weapon")
		page_type=PAGETYPE_WEAPON;
	else if(type_name=="Robot")
		page_type=PAGETYPE_ROBOT;
	else if(type_name=="Powerup")
		page_type=PAGETYPE_POWERUP;
	else if(type_name=="Door")
		page_type=PAGETYPE_DOOR;
	else if(type_name=="Ship")
		page_type=PAGETYPE_SHIP;
	else if(type_name=="Sound")
		page_type=PAGETYPE_SOUND;
	else if(type_name=="Megacell")
		page_type=PAGETYPE_MEGACELL;
	else if(type_name=="Gamefile")
		page_type=PAGETYPE_GAMEFILE;
	else 
		page_type=PAGETYPE_GENERIC;

	CDialog::OnOK();
}

void CTableFileFilterAddDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}
