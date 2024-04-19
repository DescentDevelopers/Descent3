#if !defined(AFX_DRIVERSDLG_H__0325ACC0_CC01_11D2_A4E0_00A0C96ED60D__INCLUDED_)
#define AFX_DRIVERSDLG_H__0325ACC0_CC01_11D2_A4E0_00A0C96ED60D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DriversDlg.h : header file
//


// Installation selections
#define INSTALL_NONE				0
#define INSTALL_DIRECTX			1
#define INSTALL_GLSETUP			2

/////////////////////////////////////////////////////////////////////////////
// CDriversDlg dialog

class CDriversDlg : public CDialog
{
// Construction
public:
	CDriversDlg(CWnd* pParent = NULL);   // standard constructor

	int m_Selection;

// Dialog Data
	//{{AFX_DATA(CDriversDlg)
	enum { IDD = IDD_DRIVERS_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDriversDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDriversDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDirectxButton();
	afx_msg void OnGlsetupButton();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DRIVERSDLG_H__0325ACC0_CC01_11D2_A4E0_00A0C96ED60D__INCLUDED_)
