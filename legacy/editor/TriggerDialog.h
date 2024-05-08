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

 * $Logfile: /DescentIII/Main/editor/TriggerDialog.h $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:57:39 $
 * $Author: kevinb $
 *
 * Trigger keypad dialog
 *
 * $Log: not supported by cvs2svn $
 * 
 * 8     4/03/99 7:55p Matt
 * Use new keypad update system instead of timer to update.
 * 
 * 7     3/23/99 5:52p Matt
 * Added clutter activators for triggers
 * 
 * 6     1/14/99 11:06a Matt
 * Added names to triggers
 * 
 * 5     1/08/99 2:56p Samir
 * Ripped out OSIRIS1.
 * 
 * 4     9/15/97 11:38a Matt
 * Added better trigger selection functions
 * 
 * 3     9/12/97 3:59p Matt
 * Lots of changes to trigger dialog
 * 
 * 2     9/08/97 10:01a Matt
 * Ripped out old trigger code
 * 
 * $NoKeywords: $
 */


#include "KeypadDialog.h"
#include "trigger.h"

/////////////////////////////////////////////////////////////////////////////
// CTriggerDialog dialog

class CTriggerDialog : public CKeypadDialog
{
// Construction
public:
	CTriggerDialog(CWnd* pParent = NULL);   // standard constructor
	void UpdateKeypad(int mask);

// Dialog Data
	//{{AFX_DATA(CTriggerDialog)
	enum { IDD = IDD_TRIGGERKEYPAD };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTriggerDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTriggerDialog)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnTrigAddToCurface();
	afx_msg void OnTrigGoto();
	afx_msg void OnTrigDelete();
	afx_msg void OnTrigAddToCurportal();
	afx_msg void OnTrigAddFloating();
	afx_msg void OnTrigRenderFloating();
	afx_msg void OnTrigActivPlayer();
	afx_msg void OnTrigActivPlayerWeapons();
	afx_msg void OnTrigActivRobotWeapons();
	afx_msg void OnTrigActivRobots();
	afx_msg void OnTrigFloatNarrower();
	afx_msg void OnTrigFloatShorter();
	afx_msg void OnTrigFloatTaller();
	afx_msg void OnTrigFloatWider();
	afx_msg void OnTrigOneShot();
	afx_msg void OnTrigNextPortal();
	afx_msg void OnTrigNextInMine();
	afx_msg void OnTrigNextInRoom();
	afx_msg void OnTrigPrevInMine();
	afx_msg void OnTrigPrevInRoom();
	afx_msg void OnTrigEditName();
	afx_msg void OnTrigEditScript();
	afx_msg void OnTrigActivClutter();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	virtual void OnOK();					// prevent auto closing of dialog when in edit control

private:
	void UpdateDialog();
};
