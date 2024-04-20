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

// WorldObjectsDoorDialog.h : header file
//

#include "gr.h"

/////////////////////////////////////////////////////////////////////////////
// CWorldObjectsDoorDialog dialog

class CWorldObjectsDoorDialog : public CDialog
{
// Construction
public:
	CWorldObjectsDoorDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWorldObjectsDoorDialog)
	enum { IDD = IDD_WORLDOBJECTSDOOR };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWorldObjectsDoorDialog)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation

private:

	void UpdateDialog (void);
	void SaveDoorsOnClose();
	void UpdateDoorView();

	grHardwareSurface m_DoorSurf;

protected:

	// Generated message map functions
	//{{AFX_MSG(CWorldObjectsDoorDialog)
	afx_msg void OnAddDoor();
	afx_msg void OnCheckinDoor();
	afx_msg void OnKillfocusCloseTime();
	afx_msg void OnDeleteDoor();
	afx_msg void OnDoorNext();
	afx_msg void OnKillfocusDoorOpenTime();
	afx_msg void OnDoorPrev();
	afx_msg void OnKillfocusDoorStaysOpen();
	afx_msg void OnDoorsOut();
	afx_msg void OnLoadDoorModel();
	afx_msg void OnLockDoor();
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSelendokDoorPulldown();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnDoorChangeName();
	afx_msg void OnTransparency();
	afx_msg void OnDestroy();
	afx_msg void OnSelendokDoorCloseSound();
	afx_msg void OnSelendokDoorOpenSound();
	afx_msg void OnBrowse();
	afx_msg void OnKillfocusScriptname();
	afx_msg void OnDoorBlastable();
	afx_msg void OnKillfocusDoorHitpointsEdit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
