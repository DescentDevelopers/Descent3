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

* $Logfile: /DescentIII/Main/editor/DallasGenericPromptDlg.h $
* $Revision: 1.1.1.1 $
* $Date: 2003-08-26 03:57:37 $
* $Author: kevinb $
*
* This is the header for the DALLAS generic prompt dialog
*
* $Log: not supported by cvs2svn $
 *
 * 2     12/13/98 6:08p Nate
 * Initial version
*
* $NoKeywords: $
*/

#if !defined(AFX_DALLASGENERICPROMPTDLG_H__94807B41_929B_11D2_A4E0_00A0C96ED60D__INCLUDED_)
#define AFX_DALLASGENERICPROMPTDLG_H__94807B41_929B_11D2_A4E0_00A0C96ED60D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DallasGenericPromptDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDallasGenericPromptDlg dialog

class CDallasGenericPromptDlg : public CDialog {
  // Construction
public:
  CDallasGenericPromptDlg(CWnd *pParent = NULL); // standard constructor

  CString m_DialogTitle;
  CString m_PromptText;
  CString m_PromptData;
  int m_MaxDataLength;

  // Dialog Data
  //{{AFX_DATA(CDallasGenericPromptDlg)
  enum { IDD = IDD_DALLAS_GENERIC_PROMPT_DIALOG };
  CEdit m_GenericDataEdit;
  CStatic m_GenericPromptStatic;
  //}}AFX_DATA

  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CDallasGenericPromptDlg)
protected:
  virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
                                                   //}}AFX_VIRTUAL

  // Implementation
protected:
  // Generated message map functions
  //{{AFX_MSG(CDallasGenericPromptDlg)
  virtual BOOL OnInitDialog();
  virtual void OnOK();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DALLASGENERICPROMPTDLG_H__94807B41_929B_11D2_A4E0_00A0C96ED60D__INCLUDED_)
