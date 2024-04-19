// RawCvtDlg.cpp : implementation file
//

#include "stdafx.h"
#include "musicutils.h"
#include "RawCvtDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRawCvtDlg dialog


CRawCvtDlg::CRawCvtDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRawCvtDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRawCvtDlg)
	m_Frequency = -1;
	m_Resolution = -1;
	m_Channels = -1;
	m_VolScale = _T("");
	m_StreamName = _T("");
	m_MeasureSize = 44100;
	//}}AFX_DATA_INIT
}


void CRawCvtDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRawCvtDlg)
	DDX_Radio(pDX, IDC_11khz, m_Frequency);
	DDX_Radio(pDX, IDC_8BIT, m_Resolution);
	DDX_Radio(pDX, IDC_MONO, m_Channels);
	DDX_Text(pDX, IDC_VOLSCALEEDIT, m_VolScale);
	DDX_Text(pDX, IDC_STREAM_NAME, m_StreamName);
	DDX_Text(pDX, IDC_MEASURE, m_MeasureSize);
	DDV_MinMaxInt(pDX, m_MeasureSize, 16384, 131052);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRawCvtDlg, CDialog)
	//{{AFX_MSG_MAP(CRawCvtDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CRawCvtDlg message handlers

BOOL CRawCvtDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if (m_Xforms > 7) 
		m_Xforms = 7;
	((CSliderCtrl *)GetDlgItem(IDC_XFORMSLIDER))->SetRange(0, 7);
	((CSliderCtrl *)GetDlgItem(IDC_XFORMSLIDER))->SetPos(m_Xforms);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CRawCvtDlg::OnOK()
{
	m_Xforms = ((CSliderCtrl *)GetDlgItem(IDC_XFORMSLIDER))->GetPos();

	CDialog::OnOK();
}

