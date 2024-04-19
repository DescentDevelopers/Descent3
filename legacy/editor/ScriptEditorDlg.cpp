/*
 * $Logfile: /DescentIII/Main/editor/ScriptEditorDlg.cpp $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:57:38 $
 * $Author: kevinb $
 *
 *	The script editor dialog.
 *
 * $Log: not supported by cvs2svn $
 * 
 * 15    10/08/98 4:24p Kevin
 * Changed code to comply with memory library usage. Always use mem_malloc
 * , mem_free and mem_strdup
 * 
 * 14    9/25/97 5:28p Samir
 * Even newer script code due to more changes in ObjCScript.cpp
 * 
 * 13    9/04/97 4:39p Matt
 * Added includes needed as a result of removing includes from d3edit.h
 * 
 * 12    9/02/97 3:28p Matt
 * Got rid of warnings
 * 
 * 11    8/29/97 2:21p Samir
 * Add some rich edit functionality.
 *
 * $NoKeywords: $
 */


// ScriptEditorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "editor.h"
#include "ddio.h"
#include "ScriptEditorDlg.h"
#include <process.h>
#include "CFILE.h"
#include "OsirisStatusDlg.h"
#include "pserror.h"
#include "mem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MAX_SCRIPT_LINE_SIZE	800
#define DEFAULT_SCRIPT_LOCATION "C:\\OSIRIS.TXT"

/////////////////////////////////////////////////////////////////////////////
// CScriptEditorDlg dialog

static UINT WM_FINDREPLACE =RegisterWindowMessage(FINDMSGSTRING);
char Default_external_editor[256];

CScriptEditorDlg::CScriptEditorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CScriptEditorDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CScriptEditorDlg)
	m_sScript = _T("");
	//}}AFX_DATA_INIT
	m_ExternalEditor="None";
}


void CScriptEditorDlg::DoDataExchange(CDataExchange* pDX)
{
  	/* DDX_Text(pDX, IDC_SCRIPTVIEW, m_sScript);*/

	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CScriptEditorDlg)
  	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CScriptEditorDlg, CDialog)
	//{{AFX_MSG_MAP(CScriptEditorDlg)
	ON_BN_CLICKED(IDC_SET_EXTERNAL_VIEWER, OnSetExternalViewer)
	ON_BN_CLICKED(IDC_USE_EXTERNAL, OnUseExternal)
	ON_BN_CLICKED(ID_SCRIPTEDITOR_EXPORT, OnScripteditorExport)
	ON_BN_CLICKED(ID_SCRIPTEDITOR_IMPORT, OnScripteditorImport)
	ON_BN_CLICKED(IDC_SCRIPT_COMPILE, OnScriptCompile)
	ON_COMMAND(ID_OSIRIS_COMPILE_SCRIPT, OnOsirisCompileScript)
	ON_COMMAND(ID_OSIRIS_COPY, OnOsirisCopy)
	ON_COMMAND(ID_OSIRIS_CUT, OnOsirisCut)
	ON_COMMAND(ID_OSIRIS_EXPORT_SCRIPT, OnOsirisExportScript)
	ON_COMMAND(ID_OSIRIS_FIND, OnOsirisFind)
	ON_COMMAND(ID_OSIRIS_FIND_AND_REPLACE, OnOsirisFindAndReplace)
	ON_COMMAND(ID_OSIRIS_IMPORT_SCRIPT, OnOsirisImportScript)
	ON_COMMAND(ID_OSIRIS_PASTE, OnOsirisPaste)
	ON_COMMAND(ID_OSIRIS_SAVE_EXIT, OnOsirisSaveExit)
	ON_COMMAND(ID_OSIRIS_SET_EXTERNAL, OnOsirisSetExternal)
	ON_COMMAND(ID_OSIRIS_USE_EXTERNAL, OnOsirisUseExternal)
	ON_UPDATE_COMMAND_UI(ID_OSIRIS_PASTE, OnUpdateOsirisPaste)
	//}}AFX_MSG_MAP
	ON_REGISTERED_MESSAGE(WM_FINDREPLACE,OnFindReplace)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScriptEditorDlg message handlers

