// InsSongDlg.cpp : implementation file
//

#include "stdafx.h"
#include "musicutils.h"
#include "InsSongDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInsSongDlg dialog


CInsSongDlg::CInsSongDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CInsSongDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CInsSongDlg)
	m_SongPath = _T("");
	//}}AFX_DATA_INIT
}


void CInsSongDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInsSongDlg)
	DDX_Text(pDX, IDC_EDIT, m_SongPath);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInsSongDlg, CDialog)
	//{{AFX_MSG_MAP(CInsSongDlg)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInsSongDlg message handlers

BOOL CInsSongDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CInsSongDlg::OnBrowse() 
{
	CFileDialog opendlg(TRUE, "osf", NULL, OFN_PATHMUSTEXIST, "Outrage Stream Files(*.osf)|*.osf||", this);
	CEdit *edit = (CEdit *)GetDlgItem(IDC_EDIT);

//	get filename to play
	if (opendlg.DoModal() != IDOK) {
		return;
	}


	edit->SetWindowText((LPCTSTR)opendlg.GetPathName());
}

