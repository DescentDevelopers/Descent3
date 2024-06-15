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

// matcenkeypad.cpp : implementation file
//

#include "stdafx.h"
#include "editor.h"
#include "matcenkeypad.h"
#include "matcen.h"
#include "pserror.h"
#include "objinfo.h"
#include "room.h"
#include "object.h"
#include "terrain.h"
#include "hlsoundlib.h"
#include "soundload.h"
#include "sounds.h"
#include "boa.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

matcen MatCopyBuffer;
bool f_copy_buffer_valid = false;

/////////////////////////////////////////////////////////////////////////////
// matcenkeypad dialog

matcenkeypad::matcenkeypad(CWnd *pParent /*=NULL*/) : CKeypadDialog(matcenkeypad::IDD, pParent) {
  //{{AFX_DATA_INIT(matcenkeypad)
  // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT

  m_matcen_id = 0;
}

void matcenkeypad::EnableSpawnFields(int num_enabled) {
  bool f_enable = true;

  if (num_enabled < 1)
    f_enable = false;
  ((CButton *)GetDlgItem(IDC_MAT_SPAWN0_EDIT))->EnableWindow(f_enable);
  if (num_enabled < 2)
    f_enable = false;
  ((CButton *)GetDlgItem(IDC_MAT_SPAWN1_EDIT))->EnableWindow(f_enable);
  if (num_enabled < 3)
    f_enable = false;
  ((CButton *)GetDlgItem(IDC_MAT_SPAWN2_EDIT))->EnableWindow(f_enable);
  if (num_enabled < 4)
    f_enable = false;
  ((CButton *)GetDlgItem(IDC_MAT_SPAWN3_EDIT))->EnableWindow(f_enable);
}

void matcenkeypad::EnableProdFields(int num_enabled) {
  bool f_enable = true;

  if (num_enabled < 1)
    f_enable = false;
  ((CButton *)GetDlgItem(IDC_MAT_PROD_0_COMBO))->EnableWindow(f_enable);
  ((CButton *)GetDlgItem(IDC_MAT_PROD_0_QUATITY_EDIT))->EnableWindow(f_enable);
  ((CButton *)GetDlgItem(IDC_MAT_PROD_0_SEC_EDIT))->EnableWindow(f_enable);
  ((CButton *)GetDlgItem(IDC_MAT_PROD_0_PRIORITY_EDIT))->EnableWindow(f_enable);
  if (num_enabled < 2)
    f_enable = false;
  ((CButton *)GetDlgItem(IDC_MAT_PROD_1_COMBO))->EnableWindow(f_enable);
  ((CButton *)GetDlgItem(IDC_MAT_PROD_1_QUATITY_EDIT))->EnableWindow(f_enable);
  ((CButton *)GetDlgItem(IDC_MAT_PROD_1_SEC_EDIT))->EnableWindow(f_enable);
  ((CButton *)GetDlgItem(IDC_MAT_PROD_1_PRIORITY_EDIT))->EnableWindow(f_enable);
  if (num_enabled < 3)
    f_enable = false;
  ((CButton *)GetDlgItem(IDC_MAT_PROD_2_COMBO))->EnableWindow(f_enable);
  ((CButton *)GetDlgItem(IDC_MAT_PROD_2_QUATITY_EDIT))->EnableWindow(f_enable);
  ((CButton *)GetDlgItem(IDC_MAT_PROD_2_SEC_EDIT))->EnableWindow(f_enable);
  ((CButton *)GetDlgItem(IDC_MAT_PROD_2_PRIORITY_EDIT))->EnableWindow(f_enable);
  if (num_enabled < 4)
    f_enable = false;
  ((CButton *)GetDlgItem(IDC_MAT_PROD_3_COMBO))->EnableWindow(f_enable);
  ((CButton *)GetDlgItem(IDC_MAT_PROD_3_QUATITY_EDIT))->EnableWindow(f_enable);
  ((CButton *)GetDlgItem(IDC_MAT_PROD_3_SEC_EDIT))->EnableWindow(f_enable);
  ((CButton *)GetDlgItem(IDC_MAT_PROD_3_PRIORITY_EDIT))->EnableWindow(f_enable);
  if (num_enabled < 5)
    f_enable = false;
  ((CButton *)GetDlgItem(IDC_MAT_PROD_4_COMBO))->EnableWindow(f_enable);
  ((CButton *)GetDlgItem(IDC_MAT_PROD_4_QUATITY_EDIT))->EnableWindow(f_enable);
  ((CButton *)GetDlgItem(IDC_MAT_PROD_4_SEC_EDIT))->EnableWindow(f_enable);
  ((CButton *)GetDlgItem(IDC_MAT_PROD_4_PRIORITY_EDIT))->EnableWindow(f_enable);
  if (num_enabled < 6)
    f_enable = false;
  ((CButton *)GetDlgItem(IDC_MAT_PROD_5_COMBO))->EnableWindow(f_enable);
  ((CButton *)GetDlgItem(IDC_MAT_PROD_5_QUATITY_EDIT))->EnableWindow(f_enable);
  ((CButton *)GetDlgItem(IDC_MAT_PROD_5_SEC_EDIT))->EnableWindow(f_enable);
  ((CButton *)GetDlgItem(IDC_MAT_PROD_5_PRIORITY_EDIT))->EnableWindow(f_enable);
  if (num_enabled < 7)
    f_enable = false;
  ((CButton *)GetDlgItem(IDC_MAT_PROD_6_COMBO))->EnableWindow(f_enable);
  ((CButton *)GetDlgItem(IDC_MAT_PROD_6_QUATITY_EDIT))->EnableWindow(f_enable);
  ((CButton *)GetDlgItem(IDC_MAT_PROD_6_SEC_EDIT))->EnableWindow(f_enable);
  ((CButton *)GetDlgItem(IDC_MAT_PROD_6_PRIORITY_EDIT))->EnableWindow(f_enable);
  if (num_enabled < 8)
    f_enable = false;
  ((CButton *)GetDlgItem(IDC_MAT_PROD_7_COMBO))->EnableWindow(f_enable);
  ((CButton *)GetDlgItem(IDC_MAT_PROD_7_QUATITY_EDIT))->EnableWindow(f_enable);
  ((CButton *)GetDlgItem(IDC_MAT_PROD_7_SEC_EDIT))->EnableWindow(f_enable);
  ((CButton *)GetDlgItem(IDC_MAT_PROD_7_PRIORITY_EDIT))->EnableWindow(f_enable);
}

void matcenkeypad::EnableFields(bool f_enable) {
  if (f_enable) {
    EnableSpawnFields(Matcen[m_matcen_id]->GetNumSpawnPnts());
    EnableProdFields(Matcen[m_matcen_id]->GetNumProdTypes());

    PrintToDlgItem(this, IDC_MAT_CUR_STATIC, "Current Matcen: %d", m_matcen_id + 1);
    PrintToDlgItem(this, IDC_MAT_NUM_STATIC, "Number of Matcens: %d", Num_matcens);
  } else {
    EnableSpawnFields(0);
    EnableProdFields(0);

    PrintToDlgItem(this, IDC_MAT_CUR_STATIC, "Current Matcen: NA");
    PrintToDlgItem(this, IDC_MAT_NUM_STATIC, "Number of Matcens: 0");
  }

  ((CButton *)GetDlgItem(IDC_MAT_EFFECT_PREPROD_TIME_EDIT))->EnableWindow(f_enable);
  ((CButton *)GetDlgItem(IDC_MAT_PROD_EFFECT_COMBO))->EnableWindow(f_enable);
  ((CButton *)GetDlgItem(IDC_MAT_PROD_SOUND_COMBO))->EnableWindow(f_enable);
  ((CButton *)GetDlgItem(IDC_MAT_PROD_TEXTURE_COMBO))->EnableWindow(f_enable);
  ((CButton *)GetDlgItem(IDC_MAT_DISABLED_SOUND_COMBO))->EnableWindow(f_enable);
  ((CButton *)GetDlgItem(IDC_MAT_ENABLED_SOUND_COMBO))->EnableWindow(f_enable);
  ((CButton *)GetDlgItem(IDC_MAT_SC_RADIO))->EnableWindow(f_enable);
  ((CButton *)GetDlgItem(IDC_MAT_WPV_RADIO))->EnableWindow(f_enable);
  ((CButton *)GetDlgItem(IDC_MAT_WPN_RADIO))->EnableWindow(f_enable);
  ((CButton *)GetDlgItem(IDC_MAT_APN_RADIO))->EnableWindow(f_enable);
  ((CButton *)GetDlgItem(IDC_MAT_RANDOM_RADIO))->EnableWindow(f_enable);
  ((CButton *)GetDlgItem(IDC_MAT_ORDERED_RADIO))->EnableWindow(f_enable);
  ((CButton *)GetDlgItem(IDC_MAT_ENABLED_CHECK))->EnableWindow(f_enable);
  ((CButton *)GetDlgItem(IDC_PROD_RATE_MULTI_EDIT))->EnableWindow(f_enable);
  ((CButton *)GetDlgItem(IDC_MAX_ALIVE_CHILDREN_EDIT))->EnableWindow(f_enable);
  ((CButton *)GetDlgItem(IDC_MAT_NUM_TYPES_EDIT))->EnableWindow(f_enable);
  ((CButton *)GetDlgItem(IDC_MAT_ROOM_RADIO))->EnableWindow(f_enable);
  ((CButton *)GetDlgItem(IDC_MAT_OBJECT_RADIO))->EnableWindow(f_enable);
  ((CButton *)GetDlgItem(IDC_MAT_UNASSIGNED_RADIO))->EnableWindow(f_enable);
  ((CButton *)GetDlgItem(IDC_MAT_NUM_SPAWN_EDIT))->EnableWindow(f_enable);
  ((CButton *)GetDlgItem(IDC_MAT_NAME_EDIT))->EnableWindow(f_enable);
  ((CButton *)GetDlgItem(IDC_MAT_MOVE_VIEWER_BUTTON))->EnableWindow(f_enable);
  ((CButton *)GetDlgItem(IDC_MAT_PASTE_OBJREF_BUTTON))->EnableWindow(f_enable);
  ((CButton *)GetDlgItem(IDC_MAT_PREV_BUTTON))->EnableWindow(f_enable);
  ((CButton *)GetDlgItem(IDC_MAT_NEXT_BUTTON))->EnableWindow(f_enable);

  ((CButton *)GetDlgItem(IDC_MAT_NEW_BUTTON))->EnableWindow(Num_matcens < MAX_MATCENS);

  ((CButton *)GetDlgItem(IDC_MAT_DELETE_BUTTON))->EnableWindow(f_enable);
  ((CButton *)GetDlgItem(IDC_MAT_COPY_BUTTON))->EnableWindow(f_enable);
  ((CButton *)GetDlgItem(IDC_MAT_PASTE_BUTTON))->EnableWindow(f_enable);
  ((CButton *)GetDlgItem(IDC_MAT_APV_RADIO))->EnableWindow(f_enable);
  //	((CButton *)GetDlgItem(IDC_MAT_ATTACH_EDIT))->EnableWindow(f_enable);
  ((CButton *)GetDlgItem(IDC_MAT_ROOM_OBJECT_COMBO))->EnableWindow(f_enable);
  ((CButton *)GetDlgItem(IDC_MAT_EFFECT_POSTPROD_TIME_EDIT))->EnableWindow(f_enable);
  ((CButton *)GetDlgItem(IDC_MAX_OBJ_PROD_EDIT))->EnableWindow(f_enable);
}

