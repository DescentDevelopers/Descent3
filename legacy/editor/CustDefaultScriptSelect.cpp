// CustDefaultScriptSelect.cpp : implementation file
//

#include "stdafx.h"
#include "editor.h"
#include "CustDefaultScriptSelect.h"
#include "manage.h"
#include "ddio.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCustDefaultScriptSelect dialog


CCustDefaultScriptSelect::CCustDefaultScriptSelect(CWnd* pParent /*=NULL*/)
	: CDialog(CCustDefaultScriptSelect::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCustDefaultScriptSelect)
	m_Name = _T("");
	m_Module = _T("");
	//}}AFX_DATA_INIT
}


void CCustDefaultScriptSelect::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCustDefaultScriptSelect)
	DDX_Text(pDX, IDC_NAME, m_Name);
	DDX_Text(pDX, IDC_MODULE, m_Module);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCustDefaultScriptSelect, CDialog)
	//{{AFX_MSG_MAP(CCustDefaultScriptSelect)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCustDefaultScriptSelect message handlers

void CCustDefaultScriptSelect::OnOK() 
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
}

void CCustDefaultScriptSelect::OnBrowse() 
{

	UpdateData(true);

	CString filter = "D3 Compiled Scripts (*.dll)|*.dll||";
	
	char filename[_MAX_PATH];
	char pathname[_MAX_PATH],name[_MAX_PATH];
	strcpy(pathname,LocalScriptDir);

	if (!OpenFileDialog(this, (LPCTSTR)filter, filename, pathname, sizeof(pathname))) 
		return;

	ddio_SplitPath (filename,NULL,name,NULL);

	m_Module = name;
	
	UpdateData(false);
}