void CScriptEditorDlg::OnOK() 
{
	// TODO: Add extra validation here
	ddio_KeyFlush();	
	CDialog::OnOK();
}

void CScriptEditorDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	ddio_KeyFlush();
	CDialog::OnCancel();
}


//This sets the script text data in the editor.  Call this before the DoModal() call
void CScriptEditorDlg::SetScript(char *script, tD3XProgram *d3x)
{
	m_Program = d3x;
	m_sScript=script;
}

//This sets the default external editor.  Call this before the DoModal().  If this isn't called, then
//The external editor is set to "None"
void CScriptEditorDlg::SetExternalEditor(char *external)
{
	m_ExternalEditor=external;
}

//Returns the string of script data
char *CScriptEditorDlg::GetScript()
{
	return m_sScript.GetBuffer(1);
}

//Returns the length of the script data (includes end NULL character)
int CScriptEditorDlg::GetScriptLength()
{
	return lstrlen(m_sScript.GetBuffer(1))+1;
}

//This initializes a Find/Replace dialog box (if bFind==false it sets up a Find and Replace, else it's just Find)
void CScriptEditorDlg::InitFindReplace(BOOL bFind) 
{
   //
   // If the dialog has been called already, and then closed,
   // the pointer is still non-NULL, but the object is already
   // dead, so reset the pointer!
   //
   m_pdlgFindReplace = NULL;
   m_LastWord=" ";

   // Construct the dialog
   m_pdlgFindReplace = new CFindReplaceDialog;
   ASSERT(m_pdlgFindReplace != NULL);

   // Initialize the dialog
   if (!m_pdlgFindReplace->Create(bFind, 0, 0, FR_DOWN, this))
   {
      mprintf((0,"Error allocating find/replace dialog!"));
      m_pdlgFindReplace = NULL;
      return;
   }
 
   // Display the dialog
   ASSERT(m_pdlgFindReplace != NULL);
   m_pdlgFindReplace->SetActiveWindow();
   m_pdlgFindReplace->ShowWindow(SW_SHOW);
}


//This is the message handler of sorts for the Find/Replace dialog, it interprets the message and passes the control
//to the appropriate function
LONG CScriptEditorDlg::OnFindReplace(WPARAM wParam, LPARAM lParam)
{
   // Get a pointer to the calling dialog
   CFindReplaceDialog* pDlg = 
      CFindReplaceDialog::GetNotifier(lParam);
   ASSERT(pDlg != NULL);
   
   // See what the user is up to out there...
   if (pDlg->IsTerminating()) 
   {
      // Time to kill the dialog box
      return 0;
   }

   if (pDlg->ReplaceAll())
   {   
      // Put a call to your ReplaceAll() method here...
	   CString find,repl;
	   find=pDlg->GetFindString();
	   repl=pDlg->GetReplaceString();
	   if(pDlg->MatchCase()) m_FindMatchCase=true; else m_FindMatchCase=false;
       ReplaceAll(find.GetBuffer(1),repl.GetBuffer(1));
      return 0;
   }
   
   if (pDlg->ReplaceCurrent()) 
   {   
      // Put a call to your ReplaceCurrent() method here...
	   CString repl;
	   repl=pDlg->GetReplaceString();
	   if(pDlg->MatchCase()) m_FindMatchCase=true; else m_FindMatchCase=false;
	   ReplaceCurrent(repl.GetBuffer(1));
      return 0;
   }
   
   CString str;
   if(pDlg->MatchCase()) m_FindMatchCase=true; else m_FindMatchCase=false;
   str=pDlg->GetFindString();
   
   FindNext(str.GetBuffer(1));

   return 0;
}


