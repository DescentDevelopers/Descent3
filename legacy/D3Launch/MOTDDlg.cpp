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

// MOTDDlg.cpp : implementation file
//

#include "stdafx.h"
#include "d3launch.h"
#include "MOTDDlg.h"
#include "PsTypes.h"
#include "LaunchNames.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMOTDDlg dialog


CMOTDDlg::CMOTDDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMOTDDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMOTDDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CMOTDDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMOTDDlg)
	DDX_Control(pDX, IDC_LIST1, m_TextList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMOTDDlg, CDialog)
	//{{AFX_MSG_MAP(CMOTDDlg)
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMOTDDlg message handlers

BOOL CMOTDDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_TextList.ResetContent();	

	if(!ParseMOTDFile())
		return FALSE;

	m_TextList.SetTopIndex(0);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


// Reads the message of the day text into the list box 
BOOL CMOTDDlg::ParseMOTDFile(void)
{
	char filebuffer[_MAX_PATH+1];
	FILE *f;

	f = fopen(MOTD_LOC_FNAME, "rt");
	if(f == NULL){
		return FALSE;
	}

	while (!feof(f)) {
		fgets(filebuffer, _MAX_PATH, f);
		if(strlen(filebuffer)>0 && filebuffer[strlen(filebuffer) - 1] == '\n')
			filebuffer[strlen(filebuffer) - 1] = '\0';
		m_TextList.AddString(filebuffer);
	}
	fclose(f);

	return TRUE;
}

void CMOTDDlg::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	
	// TODO: Add your message handler code here
	//if(bShow==TRUE && nStatus==SW_PARENTOPENING) {
	//	OutputDebugString("Update dlg has opened\n");
	//}
}
