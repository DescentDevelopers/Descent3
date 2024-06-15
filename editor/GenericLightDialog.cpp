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

// GenericLightDialog.cpp : implementation file
//

#include "stdafx.h"
#include "editor.h"
#include "GenericLightDialog.h"
#include "objinfo.h"
#include "object.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGenericLightDialog dialog

CGenericLightDialog::CGenericLightDialog(light_info *current, CWnd *pParent)
    : CDialog(CGenericLightDialog::IDD, pParent) {
  //{{AFX_DATA_INIT(CGenericLightDialog)
  // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT
  m_lightinfo = current;
}

void CGenericLightDialog::DoDataExchange(CDataExchange *pDX) {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CGenericLightDialog)
  // NOTE: the ClassWizard will add DDX and DDV calls here
  //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CGenericLightDialog, CDialog)
//{{AFX_MSG_MAP(CGenericLightDialog)
ON_EN_KILLFOCUS(IDC_GENERIC_LIGHT_DISTANCE, OnKillfocusGenericLightDistance)
ON_EN_KILLFOCUS(IDC_GENERIC_BLUE_LIGHT, OnKillfocusGenericBlueLight)
ON_EN_KILLFOCUS(IDC_GENERIC_GREEN_LIGHT, OnKillfocusGenericGreenLight)
ON_EN_KILLFOCUS(IDC_GENERIC_RED_LIGHT, OnKillfocusGenericRedLight)
ON_EN_KILLFOCUS(IDC_GENERIC_PULSE_TIME, OnKillfocusGenericPulseTime)
ON_BN_CLICKED(IDC_FLICKER_RADIO, OnFlickerRadio)
ON_BN_CLICKED(IDC_USE_TIMEBITS_RADIO, OnUseTimebitsRadio)
ON_BN_CLICKED(IDC_PULSE, OnPulse)
ON_BN_CLICKED(IDC_ALWAYS_ON_RADIO, OnAlwaysOnRadio)
ON_BN_CLICKED(IDC_TIME_CHECK1, OnTimeCheck1)
ON_BN_CLICKED(IDC_TIME_CHECK2, OnTimeCheck2)
ON_BN_CLICKED(IDC_TIME_CHECK3, OnTimeCheck3)
ON_BN_CLICKED(IDC_TIME_CHECK4, OnTimeCheck4)
ON_BN_CLICKED(IDC_TIME_CHECK5, OnTimeCheck5)
ON_BN_CLICKED(IDC_TIME_CHECK6, OnTimeCheck6)
ON_BN_CLICKED(IDC_TIME_CHECK7, OnTimeCheck7)
ON_BN_CLICKED(IDC_TIME_CHECK8, OnTimeCheck8)
ON_BN_CLICKED(IDC_RENDER_STATIC, OnRenderStatic)
ON_BN_CLICKED(IDC_RENDER_GOURAUD, OnRenderGouraud)
ON_BN_CLICKED(IDC_RENDER_LIGHTMAPS, OnRenderLightmaps)
ON_BN_CLICKED(IDC_PULSE_TO_SECOND, OnPulseToSecond)
ON_EN_KILLFOCUS(IDC_GENERIC_RED_LIGHT2, OnKillfocusGenericRedLight2)
ON_EN_KILLFOCUS(IDC_GENERIC_GREEN_LIGHT2, OnKillfocusGenericGreenLight2)
ON_EN_KILLFOCUS(IDC_GENERIC_BLUE_LIGHT2, OnKillfocusGenericBlueLight2)
ON_BN_CLICKED(IDC_FLICKER_SLIGHTLY_CHECK, OnFlickerSlightlyCheck)
ON_EN_KILLFOCUS(IDC_FLICKER_SLIGHTLY_EDIT, OnKillfocusFlickerSlightlyEdit)
ON_BN_CLICKED(IDC_DIRECTIONAL, OnDirectional)
ON_EN_KILLFOCUS(IDC_FOV_EDIT, OnKillfocusFovEdit)
ON_BN_CLICKED(IDC_NO_SPECULAR_CHECK, OnNoSpecularCheck)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGenericLightDialog message handlers

