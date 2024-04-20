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

// MessageWindowDlg.cpp : implementation file
//

#include "stdafx.h"
#include "d3launch.h"
#include "MessageWindowDlg.h"
#include "UpdateDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CMessageWindowDlg *message_window;

/////////////////////////////////////////////////////////////////////////////
// CMessageWindowDlg dialog


CMessageWindowDlg::CMessageWindowDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMessageWindowDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMessageWindowDlg)
	m_WndMsgMain = _T("");
	m_WndMsgStatus = _T("");
	//}}AFX_DATA_INIT
}


void CMessageWindowDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMessageWindowDlg)
	DDX_Text(pDX, IDC_WND_MSG_MAIN, m_WndMsgMain);
	DDX_Text(pDX, IDC_WND_MSG_STATUS, m_WndMsgStatus);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMessageWindowDlg, CDialog)
	//{{AFX_MSG_MAP(CMessageWindowDlg)
	ON_BN_CLICKED(IDC_BTN_CANCEL, OnBtnCancel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMessageWindowDlg message handlers


void CMessageWindowDlg::OnBtnCancel() 
{
	// TODO: Add your control notification handler code here
	// Dim the button once pressed
	HWND hwnd;

	GetDlgItem(IDC_BTN_CANCEL,&hwnd);
	::EnableWindow(hwnd,FALSE);

	// Mark cancel pressed global variable
	CancelPressed=TRUE;
}
