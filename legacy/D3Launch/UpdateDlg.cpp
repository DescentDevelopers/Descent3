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

 * $Logfile: /DescentIII/Main/D3Launch/UpdateDlg.cpp $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:56:51 $
 * $Author: kevinb $
 *
 * 
 *
 * $Log: not supported by cvs2svn $
 * 
 * 28    4/15/99 5:02p Nate
 * Added localization support for the motd.txt file
 * 
 * 27    3/19/99 10:18a Nate
 * Added OEM_GENERIC compile type
 * 
 * 26    2/28/99 5:29p Nate
 * Added Auto-Update prompt
 * 
 * 25    2/05/99 3:51p Nate
 * Added conditional compilation directives for OEM support
 * 
 * 24    1/28/99 11:28a Nate
 * Fixed memory leak
 * 
 * 23    11/03/98 1:04p Nate
 * Added label class for user message.
 * 
 * 22    10/21/98 4:51p Nate
 * More fixes.
 * 
 * 21    10/20/98 3:18p Nate
 * 
 * 20    10/18/98 2:43p Nate
 * Changed version number formatting
 * 
 * 19    10/17/98 5:43p Nate
 * Fixed local patch filename in ApplyPatch()
 * 
 * 18    10/13/98 3:03p Nate
 * More fixes and changes.
 * 
 * 17    10/12/98 8:17p Nate
 * Fixed launcher patching bug
 * 
 * 16    10/12/98 7:13p Nate
 * Fixed several bugs.
 * 
 * 15    10/08/98 6:23p Nate
 * Fixed a few bugs.
 * 
 * 14    10/05/98 5:33p Nate
 * Fixed VC6.0 warnings
 * 
 * 13    10/01/98 3:51p Nate
 * Changed some file names and paths.
 * 
 * 12    9/22/98 3:33p Nate
 * Added conditional compiling to help system (can toggle between HTML and
 * standard windows help)
 * 
 * 11    9/21/98 5:40p Nate
 * Incorporated the new HTML help system
 * 
 * 10    9/13/98 2:37p Nate
 * Added update file format information.
 * 
 * 9     9/09/98 1:06p Nate
 * Added Cleanup()
 * 
 * 8     9/02/98 6:44p Nate
 * Added m_should_verify initialization
 * 
 * 7     9/01/98 7:22p Nate
 * Fixed GetVersionNFO() bug.
 * 
 * 6     9/01/98 7:15p Nate
 * Major Revision #2
 * 
 * 5     8/31/98 6:44p Nate
 * Major Revision
 * 
 * 4     8/25/98 6:15p Nate
 * Added the Message of the Day to the Auto Update Dialog
 * 
 * 3     8/24/98 7:07p Nate
 * Added new AutoUpdate features, and fixed display glitches
 * 
 * 2     8/05/98 11:54a Nate
 * Initial Version
 *
 * $NoKeywords: $
 */

// UpdateDlg.cpp : implementation file
//

// The following are the files used by the Auto-Update system:

/***************************************************************************************
version.nfo Format
------------------

; Comments <- Note: ';' must be first char on line

1 00 00 <- first version
http:\\www.outrage.com\update\sites100.nfo	<-	site file of patches from above version
												to latest version

X XX XX <- next version
http:\\...com <- site file of patches to take this version to the newest version
...
...
...

N NN NN <- must end with the latest version (that all the other versions will patch to)

****************************************************************************************
motd.txt Format
---------------

Just make a text file with the text you wish to include in the MOTD box.

****************************************************************************************
sites.nfo Format
----------------

; Comments <- Note: ';' must be first char on line

Site #1's Name 
Site #1's Location
Site #1's URL
Site #1's patch path (directory and filename)

...
...
...

Site #n's Name 
Site #n's Location
Site #n's URL
Site #n's patch path (directory and filename)


Example:
Outrage's Website
United States
http://www.outrage.com
/updates/patch2.rtp


NOTE:
The full path of the patch file to be downloaded will be constructed by
appending the "patch path" to the "URL"

***************************************************************************************/


#include "stdafx.h"
#include "afxpriv.h"
#include "afxext.h"
#include "D3Launch.h"
#include "D3LaunchDlg.h"
#include "MessageWindowDlg.h"
#include "UpdateDlg.h"
#include "SiteSelectDlg.h"
#include <winsock2.h>
#include "inetgetfile.h"
#include "OS_Config.h"
#include "io.h"
#include <sys/stat.h>
#include "LaunchNames.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Globals
//#define WEB_SITE		"www.volition-inc.com"
//#define SITE_TYPE		"http"


bool downloading;		// indicates that a download is in progress
bool patching;			// indicates that a patch is in progress
bool CancelPressed;		// indicates that the user has pressed cancel button
bool UpdateComplete;	// indicates that the update has successfully completed

int update_launcher_status;	// indicates if the launcher itself needs updating

CUpdateDlg *dlg;
uint32_t filesize;
WSADATA wsa_data; 	

// Declare the function pointer to get the Patch apply function from the DLL
extern "C" {
	uint32_t (pascal *RTPatchApply32)( LPSTR cmdline, LPVOID (CALLBACK *lpfCallback)(UINT id, LPVOID lpParm), BOOL WaitFlag);
}

/////////////////////////////////////////////////////////////////////////////
// CUpdateDlg dialog


CUpdateDlg::CUpdateDlg(CWnd* pParent /*=NULL*/, int should_verify /*=1*/)
	: CDialog(CUpdateDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CUpdateDlg)
	m_statusbar = _T("");
	m_usermsg = _T("");
	//}}AFX_DATA_INIT

	m_should_verify=should_verify;
}


void CUpdateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUpdateDlg)
	DDX_Control(pDX, IDC_USERMSG, m_UserMsgStatic);
	DDX_Control(pDX, IDC_LATEST_VERSION_EDIT, m_LatestVersionEdit);
	DDX_Control(pDX, IDC_YOUR_VERSION_EDIT, m_YourVersionEdit);
	DDX_Control(pDX, IDC_MOTD_EDIT, m_MOTDBox);
	DDX_Control(pDX, IDC_PROGRESS1, m_progress);
	DDX_Control(pDX, IDC_LIST1, m_list);
	DDX_Text(pDX, IDC_STATUSBAR, m_statusbar);
	DDX_Text(pDX, IDC_USERMSG, m_usermsg);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CUpdateDlg, CDialog)
	//{{AFX_MSG_MAP(CUpdateDlg)
	ON_BN_CLICKED(IDYES, OnYes)
	ON_WM_CLOSE()
	ON_MESSAGE(WM_COMMANDHELP,OnCommandHelp)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUpdateDlg message handlers


// Downloads the version info, and checks to see if the user needs to update
BOOL CUpdateDlg::GetVersionNfo()
{
	int get_file_ret;

	// Download the version info file
	if (!(InitHTTP())) {
		LocalizedMessageBox(IDS_UPDATEDLG_NO_CON,IDS_UPDATEDLG_ERROR,MB_OK|MB_ICONERROR);
		return FALSE;
	}
	AddToList(IDS_UPDATEDLG_GVN_MSG2);

	get_file_ret=GetHTTPFile(VERSION_PATH, VERSION_LOC_FNAME);
	if (get_file_ret != GETHTTPFILE_OK) {
		CloseHTTP();
		GetHTTPFileErrorBox(get_file_ret,VERSION_PATH, VERSION_LOC_FNAME);
		return FALSE;
	}
	CloseHTTP();

	// parse the most up to date version number out of the file
	FILE *f = fopen(VERSION_LOC_FNAME, "rt");
	if (f==NULL) {
		LocalizedMessageBox(IDS_UPDATEDLG_NO_FIND,IDS_UPDATEDLG_ERROR,MB_OK|MB_ICONERROR);
		return FALSE;
	}

	// grab the last line in file which isn't empty and isn't a comment
	char buffer[_MAX_PATH+1], verbuffer[_MAX_PATH+1];

	strcpy(verbuffer,"");
	strcpy(buffer,"");
	while (!feof(f)) {

		// Read the line into a temporary buffer
		fgets(buffer, _MAX_PATH, f);

		// take the \n off the end of it
		if(strlen(buffer)>0 && buffer[strlen(buffer) - 1] == '\n')
			buffer[strlen(buffer) - 1] = 0;

		// If the line is empty, go get another one
		if(strlen(buffer)==0) continue;

		// If the line is a comment, go get another one
		if(buffer[0]==VERSION_FILE_COMMENT_CHAR) continue;

		// Line is a good one, so save it...
		strcpy(verbuffer,buffer);
	}
	fclose(f);

	// Make sure a version line was found
	if(strlen(verbuffer)==0) {
		LocalizedMessageBox(IDS_UPDATEDLG_NO_FIND,IDS_UPDATEDLG_ERROR,MB_OK|MB_ICONERROR);
		return FALSE;
	}

	// Get the most up to date Version number
	NewMajor=0; NewMinor=0; NewBuild=0;
	if(sscanf(verbuffer, "%i %i %i", &NewMajor, &NewMinor, &NewBuild) != 3) {
		LocalizedMessageBox(IDS_UPDATEDLG_NO_FIND,IDS_UPDATEDLG_ERROR,MB_OK|MB_ICONERROR);
		return FALSE;
	}

	// retrieve the user's current version
	UsrMajor = os_config_read_uint("Version", "Major", 0);
	UsrMinor = os_config_read_uint("Version", "Minor", 0);
	UsrBuild = os_config_read_uint("Version", "Build", 0);
	
	// Make sure the user's Version was found!
	if (VER(UsrMajor, UsrMinor, UsrBuild)==0) {
		LocalizedMessageBox(IDS_UPDATEDLG_NOREGVER_MSG,IDS_UPDATEDLG_NOREGVER_TITLE,MB_OK|MB_ICONERROR);
		return FALSE;
	}

	// Display the versions in their respective boxes
	DisplayVersions(UsrMajor,UsrMinor,UsrBuild,NewMajor,NewMinor,NewBuild);

	// check to see if the user's version is up to date
	if (VER(UsrMajor, UsrMinor, UsrBuild) < VER(NewMajor, NewMinor, NewBuild)) {

		// Change the user message to a bold dark red
		m_UserMsgStatic.SetTextColor(PALETTERGB(128,0,0));
		//m_UserMsgStatic.SetFontBold(TRUE);

		// Light up the progress bar
		// (as further indicator that user's version is already complete)
		m_progress.SetRange(0,100);
		m_progress.SetPos(0);

		// Display the user message
		AddToList(IDS_UPDATEDLG_SRV);
		StatusBar(IDS_UPDATEDLG_WAIT);
		m_usermsg.LoadString(IDS_UPDATEDLG_USERMSG1);
		UpdateData(FALSE);
		return TRUE;
	}

	// Version is ok, so dim the cancel button, and un-dim the ok/yes button
	HWND hwnd;
	GetDlgItem(IDYES,&hwnd);
	::EnableWindow(hwnd,TRUE);
	GetDlgItem(IDCANCEL,&hwnd);
	::EnableWindow(hwnd,FALSE);
	GetDlgItem(IDYES,&hwnd);
	::SetFocus(hwnd);

	// Light up the progress bar
	// (as further indicator that user's version is already complete)
	m_progress.SetRange(0,100);
	m_progress.SetPos(100);

	// The user does not need to do any patching
	UpdateComplete = TRUE;

	// Inform the user that his version is up to date
	AddToList(IDS_UPDATEDLG_SRV);
	AddToList(IDS_UPDATEDLG_UTDT);
	StatusBar(IDS_UPDATEDLG_WAIT);
	m_usermsg.Format( IDS_UPDATEDLG_UTD, UsrMajor, UsrMinor, UsrBuild );
	UpdateData(FALSE);

	return TRUE;
}

