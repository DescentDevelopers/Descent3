/*
 * $Logfile: /DescentIII/Main/editor/ScriptParmDialog.cpp $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:57:39 $
 * $Author: kevinb $
 * 
 * Script parameter dialog
 *
 * $Log: not supported by cvs2svn $
 * 
 * 6     10/08/98 4:24p Kevin
 * Changed code to comply with memory library usage. Always use mem_malloc
 * , mem_free and mem_strdup
 * 
 * 5     2/11/98 12:21p Samir
 * Fixed bug when pasting object ids.
 *
 * $NoKeywords: $
 */

// ScriptParmDialog.cpp : implementation file
//

#include "stdafx.h"
#include "editor.h"
#include "ScriptParmDialog.h"
#include "ObjectListDialog.h"
#include "mem.h"

#include "d3x.h"
#include "pserror.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define POPUP_PASTEID		0x60


/////////////////////////////////////////////////////////////////////////////
// CScriptParmDialog dialog


CScriptParmDialog::CScriptParmDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CScriptParmDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CScriptParmDialog)
	m_Parm1 = _T("");
	m_Parm2 = _T("");
	m_Parm3 = _T("");
	m_Parm4 = _T("");
	m_Parm5 = _T("");
	m_Parm6 = _T("");
	m_Parm7 = _T("");
	m_Parm8 = _T("");
	m_ParmEdit1 = _T("");
	m_ParmEdit2 = _T("");
	m_ParmEdit3 = _T("");
	m_ParmEdit4 = _T("");
	m_ParmEdit5 = _T("");
	m_ParmEdit6 = _T("");
	m_ParmEdit7 = _T("");
	m_ParmEdit8 = _T("");
	//}}AFX_DATA_INIT
}


void CScriptParmDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CScriptParmDialog)
	DDX_Text(pDX, IDC_PARM1, m_Parm1);
	DDX_Text(pDX, IDC_PARM2, m_Parm2);
	DDX_Text(pDX, IDC_PARM3, m_Parm3);
	DDX_Text(pDX, IDC_PARM4, m_Parm4);
	DDX_Text(pDX, IDC_PARM5, m_Parm5);
	DDX_Text(pDX, IDC_PARM6, m_Parm6);
	DDX_Text(pDX, IDC_PARM7, m_Parm7);
	DDX_Text(pDX, IDC_PARM8, m_Parm8);
	DDX_Text(pDX, IDC_PARMEDIT1, m_ParmEdit1);
	DDX_Text(pDX, IDC_PARMEDIT2, m_ParmEdit2);
	DDX_Text(pDX, IDC_PARMEDIT3, m_ParmEdit3);
	DDX_Text(pDX, IDC_PARMEDIT4, m_ParmEdit4);
	DDX_Text(pDX, IDC_PARMEDIT5, m_ParmEdit5);
	DDX_Text(pDX, IDC_PARMEDIT6, m_ParmEdit6);
	DDX_Text(pDX, IDC_PARMEDIT7, m_ParmEdit7);
	DDX_Text(pDX, IDC_PARMEDIT8, m_ParmEdit8);
	//}}AFX_DATA_MAP
}


void CScriptParmDialog::ConvertToScriptParameter(tScriptParm *parm, const char *type, const char *text)
{
	ZERO_SCRIPT_PARM(parm);

	if (!type || !text) 
		return;

	if (strcmp(type, "vector") == 0) {
		parm->type = PARMTYPE_VECTOR;
		Int3();							// not yet implemented - samir
	}
	else if (strcmp(type, "number") == 0) {
		parm->type = PARMTYPE_NUMBER;
		sscanf(text, "%f", &parm->val.x);
	}
	else {
		int num;
		char *str;
		parm->type = PARMTYPE_REF;
		str = _strlwr(mem_strdup((char *)text));
		if (strstr(str, "0x"))
			sscanf(str+2, "%x",&num);
		else
			sscanf(str, "%d",&num);
		*((int *)&parm->val.x) = num;
	}
}


void CScriptParmDialog::ConvertParamToText(CString& text, tScriptParm *parm)
{
	if (parm->type == PARMTYPE_VECTOR) {
		Int3();									// get samir.
	}
	else if (parm->type == PARMTYPE_NUMBER) {
		text.Format("%f",parm->val.x);
	}
	else {
		text.Format("0x%x",*((int *)&parm->val.x));
	}
}



BEGIN_MESSAGE_MAP(CScriptParmDialog, CDialog)
	//{{AFX_MSG_MAP(CScriptParmDialog)
	ON_BN_CLICKED(IDC_PARAMLIST, OnParamlist)
	ON_BN_CLICKED(IDC_PARAMLIST2, OnParamlist2)
	ON_BN_CLICKED(IDC_PARAMLIST3, OnParamlist3)
	ON_BN_CLICKED(IDC_PARAMLIST4, OnParamlist4)
	ON_BN_CLICKED(IDC_PARAMLIST5, OnParamlist5)
	ON_BN_CLICKED(IDC_PARAMLIST6, OnParamlist6)
	ON_BN_CLICKED(IDC_PARAMLIST7, OnParamlist7)
	ON_BN_CLICKED(IDC_PARAMLIST8, OnParamlist8)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScriptParmDialog message handlers

