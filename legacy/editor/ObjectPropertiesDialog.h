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

#if !defined(AFX_OBJECTPROPERTIESDIALOG_H__B4ABFD41_F15C_11D0_8901_0020A90CB6F2__INCLUDED_)
#define AFX_OBJECTPROPERTIESDIALOG_H__B4ABFD41_F15C_11D0_8901_0020A90CB6F2__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ObjectPropertiesDialog.h : header file
//

#ifndef _OBJ_PROP_DLG_H_
#define _OBJ_PROP_DLG_H_

#include "objscript.h"


class CPropertyAIDlg;
class CPropertyPhysicsDlg;

#define TAB_PHYSICS	0
#define TAB_AI		1

/////////////////////////////////////////////////////////////////////////////
// CObjectPropertiesDialog dialog

class CObjectPropertiesDialog : public CDialog
{
// Construction
public:
	CObjectPropertiesDialog(CWnd* pParent = NULL);   // standard constructor

	BOOL	m_bBounce;
	float	m_fDrag;
	float	m_fFullRotThrust;
	float	m_fFullThrust;
	BOOL	m_bGravity;
	BOOL	m_bLevelling;
	BOOL	m_bMagnetism;
	float	m_fMass;
	float	m_fMaxTurnrollrate;
	int		m_iNumBounces;
	BOOL	m_bPersistent;
	float	m_fRotDrag;
	float	m_fRotThrustX;
	float	m_fRotThrustY;
	float	m_fRotThrustZ;
	float	m_fRotVelX;
	float	m_fRotVelY;
	float	m_fRotVelZ;
	BOOL	m_bSticks;
	float	m_fThrustX;
	float	m_fThrustY;
	float	m_fThrustZ;
	BOOL	m_bTurnroll;
	float	m_fTurnrollRatio;
	short	m_sTurnrollAngle;
	BOOL	m_bUsesThrust;
	float	m_fVelocityX;
	float	m_fVelocityY;
	float	m_fVelocityZ;
	BOOL	m_bWiggle;
	float	m_fWiggleAmplitude;
	float	m_fWigglesPerSecond;
	BOOL	m_bWind;
	BOOL	m_bUnlimitedBounce;

	CString m_ScriptName;
	int m_NumParms;

// Dialog Data
	//{{AFX_DATA(CObjectPropertiesDialog)
	enum { IDD = IDD_OBJECTPROPERTIES };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CObjectPropertiesDialog)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
	private:
		int m_current_tab;
		CWnd *m_Parent;							// parent window to this dialog
		BOOL m_Initializing;					// Are we done initializing?
		float m_keypad_w,m_keypad_h,m_keypad_x,m_keypad_y;
	public:
		void RunKeypadFunction(int code);
		void Deactivate();								// deactivates current tab
		void Activate();								// activates current tab.
		CPropertyAIDlg *m_PropAIDlg;
		CPropertyPhysicsDlg *m_PropPhysicsDlg;
		CKeypadDialog *m_KeypadTabDialog;

	private:
		void DoKeypadTabNotify(NMHDR *);		// take care of tab control notifications

		
	protected:

	// Generated message map functions
	//{{AFX_MSG(CObjectPropertiesDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMove(int x, int y);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OBJECTPROPERTIESDIALOG_H__B4ABFD41_F15C_11D0_8901_0020A90CB6F2__INCLUDED_)