// Downloads the Message of the Day
BOOL CUpdateDlg::GetMOTD()
{
	int get_file_ret;

	AddToList(IDS_UPDATEDLG_GVN_MSG1);
	if (!(InitHTTP())) {
		LocalizedMessageBox(IDS_UPDATEDLG_NO_CON,IDS_UPDATEDLG_ERROR,MB_OK|MB_ICONERROR);
		EndDialog(0);
		return FALSE;
	}

#ifdef USE_MULTI_LANGUAGES
	int lang_type=os_config_read_uint(szSectionName,"LanguageType",LANGUAGE_ENGLISH);
	if(lang_type!=LANGUAGE_ENGLISH) {
		char *path=NULL;
		switch(lang_type) {
			case LANGUAGE_FRENCH: 
				path=FRN_MOTD_PATH; 
				break;
			case LANGUAGE_GERMAN: 
				path=GER_MOTD_PATH; 
				break;
			case LANGUAGE_ITALIAN: 
				path=ITN_MOTD_PATH; 
				break;
			case LANGUAGE_SPANISH: 
				path=SPN_MOTD_PATH; 
				break;
		}

		// See if a motd file exists for this language
		if(path!=NULL) {
			get_file_ret=GetHTTPFile(path, MOTD_LOC_FNAME);
			if (get_file_ret == GETHTTPFILE_OK) {
				CloseHTTP();
				return TRUE;
			}
		}
	}
#endif

	get_file_ret=GetHTTPFile(MOTD_PATH, MOTD_LOC_FNAME);
	if (get_file_ret != GETHTTPFILE_OK) {
		CloseHTTP();
		GetHTTPFileErrorBox(get_file_ret,MOTD_PATH, MOTD_LOC_FNAME);

		// If can't get MOTD, just display the error box and set
		// the MOTD to "no message" (don't close down the dialog)
		CString MOTD_buffer;
		MOTD_buffer.LoadString(IDS_UPDATEDLG_NO_MOTD);
		m_MOTDBox.ReplaceSel(MOTD_buffer.GetBuffer(0));
		return FALSE;	
	}
	CloseHTTP();

	return TRUE;
}