//This will look for the next series of charaters (given by the null terminated w)
//If it finds the string, it will highlight/select it and move the caret to the beginning of the word
//returns true if it found the string
//else false
bool CScriptEditorDlg::FindNext(char *w)
{
	int index,max;
	bool word_found;
	bool new_word;
	static int start=0;

	word_found=false;

	//See if this is a new word to look for (as compared to the last searched word)
	if(m_LastWord!=w)
	{
		m_LastWord=w;
		start=0;
		new_word=true;
	}
	else new_word=false;

	//This will update the m_sScript string so it has the current script in it
	UpdateData();
	
	//Make a copy of the script to work with
	char *text;
	text = (char *) mem_malloc(GetScriptLength());
	strcpy(text,GetScript());
	max=GetScriptLength();

	//Make sure we aren't gonna try to go past the end of the buffer, start at 0 if we are
	if(start+(signed)strlen(w)>=max-1) start=0;

	//Here we begin the search
	for(index=start;index<max;index++)
	{
		//check the character it's at in the buffer to the first character of the find string, if they are the same
		//it's a possibility, so Check the word
		if(toupper(text[index])==toupper(w[0]))
			if(CheckWord(index,text,w))
			{
				//We got a match, so move the caret, and select the word
				SetCurrentIndex(index);
				SetSelection(index,strlen(w));
				word_found=true;
				//adjust start so on the next FindNext() it will start from the next character
				start=index+1;
				index=max;
			}
	}

	if(text) mem_free(text);

	if(!word_found)
	{
		//This can mean 2 things.  We searched to the end of the file and either never found a word, or we did at one time

		//either way, start back at the beginning next time
		start=0;

		//if it was a new word then we never found it
		if(new_word)
		{
			OutrageMessageBox("Word Not Found!");
			m_LastWord=" ";
		}
		else
		{
			OutrageMessageBox("Searched to end of file");
		}
		return false;
	}

	return true;
}


//This selects a word/string in the edit box, given the starting zero based character index and the length of the string
void CScriptEditorDlg::SetSelection(int index,int length)
{
	CEdit *edit;
	edit=(CEdit *)GetDlgItem(IDC_SCRIPTVIEW);
	edit->SetActiveWindow();
	edit->SetFocus();
	edit->ShowCaret();
	edit->SetSel(-1,0,false);
	edit->SetSel(index,index+length,false);
}

//This function clears all selections
void CScriptEditorDlg::RemoveAllSelections()
{
	CEdit *edit;
	edit=(CEdit *)GetDlgItem(IDC_SCRIPTVIEW);
	edit->SetFocus();
	edit->ShowCaret();
	edit->SetSel(-1,0,false);
}

//Replaces the selected word with the passed word
void CScriptEditorDlg::ReplaceSelected(char *replace_word)
{
	UpdateData(false);

	CEdit *edit;
	int start_index,end_index;

	edit=(CEdit *)GetDlgItem(IDC_SCRIPTVIEW);
	edit->SetFocus();
	edit->GetSel(start_index,end_index);
	edit->ReplaceSel(replace_word);

	UpdateData();

	SetCurrentIndex(start_index);
}


//This will start from the beginning and replace all the occurences
void CScriptEditorDlg::ReplaceAll(char *find_word,char *replace_word)
{
	SetCurrentIndex(0);
	m_LastWord=" ";
	while(FindNext(find_word))
	{
		ReplaceSelected(replace_word);
	}
}

//This only replaces the selected string with the replace_word
void CScriptEditorDlg::ReplaceCurrent(char *replace_word)
{
	ReplaceSelected(replace_word);
}

//This function moves the caret to the position specified in index
void CScriptEditorDlg::SetCurrentIndex(int index)
{
	CEdit *edit;
	CPoint point;
	edit=(CEdit *) GetDlgItem(IDC_SCRIPTVIEW);
	edit->SetFocus();
	edit->ShowCaret();	
	point=edit->PosFromChar(index);
	edit->SetCaretPos(point);
}


