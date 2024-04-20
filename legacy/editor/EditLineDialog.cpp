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

// EditLineDialog.cpp : implementation file
//

#include "stdafx.h"
#include "editor.h"
#include "EditLineDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditLineDialog dialog


CEditLineDialog::CEditLineDialog(char *title, char *caption, char *initial, bool numeric, CWnd* pParent)
	: CDialog(CEditLineDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEditLineDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_Caption = caption;
	m_Initial = initial;
	m_Title = title;

	m_Numeric = numeric;
}


CEditLineDialog::CEditLineDialog(char *caption, CWnd* pParent)
	: CDialog(CEditLineDialog::IDD, pParent)
{
	//NOTE:  ClassWizard can't handle two DATA_INIT sections, so anything that gets
	// added above should be copied down here.

	m_Caption = caption;
	m_Initial = NULL;
	m_Title = caption;

	m_Numeric = 0;
}


void CEditLineDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditLineDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEditLineDialog, CDialog)
	//{{AFX_MSG_MAP(CEditLineDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CEditLineDialog message handlers

void CEditLineDialog::OnOK() 
{
	CEdit *ctl = (CEdit *)GetDlgItem(IDC_EDIT);
	ctl->GetWindowText(m_EditBuf);	
	CDialog::OnOK();
}

BOOL CEditLineDialog::OnInitDialog() 
{
	CEdit *edit_ctl = (CEdit *)GetDlgItem(IDC_EDIT);

	CDialog::OnInitDialog();
	
	SetWindowText(m_Title);

	((CEdit *) GetDlgItem(IDC_PROMPT))->SetWindowText(m_Caption);

	if (m_Initial)
		edit_ctl->SetWindowText(m_Initial);

	//Turn on/off numeric-only input
	if (m_Numeric)
		edit_ctl->ModifyStyle(0,ES_NUMBER);
	else
		edit_ctl->ModifyStyle(ES_NUMBER,0);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//Gets a string from the user
//Parameters:	buf - buffer the string is written to.  should be initialized to default value
//					maxsize - the length of buf
//					title - the title for the input window
//					prompt - the prompt for the input box
//Returns:	false if cancel was pressed on the dialog, else true
//				If false returned, buf is unchanged
bool InputString(char *buf,int maxsize,char *title,char *prompt,CWnd *wnd)
{
	CEditLineDialog dlg(title,prompt,buf,0,wnd);

	if (dlg.DoModal() == IDOK) {
		strncpy(buf,(char *) dlg.GetText(),maxsize);
		buf[maxsize-1] = 0;		//strncpy doesn't terminate if string is too long
		return 1;
	}
	else
		return 0;
}

//Gets a number from the user
//Parameters:	n - filled in the with return value
//					title - the title for the input window
//					prompt - the prompt for the input box
//Returns:	false if cancel was pressed on the dialog, else true
//				If false returned, n is unchanged
bool InputNumber(int *n,char *title,char *prompt,CWnd *wnd)
{
	char buf[100] = "";
	CEditLineDialog dlg(title,prompt,buf,1,wnd);

	if (dlg.DoModal() == IDOK) {
		*n = atoi((char *) dlg.GetText());
		return 1;
	}
	else
		return 0;
}


