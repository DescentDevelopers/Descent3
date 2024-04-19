// PropertyAIDlg.cpp : implementation file
//

#include "stdafx.h"
#include "editor.h"
#include "PropertyAIDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPropertyAIDlg dialog


CPropertyAIDlg::CPropertyAIDlg(CWnd* pParent /*=NULL*/)
	: CKeypadDialog(CPropertyAIDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPropertyAIDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CPropertyAIDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropertyAIDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropertyAIDlg, CDialog)
	//{{AFX_MSG_MAP(CPropertyAIDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropertyAIDlg message handlers
