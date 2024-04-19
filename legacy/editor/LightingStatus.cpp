// LightingStatus.cpp : implementation file
//

#include "stdafx.h"
#include "editor.h"
#include "LightingStatus.h"
#include "radiosity.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLightingStatus dialog


CLightingStatus::CLightingStatus(CWnd* pParent /*=NULL*/)
	: CDialog(CLightingStatus::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLightingStatus)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CLightingStatus::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLightingStatus)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLightingStatus, CDialog)
	//{{AFX_MSG_MAP(CLightingStatus)
	ON_BN_CLICKED(IDC_STOP_LIGHTING, OnStopLighting)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLightingStatus message handlers

void CLightingStatus::OnStopLighting() 
{
	int answer=MessageBox ("Are you sure you wish to stop lighting?","Light Question",MB_YESNO);
	if (answer==IDNO)
		return;

	OutrageMessageBox ("Lighting will stop at the next iteration, this might take a minute...");
	
	rad_DoneCalculating=1;
}

BOOL CLightingStatus::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CWnd::SetTimer(1,500,NULL);  // twice per second
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CLightingStatus::OnTimer(UINT nIDEvent) 
{
	char str[100];

	sprintf (str,"Percentage complete: %.2f %%",(1.0-rad_Convergence)*100.0);
	CEdit *ebox;
	ebox=(CEdit *) GetDlgItem (IDC_LIGHTING_COMPLETE);
	ebox->SetWindowText (str);
	
	CDialog::OnTimer(nIDEvent);
}

BOOL CLightingStatus::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CWnd::KillTimer (1);
	return CDialog::DestroyWindow();
}
