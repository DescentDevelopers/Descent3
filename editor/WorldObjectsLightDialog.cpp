// WorldObjectsLightDialog.cpp : implementation file
//

#include "stdafx.h"
#include "editor.h"
#include "WorldObjectsLightDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWorldObjectsLightDialog dialog


CWorldObjectsLightDialog::CWorldObjectsLightDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CWorldObjectsLightDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWorldObjectsLightDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CWorldObjectsLightDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWorldObjectsLightDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWorldObjectsLightDialog, CDialog)
	//{{AFX_MSG_MAP(CWorldObjectsLightDialog)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWorldObjectsLightDialog message handlers

BOOL CWorldObjectsLightDialog::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	// TODO: Add your message handler code here and/or call default
	WinHelp(HID_LIGHTSDLG,HELP_CONTEXT);
	return TRUE;
	//return CDialog::OnHelpInfo(pHelpInfo);
}
