// PlaySongDlg.cpp : implementation file
//

#include "stdafx.h"
#include "musicutils.h"
#include "PlaySongDlg.h"

#include "musicplay.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPlaySongDlg dialog


CPlaySongDlg::CPlaySongDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPlaySongDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPlaySongDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CPlaySongDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPlaySongDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPlaySongDlg, CDialog)
	//{{AFX_MSG_MAP(CPlaySongDlg)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CPlaySongDlg message handlers

BOOL CPlaySongDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

// set up stream to play.
	m_Stream.Open((LPCTSTR)m_StreamName);
	SetTimer(0, 100, NULL);

//	okay we should start playing the song
	CProgressCtrl *progress = (CProgressCtrl *)GetDlgItem(IDC_PLAYPROGRESS);
	progress->SetRange(0, m_Stream.GetLength()/1024);
	
	m_Stream.Play();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CPlaySongDlg::OnTimer(UINT nIDEvent) 
{
	CProgressCtrl *progress = (CProgressCtrl *)GetDlgItem(IDC_PLAYPROGRESS);
  	CStatic *text = (CStatic *)GetDlgItem(IDC_STATUS_TEXT);
	char buf[80];
	sprintf(buf, "%s (%d%%)", (LPCTSTR)m_StreamName, m_Stream.GetPos()*100/m_Stream.GetLength());

	text->SetWindowText(buf);
	progress->SetPos(m_Stream.GetPos()/1024);

	MusicPlayFrame();

	if (m_Stream.State() == STRM_STOPPED || m_Stream.State() == STRM_INVALID) {
		OnOK();
	}

	CDialog::OnTimer(nIDEvent);
}


void CPlaySongDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	KillTimer(0);
	m_Stream.Close();
}
