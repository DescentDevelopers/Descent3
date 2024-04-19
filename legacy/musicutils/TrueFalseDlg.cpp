// TrueFalseDlg.cpp : implementation file
//

#include "stdafx.h"
#include "musicutils.h"
#include "TrueFalseDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// TrueFalseDlg dialog


TrueFalseDlg::TrueFalseDlg(CWnd* pParent /*=NULL*/)
	: CDialog(TrueFalseDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(TrueFalseDlg)
	m_IsFalse = -1;
	//}}AFX_DATA_INIT
}


void TrueFalseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(TrueFalseDlg)
	DDX_Radio(pDX, IDC_TRUE, m_IsFalse);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(TrueFalseDlg, CDialog)
	//{{AFX_MSG_MAP(TrueFalseDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// TrueFalseDlg message handlers
