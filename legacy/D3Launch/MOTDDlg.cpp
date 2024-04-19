// MOTDDlg.cpp : implementation file
//

#include "stdafx.h"
#include "d3launch.h"
#include "MOTDDlg.h"
#include "PsTypes.h"
#include "LaunchNames.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMOTDDlg dialog


CMOTDDlg::CMOTDDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMOTDDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMOTDDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CMOTDDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMOTDDlg)
	DDX_Control(pDX, IDC_LIST1, m_TextList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMOTDDlg, CDialog)
	//{{AFX_MSG_MAP(CMOTDDlg)
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMOTDDlg message handlers

BOOL CMOTDDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_TextList.ResetContent();	

	if(!ParseMOTDFile())
		return FALSE;

	m_TextList.SetTopIndex(0);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


// Reads the message of the day text into the list box 
BOOL CMOTDDlg::ParseMOTDFile(void)
{
	char filebuffer[PSPATHNAME_LEN+1];
	FILE *f;

	f = fopen(MOTD_LOC_FNAME, "rt");
	if(f == NULL){
		return FALSE;
	}

	while (!feof(f)) {
		fgets(filebuffer, PSPATHNAME_LEN, f);
		if(strlen(filebuffer)>0 && filebuffer[strlen(filebuffer) - 1] == '\n')
			filebuffer[strlen(filebuffer) - 1] = '\0';
		m_TextList.AddString(filebuffer);
	}
	fclose(f);

	return TRUE;
}

void CMOTDDlg::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	
	// TODO: Add your message handler code here
	//if(bShow==TRUE && nStatus==SW_PARENTOPENING) {
	//	OutputDebugString("Update dlg has opened\n");
	//}
}
