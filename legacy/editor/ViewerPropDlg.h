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
* $Logfile: /DescentIII/Main/editor/ViewerPropDlg.h $
* $Revision: 1.1.1.1 $
* $Date: 2003-08-26 03:57:39 $
* $Author: kevinb $
*
* Viewer Properties Dialog Header
*
* $Log: not supported by cvs2svn $
 * 
 * 3     4/17/99 2:46p Matt
 * Implemented viewer properties dialog functions.
 * 
 * 2     4/16/99 3:53p Nate
 * Initial version
*
* $NoKeywords: $
*/

#if !defined(AFX_VIEWERPROPDLG_H__56E11500_F3FE_11D2_A4E0_00A0C96ED60D__INCLUDED_)
#define AFX_VIEWERPROPDLG_H__56E11500_F3FE_11D2_A4E0_00A0C96ED60D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ViewerPropDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CViewerPropDlg dialog

class CViewerPropDlg : public CDialog
{
// Construction
public:
	CViewerPropDlg(CWnd* pParent = NULL);   // standard constructor

	void UpdateOrientation();
	void UpdatePosition();

// Dialog Data
	//{{AFX_DATA(CViewerPropDlg)
	enum { IDD = IDD_VIEWER_DIALOG };
	CEdit	m_ZPosEdit;
	CEdit	m_YPosEdit;
	CEdit	m_XPosEdit;
	CButton	m_ZMoveCheck;
	CButton	m_YMoveCheck;
	CButton	m_XMoveCheck;
	CEdit	m_PitchEdit;
	CEdit	m_HeadingEdit;
	CEdit	m_BankEdit;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CViewerPropDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CViewerPropDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnAlignUpWithYposButton();
	afx_msg void OnAlignXnegButton();
	afx_msg void OnAlignXposButton();
	afx_msg void OnAlignYnegButton();
	afx_msg void OnAlignYposButton();
	afx_msg void OnAlignZnegButton();
	afx_msg void OnAlignZposButton();
	afx_msg void OnOrientCommitButton();
	afx_msg void OnPosCommitButton();
	afx_msg void OnClose();
	afx_msg void OnXMoveCheck();
	afx_msg void OnYMoveCheck();
	afx_msg void OnZMoveCheck();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIEWERPROPDLG_H__56E11500_F3FE_11D2_A4E0_00A0C96ED60D__INCLUDED_)
