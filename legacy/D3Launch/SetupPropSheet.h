#if !defined(AFX_SETUPPROPSHEET_H__00C45F60_1F5F_11D3_A4E1_00A0C96ED60D__INCLUDED_)
#define AFX_SETUPPROPSHEET_H__00C45F60_1F5F_11D3_A4E1_00A0C96ED60D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SetupPropSheet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSetupPropSheet

class CSetupPropSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CSetupPropSheet)

// Construction
public:
	CSetupPropSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CSetupPropSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSetupPropSheet)
	public:
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSetupPropSheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSetupPropSheet)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETUPPROPSHEET_H__00C45F60_1F5F_11D3_A4E1_00A0C96ED60D__INCLUDED_)
