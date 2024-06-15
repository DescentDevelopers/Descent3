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

#if !defined(AFX_PLAYERWEAPONSDIALOG_H__C8CA18E3_545B_11D1_97A7_004095AA4FAE__INCLUDED_)
#define AFX_PLAYERWEAPONSDIALOG_H__C8CA18E3_545B_11D1_97A7_004095AA4FAE__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PlayerWeaponsDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// PlayerWeaponsDialog dialog

class PlayerWeaponsDialog : public CDialog {
  // Construction
public:
  PlayerWeaponsDialog(int current_ship, CWnd *pParent = NULL); // standard constructor

  // Dialog Data
  //{{AFX_DATA(PlayerWeaponsDialog)
  enum { IDD = IDD_PLAYER_WEAPONS };
  CString m_current_wb_text;
  //}}AFX_DATA

  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(PlayerWeaponsDialog)
protected:
  virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
                                                   //}}AFX_VIRTUAL

  // Implementation
protected:
  void UpdateDialog(void);
  int CurrentWBIndex(void);

  int m_current_ship;

  // Generated message map functions
  //{{AFX_MSG(PlayerWeaponsDialog)
  afx_msg void OnEditWbButton();
  virtual BOOL OnInitDialog();
  afx_msg void OnSelendokCurrentWeaponBatteryCombo();
  afx_msg void OnFiresFusion();
  afx_msg void OnContinuousFiringSound();
  afx_msg void OnFiringReleaseSound();
  afx_msg void OnSelchangeFiringSoundPulldown();
  afx_msg void OnSelchangeReleaseSoundPulldown();
  afx_msg void OnSelchangeSpewPowerupPulldown();
  afx_msg void OnOnoff();
  afx_msg void OnZoom();
  afx_msg void OnKillfocusMaxAmmo();
  afx_msg void OnShowTenths();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLAYERWEAPONSDIALOG_H__C8CA18E3_545B_11D1_97A7_004095AA4FAE__INCLUDED_)
