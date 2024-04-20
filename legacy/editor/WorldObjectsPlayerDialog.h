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

// WorldObjectsPlayerDialog.h : header file
//

#include "gr.h"

/////////////////////////////////////////////////////////////////////////////
// CWorldObjectsPlayerDialog dialog

class CWorldObjectsPlayerDialog : public CDialog
{
// Construction
public:
	CWorldObjectsPlayerDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWorldObjectsPlayerDialog)
	enum { IDD = IDD_WORLDOBJECTSPLAYER };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWorldObjectsPlayerDialog)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWorldObjectsPlayerDialog)
	afx_msg void OnAddPship();
	afx_msg void OnPshipCheckin();
	afx_msg void OnPshipDelete();
	afx_msg void OnPshipLoadModel();
	afx_msg void OnPshipLock();
	afx_msg void OnKillfocusPshipNameEdit();
	afx_msg void OnPshipNext();
	afx_msg void OnPshipPrev();
	afx_msg void OnPshipsOut();
	afx_msg void OnSelendokPshipPulldown();
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnPshipEditPhysics();
	afx_msg void OnPshipDyingModel();
	afx_msg void OnNullDying();
	afx_msg void OnEditWeapons();
	afx_msg void OnPshipCockpit();
	afx_msg void OnKillfocusPshipCockpitEdit();
	afx_msg void OnDestroy();
	afx_msg void OnKillfocusShipArmorEdit();
	afx_msg void OnDefaultallow();
	afx_msg void OnHiresRadio();
	afx_msg void OnMedresRadio();
	afx_msg void OnLoresRadio();
	afx_msg void OnNolod();
	afx_msg void OnKillfocusLodDistanceEdit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	void UpdateDialog (void);
	void SaveShipsOnClose();
	void UpdateShipView();
	int m_lod;

	grHardwareSurface m_ShipSurf;

};
