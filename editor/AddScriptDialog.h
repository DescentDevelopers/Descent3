#if !defined(AFX_ADDSCRIPTDIALOG_H__A86E5781_3427_11D1_BDC3_00A0C96ED595__INCLUDED_)
#define AFX_ADDSCRIPTDIALOG_H__A86E5781_3427_11D1_BDC3_00A0C96ED595__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// AddScriptDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAddScriptDialog dialog

class CAddScriptDialog : public CDialog
{
// Construction
public:
	CAddScriptDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAddScriptDialog)
	enum { IDD = IDD_ADDSCRIPT };
	CString	m_Name;
	CString	m_TypeName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAddScriptDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAddScriptDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADDSCRIPTDIALOG_H__A86E5781_3427_11D1_BDC3_00A0C96ED595__INCLUDED_)
