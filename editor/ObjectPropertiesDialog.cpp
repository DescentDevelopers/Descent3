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

// ObjectPropertiesDialog.cpp : implementation file
//

#include "stdafx.h"
#include "editor.h"
#include "PropertyAIDlg.h"
#include "PropertyPhysicsDlg.h"
#include "ObjectPropertiesDialog.h"
#include "pserror.h"
#include "ddio.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CObjectPropertiesDialog dialog

CObjectPropertiesDialog::CObjectPropertiesDialog(CWnd *pParent /*=NULL*/)
    : CDialog(CObjectPropertiesDialog::IDD, pParent) {
  //{{AFX_DATA_INIT(CObjectPropertiesDialog)
  // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT
  m_Parent = pParent;
  m_Initializing = FALSE;
  m_KeypadTabDialog = NULL;
  m_PropPhysicsDlg = NULL;
  m_current_tab = 0;
}

void CObjectPropertiesDialog::DoDataExchange(CDataExchange *pDX) {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CObjectPropertiesDialog)
  // NOTE: the ClassWizard will add DDX and DDV calls here
  //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CObjectPropertiesDialog, CDialog)
//{{AFX_MSG_MAP(CObjectPropertiesDialog)
ON_WM_SHOWWINDOW()
ON_WM_SIZE()
ON_WM_MOVE()
ON_WM_CREATE()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CObjectPropertiesDialog message handlers

BOOL CObjectPropertiesDialog::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT &rect,
                                     CWnd *pParentWnd, UINT nID, CCreateContext *pContext) {
  // TODO: Add your specialized code here and/or call the base class

  return CDialog::Create(IDD, pParentWnd);
}

BOOL CObjectPropertiesDialog::OnInitDialog() {
  CDialog::OnInitDialog();

  CTabCtrl *tab_ctrl;
  TC_ITEM tc_item;

  m_current_tab = 0;

  tab_ctrl = (CTabCtrl *)GetDlgItem(IDC_PROPERTYTAB);

  //	Add tabs to tabcontrol in the keypad dialog bar.
  tc_item.mask = TCIF_TEXT;
  tc_item.pszText = "Physics";
  tc_item.cchTextMax = lstrlen(tc_item.pszText) + 1;
  tab_ctrl->InsertItem(TAB_PHYSICS, &tc_item);

  tc_item.mask = TCIF_TEXT;
  tc_item.pszText = "AI";
  tc_item.cchTextMax = lstrlen(tc_item.pszText) + 1;
  tab_ctrl->InsertItem(TAB_AI, &tc_item);

  //	fit keypad dialogs in tab control
  RECT tab_rect, tab_winrect;
  int tx, ty, tw, th;

  tab_ctrl->GetWindowRect(&tab_winrect);
  ::CopyRect(&tab_rect, &tab_winrect);
  tab_ctrl->AdjustRect(FALSE, &tab_rect);
  tx = tab_rect.left - tab_winrect.left;
  ty = tab_rect.top - tab_winrect.top;
  tw = tab_rect.right - tab_rect.left;
  th = tab_rect.bottom - tab_rect.top;

  //	Create tab dialogs
  //	resize them to fit the tab control
  //	set scroll bars accordingly
  m_PropAIDlg = new CPropertyAIDlg(tab_ctrl);
  m_PropAIDlg->Create();
  m_PropAIDlg->SetWindowPos(tab_ctrl, tx, ty, tw, th, SWP_NOZORDER);

  m_PropPhysicsDlg = new CPropertyPhysicsDlg(tab_ctrl);
  m_PropPhysicsDlg->Create();
  m_PropPhysicsDlg->SetWindowPos(tab_ctrl, tx, ty, tw, th, SWP_NOZORDER);

  //	Display current tab dialog
  switch (m_current_tab) {
  case TAB_PHYSICS:
    m_KeypadTabDialog = m_PropPhysicsDlg;
    break;
  case TAB_AI:
    m_KeypadTabDialog = m_PropAIDlg;
    break;
  default:
    Int3();
  }

  tab_ctrl->SetCurSel(m_current_tab);

  return TRUE; // return TRUE unless you set the focus to a control
               // EXCEPTION: OCX Property Pages should return FALSE
}

