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

#if !defined(AFX_OSIRISSTATUSDLG_H__0D482AE1_028E_11D1_8901_0020A90CB6F2__INCLUDED_)
#define AFX_OSIRISSTATUSDLG_H__0D482AE1_028E_11D1_8901_0020A90CB6F2__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// OsirisStatusDlg.h : header file
//

#include "d3x.h"

/////////////////////////////////////////////////////////////////////////////
// COsirisStatusDlg dialog

class COsirisStatusDlg : public CDialog
{
// Construction
public:
	COsirisStatusDlg(CWnd* pParent = NULL);   // standard constructor
	void SetScript(char *script, tD3XProgram *d3x);

	// Dialog Data
	//{{AFX_DATA(COsirisStatusDlg)
	enum { IDD = IDD_OSIRIS_STATUS };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COsirisStatusDlg)
	public:
	virtual INT_PTR DoModal();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COsirisStatusDlg)
	afx_msg void OnOsirisDone();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	CString m_Script;				
	tD3XProgram *m_Program;								// program to output to.
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OSIRISSTATUSDLG_H__0D482AE1_028E_11D1_8901_0020A90CB6F2__INCLUDED_)
