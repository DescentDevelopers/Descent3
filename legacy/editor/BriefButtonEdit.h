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

#if !defined(AFX_BRIEFBUTTONEDIT_H__01F120A2_427B_11D2_B359_006008BF0B09__INCLUDED_)
#define AFX_BRIEFBUTTONEDIT_H__01F120A2_427B_11D2_B359_006008BF0B09__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// BriefButtonEdit.h : header file
//

#include "telcomefxstructs.h"
#include "TelcomEffects.h"

/////////////////////////////////////////////////////////////////////////////
// CBriefButtonEdit dialog

class CBriefButtonEdit : public CDialog
{
// Construction
public:
	TCBUTTONDESC desc;
	CBriefButtonEdit(TCBUTTONDESC* desc=NULL,int id=0,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CBriefButtonEdit)
	enum { IDD = IDD_BRIEF_ADDBUTTON };
	CString	m_sFilename;
	CString	m_sFlashFilename;
	BOOL	m_bFlasher;
	float	m_fFlashStart;
	BOOL	m_bGlow;
	int		m_iID;
	int		m_iPID;
	int		m_iSID;
	int		m_iClickType;
	int		m_iButtonType;
	CString	m_sDesc;
	CString	m_sFilenameFocus;
	CString	m_sFlashFilenameFocus;
	int		m_iX;
	int		m_iY;
	int		m_ScreenToJump;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBriefButtonEdit)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void UpdateDialog(void);
	bool VerifyData(void);

	// Generated message map functions
	//{{AFX_MSG(CBriefButtonEdit)
	afx_msg void OnBriefBtChoose();
	afx_msg void OnBriefBtFlashChoose();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnBriefBtFlasher();
	afx_msg void OnBriefBtGlow();
	afx_msg void OnBriefBtChooseFocus();
	afx_msg void OnBriefBtFlashChooseFocus();
	afx_msg void OnMissionflags();
	afx_msg void OnBriefBtJump();
	afx_msg void OnBriefBtNextscreen();
	afx_msg void OnBriefBtPrevscreen();
	afx_msg void OnBriefBtQuit();
	afx_msg void OnBriefBtScrolldown();
	afx_msg void OnBriefBtScrollup();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BRIEFBUTTONEDIT_H__01F120A2_427B_11D2_B359_006008BF0B09__INCLUDED_)
