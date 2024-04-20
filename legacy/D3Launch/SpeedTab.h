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
 * $Logfile: /DescentIII/Main/D3Launch/SpeedTab.h $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:56:51 $
 * $Author: kevinb $
 *
 * 
 *
 * $Log: not supported by cvs2svn $
 * 
 * 6     4/21/99 7:17p Nate
 * Added "-nolightmaps" switch for LOW_DETAIL setting
 * 
 * 5     9/28/98 10:49a Nate
 * Added flag for when list box selection changes (if flag is set when ok
 * button is pressed, registry settings are changed to the selected
 * default settings)
 * 
 * 4     9/25/98 6:59p Nate
 * Added code to write out detail level registry values.
 * 
 * 3     9/21/98 5:40p Nate
 * Incorporated the new HTML help system
 * 
 * 2     8/05/98 11:54a Nate
 * Initial Version
 *
 * $NoKeywords: $
 */

#if !defined(AFX_SPEEDTAB_H__FAB3D183_22D6_11D2_8CBD_00A0C96ED60D__INCLUDED_)
#define AFX_SPEEDTAB_H__FAB3D183_22D6_11D2_8CBD_00A0C96ED60D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// SpeedTab.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSpeedTab dialog

#define LOW_DETAIL			0
#define MEDIUM_DETAIL		1
#define HIGH_DETAIL			2
#define VERY_HIGH_DETAIL	3
#define CUSTOM_DETAIL		4

class CSpeedTab : public CPropertyPage
{
	DECLARE_DYNCREATE(CSpeedTab)

// Construction
public:
	CSpeedTab();
	~CSpeedTab();

	bool m_WriteDetailValues;  

// Dialog Data
	//{{AFX_DATA(CSpeedTab)
	enum { IDD = IDD_PROPPAGE_SPEED };
	CComboBox	m_speed_list;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSpeedTab)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSpeedTab)
	virtual BOOL OnInitDialog();
	afx_msg void OnBtnSpeedDetect();
	afx_msg LRESULT OnCommandHelp(WPARAM wParam, LPARAM lParam);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnSelchangeSpeedList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SPEEDTAB_H__FAB3D183_22D6_11D2_8CBD_00A0C96ED60D__INCLUDED_)
