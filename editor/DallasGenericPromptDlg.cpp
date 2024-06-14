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

* $Logfile: /DescentIII/Main/editor/DallasGenericPromptDlg.cpp $
* $Revision: 1.1.1.1 $
* $Date: 2003-08-26 03:57:37 $
* $Author: kevinb $
*
* This is the header for the DALLAS generic prompt dialog
*
* $Log: not supported by cvs2svn $
 * 
 * 2     12/13/98 6:08p Nate
 * Initial version
*
* $NoKeywords: $
*/

// DallasGenericPromptDlg.cpp : implementation file
//

#include "stdafx.h"
#include "editor.h"
#include "DallasGenericPromptDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDallasGenericPromptDlg dialog


CDallasGenericPromptDlg::CDallasGenericPromptDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDallasGenericPromptDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDallasGenericPromptDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_DialogTitle="";
	m_PromptText="";
	m_PromptData="";
	m_MaxDataLength=10;
}


void CDallasGenericPromptDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDallasGenericPromptDlg)
	DDX_Control(pDX, IDC_GENERIC_DATA_EDIT, m_GenericDataEdit);
	DDX_Control(pDX, IDC_GENERIC_PROMPT_STATIC, m_GenericPromptStatic);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDallasGenericPromptDlg, CDialog)
	//{{AFX_MSG_MAP(CDallasGenericPromptDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDallasGenericPromptDlg message handlers

BOOL CDallasGenericPromptDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	if(!m_DialogTitle.IsEmpty())
		SetWindowText(m_DialogTitle.GetBuffer(0));

	if(!m_PromptText.IsEmpty())
		m_GenericPromptStatic.SetWindowText(m_PromptText.GetBuffer(0));

	m_GenericDataEdit.SetLimitText(m_MaxDataLength);
	if(!m_PromptData.IsEmpty())
		m_GenericDataEdit.SetWindowText(m_PromptData.GetBuffer(0));
	m_GenericDataEdit.SetSel(0,-1);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDallasGenericPromptDlg::OnOK() 
{
	// TODO: Add extra validation here
	m_GenericDataEdit.GetWindowText(m_PromptData);
	
	CDialog::OnOK();
}
