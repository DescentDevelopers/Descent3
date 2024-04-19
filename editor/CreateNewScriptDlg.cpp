// CreateNewScriptDlg.cpp : implementation file
//

#include "stdafx.h"
#include "editor.h"
#include "CreateNewScriptDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCreateNewScriptDlg dialog


CCreateNewScriptDlg::CCreateNewScriptDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCreateNewScriptDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCreateNewScriptDlg)
	m_Filename = _T("");
	m_ScriptType = -1;
	//}}AFX_DATA_INIT
}


void CCreateNewScriptDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCreateNewScriptDlg)
	DDX_Text(pDX, IDC_FILENAME, m_Filename);
	DDX_Radio(pDX, IDC_LEVEL, m_ScriptType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCreateNewScriptDlg, CDialog)
	//{{AFX_MSG_MAP(CCreateNewScriptDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCreateNewScriptDlg message handlers

void CCreateNewScriptDlg::OnOK() 
{
	UpdateData(true);

	int length = m_Filename.GetLength();
	char *str = m_Filename.GetBuffer(0);
	if(length<=0){
		MessageBox("You must specify a filename","Error");
		return;
	}

	if((length<=4) || stricmp(&str[length-4],".cpp") ){
		//we need to add a .cpp to the end of the name
		m_Filename += ".cpp";
	}

	UpdateData(false);
	
	CDialog::OnOK();
}

BOOL CCreateNewScriptDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_ScriptType = 0;
	
	UpdateData(false);
	
	return TRUE;
}
