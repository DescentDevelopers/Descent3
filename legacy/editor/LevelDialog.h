#if !defined(AFX_LEVELDIALOG_H__69096DD1_2927_11D1_BDB5_00A0C96ED595__INCLUDED_)
#define AFX_LEVELDIALOG_H__69096DD1_2927_11D1_BDB5_00A0C96ED595__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// LevelDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLevelDialog dialog

class CLevelDialog : public CDialog
{
// Construction
public:
	CLevelDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CLevelDialog)
	enum { IDD = IDD_LEVELPROPERTIES };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLevelDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual void OnOK();

	// Generated message map functions
	//{{AFX_MSG(CLevelDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LEVELDIALOG_H__69096DD1_2927_11D1_BDB5_00A0C96ED595__INCLUDED_)
