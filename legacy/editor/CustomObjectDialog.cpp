// CustomObjectDialog.cpp : implementation file
//

#include "stdafx.h"
#include "editor.h"
#include "CustomObjectDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCustomObjectDialog dialog


CCustomObjectDialog::CCustomObjectDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CCustomObjectDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCustomObjectDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CCustomObjectDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCustomObjectDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCustomObjectDialog, CDialog)
	//{{AFX_MSG_MAP(CCustomObjectDialog)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCustomObjectDialog message handlers
