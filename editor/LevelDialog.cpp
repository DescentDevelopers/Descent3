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

// LevelDialog.cpp : implementation file
//

#include "stdafx.h"
#include "editor.h"
#include "d3edit.h"
#include "LevelDialog.h"

#include "room.h"
#include "ambient.h"
#include "hlsoundlib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern const char *EnvAudio_PresetNames[N_ENVAUDIO_PRESETS];

/////////////////////////////////////////////////////////////////////////////
// CLevelDialog dialog

CLevelDialog::CLevelDialog(CWnd *pParent /*=NULL*/) : CDialog(CLevelDialog::IDD, pParent) {
  //{{AFX_DATA_INIT(CLevelDialog)
  // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT
}

void CLevelDialog::DoDataExchange(CDataExchange *pDX) {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CLevelDialog)
  // NOTE: the ClassWizard will add DDX and DDV calls here
  //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CLevelDialog, CDialog)
//{{AFX_MSG_MAP(CLevelDialog)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLevelDialog message handlers

#define NULL_NAME "<none>"

BOOL CLevelDialog::OnInitDialog() {
  CDialog::OnInitDialog();

  // set up reverb dropdown listbox
  CComboBox *cbox = (CComboBox *)GetDlgItem(IDC_ENVAUD_LIST);
  int i;

  cbox->ResetContent();
  for (i = 0; i < N_ENVAUDIO_PRESETS; i++) {
    cbox->AddString(EnvAudio_PresetNames[i]);
  }

  cbox = (CComboBox *)GetDlgItem(IDC_AMBIENT_SOUND);
  cbox->ResetContent();
  cbox->AddString(NULL_NAME);
  for (i = 0; i < Num_ambient_sound_patterns; i++) {
    if (Ambient_sound_patterns[i].name[0]) {
      cbox->AddString((LPCTSTR)AmbientSoundPatternName(i));
    }
  }

  //	select based off of selected rooms.
  bool single_ambient_sel = true, single_env_sel = true;

  if (N_selected_rooms) {
    room *rp = &Rooms[Selected_rooms[0]];
    int amb_sound = rp->ambient_sound;
    uint8_t env_preset = rp->env_reverb;

    for (i = 1; i < N_selected_rooms; i++) {
      rp = &Rooms[Selected_rooms[i]];
      if (rp->ambient_sound != amb_sound) {
        single_ambient_sel = false;
      }
      if (rp->env_reverb != env_preset) {
        single_env_sel = false;
      }
    }

    rp = &Rooms[Selected_rooms[0]];

    cbox = (CComboBox *)GetDlgItem(IDC_AMBIENT_SOUND);
    if (single_ambient_sel) {
      cbox->SelectString(-1, (rp->ambient_sound == -1) ? NULL_NAME : AmbientSoundPatternName(rp->ambient_sound));
    } else {
      cbox->SetCurSel(-1);
    }

    cbox = (CComboBox *)GetDlgItem(IDC_ENVAUD_LIST);
    if (single_env_sel) {
      cbox->SelectString(-1, EnvAudio_PresetNames[(rp->env_reverb < N_ENVAUDIO_PRESETS) ? rp->env_reverb : 0]);
    } else {
      cbox->SetCurSel(-1);
    }
  } else {
    ((CComboBox *)GetDlgItem(IDC_AMBIENT_SOUND))
        ->SelectString(-1,
                       (Curroomp->ambient_sound == -1) ? NULL_NAME : AmbientSoundPatternName(Curroomp->ambient_sound));
    ((CComboBox *)GetDlgItem(IDC_ENVAUD_LIST))
        ->SelectString(-1,
                       EnvAudio_PresetNames[(Curroomp->env_reverb < N_ENVAUDIO_PRESETS) ? Curroomp->env_reverb : 0]);
  }

  return TRUE; // return TRUE unless you set the focus to a control
               // EXCEPTION: OCX Property Pages should return FALSE
}

void CLevelDialog::OnOK() {
  //	if there's a current selection for each of the dropdown lists, set each room's properties via
  //	the selected room list.
  CComboBox *cbox = (CComboBox *)GetDlgItem(IDC_ENVAUD_LIST);
  CString selection;
  int i;
  room *rp;

  if (cbox->GetCurSel() != CB_ERR) {
    uint8_t env_reverb = 0;

    cbox->GetLBText(cbox->GetCurSel(), selection);
    for (i = 0; i < N_ENVAUDIO_PRESETS; i++) {
      if (selection == EnvAudio_PresetNames[i]) {
        env_reverb = i;
        break;
      }
    }
    if (i == N_ENVAUDIO_PRESETS) {
      Int3(); // Samir!
    }

    if (N_selected_rooms) {
      for (i = 0; i < N_selected_rooms; i++) {
        rp = &Rooms[Selected_rooms[i]];
        rp->env_reverb = env_reverb;
      }
    } else {
      Curroomp->env_reverb = env_reverb;
    }
  }

  cbox = (CComboBox *)GetDlgItem(IDC_AMBIENT_SOUND);
  if (cbox->GetCurSel() != CB_ERR) {
    int amb_sound = -1;

    cbox->GetLBText(cbox->GetCurSel(), selection);
    if (selection != NULL_NAME) {
      for (i = 0; i < Num_ambient_sound_patterns; i++) {
        if (Ambient_sound_patterns[i].name[0]) {
          if (selection == AmbientSoundPatternName(i)) {
            amb_sound = i;
            break;
          }
        }
      }
      if (i == Num_ambient_sound_patterns) {
        Int3(); // Samir!
      }
    }

    if (N_selected_rooms) {
      for (i = 0; i < N_selected_rooms; i++) {
        rp = &Rooms[Selected_rooms[i]];
        rp->ambient_sound = amb_sound;
      }
    } else {
      Curroomp->ambient_sound = amb_sound;
    }
  }

  Mine_changed = 1;

  CDialog::OnOK();
}