void matcenkeypad::DoDataExchange(CDataExchange *pDX) {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(matcenkeypad)
  // NOTE: the ClassWizard will add DDX and DDV calls here
  //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(matcenkeypad, CDialog)
//{{AFX_MSG_MAP(matcenkeypad)
ON_WM_VSCROLL()
ON_WM_SIZE()
ON_WM_HSCROLL()
ON_EN_KILLFOCUS(IDC_MAT_EFFECT_PREPROD_TIME_EDIT, OnKillfocusMatEffectPreprodTimeEdit)
ON_CBN_SELENDOK(IDC_MAT_PROD_EFFECT_COMBO, OnSelendokMatProdEffectCombo)
ON_CBN_SELENDOK(IDC_MAT_PROD_SOUND_COMBO, OnSelendokMatProdSoundCombo)
ON_CBN_SELENDOK(IDC_MAT_DISABLED_SOUND_COMBO, OnSelendokMatDisabledSoundCombo)
ON_CBN_SELENDOK(IDC_MAT_ENABLED_SOUND_COMBO, OnSelendokMatEnabledSoundCombo)
ON_BN_CLICKED(IDC_MAT_SC_RADIO, OnMatScRadio)
ON_BN_CLICKED(IDC_MAT_WPV_RADIO, OnMatWpvRadio)
ON_BN_CLICKED(IDC_MAT_WPN_RADIO, OnMatWpnRadio)
ON_BN_CLICKED(IDC_MAT_APN_RADIO, OnMatApnRadio)
ON_BN_CLICKED(IDC_MAT_RANDOM_RADIO, OnMatRandomRadio)
ON_BN_CLICKED(IDC_MAT_ORDERED_RADIO, OnMatOrderedRadio)
ON_BN_CLICKED(IDC_MAT_ENABLED_CHECK, OnMatEnabledCheck)
ON_EN_KILLFOCUS(IDC_PROD_RATE_MULTI_EDIT, OnKillfocusProdRateMultiEdit)
ON_EN_KILLFOCUS(IDC_MAX_ALIVE_CHILDREN_EDIT, OnKillfocusMaxAliveChildrenEdit)
ON_CBN_SELENDOK(IDC_MAT_PROD_0_COMBO, OnSelendokMatProd0Combo)
ON_CBN_SELENDOK(IDC_MAT_PROD_1_COMBO, OnSelendokMatProd1Combo)
ON_CBN_SELENDOK(IDC_MAT_PROD_2_COMBO, OnSelendokMatProd2Combo)
ON_CBN_SELENDOK(IDC_MAT_PROD_3_COMBO, OnSelendokMatProd3Combo)
ON_CBN_SELENDOK(IDC_MAT_PROD_4_COMBO, OnSelendokMatProd4Combo)
ON_CBN_SELENDOK(IDC_MAT_PROD_5_COMBO, OnSelendokMatProd5Combo)
ON_CBN_SELENDOK(IDC_MAT_PROD_6_COMBO, OnSelendokMatProd6Combo)
ON_CBN_SELENDOK(IDC_MAT_PROD_7_COMBO, OnSelendokMatProd7Combo)
ON_EN_KILLFOCUS(IDC_MAT_PROD_0_QUATITY_EDIT, OnKillfocusMatProd0QuatityEdit)
ON_EN_KILLFOCUS(IDC_MAT_PROD_1_QUATITY_EDIT, OnKillfocusMatProd1QuatityEdit)
ON_EN_KILLFOCUS(IDC_MAT_PROD_2_QUATITY_EDIT, OnKillfocusMatProd2QuatityEdit)
ON_EN_KILLFOCUS(IDC_MAT_PROD_3_QUATITY_EDIT, OnKillfocusMatProd3QuatityEdit)
ON_EN_KILLFOCUS(IDC_MAT_PROD_4_QUATITY_EDIT, OnKillfocusMatProd4QuatityEdit)
ON_EN_KILLFOCUS(IDC_MAT_PROD_5_QUATITY_EDIT, OnKillfocusMatProd5QuatityEdit)
ON_EN_KILLFOCUS(IDC_MAT_PROD_6_QUATITY_EDIT, OnKillfocusMatProd6QuatityEdit)
ON_EN_KILLFOCUS(IDC_MAT_PROD_7_QUATITY_EDIT, OnKillfocusMatProd7QuatityEdit)
ON_EN_KILLFOCUS(IDC_MAT_PROD_0_SEC_EDIT, OnKillfocusMatProd0SecEdit)
ON_EN_KILLFOCUS(IDC_MAT_PROD_1_SEC_EDIT, OnKillfocusMatProd1SecEdit)
ON_EN_KILLFOCUS(IDC_MAT_PROD_2_SEC_EDIT, OnKillfocusMatProd2SecEdit)
ON_EN_KILLFOCUS(IDC_MAT_PROD_3_SEC_EDIT, OnKillfocusMatProd3SecEdit)
ON_EN_KILLFOCUS(IDC_MAT_PROD_4_SEC_EDIT, OnKillfocusMatProd4SecEdit)
ON_EN_KILLFOCUS(IDC_MAT_PROD_5_SEC_EDIT, OnKillfocusMatProd5SecEdit)
ON_EN_KILLFOCUS(IDC_MAT_PROD_6_SEC_EDIT, OnKillfocusMatProd6SecEdit)
ON_EN_KILLFOCUS(IDC_MAT_PROD_0_PRIORITY_EDIT, OnKillfocusMatProd0PriorityEdit)
ON_EN_KILLFOCUS(IDC_MAT_PROD_1_PRIORITY_EDIT, OnKillfocusMatProd1PriorityEdit)
ON_EN_KILLFOCUS(IDC_MAT_PROD_2_PRIORITY_EDIT, OnKillfocusMatProd2PriorityEdit)
ON_EN_KILLFOCUS(IDC_MAT_PROD_3_PRIORITY_EDIT, OnKillfocusMatProd3PriorityEdit)
ON_EN_KILLFOCUS(IDC_MAT_PROD_4_PRIORITY_EDIT, OnKillfocusMatProd4PriorityEdit)
ON_EN_KILLFOCUS(IDC_MAT_PROD_5_PRIORITY_EDIT, OnKillfocusMatProd5PriorityEdit)
ON_EN_KILLFOCUS(IDC_MAT_PROD_6_PRIORITY_EDIT, OnKillfocusMatProd6PriorityEdit)
ON_EN_KILLFOCUS(IDC_MAT_PROD_7_PRIORITY_EDIT, OnKillfocusMatProd7PriorityEdit)
ON_EN_KILLFOCUS(IDC_MAT_NUM_TYPES_EDIT, OnKillfocusMatNumTypesEdit)
ON_EN_KILLFOCUS(IDC_MAT_SPAWN0_EDIT, OnKillfocusMatSpawn0Edit)
ON_EN_KILLFOCUS(IDC_MAT_SPAWN1_EDIT, OnKillfocusMatSpawn1Edit)
ON_EN_KILLFOCUS(IDC_MAT_SPAWN2_EDIT, OnKillfocusMatSpawn2Edit)
ON_EN_KILLFOCUS(IDC_MAT_SPAWN3_EDIT, OnKillfocusMatSpawn3Edit)
ON_BN_CLICKED(IDC_MAT_ROOM_RADIO, OnMatRoomRadio)
ON_BN_CLICKED(IDC_MAT_OBJECT_RADIO, OnMatObjectRadio)
ON_BN_CLICKED(IDC_MAT_UNASSIGNED_RADIO, OnMatUnassignedRadio)
ON_EN_KILLFOCUS(IDC_MAT_NUM_SPAWN_EDIT, OnKillfocusMatNumSpawnEdit)
ON_EN_KILLFOCUS(IDC_MAT_NAME_EDIT, OnKillfocusMatNameEdit)
ON_BN_CLICKED(IDC_MAT_MOVE_VIEWER_BUTTON, OnMatMoveViewerButton)
ON_BN_CLICKED(IDC_MAT_PASTE_OBJREF_BUTTON, OnMatPasteObjrefButton)
ON_BN_CLICKED(IDC_MAT_PREV_BUTTON, OnMatPrevButton)
ON_BN_CLICKED(IDC_MAT_NEXT_BUTTON, OnMatNextButton)
ON_BN_CLICKED(IDC_MAT_NEW_BUTTON, OnMatNewButton)
ON_BN_CLICKED(IDC_MAT_DELETE_BUTTON, OnMatDeleteButton)
ON_BN_CLICKED(IDC_MAT_COPY_BUTTON, OnMatCopyButton)
ON_BN_CLICKED(IDC_MAT_PASTE_BUTTON, OnMatPasteButton)
ON_BN_CLICKED(IDC_MAT_APV_RADIO, OnMatApvRadio)
ON_EN_KILLFOCUS(IDC_MAT_EFFECT_POSTPROD_TIME_EDIT, OnKillfocusMatEffectPostprodTimeEdit)
ON_EN_KILLFOCUS(IDC_MAT_PROD_7_SEC_EDIT, OnKillfocusMatProd7SecEdit)
ON_EN_KILLFOCUS(IDC_MAX_OBJ_PROD_EDIT, OnKillfocusMaxObjProdEdit)
ON_WM_PAINT()
ON_CBN_SELENDOK(IDC_MAT_PROD_TEXTURE_COMBO, OnSelendokMatProdTextureCombo)
ON_BN_CLICKED(IDC_MAT_NHP_CHECK, OnMatNhpCheck)
ON_CBN_SELENDOK(IDC_MAT_ROOM_OBJECT_COMBO, OnSelendokMatRoomObjectCombo)
ON_WM_DESTROY()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// matcenkeypad message handlers
//
//	CKeypadDialog::OnHScroll(nSBCode, nPos, pScrollBar);

void matcenkeypad::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar) {
  CKeypadDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

void matcenkeypad::OnSize(UINT nType, int cx, int cy) { CKeypadDialog::OnSize(nType, cx, cy); }

void matcenkeypad::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar) {
  CKeypadDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void matcenkeypad::OnKillfocusMatEffectPreprodTimeEdit() {
  char str[20];
  float val;

  ((CEdit *)GetDlgItem(IDC_MAT_EFFECT_PREPROD_TIME_EDIT))->GetWindowText(str, 20);

  val = atof(str);

  Matcen[m_matcen_id]->SetPreProdTime(val);

  UpdateDialog();
}

void matcenkeypad::OnSelendokMatProdEffectCombo() {
  int i;

  i = SendDlgItemMessage(IDC_MAT_PROD_EFFECT_COMBO, CB_GETCURSEL, 0, 0);

  Matcen[m_matcen_id]->SetCreationEffect(i);

  UpdateDialog();
}

void matcenkeypad::OnSelendokMatProdSoundCombo() {
  int i, cur;
  char name[200];

  cur = SendDlgItemMessage(IDC_MAT_PROD_SOUND_COMBO, CB_GETCURSEL, 0, 0);
  SendDlgItemMessage(IDC_MAT_PROD_SOUND_COMBO, CB_GETLBTEXT, cur, (LPARAM)(LPCTSTR)name);

  i = FindSoundName(name);

  if (i == -1) {
    i = SOUND_NONE_INDEX;
  }

  Matcen[m_matcen_id]->SetSound(MATCEN_PROD_SOUND, i);

  UpdateDialog();
}

void matcenkeypad::OnSelendokMatDisabledSoundCombo() {
  int i, cur;
  char name[200];

  cur = SendDlgItemMessage(IDC_MAT_DISABLED_SOUND_COMBO, CB_GETCURSEL, 0, 0);
  SendDlgItemMessage(IDC_MAT_DISABLED_SOUND_COMBO, CB_GETLBTEXT, cur, (LPARAM)(LPCTSTR)name);

  i = FindSoundName(name);

  if (i == -1) {
    i = SOUND_NONE_INDEX;
  }

  Matcen[m_matcen_id]->SetSound(MATCEN_DISABLE_SOUND, i);

  UpdateDialog();
}

void matcenkeypad::OnSelendokMatEnabledSoundCombo() {
  int i, cur;
  char name[200];

  cur = SendDlgItemMessage(IDC_MAT_ENABLED_SOUND_COMBO, CB_GETCURSEL, 0, 0);
  SendDlgItemMessage(IDC_MAT_ENABLED_SOUND_COMBO, CB_GETLBTEXT, cur, (LPARAM)(LPCTSTR)name);

  i = FindSoundName(name);

  if (i == -1) {
    i = SOUND_NONE_INDEX;
  }

  Matcen[m_matcen_id]->SetSound(MATCEN_ACTIVE_SOUND, i);

  UpdateDialog();
}

void matcenkeypad::OnMatScRadio() {
  Matcen[m_matcen_id]->SetControlType(MPC_SCRIPT);
  UpdateDialog();
}

void matcenkeypad::OnMatWpvRadio() {
  Matcen[m_matcen_id]->SetControlType(MPC_WHILE_PLAYER_VISIBLE);
  UpdateDialog();
}

void matcenkeypad::OnMatWpnRadio() {
  Matcen[m_matcen_id]->SetControlType(MPC_WHILE_PLAYER_NEAR);
  UpdateDialog();
}

void matcenkeypad::OnMatApnRadio() {
  Matcen[m_matcen_id]->SetControlType(MPC_AFTER_PLAYER_NEAR);
  UpdateDialog();
}

void matcenkeypad::OnMatRandomRadio() {
  Matcen[m_matcen_id]->SetStatus(MSTAT_RANDOM_PROD_ORDER, true);
  UpdateDialog();
}

void matcenkeypad::OnMatOrderedRadio() {
  Matcen[m_matcen_id]->SetStatus(MSTAT_RANDOM_PROD_ORDER, false);
  UpdateDialog();
}

void matcenkeypad::OnMatEnabledCheck() {
  int status = Matcen[m_matcen_id]->GetStatus();

  if (status & MSTAT_DISABLED)
    Matcen[m_matcen_id]->SetStatus(MSTAT_DISABLED, false);
  else
    Matcen[m_matcen_id]->SetStatus(MSTAT_DISABLED, true);

  UpdateDialog();
}

void matcenkeypad::OnKillfocusProdRateMultiEdit() {
  char str[20];
  float val;

  ((CEdit *)GetDlgItem(IDC_PROD_RATE_MULTI_EDIT))->GetWindowText(str, 20);

  val = atof(str);
  Matcen[m_matcen_id]->SetProdMultiplier(val);

  UpdateDialog();
}

void matcenkeypad::OnKillfocusMaxAliveChildrenEdit() {
  char str[20];
  int val;

  ((CEdit *)GetDlgItem(IDC_MAX_ALIVE_CHILDREN_EDIT))->GetWindowText(str, 20);

  val = atoi(str);
  Matcen[m_matcen_id]->SetMaxAliveChildren(val);

  UpdateDialog();
}

void matcenkeypad::OnKillfocusMaxObjProdEdit() {
  char str[20];
  int val;

  ((CEdit *)GetDlgItem(IDC_MAX_OBJ_PROD_EDIT))->GetWindowText(str, 20);

  val = atoi(str);
  Matcen[m_matcen_id]->SetMaxProd(val);

  UpdateDialog();
}

void matcenkeypad::OnSelendokMatProd0Combo() {
  int cur;
  char name[200];

  cur = SendDlgItemMessage(IDC_MAT_PROD_0_COMBO, CB_GETCURSEL, 0, 0);
  SendDlgItemMessage(IDC_MAT_PROD_0_COMBO, CB_GETLBTEXT, cur, (LPARAM)(LPCTSTR)name);

  int obj_id = FindObjectIDName(name);
  Matcen[m_matcen_id]->SetProdInfo(0, &obj_id, NULL, NULL, NULL);

  UpdateDialog();
}

void matcenkeypad::OnSelendokMatProd1Combo() {
  int cur;
  char name[200];

  cur = SendDlgItemMessage(IDC_MAT_PROD_1_COMBO, CB_GETCURSEL, 0, 0);
  SendDlgItemMessage(IDC_MAT_PROD_1_COMBO, CB_GETLBTEXT, cur, (LPARAM)(LPCTSTR)name);

  int obj_id = FindObjectIDName(name);
  Matcen[m_matcen_id]->SetProdInfo(1, &obj_id, NULL, NULL, NULL);

  UpdateDialog();
}

void matcenkeypad::OnSelendokMatProd2Combo() {
  int cur;
  char name[200];

  cur = SendDlgItemMessage(IDC_MAT_PROD_2_COMBO, CB_GETCURSEL, 0, 0);
  SendDlgItemMessage(IDC_MAT_PROD_2_COMBO, CB_GETLBTEXT, cur, (LPARAM)(LPCTSTR)name);

  int obj_id = FindObjectIDName(name);
  Matcen[m_matcen_id]->SetProdInfo(2, &obj_id, NULL, NULL, NULL);

  UpdateDialog();
}

void matcenkeypad::OnSelendokMatProd3Combo() {
  int cur;
  char name[200];

  cur = SendDlgItemMessage(IDC_MAT_PROD_3_COMBO, CB_GETCURSEL, 0, 0);
  SendDlgItemMessage(IDC_MAT_PROD_3_COMBO, CB_GETLBTEXT, cur, (LPARAM)(LPCTSTR)name);

  int obj_id = FindObjectIDName(name);
  Matcen[m_matcen_id]->SetProdInfo(3, &obj_id, NULL, NULL, NULL);

  UpdateDialog();
}

void matcenkeypad::OnSelendokMatProd4Combo() {
  int cur;
  char name[200];

  cur = SendDlgItemMessage(IDC_MAT_PROD_4_COMBO, CB_GETCURSEL, 0, 0);
  SendDlgItemMessage(IDC_MAT_PROD_4_COMBO, CB_GETLBTEXT, cur, (LPARAM)(LPCTSTR)name);

  int obj_id = FindObjectIDName(name);
  Matcen[m_matcen_id]->SetProdInfo(4, &obj_id, NULL, NULL, NULL);

  UpdateDialog();
}

void matcenkeypad::OnSelendokMatProd5Combo() {
  int cur;
  char name[200];

  cur = SendDlgItemMessage(IDC_MAT_PROD_5_COMBO, CB_GETCURSEL, 0, 0);
  SendDlgItemMessage(IDC_MAT_PROD_5_COMBO, CB_GETLBTEXT, cur, (LPARAM)(LPCTSTR)name);

  int obj_id = FindObjectIDName(name);
  Matcen[m_matcen_id]->SetProdInfo(5, &obj_id, NULL, NULL, NULL);

  UpdateDialog();
}

void matcenkeypad::OnSelendokMatProd6Combo() {
  int cur;
  char name[200];

  cur = SendDlgItemMessage(IDC_MAT_PROD_6_COMBO, CB_GETCURSEL, 0, 0);
  SendDlgItemMessage(IDC_MAT_PROD_6_COMBO, CB_GETLBTEXT, cur, (LPARAM)(LPCTSTR)name);

  int obj_id = FindObjectIDName(name);
  Matcen[m_matcen_id]->SetProdInfo(6, &obj_id, NULL, NULL, NULL);

  UpdateDialog();
}

void matcenkeypad::OnSelendokMatProd7Combo() {
  int cur;
  char name[200];

  cur = SendDlgItemMessage(IDC_MAT_PROD_7_COMBO, CB_GETCURSEL, 0, 0);
  SendDlgItemMessage(IDC_MAT_PROD_7_COMBO, CB_GETLBTEXT, cur, (LPARAM)(LPCTSTR)name);

  int obj_id = FindObjectIDName(name);
  Matcen[m_matcen_id]->SetProdInfo(7, &obj_id, NULL, NULL, NULL);

  UpdateDialog();
}

void matcenkeypad::OnKillfocusMatProd0QuatityEdit() {
  char str[20];
  int val;

  ((CEdit *)GetDlgItem(IDC_MAT_PROD_0_QUATITY_EDIT))->GetWindowText(str, 20);

  val = atoi(str);
  Matcen[m_matcen_id]->SetProdInfo(0, NULL, NULL, NULL, &val);

  UpdateDialog();
}

void matcenkeypad::OnKillfocusMatProd1QuatityEdit() {
  char str[20];
  int val;

  ((CEdit *)GetDlgItem(IDC_MAT_PROD_1_QUATITY_EDIT))->GetWindowText(str, 20);

  val = atoi(str);
  Matcen[m_matcen_id]->SetProdInfo(1, NULL, NULL, NULL, &val);

  UpdateDialog();
}

void matcenkeypad::OnKillfocusMatProd2QuatityEdit() {
  char str[20];
  int val;

  ((CEdit *)GetDlgItem(IDC_MAT_PROD_2_QUATITY_EDIT))->GetWindowText(str, 20);

  val = atoi(str);
  Matcen[m_matcen_id]->SetProdInfo(2, NULL, NULL, NULL, &val);

  UpdateDialog();
}

void matcenkeypad::OnKillfocusMatProd3QuatityEdit() {
  char str[20];
  int val;

  ((CEdit *)GetDlgItem(IDC_MAT_PROD_3_QUATITY_EDIT))->GetWindowText(str, 20);

  val = atoi(str);
  Matcen[m_matcen_id]->SetProdInfo(3, NULL, NULL, NULL, &val);

  UpdateDialog();
}

void matcenkeypad::OnKillfocusMatProd4QuatityEdit() {
  char str[20];
  int val;

  ((CEdit *)GetDlgItem(IDC_MAT_PROD_4_QUATITY_EDIT))->GetWindowText(str, 20);

  val = atoi(str);
  Matcen[m_matcen_id]->SetProdInfo(4, NULL, NULL, NULL, &val);

  UpdateDialog();
}

void matcenkeypad::OnKillfocusMatProd5QuatityEdit() {
  char str[20];
  int val;

  ((CEdit *)GetDlgItem(IDC_MAT_PROD_5_QUATITY_EDIT))->GetWindowText(str, 20);

  val = atoi(str);
  Matcen[m_matcen_id]->SetProdInfo(5, NULL, NULL, NULL, &val);

  UpdateDialog();
}

void matcenkeypad::OnKillfocusMatProd6QuatityEdit() {
  char str[20];
  int val;

  ((CEdit *)GetDlgItem(IDC_MAT_PROD_6_QUATITY_EDIT))->GetWindowText(str, 20);

  val = atoi(str);
  Matcen[m_matcen_id]->SetProdInfo(6, NULL, NULL, NULL, &val);

  UpdateDialog();
}

void matcenkeypad::OnKillfocusMatProd7QuatityEdit() {
  char str[20];
  int val;

  ((CEdit *)GetDlgItem(IDC_MAT_PROD_7_QUATITY_EDIT))->GetWindowText(str, 20);

  val = atoi(str);
  Matcen[m_matcen_id]->SetProdInfo(7, NULL, NULL, NULL, &val);

  UpdateDialog();
}

void matcenkeypad::OnKillfocusMatProd0SecEdit() {
  char str[20];
  float val;

  ((CEdit *)GetDlgItem(IDC_MAT_PROD_0_SEC_EDIT))->GetWindowText(str, 20);

  val = atof(str);
  Matcen[m_matcen_id]->SetProdInfo(0, NULL, NULL, &val, NULL);

  UpdateDialog();
}

void matcenkeypad::OnKillfocusMatProd1SecEdit() {
  char str[20];
  float val;

  ((CEdit *)GetDlgItem(IDC_MAT_PROD_1_SEC_EDIT))->GetWindowText(str, 20);

  val = atof(str);
  Matcen[m_matcen_id]->SetProdInfo(1, NULL, NULL, &val, NULL);

  UpdateDialog();
}

void matcenkeypad::OnKillfocusMatProd2SecEdit() {
  char str[20];
  float val;

  ((CEdit *)GetDlgItem(IDC_MAT_PROD_2_SEC_EDIT))->GetWindowText(str, 20);

  val = atof(str);
  Matcen[m_matcen_id]->SetProdInfo(2, NULL, NULL, &val, NULL);

  UpdateDialog();
}

void matcenkeypad::OnKillfocusMatProd3SecEdit() {
  char str[20];
  float val;

  ((CEdit *)GetDlgItem(IDC_MAT_PROD_3_SEC_EDIT))->GetWindowText(str, 20);

  val = atof(str);
  Matcen[m_matcen_id]->SetProdInfo(3, NULL, NULL, &val, NULL);

  UpdateDialog();
}

void matcenkeypad::OnKillfocusMatProd4SecEdit() {
  char str[20];
  float val;

  ((CEdit *)GetDlgItem(IDC_MAT_PROD_4_SEC_EDIT))->GetWindowText(str, 20);

  val = atof(str);
  Matcen[m_matcen_id]->SetProdInfo(4, NULL, NULL, &val, NULL);

  UpdateDialog();
}

void matcenkeypad::OnKillfocusMatProd5SecEdit() {
  char str[20];
  float val;

  ((CEdit *)GetDlgItem(IDC_MAT_PROD_5_SEC_EDIT))->GetWindowText(str, 20);

  val = atof(str);
  Matcen[m_matcen_id]->SetProdInfo(5, NULL, NULL, &val, NULL);

  UpdateDialog();
}

void matcenkeypad::OnKillfocusMatProd6SecEdit() {
  char str[20];
  float val;

  ((CEdit *)GetDlgItem(IDC_MAT_PROD_6_SEC_EDIT))->GetWindowText(str, 20);

  val = atof(str);
  Matcen[m_matcen_id]->SetProdInfo(6, NULL, NULL, &val, NULL);

  UpdateDialog();
}

void matcenkeypad::OnKillfocusMatProd7SecEdit() {
  char str[20];
  float val;

  ((CEdit *)GetDlgItem(IDC_MAT_PROD_7_SEC_EDIT))->GetWindowText(str, 20);

  val = atof(str);
  Matcen[m_matcen_id]->SetProdInfo(7, NULL, NULL, &val, NULL);

  UpdateDialog();
}

void matcenkeypad::OnKillfocusMatProd0PriorityEdit() {
  char str[20];
  int val;

  ((CEdit *)GetDlgItem(IDC_MAT_PROD_0_PRIORITY_EDIT))->GetWindowText(str, 20);

  val = atoi(str);
  Matcen[m_matcen_id]->SetProdInfo(0, NULL, &val, NULL, NULL);

  UpdateDialog();
}

void matcenkeypad::OnKillfocusMatProd1PriorityEdit() {
  char str[20];
  int val;

  ((CEdit *)GetDlgItem(IDC_MAT_PROD_1_PRIORITY_EDIT))->GetWindowText(str, 20);

  val = atoi(str);
  Matcen[m_matcen_id]->SetProdInfo(1, NULL, &val, NULL, NULL);

  UpdateDialog();
}

void matcenkeypad::OnKillfocusMatProd2PriorityEdit() {
  char str[20];
  int val;

  ((CEdit *)GetDlgItem(IDC_MAT_PROD_2_PRIORITY_EDIT))->GetWindowText(str, 20);

  val = atoi(str);
  Matcen[m_matcen_id]->SetProdInfo(2, NULL, &val, NULL, NULL);

  UpdateDialog();
}

void matcenkeypad::OnKillfocusMatProd3PriorityEdit() {
  char str[20];
  int val;

  ((CEdit *)GetDlgItem(IDC_MAT_PROD_3_PRIORITY_EDIT))->GetWindowText(str, 20);

  val = atoi(str);
  Matcen[m_matcen_id]->SetProdInfo(3, NULL, &val, NULL, NULL);

  UpdateDialog();
}

void matcenkeypad::OnKillfocusMatProd4PriorityEdit() {
  char str[20];
  int val;

  ((CEdit *)GetDlgItem(IDC_MAT_PROD_4_PRIORITY_EDIT))->GetWindowText(str, 20);

  val = atoi(str);
  Matcen[m_matcen_id]->SetProdInfo(4, NULL, &val, NULL, NULL);

  UpdateDialog();
}

void matcenkeypad::OnKillfocusMatProd5PriorityEdit() {
  char str[20];
  int val;

  ((CEdit *)GetDlgItem(IDC_MAT_PROD_5_PRIORITY_EDIT))->GetWindowText(str, 20);

  val = atoi(str);
  Matcen[m_matcen_id]->SetProdInfo(5, NULL, &val, NULL, NULL);

  UpdateDialog();
}

void matcenkeypad::OnKillfocusMatProd6PriorityEdit() {
  char str[20];
  int val;

  ((CEdit *)GetDlgItem(IDC_MAT_PROD_6_PRIORITY_EDIT))->GetWindowText(str, 20);

  val = atoi(str);
  Matcen[m_matcen_id]->SetProdInfo(6, NULL, &val, NULL, NULL);

  UpdateDialog();
}

void matcenkeypad::OnKillfocusMatProd7PriorityEdit() {
  char str[20];
  int val;

  ((CEdit *)GetDlgItem(IDC_MAT_PROD_7_PRIORITY_EDIT))->GetWindowText(str, 20);

  val = atoi(str);
  Matcen[m_matcen_id]->SetProdInfo(7, NULL, &val, NULL, NULL);

  UpdateDialog();
}

void matcenkeypad::OnKillfocusMatNumTypesEdit() {
  char str[20];
  int val;

  ((CEdit *)GetDlgItem(IDC_MAT_NUM_TYPES_EDIT))->GetWindowText(str, 20);

  val = atoi(str);

  if (val < -1)
    val = -1;
  else if (val > MAX_PROD_TYPES)
    val = MAX_PROD_TYPES;

  Matcen[m_matcen_id]->SetNumProdTypes(val);

  UpdateDialog();
}

void matcenkeypad::OnKillfocusMatSpawn0Edit() {
  char str[20];
  int val;

  ((CEdit *)GetDlgItem(IDC_MAT_SPAWN0_EDIT))->GetWindowText(str, 20);

  val = atoi(str);
  Matcen[m_matcen_id]->SetSpawnPnt(0, val);

  UpdateDialog();
}

void matcenkeypad::OnKillfocusMatSpawn1Edit() {
  char str[20];
  int val;

  ((CEdit *)GetDlgItem(IDC_MAT_SPAWN1_EDIT))->GetWindowText(str, 20);

  val = atoi(str);
  Matcen[m_matcen_id]->SetSpawnPnt(1, val);

  UpdateDialog();
}

void matcenkeypad::OnKillfocusMatSpawn2Edit() {
  char str[20];
  int val;

  ((CEdit *)GetDlgItem(IDC_MAT_SPAWN2_EDIT))->GetWindowText(str, 20);

  val = atoi(str);
  Matcen[m_matcen_id]->SetSpawnPnt(2, val);

  UpdateDialog();
}

void matcenkeypad::OnKillfocusMatSpawn3Edit() {
  char str[20];
  int val;

  ((CEdit *)GetDlgItem(IDC_MAT_SPAWN3_EDIT))->GetWindowText(str, 20);

  val = atoi(str);
  Matcen[m_matcen_id]->SetSpawnPnt(3, val);

  UpdateDialog();
}

void matcenkeypad::OnMatRoomRadio() {
  Matcen[m_matcen_id]->SetAttachType(MT_ROOM);
  UpdateDialog();
}

void matcenkeypad::OnMatObjectRadio() {
  Matcen[m_matcen_id]->SetAttachType(MT_OBJECT);
  UpdateDialog();
}

void matcenkeypad::OnMatUnassignedRadio() {
  Matcen[m_matcen_id]->SetAttachType(MT_UNASSIGNED);
  UpdateDialog();
}

void matcenkeypad::OnKillfocusMatNumSpawnEdit() {
  char str[20];
  int val;

  ((CEdit *)GetDlgItem(IDC_MAT_NUM_SPAWN_EDIT))->GetWindowText(str, 20);

  val = atoi(str);

  if (val < 0)
    val = 0;
  else if (val > MAX_SPAWN_PNTS)
    val = MAX_SPAWN_PNTS;

  Matcen[m_matcen_id]->SetNumSpawnPnts(val);

  UpdateDialog();
}

void matcenkeypad::OnKillfocusMatNameEdit() {
  char str[MAX_MATCEN_NAME_LEN];

  ((CEdit *)GetDlgItem(IDC_MAT_NAME_EDIT))->GetWindowText(str, 20);

  Matcen[m_matcen_id]->SetName(str);

  UpdateDialog();
}

void matcenkeypad::OnMatMoveViewerButton() {
  char a_type = Matcen[m_matcen_id]->GetAttachType();
  int attach = Matcen[m_matcen_id]->GetAttach();
  vector c_pnt;

  MakeBOA();

  if (a_type == MT_OBJECT && ObjGet(attach) && Matcen[m_matcen_id]->GetCreatePnt(&c_pnt)) {
    ObjSetPos(Viewer_object, &c_pnt, Objects[attach].roomnum, NULL, false);
  } else if (a_type == MT_ROOM && attach >= 0 && attach <= Highest_room_index && Rooms[attach].used &&
             Matcen[m_matcen_id]->GetCreatePnt(&c_pnt)) {
    int roomnum;

    if (Rooms[attach].flags & RF_EXTERNAL) {
      roomnum = GetTerrainRoomFromPos(&c_pnt);
    } else {
      roomnum = attach;
    }

    ObjSetPos(Viewer_object, &c_pnt, roomnum, NULL, false);
    Viewer_moved = 1;
  } else {
    OutrageMessageBox(MBOX_OK,
                      "Cannot move viewer: This Matcen is either unassigned or linked to an invalid room/object.");
  }

  UpdateDialog();
}

void matcenkeypad::OnMatPasteObjrefButton() {

  Matcen[m_matcen_id]->SetAttachType(MT_OBJECT);
  Matcen[m_matcen_id]->SetAttach(Copied_object_id);

  UpdateDialog();
}

void matcenkeypad::OnMatPrevButton() {
  int i;

  if (!MatcenValid(m_matcen_id))
    return;

  for (i = m_matcen_id - 1; i >= 0; i--) {
    if (Matcen[i]) {
      m_matcen_id = i;
      UpdateDialog();
      return;
    }
  }

  for (i = Num_matcens - 1; i >= m_matcen_id; i--) {
    if (Matcen[i]) {
      m_matcen_id = i;
      UpdateDialog();
      return;
    }
  }

  UpdateDialog();
}

void matcenkeypad::OnMatNextButton() {
  int i;

  if (!MatcenValid(m_matcen_id))
    return;

  for (i = m_matcen_id + 1; i < Num_matcens; i++) {
    if (Matcen[i]) {
      m_matcen_id = i;
      UpdateDialog();
      return;
    }
  }

  for (i = 0; i <= m_matcen_id; i++) {
    if (Matcen[i]) {
      m_matcen_id = i;
      UpdateDialog();
      return;
    }
  }

  UpdateDialog();
}

void matcenkeypad::OnMatNewButton() {
  char name[MAX_MATCEN_NAME_LEN];
  bool f_name_changed;
  int id;

  strcpy(name, "temp matcen name");
  id = CreateMatcen(name, &f_name_changed);

  if (id == MATCEN_ERROR) {
    OutrageMessageBox(MB_OK, "Sorry, no free matcen slots.");
  } else {
    sprintf(name, "Matcen %d", id + 1);

    Matcen[id]->SetName(name);
    m_matcen_id = id;
  }

  UpdateDialog();
}

void matcenkeypad::OnMatDeleteButton() {
  if (MatcenValid(m_matcen_id)) {
    DestroyMatcen(m_matcen_id, true);
  }

  UpdateDialog();
}

void matcenkeypad::OnMatCopyButton() {
  if (MatcenValid(m_matcen_id)) {
    MatCopyBuffer = *Matcen[m_matcen_id];
    f_copy_buffer_valid = true;
  }

  UpdateDialog();
}

void matcenkeypad::OnMatPasteButton() {
  if (MatcenValid(m_matcen_id) && f_copy_buffer_valid) {
    *Matcen[m_matcen_id] = MatCopyBuffer;
  }

  UpdateDialog();
}

void matcenkeypad::OnMatApvRadio() {
  Matcen[m_matcen_id]->SetControlType(MPC_AFTER_PLAYER_VISIBLE);
  UpdateDialog();
}

//@@void matcenkeypad::OnKillfocusMatAttachEdit()
//@@{
//@@	char str[20];
//@@	int val;
//@@
//@@	((CEdit *) GetDlgItem(IDC_MAT_ATTACH_EDIT))->GetWindowText(str,20);
//@@
//@@	val = atoi(str);
//@@	Matcen[m_matcen_id]->SetAttach(val);
//@@
//@@	UpdateDialog();
//@@}

void matcenkeypad::OnKillfocusMatEffectPostprodTimeEdit() {
  char str[20];
  float val;

  ((CEdit *)GetDlgItem(IDC_MAT_EFFECT_POSTPROD_TIME_EDIT))->GetWindowText(str, 20);

  val = atof(str);
  Matcen[m_matcen_id]->SetPostProdTime(val);

  UpdateDialog();
}

BOOL matcenkeypad::OnInitDialog() {
  CDialog::OnInitDialog();

  UpdateDialog();

  return TRUE; // return TRUE unless you set the focus to a control
               // EXCEPTION: OCX Property Pages should return FALSE
}

void matcenkeypad::OnPaint() {
  CPaintDC dc(this); // device context for painting

  UpdateDialog();
}

#define SET_CHECK(id, state) ((CButton *)GetDlgItem(id))->SetCheck(state)
#define ENABLE(id, state) ((CButton *)GetDlgItem(id))->EnableWindow(state);

// which class is attached to the room/object combo box?
byte attached = -1;

void matcenkeypad::UpdateDialog() {
  bool f_valid = true;
  int i;
  char stemp[MAX_MATCEN_NAME_LEN];
  float ftemp;
  int itemp;
  CButton *bbox;

  // Check if the current matcen is valid
  if (!MatcenValid(m_matcen_id)) {
    f_valid = false;

    // Scan for a valid matcen id
    for (i = 0; i < Num_matcens; i++) {
      if (Matcen[i] != NULL) {
        m_matcen_id = i;
        f_valid = true;
        break;
      }
    }
  }

  // If valid enable fields and continue -- otherwise disable fields and exit
  if (f_valid) {
    EnableFields(true);
  } else {
    EnableFields(false);
    return;
  }

  PrintToDlgItem(this, IDC_MAT_EFFECT_PREPROD_TIME_EDIT, "%f", Matcen[m_matcen_id]->GetPreProdTime());
  PrintToDlgItem(this, IDC_MAT_EFFECT_POSTPROD_TIME_EDIT, "%f", Matcen[m_matcen_id]->GetPostProdTime());

  SendDlgItemMessage(IDC_MAT_PROD_EFFECT_COMBO, CB_RESETCONTENT, 0, 0);
  for (i = 0; i < NUM_MATCEN_EFFECTS; i++) {
    SendDlgItemMessage(IDC_MAT_PROD_EFFECT_COMBO, CB_ADDSTRING, 0, (LPARAM)(LPCTSTR)MatcenEffectStrings[i]);
  }
  SendDlgItemMessage(IDC_MAT_PROD_EFFECT_COMBO, CB_SELECTSTRING, 0,
                     (LPARAM)(LPCTSTR)MatcenEffectStrings[Matcen[m_matcen_id]->GetCreationEffect()]);

  SendDlgItemMessage(IDC_MAT_PROD_SOUND_COMBO, CB_RESETCONTENT, 0, 0);
  for (i = 0; i < MAX_SOUNDS; i++) {
    if (Sounds[i].used)
      SendDlgItemMessage(IDC_MAT_PROD_SOUND_COMBO, CB_ADDSTRING, 0, (LPARAM)(LPCTSTR)Sounds[i].name);
  }
  SendDlgItemMessage(IDC_MAT_PROD_SOUND_COMBO, CB_SELECTSTRING, 0,
                     (LPARAM)(LPCTSTR)Sounds[Matcen[m_matcen_id]->GetSound(MATCEN_PROD_SOUND)].name);

  SendDlgItemMessage(IDC_MAT_PROD_TEXTURE_COMBO, CB_RESETCONTENT, 0, 0);
  for (i = 0; i < MAX_TEXTURES; i++) {
    if (GameTextures[i].used)
      SendDlgItemMessage(IDC_MAT_PROD_TEXTURE_COMBO, CB_ADDSTRING, 0, (LPARAM)(LPCTSTR)GameTextures[i].name);
  }
  SendDlgItemMessage(IDC_MAT_PROD_TEXTURE_COMBO, CB_SELECTSTRING, 0,
                     (LPARAM)(LPCTSTR)GameTextures[Matcen[m_matcen_id]->GetCreationTexture()].name);

  SendDlgItemMessage(IDC_MAT_DISABLED_SOUND_COMBO, CB_RESETCONTENT, 0, 0);
  for (i = 0; i < MAX_SOUNDS; i++) {
    if (Sounds[i].used)
      SendDlgItemMessage(IDC_MAT_DISABLED_SOUND_COMBO, CB_ADDSTRING, 0, (LPARAM)(LPCTSTR)Sounds[i].name);
  }
  SendDlgItemMessage(IDC_MAT_DISABLED_SOUND_COMBO, CB_SELECTSTRING, 0,
                     (LPARAM)(LPCTSTR)Sounds[Matcen[m_matcen_id]->GetSound(MATCEN_DISABLE_SOUND)].name);

  SendDlgItemMessage(IDC_MAT_ENABLED_SOUND_COMBO, CB_RESETCONTENT, 0, 0);
  for (i = 0; i < MAX_SOUNDS; i++) {
    if (Sounds[i].used)
      SendDlgItemMessage(IDC_MAT_ENABLED_SOUND_COMBO, CB_ADDSTRING, 0, (LPARAM)(LPCTSTR)Sounds[i].name);
  }
  SendDlgItemMessage(IDC_MAT_ENABLED_SOUND_COMBO, CB_SELECTSTRING, 0,
                     (LPARAM)(LPCTSTR)Sounds[Matcen[m_matcen_id]->GetSound(MATCEN_ACTIVE_SOUND)].name);

  bbox = (CButton *)GetDlgItem(IDC_MAT_SC_RADIO);
  bbox->SetCheck((Matcen[m_matcen_id]->GetControlType() == MPC_SCRIPT));

  bbox = (CButton *)GetDlgItem(IDC_MAT_WPV_RADIO);
  bbox->SetCheck((Matcen[m_matcen_id]->GetControlType() == MPC_WHILE_PLAYER_VISIBLE));

  bbox = (CButton *)GetDlgItem(IDC_MAT_WPN_RADIO);
  bbox->SetCheck((Matcen[m_matcen_id]->GetControlType() == MPC_WHILE_PLAYER_NEAR));

  bbox = (CButton *)GetDlgItem(IDC_MAT_APN_RADIO);
  bbox->SetCheck((Matcen[m_matcen_id]->GetControlType() == MPC_AFTER_PLAYER_NEAR));

  bbox = (CButton *)GetDlgItem(IDC_MAT_APV_RADIO);
  bbox->SetCheck((Matcen[m_matcen_id]->GetControlType() == MPC_AFTER_PLAYER_VISIBLE));

  bbox = (CButton *)GetDlgItem(IDC_MAT_RANDOM_RADIO);
  bbox->SetCheck((Matcen[m_matcen_id]->GetStatus() & MSTAT_RANDOM_PROD_ORDER));

  bbox = (CButton *)GetDlgItem(IDC_MAT_ORDERED_RADIO);
  bbox->SetCheck(!(Matcen[m_matcen_id]->GetStatus() & MSTAT_RANDOM_PROD_ORDER));

  bbox = (CButton *)GetDlgItem(IDC_MAT_ENABLED_CHECK);
  bbox->SetCheck(!(Matcen[m_matcen_id]->GetStatus() & MSTAT_DISABLED));

  bbox = (CButton *)GetDlgItem(IDC_MAT_NHP_CHECK);
  bbox->SetCheck(Matcen[m_matcen_id]->GetStatus() & MSTAT_NOT_HURT_PLAYER);

  PrintToDlgItem(this, IDC_PROD_RATE_MULTI_EDIT, "%f", Matcen[m_matcen_id]->GetProdMultiplier());
  PrintToDlgItem(this, IDC_MAX_ALIVE_CHILDREN_EDIT, "%d", Matcen[m_matcen_id]->GetMaxAliveChildren());

  SendDlgItemMessage(IDC_MAT_PROD_0_COMBO, CB_RESETCONTENT, 0, 0);
  SendDlgItemMessage(IDC_MAT_PROD_1_COMBO, CB_RESETCONTENT, 0, 0);
  SendDlgItemMessage(IDC_MAT_PROD_2_COMBO, CB_RESETCONTENT, 0, 0);
  SendDlgItemMessage(IDC_MAT_PROD_3_COMBO, CB_RESETCONTENT, 0, 0);
  SendDlgItemMessage(IDC_MAT_PROD_4_COMBO, CB_RESETCONTENT, 0, 0);
  SendDlgItemMessage(IDC_MAT_PROD_5_COMBO, CB_RESETCONTENT, 0, 0);
  SendDlgItemMessage(IDC_MAT_PROD_6_COMBO, CB_RESETCONTENT, 0, 0);
  SendDlgItemMessage(IDC_MAT_PROD_7_COMBO, CB_RESETCONTENT, 0, 0);

  SendDlgItemMessage(IDC_MAT_PROD_0_COMBO, CB_ADDSTRING, 0, (LPARAM)(LPCTSTR) "None");
  SendDlgItemMessage(IDC_MAT_PROD_1_COMBO, CB_ADDSTRING, 0, (LPARAM)(LPCTSTR) "None");
  SendDlgItemMessage(IDC_MAT_PROD_2_COMBO, CB_ADDSTRING, 0, (LPARAM)(LPCTSTR) "None");
  SendDlgItemMessage(IDC_MAT_PROD_3_COMBO, CB_ADDSTRING, 0, (LPARAM)(LPCTSTR) "None");
  SendDlgItemMessage(IDC_MAT_PROD_4_COMBO, CB_ADDSTRING, 0, (LPARAM)(LPCTSTR) "None");
  SendDlgItemMessage(IDC_MAT_PROD_5_COMBO, CB_ADDSTRING, 0, (LPARAM)(LPCTSTR) "None");
  SendDlgItemMessage(IDC_MAT_PROD_6_COMBO, CB_ADDSTRING, 0, (LPARAM)(LPCTSTR) "None");
  SendDlgItemMessage(IDC_MAT_PROD_7_COMBO, CB_ADDSTRING, 0, (LPARAM)(LPCTSTR) "None");

  for (i = 0; i < MAX_OBJECT_IDS; i++) {
    if (Object_info[i].type != OBJ_NONE) {
      SendDlgItemMessage(IDC_MAT_PROD_0_COMBO, CB_ADDSTRING, 0, (LPARAM)(LPCTSTR)Object_info[i].name);
      SendDlgItemMessage(IDC_MAT_PROD_1_COMBO, CB_ADDSTRING, 0, (LPARAM)(LPCTSTR)Object_info[i].name);
      SendDlgItemMessage(IDC_MAT_PROD_2_COMBO, CB_ADDSTRING, 0, (LPARAM)(LPCTSTR)Object_info[i].name);
      SendDlgItemMessage(IDC_MAT_PROD_3_COMBO, CB_ADDSTRING, 0, (LPARAM)(LPCTSTR)Object_info[i].name);
      SendDlgItemMessage(IDC_MAT_PROD_4_COMBO, CB_ADDSTRING, 0, (LPARAM)(LPCTSTR)Object_info[i].name);
      SendDlgItemMessage(IDC_MAT_PROD_5_COMBO, CB_ADDSTRING, 0, (LPARAM)(LPCTSTR)Object_info[i].name);
      SendDlgItemMessage(IDC_MAT_PROD_6_COMBO, CB_ADDSTRING, 0, (LPARAM)(LPCTSTR)Object_info[i].name);
      SendDlgItemMessage(IDC_MAT_PROD_7_COMBO, CB_ADDSTRING, 0, (LPARAM)(LPCTSTR)Object_info[i].name);
    }
  }

  Matcen[m_matcen_id]->GetProdInfo(0, &i, NULL, NULL, NULL);
  if (i >= 0 && i < MAX_OBJECT_IDS && Object_info[i].type != OBJ_NONE)
    SendDlgItemMessage(IDC_MAT_PROD_0_COMBO, CB_SELECTSTRING, 0, (LPARAM)(LPCTSTR)Object_info[i].name);
  else
    SendDlgItemMessage(IDC_MAT_PROD_0_COMBO, CB_SELECTSTRING, 0, (LPARAM)(LPCTSTR) "None");

  Matcen[m_matcen_id]->GetProdInfo(1, &i, NULL, NULL, NULL);
  if (i >= 0 && i < MAX_OBJECT_IDS && Object_info[i].type != OBJ_NONE)
    SendDlgItemMessage(IDC_MAT_PROD_1_COMBO, CB_SELECTSTRING, 0, (LPARAM)(LPCTSTR)Object_info[i].name);
  else
    SendDlgItemMessage(IDC_MAT_PROD_1_COMBO, CB_SELECTSTRING, 0, (LPARAM)(LPCTSTR) "None");

  Matcen[m_matcen_id]->GetProdInfo(2, &i, NULL, NULL, NULL);
  if (i >= 0 && i < MAX_OBJECT_IDS && Object_info[i].type != OBJ_NONE)
    SendDlgItemMessage(IDC_MAT_PROD_2_COMBO, CB_SELECTSTRING, 0, (LPARAM)(LPCTSTR)Object_info[i].name);
  else
    SendDlgItemMessage(IDC_MAT_PROD_2_COMBO, CB_SELECTSTRING, 0, (LPARAM)(LPCTSTR) "None");

  Matcen[m_matcen_id]->GetProdInfo(3, &i, NULL, NULL, NULL);
  if (i >= 0 && i < MAX_OBJECT_IDS && Object_info[i].type != OBJ_NONE)
    SendDlgItemMessage(IDC_MAT_PROD_3_COMBO, CB_SELECTSTRING, 0, (LPARAM)(LPCTSTR)Object_info[i].name);
  else
    SendDlgItemMessage(IDC_MAT_PROD_3_COMBO, CB_SELECTSTRING, 0, (LPARAM)(LPCTSTR) "None");

  Matcen[m_matcen_id]->GetProdInfo(4, &i, NULL, NULL, NULL);
  if (i >= 0 && i < MAX_OBJECT_IDS && Object_info[i].type != OBJ_NONE)
    SendDlgItemMessage(IDC_MAT_PROD_4_COMBO, CB_SELECTSTRING, 0, (LPARAM)(LPCTSTR)Object_info[i].name);
  else
    SendDlgItemMessage(IDC_MAT_PROD_4_COMBO, CB_SELECTSTRING, 0, (LPARAM)(LPCTSTR) "None");

  Matcen[m_matcen_id]->GetProdInfo(5, &i, NULL, NULL, NULL);
  if (i >= 0 && i < MAX_OBJECT_IDS && Object_info[i].type != OBJ_NONE)
    SendDlgItemMessage(IDC_MAT_PROD_5_COMBO, CB_SELECTSTRING, 0, (LPARAM)(LPCTSTR)Object_info[i].name);
  else
    SendDlgItemMessage(IDC_MAT_PROD_5_COMBO, CB_SELECTSTRING, 0, (LPARAM)(LPCTSTR) "None");

  Matcen[m_matcen_id]->GetProdInfo(6, &i, NULL, NULL, NULL);
  if (i >= 0 && i < MAX_OBJECT_IDS && Object_info[i].type != OBJ_NONE)
    SendDlgItemMessage(IDC_MAT_PROD_6_COMBO, CB_SELECTSTRING, 0, (LPARAM)(LPCTSTR)Object_info[i].name);
  else
    SendDlgItemMessage(IDC_MAT_PROD_6_COMBO, CB_SELECTSTRING, 0, (LPARAM)(LPCTSTR) "None");

  Matcen[m_matcen_id]->GetProdInfo(7, &i, NULL, NULL, NULL);
  if (i >= 0 && i < MAX_OBJECT_IDS && Object_info[i].type != OBJ_NONE)
    SendDlgItemMessage(IDC_MAT_PROD_7_COMBO, CB_SELECTSTRING, 0, (LPARAM)(LPCTSTR)Object_info[i].name);
  else
    SendDlgItemMessage(IDC_MAT_PROD_7_COMBO, CB_SELECTSTRING, 0, (LPARAM)(LPCTSTR) "None");

  PrintToDlgItem(this, IDC_MAX_OBJ_PROD_EDIT, "%d", Matcen[m_matcen_id]->GetMaxProd());

  Matcen[m_matcen_id]->GetProdInfo(0, NULL, NULL, NULL, &itemp);
  PrintToDlgItem(this, IDC_MAT_PROD_0_QUATITY_EDIT, "%d", itemp);
  Matcen[m_matcen_id]->GetProdInfo(1, NULL, NULL, NULL, &itemp);
  PrintToDlgItem(this, IDC_MAT_PROD_1_QUATITY_EDIT, "%d", itemp);
  Matcen[m_matcen_id]->GetProdInfo(2, NULL, NULL, NULL, &itemp);
  PrintToDlgItem(this, IDC_MAT_PROD_2_QUATITY_EDIT, "%d", itemp);
  Matcen[m_matcen_id]->GetProdInfo(3, NULL, NULL, NULL, &itemp);
  PrintToDlgItem(this, IDC_MAT_PROD_3_QUATITY_EDIT, "%d", itemp);
  Matcen[m_matcen_id]->GetProdInfo(4, NULL, NULL, NULL, &itemp);
  PrintToDlgItem(this, IDC_MAT_PROD_4_QUATITY_EDIT, "%d", itemp);
  Matcen[m_matcen_id]->GetProdInfo(5, NULL, NULL, NULL, &itemp);
  PrintToDlgItem(this, IDC_MAT_PROD_5_QUATITY_EDIT, "%d", itemp);
  Matcen[m_matcen_id]->GetProdInfo(6, NULL, NULL, NULL, &itemp);
  PrintToDlgItem(this, IDC_MAT_PROD_6_QUATITY_EDIT, "%d", itemp);
  Matcen[m_matcen_id]->GetProdInfo(7, NULL, NULL, NULL, &itemp);
  PrintToDlgItem(this, IDC_MAT_PROD_7_QUATITY_EDIT, "%d", itemp);

  Matcen[m_matcen_id]->GetProdInfo(0, NULL, NULL, &ftemp, NULL);
  PrintToDlgItem(this, IDC_MAT_PROD_0_SEC_EDIT, "%f", ftemp);
  Matcen[m_matcen_id]->GetProdInfo(1, NULL, NULL, &ftemp, NULL);
  PrintToDlgItem(this, IDC_MAT_PROD_1_SEC_EDIT, "%f", ftemp);
  Matcen[m_matcen_id]->GetProdInfo(2, NULL, NULL, &ftemp, NULL);
  PrintToDlgItem(this, IDC_MAT_PROD_2_SEC_EDIT, "%f", ftemp);
  Matcen[m_matcen_id]->GetProdInfo(3, NULL, NULL, &ftemp, NULL);
  PrintToDlgItem(this, IDC_MAT_PROD_3_SEC_EDIT, "%f", ftemp);
  Matcen[m_matcen_id]->GetProdInfo(4, NULL, NULL, &ftemp, NULL);
  PrintToDlgItem(this, IDC_MAT_PROD_4_SEC_EDIT, "%f", ftemp);
  Matcen[m_matcen_id]->GetProdInfo(5, NULL, NULL, &ftemp, NULL);
  PrintToDlgItem(this, IDC_MAT_PROD_5_SEC_EDIT, "%f", ftemp);
  Matcen[m_matcen_id]->GetProdInfo(6, NULL, NULL, &ftemp, NULL);
  PrintToDlgItem(this, IDC_MAT_PROD_6_SEC_EDIT, "%f", ftemp);
  Matcen[m_matcen_id]->GetProdInfo(7, NULL, NULL, &ftemp, NULL);
  PrintToDlgItem(this, IDC_MAT_PROD_7_SEC_EDIT, "%f", ftemp);

  Matcen[m_matcen_id]->GetProdInfo(0, NULL, &itemp, NULL, NULL);
  PrintToDlgItem(this, IDC_MAT_PROD_0_PRIORITY_EDIT, "%d", itemp);
  Matcen[m_matcen_id]->GetProdInfo(1, NULL, &itemp, NULL, NULL);
  PrintToDlgItem(this, IDC_MAT_PROD_1_PRIORITY_EDIT, "%d", itemp);
  Matcen[m_matcen_id]->GetProdInfo(2, NULL, &itemp, NULL, NULL);
  PrintToDlgItem(this, IDC_MAT_PROD_2_PRIORITY_EDIT, "%d", itemp);
  Matcen[m_matcen_id]->GetProdInfo(3, NULL, &itemp, NULL, NULL);
  PrintToDlgItem(this, IDC_MAT_PROD_3_PRIORITY_EDIT, "%d", itemp);
  Matcen[m_matcen_id]->GetProdInfo(4, NULL, &itemp, NULL, NULL);
  PrintToDlgItem(this, IDC_MAT_PROD_4_PRIORITY_EDIT, "%d", itemp);
  Matcen[m_matcen_id]->GetProdInfo(5, NULL, &itemp, NULL, NULL);
  PrintToDlgItem(this, IDC_MAT_PROD_5_PRIORITY_EDIT, "%d", itemp);
  Matcen[m_matcen_id]->GetProdInfo(6, NULL, &itemp, NULL, NULL);
  PrintToDlgItem(this, IDC_MAT_PROD_6_PRIORITY_EDIT, "%d", itemp);
  Matcen[m_matcen_id]->GetProdInfo(7, NULL, &itemp, NULL, NULL);
  PrintToDlgItem(this, IDC_MAT_PROD_7_PRIORITY_EDIT, "%d", itemp);

  PrintToDlgItem(this, IDC_MAT_NUM_TYPES_EDIT, "%d", Matcen[m_matcen_id]->GetNumProdTypes());
  PrintToDlgItem(this, IDC_MAT_SPAWN0_EDIT, "%d", Matcen[m_matcen_id]->GetSpawnPnt(0));
  PrintToDlgItem(this, IDC_MAT_SPAWN1_EDIT, "%d", Matcen[m_matcen_id]->GetSpawnPnt(1));
  PrintToDlgItem(this, IDC_MAT_SPAWN2_EDIT, "%d", Matcen[m_matcen_id]->GetSpawnPnt(2));
  PrintToDlgItem(this, IDC_MAT_SPAWN3_EDIT, "%d", Matcen[m_matcen_id]->GetSpawnPnt(3));

  //	PrintToDlgItem(this, IDC_MAT_ATTACH_EDIT, "%d", Matcen[m_matcen_id]->GetAttach());

  switch (Matcen[m_matcen_id]->GetAttachType()) {
  case MT_ROOM:
    SET_CHECK(IDC_MAT_ROOM_RADIO, true);
    SET_CHECK(IDC_MAT_OBJECT_RADIO, false);
    SET_CHECK(IDC_MAT_UNASSIGNED_RADIO, false);
    if (attached != MT_ROOM) {
      if (attached == MT_OBJECT)
        m_object_combo.Detach();
      m_room_combo.Attach(((CWnd *)GetDlgItem(IDC_MAT_ROOM_OBJECT_COMBO))->m_hWnd);
      m_room_combo.Init(Matcen[m_matcen_id]->GetAttach());
      attached = MT_ROOM;
    }
    m_room_combo.SetSelected(Matcen[m_matcen_id]->GetAttach());
    break;
  case MT_OBJECT:
    SET_CHECK(IDC_MAT_ROOM_RADIO, false);
    SET_CHECK(IDC_MAT_OBJECT_RADIO, true);
    SET_CHECK(IDC_MAT_UNASSIGNED_RADIO, false);
    if (attached != MT_OBJECT) {
      if (attached == MT_ROOM)
        m_room_combo.Detach();
      m_object_combo.Attach(((CWnd *)GetDlgItem(IDC_MAT_ROOM_OBJECT_COMBO))->m_hWnd);
      m_object_combo.Init(OBJ_NONE, Matcen[m_matcen_id]->GetAttach());
      attached = MT_OBJECT;
    }
    m_object_combo.SetSelected(Matcen[m_matcen_id]->GetAttach());
    break;
  case MT_UNASSIGNED:
    SET_CHECK(IDC_MAT_ROOM_RADIO, false);
    SET_CHECK(IDC_MAT_OBJECT_RADIO, false);
    SET_CHECK(IDC_MAT_UNASSIGNED_RADIO, true);
    ENABLE(IDC_MAT_ROOM_OBJECT_COMBO, false);
    break;
  }

  PrintToDlgItem(this, IDC_MAT_NUM_SPAWN_EDIT, "%d", Matcen[m_matcen_id]->GetNumSpawnPnts());

  Matcen[m_matcen_id]->GetName(stemp);
  PrintToDlgItem(this, IDC_MAT_NAME_EDIT, "%s", stemp);
}

void matcenkeypad::OnSelendokMatProdTextureCombo() {
  int i, cur;
  char name[200];

  cur = SendDlgItemMessage(IDC_MAT_PROD_TEXTURE_COMBO, CB_GETCURSEL, 0, 0);
  SendDlgItemMessage(IDC_MAT_PROD_TEXTURE_COMBO, CB_GETLBTEXT, cur, (LPARAM)(LPCTSTR)name);

  i = FindTextureName(name);

  if (i == -1) {
    i = 0;
  }

  Matcen[m_matcen_id]->SetCreationTexture(i);

  UpdateDialog();
}

void matcenkeypad::OnMatNhpCheck() {
  int status = Matcen[m_matcen_id]->GetStatus();

  if (status & MSTAT_NOT_HURT_PLAYER)
    Matcen[m_matcen_id]->SetStatus(MSTAT_NOT_HURT_PLAYER, false);
  else
    Matcen[m_matcen_id]->SetStatus(MSTAT_NOT_HURT_PLAYER, true);

  UpdateDialog();
}

void matcenkeypad::OnSelendokMatRoomObjectCombo() {
  switch (Matcen[m_matcen_id]->GetAttachType()) {
  case MT_ROOM:
    Matcen[m_matcen_id]->SetAttach(m_room_combo.GetSelected());
    break;
  case MT_OBJECT:
    Matcen[m_matcen_id]->SetAttach(m_object_combo.GetSelected());
    break;
  case MT_UNASSIGNED:
    Int3();
    break;
  }

  UpdateDialog();
}

void matcenkeypad::OnDestroy() {
  if (attached == MT_OBJECT)
    m_object_combo.Detach();
  else if (attached == MT_ROOM)
    m_room_combo.Detach();

  CDialog::OnDestroy();
}
