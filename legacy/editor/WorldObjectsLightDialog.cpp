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

// WorldObjectsLightDialog.cpp : implementation file
//

#include "stdafx.h"
#include "editor.h"
#include "WorldObjectsLightDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWorldObjectsLightDialog dialog


CWorldObjectsLightDialog::CWorldObjectsLightDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CWorldObjectsLightDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWorldObjectsLightDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CWorldObjectsLightDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWorldObjectsLightDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWorldObjectsLightDialog, CDialog)
	//{{AFX_MSG_MAP(CWorldObjectsLightDialog)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWorldObjectsLightDialog message handlers

BOOL CWorldObjectsLightDialog::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	// TODO: Add your message handler code here and/or call default
	WinHelp(HID_LIGHTSDLG,HELP_CONTEXT);
	return TRUE;
	//return CDialog::OnHelpInfo(pHelpInfo);
}
