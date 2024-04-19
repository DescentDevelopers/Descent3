// PowerupPropDialog.cpp : implementation file
//

#include "stdafx.h"
#include "editor.h"
#include "PowerupPropDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPowerupPropDialog dialog


CPowerupPropDialog::CPowerupPropDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CPowerupPropDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPowerupPropDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CPowerupPropDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPowerupPropDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPowerupPropDialog, CDialog)
	//{{AFX_MSG_MAP(CPowerupPropDialog)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPowerupPropDialog message handlers