void CObjectPropertiesDialog::RunKeypadFunction(int code) {
  if (m_KeypadTabDialog)
    m_KeypadTabDialog->RunKeypadFunction(code);
}

void CObjectPropertiesDialog::Activate() {
  if (m_KeypadTabDialog)
    m_KeypadTabDialog->Activate();
}

void CObjectPropertiesDialog::Deactivate() {
  if (m_KeypadTabDialog)
    m_KeypadTabDialog->Deactivate();
}

void CObjectPropertiesDialog::OnShowWindow(BOOL bShow, UINT nStatus) {
  CDialog::OnShowWindow(bShow, nStatus);

  m_Initializing = FALSE;

  if (bShow)
    m_KeypadTabDialog->ShowWindow(SW_SHOW);
}

BOOL CObjectPropertiesDialog::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT *pResult) {
  // TODO: Add your specialized code here and/or call the base class
  //	Perform notification requests on the keypad tab dialog bar and other
  //	custom control bars.
  NMHDR *nmhdr;

  assert(lParam != 0);

  nmhdr = (NMHDR *)lParam;

  switch (wParam) {
  case IDC_PROPERTYTAB:
    DoKeypadTabNotify(nmhdr);
    break;
  }

  return CDialog::OnNotify(wParam, lParam, pResult);
}

void CObjectPropertiesDialog::OnSize(UINT nType, int cx, int cy) {
  RECT rect;
  CTabCtrl *tab_ctrl;

  CDialog::OnSize(nType, cx, cy);

  if (m_Initializing)
    return;

  SetRect(&rect, 0, 0, cx, cy);
  ::AdjustWindowRect(&rect, GetStyle(), FALSE);

  m_keypad_w = rect.right - rect.left;
  m_keypad_h = rect.bottom - rect.top;

  SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);

  //	Resize tab to fit current modeless dialog size
  RECT tab_rect, tab_winrect;
  int tx, ty, tw, th;

  tab_ctrl = (CTabCtrl *)GetDlgItem(IDC_PROPERTYTAB);

  GetClientRect(&tab_rect);
  tab_ctrl->MoveWindow(&tab_rect);

  tab_ctrl->GetWindowRect(&tab_winrect);
  ::CopyRect(&tab_rect, &tab_winrect);
  tab_ctrl->AdjustRect(FALSE, &tab_rect);
  tx = tab_rect.left - tab_winrect.left;
  ty = tab_rect.top - tab_winrect.top;
  tw = tab_rect.right - tab_rect.left;
  th = tab_rect.bottom - tab_rect.top;

  m_PropAIDlg->SetWindowPos(tab_ctrl, tx, ty, tw, th, SWP_NOZORDER);
  m_PropPhysicsDlg->SetWindowPos(tab_ctrl, tx, ty, tw, th, SWP_NOZORDER);
}

void CObjectPropertiesDialog::OnMove(int x, int y) {
  CDialog::OnMove(x, y);

  // TODO: Add your message handler code here
  //	if we haven't placed this dialog anywhere, wait till parent does it for us.
  if (m_Initializing)
    return;

  RECT rect;
  GetWindowRect(&rect);
  GetParent()->ScreenToClient(&rect);
  m_keypad_x = rect.left;
  m_keypad_y = rect.top;

  SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
}

int CObjectPropertiesDialog::OnCreate(LPCREATESTRUCT lpCreateStruct) {
  if (CDialog::OnCreate(lpCreateStruct) == -1)
    return -1;

  // TODO: Add your specialized creation code here
  m_Initializing = TRUE;

  return 0;
}

