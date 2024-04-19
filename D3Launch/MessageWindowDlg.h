#if !defined(AFX_MESSAGEWINDOWDLG_H__65671E22_38F7_11D2_8CBD_00A0C96ED60D__INCLUDED_)
#define AFX_MESSAGEWINDOWDLG_H__65671E22_38F7_11D2_8CBD_00A0C96ED60D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// MessageWindowDlg.h : header file
//


/////////////////////////////////////////////////////////////////////////////
// CMessageWindowDlg dialog

class CMessageWindowDlg : public CDialog
{
// Construction
public:
	CMessageWindowDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMessageWindowDlg)
	enum { IDD = IDD_MESSAGE_WINDOW };
	CString	m_WndMsgMain;
	CString	m_WndMsgStatus;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMessageWindowDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMessageWindowDlg)
	afx_msg void OnBtnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

extern CMessageWindowDlg *message_window;

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MESSAGEWINDOWDLG_H__65671E22_38F7_11D2_8CBD_00A0C96ED60D__INCLUDED_)
