/*
* Descent 3 
* Copyright (C) 2024 Parallax Software
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.

--- HISTORICAL COMMENTS FOLLOW ---

 * $Logfile: /DescentIII/Main/hogedit/HogEdit.cpp $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:57:56 $
 * $Author: kevinb $
 *
 * 
 *
 * $Log: not supported by cvs2svn $
 * 
 * 11    10/30/98 11:15a Nate
 * Added support for modification of hog files.
 * 
 * 10    10/28/98 11:24a Nate
 * Added command line auto-processing (create hog from a rib)
 * 
 * 9     10/27/98 5:55p Nate
 * Added command line processing
 * 
 * 8     10/15/98 2:53p Nate
 * Added mem_Init()
 * 
 * 7     8/16/98 4:22p Nate
 * Added message deferal and new hog info dialog
 * 
 * 6     8/14/98 4:38p Nate
 * Added number of files field to status bar.
 * 
 * 5     8/14/98 4:38p Nate
 * Fixed a few minor bugs and added better error reporting
 * 
 * 4     8/14/98 1:01p Nate
 * Added better error reporting for the HogEditor
 * 
 * 3     7/22/98 2:38p Nate
 * Added Modified File prompt when exiting
 * 
 * 2     7/15/98 12:31p Nate
 * Initial version
 *
 * $NoKeywords: $
 */

// HogEdit.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include <afxtempl.h>
#include "HogEdit.h"

#include "MainFrm.h"
#include "hogfile.h"
#include "HogEditDoc.h"
#include "listvwex.h"
#include "HogEditView.h"
#include "mem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CStatusBar *MainStatusBar;	// pointer to the main status bar of the app

// globals for automated hog processing
CString AutoRibFilename;
CString AutoHogFilename;
bool	DoAutoProcess;
bool	UseCurrentHogFilename;

// globals for command line processing
#define MAX_CMDLINE_ARGS		3
#define MAX_CMDLINE_ARG_SIZE	512
char *app_argv[MAX_CMDLINE_ARGS];
int  app_argc;

// Command line parsing function prototype
int ParseAppCommandLine(char *cmdline, char **argv, int max_args);

/////////////////////////////////////////////////////////////////////////////
// CHogEditApp

BEGIN_MESSAGE_MAP(CHogEditApp, CWinApp)
	//{{AFX_MSG_MAP(CHogEditApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHogEditApp construction

CHogEditApp::CHogEditApp()
{
	// TODO: add construction code here,
	WaitCursor=NULL;
	DoAutoProcess=FALSE;
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CHogEditApp object

CHogEditApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CHogEditApp initialization

BOOL CHogEditApp::InitInstance()
{
	int k;

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// Change the registry key under which our settings are stored.
	// You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)


	// Allocate mem for cmd line args
	for(k=0;k<MAX_CMDLINE_ARGS;k++)
		app_argv[k]=new char[MAX_CMDLINE_ARG_SIZE+1];

	// Get the command line arguments
	app_argc=ParseAppCommandLine(m_lpCmdLine,app_argv,MAX_CMDLINE_ARGS);

	// See if user wants to do auto-processing of hog
	DoAutoProcess=FALSE;
	if(app_argc==2) {
		AutoRibFilename=app_argv[0];
		AutoHogFilename=app_argv[1];
		DoAutoProcess=TRUE;
		/*
		AfxMessageBox(AutoRibFilename);
		AfxMessageBox(AutoHogFilename);
		*/
	}
	UseCurrentHogFilename=FALSE;
	OutputDebugString("Parsing command line...\n");

	// Delete mem for cmd line args
	for(k=0;k<MAX_CMDLINE_ARGS;k++)
		delete[] app_argv[k];
	app_argc=0;


	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CHogEditDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CHogEditView));
	AddDocTemplate(pDocTemplate);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	//ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;


	// The one and only window has been initialized, so show and update it.
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();

	// Initialize the memory library
	mem_Init();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CHogEditApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CHogEditApp commands


// Displays the "are you sure?" dialog
int ModifiedPrompt(CWnd *caller)
{
	// If current file has been modified, check with user first...
	if(!DocModified) return(IDYES);
	if(caller->MessageBox("Your current file will be lost. Proceed?",
		          "File Has Been Modified",
				  MB_YESNO)==IDYES) return(IDYES);
	return(IDNO);
}

// Checks if there are any messages and translates and dispatches them
#define MAX_MESSAGES 25
void ProcessMessages(void)
{
	MSG msg;
	int MsgCount;

	for (	MsgCount = MAX_MESSAGES;
			MsgCount && PeekMessage( &msg, NULL, 0, 0, PM_REMOVE );
			MsgCount--) {
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}
}

// Updates the total files indicator
void UpdateTotalFiles(int num_files)
{
	char msg[256];

	sprintf(msg,"Total Files:  %d", num_files);
	MainStatusBar->SetPaneText(1,msg,TRUE);
	ProcessMessages();
}

// Updates the status bar with the given message
CWaitCursor *WaitCursor;
void UpdateStatusBar(char *msg)
{
	if(msg==NULL) return;

	MainStatusBar->SetPaneText(0,msg,TRUE);
	ProcessMessages();

	// If the WaitCursor is not null, restore it
	if(WaitCursor!=NULL)
		WaitCursor->Restore();
}

// Parses the given command line into a list of arguments
#define MAX_CMDLINE_SIZE	1024
int ParseAppCommandLine(char *cmdline, char **argv, int max_args)
{
	int i, argc, done;
	char *s, *q;
	char line[MAX_CMDLINE_SIZE+1];

	// make working copy of command line
	strncpy(line,cmdline,MAX_CMDLINE_SIZE);
	line[MAX_CMDLINE_SIZE]='\0';

	// Init arglist to empty
	for(i=0;i<max_args;i++)
		argv[i][0]='\0';

	argc=0;
	s=line;
	done=FALSE;

	// Get the arguments
	while(!done && argc<max_args) {

		// strip off leading whitespace
		while(isspace(*s)) s++;

		// if it's the end of the line, we're done
		if(strlen(s)==0) {
			done=TRUE;
			continue;
		}

		// process the argument
		if((*s)=='\"') {		// handle quoted arguments
			s++;				// skip to char after quote
			
			q=strchr(s,'\"');	// find ending quote
			if(q==NULL) {
				strcpy(argv[argc],s);
				s+=strlen(argv[argc]);
			}
			else {
				(*q)='\0';
				strcpy(argv[argc],s);
				s=q+1;
			}
		}
		else {					// handle whitespace delimited args
			q=s;
			while(!isspace(*q) && (*q)!='\0') q++;
			if((*q)!='\0') {
				(*q)='\0';
				q++;
			}
			strcpy(argv[argc],s);
			s=q;
		}

		argc++;
	}
	
	return(argc);
}
