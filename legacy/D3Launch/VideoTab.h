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
 * $Logfile: /DescentIII/Main/D3Launch/VideoTab.h $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:56:51 $
 * $Author: kevinb $
 *
 * 
 *
 * $Log: not supported by cvs2svn $
 * 
 * 10    4/26/99 11:38a Nate
 * Added warning message for opengl or direct3d renderer selection when
 * user has glide as an option.
 * 
 * 9     10/12/98 7:13p Nate
 * Fixed several bugs.
 * 
 * 8     10/10/98 4:05p Nate
 * Added detection/scanning window messages
 * 
 * 7     10/08/98 6:23p Nate
 * Fixed a few bugs.
 * 
 * 6     9/21/98 5:40p Nate
 * Incorporated the new HTML help system
 * 
 * 5     9/02/98 9:48a Nate
 * Fixed hardware accelerator display bug in the Speed Tab
 * 
 * 4     9/01/98 7:15p Nate
 * Major Revision #2
 * 
 * 3     8/31/98 6:44p Nate
 * Major Revision
 * 
 * 2     8/05/98 11:54a Nate
 * Initial Version
 *
 * $NoKeywords: $
 */

#if !defined(AFX_VIDEOTAB_H__78170D82_2215_11D2_8CBD_00A0C96ED60D__INCLUDED_)
#define AFX_VIDEOTAB_H__78170D82_2215_11D2_8CBD_00A0C96ED60D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// VideoTab.h : header file
//

#include "MsgDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CVideoTab dialog

class CVideoTab : public CPropertyPage
{
	DECLARE_DYNCREATE(CVideoTab)

// Construction
public:
	CVideoTab();
	~CVideoTab();

	int GetCurrentCard();
	CMsgDlg m_MsgDlg;

// Dialog Data
	//{{AFX_DATA(CVideoTab)
	enum { IDD = IDD_PROPPAGE_VIDEO };
	CComboBox	m_videocard_list;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CVideoTab)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CVideoTab)
	afx_msg void OnVideocardDetect();
	virtual BOOL OnInitDialog();
	afx_msg LRESULT OnCommandHelp(WPARAM wParam, LPARAM lParam);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnSelchangeVideocardList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

extern CVideoTab * CurrentVideoTab;

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIDEOTAB_H__78170D82_2215_11D2_8CBD_00A0C96ED60D__INCLUDED_)
