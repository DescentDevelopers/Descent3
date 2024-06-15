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

#if !defined(AFX_INSSONGDLG_H__B4518671_1FF8_11D2_B2B3_00A0C94B8467__INCLUDED_)
#define AFX_INSSONGDLG_H__B4518671_1FF8_11D2_B2B3_00A0C94B8467__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// InsSongDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CInsSongDlg dialog

class CInsSongDlg : public CDialog {
  // Construction
public:
  CInsSongDlg(CWnd *pParent = NULL); // standard constructor

  // Dialog Data
  //{{AFX_DATA(CInsSongDlg)
  enum { IDD = IDD_INSERT_SONG };
  CString m_SongPath;
  //}}AFX_DATA

  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CInsSongDlg)
protected:
  virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
                                                   //}}AFX_VIRTUAL

  // Implementation
protected:
  // Generated message map functions
  //{{AFX_MSG(CInsSongDlg)
  afx_msg void OnBrowse();
  virtual BOOL OnInitDialog();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INSSONGDLG_H__B4518671_1FF8_11D2_B2B3_00A0C94B8467__INCLUDED_)
