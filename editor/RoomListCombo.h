#if !defined(AFX_ROOMLISTCOMBO_H__EBF11B60_C8D2_11D2_A13A_0060089A83BE__INCLUDED_)
#define AFX_ROOMLISTCOMBO_H__EBF11B60_C8D2_11D2_A13A_0060089A83BE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RoomListCombo.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRoomListCombo window

class CRoomListCombo : public CComboBox
{
// Construction
public:
	CRoomListCombo();
	void Init(int selected);
	void SetSelected(int selected);
	int GetSelected();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRoomListCombo)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CRoomListCombo();

	// Generated message map functions
protected:
	//{{AFX_MSG(CRoomListCombo)
	afx_msg void OnSetfocus();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ROOMLISTCOMBO_H__EBF11B60_C8D2_11D2_A13A_0060089A83BE__INCLUDED_)
