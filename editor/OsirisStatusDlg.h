#if !defined(AFX_OSIRISSTATUSDLG_H__0D482AE1_028E_11D1_8901_0020A90CB6F2__INCLUDED_)
#define AFX_OSIRISSTATUSDLG_H__0D482AE1_028E_11D1_8901_0020A90CB6F2__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// OsirisStatusDlg.h : header file
//

#include "d3x.h"

/////////////////////////////////////////////////////////////////////////////
// COsirisStatusDlg dialog

class COsirisStatusDlg : public CDialog
{
// Construction
public:
	COsirisStatusDlg(CWnd* pParent = NULL);   // standard constructor
	void SetScript(char *script, tD3XProgram *d3x);

	// Dialog Data
	//{{AFX_DATA(COsirisStatusDlg)
	enum { IDD = IDD_OSIRIS_STATUS };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COsirisStatusDlg)
	public:
	virtual int DoModal();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COsirisStatusDlg)
	afx_msg void OnOsirisDone();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	CString m_Script;				
	tD3XProgram *m_Program;								// program to output to.
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OSIRISSTATUSDLG_H__0D482AE1_028E_11D1_8901_0020A90CB6F2__INCLUDED_)
