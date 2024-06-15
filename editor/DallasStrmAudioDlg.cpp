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

// DallasStrmAudioDlg.cpp : implementation file
//

#include "stdafx.h"
#include "editor.h"
#include "DallasUtilities.h"
#include "manage.h"
#include "streamaudio.h"
#include "DallasStrmAudioDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDallasStrmAudioDlg dialog

CDallasStrmAudioDlg::CDallasStrmAudioDlg(CWnd *pParent /*=NULL*/) : CDialog(CDallasStrmAudioDlg::IDD, pParent) {
  //{{AFX_DATA_INIT(CDallasStrmAudioDlg)
  // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT
  m_Filename = "";
}

void CDallasStrmAudioDlg::DoDataExchange(CDataExchange *pDX) {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CDallasStrmAudioDlg)
  DDX_Control(pDX, IDC_STRM_AUDIO_LIST, m_StrmAudioBox);
  //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDallasStrmAudioDlg, CDialog)
//{{AFX_MSG_MAP(CDallasStrmAudioDlg)
ON_LBN_DBLCLK(IDC_STRM_AUDIO_LIST, OnDblclkStrmAudioList)
ON_BN_CLICKED(IDC_PLAY_STRM_AUDIO_BUTTON, OnPlayStrmAudioButton)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDallasStrmAudioDlg message handlers

void CDallasStrmAudioDlg::OnOK() {
  // TODO: Add extra validation here
  int index = m_StrmAudioBox.GetCurSel();
  if (index == LB_ERR)
    return;

  m_StrmAudioBox.GetText(index, m_Filename);

  CDialog::OnOK();
}

void CDallasStrmAudioDlg::OnCancel() {
  // TODO: Add extra cleanup here

  CDialog::OnCancel();
}

BOOL CDallasStrmAudioDlg::OnInitDialog() {
  CDialog::OnInitDialog();

  FillStrmAudioList();

  if (!m_Filename.IsEmpty())
    m_StrmAudioBox.SelectString(-1, m_Filename.GetBuffer(0));

  return TRUE; // return TRUE unless you set the focus to a control
               // EXCEPTION: OCX Property Pages should return FALSE
}

void CDallasStrmAudioDlg::OnDblclkStrmAudioList() {
  CString filename;

  int index = m_StrmAudioBox.GetCurSel();
  if (index == LB_ERR)
    return;

  m_StrmAudioBox.GetText(index, filename);
  StreamPlay(filename.GetBuffer(0), 1, 0);
}

void CDallasStrmAudioDlg::OnPlayStrmAudioButton() { OnDblclkStrmAudioList(); }

void CDallasStrmAudioDlg::FillStrmAudioList(void) {
  bool file_found;
  char filename[PAGENAME_LEN + 1];

  file_found = FindManageFirst(filename, "*.osf");
  while (file_found) {
    m_StrmAudioBox.AddString(filename);
    file_found = FindManageNext(filename);
  }
  FindManageClose();
}
