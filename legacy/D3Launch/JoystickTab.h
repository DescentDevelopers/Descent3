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

 * $Logfile: /DescentIII/Main/D3Launch/JoystickTab.h $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:56:51 $
 * $Author: kevinb $
 *
 * 
 *
 * $Log: not supported by cvs2svn $
 * 
 * 8     5/20/99 12:53p Nate
 * Added GLSetup window, EAX mixer support, and CHFlight+Mouseman Mode
 * options
 * 
 * 7     10/10/98 4:05p Nate
 * Added detection/scanning window messages
 * 
 * 6     10/08/98 6:23p Nate
 * Fixed a few bugs.
 * 
 * 5     9/21/98 5:40p Nate
 * Incorporated the new HTML help system
 * 
 * 4     9/15/98 6:02p Nate
 * Added Force Feedback detection.
 * 
 * 3     9/02/98 6:43p Nate
 * Added JoystickData structure
 * 
 * 2     8/05/98 11:54a Nate
 * Initial Version
 *
 * $NoKeywords: $
 */

#if !defined(AFX_JOYSTICKTAB_H__DB7960A2_20B5_11D2_8CBD_00A0C96ED60D__INCLUDED_)
#define AFX_JOYSTICKTAB_H__DB7960A2_20B5_11D2_8CBD_00A0C96ED60D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// JoystickTab.h : header file
//

#include "MsgDlg.h"

#define JOYSTICK_NONE_ID	99999
#define MAX_NUM_JOYSTICKS	16

struct JoystickData {
	int ID;
	char name[1024];
	bool FF_capable;
};

/////////////////////////////////////////////////////////////////////////////
// CJoystickTab dialog

class CJoystickTab : public CPropertyPage
{
	DECLARE_DYNCREATE(CJoystickTab)

// Construction
public:
	CJoystickTab();
	~CJoystickTab();

	CMsgDlg m_MsgDlg;

// Dialog Data
	//{{AFX_DATA(CJoystickTab)
	enum { IDD = IDD_PROPPAGE_JOYSTICK };
	CButton	m_CHFlightEnabled;
	CButton	m_MousemanEnabled;
	CComboBox	m_joystick_list;
	CButton	m_ff_enabled;
	//}}AFX_DATA

	void DetectSticks();

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CJoystickTab)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CJoystickTab)
	virtual BOOL OnInitDialog();
	afx_msg void OnBtnCalibrate();
	afx_msg void OnBtnDetect();
	afx_msg void OnSelchangeJoystickList();
	afx_msg LRESULT OnCommandHelp(WPARAM wParam, LPARAM lParam);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_JOYSTICKTAB_H__DB7960A2_20B5_11D2_8CBD_00A0C96ED60D__INCLUDED_)
