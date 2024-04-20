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

// AnimStatesDialog.h : header file
//

#include "objinfo.h"

/////////////////////////////////////////////////////////////////////////////
// CAnimStatesDialog dialog

class CAnimStatesDialog : public CDialog
{
// Construction
public:
	CAnimStatesDialog(int render_handle,anim_elem *anim_info,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAnimStatesDialog)
	enum { IDD = IDD_ANIMSTATES };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	//render handle of the object we're editing
	int	m_render_handle;
	bool m_setup_sound_dialogs;

	//Local copy of the anim states
	anim_elem 	m_anim[NUM_MOVEMENT_CLASSES];	// which anim states are active
	
	//Copy data from CAnimStatesDialog class to anim struct
	void GetAnimData(anim_elem *anim_info);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAnimStatesDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
private:
	void UpdateDialog();
	void UpdateAnimStateView();
	void KillFocusFroms(int);
	void KillFocusTos(int);
	void KillFocusSPC(int num);
	void KillFocusAnimSoundName(int num);

	int m_current_state;
	int m_current_anim;
	int m_SpinFrame;

protected:

	// Generated message map functions
	//{{AFX_MSG(CAnimStatesDialog)
	afx_msg void OnAlertCheck();
	afx_msg void OnDeathCheck();
	afx_msg void OnFlinchCheck();
	afx_msg void OnGotoFlyingCheck();
	afx_msg void OnGotoJumpingCheck();
	afx_msg void OnGotoRollingCheck();
	afx_msg void OnGotoStandingCheck();
	afx_msg void OnGotoWalkingCheck();
	afx_msg void OnIdleCheck();
	afx_msg void OnMelee1Check();
	afx_msg void OnMelee1RecoilCheck();
	afx_msg void OnMelee2Check();
	afx_msg void OnMelee2RecoilCheck();
	afx_msg void OnMissile1Check();
	afx_msg void OnMissile1RecoilCheck();
	afx_msg void OnMissile2Check();
	afx_msg void OnMissile2RecoilCheck();
	afx_msg void OnQuirkCheck();
	afx_msg void OnSpecial1Check();
	afx_msg void OnSpecial2Check();
	afx_msg void OnSpecial3Check();
	afx_msg void OnSpecial4Check();
	afx_msg void OnSpecial5Check();
	afx_msg void OnTauntCheck();
	afx_msg void OnSelendokCurrentAnimationPulldown();
	afx_msg void OnSelendokCurrentStatePulldown();
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnKillfocusAlertFromEdit();
	afx_msg void OnKillfocusAlertToEdit();
	afx_msg void OnKillfocusDeathFromEdit();
	afx_msg void OnKillfocusDeathToEdit();
	afx_msg void OnKillfocusFlinchFromEdit();
	afx_msg void OnKillfocusFlinchToEdit();
	afx_msg void OnKillfocusFlyingFromEdit();
	afx_msg void OnKillfocusFlyingToEdit();
	afx_msg void OnKillfocusIdleFromEdit();
	afx_msg void OnKillfocusIdleToEdit();
	afx_msg void OnKillfocusJumpingFromEdit();
	afx_msg void OnKillfocusJumpingToEdit();
	afx_msg void OnKillfocusMelee1FromEdit();
	afx_msg void OnKillfocusMelee1ToEdit();
	afx_msg void OnKillfocusMelee2FromEdit();
	afx_msg void OnKillfocusMissile1FromEdit();
	afx_msg void OnKillfocusMissile1ToEdit();
	afx_msg void OnKillfocusMissile2FromEdit();
	afx_msg void OnKillfocusMissile2ToEdit();
	afx_msg void OnKillfocusMrecoil1ToEdit();
	afx_msg void OnKillfocusMrecoil1FromEdit();
	afx_msg void OnKillfocusMrecoil2FromEdit();
	afx_msg void OnKillfocusMrecoil2ToEdit();
	afx_msg void OnKillfocusQuirkFromEdit();
	afx_msg void OnKillfocusQuirkToEdit();
	afx_msg void OnKillfocusRecoil1FromEdit();
	afx_msg void OnKillfocusRecoil1ToEdit();
	afx_msg void OnKillfocusRecoil2FromEdit();
	afx_msg void OnKillfocusRecoil2ToEdit();
	afx_msg void OnKillfocusRollingFromEdit();
	afx_msg void OnKillfocusRollingToEdit();
	afx_msg void OnKillfocusSpecial1FromEdit();
	afx_msg void OnKillfocusSpecial1ToEdit();
	afx_msg void OnKillfocusSpecial2FromEdit();
	afx_msg void OnKillfocusSpecial2ToEdit();
	afx_msg void OnKillfocusSpecial3FromEdit();
	afx_msg void OnKillfocusSpecial3ToEdit();
	afx_msg void OnKillfocusSpecial4FromEdit();
	afx_msg void OnKillfocusSpecial4ToEdit();
	afx_msg void OnKillfocusSpecial5FromEdit();
	afx_msg void OnKillfocusSpecial5ToEdit();
	afx_msg void OnKillfocusStandingFromEdit();
	afx_msg void OnKillfocusStandingToEdit();
	afx_msg void OnKillfocusTauntFromEdit();
	afx_msg void OnKillfocusTauntToEdit();
	afx_msg void OnKillfocusWalkingFromEdit();
	afx_msg void OnKillfocusWalkingToEdit();
	afx_msg void OnKillfocusMelee2ToEdit();
	afx_msg void OnResetAnimZero();
	afx_msg void OnRotLeft();
	afx_msg void OnRotRight();
	afx_msg void OnKillfocusSecPerCycleEdit1();
	afx_msg void OnKillfocusSecPerCycleEdit2();
	afx_msg void OnKillfocusSecPerCycleEdit3();
	afx_msg void OnKillfocusSecPerCycleEdit4();
	afx_msg void OnKillfocusSecPerCycleEdit5();
	afx_msg void OnKillfocusSecPerCycleEdit6();
	afx_msg void OnKillfocusSecPerCycleEdit7();
	afx_msg void OnKillfocusSecPerCycleEdit8();
	afx_msg void OnKillfocusSecPerCycleEdit9();
	afx_msg void OnKillfocusSecPerCycleEdit10();
	afx_msg void OnKillfocusSecPerCycleEdit11();
	afx_msg void OnKillfocusSecPerCycleEdit12();
	afx_msg void OnKillfocusSecPerCycleEdit13();
	afx_msg void OnKillfocusSecPerCycleEdit14();
	afx_msg void OnKillfocusSecPerCycleEdit15();
	afx_msg void OnKillfocusSecPerCycleEdit16();
	afx_msg void OnKillfocusSecPerCycleEdit17();
	afx_msg void OnKillfocusSecPerCycleEdit18();
	afx_msg void OnKillfocusSecPerCycleEdit19();
	afx_msg void OnKillfocusSecPerCycleEdit20();
	afx_msg void OnKillfocusSecPerCycleEdit21();
	afx_msg void OnKillfocusSecPerCycleEdit22();
	afx_msg void OnKillfocusSecPerCycleEdit23();
	afx_msg void OnKillfocusSecPerCycleEdit24();
	afx_msg void OnSelendokAnimSound0Pulldown();
	afx_msg void OnSelendokAnimSound1Pulldown();
	afx_msg void OnSelendokAnimSound2Pulldown();
	afx_msg void OnSelendokAnimSound10Pulldown();
	afx_msg void OnSelendokAnimSound11Pulldown();
	afx_msg void OnSelendokAnimSound12Pulldown();
	afx_msg void OnSelendokAnimSound13Pulldown();
	afx_msg void OnSelendokAnimSound14Pulldown();
	afx_msg void OnSelendokAnimSound15Pulldown();
	afx_msg void OnSelendokAnimSound16Pulldown();
	afx_msg void OnSelendokAnimSound17Pulldown();
	afx_msg void OnSelendokAnimSound18Pulldown();
	afx_msg void OnSelendokAnimSound19Pulldown();
	afx_msg void OnSelendokAnimSound20Pulldown();
	afx_msg void OnSelendokAnimSound21Pulldown();
	afx_msg void OnSelendokAnimSound22Pulldown();
	afx_msg void OnSelendokAnimSound23Pulldown();
	afx_msg void OnSelendokAnimSound3Pulldown();
	afx_msg void OnSelendokAnimSound4Pulldown();
	afx_msg void OnSelendokAnimSound5Pulldown();
	afx_msg void OnSelendokAnimSound6Pulldown();
	afx_msg void OnSelendokAnimSound7Pulldown();
	afx_msg void OnSelendokAnimSound8Pulldown();
	afx_msg void OnSelendokAnimSound9Pulldown();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

