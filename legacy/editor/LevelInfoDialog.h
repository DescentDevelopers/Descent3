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

#if !defined(AFX_LEVELINFODIALOG_H__0663E763_D375_11D1_A13A_0060089A8026__INCLUDED_)
#define AFX_LEVELINFODIALOG_H__0663E763_D375_11D1_A13A_0060089A8026__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// LevelInfoDialog.h : header file
//

#include "mission.h"

/////////////////////////////////////////////////////////////////////////////
// CLevelInfoDialog dialog

class CLevelInfoDialog : public CDialog
{
// Construction
public:
	CLevelInfoDialog(level_info *li,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CLevelInfoDialog)
	enum { IDD = IDD_LEVEL_INFO };
	CString	m_copyright;
	CString	m_designer;
	CString	m_name;
	CString	m_notes;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLevelInfoDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	//Copy data back out of dialog
	void GetLevelInfo(level_info *li);

	//The data we're editing
	level_info *m_level_info;

	// Generated message map functions
	//{{AFX_MSG(CLevelInfoDialog)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LEVELINFODIALOG_H__0663E763_D375_11D1_A13A_0060089A8026__INCLUDED_)
