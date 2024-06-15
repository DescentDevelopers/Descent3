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

#if !defined(AFX_ROOMLISTCOMBO_H__EBF11B60_C8D2_11D2_A13A_0060089A83BE__INCLUDED_)
#define AFX_ROOMLISTCOMBO_H__EBF11B60_C8D2_11D2_A13A_0060089A83BE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RoomListCombo.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRoomListCombo window

class CRoomListCombo : public CComboBox {
  // Construction
public:
  CRoomListCombo();
  void Init(int selected);
  void SetSelected(int selected);
  int GetSelected();

  // Attributes
public:
  // Operations
public:
  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CRoomListCombo)
  //}}AFX_VIRTUAL

  // Implementation
public:
  virtual ~CRoomListCombo();

  // Generated message map functions
protected:
  //{{AFX_MSG(CRoomListCombo)
  afx_msg void OnSetfocus();
  //}}AFX_MSG

  DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ROOMLISTCOMBO_H__EBF11B60_C8D2_11D2_A13A_0060089A83BE__INCLUDED_)
