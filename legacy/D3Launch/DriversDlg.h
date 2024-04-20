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

#if !defined(AFX_DRIVERSDLG_H__0325ACC0_CC01_11D2_A4E0_00A0C96ED60D__INCLUDED_)
#define AFX_DRIVERSDLG_H__0325ACC0_CC01_11D2_A4E0_00A0C96ED60D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DriversDlg.h : header file
//


// Installation selections
#define INSTALL_NONE				0
#define INSTALL_DIRECTX			1
#define INSTALL_GLSETUP			2

/////////////////////////////////////////////////////////////////////////////
// CDriversDlg dialog

class CDriversDlg : public CDialog
{
// Construction
public:
	CDriversDlg(CWnd* pParent = NULL);   // standard constructor

	int m_Selection;

// Dialog Data
	//{{AFX_DATA(CDriversDlg)
	enum { IDD = IDD_DRIVERS_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDriversDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDriversDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDirectxButton();
	afx_msg void OnGlsetupButton();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DRIVERSDLG_H__0325ACC0_CC01_11D2_A4E0_00A0C96ED60D__INCLUDED_)
