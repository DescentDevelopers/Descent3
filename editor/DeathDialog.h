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

#if !defined(AFX_DEATHDIALOG_H__ADECD300_ABA8_11D2_A13A_0060089A83BE__INCLUDED_)
#define AFX_DEATHDIALOG_H__ADECD300_ABA8_11D2_A13A_0060089A83BE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DeathDialog.h : header file
//

#include "DeathInfo.h"
#include "objinfo.h"

/////////////////////////////////////////////////////////////////////////////
// CDeathDialog dialog

class CDeathDialog : public CDialog
{
// Construction
public:
	CDeathDialog(death_info *death_info,CWnd* pParent = NULL);   // standard constructor

	death_info	*m_death_info_ptr;
	int			m_flags;


// Dialog Data
	//{{AFX_DATA(CDeathDialog)
	enum { IDD = IDD_DEATH_DIALOG };
	int		m_explosion_size_radio;
	float	m_delay_max;
	float	m_delay_min;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDeathDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
private:
	void UpdateDialog();

protected:


	// Generated message map functions
	//{{AFX_MSG(CDeathDialog)
	afx_msg void OnDeathBreaksApart();
	afx_msg void OnDeathCommonAnimal();
	afx_msg void OnDeathCommonDescent();
	afx_msg void OnDeathCommonQuickExplosion();
	afx_msg void OnDeathCommonR2D2();
	afx_msg void OnDeathCommonShootUp();
	afx_msg void OnDeathContactBreaksApart();
	afx_msg void OnDeathContactExplodes();
	afx_msg void OnDeathContactRemains();
	afx_msg void OnDeathCopy();
	afx_msg void OnDeathDebrisContactExplodes();
	afx_msg void OnDeathDebrisContactRemains();
	afx_msg void OnDeathDelayFireballs();
	afx_msg void OnDeathDelayFlying();
	afx_msg void OnDeathDelayLosesAntigrav();
	afx_msg void OnChangeDeathDelayMax();
	afx_msg void OnChangeDeathDelayMin();
	afx_msg void OnDeathDelaySparks();
	afx_msg void OnDeathExplodes();
	afx_msg void OnDeathExplosionLarge();
	afx_msg void OnDeathExplosionMedium();
	afx_msg void OnDeathExplosionSmall();
	afx_msg void OnDeathPaste();
	afx_msg void OnDeathBlastRing();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnDeathContactBlastRing();
	afx_msg void OnDeathContactFireball();
	afx_msg void OnDeathDebrisContactBlastRing();
	afx_msg void OnDeathDebrisContactFireball();
	afx_msg void OnDeathDebrisSmokes();
	afx_msg void OnDeathDelayFromAnim();
	afx_msg void OnDeathDelaySmoke();
	afx_msg void OnDeathFireball();
	afx_msg void OnDeathLosesAntigrav();
	afx_msg void OnDeathRemains();
	afx_msg void OnDeathDelayFadeAway();
	afx_msg void OnDeathFadesAway();
	afx_msg void OnDeathDelaySound();
	afx_msg void OnDeathDelayNoTumble();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DEATHDIALOG_H__ADECD300_ABA8_11D2_A13A_0060089A83BE__INCLUDED_)
