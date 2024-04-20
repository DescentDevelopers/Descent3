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

// DallasVectorPromptDialog.cpp : implementation file
//

#include "stdafx.h"
#include "editor.h"
#include "DallasVectorPromptDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDallasVectorPromptDialog dialog


CDallasVectorPromptDialog::CDallasVectorPromptDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CDallasVectorPromptDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDallasVectorPromptDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_PromptData1=0.0;
	m_PromptData2=0.0;
	m_PromptData3=0.0;
}


void CDallasVectorPromptDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDallasVectorPromptDialog)
	DDX_Control(pDX, IDC_DATA3_EDIT, m_Data3Edit);
	DDX_Control(pDX, IDC_DATA2_EDIT, m_Data2Edit);
	DDX_Control(pDX, IDC_DATA1_EDIT, m_Data1Edit);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDallasVectorPromptDialog, CDialog)
	//{{AFX_MSG_MAP(CDallasVectorPromptDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDallasVectorPromptDialog message handlers

BOOL CDallasVectorPromptDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CString value;

	value.Format("%f",m_PromptData1);
	m_Data1Edit.SetWindowText(value.GetBuffer(0));

	value.Format("%f",m_PromptData2);
	m_Data2Edit.SetWindowText(value.GetBuffer(0));

	value.Format("%f",m_PromptData3);
	m_Data3Edit.SetWindowText(value.GetBuffer(0));

	m_Data1Edit.SetLimitText(10);
	m_Data2Edit.SetLimitText(10);
	m_Data3Edit.SetLimitText(10);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDallasVectorPromptDialog::OnOK() 
{
	// TODO: Add extra validation here
	CString value;

	m_Data1Edit.GetWindowText(value);
	m_PromptData1=atof(value.GetBuffer(0));

	m_Data2Edit.GetWindowText(value);
	m_PromptData2=atof(value.GetBuffer(0));

	m_Data3Edit.GetWindowText(value);
	m_PromptData3=atof(value.GetBuffer(0));

	CDialog::OnOK();
}
