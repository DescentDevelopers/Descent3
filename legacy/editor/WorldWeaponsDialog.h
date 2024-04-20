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

// WorldWeaponsDialog.h : header file
//

#include "gr.h"

/////////////////////////////////////////////////////////////////////////////
// CWorldWeaponsDialog dialog

class CWorldWeaponsDialog : public CDialog
{
// Construction
public:
	CWorldWeaponsDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWorldWeaponsDialog)
	enum { IDD = IDD_WORLDWEAPONS };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWorldWeaponsDialog)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWorldWeaponsDialog)
	afx_msg void OnAddWeapon();
	afx_msg void OnDeleteWeapon();
	afx_msg void OnCheckinWeapon();
	afx_msg void OnLockWeapon();
	afx_msg void OnNextWeapon();
	afx_msg void OnPrevWeapon();
	afx_msg void OnLoadWeaponAnim();
	afx_msg void OnSelendokWeaponPulldown();
	afx_msg void OnWeaponsOut();
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnLoadWeaponDischarge();
	afx_msg void OnSelendokFireSoundPulldown();
	afx_msg void OnSelendokWeaponWallSoundPulldown();
	afx_msg void OnKillfocusWeaponFireDelayEdit();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnWeaponHomingCheck();
	afx_msg void OnWeaponCollideWithSiblingCheck();
	afx_msg void OnWeaponUseParentVelocityCheck();
	afx_msg void OnKillfocusWeaponThrustTimeEdit();
	afx_msg void OnKillfocusWeaponBlobSizeEdit();
	afx_msg void OnKillfocusWeaponImpactSizeEdit();
	afx_msg void OnKillfocusWeaponLifeTimeEdit();
	afx_msg void OnKillfocusWeaponDamageEdit();
	afx_msg void OnSmokeCheck();
	afx_msg void OnEditPhysics();
	afx_msg void OnChangeName();
	afx_msg void OnWeaponLightInfo();
	afx_msg void OnEnergyRadio();
	afx_msg void OnMatterRadio();
	afx_msg void OnElectricalCheck();
	afx_msg void OnDefaultSize();
	afx_msg void OnKillfocusWeaponImpactTimeEdit();
	afx_msg void OnSelendokSmokePulldown();
	afx_msg void OnSelendokExplodePulldown();
	afx_msg void OnSprayCheck();
	afx_msg void OnInstantCheck();
	afx_msg void OnInvisible();
	afx_msg void OnRing();
	afx_msg void OnExplodeRing();
	afx_msg void OnPlanarBlast();
	afx_msg void OnSaturateCheck();
	afx_msg void OnKillfocusWeaponAlphaEdit();
	afx_msg void OnPlanarCheck();
	afx_msg void OnKillfocusExplodeSizeEdit();
	afx_msg void OnKillfocusExplodeTimeEdit();
	afx_msg void OnSelendokWeaponSpawnPulldown();
	afx_msg void OnKillfocusWeaponSpawnEdit();
	afx_msg void OnSpawnsOnImpact();
	afx_msg void OnSpawnsOnTimeout();
	afx_msg void OnOverride();
	afx_msg void OnKillfocusParticleCountEdit();
	afx_msg void OnKillfocusParticleLifeEdit();
	afx_msg void OnKillfocusParticleSizeEdit();
	afx_msg void OnSelendokParticlePulldown();
	afx_msg void OnWeaponCopyButton();
	afx_msg void OnWeaponPasteButton();
	afx_msg void OnExpandingCheck();
	afx_msg void OnMuzzleFlash();
	afx_msg void OnMicrowave();
	afx_msg void OnNapalm();
	afx_msg void OnSelendokWeaponBounceSoundCombo();
	afx_msg void OnKillfocusWeaponImpactTimeEdit2();
	afx_msg void OnKillfocusWeaponImpactDamageEdit();
	afx_msg void OnKillfocusWeaponImpactForceEdit();
	afx_msg void OnSelendokScorchPulldown();
	afx_msg void OnKillfocusWeaponScorchSizeEdit();
	afx_msg void OnSelendokSmallimgPulldown();
	afx_msg void OnSelendokSpawn2Pulldown();
	afx_msg void OnKillfocusAlternateChanceEdit();
	afx_msg void OnReverseSmokeCheck();
	afx_msg void OnGravityFieldCheck();
	afx_msg void OnKillfocusGravitySize();
	afx_msg void OnKillfocusGravityTime();
	afx_msg void OnCountermeasureCheck();
	afx_msg void OnSpawnsRobotCheck();
	afx_msg void OnSelendokSpawnRobotPulldown();
	afx_msg void OnKillfocusTerrianDamageSize();
	afx_msg void OnKillfocusTerrainDamageDepth();
	afx_msg void OnFreezeCheck();
	afx_msg void OnSilentHomingCheck();
	afx_msg void OnHomedSplitCheck();
	afx_msg void OnKillfocusHomingFovText();
	afx_msg void OnNoRotateCheck();
	afx_msg void OnCustomSizeCheck();
	afx_msg void OnKillfocusCustomSizeEdit();
	afx_msg void OnTimeoutWallCheck();
	afx_msg void OnEnableCamera();
	afx_msg void OnKillfocusWeaponGenericDamageEdit();
	afx_msg void OnKillfocusWeaponImpactGenericDamageEdit();
	afx_msg void OnPlanarSmokeCheck();
	afx_msg void OnKillfocusWeaponRecoilForceEdit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	private:

	void UpdateDialog (void);
	void SaveWeaponsOnClose();
	void UpdateWeaponView();

	grHardwareSurface m_WeaponSurf;

};
