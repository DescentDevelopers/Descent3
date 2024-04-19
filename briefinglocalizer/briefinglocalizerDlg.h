// briefinglocalizerDlg.h : header file
//

#if !defined(AFX_BRIEFINGLOCALIZERDLG_H__3C009905_F349_11D2_8F49_00104B27BFF0__INCLUDED_)
#define AFX_BRIEFINGLOCALIZERDLG_H__3C009905_F349_11D2_8F49_00104B27BFF0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CBriefinglocalizerDlg dialog

class CBriefinglocalizerDlg : public CDialog
{
// Construction
public:
	int m_TitleRight;
	int m_TitleLeft;
	int m_CurrentText;
	CString m_BigStringTextLower;
	int m_CurTextRight;
	int m_CurTextLeft;
	CString m_BigTextString;
	CString m_TempFileName;
	CString m_RealFileName;
	bool m_FileChanged;
	CBriefinglocalizerDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CBriefinglocalizerDlg)
	enum { IDD = IDD_BRIEFINGLOCALIZER_DIALOG };
	CRichEditCtrl	m_RichText;
	CString	m_Title;
	CString	m_Text;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBriefinglocalizerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CBriefinglocalizerDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnNext();
	afx_msg void OnPrev();
	afx_msg void OnQuit();
	afx_msg void OnSave();
	afx_msg void OnKillfocusTitle();
	virtual void OnCancel();
	afx_msg void OnChangeText();
	afx_msg void OnChangeTitle();
	afx_msg void OnKillfocusText(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BRIEFINGLOCALIZERDLG_H__3C009905_F349_11D2_8F49_00104B27BFF0__INCLUDED_)
