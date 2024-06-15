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

// SetupPropSheet.cpp : implementation file
//

#include "stdafx.h"
#include "d3launch.h"
#include "SetupPropSheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetupPropSheet

IMPLEMENT_DYNAMIC(CSetupPropSheet, CPropertySheet)

CSetupPropSheet::CSetupPropSheet(UINT nIDCaption, CWnd *pParentWnd, UINT iSelectPage)
    : CPropertySheet(nIDCaption, pParentWnd, iSelectPage) {}

CSetupPropSheet::CSetupPropSheet(LPCTSTR pszCaption, CWnd *pParentWnd, UINT iSelectPage)
    : CPropertySheet(pszCaption, pParentWnd, iSelectPage) {}

CSetupPropSheet::~CSetupPropSheet() {}

BEGIN_MESSAGE_MAP(CSetupPropSheet, CPropertySheet)
//{{AFX_MSG_MAP(CSetupPropSheet)
// NOTE - the ClassWizard will add and remove mapping macros here.
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetupPropSheet message handlers

BOOL CSetupPropSheet::OnInitDialog() {
  BOOL bResult = CPropertySheet::OnInitDialog();

  // TODO: Add your specialized code here
  HWND OK_btn = ::GetDlgItem(this->m_hWnd, IDOK);
  if (OK_btn != NULL) {
    CString title;
    title.LoadString(IDS_OK);
    ::SetWindowText(OK_btn, title.GetBuffer(0));
  }

  HWND Cancel_btn = ::GetDlgItem(this->m_hWnd, IDCANCEL);
  if (Cancel_btn != NULL) {
    CString title;
    title.LoadString(IDS_CANCEL);
    ::SetWindowText(Cancel_btn, title.GetBuffer(0));
  }

  HWND Help_btn = ::GetDlgItem(this->m_hWnd, IDHELP);
  if (Help_btn != NULL) {
    CString title;
    title.LoadString(IDS_HELP);
    ::SetWindowText(Help_btn, title.GetBuffer(0));
  }

  return bResult;
}
