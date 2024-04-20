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

// WorldObjectsPowerupDialog.h : header file
//

#include "gr.h"

/////////////////////////////////////////////////////////////////////////////
// CWorldObjectsPowerupDialog dialog

class CWorldObjectsPowerupDialog : public CDialog
{
// Construction
public:
	CWorldObjectsPowerupDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWorldObjectsPowerupDialog)
	enum { IDD = IDD_WORLDOBJECTSPOWERUP };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWorldObjectsPowerupDialog)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

private:
	void UpdateDialog();
	void SavePowerupsOnClose();
	void UpdatePowerupView();

	grHardwareSurface m_PowerupSurf;

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWorldObjectsPowerupDialog)
	afx_msg void OnAddNewPowerup();
	afx_msg void OnCheckin();
	afx_msg void OnDeletePowerup();
	afx_msg void OnLockPowerup();
	afx_msg void OnNextPowerup();
	afx_msg void OnPrev();
	afx_msg void OnPowerupsOut();
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnKillfocusPowerNameEdit();
	afx_msg void OnKillfocusPowerupSizeEdit();
	afx_msg void OnSelendokPowerPulldown();
	afx_msg void OnLoadImage();
	afx_msg void OnGravityCheck();
	afx_msg void OnMagnetismCheck();
	afx_msg void OnWindCheck();
	afx_msg void OnKillfocusPowMassEdit();
	afx_msg void OnKillfocusPowDragEdit();
	afx_msg void OnSelendokPowerupAmbientSoundList();
	afx_msg void OnSelendokPowerupPickupSoundList();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
