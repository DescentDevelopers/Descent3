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

// RoomLinkDlg.cpp : implementation file
//

#include "stdafx.h"
#include "musicutils.h"
#include "RoomLinkDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRoomLinkDlg dialog

CRoomLinkDlg::CRoomLinkDlg(CWnd *pParent /*=NULL*/) : CDialog(CRoomLinkDlg::IDD, pParent) {
  //{{AFX_DATA_INIT(CRoomLinkDlg)
  m_Room = 0;
  m_RoomCheck = FALSE;
  //}}AFX_DATA_INIT
}

void CRoomLinkDlg::DoDataExchange(CDataExchange *pDX) {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CRoomLinkDlg)
  DDX_Text(pDX, IDC_ROOMLINK, m_Room);
  DDV_MinMaxInt(pDX, m_Room, -1, 65536);
  DDX_Check(pDX, IDC_DOROOMLINK, m_RoomCheck);
  //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CRoomLinkDlg, CDialog)
//{{AFX_MSG_MAP(CRoomLinkDlg)
ON_BN_CLICKED(IDC_DOROOMLINK, OnDoroomlink)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRoomLinkDlg message handlers

BOOL CRoomLinkDlg::OnInitDialog() {
  CDialog::OnInitDialog();

  //	check if m_Room is valid, if so then check 'room checkbox'
  if (m_Room > -1) {
    ((CButton *)GetDlgItem(IDC_DOROOMLINK))->SetCheck(1);
    GetDlgItem(IDC_ROOMLINK)->EnableWindow(TRUE);
  }

  return TRUE; // return TRUE unless you set the focus to a control
               // EXCEPTION: OCX Property Pages should return FALSE
}

void CRoomLinkDlg::OnDoroomlink() {
  int check = ((CButton *)GetDlgItem(IDC_DOROOMLINK))->GetCheck();
  if (check)
    GetDlgItem(IDC_ROOMLINK)->EnableWindow(TRUE);
  else
    GetDlgItem(IDC_ROOMLINK)->EnableWindow(FALSE);
}
