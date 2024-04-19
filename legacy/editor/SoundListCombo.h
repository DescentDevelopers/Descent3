#if !defined(AFX_SOUNDLISTCOMBO_H__AE8C8860_B1DD_11D2_A13A_0060089A83BE__INCLUDED_)
#define AFX_SOUNDLISTCOMBO_H__AE8C8860_B1DD_11D2_A13A_0060089A83BE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SoundListCombo.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSoundListCombo window

class CSoundListCombo : public CComboBox
{
// Construction
public:
	CSoundListCombo();
	void Init(int selected);
	void SetSelected(int selected);
	int GetSelected();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSoundListCombo)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSoundListCombo();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSoundListCombo)
	afx_msg void OnSetfocus();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SOUNDLISTCOMBO_H__AE8C8860_B1DD_11D2_A13A_0060089A83BE__INCLUDED_)
