#if !defined(AFX_MATCENKEYPAD_H__01EF05C6_3D9C_11D2_A13A_0060089A8026__INCLUDED_)
#define AFX_MATCENKEYPAD_H__01EF05C6_3D9C_11D2_A13A_0060089A8026__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// matcenkeypad.h : header file
//

#include "keypaddialog.h"
#include "ObjectListCombo.h"
#include "RoomListCombo.h"

/////////////////////////////////////////////////////////////////////////////
// matcenkeypad dialog

class matcenkeypad : public CKeypadDialog
{
// Construction
public:
	matcenkeypad(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(matcenkeypad)
	enum { IDD = IDD_MATCENKEYPAD };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(matcenkeypad)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	void UpdateDialog();

// Implementation
protected:

	int m_matcen_id;
	void EnableFields(bool f_enable);
	void EnableSpawnFields(int num_enabled);
	void EnableProdFields(int num_enabled);

	CObjectListCombo	m_object_combo;
	CRoomListCombo	m_room_combo;

	// Generated message map functions
	//{{AFX_MSG(matcenkeypad)
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnKillfocusMatEffectPreprodTimeEdit();
	afx_msg void OnSelendokMatProdEffectCombo();
	afx_msg void OnSelendokMatProdSoundCombo();
	afx_msg void OnSelendokMatDisabledSoundCombo();
	afx_msg void OnSelendokMatEnabledSoundCombo();
	afx_msg void OnMatScRadio();
	afx_msg void OnMatWpvRadio();
	afx_msg void OnMatWpnRadio();
	afx_msg void OnMatApnRadio();
	afx_msg void OnMatRandomRadio();
	afx_msg void OnMatOrderedRadio();
	afx_msg void OnMatEnabledCheck();
	afx_msg void OnKillfocusProdRateMultiEdit();
	afx_msg void OnKillfocusMaxAliveChildrenEdit();
	afx_msg void OnSelendokMatProd0Combo();
	afx_msg void OnSelendokMatProd1Combo();
	afx_msg void OnSelendokMatProd2Combo();
	afx_msg void OnSelendokMatProd3Combo();
	afx_msg void OnSelendokMatProd4Combo();
	afx_msg void OnSelendokMatProd5Combo();
	afx_msg void OnSelendokMatProd6Combo();
	afx_msg void OnSelendokMatProd7Combo();
	afx_msg void OnKillfocusMatProd0QuatityEdit();
	afx_msg void OnKillfocusMatProd1QuatityEdit();
	afx_msg void OnKillfocusMatProd2QuatityEdit();
	afx_msg void OnKillfocusMatProd3QuatityEdit();
	afx_msg void OnKillfocusMatProd4QuatityEdit();
	afx_msg void OnKillfocusMatProd5QuatityEdit();
	afx_msg void OnKillfocusMatProd6QuatityEdit();
	afx_msg void OnKillfocusMatProd7QuatityEdit();
	afx_msg void OnKillfocusMatProd0SecEdit();
	afx_msg void OnKillfocusMatProd1SecEdit();
	afx_msg void OnKillfocusMatProd2SecEdit();
	afx_msg void OnKillfocusMatProd3SecEdit();
	afx_msg void OnKillfocusMatProd4SecEdit();
	afx_msg void OnKillfocusMatProd5SecEdit();
	afx_msg void OnKillfocusMatProd6SecEdit();
	afx_msg void OnKillfocusMatProd0PriorityEdit();
	afx_msg void OnKillfocusMatProd1PriorityEdit();
	afx_msg void OnKillfocusMatProd2PriorityEdit();
	afx_msg void OnKillfocusMatProd3PriorityEdit();
	afx_msg void OnKillfocusMatProd4PriorityEdit();
	afx_msg void OnKillfocusMatProd5PriorityEdit();
	afx_msg void OnKillfocusMatProd6PriorityEdit();
	afx_msg void OnKillfocusMatProd7PriorityEdit();
	afx_msg void OnKillfocusMatNumTypesEdit();
	afx_msg void OnKillfocusMatSpawn0Edit();
	afx_msg void OnKillfocusMatSpawn1Edit();
	afx_msg void OnKillfocusMatSpawn2Edit();
	afx_msg void OnKillfocusMatSpawn3Edit();
	afx_msg void OnMatRoomRadio();
	afx_msg void OnMatObjectRadio();
	afx_msg void OnMatUnassignedRadio();
	afx_msg void OnKillfocusMatNumSpawnEdit();
	afx_msg void OnKillfocusMatNameEdit();
	afx_msg void OnMatMoveViewerButton();
	afx_msg void OnMatPasteObjrefButton();
	afx_msg void OnMatPrevButton();
	afx_msg void OnMatNextButton();
	afx_msg void OnMatNewButton();
	afx_msg void OnMatDeleteButton();
	afx_msg void OnMatCopyButton();
	afx_msg void OnMatPasteButton();
	afx_msg void OnMatApvRadio();
	afx_msg void OnKillfocusMatEffectPostprodTimeEdit();
	afx_msg void OnKillfocusMatProd7SecEdit();
	afx_msg void OnKillfocusMaxObjProdEdit();
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnSelendokMatProdTextureCombo();
	afx_msg void OnMatNhpCheck();
	afx_msg void OnSelendokMatRoomObjectCombo();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MATCENKEYPAD_H__01EF05C6_3D9C_11D2_A13A_0060089A8026__INCLUDED_)
