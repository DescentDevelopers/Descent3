#if !defined(AFX_CUSTDEFAULTSCRIPTSELECT_H__919695A1_B1F3_11D2_AB2B_006008BF0B09__INCLUDED_)
#define AFX_CUSTDEFAULTSCRIPTSELECT_H__919695A1_B1F3_11D2_AB2B_006008BF0B09__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CustDefaultScriptSelect.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCustDefaultScriptSelect dialog

class CCustDefaultScriptSelect : public CDialog
{
// Construction
public:
	CCustDefaultScriptSelect(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCustDefaultScriptSelect)
	enum { IDD = IDD_CUSTDEFSCRIPT };
	CString	m_Name;
	CString	m_Module;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCustDefaultScriptSelect)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCustDefaultScriptSelect)
	virtual void OnOK();
	afx_msg void OnBrowse();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CUSTDEFAULTSCRIPTSELECT_H__919695A1_B1F3_11D2_AB2B_006008BF0B09__INCLUDED_)
