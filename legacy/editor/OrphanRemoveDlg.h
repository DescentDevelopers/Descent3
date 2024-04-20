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

/*
* $Logfile: /DescentIII/Main/editor/OrphanRemoveDlg.h $
* $Revision: 1.1.1.1 $
* $Date: 2003-08-26 03:57:38 $
* $Author: kevinb $
*
* Orphan file delete
*
* $Log: not supported by cvs2svn $
 * 
 * 3     3/19/99 10:35a Jeff
 * enable/disable controls when thinking
 * 
 * 2     3/18/99 8:41p Jeff
 * created orphan hunter dialog
*
* $NoKeywords: $
*/

#if !defined(AFX_ORPHANREMOVEDLG_H__CCDDAB80_DD4F_11D2_AB2B_006008BF0B09__INCLUDED_)
#define AFX_ORPHANREMOVEDLG_H__CCDDAB80_DD4F_11D2_AB2B_006008BF0B09__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OrphanRemoveDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COrphanRemoveDlg dialog

class COrphanRemoveDlg : public CDialog
{
// Construction
public:
	COrphanRemoveDlg(CWnd* pParent = NULL);   // standard constructor
	void SetStatus(char *status);
	void Enable(bool enable);

// Dialog Data
	//{{AFX_DATA(COrphanRemoveDlg)
	enum { IDD = IDD_ORPHAN };
	CCheckListBox	m_List;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COrphanRemoveDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COrphanRemoveDlg)
	virtual void OnOK();
	afx_msg void OnSelchangeDirectory();
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ORPHANREMOVEDLG_H__CCDDAB80_DD4F_11D2_AB2B_006008BF0B09__INCLUDED_)
