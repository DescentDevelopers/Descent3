/*
* $Logfile: /DescentIII/Main/D3Launch/GLSetupDlg.cpp $
* $Revision: 1.1.1.1 $
* $Date: 2003-08-26 03:56:51 $
* $Author: kevinb $
*
* <insert description of file here>
*
* $Log: not supported by cvs2svn $
 * 
 * 2     5/20/99 12:53p Nate
 * Added GLSetup window, EAX mixer support, and CHFlight+Mouseman Mode
 * options
*
* $NoKeywords: $
*/

// GLSetupDlg.cpp : implementation file
//

#include "stdafx.h"
#include "d3launch.h"
#include "GLSetupDlg.h"

#include <time.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGLSetupDlg dialog


CGLSetupDlg::CGLSetupDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGLSetupDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGLSetupDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CGLSetupDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGLSetupDlg)
	DDX_Control(pDX, IDC_INSTALL_BUTTON, m_InstallButton);
	DDX_Control(pDX, IDC_INFO3_STATIC, m_Info3Text);
	DDX_Control(pDX, IDC_INFO2_STATIC, m_Info2Text);
	DDX_Control(pDX, IDC_INFO_STATIC, m_Info1Text);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGLSetupDlg, CDialog)
	//{{AFX_MSG_MAP(CGLSetupDlg)
	ON_BN_CLICKED(IDC_INSTALL_BUTTON, OnInstallButton)
	ON_BN_CLICKED(IDC_VISIT_BUTTON, OnVisitButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGLSetupDlg message handlers

BOOL CGLSetupDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// Check the system date, if it is after July 1999, the beta has expired
	bool beta_expired=FALSE;
	SYSTEMTIME sys_time;

	GetSystemTime(&sys_time);
	if(sys_time.wYear>1999) 
		beta_expired=TRUE;
	else if(sys_time.wYear==1999 && sys_time.wMonth>7) 
		beta_expired=TRUE;
	
	if(beta_expired) {
		CString msg;
		msg.LoadString(IDS_GLSETUP_EXPIRED_MSG);
		m_Info1Text.SetWindowText(msg.GetBuffer(0));
		m_Info2Text.SetWindowText("");
		m_Info3Text.SetWindowText("");
		m_InstallButton.EnableWindow(FALSE);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CGLSetupDlg::OnInstallButton() 
{
	CDialog::OnOK();	
}

void CGLSetupDlg::OnVisitButton() 
{
	url_launch("http://www.glsetup.com/");	
}

void CGLSetupDlg::OnCancel() 
{
	CDialog::OnCancel();
}
