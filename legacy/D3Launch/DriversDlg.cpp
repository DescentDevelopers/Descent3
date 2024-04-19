// DriversDlg.cpp : implementation file
//

#include "stdafx.h"
#include "d3launch.h"
#include "DriversDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDriversDlg dialog


CDriversDlg::CDriversDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDriversDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDriversDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_Selection=INSTALL_NONE;
}


void CDriversDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDriversDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDriversDlg, CDialog)
	//{{AFX_MSG_MAP(CDriversDlg)
	ON_BN_CLICKED(IDC_DIRECTX_BUTTON, OnDirectxButton)
	ON_BN_CLICKED(IDC_GLSETUP_BUTTON, OnGlsetupButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDriversDlg message handlers

BOOL CDriversDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_Selection=INSTALL_NONE;

#if (defined(OEM_VOODOO3) || defined(DISABLE_GLSETUP))
	CWnd *button;
	button=GetDlgItem(IDC_GLSETUP_BUTTON);
	if(button!=NULL) button->EnableWindow(FALSE);
#endif	

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDriversDlg::OnDirectxButton() 
{
	m_Selection=INSTALL_DIRECTX;
	CDialog::OnOK();
}

void CDriversDlg::OnGlsetupButton() 
{
	m_Selection=INSTALL_GLSETUP;
	CDialog::OnOK();
}

void CDriversDlg::OnOK() 
{
	m_Selection=INSTALL_NONE;
	CDialog::OnOK();
}
