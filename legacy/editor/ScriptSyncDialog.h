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

#if !defined(AFX_SCRIPTSYNCDIALOG_H__9C4083C0_CB58_11D2_AB2B_006008BF0B09__INCLUDED_)
#define AFX_SCRIPTSYNCDIALOG_H__9C4083C0_CB58_11D2_AB2B_006008BF0B09__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ScriptSyncDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CScriptSyncDialog dialog

/////////////////////////////////////////////////////////////////////////////
// CMyListCtrl window
#include "manage_external.h"

#define STATE_COMPILED	0
#define STATE_OOD		1
#define STATE_OOS		2
#define STATE_MISSING	3
#define STATE_ERROOD	4
#define STATE_ERROOS	5
#define STATE_NOCOMPILER	6

struct
{
	char filename[PAGENAME_LEN];
	int state;
}tFileInfo;


class CMyListCtrl : public CListCtrl
{
// Construction
public:
	CMyListCtrl();
	void Initialize(void);
	int AddItem(char *name,int state);
	bool SetItem(int item,int state);
	bool GetItem(int item,char *name,char *state);	
	void SelectItem(int index);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyListCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMyListCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CMyListCtrl)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};


class CScriptSyncDialog : public CDialog
{
// Construction
public:
	CScriptSyncDialog(CWnd* pParent = NULL);   // standard constructor
	void DoStuff(void);
	void BuildList(void);
	void defer(void);
	bool early_exit;
	bool first_paint;

	tFileInfo *m_Files;
	int m_NumFiles;
	bool m_InStuff;
	int m_Index;
	bool m_Done;
	char old_dir[_MAX_PATH];
	bool m_FillingIn;
	int m_ErrorCount;
	int m_HasCompiler;

// Dialog Data
	//{{AFX_DATA(CScriptSyncDialog)
	enum { IDD = IDD_SYNCSCRIPTS };
	CMyListCtrl	m_List;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScriptSyncDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CScriptSyncDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};



//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCRIPTSYNCDIALOG_H__9C4083C0_CB58_11D2_AB2B_006008BF0B09__INCLUDED_)
