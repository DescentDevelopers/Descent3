/*
 * $Logfile: /DescentIII/Main/hogedit/HogInfo.cpp $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:57:56 $
 * $Author: kevinb $
 *
 * 
 *
 * $Log: not supported by cvs2svn $
 * 
 * 2     8/16/98 4:22p Nate
 * Added message deferal and new hog info dialog
 *
 * $NoKeywords: $
 */

// HogInfo.cpp : implementation file
//

#include "stdafx.h"
#include "hogedit.h"
#include "HogInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHogInfo dialog


CHogInfo::CHogInfo(CWnd* pParent /*=NULL*/)
	: CDialog(CHogInfo::IDD, pParent)
{
	//{{AFX_DATA_INIT(CHogInfo)
	m_NumFiles = _T("");
	m_Location = _T("");
	m_SizeBytes = _T("");
	m_SizeMB = _T("");
	//}}AFX_DATA_INIT
}


void CHogInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHogInfo)
	DDX_Text(pDX, IDC_HOGINFO_FILES, m_NumFiles);
	DDX_Text(pDX, IDC_HOGINFO_LOCATION, m_Location);
	DDX_Text(pDX, IDC_HOGINFO_SIZE_BYTES, m_SizeBytes);
	DDX_Text(pDX, IDC_HOGINFO_SIZE_MB, m_SizeMB);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CHogInfo, CDialog)
	//{{AFX_MSG_MAP(CHogInfo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHogInfo message handlers

BOOL CHogInfo::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
