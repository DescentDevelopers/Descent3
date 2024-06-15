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

#if !defined(AFX_DALLASUSERTYPESDLG_H__92061D21_B460_11D2_A4E0_00A0C96ED60D__INCLUDED_)
#define AFX_DALLASUSERTYPESDLG_H__92061D21_B460_11D2_A4E0_00A0C96ED60D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DallasUserTypesDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDallasUserTypesDlg dialog

class CDallasUserTypesDlg : public CDialog {
  // Construction
public:
  CDallasUserTypesDlg(CWnd *pParent = NULL); // standard constructor

  // Dialog Data
  //{{AFX_DATA(CDallasUserTypesDlg)
  enum { IDD = IDD_DALLAS_USERTYPES_DIALOG };
  CListBox m_ValuesListBox;
  CComboBox m_UserTypeCombo;
  //}}AFX_DATA

  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CDallasUserTypesDlg)
protected:
  virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
                                                   //}}AFX_VIRTUAL

  // Implementation
protected:
  // Generated message map functions
  //{{AFX_MSG(CDallasUserTypesDlg)
  virtual BOOL OnInitDialog();
  afx_msg void OnAddButton();
  afx_msg void OnChangeButton();
  afx_msg void OnDeleteButton();
  virtual void OnOK();
  afx_msg void OnSelchangeUtypeCombo();
  afx_msg void OnDblclkValuesList();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DALLASUSERTYPESDLG_H__92061D21_B460_11D2_A4E0_00A0C96ED60D__INCLUDED_)
