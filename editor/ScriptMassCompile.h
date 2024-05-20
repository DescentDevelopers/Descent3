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

* $Logfile: /DescentIII/Main/editor/ScriptMassCompile.h $
* $Revision: 1.1.1.1 $
* $Date: 2003-08-26 03:57:39 $
* $Author: kevinb $
*
* Header file for mass script compile dialog
*
* $Log: not supported by cvs2svn $
 * 
 * 8     4/03/99 1:36a Jeff
 * added no-check out option for mass compiler
 * 
 * 7     2/21/99 8:05p Jeff
 * better handling of out-of-sync scripts
 * 
 * 6     1/11/99 3:34p Jeff
 * added checks for when going editor->game to see if scripts are out of
 * date, if so give the option of breaking out.  Add some options to mass
 * script compiler, along with a toolbar shortcut.
 * 
 * 5     12/30/98 4:39p Jeff
 * selectable to what scripts to display (checked out, non-checked out or
 * both)
 * 
 * 4     12/20/98 9:57p Jeff
 * finished mass compile dialog
 * 
 * 3     12/18/98 12:36p Jeff
 * created a function to determine a script's type and hooked it in
 * 
 * 2     12/18/98 12:11p Jeff
 * added mass script compile dialog
*
* $NoKeywords: $
*/



#if !defined(AFX_SCRIPTMASSCOMPILE_H__7A381E00_966F_11D2_AB2B_006008BF0B09__INCLUDED_)
#define AFX_SCRIPTMASSCOMPILE_H__7A381E00_966F_11D2_AB2B_006008BF0B09__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ScriptMassCompile.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CScriptMassCompile dialog
#include "pstypes.h"

uint8_t DetermineScriptType(char *filename);

class CScriptMassCompile : public CDialog
{
// Construction
public:
	CScriptMassCompile(CWnd* pParent = NULL);   // standard constructor

	void SetStepText(int step,char *format,...);
	bool Step1(char *filename);
	bool Step2(char *filename);
	bool Step3(char *filename,bool islevel);
	bool Step4(char *filename);
	void BuildList(void);

// Dialog Data
	//{{AFX_DATA(CScriptMassCompile)
	enum { IDD = IDD_COMPILEALL };
	CCheckListBox	m_ScriptList;
	BOOL	m_AutoCheckIn;
	int		m_DisplayType;
	BOOL	m_NoCheckout;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScriptMassCompile)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CScriptMassCompile)
	afx_msg void OnBuild();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnLocked();
	afx_msg void OnNotlocked();
	afx_msg void OnBoth();
	afx_msg void OnSelectall();
	afx_msg void OnOodscripts();
	afx_msg void OnOosscripts();
	afx_msg void OnNocheckout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCRIPTMASSCOMPILE_H__7A381E00_966F_11D2_AB2B_006008BF0B09__INCLUDED_)