// Start the update (download and patch) process
void CUpdateDlg::OnYes() 
{
	// if the update has already been completed, then pressing the 
	// yes/OK button merely closes down the Update dialog 
	// (or the entire launcher if a launcher update needs to be performed)
	if(UpdateComplete) {

		// Display launcher update messages
		switch(update_launcher_status) {
			case LAUNCHER_REPLACE_NONE:	
				break;
			case LAUNCHER_REPLACE_ACCESS_ERROR:
				LocalizedMessageBox(IDS_UPDATEDLG_LNCH_UPD3, IDS_UPDATEDLG_LNCH_UPD_TITLE, MB_OK|MB_ICONERROR);
				break;
			case LAUNCHER_REPLACE_CREATEPROC_ERROR:
				LocalizedMessageBox(IDS_UPDATEDLG_LNCH_UPD2, IDS_UPDATEDLG_LNCH_UPD_TITLE, MB_OK|MB_ICONERROR);
				break;
			case LAUNCHER_REPLACE_NEEDED:
				LocalizedMessageBox(IDS_UPDATEDLG_LNCH_UPD1, IDS_UPDATEDLG_LNCH_UPD_TITLE, MB_OK|MB_ICONINFORMATION);
				break;
		}

		// Remove the temporary files
		Cleanup();

		// If the launcher needs to be updated, shut down the entire launcher
		if(update_launcher_status==LAUNCHER_REPLACE_NEEDED) {
			EndDialog(IDSHUTDOWN);
			return;
		}

		// Otherwise, just close down the Update dialog
		EndDialog(0);
		return;
	}

	// TODO: Add your control notification handler code here
	char buffer[_MAX_PATH+1];
	char verbuffer[_MAX_PATH+1], filebuffer[_MAX_PATH+1];
	uint32_t major, minor, build;
	int get_file_ret;
	CString str_msg;
	int line_num;
	char *filename_only;
	
	FILE *f;

	// Default to not updating the launcher
	update_launcher_status=LAUNCHER_REPLACE_NONE;

	// Dim the YES/OK button and focus on the cancel button
	HWND hwnd;	
	GetDlgItem(IDYES,&hwnd);
	::EnableWindow(hwnd,FALSE);
	GetDlgItem(IDCANCEL,&hwnd);
	::SetFocus(hwnd);

	// Change the user message back to normal
	m_UserMsgStatic.SetTextColor(PALETTERGB(0,0,0));
	//m_UserMsgStatic.SetFontBold(FALSE);

	// Set progress bar range to that of a percentage (0-100)
	m_progress.SetRange(0,100);

	// Open the version file
	f = fopen(VERSION_LOC_FNAME, "rt");
	if(f == NULL){
		LocalizedMessageBox(IDS_UPDATEDLG_YES_ERR1,IDS_UPDATEDLG_ERROR,MB_OK|MB_ICONERROR);
		EndDialog(0);
		return;
	}

	// Read in groups of lines:
	//	line 0 - the Version number
	//	line 1 - full path to the sites.NFO file for the patch
	line_num=0;
	strcpy(buffer,"");
	while (!feof(f)) {

		// Read the line into a temporary buffer
		fgets(buffer, _MAX_PATH, f);

		// take the \n off the end of it
		if(strlen(buffer)>0 && buffer[strlen(buffer) - 1] == '\n')
			buffer[strlen(buffer) - 1] = 0;

		// If the line is empty, go get another one
		if(strlen(buffer)==0) continue;

		// If the line is a comment, go get another one
		if(buffer[0]==VERSION_FILE_COMMENT_CHAR) continue;

		// Read in the appropriate line
		switch(line_num) {
			case 0:
				strcpy(verbuffer, buffer);
				sscanf(verbuffer, "%i %i %i", &major, &minor, &build);
				break;
			case 1:
				strcpy(filebuffer, buffer);
				break;
		}
		line_num++;

		// If a set of lines hasn't been read in yet, get the rest
		if(line_num < 2) continue;

		// Reset the current line for the next set
		line_num=0;


		// If read in version is larger than user's version, update it!
		if (VER(major, minor, build) == VER(UsrMajor, UsrMinor, UsrBuild)) {

			// Download the site info file
			filename_only=strrchr(filebuffer,'/');
			if(filename_only==NULL) 
				filename_only=filebuffer;
			else
				filename_only++;
			str_msg.Format(IDS_UPDATEDLG_GET_FILE,filename_only);
			DebugSpew(str_msg);

			if (!(InitHTTP())) {
				LocalizedMessageBox(IDS_UPDATEDLG_NO_CON,IDS_UPDATEDLG_ERROR,MB_OK|MB_ICONERROR);
				EndDialog(0);
				return;
			}
	
			get_file_ret=GetHTTPFile(filebuffer, SITES_LOC_FNAME);
			if (get_file_ret!=GETHTTPFILE_OK) {
				CloseHTTP();
				fclose(f);
				Cleanup();	
				
				// Output the appropriate error message
				GetHTTPFileErrorBox(get_file_ret, filebuffer, SITES_LOC_FNAME);

				EndDialog(0);				
				return;
			}
			CloseHTTP();

			// Display the site selection dialog
			CSiteSelectDlg site_select_dlg;

			int nResponse = site_select_dlg.DoModal();
			if (nResponse == IDCANCEL)
			{
				// TODO: Place code here to handle when the dialog is
				//  dismissed with Cancel
				fclose(f);
				return;
			}

			// If no site was selected, return
			if(strlen(site_select_dlg.download_path)==0) {
				fclose(f);
				return;
			}

			
			// Download the patch file
			filename_only=strrchr(site_select_dlg.download_path,'/');
			if(filename_only==NULL) 
				filename_only=site_select_dlg.download_path;
			else
				filename_only++;
			str_msg.Format(IDS_UPDATEDLG_GET_FILE,filename_only);
			DebugSpew(str_msg);

			if (!(InitHTTP())) {
				LocalizedMessageBox(IDS_UPDATEDLG_NO_CON,IDS_UPDATEDLG_ERROR,MB_OK|MB_ICONERROR);
				EndDialog(0);				
				return;
			}
	
			get_file_ret=GetHTTPFile(site_select_dlg.download_path, PATCH_LOC_FNAME);
			if (get_file_ret!=GETHTTPFILE_OK) {
				CloseHTTP();
				fclose(f);
				Cleanup();
				
				// Output the appropriate error message
				GetHTTPFileErrorBox(get_file_ret,site_select_dlg.download_path,PATCH_LOC_FNAME);

				EndDialog(0);
				return;
			}
			CloseHTTP();

			str_msg.Format(IDS_UPDATEDLG_PATCHING, NewMajor, NewMinor, NewBuild);
			DebugSpew(str_msg);

			m_usermsg.Format(IDS_UPDATEDLG_USERMSG3,NewMajor,NewMinor,NewBuild);
			UpdateData(FALSE);

			if (!ApplyPatch()) {
				fclose(f);
				Cleanup();
				LocalizedMessageBox(IDS_UPDATEDLG_CANT_PATCH, IDS_UPDATEDLG_ERROR, MB_OK|MB_ICONERROR);
				EndDialog(0);		
				return;
			}

			// Set list view so that all the patch results fit in window
			//int i = m_list.GetCount();
			//i -= 17;
			//i = max(0,i);
			//m_list.SetTopIndex(i);
			UpdateData(FALSE);

			// check if the launcher needs to be updated
			update_launcher_status=UpdateTheLauncher();

			// Write out the updated version number to registry and break out of loop
			UsrMajor = NewMajor;
			UsrMinor = NewMinor;
			UsrBuild = NewBuild;
			os_config_write_uint("Version", "Major", UsrMajor);
			os_config_write_uint("Version", "Minor", UsrMinor);
			os_config_write_uint("Version", "Build", UsrBuild);

			// Display the user's new version in its edit box
			DisplayVersions(UsrMajor,UsrMinor,UsrBuild);

			break;

		} else {
			CString str_msg;
			filename_only=strrchr(filebuffer,'/');
			if(filename_only==NULL) 
				filename_only=filebuffer;
			else
				filename_only++;
			str_msg.Format(IDS_UPDATEDLG_SKIPPING,filename_only);
			DebugSpew(str_msg);
		}

		DeferMessages();
	}

	fclose(f);
	Cleanup();
	LocalizedMessageBox(IDS_UPDATEDLG_PATCH_OK, IDS_UPDATEDLG_SUCCESS, MB_OK|MB_ICONINFORMATION);

	// Dim the cancel button, and un-dim the ok/yes button
	GetDlgItem(IDYES,&hwnd);
	::EnableWindow(hwnd,TRUE);
	GetDlgItem(IDCANCEL,&hwnd);
	::EnableWindow(hwnd,FALSE);
	GetDlgItem(IDYES,&hwnd);
	::SetFocus(hwnd);

	// The patching process has completed successfully!!!
	UpdateComplete = TRUE;

	// Update status info
	StatusBar(IDS_UPDATEDLG_DONE_UPDATING);
	m_usermsg.LoadString(IDS_UPDATEDLG_CONTINUE_MSG);
	UpdateData(FALSE);
	DeferMessages();

	return;
}

void CUpdateDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	CancelPressed = TRUE;
	
	// Only close it down if we're not downloading!
	if(!downloading && !patching) {
		Cleanup();
		EndDialog(0);
		return;
		//CDialog::OnCancel();
	}
}

// Setup the dialog by displaying a temporary connection message, and then
// grabbing the Version info and Message of the Day files
BOOL CUpdateDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_progress.SetRange(0,100);
	m_progress.SetPos(0);
	UpdateComplete = FALSE;
	CancelPressed = FALSE;
	downloading = FALSE;
	patching = FALSE;
	dlg=NULL;

	// Don't put a limit on the MOTD box
	m_MOTDBox.LimitText(0);
	m_MOTDBox.SetMargins(5,5);

	// Display the temporary message window
	msg_dlg.m_WndMsgMain.LoadString(IDS_UPDATEDLG_WNDMSG_MAIN);
	msg_dlg.m_WndMsgStatus.LoadString(IDS_UPDATEDLG_WNDMSG_STATUS);
	msg_dlg.Create(IDD_MESSAGE_WINDOW,this);
	msg_dlg.ShowWindow(SW_SHOW);
	
	// Display wait cursor in case verification takes a while
	CWaitCursor wc;

	// Download the Message of the Day text file
	if(GetMOTD() && ParseMOTDFile()) 
		AddToList(IDS_UPDATEDLG_DL_MOTD);
	else
		AddToList(IDS_UPDATEDLG_NODL_MOTD);

	// Get the Version info file and check it (if user didn't press Cancel already)
	if (CancelPressed || !GetVersionNfo()){
		Cleanup();
		msg_dlg.EndDialog(0);
		EndDialog(0);
		return FALSE;
	}

	// Set the font for the data list to a fixed-width font
	// (so the patching output looks good)
	if(	m_listfont.CreateFont(  15, 0, 0, 0, 0, 0, 0, 0, 0, 
								0, 0, 0, 0, _T("Courier New") ))
								m_list.SetFont(&m_listfont);

	// Get rid of the temporary message window
	msg_dlg.EndDialog(0);

	// Don't need to show update prompt if user's been here already
	os_config_write_uint(NULL,"UpdatePromptShown",1);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


// Cleans a string of any new lines or carriage returns
char *CleanString(char *src)
{
	CString temp;
	uint32_t j;

	// Allocate a temporary string workspace
	temp="";

	// Only copy over wantedcharacters
	for(j=0; j < strlen(src); j++) {
		if(src[j]!='\n' && src[j]!='\r') {
			temp+=src[j];
		}
	}
	strcpy(src,temp.GetBuffer(0));
	return src;
}


// Callback function for patch application (reports status messages from patch process)
__declspec( dllexport )
LPVOID CALLBACK CallBack( UINT Id, LPVOID Parm )
{ 
	CString temp_msg;
	LPVOID RetVal;

	RetVal = "";
  
	// do a few messages while we're here..
	DeferMessages();

	if (dlg) {

		switch( Id )
		{
			case 1:
				// Warning message header
			case 2:
				// Warning message text
				dlg->DebugSpew(CleanString((char *)Parm));
				break;
			case 3:
				// Error message header
				break;
			case 4:
				// Error message text
				temp_msg.LoadString(IDS_UPDATEDLG_PATCH_ERROR);
				MessageBox(NULL, CleanString((char *)Parm), temp_msg, MB_OK|MB_ICONERROR);
				break;
			case 5:
				// % completed
				dlg->m_progress.SetPos(*(UINT *)Parm);
				break;
			case 6:
				// Number of patch files
				// PATCHGUI ignores this
				break;
			case 7:
				// begin patch
				temp_msg.Format(IDS_UPDATEDLG_PROCESSING,(char *)Parm);
				dlg->StatusBar( temp_msg.GetBuffer(0) );	// put on status line
				// max out the progress meter
				dlg->m_progress.SetPos(0x0000);
				break;
			case 8:
				// end patch
				dlg->StatusBar(IDS_UPDATEDLG_FILE_COMPLETE);
				// max out the progress meter
				dlg->m_progress.SetPos(0x8000);
				break;
			case 9:
				// progress message
				dlg->DebugSpew(CleanString((char *)Parm));
				break;
			case 0xa:
				// help message
				//dlg->DebugSpew(CleanString((char *)Parm));
				break;
			case 0xb:
				// patch file comment
				//dlg->DebugSpew(CleanString((char *)Parm));
				break;
			case 0xc:
				// copyright message
				//WriteIt( (char *) Parm );	
				break;
			case 0xd:
				// this one shouldn't happen (only occurs if the command line
				// doesn't have a patch file in it, and we insure that it does).
				break;
			case 0xe:
				// this one shouldn't happen either (same reason)
				break;
			case 0xf:
				// Password Dialog
				// lpProcDlg = MakeProcInstance( PasswordProc, hInst );
				// DialogBox( hInst, "PasswordDialog", hwnd, lpProcDlg);
				// FreeProcInstance( lpProcDlg );
				//InCallBack = FALSE;
				// RetVal = Password;
				break;
			case 0x10:
				// Invalid Password Alert
				// if (IDCANCEL == 
				// MessageBox( 
				// hwnd, 
				// "Password entered was incorrect.  A valid \npassword is required to apply this patch.",
				// "Invalid Password", 
				// MB_OKCANCEL | MB_ICONEXCLAMATION ))
				// AbortPatch = TRUE;
				break;
			case 0x11:
				// Disk Change Dialog
				// lpProcDlg = MakeProcInstance( DiskChgProc, hInst );
				// DialogBox( hInst, "DiskChangeDialog", hwnd, lpProcDlg);
				// FreeProcInstance( lpProcDlg );
				break;
			case 0x12:
				// Disk Change Alert
				// if (IDCANCEL ==
				// MessageBox( 
				// hwnd, 
				// "Invalid patch disk was inserted.\nPlease insert the correct disk.",
				// "Invalid patch disk", 
				// MB_OKCANCEL | MB_ICONEXCLAMATION ))
				// AbortPatch = TRUE;
				break;
			case 0x13:
				// Confirmation Dialog
				// lpProcDlg = MakeProcInstance( ConfirmProc, hInst );
				// DialogBox( hInst, "ConfirmationDialog", hwnd, lpProcDlg);
				// FreeProcInstance( lpProcDlg );
				// InCallBack = FALSE; 
				// RetVal = YNString;
				break;
			case 0x14:
				// Location Dialog
				// lpProcDlg = MakeProcInstance( LocationProc, hInst );
				// DialogBox( hInst, "LocationDialog", hwnd, lpProcDlg);
				// FreeProcInstance( lpProcDlg );
				// InCallBack = FALSE;
				// RetVal = SystemLocation;
				break;
			case 0x16:
				// Searching Call-back
				// PutStatus( "Searching for Systems to Update..." );
				temp_msg.LoadString(IDS_UPDATEDLG_SEARCHING);
				dlg->DebugSpew(temp_msg);
				break;
			case 0x15:
				// Idle...
			default:
				break;
		} 
	}

	// do a few more messages while we're here...
	DeferMessages();

	if (CancelPressed)	// all of our call-back dialogs set this
  						// flag if the user pushes a CANCEL button
		return (NULL);
	else
		return (RetVal);
}