void CGenericLightDialog::OnKillfocusGenericLightDistance() {
  char str[20];
  float val;

  ((CEdit *)GetDlgItem(IDC_GENERIC_LIGHT_DISTANCE))->GetWindowText(str, 20);
  val = atof(str);
  if (val < 0)
    val = 0.0f;
  if (val > 300)
    val = 300.0f;
  m_lightinfo->light_distance = val;

  UpdateDialog();
}

void CGenericLightDialog::OnKillfocusGenericBlueLight() {
  char str[20];
  float val;

  ((CEdit *)GetDlgItem(IDC_GENERIC_BLUE_LIGHT))->GetWindowText(str, 20);

  val = atof(str);
  if (val < 0)
    val = 0.0f;
  if (val > 1)
    val = 1.0f;
  m_lightinfo->blue_light1 = val;
  UpdateDialog();
}

void CGenericLightDialog::OnKillfocusGenericGreenLight() {
  char str[20];
  float val;

  ((CEdit *)GetDlgItem(IDC_GENERIC_GREEN_LIGHT))->GetWindowText(str, 20);

  val = atof(str);
  if (val < 0)
    val = 0.0f;
  if (val > 1)
    val = 1.0f;
  m_lightinfo->green_light1 = val;
  UpdateDialog();
}

void CGenericLightDialog::OnKillfocusGenericRedLight() {
  char str[20];
  float val;

  ((CEdit *)GetDlgItem(IDC_GENERIC_RED_LIGHT))->GetWindowText(str, 20);

  val = atof(str);
  if (val < 0)
    val = 0.0f;
  if (val > 1)
    val = 1.0f;
  m_lightinfo->red_light1 = val;
  UpdateDialog();
}

void CGenericLightDialog::OnKillfocusGenericPulseTime() {
  char str[20];
  float val;

  ((CEdit *)GetDlgItem(IDC_GENERIC_PULSE_TIME))->GetWindowText(str, 20);

  val = atof(str);
  if (val < 0)
    val = 0.0f;
  if (val > 300)
    val = 300.0f;
  m_lightinfo->time_interval = val;
  UpdateDialog();
}

void CGenericLightDialog::OnFlickerRadio() {
  int c1 = IsDlgButtonChecked(IDC_FLICKER_RADIO);
  if (c1) {
    m_lightinfo->flags |= OLF_FLICKERING;
    m_lightinfo->flags &= ~OLF_TIMEBITS;
  }
  UpdateDialog();
}

void CGenericLightDialog::OnUseTimebitsRadio() {
  // TODO: Add your control notification handler code here
  int c1 = IsDlgButtonChecked(IDC_USE_TIMEBITS_RADIO);
  if (c1) {
    m_lightinfo->flags |= OLF_TIMEBITS;
    m_lightinfo->flags &= ~OLF_FLICKERING;
  }
  UpdateDialog();
}

void CGenericLightDialog::OnPulse() {
  int c1 = IsDlgButtonChecked(IDC_PULSE);
  if (c1)
    m_lightinfo->flags |= OLF_PULSE;
  else
    m_lightinfo->flags &= ~OLF_PULSE;

  UpdateDialog();
}

