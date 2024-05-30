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

#if !defined(AFX_BRIEFBITMAPEDIT_H__01F120A1_427B_11D2_B359_006008BF0B09__INCLUDED_)
#define AFX_BRIEFBITMAPEDIT_H__01F120A1_427B_11D2_B359_006008BF0B09__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// BriefBitmapEdit.h : header file
//

#include "telcomefxstructs.h"
#include "TelcomEffects.h"

/////////////////////////////////////////////////////////////////////////////
// CBriefBitmapEdit dialog

class CBriefBitmapEdit : public CDialog
{
// Construction
public:
	CBriefBitmapEdit(int curr_screen = 0,TCBMPDESC* d=NULL,CWnd* pParent = NULL);   // standard constructor
	TCBMPDESC desc;
	int m_Screen;

// Dialog Data
	//{{AFX_DATA(CBriefBitmapEdit)
	enum { IDD = IDD_BRIEF_ADDBITMAP };
	CString	m_sFilename;
	float	m_fSpeed;
	float	m_fStartTime;
	int		m_iX;
	int		m_iY;
	int		m_iEffectType;
	CString	m_sDesc;
	BOOL	m_NoRenderUntilStart;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBriefBitmapEdit)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CBriefBitmapEdit)
	virtual void OnOK();
	afx_msg void OnBriefBChoose();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeBriefBPredef();
	afx_msg void OnMissionflags();
	afx_msg void OnBriefBNorender();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BRIEFBITMAPEDIT_H__01F120A1_427B_11D2_B359_006008BF0B09__INCLUDED_)
