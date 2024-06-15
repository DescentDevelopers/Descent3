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

// DallasFlagDlg.cpp : implementation file
//

#include "stdafx.h"

#ifdef NEWEDITOR
#include "NewEditor.h"
#else
#include "editor.h"
#endif

#include "cfile.h"
#include "DallasMainDlg.h"
#include "DallasFlagDlg.h"

CDallasMainDlg *GetDallasDialogPtr(void);

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDallasFlagDlg dialog

CDallasFlagDlg::CDallasFlagDlg(CWnd *pParent /*=NULL*/) : CDialog(CDallasFlagDlg::IDD, pParent) {
  //{{AFX_DATA_INIT(CDallasFlagDlg)
  // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT
  m_FlagsValue = 0;
  m_ValidFlagsMask = 0;
  m_FlagsName = "";
}

void CDallasFlagDlg::DoDataExchange(CDataExchange *pDX) {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CDallasFlagDlg)
  DDX_Control(pDX, IDC_FLAG_LIST, m_FlagListBox);
  DDX_Control(pDX, IDC_FLAG_PROMPT_STATIC, m_FlagPromptText);
  //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDallasFlagDlg, CDialog)
//{{AFX_MSG_MAP(CDallasFlagDlg)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDallasFlagDlg message handlers

BOOL CDallasFlagDlg::OnInitDialog() {
  CDialog::OnInitDialog();

  CDallasMainDlg *m_DallasModelessDlgPtr;
  m_DallasModelessDlgPtr = GetDallasDialogPtr();

  // If Dallas is up, fill in the list box
  if (m_DallasModelessDlgPtr != NULL) {
    m_DallasModelessDlgPtr->FillFlagValuesBox(&m_FlagListBox, m_FlagsName.GetBuffer(0), m_FlagsValue, m_ValidFlagsMask);
  }

  return TRUE; // return TRUE unless you set the focus to a control
               // EXCEPTION: OCX Property Pages should return FALSE
}

void CDallasFlagDlg::OnOK() {
  int list_size, j, data;

  // Fill the flags value with checklistbox data
  m_FlagsValue = 0;
  list_size = m_FlagListBox.GetCount();
  for (j = 0; j < list_size; j++) {
    if (m_FlagListBox.GetCheck(j)) {
      data = m_FlagListBox.GetItemData(j);
      m_FlagsValue |= data;
    }
  }

  CDialog::OnOK();
}

void CDallasFlagDlg::OnCancel() {
  // TODO: Add extra cleanup here

  CDialog::OnCancel();
}
