// SetupPropSheet.cpp : implementation file
//

#include "stdafx.h"
#include "d3launch.h"
#include "SetupPropSheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetupPropSheet

IMPLEMENT_DYNAMIC(CSetupPropSheet, CPropertySheet)

CSetupPropSheet::CSetupPropSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

CSetupPropSheet::CSetupPropSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
}

CSetupPropSheet::~CSetupPropSheet()
{
}


BEGIN_MESSAGE_MAP(CSetupPropSheet, CPropertySheet)
	//{{AFX_MSG_MAP(CSetupPropSheet)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetupPropSheet message handlers

BOOL CSetupPropSheet::OnInitDialog() 
{
	BOOL bResult = CPropertySheet::OnInitDialog();
	
	// TODO: Add your specialized code here
	HWND OK_btn = ::GetDlgItem(this->m_hWnd, IDOK);
	if(OK_btn!=NULL) {
		CString title;
		title.LoadString(IDS_OK);
		::SetWindowText(OK_btn, title.GetBuffer(0));
	}

	HWND Cancel_btn = ::GetDlgItem(this->m_hWnd, IDCANCEL);
	if(Cancel_btn!=NULL) {
		CString title;
		title.LoadString(IDS_CANCEL);
		::SetWindowText(Cancel_btn, title.GetBuffer(0));
	}

	HWND Help_btn = ::GetDlgItem(this->m_hWnd, IDHELP);
	if(Help_btn!=NULL) {
		CString title;
		title.LoadString(IDS_HELP);
		::SetWindowText(Help_btn, title.GetBuffer(0));
	}
	
	return bResult;
}
