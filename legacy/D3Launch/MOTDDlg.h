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

#if !defined(AFX_MOTDDLG_H__C5900F80_3B79_11D2_8CBD_00A0C96ED60D__INCLUDED_)
#define AFX_MOTDDLG_H__C5900F80_3B79_11D2_8CBD_00A0C96ED60D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// MOTDDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMOTDDlg dialog

class CMOTDDlg : public CDialog
{
// Construction
public:
	CMOTDDlg(CWnd* pParent = NULL);   // standard constructor
	BOOL ParseMOTDFile(void);

// Dialog Data
	//{{AFX_DATA(CMOTDDlg)
	enum { IDD = IDD_MOTD_DIALOG };
	CListBox	m_TextList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMOTDDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMOTDDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MOTDDLG_H__C5900F80_3B79_11D2_8CBD_00A0C96ED60D__INCLUDED_)
