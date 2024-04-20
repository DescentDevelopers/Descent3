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

// FindFastDlg.cpp : implementation file
//

#include "stdafx.h"
#include "d3launch.h"
#include "FindFastDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFindFastDlg dialog


CFindFastDlg::CFindFastDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFindFastDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFindFastDlg)
	m_AlwaysTerminateFF = FALSE;
	//}}AFX_DATA_INIT
}


void CFindFastDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFindFastDlg)
	DDX_Check(pDX, IDC_FINDFASTCHKBOX, m_AlwaysTerminateFF);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFindFastDlg, CDialog)
	//{{AFX_MSG_MAP(CFindFastDlg)
	ON_BN_CLICKED(IDC_FINDFASTCHKBOX, OnFindfastchkbox)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFindFastDlg message handlers

void CFindFastDlg::OnFindfastchkbox() 
{
	CButton *chkbox = (CButton *)GetDlgItem(IDC_FINDFASTCHKBOX);
	CButton *no_btn = (CButton *)GetDlgItem(IDCANCEL);

	if (chkbox->GetCheck() == 1) {
		no_btn->EnableWindow(FALSE);
	}
	else {
		no_btn->EnableWindow(TRUE);
	}
}


BOOL CFindFastDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CButton *no_btn = (CButton *)GetDlgItem(IDCANCEL);

	if (m_AlwaysTerminateFF) {
		no_btn->EnableWindow(FALSE);		// this should never happen anyway, but just in case.
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
