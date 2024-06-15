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

// DriversDlg.cpp : implementation file
//

#include "stdafx.h"
#include "d3launch.h"
#include "DriversDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDriversDlg dialog

CDriversDlg::CDriversDlg(CWnd *pParent /*=NULL*/) : CDialog(CDriversDlg::IDD, pParent) {
  //{{AFX_DATA_INIT(CDriversDlg)
  // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT
  m_Selection = INSTALL_NONE;
}

void CDriversDlg::DoDataExchange(CDataExchange *pDX) {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CDriversDlg)
  // NOTE: the ClassWizard will add DDX and DDV calls here
  //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDriversDlg, CDialog)
//{{AFX_MSG_MAP(CDriversDlg)
ON_BN_CLICKED(IDC_DIRECTX_BUTTON, OnDirectxButton)
ON_BN_CLICKED(IDC_GLSETUP_BUTTON, OnGlsetupButton)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDriversDlg message handlers

BOOL CDriversDlg::OnInitDialog() {
  CDialog::OnInitDialog();

  m_Selection = INSTALL_NONE;

#if (defined(OEM_VOODOO3) || defined(DISABLE_GLSETUP))
  CWnd *button;
  button = GetDlgItem(IDC_GLSETUP_BUTTON);
  if (button != NULL)
    button->EnableWindow(FALSE);
#endif

  return TRUE; // return TRUE unless you set the focus to a control
               // EXCEPTION: OCX Property Pages should return FALSE
}

void CDriversDlg::OnDirectxButton() {
  m_Selection = INSTALL_DIRECTX;
  CDialog::OnOK();
}

void CDriversDlg::OnGlsetupButton() {
  m_Selection = INSTALL_GLSETUP;
  CDialog::OnOK();
}

void CDriversDlg::OnOK() {
  m_Selection = INSTALL_NONE;
  CDialog::OnOK();
}
