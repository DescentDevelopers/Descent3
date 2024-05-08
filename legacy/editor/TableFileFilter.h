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

 * $Logfile: /DescentIII/Main/editor/TableFileFilter.h $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:57:39 $
 * $Author: kevinb $
 *
 * 
 *
 * $Log: not supported by cvs2svn $
 * 
 * 5     2/09/99 4:19p Nate
 * Added to the TableFileFilter interface
 * 
 * 4     10/08/98 10:30a Nate
 * Initial version
 *
 * $NoKeywords: $
 */

#if !defined(AFX_TABLEFILEFILTER_H__E849A9C0_5D0F_11D2_A4E0_00A0C96ED60D__INCLUDED_)
#define AFX_TABLEFILEFILTER_H__E849A9C0_5D0F_11D2_A4E0_00A0C96ED60D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TableFileFilter.h : header file
//

#include "TableFileFilterMng.h"

/////////////////////////////////////////////////////////////////////////////
// TableFileFilter dialog

class TableFileFilter : public CDialog
{
// Construction
public:
	TableFileFilter(CWnd* pParent = NULL);   // standard constructor
	void SetColumnWidths(void);
	
	PageDataList m_PageList;	// the page data list

// Dialog Data
	//{{AFX_DATA(TableFileFilter)
	enum { IDD = IDD_TABLE_FILE_FILTER };
	CEdit	m_OutputFileEdit;
	CListBox	m_LevelList;
	CListCtrl	m_DataList;
	CString	m_NumPagesText;
	CString	m_StatusText;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(TableFileFilter)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(TableFileFilter)
	afx_msg void OnFileExit();
	afx_msg void OnFileSave();
	afx_msg void OnFileSaveAs();
	afx_msg void OnFileLoad();
	afx_msg void OnClose();
	virtual BOOL OnInitDialog();
	afx_msg void OnBtnAddLevel();
	afx_msg void OnBtnAddPage();
	afx_msg void OnBtnCreateNewTableFile();
	afx_msg void OnBtnRemoveLevel();
	afx_msg void OnBtnRemovePage();
	afx_msg void OnFileNew();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABLEFILEFILTER_H__E849A9C0_5D0F_11D2_A4E0_00A0C96ED60D__INCLUDED_)
