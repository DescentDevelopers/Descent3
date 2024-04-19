// AddScriptDialog.cpp : implementation file
//

#include "stdafx.h"
#include "editor.h"
#include "AddScriptDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAddScriptDialog dialog


CAddScriptDialog::CAddScriptDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CAddScriptDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAddScriptDialog)
	m_Name = _T("");
	m_TypeName = _T("");
	//}}AFX_DATA_INIT
}


void CAddScriptDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAddScriptDialog)
	DDX_Text(pDX, IDC_EDITNAME, m_Name);
	DDV_MaxChars(pDX, m_Name, 32);
	DDX_CBString(pDX, IDC_TYPESEL, m_TypeName);
	DDV_MaxChars(pDX, m_TypeName, 16);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAddScriptDialog, CDialog)
	//{{AFX_MSG_MAP(CAddScriptDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAddScriptDialog message handlers


BOOL CAddScriptDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CComboBox *cbox = (CComboBox *)GetDlgItem(IDC_TYPESEL);

	cbox->AddString("object");
	cbox->AddString("trigger");
	cbox->SelectString(-1, "object");

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