// This function attempts to open the .RTPatch dll and apply the downloaded
// patch to the game, returning TRUE if successful
BOOL CUpdateDlg::ApplyPatch()
{
	char patch_cmd_line[1024];

	HMODULE patchlib = LoadLibrary(PATCH_DLL_FNAME);
	if (!patchlib) {
		LocalizedMessageBox(IDS_UPDATEDLG_NO_PATCH_LIB, IDS_UPDATEDLG_ERROR, MB_OK|MB_ICONERROR);
		return FALSE;
	}

	patching=TRUE;
	sprintf(patch_cmd_line,"-undo %s",PATCH_LOC_FNAME);
	RTPatchApply32 = (uint32_t (__stdcall *)(char *,void *(__stdcall *)(uint32_t,void *),int))GetProcAddress(patchlib, "RTPatchApply32@12");
	if (RTPatchApply32) {
		m_progress.SetRange(0,int16_t(0x8000));
		m_progress.SetPos(0);
		dlg = this;
		StatusBar(IDS_UPDATEDLG_BEG_PATCH);
		if (!(RTPatchApply32(patch_cmd_line, CallBack, TRUE))) {
			FreeLibrary(patchlib);
			StatusBar(IDS_UPDATEDLG_PATCH_COMPLETE);
			patching=FALSE;
			return TRUE;
		} else {
			FreeLibrary(patchlib);
			StatusBar(IDS_UPDATEDLG_PATCH_FAILED);
			patching=FALSE;
			return FALSE;
		}
	}

	FreeLibrary(patchlib);
	patching=FALSE;
	return FALSE;
}


// Checks to see if the Launcher needs to be updated,
// and, if it does, starts up the LauncherUpdate program
// and shuts down the launcher
int CUpdateDlg::UpdateTheLauncher()
{
	// if the new launcher replacement file doesn't exist,
	// and the launcher patch file doesn't exist,
	// then we don't need to replace the launcher

	if( _access(NEW_LAUNCHER_FNAME,0x00) != -1) {	// does new executable exist?
		
		// make sure the replacement file has read/write access
		if( _access(NEW_LAUNCHER_FNAME,0x06) == -1) {
			// File doesn't have read/write access, so give it access
			if( _chmod(NEW_LAUNCHER_FNAME, _S_IREAD | _S_IWRITE ) == -1)
				return LAUNCHER_REPLACE_ACCESS_ERROR;
		}

	}
	else if( _access(LAUNCHER_PATCH_FNAME,0x00) != -1) {	// does patch file exist?
		
		// make sure the replacement file has read/write access
		if( _access(LAUNCHER_PATCH_FNAME,0x06) == -1) {
			// File doesn't have read/write access, so give it access
			if( _chmod(LAUNCHER_PATCH_FNAME, _S_IREAD | _S_IWRITE ) == -1)
				return LAUNCHER_REPLACE_ACCESS_ERROR;
		}

	}
	else
		return LAUNCHER_REPLACE_NONE;  // neither exists, so don't do anything

	// verify that the UpdateLauncher executable exists
	if( _access(REPLACER_FNAME,0x00) == -1) 
		return LAUNCHER_REPLACE_CREATEPROC_ERROR;
		
	// fire up the UpdateLauncher executable
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	CString CommandLine;

	CommandLine.Format("%s %s",REPLACER_FNAME,szTitle.GetBuffer(0));

	memset( &si, 0, sizeof(STARTUPINFO) );
	si.cb = sizeof(si);

	BOOL ret = CreateProcess(	REPLACER_FNAME,				// pointer to name of executable module 
								CommandLine.GetBuffer(0),	// pointer to command line string
								NULL,						// pointer to process security attributes 
								NULL,						// pointer to thread security attributes 
								FALSE,						// handle inheritance flag 
								CREATE_DEFAULT_ERROR_MODE,	// creation flags 
								NULL,						// pointer to new environment block 
								NULL,						// pointer to current directory name 
								&si,						// pointer to STARTUPINFO 
								&pi 						// pointer to PROCESS_INFORMATION  
							);			

	if (!ret) return LAUNCHER_REPLACE_CREATEPROC_ERROR;

	// return "close down the Launcher" ID
	return LAUNCHER_REPLACE_NEEDED;

}

