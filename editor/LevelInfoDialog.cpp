// LevelInfoDialog.cpp : implementation file
//

#include "stdafx.h"
#include "editor.h"
#include "LevelInfoDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLevelInfoDialog dialog

CLevelInfoDialog::CLevelInfoDialog(level_info *li,CWnd* pParent /*=NULL*/)
	: CDialog(CLevelInfoDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLevelInfoDialog)
	m_copyright = _T("");
	m_designer = _T("");
	m_name = _T("");
	m_notes = _T("");
	//}}AFX_DATA_INIT

	m_level_info = li;

	m_name = li->name;
	m_designer = li->designer;
	m_copyright = li->copyright;
	m_notes = li->notes;

}

void CLevelInfoDialog::GetLevelInfo(level_info *li)
{
	strcpy(li->name,(LPCSTR) m_name);
	strcpy(li->designer,(LPCSTR) m_designer);
	strcpy(li->copyright,(LPCSTR) m_copyright);
	strcpy(li->notes,(LPCSTR) m_notes);
}

void CLevelInfoDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLevelInfoDialog)
	DDX_Text(pDX, IDC_COPYRIGHT, m_copyright);
	DDX_Text(pDX, IDC_DESIGNER, m_designer);
	DDX_Text(pDX, IDC_LEVEL_NAME, m_name);
	DDX_Text(pDX, IDC_NOTES, m_notes);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLevelInfoDialog, CDialog)
	//{{AFX_MSG_MAP(CLevelInfoDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLevelInfoDialog message handlers

void CLevelInfoDialog::OnOK() 
{
	CDialog::OnOK();

	//Copy the new data into our original struct
	GetLevelInfo(m_level_info);
}
