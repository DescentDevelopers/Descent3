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

// PlayerWeaponsDialog.cpp : implementation file
//

#include "stdafx.h"
#include "editor.h"
#include "PlayerWeaponsDialog.h"
#include "RobotEditWeaponsDialog.h"
#include "ship.h"
#include "weapon.h"
#include "pserror.h"
#include "soundload.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// char *WeaponListText[MAX_PLAYER_WEAPONS] =
//{
//	"Primary  1",
//	"Primary  2",
//	"Primary  3",
//	"Primary  4",
//	"Primary  5",
//	"Primary  6",
//	"Primary  7",
//	"Primary  8",
//	"Primary  9",
//	"Primary 10",
//	"Secondary  1",
//	"Secondary  2",
//	"Secondary  3",
//	"Secondary  4",
//	"Secondary  5",
//	"Secondary  6",
//	"Secondary  7",
//	"Secondary  8",
//	"Secondary  9",
//	"Secondary 10",
//	"Flare"
// };

// Use the real weapon names in this dialog
#define WeaponListText Static_weapon_names

/////////////////////////////////////////////////////////////////////////////
// PlayerWeaponsDialog dialog

PlayerWeaponsDialog::PlayerWeaponsDialog(int current_ship, CWnd *pParent /*=NULL*/)
    : CDialog(PlayerWeaponsDialog::IDD, pParent) {
  //{{AFX_DATA_INIT(PlayerWeaponsDialog)
  m_current_wb_text = _T("");
  //}}AFX_DATA_INIT

  m_current_ship = current_ship;
}

