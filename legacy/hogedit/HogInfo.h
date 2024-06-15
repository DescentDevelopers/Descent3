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

--- HISTORICAL COMMENTS FOLLOW ---

 * $Logfile: /DescentIII/Main/hogedit/HogInfo.h $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:57:56 $
 * $Author: kevinb $
 *
 *
 *
 * $Log: not supported by cvs2svn $
 *
 * 2     8/16/98 4:22p Nate
 * Added message deferal and new hog info dialog
 *
 * $NoKeywords: $
 */

#if !defined(AFX_HOGINFO_H__2BF70B62_351D_11D2_8CBD_00A0C96ED60D__INCLUDED_)
#define AFX_HOGINFO_H__2BF70B62_351D_11D2_8CBD_00A0C96ED60D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// HogInfo.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CHogInfo dialog

class CHogInfo : public CDialog {
  // Construction
public:
  CHogInfo(CWnd *pParent = NULL); // standard constructor

  // Dialog Data
  //{{AFX_DATA(CHogInfo)
  enum { IDD = IDD_HOGINFO };
  CString m_NumFiles;
  CString m_Location;
  CString m_SizeBytes;
  CString m_SizeMB;
  //}}AFX_DATA

  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CHogInfo)
protected:
  virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
                                                   //}}AFX_VIRTUAL

  // Implementation
protected:
  // Generated message map functions
  //{{AFX_MSG(CHogInfo)
  virtual BOOL OnInitDialog();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HOGINFO_H__2BF70B62_351D_11D2_8CBD_00A0C96ED60D__INCLUDED_)
