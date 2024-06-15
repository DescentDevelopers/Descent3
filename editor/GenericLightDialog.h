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

#if !defined(AFX_GENERICLIGHTDIALOG_H__913198E1_25DA_11D1_9798_006097D5770E__INCLUDED_)
#define AFX_GENERICLIGHTDIALOG_H__913198E1_25DA_11D1_9798_006097D5770E__INCLUDED_

#include "objinfo.h"

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// GenericLightDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGenericLightDialog dialog

class CGenericLightDialog : public CDialog {
  // Construction
public:
  CGenericLightDialog(light_info *current, CWnd *pParent = NULL); // standard constructor

  // Dialog Data
  //{{AFX_DATA(CGenericLightDialog)
  enum { IDD = IDD_GENERICLIGHT };
  // NOTE: the ClassWizard will add data members here
  //}}AFX_DATA

  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CGenericLightDialog)
protected:
  virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
                                                   //}}AFX_VIRTUAL

  // Implementation
protected:
  // Generated message map functions
  //{{AFX_MSG(CGenericLightDialog)
  afx_msg void OnKillfocusGenericLightDistance();
  afx_msg void OnKillfocusGenericBlueLight();
  afx_msg void OnKillfocusGenericGreenLight();
  afx_msg void OnKillfocusGenericRedLight();
  afx_msg void OnKillfocusGenericPulseTime();
  afx_msg void OnFlickerRadio();
  afx_msg void OnUseTimebitsRadio();
  afx_msg void OnPulse();
  afx_msg void OnAlwaysOnRadio();
  virtual BOOL OnInitDialog();
  afx_msg void OnTimeCheck1();
  afx_msg void OnTimeCheck2();
  afx_msg void OnTimeCheck3();
  afx_msg void OnTimeCheck4();
  afx_msg void OnTimeCheck5();
  afx_msg void OnTimeCheck6();
  afx_msg void OnTimeCheck7();
  afx_msg void OnTimeCheck8();
  afx_msg void OnRenderStatic();
  afx_msg void OnRenderGouraud();
  afx_msg void OnRenderLightmaps();
  afx_msg void OnPulseToSecond();
  afx_msg void OnKillfocusGenericRedLight2();
  afx_msg void OnKillfocusGenericGreenLight2();
  afx_msg void OnKillfocusGenericBlueLight2();
  afx_msg void OnFlickerSlightlyCheck();
  afx_msg void OnKillfocusFlickerSlightlyEdit();
  afx_msg void OnDirectional();
  afx_msg void OnKillfocusFovEdit();
  afx_msg void OnNoSpecularCheck();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()

private:
  light_info *m_lightinfo;
  void UpdateDialog();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GENERICLIGHTDIALOG_H__913198E1_25DA_11D1_9798_006097D5770E__INCLUDED_)
