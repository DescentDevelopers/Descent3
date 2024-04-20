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

#if !defined(AFX_TERRAINSOUNDDIALOG_H__699A56A0_C8F9_11D2_A13A_0060089A83BE__INCLUDED_)
#define AFX_TERRAINSOUNDDIALOG_H__699A56A0_C8F9_11D2_A13A_0060089A83BE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TerrainSoundDialog.h : header file
//

#include "game.h"
#include "SoundListCombo.h"

/////////////////////////////////////////////////////////////////////////////
// CTerrainSoundDialog dialog

class CTerrainSoundDialog : public CDialog
{
// Construction
public:
	CTerrainSoundDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTerrainSoundDialog)
	enum { IDD = IDD_TERRAIN_SOUND_DIALOG };
	CSoundListCombo	m_sound_combo;
	int		m_high_alt;
	int		m_low_alt;
	float	m_high_volume;
	float	m_low_volume;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTerrainSoundDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
private:
	void UpdateDialog();
	void EnableDisableAll(int flag);
	void CopyToControls();
	void CopyFromControls();

	int m_current;
	terrain_sound_band m_bands[NUM_TERRAIN_SOUND_BANDS];


protected:

	// Generated message map functions
	//{{AFX_MSG(CTerrainSoundDialog)
	afx_msg void OnTerrainSoundNext();
	afx_msg void OnTerrainSoundPrev();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeTerrainSoundCombo();
	afx_msg void OnKillfocusTerrainSoundHighAlt();
	afx_msg void OnKillfocusTerrainSoundHighVolume();
	afx_msg void OnKillfocusTerrainSoundLowAlt();
	afx_msg void OnKillfocusTerrainSoundLowVolume();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TERRAINSOUNDDIALOG_H__699A56A0_C8F9_11D2_A13A_0060089A83BE__INCLUDED_)
