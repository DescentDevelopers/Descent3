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

#if !defined(AFX_MANAGECHECKIN_H__2F961C80_7586_11D2_9798_004095AA4E37__INCLUDED_)
#define AFX_MANAGECHECKIN_H__2F961C80_7586_11D2_9798_004095AA4E37__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ManageCheckin.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CManageCheckin dialog

class CManageCheckin : public CDialog {
  // Construction
public:
  CManageCheckin(CWnd *pParent = NULL); // standard constructor

  // Dialog Data
  //{{AFX_DATA(CManageCheckin)
  enum { IDD = IDD_MULTICHECK };
  // NOTE: the ClassWizard will add data members here
  //}}AFX_DATA

  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CManageCheckin)
protected:
  virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
                                                   //}}AFX_VIRTUAL

  // Implementation
protected:
  // Generated message map functions
  //{{AFX_MSG(CManageCheckin)
  virtual BOOL OnInitDialog();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MANAGECHECKIN_H__2F961C80_7586_11D2_9798_004095AA4E37__INCLUDED_)