// Downloads a given remote file and saves it as a local file
int CUpdateDlg::GetHTTPFile(char *remote, char *local)
{
	uint32_t LastPrintbytes = time(NULL);
	InetGetFile *inetfile;
	WORD ver=MAKEWORD(1,1);
	char URL[_MAX_PATH+1];

	//sprintf(URL,"%s://%s%s",SITE_TYPE,WEB_SITE,remote);
	strcpy(URL,remote);
	m_progress.SetRange(0,100);
	m_progress.SetPos(0);

	CancelPressed=FALSE;
	downloading=TRUE;
	inetfile = new InetGetFile(URL,local);
	do
	{
		// Check if a download error has occurred
		if(inetfile->IsFileError())
		{
			int new_error;
			int error=inetfile->GetErrorCode();
			delete inetfile;
			downloading=FALSE;

			switch(error) {
				case INET_ERROR_BADPARMS:
					new_error=GETHTTPFILE_BADPARMS;
					break;
				case INET_ERROR_CANT_WRITE_FILE:
					new_error=GETHTTPFILE_CANT_WRITE_FILE;
					break;
				case INET_ERROR_CANT_PARSE_URL:
					new_error=GETHTTPFILE_CANT_PARSE_URL;
					break;
				case INET_ERROR_BAD_FILE_OR_DIR:
					new_error=GETHTTPFILE_BAD_FILE_OR_DIR;
					break;
				case INET_ERROR_HOST_NOT_FOUND:
					new_error=GETHTTPFILE_HOST_NOT_FOUND;
					break;
				case INET_ERROR_NO_MEMORY:
					new_error=GETHTTPFILE_NO_MEMORY;
					break;
				case INET_ERROR_UNKNOWN_ERROR:
				default:
					new_error=GETHTTPFILE_UNKNOWN_ERROR;
					break;
			}

			return new_error;
		}

		// check if the file has been received
		if(inetfile->IsFileReceived())
		{
			delete inetfile;
			downloading=FALSE;
			//m_progress.SetPos(100);
			return GETHTTPFILE_OK;
		}

		// update the download status information
		if(time(NULL)-LastPrintbytes>=1)
		{
			if(inetfile->GetTotalBytes())
			{
				int ipct=0;

				if(inetfile->GetTotalBytes()!=0)
					ipct = int(100.0*(double(inetfile->GetBytesIn())/double(inetfile->GetTotalBytes())));

				m_usermsg.Format(IDS_UPDATEDLG_USERMSG2,inetfile->GetBytesIn(),inetfile->GetTotalBytes(),ipct);
				UpdateData(FALSE);

				m_progress.SetPos(ipct);
				StatusBar(IDS_UPDATEDLG_DL_FILE);
			}
			LastPrintbytes = time(NULL);
		}

		// See if cancel has been pushed
		DeferMessages();

	}while(!CancelPressed);  //}while(!CancelPressed || !inetfile->IsReceiving());

	inetfile->AbortGet();
	delete inetfile;
	downloading=FALSE;

	return GETHTTPFILE_CANCELLED;
}

// Displays the appropriate error message box for a GetHTTPFile() error
void CUpdateDlg::GetHTTPFileErrorBox(int error_id, char *remote, char *local)
{
	CString msg, title;

	// Make sure the data is not NULL
	if(remote==NULL || local==NULL) return;

	// Make sure an error has occurred
	if(error_id==GETHTTPFILE_OK) return;

	// Set the message body according to the given error
	switch(error_id) {
		case GETHTTPFILE_BADPARMS:
			msg.Format(IDS_UPDATEDLG_BADPARMS,remote,local);
			break;
		case GETHTTPFILE_CANT_WRITE_FILE:
			msg.Format(IDS_UPDATEDLG_CANT_WRITE_FILE,local);
			break;
		case GETHTTPFILE_CANT_PARSE_URL:
			msg.Format(IDS_UPDATEDLG_CANT_PARSE_URL,remote);
			break;
		case GETHTTPFILE_BAD_FILE_OR_DIR:
			msg.Format(IDS_UPDATEDLG_BAD_FILE_OR_DIR,remote,local);
			break;
		case GETHTTPFILE_HOST_NOT_FOUND:
			msg.Format(IDS_UPDATEDLG_HOST_NOT_FOUND,remote);
			break;
		case GETHTTPFILE_NO_MEMORY:
			msg.Format(IDS_UPDATEDLG_NO_MEMORY);
			break;
		case GETHTTPFILE_CANCELLED:
			msg.Format(IDS_UPDATEDLG_CANCELLED);
			break;
		case GETHTTPFILE_UNKNOWN_ERROR:
		default:
			msg.Format(IDS_UPDATEDLG_UNKNOWN_ERROR);
			break;
	}

	title.LoadString(IDS_UPDATEDLG_DOWNLOAD_ERROR);

	MessageBox(msg,title, MB_OK | MB_ICONWARNING);
}


// Send a string to the output message list
void CUpdateDlg::DebugSpew(CString &msg)
{
	DebugSpew(msg.GetBuffer(0));
}

void CUpdateDlg::DebugSpew(char *data)
{
	m_list.AddString(data);
	
	int i = m_list.GetCount();
	i -= 13;
	i = max(0,i);
	
	m_list.SetTopIndex(i);

	UpdateData(FALSE);
}