//This function checks to see if the string w is the same as the string that begins in text[index]
//returns true if it is
bool CScriptEditorDlg::CheckWord(int index,char *text,char *w)
{
	int word_length=strlen(w);
	int buffer_size=strlen(text);
	int count;
	if(m_FindMatchCase)
	{
		for(count=0;count<word_length;count++)
		{
			if(index+count>=buffer_size) return false;
			if(text[index+count]!=w[count]) return false;
		}
	}
	else
	{
		for(count=0;count<word_length;count++)
		{
			if(index+count>=buffer_size) return false;
			if(toupper(text[index+count])!=toupper(w[count])) return false;
		}
	}

	return true;
}

//Sets the path to the external editor/viewer
void CScriptEditorDlg::OnSetExternalViewer() 
{
	// TODO: Add your control notification handler code here
	CString filter = "Executables (*.exe)|*.exe|All Files (*.*)|*.*||";

	OutrageMessageBox("WARNING: DO NOT USE WORDPAD OR WRITE!");

	CFileDialog dlg(true, 0,0, OFN_FILEMUSTEXIST, (LPCTSTR)filter, this);
	if(dlg.DoModal()==IDOK)
	{
		m_ExternalEditor=dlg.GetPathName();
		AdjustExternalName();
	}
}

//Called when we want to differ to the external editor/viewer
void CScriptEditorDlg::OnUseExternal() 
{
	// TODO: Add your control notification handler code here
	int flag;
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	CString external;
	bool done=false;

	UpdateData(true);
	ExportScript(DEFAULT_SCRIPT_LOCATION);
	
	external=m_ExternalEditor;
	external+=" ";
	external+=DEFAULT_SCRIPT_LOCATION;

	memset(&si,0,sizeof(si));
	si.cb=sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_SHOW;

	OutrageMessageBox("You are about to enter an external editor.  Please remember to save your work back as a text file");
	flag=CreateProcess(NULL,external.GetBuffer(1),NULL,NULL,false,NORMAL_PRIORITY_CLASS,NULL,NULL,
						&si,&pi);
	if(!flag) 
	{
		OutrageMessageBox("Unable to start external editor");
		return;
	}

	mprintf((0, "External Editor started\n"));
	WaitForSingleObject(pi.hProcess,INFINITE);
	TerminateProcess(pi.hProcess,0);
	Sleep(100); 
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
	mprintf((0, "External Editor finished\n"));

	ImportScript(DEFAULT_SCRIPT_LOCATION);
	UpdateData(false);
}

BOOL CScriptEditorDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
//	RECT rect;
//	BOOL res;
	
	// TODO: Add extra initialization here
//	CWnd *wnd = GetDlgItem(IDC_SCRMOD_BOX);
//	wnd->GetWindowRect(&rect);
//	ScreenToClient(&rect);
//	res = m_RichEdit.Create(WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_WANTRETURN, rect, this, IDC_SCRIPTVIEW);
	
//	CEdit *edit;
//	edit=(CEdit *)GetDlgItem(IDC_SCRIPTVIEW);
//	edit->SetTabStops(5);
	
	AdjustExternalName();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//This takes m_ExternalEditor string and rips out the name of the external editor and displays it
void CScriptEditorDlg::AdjustExternalName()
{
	char temp[20];
	char temp_name[256];
	int i,start=0;

	strcpy(temp_name,m_ExternalEditor.GetBuffer(1));
	for(i=strlen(temp_name);i>=0;i--)
	{
		if(temp_name[i]=='.')
		{
			temp_name[i]='\0';
			i=-1;
		}
	}

	for(i=strlen(temp_name);i>=0;i--)
	{
		if(temp_name[i]=='\\')
		{
			start=i+1;
			i=-1;
		}
	}

	for(i=0;i<20;i++)
	{
		temp[i]=temp_name[i+start];
	}

	CWnd *text;
	text=GetDlgItem(IDC_SCRIPTEXTERNAL);
	text->SetWindowText(temp);
}

void CScriptEditorDlg::ExportScript(char *filename)
{
	CFILE *file;

	file=cfopen(filename,"wt");
	mprintf((0,"Exporting Script to file %s\n",filename));
	cf_WriteString(file, m_sScript.GetBuffer(1));
	cfclose(file);
}



