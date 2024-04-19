/*
* $Logfile: /DescentIII/Main/editor/QuickCompile.cpp $
* $Revision: 1.1.1.1 $
* $Date: 2003-08-26 03:57:38 $
* $Author: kevinb $
*
* Does a quick compile of a script
*
* $Log: not supported by cvs2svn $
 * 
 * 2     12/30/98 4:38p Jeff
 * initial creation
*
* $NoKeywords: $
*/



// QuickCompile.cpp : implementation file
//

#include "stdafx.h"
#include "editor.h"
#include "QuickCompile.h"
#include "scriptcompilerapi.h"
#include "ScriptMassCompile.h"
#include "ddio.h"
#include "cfile.h"
#include "manage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CQuickCompile dialog


CQuickCompile::CQuickCompile(char *scriptname,CWnd* pParent /*=NULL*/)
	: CDialog(CQuickCompile::IDD, pParent)
{
	//{{AFX_DATA_INIT(CQuickCompile)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	strcpy(m_scriptname,scriptname);
	ret_value = 0;
}


void CQuickCompile::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CQuickCompile)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CQuickCompile, CDialog)
	//{{AFX_MSG_MAP(CQuickCompile)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CQuickCompile message handlers

CEdit *quick_compile_edit;
CString quick_compile_string;

void quickcompilercallback(char *str)
{
	quick_compile_string += str;
	quick_compile_edit->SetWindowText(quick_compile_string);
}

BOOL CQuickCompile::OnInitDialog() 
{
	CDialog::OnInitDialog();

	quick_compile_string = "";
	quick_compile_edit = (CEdit *)GetDlgItem(IDC_TEXT);
	ret_value = 0;

	char filename[_MAX_PATH];
	char compiled_filename[_MAX_PATH];
	bool isours = false;

	ddio_SplitPath(m_scriptname,NULL,filename,NULL);
	strcpy(compiled_filename,filename);
	strcat(filename,".cpp");
	strcat(compiled_filename,".dll");

	if(mng_FindTrackLock(filename,PAGETYPE_GAMEFILE)!=-1)
		isours = true;

	//grey out the OK button
	CWnd *wnd = (CWnd *)GetDlgItem(IDOK);
	wnd->EnableWindow(false);

	if(cfexist(filename)){
		char full_path[_MAX_PATH];
		ddio_MakePath(full_path,LocalScriptDir,compiled_filename,NULL);
		if(cfexist(full_path)){
			ddio_DeleteFile (full_path);
		}

		if(!isours){
			quick_compile_string += "====================================================\r\n";
			quick_compile_string += "===     SCRIPT IS NOT CHECKED OUT BY YOU!!!!!\r\n";
			quick_compile_string += "====================================================\r\n";
			quick_compile_edit->SetWindowText(quick_compile_string);
		}

		tCompilerInfo ci;
		ci.callback = quickcompilercallback;
		ci.script_type = DetermineScriptType(filename);
		strcpy(ci.source_filename,filename);
		ScriptCompile(&ci);

		if(cfexist(compiled_filename)){
			ret_value = 1;
		}

	}else{
		ret_value = -1;
		char buffer[256];
		sprintf(buffer,"'%s' does not exist",filename);
		quick_compile_edit->SetWindowText(buffer);
	}

	wnd->EnableWindow(true);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CQuickCompile::OnDestroy() 
{
	CDialog::OnDestroy();
	
	quick_compile_string = "";	
}

int CQuickCompile::DoModal() 
{

	return CDialog::DoModal();
}