BOOL CScriptParmDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

	if (!m_Parm1.IsEmpty()) {
		GetDlgItem(IDC_PARMEDIT1)->EnableWindow(TRUE);
		GetDlgItem(IDC_PARAMLIST)->EnableWindow(TRUE);
	}
	if (!m_Parm2.IsEmpty()) {
		GetDlgItem(IDC_PARMEDIT2)->EnableWindow(TRUE);
		GetDlgItem(IDC_PARAMLIST2)->EnableWindow(TRUE);
	}
	if (!m_Parm3.IsEmpty()) {
		GetDlgItem(IDC_PARMEDIT3)->EnableWindow(TRUE);
		GetDlgItem(IDC_PARAMLIST3)->EnableWindow(TRUE);
	}
	if (!m_Parm4.IsEmpty()) {
		GetDlgItem(IDC_PARMEDIT4)->EnableWindow(TRUE);
		GetDlgItem(IDC_PARAMLIST4)->EnableWindow(TRUE);
	}
	if (!m_Parm5.IsEmpty()) {
		GetDlgItem(IDC_PARMEDIT5)->EnableWindow(TRUE);
		GetDlgItem(IDC_PARAMLIST5)->EnableWindow(TRUE);
	}
	if (!m_Parm6.IsEmpty()) {
		GetDlgItem(IDC_PARMEDIT6)->EnableWindow(TRUE);
		GetDlgItem(IDC_PARAMLIST6)->EnableWindow(TRUE);
	}
	if (!m_Parm7.IsEmpty()) {
		GetDlgItem(IDC_PARMEDIT7)->EnableWindow(TRUE);
		GetDlgItem(IDC_PARAMLIST7)->EnableWindow(TRUE);
	}
	if (!m_Parm8.IsEmpty()) {
		GetDlgItem(IDC_PARMEDIT8)->EnableWindow(TRUE);
		GetDlgItem(IDC_PARAMLIST8)->EnableWindow(TRUE);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


//@@void CScriptParmDialog::OnContextMenu(CWnd* pWnd, CPoint point) 
//@@{
//@@	CMenu popup;
//@@
//@@	if(popup.CreatePopupMenu()) {
//@@		popup.AppendMenu(MF_STRING,POPUP_PASTEID,"Paste UID");
//@@			
//@@		if (Copied_object_id == 0) 
//@@			popup.EnableMenuItem(POPUP_PASTEID,MF_GRAYED);
//@@
//@@		m_ContextWnd = pWnd;
//@@		if(!popup.TrackPopupMenu(TPM_LEFTALIGN,point.x,point.y,this,NULL)) {
//@@			mprintf((0,"TrackPopupMenu error!\n"));
//@@		}
//@@	}
//@@	else {
//@@		mprintf((1, "Failed to create shortcut menu!\n"));
//@@	}
//@@}


BOOL CScriptParmDialog::OnCommand(WPARAM wParam, LPARAM lParam) 
{
//@@	if(HIWORD(wParam)==0) {
//@@		switch (LOWORD(wParam))
//@@		{
//@@			case POPUP_PASTEID: 
//@@			{
//@@				char str[32];
//@@				sprintf(str, "%x", Copied_object_id);
//@@				m_ContextWnd->SetWindowText(str);
//@@			}
//@@		}
//@@	}
//@@	
	return CDialog::OnCommand(wParam, lParam);
}



void CScriptParmDialog::OnParamlist() 
{
	CEdit *edit = (CEdit *)GetDlgItem(IDC_PARMEDIT1);
	char buf[32];
	sprintf(buf,"0x%x", Copied_object_id);
	edit->SetWindowText(buf);
}

void CScriptParmDialog::OnParamlist2() 
{
	CEdit *edit = (CEdit *)GetDlgItem(IDC_PARMEDIT2);
	char buf[32];
	sprintf(buf,"0x%x", Copied_object_id);
	edit->SetWindowText(buf);
}

void CScriptParmDialog::OnParamlist3() 
{
	CEdit *edit = (CEdit *)GetDlgItem(IDC_PARMEDIT3);
	char buf[32];
	sprintf(buf,"0x%x", Copied_object_id);
	edit->SetWindowText(buf);
}

void CScriptParmDialog::OnParamlist4() 
{
	CEdit *edit = (CEdit *)GetDlgItem(IDC_PARMEDIT4);
	char buf[32];
	sprintf(buf,"0x%x", Copied_object_id);
	edit->SetWindowText(buf);
}

void CScriptParmDialog::OnParamlist5() 
{
	CEdit *edit = (CEdit *)GetDlgItem(IDC_PARMEDIT5);
	char buf[32];
	sprintf(buf,"0x%x", Copied_object_id);
	edit->SetWindowText(buf);
}

void CScriptParmDialog::OnParamlist6() 
{
	CEdit *edit = (CEdit *)GetDlgItem(IDC_PARMEDIT6);
	char buf[32];
	sprintf(buf,"0x%x", Copied_object_id);
	edit->SetWindowText(buf);
}

void CScriptParmDialog::OnParamlist7() 
{
	CEdit *edit = (CEdit *)GetDlgItem(IDC_PARMEDIT7);
	char buf[32];
	sprintf(buf,"0x%x", Copied_object_id);
	edit->SetWindowText(buf);
}

void CScriptParmDialog::OnParamlist8() 
{
	CEdit *edit = (CEdit *)GetDlgItem(IDC_PARMEDIT8);
	char buf[32];
	sprintf(buf,"0x%x", Copied_object_id);
	edit->SetWindowText(buf);
}
