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

* $Logfile: /DescentIII/Main/D3Launch/GLSetupDlg.h $
* $Revision: 1.1.1.1 $
* $Date: 2003-08-26 03:56:51 $
* $Author: kevinb $
*
* <insert description of file here>
*
* $Log: not supported by cvs2svn $
 * 
 * 2     5/20/99 12:53p Nate
 * Added GLSetup window, EAX mixer support, and CHFlight+Mouseman Mode
 * options
*
* $NoKeywords: $
*/

#if !defined(AFX_GLSETUPDLG_H__55A5E240_0E9F_11D3_A4E0_00A0C96ED60D__INCLUDED_)
#define AFX_GLSETUPDLG_H__55A5E240_0E9F_11D3_A4E0_00A0C96ED60D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GLSetupDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGLSetupDlg dialog

class CGLSetupDlg : public CDialog
{
// Construction
public:
	CGLSetupDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CGLSetupDlg)
	enum { IDD = IDD_GLSETUP_DIALOG };
	CButton	m_InstallButton;
	CStatic	m_Info3Text;
	CStatic	m_Info2Text;
	CStatic	m_Info1Text;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGLSetupDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGLSetupDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnInstallButton();
	afx_msg void OnVisitButton();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GLSETUPDLG_H__55A5E240_0E9F_11D3_A4E0_00A0C96ED60D__INCLUDED_)
