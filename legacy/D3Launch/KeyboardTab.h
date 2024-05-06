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

 * $Logfile: /DescentIII/Main/D3Launch/KeyboardTab.h $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:56:51 $
 * $Author: kevinb $
 *
 * 
 *
 * $Log: not supported by cvs2svn $
 * 
 * 6     5/22/99 1:39a Nate
 * Added Find Fast support
 * 
 * 5     2/24/99 1:46p Nate
 * Added multi-language support
 * 
 * 4     10/13/98 3:03p Nate
 * More fixes and changes.
 * 
 * 3     9/21/98 5:40p Nate
 * Incorporated the new HTML help system
 * 
 * 2     8/17/98 10:35a Nate
 * Added Keyboard Type page to Setup
 *
 * $NoKeywords: $
 */

#if !defined(AFX_KEYBOARDTAB_H__9725ED00_35BA_11D2_8CBD_00A0C96ED60D__INCLUDED_)
#define AFX_KEYBOARDTAB_H__9725ED00_35BA_11D2_8CBD_00A0C96ED60D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// KeyboardTab.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CKeyboardTab dialog

class CKeyboardTab : public CPropertyPage
{
	DECLARE_DYNCREATE(CKeyboardTab)

// Construction
public:
	CKeyboardTab();
	~CKeyboardTab();

	bool AddLangToList(int stringID, int langID);
	bool SelectLangInList(int langID);
	int GetSelectedLangFromList(void);

// Dialog Data
	//{{AFX_DATA(CKeyboardTab)
	enum { IDD = IDD_PROPPAGE_KEYBOARD };
	CButton	m_FindFastCheck;
	CComboBox	m_LanguageCombo;
	CEdit	m_CommandLineEdit;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CKeyboardTab)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CKeyboardTab)
	virtual BOOL OnInitDialog();
	afx_msg LRESULT OnCommandHelp(WPARAM wParam, LPARAM lParam);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KEYBOARDTAB_H__9725ED00_35BA_11D2_8CBD_00A0C96ED60D__INCLUDED_)
