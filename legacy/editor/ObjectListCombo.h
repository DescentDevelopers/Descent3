#if !defined(AFX_OBJECTLISTCOMBO_H__C8C81360_C67E_11D2_A13A_0060089A83BE__INCLUDED_)
#define AFX_OBJECTLISTCOMBO_H__C8C81360_C67E_11D2_A13A_0060089A83BE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ObjectListCombo.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CObjectListCombo window

class CObjectListCombo : public CComboBox
{
// Construction
public:
	CObjectListCombo();
	void Init(int type,int selected_handle);
	void SetSelected(int selected_handle);
	int GetSelected();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CObjectListCombo)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CObjectListCombo();

private:
	int m_type;

	// Generated message map functions
protected:
	//{{AFX_MSG(CObjectListCombo)
	afx_msg void OnSetfocus();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OBJECTLISTCOMBO_H__C8C81360_C67E_11D2_A13A_0060089A83BE__INCLUDED_)
