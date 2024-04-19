#if !defined(AFX_GENERICDEATHDIALOG_H__A2CC96A1_CD8F_11D2_A13A_0060089A83BE__INCLUDED_)
#define AFX_GENERICDEATHDIALOG_H__A2CC96A1_CD8F_11D2_A13A_0060089A83BE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GenericDeathDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGenericDeathDialog dialog

#include "objinfo.h"

class CGenericDeathDialog : public CDialog
{
// Construction
public:
	CGenericDeathDialog(object_info *objinfo,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CGenericDeathDialog)
	enum { IDD = IDD_GENERIC_DEATHS };
	int		m_prob1;
	int		m_prob2;
	int		m_prob3;
	int		m_prob4;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGenericDeathDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	object_info *m_objinfo;

	death_info	m_death_types[MAX_DEATH_TYPES];
	int			m_death_probabilities[MAX_DEATH_TYPES];

	// Generated message map functions
	//{{AFX_MSG(CGenericDeathDialog)
	afx_msg void OnGenericDeathEdit1();
	afx_msg void OnGenericDeathEdit2();
	afx_msg void OnGenericDeathEdit3();
	afx_msg void OnGenericDeathEdit4();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GENERICDEATHDIALOG_H__A2CC96A1_CD8F_11D2_A13A_0060089A83BE__INCLUDED_)
