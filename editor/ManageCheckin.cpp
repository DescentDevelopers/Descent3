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

// ManageCheckin.cpp : implementation file
//

#include "stdafx.h"
#include "editor.h"
#include "ManageCheckin.h"
#include "gametexture.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CManageCheckin dialog

CManageCheckin::CManageCheckin(CWnd *pParent /*=NULL*/) : CDialog(CManageCheckin::IDD, pParent) {
  //{{AFX_DATA_INIT(CManageCheckin)
  // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT
}

void CManageCheckin::DoDataExchange(CDataExchange *pDX) {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CManageCheckin)
  // NOTE: the ClassWizard will add DDX and DDV calls here
  //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CManageCheckin, CDialog)
//{{AFX_MSG_MAP(CManageCheckin)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CManageCheckin message handlers

BOOL CManageCheckin::OnInitDialog() {
  CDialog::OnInitDialog();

  SendDlgItemMessage(IDC_MULTICHECKIN_LISTBOX, LB_RESETCONTENT, 0, 0);
  for (int i = 0; i < MAX_TEXTURES; i++) {
    if (GameTextures[i].used)
      SendDlgItemMessage(IDC_MULTICHECKIN_LISTBOX, LB_ADDSTRING, 0, (LPARAM)(LPCTSTR)GameTextures[i].name);
  }

  return TRUE; // return TRUE unless you set the focus to a control
               // EXCEPTION: OCX Property Pages should return FALSE
}