void CGenericLightDialog::UpdateDialog() {
  char txt[255];
  CButton *bbox;

  // Update light stuff
  sprintf(txt, "%.2f", m_lightinfo->light_distance);
  ((CEdit *)GetDlgItem(IDC_GENERIC_LIGHT_DISTANCE))->SetWindowText(txt);

  sprintf(txt, "%.2f", m_lightinfo->red_light1);
  ((CEdit *)GetDlgItem(IDC_GENERIC_RED_LIGHT))->SetWindowText(txt);

  sprintf(txt, "%.2f", m_lightinfo->green_light1);
  ((CEdit *)GetDlgItem(IDC_GENERIC_GREEN_LIGHT))->SetWindowText(txt);

  sprintf(txt, "%.2f", m_lightinfo->blue_light1);
  ((CEdit *)GetDlgItem(IDC_GENERIC_BLUE_LIGHT))->SetWindowText(txt);

  sprintf(txt, "%.2f", m_lightinfo->red_light2);
  ((CEdit *)GetDlgItem(IDC_GENERIC_RED_LIGHT2))->SetWindowText(txt);

  sprintf(txt, "%.2f", m_lightinfo->green_light2);
  ((CEdit *)GetDlgItem(IDC_GENERIC_GREEN_LIGHT2))->SetWindowText(txt);

  sprintf(txt, "%.2f", m_lightinfo->blue_light2);
  ((CEdit *)GetDlgItem(IDC_GENERIC_BLUE_LIGHT2))->SetWindowText(txt);

  sprintf(txt, "%.2f", m_lightinfo->flicker_distance);
  ((CEdit *)GetDlgItem(IDC_FLICKER_SLIGHTLY_EDIT))->SetWindowText(txt);

  sprintf(txt, "%.3f", m_lightinfo->directional_dot);
  ((CEdit *)GetDlgItem(IDC_FOV_EDIT))->SetWindowText(txt);

  bbox = (CButton *)GetDlgItem(IDC_FLICKER_RADIO);
  bbox->SetCheck(m_lightinfo->flags & OLF_FLICKERING);

  bbox = (CButton *)GetDlgItem(IDC_DIRECTIONAL);
  bbox->SetCheck(m_lightinfo->flags & OLF_DIRECTIONAL);

  bbox = (CButton *)GetDlgItem(IDC_NO_SPECULAR_CHECK);
  bbox->SetCheck(m_lightinfo->flags & OLF_NO_SPECULARITY);

  bbox = (CButton *)GetDlgItem(IDC_USE_TIMEBITS_RADIO);
  bbox->SetCheck(m_lightinfo->flags & OLF_TIMEBITS);

  bbox = (CButton *)GetDlgItem(IDC_ALWAYS_ON_RADIO);
  if (!(m_lightinfo->flags & OLF_FLICKERING) && !(m_lightinfo->flags & OLF_TIMEBITS)) {
    bbox->SetCheck(1);
  } else {
    bbox->SetCheck(0);
  }

  bbox = (CButton *)GetDlgItem(IDC_PULSE);
  bbox->SetCheck(m_lightinfo->flags & OLF_PULSE);

  bbox = (CButton *)GetDlgItem(IDC_PULSE_TO_SECOND);
  bbox->SetCheck(m_lightinfo->flags & OLF_PULSE_TO_SECOND);

  bbox = (CButton *)GetDlgItem(IDC_FLICKER_SLIGHTLY_CHECK);
  bbox->SetCheck(m_lightinfo->flags & OLF_FLICKER_SLIGHTLY);

  sprintf(txt, "%.2f", m_lightinfo->time_interval);
  ((CEdit *)GetDlgItem(IDC_GENERIC_PULSE_TIME))->SetWindowText(txt);

  CheckDlgButton(IDC_TIME_CHECK1, (m_lightinfo->timebits & (1 << 0)) ? 1 : 0);
  CheckDlgButton(IDC_TIME_CHECK2, (m_lightinfo->timebits & (1 << 1)) ? 1 : 0);
  CheckDlgButton(IDC_TIME_CHECK3, (m_lightinfo->timebits & (1 << 2)) ? 1 : 0);
  CheckDlgButton(IDC_TIME_CHECK4, (m_lightinfo->timebits & (1 << 3)) ? 1 : 0);
  CheckDlgButton(IDC_TIME_CHECK5, (m_lightinfo->timebits & (1 << 4)) ? 1 : 0);
  CheckDlgButton(IDC_TIME_CHECK6, (m_lightinfo->timebits & (1 << 5)) ? 1 : 0);
  CheckDlgButton(IDC_TIME_CHECK7, (m_lightinfo->timebits & (1 << 6)) ? 1 : 0);
  CheckDlgButton(IDC_TIME_CHECK8, (m_lightinfo->timebits & (1 << 7)) ? 1 : 0);

  CheckDlgButton(IDC_RENDER_STATIC, m_lightinfo->lighting_render_type == LRT_STATIC);
  CheckDlgButton(IDC_RENDER_GOURAUD, m_lightinfo->lighting_render_type == LRT_GOURAUD);
  CheckDlgButton(IDC_RENDER_LIGHTMAPS, m_lightinfo->lighting_render_type == LRT_LIGHTMAPS);
}

