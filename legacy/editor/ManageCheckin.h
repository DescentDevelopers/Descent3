#if !defined(AFX_MANAGECHECKIN_H__2F961C80_7586_11D2_9798_004095AA4E37__INCLUDED_)
#define AFX_MANAGECHECKIN_H__2F961C80_7586_11D2_9798_004095AA4E37__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ManageCheckin.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CManageCheckin dialog

class CManageCheckin : public CDialog
{
// Construction
public:
	CManageCheckin(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CManageCheckin)
	enum { IDD = IDD_MULTICHECK };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CManageCheckin)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CManageCheckin)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MANAGECHECKIN_H__2F961C80_7586_11D2_9798_004095AA4E37__INCLUDED_)
