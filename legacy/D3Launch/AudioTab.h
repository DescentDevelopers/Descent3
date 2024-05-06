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

 * $Logfile: /DescentIII/Main/D3Launch/AudioTab.h $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:56:51 $
 * $Author: kevinb $
 *
 * 
 *
 * $Log: not supported by cvs2svn $
 * 
 * 9     10/21/98 4:51p Nate
 * More fixes.
 * 
 * 8     10/15/98 11:31a Nate
 * Added Launcher Sound toggling
 * 
 * 7     10/14/98 5:52p Nate
 * More fixes
 * 
 * 6     10/10/98 4:05p Nate
 * Added detection/scanning window messages
 * 
 * 5     9/21/98 5:40p Nate
 * Incorporated the new HTML help system
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

#if !defined(AFX_AUDIOTAB_H__FAB3D182_22D6_11D2_8CBD_00A0C96ED60D__INCLUDED_)
#define AFX_AUDIOTAB_H__FAB3D182_22D6_11D2_8CBD_00A0C96ED60D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// AudioTab.h : header file
//

#include "MsgDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CAudioTab dialog

class CAudioTab : public CPropertyPage
{
	DECLARE_DYNCREATE(CAudioTab)

// Construction
public:
	CAudioTab();
	~CAudioTab();
	void EnableMixerSettings(UINT mixers);
	int GetMixerButton(void);
	void SetMixerButton(int mixer_id, UINT mixer_flags);
	UINT DetermineDefaultMixer(int card_index);
	int DetermineBestCard(void);

	CMsgDlg m_MsgDlg;

// Dialog Data
	//{{AFX_DATA(CAudioTab)
	enum { IDD = IDD_PROPPAGE_AUDIO };
	CButton	m_LauncherSndCheck;
	CComboBox	m_audio_list;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CAudioTab)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CAudioTab)
	afx_msg void OnBtnAudioDetect();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeAudioList();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg LRESULT OnCommandHelp(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};


//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUDIOTAB_H__FAB3D182_22D6_11D2_8CBD_00A0C96ED60D__INCLUDED_)