void PlayerWeaponsDialog::DoDataExchange(CDataExchange *pDX) {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(PlayerWeaponsDialog)
  DDX_CBString(pDX, IDC_CURRENT_WEAPON_BATTERY_COMBO, m_current_wb_text);
  //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(PlayerWeaponsDialog, CDialog)
//{{AFX_MSG_MAP(PlayerWeaponsDialog)
ON_BN_CLICKED(IDC_EDIT_WB_BUTTON, OnEditWbButton)
ON_CBN_SELENDOK(IDC_CURRENT_WEAPON_BATTERY_COMBO, OnSelendokCurrentWeaponBatteryCombo)
ON_BN_CLICKED(IDC_FIRES_FUSION, OnFiresFusion)
ON_BN_CLICKED(IDC_CONTINUOUS_FIRING_SOUND, OnContinuousFiringSound)
ON_BN_CLICKED(IDC_FIRING_RELEASE_SOUND, OnFiringReleaseSound)
ON_CBN_SELCHANGE(IDC_FIRING_SOUND_PULLDOWN, OnSelchangeFiringSoundPulldown)
ON_CBN_SELCHANGE(IDC_RELEASE_SOUND_PULLDOWN, OnSelchangeReleaseSoundPulldown)
ON_CBN_SELCHANGE(IDC_SPEW_POWERUP_PULLDOWN, OnSelchangeSpewPowerupPulldown)
ON_BN_CLICKED(IDC_ONOFF, OnOnoff)
ON_BN_CLICKED(IDC_ZOOM, OnZoom)
ON_EN_KILLFOCUS(IDC_MAX_AMMO, OnKillfocusMaxAmmo)
ON_BN_CLICKED(IDC_SHOW_TENTHS, OnShowTenths)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// PlayerWeaponsDialog message handlers

int PlayerWeaponsDialog::CurrentWBIndex() {
  int i;

  for (i = 0; i < MAX_PLAYER_WEAPONS; i++) {
    if (!stricmp(WeaponListText[i], m_current_wb_text))
      break;
  }

  ASSERT(i < MAX_PLAYER_WEAPONS);

  return i;
}

void PlayerWeaponsDialog::OnEditWbButton() {
  int i = CurrentWBIndex();

  // All player wb profiles are considered to be wb 0 as far as RobotEditWeaponsDialog is concerned.  :)
  RobotEditWeaponsDialog dlg(&Ships[m_current_ship].static_wb[i],
                             GetPolymodelPointer(Ships[m_current_ship].model_handle));

  if (dlg.DoModal() == IDOK)
    dlg.GetData();
}

#define NULL_NAME "<none>"

BOOL PlayerWeaponsDialog::OnInitDialog() {
  CDialog::OnInitDialog();
  int i;

  SendDlgItemMessage(IDC_CURRENT_WEAPON_BATTERY_COMBO, CB_RESETCONTENT, 0, 0);
  for (i = 0; i < MAX_PLAYER_WEAPONS; i++) {
    if (!stricmp("", WeaponListText[i]))
      break;
    SendDlgItemMessage(IDC_CURRENT_WEAPON_BATTERY_COMBO, CB_ADDSTRING, 0, (LPARAM)(LPCTSTR)WeaponListText[i]);
  }
  SendDlgItemMessage(IDC_CURRENT_WEAPON_BATTERY_COMBO, CB_SELECTSTRING, 0, (LPARAM)(LPCTSTR)WeaponListText[0]);

  SendDlgItemMessage(IDC_FIRING_SOUND_PULLDOWN, CB_RESETCONTENT, 0, 0);
  for (i = 0; i < MAX_SOUNDS; i++) {
    if (Sounds[i].used)
      SendDlgItemMessage(IDC_FIRING_SOUND_PULLDOWN, CB_ADDSTRING, 0, (LPARAM)(LPCTSTR)Sounds[i].name);
  }
  SendDlgItemMessage(IDC_FIRING_SOUND_PULLDOWN, CB_SELECTSTRING, 0, (LPARAM)(LPCTSTR)Sounds[0].name);

  SendDlgItemMessage(IDC_RELEASE_SOUND_PULLDOWN, CB_RESETCONTENT, 0, 0);
  for (i = 0; i < MAX_SOUNDS; i++) {
    if (Sounds[i].used)
      SendDlgItemMessage(IDC_RELEASE_SOUND_PULLDOWN, CB_ADDSTRING, 0, (LPARAM)(LPCTSTR)Sounds[i].name);
  }
  SendDlgItemMessage(IDC_RELEASE_SOUND_PULLDOWN, CB_SELECTSTRING, 0, (LPARAM)(LPCTSTR)Sounds[0].name);

  // Update powerup list
  SendDlgItemMessage(IDC_SPEW_POWERUP_PULLDOWN, CB_RESETCONTENT, 0, 0);
  SendDlgItemMessage(IDC_SPEW_POWERUP_PULLDOWN, CB_ADDSTRING, 0, (LPARAM)(LPCTSTR)NULL_NAME);
  for (i = 0; i < MAX_OBJECT_IDS; i++)
    if (Object_info[i].type == OBJ_POWERUP)
      SendDlgItemMessage(IDC_SPEW_POWERUP_PULLDOWN, CB_ADDSTRING, 0, (LPARAM)(LPCTSTR)Object_info[i].name);
  SendDlgItemMessage(IDC_SPEW_POWERUP_PULLDOWN, CB_SELECTSTRING, 0, (LPARAM)(LPCTSTR)NULL_NAME);

  m_current_wb_text = WeaponListText[0];

  UpdateDialog();

  return TRUE; // return TRUE unless you set the focus to a control
               // EXCEPTION: OCX Property Pages should return FALSE
}

void PlayerWeaponsDialog::UpdateDialog() {
  CButton *bbox;
  int index = CurrentWBIndex();
  ship *ship = &Ships[m_current_ship];
  int fire_flags = Ships[m_current_ship].fire_flags[index];
  int result;

  // if(!UpdateData(true)) return;

  bbox = (CButton *)GetDlgItem(IDC_FIRES_FUSION);
  bbox->SetCheck(fire_flags & SFF_FUSION);

  bbox = (CButton *)GetDlgItem(IDC_ONOFF);
  bbox->SetCheck(ship->static_wb[index].flags & WBF_ON_OFF);

  bbox = (CButton *)GetDlgItem(IDC_ZOOM);
  bbox->SetCheck(fire_flags & SFF_ZOOM);

  // Set checkboxes & enable/disable pulldown boxes
  int firing_sound = Ships[m_current_ship].firing_sound[index];
  int release_sound = Ships[m_current_ship].firing_release_sound[index];
  bbox = (CButton *)GetDlgItem(IDC_CONTINUOUS_FIRING_SOUND);
  bbox->SetCheck(firing_sound != -1);
  ((CButton *)GetDlgItem(IDC_FIRING_SOUND_PULLDOWN))->EnableWindow(firing_sound != -1);

  bbox = (CButton *)GetDlgItem(IDC_FIRING_RELEASE_SOUND);
  bbox->SetCheck(release_sound != -1);
  ((CButton *)GetDlgItem(IDC_RELEASE_SOUND_PULLDOWN))->EnableWindow(release_sound != -1);

  // Update sounds in pulldown box
  SendDlgItemMessage(IDC_FIRING_SOUND_PULLDOWN, CB_SELECTSTRING, 0,
                     (LPARAM)(LPCTSTR)((firing_sound == -1) ? Sounds[0].name : Sounds[firing_sound].name));
  SendDlgItemMessage(IDC_RELEASE_SOUND_PULLDOWN, CB_SELECTSTRING, 0,
                     (LPARAM)(LPCTSTR)((release_sound == -1) ? Sounds[0].name : Sounds[release_sound].name));

  // Update powerup pulldown box
try_again:;
  result = SendDlgItemMessage(IDC_SPEW_POWERUP_PULLDOWN, CB_SELECTSTRING, 0,
                              (LPARAM)(LPCTSTR)((Ships[m_current_ship].spew_powerup[index] == -1)
                                                    ? NULL_NAME
                                                    : Object_info[Ships[m_current_ship].spew_powerup[index]].name));
  if (result == -1) {
    Int3(); // Can't find requested powerup in list, so spew powerup to none.
    Ships[m_current_ship].spew_powerup[index] = -1;
    goto try_again;
  }

  // Update max ammo
  char str[200];
  bbox = (CButton *)GetDlgItem(IDC_MAX_AMMO);
  sprintf(str, "%d", ship->max_ammo[index]);
  bbox->SetWindowText(str);

  // Update tenths checkbox
  bbox = (CButton *)GetDlgItem(IDC_SHOW_TENTHS);
  bbox->SetCheck(fire_flags & SFF_TENTHS);

  // Add new UpdateDialog stuff here.  -- note from Chris
}

void PlayerWeaponsDialog::OnSelendokCurrentWeaponBatteryCombo() {
  int cur;
  char name[200];

  // if(!UpdateData(true)) return;

  cur = SendDlgItemMessage(IDC_CURRENT_WEAPON_BATTERY_COMBO, CB_GETCURSEL, 0, 0);
  SendDlgItemMessage(IDC_CURRENT_WEAPON_BATTERY_COMBO, CB_GETLBTEXT, cur, (LPARAM)(LPCTSTR)name);

  m_current_wb_text = name;

  // UpdateData(false);

  UpdateDialog();
}

void PlayerWeaponsDialog::OnFiresFusion() {
  int i = CurrentWBIndex();

  Ships[m_current_ship].fire_flags[i] &= ~(SFF_ZOOM);
  Ships[m_current_ship].static_wb[i].flags &= ~(WBF_ON_OFF);
  Ships[m_current_ship].fire_flags[i] |= SFF_FUSION;
  UpdateDialog();
}

void PlayerWeaponsDialog::OnContinuousFiringSound() {
  int i = CurrentWBIndex();
  CButton *btn;

  btn = (CButton *)GetDlgItem(IDC_CONTINUOUS_FIRING_SOUND);

  if (btn->GetCheck())
    Ships[m_current_ship].firing_sound[i] = 0;
  else
    Ships[m_current_ship].firing_sound[i] = -1;

  UpdateDialog();
}

void PlayerWeaponsDialog::OnFiringReleaseSound() {
  int i = CurrentWBIndex();
  CButton *btn;

  btn = (CButton *)GetDlgItem(IDC_FIRING_RELEASE_SOUND);

  if (btn->GetCheck())
    Ships[m_current_ship].firing_release_sound[i] = 0;
  else
    Ships[m_current_ship].firing_release_sound[i] = -1;

  UpdateDialog();
}

void PlayerWeaponsDialog::OnSelchangeFiringSoundPulldown() {
  int cur;
  char name[200];

  cur = SendDlgItemMessage(IDC_FIRING_SOUND_PULLDOWN, CB_GETCURSEL, 0, 0);
  SendDlgItemMessage(IDC_FIRING_SOUND_PULLDOWN, CB_GETLBTEXT, cur, (LPARAM)(LPCTSTR)name);

  int i = CurrentWBIndex();

  for (int s = 0; s < MAX_SOUNDS; s++) {
    if (Sounds[s].used && !strcmp(name, Sounds[s].name)) {
      Ships[m_current_ship].firing_sound[i] = s;
      break;
    }
  }
}

void PlayerWeaponsDialog::OnSelchangeReleaseSoundPulldown() {
  int cur;
  char name[200];

  cur = SendDlgItemMessage(IDC_RELEASE_SOUND_PULLDOWN, CB_GETCURSEL, 0, 0);
  SendDlgItemMessage(IDC_RELEASE_SOUND_PULLDOWN, CB_GETLBTEXT, cur, (LPARAM)(LPCTSTR)name);

  int i = CurrentWBIndex();

  for (int s = 0; s < MAX_SOUNDS; s++) {
    if (Sounds[s].used && !strcmp(name, Sounds[s].name)) {
      Ships[m_current_ship].firing_release_sound[i] = s;
      break;
    }
  }
}

void PlayerWeaponsDialog::OnSelchangeSpewPowerupPulldown() {
  int cur;
  char name[200];
  int i, id;

  cur = SendDlgItemMessage(IDC_SPEW_POWERUP_PULLDOWN, CB_GETCURSEL, 0, 0);
  SendDlgItemMessage(IDC_SPEW_POWERUP_PULLDOWN, CB_GETLBTEXT, cur, (LPARAM)(LPCTSTR)name);

  i = CurrentWBIndex();
  id = FindObjectIDName(name);

  Ships[m_current_ship].spew_powerup[i] = id;
}

void PlayerWeaponsDialog::OnOnoff() {
  int i = CurrentWBIndex();

  Ships[m_current_ship].fire_flags[i] &= ~(SFF_FUSION | SFF_ZOOM);
  Ships[m_current_ship].static_wb[i].flags |= WBF_ON_OFF;
  UpdateDialog();
}

void PlayerWeaponsDialog::OnZoom() {
  int i = CurrentWBIndex();

  Ships[m_current_ship].fire_flags[i] &= ~(SFF_FUSION);
  Ships[m_current_ship].static_wb[i].flags &= ~(WBF_ON_OFF);
  Ships[m_current_ship].fire_flags[i] |= SFF_ZOOM;
  UpdateDialog();
}

void PlayerWeaponsDialog::OnKillfocusMaxAmmo() {
  int i = CurrentWBIndex();

  char str[200];
  CButton *bbox = (CButton *)GetDlgItem(IDC_MAX_AMMO);
  bbox->GetWindowText(str, sizeof(str));

  Ships[m_current_ship].max_ammo[i] = atoi(str);
}

void PlayerWeaponsDialog::OnShowTenths() {
  int i = CurrentWBIndex();
  CButton *bbox = (CButton *)GetDlgItem(IDC_SHOW_TENTHS);

  if (bbox->GetCheck())
    Ships[m_current_ship].fire_flags[i] |= SFF_TENTHS;
  else
    Ships[m_current_ship].fire_flags[i] &= ~SFF_TENTHS;

  UpdateDialog();
}
