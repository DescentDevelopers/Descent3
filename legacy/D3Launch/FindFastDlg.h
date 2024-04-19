#if !defined(AFX_FINDFASTDLG_H__261D1580_0FD4_11D3_AF2D_00A0C94B8467__INCLUDED_)
#define AFX_FINDFASTDLG_H__261D1580_0FD4_11D3_AF2D_00A0C94B8467__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FindFastDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFindFastDlg dialog

class CFindFastDlg : public CDialog
{
// Construction
public:
	CFindFastDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFindFastDlg)
	enum { IDD = IDD_FINDFASTDLG };
	BOOL	m_AlwaysTerminateFF;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFindFastDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFindFastDlg)
	afx_msg void OnFindfastchkbox();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FINDFASTDLG_H__261D1580_0FD4_11D3_AF2D_00A0C94B8467__INCLUDED_)
