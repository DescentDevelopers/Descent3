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

// OsirisStatusDlg.cpp : implementation file
//

#include "stdafx.h"
#include "editor.h"
#include "OsirisStatusDlg.h"
#include "d3x_op.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COsirisStatusDlg dialog


COsirisStatusDlg::COsirisStatusDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COsirisStatusDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(COsirisStatusDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_Script=" ";
	m_Program = NULL;
}


void COsirisStatusDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COsirisStatusDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COsirisStatusDlg, CDialog)
	//{{AFX_MSG_MAP(COsirisStatusDlg)
	ON_BN_CLICKED(IDC_OSIRIS_DONE, OnOsirisDone)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COsirisStatusDlg message handlers

void COsirisStatusDlg::OnOsirisDone() 
{
	// TODO: Add your control notification handler code here
	CDialog::EndDialog(IDOK);
}

void COsirisStatusDlg::SetScript(char *script, tD3XProgram *d3x)
{
	m_Program = d3x;
	m_Script=script;
}

INT_PTR COsirisStatusDlg::DoModal() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CDialog::DoModal();
}


BOOL COsirisStatusDlg::OnInitDialog() 
{
#if 0 // LGT: Legacy scripting?
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CWnd *status_text;
	CEdit *error_box;
	CButton *done_button;
	char *error, *strbuf;
	int len, nscr, nstr;
	tD3XPMap *map;
	tD3XInstruction *ins;

	done_button=(CButton *)GetDlgItem(IDC_OSIRIS_DONE);
	error_box=(CEdit *)GetDlgItem(IDC_OSIRIS_ERROR);
	status_text=GetDlgItem(IDC_OSIRIS_PROGRESS);

	status_text->SetWindowText("Compiling...Please Wait");
	if(osi_Compile(m_Script.GetBuffer(1), &len, &ins, &nscr, &map, &nstr, &strbuf))
	{
		status_text->SetWindowText("Success!");
		error_box->SetWindowText("No errors.");
		D3XReallocProgram(m_Program, len, nscr, nstr);
		D3XLoadProgramFromComponents(m_Program, ins, map, strbuf);
	}
	else
	{
		status_text->SetWindowText("There were errors");
		error=osi_GetErrorBuffer();
		error_box->SetWindowText(error);
	}
	SetActiveWindow();
	SetFocus();
	error_box->SetFocus();
	error_box->SetActiveWindow();
	error_box->SetSel(-1,0);

	
	SetWindowText("Compiling Status...Done");
#endif
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