BOOL CUpdateDlg::InitHTTP()
{
	WORD ver=MAKEWORD(1,1);

	StatusBar(IDS_UPDATEDLG_INIT_CON);
	m_usermsg.Format("");
	UpdateData(FALSE);
	DeferMessages();
	
	if (WSAStartup(ver, &wsa_data ))	{
		StatusBar(IDS_UPDATEDLG_WS_ERR);
		return FALSE;
	}

	// Succeeded
	StatusBar(IDS_UPDATEDLG_CON_OK);
	DeferMessages();
	return TRUE;
}

void CUpdateDlg::CloseHTTP()
{
	StatusBar(IDS_UPDATEDLG_CON_SHUTDOWN);
	m_usermsg.Format("");
	UpdateData(FALSE);
	DeferMessages();

	if (WSACleanup())	{
		//mprintf( "Error closing wsock!" );
	}

	StatusBar(IDS_UPDATEDLG_WAITING);
	DeferMessages();
}

void CUpdateDlg::StatusBar(UINT strID)
{
	m_statusbar.LoadString(strID);
	UpdateData(FALSE);
}

void CUpdateDlg::StatusBar(char *status)
{
	m_statusbar=status;
	UpdateData(FALSE);
}

void CUpdateDlg::LocalizedMessageBox(UINT msgID, UINT titleID, UINT type /*=MB_OK*/)
{
	CString msg, title;

	msg.LoadString(msgID);
	title.LoadString(titleID);
	MessageBox(msg,title,type);
}

void CUpdateDlg::AddToList(UINT strID)
{
	CString msg;

	msg.LoadString(strID);
	m_list.AddString(msg);
}

void CUpdateDlg::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	CancelPressed = TRUE;  // in case downloading, cancel out first

	// If the update process is complete, call the OnYes() callback in case
	// the launcher needs to be updated
	if(UpdateComplete) OnYes();
	
	// Only close it down if we're not downloading!
	if(!downloading && !patching) {
		Cleanup();
		EndDialog(0);
		return;
		//CDialog::OnClose();
	}
}

// Reads the message of the day text from the file 
// into the edit box 
BOOL CUpdateDlg::ParseMOTDFile(void)
{
	CString MOTD_buffer;
	char filebuffer[4096+1];
	FILE *f;
	BOOL add_new_line;

	// Try and open the message of the day local text file
	f = fopen(MOTD_LOC_FNAME, "rt");
	if(f == NULL){
		// Couldn't open file, so insert default message
		MOTD_buffer.LoadString(IDS_UPDATEDLG_NO_MOTD);
		m_MOTDBox.SetWindowText(MOTD_buffer.GetBuffer(0));
		return FALSE;
	}

	// Read in the Message of the Day text
	MOTD_buffer="";
	while (!feof(f)) {

		strcpy(filebuffer,"");
		fgets(filebuffer, 4096, f);

		// Check if new line is on the end
		add_new_line=FALSE;
		if(strlen(filebuffer)>0 && filebuffer[strlen(filebuffer) - 1] == '\n') {
			filebuffer[strlen(filebuffer) - 1] = '\0';
			add_new_line=TRUE;
		}

		MOTD_buffer += filebuffer;
		
		// If need to add new line, add both carriage return and line feed
		MOTD_buffer += "\r\n";
	}
	fclose(f);

	m_MOTDBox.SetWindowText(MOTD_buffer.GetBuffer(0));

	return TRUE;
}

// Displays the user's version,
// and (if desired) the latest version in the Version boxes
void CUpdateDlg::DisplayVersions(int maj1, int min1, int bld1, 
								 int maj2 /*=-1*/, int min2 /*=-1*/, int bld2 /*=-1*/)
{
	CString ver_buffer;

	// display user's version
	ver_buffer.Format("%d.%d.%d",maj1,min1,bld1);
	m_YourVersionEdit.SetWindowText(ver_buffer.GetBuffer(0));

	// If not entered, don't display the latest version
	if(maj2==-1 && min2==-1 && bld2==-1) return;
	
	// display latest version
	ver_buffer.Format("%d.%d.%d",maj2,min2,bld2);
	m_LatestVersionEdit.SetWindowText(ver_buffer.GetBuffer(0));
}


#define MAX_NUM_DELETE_TRIES	1	// number of times to try and delete file
#define DELETE_WAIT_TIME		0	// wait this long (in msecs) if delete try failed

// Performs cleanup (deletes temporary files)
void CUpdateDlg::Cleanup(void)
{
	int num_tries;
	
	// Delete the version file
	num_tries=0;
	while(!DeleteFile(VERSION_LOC_FNAME) && num_tries<MAX_NUM_DELETE_TRIES) {
		Sleep(DELETE_WAIT_TIME);
		num_tries++;
	}
	
	// Delete the sites file
	num_tries=0;
	while(!DeleteFile(SITES_LOC_FNAME) && num_tries<MAX_NUM_DELETE_TRIES) {
		Sleep(DELETE_WAIT_TIME);
		num_tries++;
	}

	// Delete the patch file
	num_tries=0;
	while(!DeleteFile(PATCH_LOC_FNAME) && num_tries<MAX_NUM_DELETE_TRIES) {
		Sleep(DELETE_WAIT_TIME);
		num_tries++;
	}
}

BOOL CUpdateDlg::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	// TODO: Add your message handler code here and/or call default

#ifdef USE_HTML_HELP_SYSTEM
	CWaitCursor wc;
	help_launch(UPDATE_HELP);
	return 1;
#else
	return CDialog::OnHelpInfo(pHelpInfo);
#endif
}

// Display the html help file
afx_msg LRESULT CUpdateDlg::OnCommandHelp(WPARAM wParam, LPARAM lParam)
{
#ifdef USE_HTML_HELP_SYSTEM
	help_launch(UPDATE_HELP);
	return 1;
#else
	return CDialog::OnCommandHelp(wParam,lParam);
#endif
}

