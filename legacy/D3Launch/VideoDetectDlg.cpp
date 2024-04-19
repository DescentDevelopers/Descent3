// VideoDetectDlg.cpp : implementation file
//

#include "stdafx.h"
#include "d3launch.h"
#include "VideoDetectDlg.h"
#include "OS_Config.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define DETECT_DIRECT3D		0x01
#define DETECT_OPENGL		0x02
#define DETECT_GLIDE		0x04


/////////////////////////////////////////////////////////////////////////////
// CVideoDetectDlg dialog


CVideoDetectDlg::CVideoDetectDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CVideoDetectDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CVideoDetectDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	DetectDirect3D=TRUE;
	DetectOpenGL=TRUE;
	DetectGlide=TRUE;
}


void CVideoDetectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CVideoDetectDlg)
	DDX_Control(pDX, IDC_OPENGL_CHECK, m_OpenGLCheck);
	DDX_Control(pDX, IDC_GLIDE_CHECK, m_GlideCheck);
	DDX_Control(pDX, IDC_DIRECT3D_CHECK, m_Direct3DCheck);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CVideoDetectDlg, CDialog)
	//{{AFX_MSG_MAP(CVideoDetectDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVideoDetectDlg message handlers

void CVideoDetectDlg::OnOK() 
{
	// TODO: Add extra validation here
	DetectDirect3D=m_Direct3DCheck.GetCheck();
	DetectOpenGL=m_OpenGLCheck.GetCheck();
	DetectGlide=m_GlideCheck.GetCheck();

	// Store these settings for next time
	int settings=0;
	if(DetectDirect3D) settings |= DETECT_DIRECT3D;
	if(DetectOpenGL) settings |= DETECT_OPENGL;
	if(DetectGlide) settings |= DETECT_GLIDE;
	os_config_write_uint(szSectionName, "VideoDetectSettings", settings);	

	RenderersDetected=TRUE;

	CDialog::OnOK();
}

BOOL CVideoDetectDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	int settings;

	// Get the settings from last time user chose
	settings=os_config_read_uint(szSectionName, "VideoDetectSettings", DETECT_DIRECT3D|DETECT_GLIDE);
	DetectDirect3D=settings & DETECT_DIRECT3D;
	DetectOpenGL=settings & DETECT_OPENGL;
	DetectGlide=settings & DETECT_GLIDE;

	// Set the check marks appropriately
	m_Direct3DCheck.SetCheck(DetectDirect3D);
	m_OpenGLCheck.SetCheck(DetectOpenGL);
	m_GlideCheck.SetCheck(DetectGlide);
		
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