void CScriptEditorDlg::ImportScript(char *filename)
{
	CFILE *file;
	CString temp;
	int size;
	char buffer[MAX_SCRIPT_LINE_SIZE];

	memset(buffer,0,MAX_SCRIPT_LINE_SIZE);

	if(!cfexist(filename)) 
	{
		OutrageMessageBox("File not found!");
		return;
	}

	file=cfopen(filename,"rt");
	mprintf((0,"Importing Script from file %s\n",filename));
	do
	{
		size=cf_ReadString(buffer,MAX_SCRIPT_LINE_SIZE-2,file);
		buffer[size]='\xd';
		buffer[size+1]='\xa';
		buffer[size+2]='\0';
		temp+=buffer;
		memset(buffer,0,size);
	}while(!cfeof(file));

	m_sScript=temp;
	cfclose(file);
}

void CScriptEditorDlg::OnScripteditorExport() 
{
	// TODO: Add your control notification handler code here
}

void CScriptEditorDlg::OnScripteditorImport() 
{
	// TODO: Add your control notification handler code here
	CString path;
	CString filter = "All Files (*.*)|*.*||";

	CFileDialog dlg(true, 0,0, OFN_FILEMUSTEXIST, (LPCTSTR)filter, this);
	if(dlg.DoModal()==IDOK)
	{
		path=dlg.GetPathName();
		ImportScript(path.GetBuffer(1));		
	}

	UpdateData(false);
}

void CScriptEditorDlg::OnScriptCompile() 
{
	// TODO: Add your control notification handler code here
	COsirisStatusDlg progress_status;

	UpdateData(true);

	progress_status.SetScript(m_sScript.GetBuffer(1), m_Program);

	progress_status.DoModal();

}

void CScriptEditorDlg::OnOsirisCompileScript() 
{
	// TODO: Add your command handler code here
	OnScriptCompile();
	
}

void CScriptEditorDlg::OnOsirisCopy() 
{
	// TODO: Add your command handler code here
	CEdit *edit;
	edit=(CEdit *) GetDlgItem(IDC_SCRIPTVIEW);
	edit->Copy();
}

void CScriptEditorDlg::OnOsirisCut() 
{
	// TODO: Add your command handler code here
	CEdit *edit;
	edit=(CEdit *) GetDlgItem(IDC_SCRIPTVIEW);
	edit->Cut();
}

void CScriptEditorDlg::OnOsirisExportScript() 
{
	// TODO: Add your command handler code here
	OnScripteditorExport();	
}

void CScriptEditorDlg::OnOsirisFind() 
{
	// TODO: Add your command handler code here
	InitFindReplace(true);
}

void CScriptEditorDlg::OnOsirisFindAndReplace() 
{
	// TODO: Add your command handler code here
	InitFindReplace(false);
}

void CScriptEditorDlg::OnOsirisImportScript() 
{
	// TODO: Add your command handler code here
	OnScripteditorImport();
}

void CScriptEditorDlg::OnOsirisPaste() 
{
	// TODO: Add your command handler code here
	CEdit *edit;
	edit=(CEdit *) GetDlgItem(IDC_SCRIPTVIEW);
	edit->Paste();
}

void CScriptEditorDlg::OnOsirisSaveExit() 
{
	// TODO: Add your command handler code here
	OnOK();	
}

void CScriptEditorDlg::OnOsirisSetExternal() 
{
	// TODO: Add your command handler code here
	OnSetExternalViewer();	
}

void CScriptEditorDlg::OnOsirisUseExternal() 
{
	// TODO: Add your command handler code here
	OnUseExternal();
}

void CScriptEditorDlg::OnUpdateOsirisPaste(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here

	//this doesn't work right...don't know why
	if(IsClipboardFormatAvailable(CF_TEXT)) pCmdUI->Enable(true); else pCmdUI->Enable(false);
}
