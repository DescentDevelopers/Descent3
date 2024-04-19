#if !defined(AFX_PROPERTYAIDLG_H__412CCAE4_F849_11D0_8901_0020A90CB6F2__INCLUDED_)
#define AFX_PROPERTYAIDLG_H__412CCAE4_F849_11D0_8901_0020A90CB6F2__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PropertyAIDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPropertyAIDlg dialog
#ifndef _PROP_AI_DLG_H_
#define _PROP_AI_DLG_H_

#include "KeyPadDialog.h"

class CPropertyAIDlg : public CKeypadDialog
{
// Construction
public:
	CPropertyAIDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPropertyAIDlg)
	enum { IDD = IDD_PROPAI };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropertyAIDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPropertyAIDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTYAIDLG_H__412CCAE4_F849_11D0_8901_0020A90CB6F2__INCLUDED_)
