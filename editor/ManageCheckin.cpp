// ManageCheckin.cpp : implementation file
//

#include "stdafx.h"
#include "editor.h"
#include "ManageCheckin.h"
#include "gametexture.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CManageCheckin dialog


CManageCheckin::CManageCheckin(CWnd* pParent /*=NULL*/)
	: CDialog(CManageCheckin::IDD, pParent)
{
	//{{AFX_DATA_INIT(CManageCheckin)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CManageCheckin::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CManageCheckin)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CManageCheckin, CDialog)
	//{{AFX_MSG_MAP(CManageCheckin)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CManageCheckin message handlers

BOOL CManageCheckin::OnInitDialog() 
{
	CDialog::OnInitDialog();

	SendDlgItemMessage( IDC_MULTICHECKIN_LISTBOX, LB_RESETCONTENT,0,0);
	for (int i=0;i<MAX_TEXTURES;i++)
	{
		if (GameTextures[i].used)
			SendDlgItemMessage( IDC_MULTICHECKIN_LISTBOX, LB_ADDSTRING,0,(LPARAM) (LPCTSTR) GameTextures[i].name);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
