#if !defined(AFX_TRUEFALSEDLG_H__D1AFB332_2301_11D2_B2B9_00A0C94B8467__INCLUDED_)
#define AFX_TRUEFALSEDLG_H__D1AFB332_2301_11D2_B2B9_00A0C94B8467__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// TrueFalseDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// TrueFalseDlg dialog

class TrueFalseDlg : public CDialog
{
// Construction
public:
	TrueFalseDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(TrueFalseDlg)
	enum { IDD = IDD_TRUEFALSE };
	int		m_IsFalse;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(TrueFalseDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(TrueFalseDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRUEFALSEDLG_H__D1AFB332_2301_11D2_B2B9_00A0C94B8467__INCLUDED_)
