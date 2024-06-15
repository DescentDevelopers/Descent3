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

// DallasSoundDlg.cpp : implementation file
//

#include "stdafx.h"
#include "editor.h"
#include "hlsoundlib.h"
#include "soundload.h"
#include "DallasSoundDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDallasSoundDlg dialog

CDallasSoundDlg::CDallasSoundDlg(CWnd *pParent /*=NULL*/) : CDialog(CDallasSoundDlg::IDD, pParent) {
  //{{AFX_DATA_INIT(CDallasSoundDlg)
  // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT

  m_SoundName = "";
  m_SoundIndex = -1;
}

void CDallasSoundDlg::DoDataExchange(CDataExchange *pDX) {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CDallasSoundDlg)
  DDX_Control(pDX, IDC_SOUND_LIST, m_SoundList);
  //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDallasSoundDlg, CDialog)
//{{AFX_MSG_MAP(CDallasSoundDlg)
ON_BN_CLICKED(IDC_PLAY_SOUND_BUTTON, OnPlaySoundButton)
ON_BN_CLICKED(IDC_STOP_SOUNDS_BUTTON, OnStopSoundsButton)
ON_LBN_DBLCLK(IDC_SOUND_LIST, OnDblclkSoundList)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDallasSoundDlg message handlers

BOOL CDallasSoundDlg::OnInitDialog() {
  CDialog::OnInitDialog();

  FillSoundList();

  if (!m_SoundName.IsEmpty())
    m_SoundList.SelectString(-1, m_SoundName.GetBuffer(0));

  Sound_system.BeginSoundFrame(FALSE);

  return TRUE; // return TRUE unless you set the focus to a control
               // EXCEPTION: OCX Property Pages should return FALSE
}

void CDallasSoundDlg::OnOK() {
  int index = m_SoundList.GetCurSel();
  if (index == LB_ERR)
    return;

  m_SoundIndex = m_SoundList.GetItemData(index);
  Sound_system.StopAllSounds();
  Sound_system.EndSoundFrame();

  CDialog::OnOK();
}

void CDallasSoundDlg::OnCancel() {
  Sound_system.StopAllSounds();
  Sound_system.EndSoundFrame();

  CDialog::OnCancel();
}

void CDallasSoundDlg::OnPlaySoundButton() {
  int index;
  index = m_SoundList.GetCurSel();
  if (index == LB_ERR)
    return;

  CString sound_name;
  m_SoundList.GetText(index, sound_name);

  Sound_system.BeginSoundFrame(FALSE);
  Sound_system.Play2dSound(FindSoundName(sound_name.GetBuffer(0)), MAX_GAME_VOLUME);
  Sound_system.EndSoundFrame();
}

void CDallasSoundDlg::OnStopSoundsButton() { Sound_system.StopAllSounds(); }

void CDallasSoundDlg::FillSoundList(void) {
  int i;

  // Fill the menus with sounds
  for (i = 0; i < MAX_SOUNDS; i++) {
    if ((Sounds[i].used) && (strlen(Sounds[i].name) > 0)) {
      int index;
      index = m_SoundList.AddString(Sounds[i].name);
      if (index != LB_ERR) {
        m_SoundList.SetItemData(index, i);
      }
    }
  }
}

void CDallasSoundDlg::OnDblclkSoundList() { OnPlaySoundButton(); }