void CGenericLightDialog::OnAlwaysOnRadio() {
  // TODO: Add your control notification handler code here
  int c1 = IsDlgButtonChecked(IDC_ALWAYS_ON_RADIO);
  if (c1) {
    m_lightinfo->flags &= ~OLF_FLICKERING;
    m_lightinfo->flags &= ~OLF_TIMEBITS;
  }
  UpdateDialog();
}

BOOL CGenericLightDialog::OnInitDialog() {
  CDialog::OnInitDialog();

  UpdateDialog();

  return TRUE; // return TRUE unless you set the focus to a control
               // EXCEPTION: OCX Property Pages should return FALSE
}

void CGenericLightDialog::OnTimeCheck1() {
  int c1 = IsDlgButtonChecked(IDC_TIME_CHECK1);
  if (c1)
    m_lightinfo->timebits |= (1 << 0);
  else
    m_lightinfo->timebits &= ~(1 << 0);

  UpdateDialog();
}

void CGenericLightDialog::OnTimeCheck2() {
  int c1 = IsDlgButtonChecked(IDC_TIME_CHECK2);
  if (c1)
    m_lightinfo->timebits |= (1 << 1);
  else
    m_lightinfo->timebits &= ~(1 << 1);

  UpdateDialog();
}

void CGenericLightDialog::OnTimeCheck3() {
  int c1 = IsDlgButtonChecked(IDC_TIME_CHECK3);
  if (c1)
    m_lightinfo->timebits |= (1 << 2);
  else
    m_lightinfo->timebits &= ~(1 << 2);

  UpdateDialog();
}

void CGenericLightDialog::OnTimeCheck4() {
  int c1 = IsDlgButtonChecked(IDC_TIME_CHECK4);
  if (c1)
    m_lightinfo->timebits |= (1 << 3);
  else
    m_lightinfo->timebits &= ~(1 << 3);

  UpdateDialog();
}

void CGenericLightDialog::OnTimeCheck5() {
  int c1 = IsDlgButtonChecked(IDC_TIME_CHECK5);
  if (c1)
    m_lightinfo->timebits |= (1 << 4);
  else
    m_lightinfo->timebits &= ~(1 << 4);

  UpdateDialog();
}

void CGenericLightDialog::OnTimeCheck6() {
  int c1 = IsDlgButtonChecked(IDC_TIME_CHECK6);
  if (c1)
    m_lightinfo->timebits |= (1 << 5);
  else
    m_lightinfo->timebits &= ~(1 << 5);

  UpdateDialog();
}

void CGenericLightDialog::OnTimeCheck7() {
  int c1 = IsDlgButtonChecked(IDC_TIME_CHECK7);
  if (c1)
    m_lightinfo->timebits |= (1 << 6);
  else
    m_lightinfo->timebits &= ~(1 << 6);

  UpdateDialog();
}

void CGenericLightDialog::OnTimeCheck8() {
  int c1 = IsDlgButtonChecked(IDC_TIME_CHECK8);
  if (c1)
    m_lightinfo->timebits |= (1 << 7);
  else
    m_lightinfo->timebits &= ~(1 << 7);

  UpdateDialog();
}

void CGenericLightDialog::OnRenderStatic() {
  m_lightinfo->lighting_render_type = LRT_STATIC;
  UpdateDialog();
}

