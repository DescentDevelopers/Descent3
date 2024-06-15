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

#if !defined(AFX_SETUPPROPSHEET_H__00C45F60_1F5F_11D3_A4E1_00A0C96ED60D__INCLUDED_)
#define AFX_SETUPPROPSHEET_H__00C45F60_1F5F_11D3_A4E1_00A0C96ED60D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SetupPropSheet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSetupPropSheet

class CSetupPropSheet : public CPropertySheet {
  DECLARE_DYNAMIC(CSetupPropSheet)

  // Construction
public:
  CSetupPropSheet(UINT nIDCaption, CWnd *pParentWnd = NULL, UINT iSelectPage = 0);
  CSetupPropSheet(LPCTSTR pszCaption, CWnd *pParentWnd = NULL, UINT iSelectPage = 0);

  // Attributes
public:
  // Operations
public:
  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CSetupPropSheet)
public:
  virtual BOOL OnInitDialog();
  //}}AFX_VIRTUAL

  // Implementation
public:
  virtual ~CSetupPropSheet();

  // Generated message map functions
protected:
  //{{AFX_MSG(CSetupPropSheet)
  // NOTE - the ClassWizard will add and remove member functions here.
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETUPPROPSHEET_H__00C45F60_1F5F_11D3_A4E1_00A0C96ED60D__INCLUDED_)
