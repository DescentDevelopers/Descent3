#if !defined(AFX_MOTDDLG_H__C5900F80_3B79_11D2_8CBD_00A0C96ED60D__INCLUDED_)
#define AFX_MOTDDLG_H__C5900F80_3B79_11D2_8CBD_00A0C96ED60D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// MOTDDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMOTDDlg dialog

class CMOTDDlg : public CDialog
{
// Construction
public:
	CMOTDDlg(CWnd* pParent = NULL);   // standard constructor
	BOOL ParseMOTDFile(void);

// Dialog Data
	//{{AFX_DATA(CMOTDDlg)
	enum { IDD = IDD_MOTD_DIALOG };
	CListBox	m_TextList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMOTDDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMOTDDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MOTDDLG_H__C5900F80_3B79_11D2_8CBD_00A0C96ED60D__INCLUDED_)