void CGenericLightDialog::OnRenderGouraud() {

  m_lightinfo->lighting_render_type = LRT_GOURAUD;
  UpdateDialog();
}

void CGenericLightDialog::OnRenderLightmaps() {
  m_lightinfo->lighting_render_type = LRT_LIGHTMAPS;
  UpdateDialog();
}

void CGenericLightDialog::OnPulseToSecond() {
  int c1 = IsDlgButtonChecked(IDC_PULSE_TO_SECOND);
  if (c1)
    m_lightinfo->flags |= OLF_PULSE_TO_SECOND;
  else
    m_lightinfo->flags &= ~OLF_PULSE_TO_SECOND;
}

void CGenericLightDialog::OnKillfocusGenericRedLight2() {
  char str[20];
  float val;

  ((CEdit *)GetDlgItem(IDC_GENERIC_RED_LIGHT2))->GetWindowText(str, 20);

  val = atof(str);
  if (val < 0)
    val = 0.0f;
  if (val > 1)
    val = 1.0f;
  m_lightinfo->red_light2 = val;
  UpdateDialog();
}

void CGenericLightDialog::OnKillfocusGenericGreenLight2() {
  char str[20];
  float val;

  ((CEdit *)GetDlgItem(IDC_GENERIC_GREEN_LIGHT2))->GetWindowText(str, 20);

  val = atof(str);
  if (val < 0)
    val = 0.0f;
  if (val > 1)
    val = 1.0f;
  m_lightinfo->green_light2 = val;
  UpdateDialog();
}

void CGenericLightDialog::OnKillfocusGenericBlueLight2() {
  char str[20];
  float val;

  ((CEdit *)GetDlgItem(IDC_GENERIC_BLUE_LIGHT2))->GetWindowText(str, 20);

  val = atof(str);
  if (val < 0)
    val = 0.0f;
  if (val > 1)
    val = 1.0f;
  m_lightinfo->blue_light2 = val;
  UpdateDialog();
}

void CGenericLightDialog::OnFlickerSlightlyCheck() {
  // TODO: Add your control notification handler code here
  int c1 = IsDlgButtonChecked(IDC_FLICKER_SLIGHTLY_CHECK);
  if (c1)
    m_lightinfo->flags |= OLF_FLICKER_SLIGHTLY;
  else
    m_lightinfo->flags &= ~OLF_FLICKER_SLIGHTLY;

  UpdateDialog();
}

void CGenericLightDialog::OnKillfocusFlickerSlightlyEdit() {
  char str[20];
  float val;

  ((CEdit *)GetDlgItem(IDC_FLICKER_SLIGHTLY_EDIT))->GetWindowText(str, 20);

  val = atof(str);
  if (val < 0)
    val = 0.0f;
  if (val > 100)
    val = 100.0f;
  m_lightinfo->flicker_distance = val;
  UpdateDialog();
}

void CGenericLightDialog::OnDirectional() {
  int c1 = IsDlgButtonChecked(IDC_DIRECTIONAL);
  if (c1)
    m_lightinfo->flags |= OLF_DIRECTIONAL;
  else
    m_lightinfo->flags &= ~OLF_DIRECTIONAL;

  UpdateDialog();
}

void CGenericLightDialog::OnKillfocusFovEdit() {
  char str[20];
  float val;

  ((CEdit *)GetDlgItem(IDC_FOV_EDIT))->GetWindowText(str, 20);

  val = atof(str);
  if (val < 0)
    val = 0.0f;
  if (val > 1.0)
    val = 1.0;
  m_lightinfo->directional_dot = val;
  UpdateDialog();
}

void CGenericLightDialog::OnNoSpecularCheck() {
  int c1 = IsDlgButtonChecked(IDC_NO_SPECULAR_CHECK);
  if (c1)
    m_lightinfo->flags |= OLF_NO_SPECULARITY;
  else
    m_lightinfo->flags &= ~OLF_NO_SPECULARITY;

  UpdateDialog();
}
