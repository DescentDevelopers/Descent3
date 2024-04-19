#if !defined(AFX_INSSONGDLG_H__B4518671_1FF8_11D2_B2B3_00A0C94B8467__INCLUDED_)
#define AFX_INSSONGDLG_H__B4518671_1FF8_11D2_B2B3_00A0C94B8467__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// InsSongDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CInsSongDlg dialog

class CInsSongDlg : public CDialog
{
// Construction
public:
	CInsSongDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CInsSongDlg)
	enum { IDD = IDD_INSERT_SONG };
	CString	m_SongPath;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInsSongDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CInsSongDlg)
	afx_msg void OnBrowse();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INSSONGDLG_H__B4518671_1FF8_11D2_B2B3_00A0C94B8467__INCLUDED_)
