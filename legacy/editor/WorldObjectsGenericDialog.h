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

#if !defined(AFX_WORLDOBJECTSGENERICDIALOG_H__631E2882_0E6D_11D1_86CC_004033312F7C__INCLUDED_)
#define AFX_WORLDOBJECTSGENERICDIALOG_H__631E2882_0E6D_11D1_86CC_004033312F7C__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// WorldObjectsGenericDialog.h : header file
//																  

#include "gr.h"
#include "SoundListCombo.h"

/////////////////////////////////////////////////////////////////////////////
// CWorldObjectsGenericDialog dialog

class CWorldObjectsGenericDialog : public CDialog
{
// Construction
public:
	CWorldObjectsGenericDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWorldObjectsGenericDialog)
	enum { IDD = IDD_WORLDOBJECTSGENERIC };
	CSoundListCombo	m_ambient_sound_combo;
	CSoundListCombo	m_explosion_sound_combo;
	CString	m_type_name;
	int		m_current;
	CString	m_sInvenDescription;
	CString	m_sInvenIconName;
	//}}AFX_DATA

	int m_type;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWorldObjectsGenericDialog)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

private:

	void UpdateDialog (void);
	void SaveGenericsOnClose();
	void EnableDisableAll(bool flag);
	void SaveObjectsOnClose(void);
	void UpdateObjectView(void);
	bool IsLocked(int n);
	int  CountLockedItems(void);
	int GetTextureUsageForPolymodel (int);

	grHardwareSurface m_ObjectSurf;
	int m_locked_count;
	int m_lod;


// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWorldObjectsGenericDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnGenericEditAI();
	afx_msg void OnGenericEditPhysics();
	afx_msg void OnGenericUsesAI();
	afx_msg void OnGenericUsesPhysics();
	afx_msg void OnGenericAddNew();
	afx_msg void OnGenericCheckedOut();
	afx_msg void OnGenericCheckIn();
	afx_msg void OnGenericDefineAnimStates();
	afx_msg void OnGenericDelete();
	afx_msg void OnGenericLock();
	afx_msg void OnGenericNext();
	afx_msg void OnGenericPrev();
	afx_msg void OnGenericChangeModel();
	afx_msg void OnGenericChangeName();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSelendokGenericNamePulldown();
	afx_msg void OnKillfocusGenericSizeEdit();
	afx_msg void OnGenericCopy();
	afx_msg void OnGenericPaste();
	afx_msg void OnKillfocusGenericScriptIDEdit();
	afx_msg void OnGenericUndoLock();
	afx_msg void OnGenericDestroyable();
	afx_msg void OnKillfocusGenericHitpointEdit();
	afx_msg void OnDestroy();
	afx_msg void OnGenericWeaponInfoButton();
	afx_msg void OnGenericLightButton();
	afx_msg void OnDefaultRadiusButton();
	afx_msg void OnSelScript();
	afx_msg void OnGenericConcussiveCheck();
	afx_msg void OnKillfocusGenericImpactDamageEdit();
	afx_msg void OnKillfocusGenericImpactSizeEdit();
	afx_msg void OnNolod();
	afx_msg void OnHiresRadio();
	afx_msg void OnMedresRadio();
	afx_msg void OnLoresRadio();
	afx_msg void OnKillfocusGenericImpactTimeEdit();
	afx_msg void OnSelendokGenericExplosionSoundCombo();
	afx_msg void OnSelendokGenericAmbientSoundCombo();
	afx_msg void OnKillfocusGenericInvenDescription();
	afx_msg void OnKillfocusGenericInvenIconname();
	afx_msg void OnOverride();
	afx_msg void OnGenericInvenSelectable();
	afx_msg void OnGenericInvenNonuseable();
	afx_msg void OnKillfocusLodDistanceEdit();
	afx_msg void OnInvtypeGame();
	afx_msg void OnInvtypeMission();
	afx_msg void OnInvenNoremove();
	afx_msg void OnInvenViswhenused();
	afx_msg void OnSelendokDeathPowerup1Pulldown();
	afx_msg void OnKillfocusDeathPowerup1NumEdit();
	afx_msg void OnKillfocusDeathPowerup1PercentEdit();
	afx_msg void OnKillfocusDeathPowerup2NumEdit();
	afx_msg void OnKillfocusDeathPowerup2PercentEdit();
	afx_msg void OnSelendokDeathPowerup2Pulldown();
	afx_msg void OnDeathPowerupUse2IfHave1Check();
	afx_msg void OnGenericDeathSpew2IfZero1();
	afx_msg void OnKillfocusScriptname();
	afx_msg void OnCompilemodule();
	afx_msg void OnKillfocusScriptOverride();
	afx_msg void OnGenericAiScriptedDeath();
	afx_msg void OnKillfocusRespawnScalarEdit();
	afx_msg void OnGenericDeaths();
	afx_msg void OnKillfocusGenericScoreEdit();
	afx_msg void OnKillfocusGenericAmmoEdit();
	afx_msg void OnObjCeilingCheck();
	afx_msg void OnObjectFlyThroughRenderedPortals();
	afx_msg void OnNscButton();
	afx_msg void OnDsmpbdCheck();
	afx_msg void OnGenericAmbient();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WORLDOBJECTSGENERICDIALOG_H__631E2882_0E6D_11D1_86CC_004033312F7C__INCLUDED_)