void CObjectPropertiesDialog::DoKeypadTabNotify(NMHDR *nmhdr) // take care of tab control notifications
{
  CTabCtrl *tab_ctrl;

  tab_ctrl = (CTabCtrl *)GetDlgItem(IDC_PROPERTYTAB);

  switch (nmhdr->code) {
  case TCN_SELCHANGE:
    // Tab control changed. So update the controls currently displayed.
    m_current_tab = (int)tab_ctrl->GetCurSel();
    assert(m_current_tab > -1);

    switch (m_current_tab) {
    case TAB_AI:
      if (m_KeypadTabDialog)
        m_KeypadTabDialog->ShowWindow(SW_HIDE);
      m_KeypadTabDialog = m_PropAIDlg;
      m_KeypadTabDialog->ShowWindow(SW_SHOW);
      break;

    case TAB_PHYSICS:
      if (m_KeypadTabDialog)
        m_KeypadTabDialog->ShowWindow(SW_HIDE);
      m_KeypadTabDialog = m_PropPhysicsDlg;
      m_KeypadTabDialog->ShowWindow(SW_SHOW);
      break;
    }
    break;
  }
}

void CObjectPropertiesDialog::OnOK() {
  // TODO: Add extra validation here
  ddio_KeyFlush();
  m_PropPhysicsDlg->UpdateData(true);
  CDialog::OnOK();
  m_bBounce = m_PropPhysicsDlg->m_bBounce;
  m_fDrag = m_PropPhysicsDlg->m_fDrag;
  m_fFullRotThrust = m_PropPhysicsDlg->m_fFullRotThrust;
  m_fFullThrust = m_PropPhysicsDlg->m_fFullThrust;
  m_bGravity = m_PropPhysicsDlg->m_bGravity;
  m_bLevelling = m_PropPhysicsDlg->m_bLevelling;
  m_bMagnetism = m_PropPhysicsDlg->m_bMagnetism;
  m_fMass = m_PropPhysicsDlg->m_fMass;
  m_fMaxTurnrollrate = m_PropPhysicsDlg->m_fMaxTurnrollrate;
  m_iNumBounces = m_PropPhysicsDlg->m_iNumBounces;
  m_bPersistent = m_PropPhysicsDlg->m_bPersistent;
  m_fRotDrag = m_PropPhysicsDlg->m_fRotDrag;
  m_fRotThrustX = m_PropPhysicsDlg->m_fRotThrustX;
  m_fRotThrustY = m_PropPhysicsDlg->m_fRotThrustY;
  m_fRotThrustZ = m_PropPhysicsDlg->m_fRotThrustZ;
  m_fRotVelX = m_PropPhysicsDlg->m_fRotVelX;
  m_fRotVelY = m_PropPhysicsDlg->m_fRotVelY;
  m_fRotVelZ = m_PropPhysicsDlg->m_fRotVelZ;
  m_bSticks = m_PropPhysicsDlg->m_bSticks;
  m_fThrustX = m_PropPhysicsDlg->m_fThrustX;
  m_fThrustY = m_PropPhysicsDlg->m_fThrustY;
  m_fThrustZ = m_PropPhysicsDlg->m_fThrustZ;
  m_bTurnroll = m_PropPhysicsDlg->m_bTurnroll;
  m_fTurnrollRatio = m_PropPhysicsDlg->m_fTurnrollRatio;
  m_sTurnrollAngle = m_PropPhysicsDlg->m_sTurnrollAngle;
  m_bUsesThrust = m_PropPhysicsDlg->m_bUsesThrust;
  m_fVelocityX = m_PropPhysicsDlg->m_fVelocityX;
  m_fVelocityY = m_PropPhysicsDlg->m_fVelocityY;
  m_fVelocityZ = m_PropPhysicsDlg->m_fVelocityZ;
  m_bWiggle = m_PropPhysicsDlg->m_bWiggle;
  m_fWiggleAmplitude = m_PropPhysicsDlg->m_fWiggleAmplitude;
  m_fWigglesPerSecond = m_PropPhysicsDlg->m_fWigglesPerSecond;
  m_bWind = m_PropPhysicsDlg->m_bWind;
  m_bUnlimitedBounce = m_PropPhysicsDlg->m_bUnlimitedBounce;
}

void CObjectPropertiesDialog::OnCancel() {
  // TODO: Add extra cleanup here
  ddio_KeyFlush();
  CDialog::OnCancel();
}
