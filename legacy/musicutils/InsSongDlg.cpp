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

// InsSongDlg.cpp : implementation file
//

#include "stdafx.h"
#include "musicutils.h"
#include "InsSongDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInsSongDlg dialog

CInsSongDlg::CInsSongDlg(CWnd *pParent /*=NULL*/) : CDialog(CInsSongDlg::IDD, pParent) {
  //{{AFX_DATA_INIT(CInsSongDlg)
  m_SongPath = _T("");
  //}}AFX_DATA_INIT
}

void CInsSongDlg::DoDataExchange(CDataExchange *pDX) {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CInsSongDlg)
  DDX_Text(pDX, IDC_EDIT, m_SongPath);
  //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CInsSongDlg, CDialog)
//{{AFX_MSG_MAP(CInsSongDlg)
ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInsSongDlg message handlers

BOOL CInsSongDlg::OnInitDialog() {
  CDialog::OnInitDialog();

  return TRUE; // return TRUE unless you set the focus to a control
               // EXCEPTION: OCX Property Pages should return FALSE
}

void CInsSongDlg::OnBrowse() {
  CFileDialog opendlg(TRUE, "osf", NULL, OFN_PATHMUSTEXIST, "Outrage Stream Files(*.osf)|*.osf||", this);
  CEdit *edit = (CEdit *)GetDlgItem(IDC_EDIT);

  //	get filename to play
  if (opendlg.DoModal() != IDOK) {
    return;
  }

  edit->SetWindowText((LPCTSTR)opendlg.GetPathName());
}
