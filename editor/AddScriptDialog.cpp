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

// AddScriptDialog.cpp : implementation file
//

#include "stdafx.h"
#include "editor.h"
#include "AddScriptDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAddScriptDialog dialog

CAddScriptDialog::CAddScriptDialog(CWnd *pParent /*=NULL*/) : CDialog(CAddScriptDialog::IDD, pParent) {
  //{{AFX_DATA_INIT(CAddScriptDialog)
  m_Name = _T("");
  m_TypeName = _T("");
  //}}AFX_DATA_INIT
}

void CAddScriptDialog::DoDataExchange(CDataExchange *pDX) {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CAddScriptDialog)
  DDX_Text(pDX, IDC_EDITNAME, m_Name);
  DDV_MaxChars(pDX, m_Name, 32);
  DDX_CBString(pDX, IDC_TYPESEL, m_TypeName);
  DDV_MaxChars(pDX, m_TypeName, 16);
  //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAddScriptDialog, CDialog)
//{{AFX_MSG_MAP(CAddScriptDialog)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAddScriptDialog message handlers

BOOL CAddScriptDialog::OnInitDialog() {
  CDialog::OnInitDialog();

  CComboBox *cbox = (CComboBox *)GetDlgItem(IDC_TYPESEL);

  cbox->AddString("object");
  cbox->AddString("trigger");
  cbox->SelectString(-1, "object");

  return TRUE; // return TRUE unless you set the focus to a control
               // EXCEPTION: OCX Property Pages should return FALSE
}
