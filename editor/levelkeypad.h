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

#if !defined(AFX_LEVELKEYPAD_H__01EF05C4_3D9C_11D2_A13A_0060089A8026__INCLUDED_)
#define AFX_LEVELKEYPAD_H__01EF05C4_3D9C_11D2_A13A_0060089A8026__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// levelkeypad.h : header file
//

#include "keypaddialog.h"
#include "RoomListCombo.h"
#include "ObjectListCombo.h"
#include "TriggerListCombo.h"

/////////////////////////////////////////////////////////////////////////////
// levelkeypad dialog

class levelkeypad : public CKeypadDialog {
  int m_current_goal;
  int m_current_item;
  void DoGoalFlagToggle(int flag);
  bool ValidateCurrentItem();
  bool ValidateCurrentGoal();
  void SetAmbCharVal(char i, char item, int n_id);
  void SetAmbType(int i, int n_id);

  // Construction
public:
  levelkeypad(CWnd *pParent = NULL); // standard constructor
  void UpdateKeypad(int mask);

  // Dialog Data
  //{{AFX_DATA(levelkeypad)
  enum { IDD = IDD_LEVELKEYPAD };
  CTriggerListCombo m_trigger_combo;
  CObjectListCombo m_object_combo;
  CRoomListCombo m_room_combo;
  //}}AFX_DATA

  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(levelkeypad)
protected:
  virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
                                                   //}}AFX_VIRTUAL

  // Implementation
protected:
  // Generated message map functions
  //{{AFX_MSG(levelkeypad)
  afx_msg void OnPaint();
  afx_msg void OnKillfocusLevelGravityEdit();
  virtual BOOL OnInitDialog();
  afx_msg void OnLevelPrevGoalButton();
  afx_msg void OnLevelNextGoalButton();
  afx_msg void OnLevelNewGoalButton();
  afx_msg void OnKillfocusLevelGoalNameEdit();
  afx_msg void OnKillfocusLevelGoalLocNameEdit();
  afx_msg void OnKillfocusLevelGoalDescEdit();
  afx_msg void OnLevelGoalNewItemButton();
  afx_msg void OnLevelGoalNextItemButton();
  afx_msg void OnLevelGoalPrevItemButton();
  afx_msg void OnLevelGoalDeleteItemButton();
  afx_msg void OnLevelGoalRoomItemRadio();
  afx_msg void OnLevelGoalCellItemRadio();
  afx_msg void OnLevelGoalObjItemRadio();
  afx_msg void OnGoalAutoCompleteCheck();
  afx_msg void OnLevelGoalSecGoalCheck();
  afx_msg void OnLevelGoalEnabledCheck();
  afx_msg void OnLevelGoalCompletedCheck();
  afx_msg void OnLevelGoalTlgCheck();
  afx_msg void OnLevelGoalGdklCheck();
  afx_msg void OnLevelGoalNlbCheck();
  afx_msg void OnLevelGoalPasteObjrefItemButton();
  afx_msg void OnLevelGoalGotoItemButton();
  afx_msg void OnLevelDeleteGoalButton();
  afx_msg void OnKillfocusLevelGoalItemEdit();
  afx_msg void OnKillfocusLevelGoalPriorityEdit1();
  afx_msg void OnLevelGoalEndLevelCheck();
  afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar);
  afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar);
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnSelendokAmbType1();
  afx_msg void OnSelendokAmbType2();
  afx_msg void OnSelendokAmbType3();
  afx_msg void OnSelendokAmbType4();
  afx_msg void OnSelendokAmbType5();
  afx_msg void OnSelendokAmbType6();
  afx_msg void OnKillfocusAmbMax1();
  afx_msg void OnKillfocusAmbMax2();
  afx_msg void OnKillfocusAmbMax3();
  afx_msg void OnKillfocusAmbMax4();
  afx_msg void OnKillfocusAmbMax5();
  afx_msg void OnKillfocusAmbMax6();
  afx_msg void OnKillfocusAmbMin1();
  afx_msg void OnKillfocusAmbMin2();
  afx_msg void OnKillfocusAmbMin3();
  afx_msg void OnKillfocusAmbMin4();
  afx_msg void OnKillfocusAmbMin5();
  afx_msg void OnKillfocusAmbMin6();
  afx_msg void OnKillfocusAmbNGroups1();
  afx_msg void OnKillfocusAmbNGroups2();
  afx_msg void OnKillfocusAmbNGroups3();
  afx_msg void OnKillfocusAmbNGroups4();
  afx_msg void OnKillfocusAmbNGroups5();
  afx_msg void OnKillfocusAmbNGroups6();
  afx_msg void OnAmbOutside1();
  afx_msg void OnAmbOutside2();
  afx_msg void OnAmbOutside3();
  afx_msg void OnAmbOutside4();
  afx_msg void OnAmbOutside5();
  afx_msg void OnAmbOutside6();
  afx_msg void OnKillfocusLevelGoalCompletionEdit();
  afx_msg void OnLgActivateRadio();
  afx_msg void OnLgEnterRadio();
  afx_msg void OnLgDestroyedRadio();
  afx_msg void OnLgPpwCollideRadio();
  afx_msg void OnLgLcRadio();
  afx_msg void OnLgDallasRadio();
  afx_msg void OnTriggerRadio();
  afx_msg void OnLevelGoalMineRadio();
  afx_msg void OnSelendokLevelgoalRoomCombo();
  afx_msg void OnSelendokLevelgoalobjectCombo();
  afx_msg void OnSelendokLevelgoaltriggerCombo();
  afx_msg void OnKillfocusEdit57();
  afx_msg void OnLevelCeilingEverywhere();
  afx_msg void OnKillfocusLevelCeilingEdit();
  afx_msg void OnSelendokFfosCombo();
  afx_msg void OnSelendokBgosCombo();
  afx_msg void OnKillfocusFft1Edit();
  afx_msg void OnKillfocusFft2Edit();
  afx_msg void OnSelendokFft1Combo();
  afx_msg void OnSelendokFft2Combo();
  afx_msg void OnKillfocusFixedFft1Edit();
  afx_msg void OnSetfocusFft1Combo();
  afx_msg void OnSetfocusFft2Combo();
  afx_msg void OnLevelPiw();
  afx_msg void OnKillfocusFft3Edit();
  afx_msg void OnSelendokFft3Combo();
  afx_msg void OnSetfocusFft3Combo();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
private:
  void UpdateDialog();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LEVELKEYPAD_H__01EF05C4_3D9C_11D2_A13A_0060089A8026__INCLUDED_)
