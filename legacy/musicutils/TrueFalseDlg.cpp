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

// TrueFalseDlg.cpp : implementation file
//

#include "stdafx.h"
#include "musicutils.h"
#include "TrueFalseDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// TrueFalseDlg dialog


TrueFalseDlg::TrueFalseDlg(CWnd* pParent /*=NULL*/)
	: CDialog(TrueFalseDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(TrueFalseDlg)
	m_IsFalse = -1;
	//}}AFX_DATA_INIT
}


void TrueFalseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(TrueFalseDlg)
	DDX_Radio(pDX, IDC_TRUE, m_IsFalse);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(TrueFalseDlg, CDialog)
	//{{AFX_MSG_MAP(TrueFalseDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// TrueFalseDlg message handlers
