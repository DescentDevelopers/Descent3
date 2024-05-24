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

 * $Logfile: /DescentIII/Main/D3Launch/D3Launch.cpp $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:56:51 $
 * $Author: kevinb $
 *
 * 
 *
 * $Log: not supported by cvs2svn $
 * 
 * 45    8/20/99 9:54a Nate
 * Full OEM Release now calls Ereg
 * 
 * 44    8/19/99 6:13p Samir
 * detect vortex1 chipsets.
 * 
 * 43    6/17/99 5:52p Nate
 * Fixed bug with Italian Help File copying
 * 
 * 42    6/09/99 2:46p Nate
 * Minor changes for different builds
 * 
 * 41    5/26/99 5:07a Nate
 * Fixed static OLE dll linking problem
 * 
 * 40    5/23/99 11:44a Nate
 * Added change to make sure the DDraw dll gets freed when launcher exits.
 * 
 * 39    5/21/99 3:38p Nate
 * Added changes for Rest of World release (new update directory path)
 * 
 * 38    5/19/99 12:29p Nate
 * Fixed openGL crash, changed Network speed default, changed OpenGL
 * detection to default to unchecked, and removed config file parsing from
 * US version
 * 
 * 37    5/07/99 11:23a Nate
 * Added support for a launcher config file
 * 
 * 36    4/27/99 10:42p Nate
 * Added vsync enabling when direct3d is chosen
 * 
 * 35    4/15/99 12:03p Nate
 * Added "Descent 3 Demo 2" build type
 * 
 * 34    4/08/99 1:13p Nate
 * Added Pentium III detection
 * 
 * 33    3/19/99 10:18a Nate
 * Added OEM_GENERIC compile type
 * 
 * 32    3/15/99 3:05p Nate
 * Added fix to multi-language help system
 * 
 * 31    3/12/99 3:29p Nate
 * Added more multi-language support
 * 
 * 30    3/02/99 5:45p Nate
 * Lots of little changes/fixes
 * 
 * 29    2/26/99 12:50p Nate
 * Changed OEM_Voodoo3 names
 * 
 * 28    2/24/99 8:37p Nate
 * Various little dialog changes, added "Install Drivers" dialog
 * 
 * 27    2/24/99 1:46p Nate
 * Added multi-language support
 * 
 * 26    2/22/99 4:05p Nate
 * Made GLU32 fix only for demo version
 * 
 * 25    2/15/99 5:46p Nate
 * Made StraightToSetup default to TRUE if it's not in registry at all
 * 
 * 24    2/15/99 1:41p Nate
 * Added DirectX installation through DirectSetup
 * 
 * 23    2/12/99 5:00p Nate
 * Added some satellite resource DLL test code
 * 
 * 22    1/21/99 11:12a Nate
 * Added hacked fix to the Glu32.dll problem
 * 
 * 21    11/30/98 3:00p Nate
 * Added StringToLower()
 * 
 * 20    10/21/98 12:10p Nate
 * 
 * 19    10/16/98 3:26p Nate
 * 
 * 18    10/15/98 7:30p Nate
 * 
 * 17    10/15/98 11:31a Nate
 * Added Launcher Sound toggling
 * 
 * 16    10/14/98 11:37a Nate
 * Added CoInitialize() call for App
 * 
 * 15    10/08/98 6:23p Nate
 * Fixed a few bugs.
 * 
 * 14    9/30/98 1:59p Nate
 * Added Version constants for demo and full builds
 * 
 * 13    9/29/98 6:05p Nate
 * Added the functionality to update the game version from a text file.
 * 
 * 12    9/21/98 5:40p Nate
 * Incorporated the new HTML help system
 * 
 * 11    9/16/98 3:23p Nate
 * Added m_straight_to_setup detection
 * 
 * 10    9/14/98 3:47p Nate
 * Added deletion of memDC's after use.
 * 
 * 9     9/13/98 2:40p Nate
 * Added re-selecting of default bitmaps and palettes for the device
 * contexts.
 * 
 * 8     9/03/98 6:57p Nate
 * Fixed StretchBlt() problem by doing some 256 color conversions
 * 
 * 7     9/02/98 6:42p Nate
 * Added improved sound support.
 * 
 * 6     9/01/98 7:15p Nate
 * Major Revision #2
 * 
 * 5     8/31/98 6:44p Nate
 * Major Revision
 * 
 * 4     8/24/98 7:06p Nate
 * Added new AutoUpdate features, and fixed display glitches
 * 
 * 3     8/10/98 10:44a Nate
 * Added Language selection support
 * 
 * 2     8/05/98 11:54a Nate
 * Initial Version
 *
 * $NoKeywords: $
 */

// D3Launch.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "mmsystem.h"
#include "D3Launch.h"
#include "D3LaunchDlg.h"
#include "LaunchNames.h"
#include "3D_detect.h"
#include "OS_Config.h"
#include <direct.h>
#include <io.h>
#include <objbase.h>
#include <assert.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Launcher config file options
bool LanguageSelectionEnabled;
bool EregEnabled;

// Launcher sound on/off flag
int LauncherSoundEnabled;

// Declare setup flags
bool VideoCardsDetected;
bool DetailLevelConfigured;
bool NewLanguageSelected;
bool RenderersDetected;

// Flag which indicates if grGlideInit() has been called
bool GlideInited;

// Global storage for DirectX version number
int Dx_version;

// Global storage for DDraw handle
HINSTANCE Dd_dll_handle=NULL;

// Global storage for OpenGL library
HINSTANCE opengl_dll_handle=NULL;

// Values to scale the bitmaps to match the dialog size
double DlgWidthModifier;
double DlgHeightModifier;

// Function to update the game Version after a manual patch
bool ProcessUpdatedVersionFile(void);

// Function to see if user needs Glu32.dll
bool ProcessGlu32DLL(void);

/////////////////////////////////////////////////////////////////////////////
// CD3LaunchApp

BEGIN_MESSAGE_MAP(CD3LaunchApp, CWinApp)
	//{{AFX_MSG_MAP(CD3LaunchApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CD3LaunchApp construction

CD3LaunchApp::CD3LaunchApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	m_straight_to_update = 0;
	m_hResInst=NULL;
	m_hDefResInst=NULL;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CD3LaunchApp object

CD3LaunchApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CD3LaunchApp initialization

BOOL CD3LaunchApp::FirstInstance()
{
	HANDLE hnd;

	// See if the D3 Launcher mutex has already been created
	hnd=OpenMutex(MUTEX_ALL_ACCESS,FALSE,D3_LAUNCHER_MUTEX_NAME);
	if(hnd==NULL) 
		return TRUE; // Nope, this is the first instance

	// close the mutex handle
	CloseHandle(hnd);

	// Determine if another window with our title exists...
	CWnd *pWndPrev, *pWndChild;
	pWndPrev = CWnd::FindWindow(NULL,szTitle );
	if (pWndPrev)
	{
		// if so, does it have any popups?
		pWndChild = pWndPrev->GetLastActivePopup();
 
		// If iconic, restore the main window
		if (pWndPrev->IsIconic())
			pWndPrev->ShowWindow(SW_RESTORE);
 
		// Bring the main window or its popup to
		// the foreground
		pWndChild->SetForegroundWindow();
	}

	// and we are done activating the previous one.
	return FALSE;
}

// parse for special (outrage) command-line arguments
void CD3LaunchApp::OutrageParseCommandLine()
{
	// right now all we do is a direct string compare on the 
	// application object's
	// command line var, for the "straight to update" argument
	if(!strcmp(m_lpCmdLine, "-straight_to_update")){
		m_straight_to_update = 1;
	}
}

BOOL CD3LaunchApp::InitInstance()
{
	//AfxEnableControlContainer();

	// Get a handle for the executable's resources (english)
	m_hDefResInst=AfxGetResourceHandle();

	// Get the title of the launcher
	SetLauncherTitleString();

	// If a previous instance of the application is already running,
	// then activate it and return FALSE from InitInstance to
	// end the execution of this instance.
	if(!FirstInstance()){
		return FALSE;
	}

	// Create the Mutex to signal that the Launcher is currently running
	if(CreateMutex(NULL,TRUE,D3_LAUNCHER_MUTEX_NAME)==NULL) {
		return FALSE;
	}

	// Initialize the com object for this ap
	CoInitialize(NULL);

	// parse the command line
	OutrageParseCommandLine();

	// check to see if the Setup dialog should be displayed right away
	m_straight_to_setup = (UINT)os_config_read_uint(szSectionName, "StraightToSetup", 1);

	// check to see if the launcher sound is enabled
	LauncherSoundEnabled = os_config_read_uint(szSectionName, "LauncherSoundEnabled", 1);

	// check for an updated version text file
	ProcessUpdatedVersionFile();

	// Setup Config Defaults for Releases
#if (defined(FULL_US_RELEASE) || defined(FULL_AUSSIE_RELEASE) || defined(OEM_GENERIC))
	LanguageSelectionEnabled=FALSE;
	EregEnabled=TRUE;
#elif (defined(FULL_ROW_RELEASE) && !defined(INITIAL_UK_RELEASE))
	LanguageSelectionEnabled=TRUE;
	EregEnabled=FALSE;
#elif (defined(FULL_ROW_RELEASE) && defined(INITIAL_UK_RELEASE))
	LanguageSelectionEnabled=FALSE;
	EregEnabled=FALSE;
#else
	LanguageSelectionEnabled=FALSE;
	EregEnabled=FALSE;
#endif

#ifdef DEMO
	// see if user needs Glu32.dll
	ProcessGlu32DLL();
#endif

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	BOOL ret;
	/*
	// Get the application palette
	ret = GetBitmapAndPalette(IDB_PALETTE_BITMAP,&m_bkBmap,&m_palette);
	if ( !ret ) return TRUE;

	// Get the background bitmap
	ret = GetBitmapAndPalette(IDB_D3LAUNCH_BG,&m_bkBmap,NULL);
	if ( !ret ) return TRUE;
	*/

	// Get the background bitmap
	ret = GetBitmapAndPalette(IDB_D3LAUNCH_BG,&m_bkBmap,&m_palette);
	if ( !ret ) return TRUE;

	// Nothing has been detected yet
	VideoCardsDetected=FALSE;
	DetailLevelConfigured=FALSE;
	NewLanguageSelected=FALSE;
	RenderersDetected=FALSE;
	GlideInited=FALSE;
	Dx_version=0;
	Dd_dll_handle=NULL;
	opengl_dll_handle=NULL;

	// Display the main menu dialog
	CD3LaunchDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

int CD3LaunchApp::ExitInstance() 
{
	// If glide has been initialized, shut it down before we exit
	//shutdown_glide();  // don't need this now (it shuts it down in 3D_detect)
	
	if(m_hResInst!=NULL) {
		FreeLibrary(m_hResInst);
		m_hResInst=NULL;
	}

	if(opengl_dll_handle!=NULL) {
		FreeLibrary(opengl_dll_handle);
		opengl_dll_handle=NULL;
	}

	// Just in case, make sure the DDraw DLL has been freed
	if (Dd_dll_handle!=NULL) {
		FreeLibrary(Dd_dll_handle);
		Dd_dll_handle=NULL;
	}

	return CWinApp::ExitInstance();
}

// Launches user's web browser with appropriate file
#define HELP_DIR	"hlp\\"
void help_launch(char *help_filename)
{
	char curr_dir[_MAX_PATH+1];
	char file_path[_MAX_PATH+1];

	if(_getcwd(curr_dir,_MAX_PATH)==NULL)
		return;

	sprintf(file_path,"%s\\%s%s",curr_dir,HELP_DIR,help_filename);

	// Make sure the help file exist
	if( _access(file_path,0x00) == -1) {	
		CString nohelp_msg;
		nohelp_msg.Format(IDS_D3LAUNCH_NO_HELP,file_path);
		AfxMessageBox( nohelp_msg, MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	// Display the help file
	url_launch(file_path);
}


// Starts up user's web browser with the given URL
void url_launch(char *url)
{
	int r;

	r = (int) ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOW);
	if (r < 32) {
		CString txt;
		switch (r) {
			case 0:	txt.LoadString(IDS_D3LAUNCH_URL_ERR1); break;
			case ERROR_BAD_FORMAT: txt.LoadString(IDS_D3LAUNCH_URL_ERR2); break;
			case SE_ERR_ACCESSDENIED: txt.LoadString(IDS_D3LAUNCH_URL_ERR3); break;
			case SE_ERR_ASSOCINCOMPLETE: txt.LoadString(IDS_D3LAUNCH_URL_ERR4); break;
			case SE_ERR_DDEBUSY: txt.LoadString(IDS_D3LAUNCH_URL_ERR5); break;
			case SE_ERR_DDEFAIL: txt.LoadString(IDS_D3LAUNCH_URL_ERR6); break;
			case SE_ERR_DDETIMEOUT: txt.LoadString(IDS_D3LAUNCH_URL_ERR7); break;
			case SE_ERR_DLLNOTFOUND: txt.LoadString(IDS_D3LAUNCH_URL_ERR8); break;
			case SE_ERR_OOM: txt.LoadString(IDS_D3LAUNCH_URL_ERR9); break;
			case SE_ERR_SHARE: txt.LoadString(IDS_D3LAUNCH_URL_ERR10); break;

			// No browser installed message
			case SE_ERR_NOASSOC:
			case ERROR_FILE_NOT_FOUND:
			case ERROR_PATH_NOT_FOUND:
				txt.LoadString(IDS_D3LAUNCH_URL_ERR11);
				break;

			default: txt.LoadString(IDS_D3LAUNCH_URL_ERR12); break;
		}
		AfxMessageBox(txt, MB_OK | MB_ICONERROR);
	}
}


// Plays a sound (.wav resource)
BOOL PlayResource(LPSTR lpName, bool WaitUntilDone)
{    
	BOOL bRtn;    
	LPSTR lpRes;    
	HANDLE hRes;
    HRSRC hResInfo;    
	HINSTANCE Nl=AfxGetResourceHandle();

    /* Find the WAVE resource. */    
	hResInfo= FindResource(Nl,lpName,"WAVE");

    if(hResInfo == NULL)       
		return FALSE;    
	
	/* Load the WAVE resource. */
    hRes = LoadResource(Nl,hResInfo);
    if (hRes == NULL)
		return FALSE;

    /* Lock the WAVE resource and play it. */
    lpRes=(LPSTR)LockResource(hRes);
    if(lpRes==NULL)
		return FALSE;

	int play_type = (WaitUntilDone) ? SND_SYNC : SND_ASYNC;

    bRtn = sndPlaySound(lpRes, SND_MEMORY | play_type);
    if(bRtn == NULL)
      return FALSE;

    /* Free the WAVE resource and return success or failure. */
    FreeResource(hRes);
    return TRUE;
}


// Assigns RGB values to a palette entry 
BOOL SetPaletteEntry(PALETTEENTRY *pal_entry, BYTE red, BYTE green, BYTE blue)
{
	pal_entry->peRed = red;
	pal_entry->peGreen = green;
	pal_entry->peBlue = blue;
	pal_entry->peFlags = 0;

	return TRUE;
}


// Loads a bitmap and palette from a given resource ID
BOOL GetBitmapAndPalette(UINT nIDResource, CBitmap *bitmap, CPalette *pal)
{
	LPCTSTR lpszResourceName = (LPCTSTR)nIDResource;
	
	return(GetBitmapAndPalette(lpszResourceName,bitmap,pal));
}


// Loads a bitmap and palette from a given resource name
BOOL GetBitmapAndPalette(LPCTSTR lpszResourceName, CBitmap *bitmap, CPalette *pal)
{
	// Make sure a bitmap is given
	if(bitmap==NULL) return TRUE;

	// Load in the bitmap resource
	HBITMAP hBmp = (HBITMAP)::LoadImage( AfxGetResourceHandle(), 
			lpszResourceName, IMAGE_BITMAP, 0,0, LR_CREATEDIBSECTION );

	if( hBmp == NULL )
		return FALSE;

	// Delete any attached objects from the bitmap
	if(!bitmap->DeleteObject()) {
		//OutputDebugString("DeleteObject() failed.\n");
	}

	// Attach the bitmap handle to the bitmap
	bitmap->Attach( hBmp );

	// If no palette is desired, get outta here
	if(pal==NULL) return TRUE;

	// Delete any attached objects from the palette
	if(!pal->DeleteObject()) {
		//OutputDebugString("DeleteObject() failed.\n");
	}
	
	// Create a logical palette for the bitmap
	DIBSECTION ds;
	BITMAPINFOHEADER &bmInfo = ds.dsBmih;
	bitmap->GetObject( sizeof(ds), &ds );

	int nColors = bmInfo.biClrUsed ? bmInfo.biClrUsed : 1 << bmInfo.biBitCount;

	// Create a halftone palette if colors > 256. 
	CClientDC dc(NULL);			// Desktop DC
	
	if( nColors > 256 )
		pal->CreateHalftonePalette( &dc );
	else	{		// Create the palette
		RGBQUAD *pRGB = new RGBQUAD[nColors];
		if(pRGB==NULL)
			return FALSE;

		CDC memDC;
		CBitmap *old_bitmap=NULL;

		memDC.CreateCompatibleDC(&dc);
		old_bitmap=memDC.SelectObject( bitmap );
		::GetDIBColorTable( memDC, 0, nColors, pRGB );

		if(old_bitmap!=NULL)
			memDC.SelectObject(old_bitmap);

		if(memDC.DeleteDC()==0)
			OutputDebugString("DeleteDC() failed!\n");

		UINT nSize = sizeof(LOGPALETTE) + (sizeof(PALETTEENTRY) * 256);
		LOGPALETTE *pLP = (LOGPALETTE *) new BYTE[nSize];
		if(pLP==NULL) {
			delete[] pRGB;
			return FALSE;
		}

		pLP->palVersion = 0x300;
		pLP->palNumEntries = 256;

		// Set the first 10 entries to default windows colors
		SetPaletteEntry(&(pLP->palPalEntry[0]),0  ,0  ,0  );	// Black
		SetPaletteEntry(&(pLP->palPalEntry[1]),128,0  ,0  );	// Dark Red
		SetPaletteEntry(&(pLP->palPalEntry[2]),0  ,128,0  );	// Dark Green
		SetPaletteEntry(&(pLP->palPalEntry[3]),128,128,0  );	// Dark Yellow
		SetPaletteEntry(&(pLP->palPalEntry[4]),0  ,0  ,128);	// Dark Blue
		SetPaletteEntry(&(pLP->palPalEntry[5]),128,0  ,128);	// Dark Magenta
		SetPaletteEntry(&(pLP->palPalEntry[6]),0  ,128,128);	// Dark Cyan
		SetPaletteEntry(&(pLP->palPalEntry[7]),192,192,192);	// Light Gray
		SetPaletteEntry(&(pLP->palPalEntry[8]),192,220,192);	// "money" Green
		SetPaletteEntry(&(pLP->palPalEntry[9]),166,202,240);	// "sky" Blue

		// Set the middle 236 entries to the first 236 entries of the palette
		for( int i=0; i < 236; i++)		{
			if( i < nColors ) {
				pLP->palPalEntry[i+10].peRed = pRGB[i].rgbRed;
				pLP->palPalEntry[i+10].peGreen = pRGB[i].rgbGreen;
				pLP->palPalEntry[i+10].peBlue = pRGB[i].rgbBlue;
				pLP->palPalEntry[i+10].peFlags = 0;
			}
			else {
				SetPaletteEntry(&(pLP->palPalEntry[i+10]),0,0,0);
			}
		}

		// Set the last 10 entries to default windows colors
		SetPaletteEntry(&(pLP->palPalEntry[246]),255,251,240);	// Cream
		SetPaletteEntry(&(pLP->palPalEntry[247]),160,160,164);	// Medium Gray
		SetPaletteEntry(&(pLP->palPalEntry[248]),128,128,128);	// Dark Gray
		SetPaletteEntry(&(pLP->palPalEntry[249]),255,0  ,0  );	// Red
		SetPaletteEntry(&(pLP->palPalEntry[250]),0  ,255,0  );	// Green
		SetPaletteEntry(&(pLP->palPalEntry[251]),255,255,0  );	// Yellow
		SetPaletteEntry(&(pLP->palPalEntry[252]),0  ,0  ,255);	// Blue
		SetPaletteEntry(&(pLP->palPalEntry[253]),255,0  ,255);	// Magenta
		SetPaletteEntry(&(pLP->palPalEntry[254]),0  ,255,255);	// Cyan
		SetPaletteEntry(&(pLP->palPalEntry[255]),255,255,255);	// White

		// Create the palette
		pal->CreatePalette( pLP );

		delete[] pLP;
		delete[] pRGB;
	}
	return TRUE;
}


#define MAX_MESSAGES 10	// max messages to process during a message deferral

// Processes and waiting messages
void DeferMessages(void) 
{
	MSG msg;
	for (	int MsgCount = MAX_MESSAGES;
			MsgCount && PeekMessage( &msg, NULL, 0, 0, PM_REMOVE );
			MsgCount--) {
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}
}



// Function to see if user needs Glu32.dll
bool ProcessGlu32DLL(void)
{
	// First, see if Glu32.xyz is even in the directory
	if(_access("Glu32.xyz",0) == -1) return FALSE;

	// Second, see if user's system already has access to a Glu32.dll
	HINSTANCE dll_handle;
	dll_handle=LoadLibrary("Glu32.dll");
	if(dll_handle!=NULL) {
		FreeLibrary(dll_handle);
		DeleteFile("Glu32.xyz");
		return FALSE;
	}

	// Ok, user doesn't have it, so rename ours
	rename("Glu32.xyz","Glu32.dll");
	return TRUE;
}


// Checks to see if a new version text file exists (such a file should 
// only be created if the user does a manual patch update).  If one is
// found, the version info is read from it.  If this version is newer
// than the version stored in the registry, it is then written into
// the registry as the new game version.    
bool ProcessUpdatedVersionFile(void) 
{
	FILE *f;
	uint32_t cur_major, cur_minor, cur_build;
	uint32_t new_major, new_minor, new_build;
	char buffer[_MAX_PATH+1];
	char verbuffer[_MAX_PATH+1];

	// See if the updated version text file exists
	f = fopen(UPDATED_VERSION_FNAME, "rt");
	if(f == NULL){
		return FALSE;
	}

	// Read in the Version info from the text file
	new_major=0; 
	new_minor=0; 
	new_build=0;
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

		// Read in the version line
		strcpy(verbuffer, buffer);
		sscanf(verbuffer, "%i %i %i", &new_major, &new_minor, &new_build);
	}
	fclose(f);

	if(VER(new_major,new_minor,new_build)==0) {
		DeleteFile(UPDATED_VERSION_FNAME);
		return FALSE;
	}

	// Read in the current version values from the registry
	cur_major = os_config_read_uint("Version", "Major", 0);
	cur_minor = os_config_read_uint("Version", "Minor", 0);
	cur_build = os_config_read_uint("Version", "Build", 0);

	// Is the version found in the text file newer than the current version?
	if( VER(new_major,new_minor,new_build) <= VER(cur_major,cur_minor,cur_build) ) {
		DeleteFile(UPDATED_VERSION_FNAME);
		return FALSE;
	}

	// Write the new version into the registry
	os_config_write_uint("Version", "Major", new_major);
	os_config_write_uint("Version", "Minor", new_minor);
	os_config_write_uint("Version", "Build", new_build);

	// Get rid of the file
	DeleteFile(UPDATED_VERSION_FNAME);

	return TRUE;
}


// This function simply returns the interger version value of DirectX
// installed on the user's system
int GetDirectXVersion(void)
{
	LONG lResult;
	HKEY hKey = NULL;

	int version_num=0;
	if (Dd_dll_handle) {
		lResult = RegOpenKeyEx(
			HKEY_LOCAL_MACHINE,					// Where it is
			"Software\\Microsoft\\DirectX",	// name of key
			NULL,										// DWORD reserved
			KEY_QUERY_VALUE,						// Allows all changes
			&hKey										// Location to store key
		);

		if (lResult == ERROR_SUCCESS) {
			char version[32];
			DWORD dwType, dwLen;

			dwLen = 32;
			lResult = RegQueryValueEx(
				hKey,									// Handle to key
				"Version",							// The values name
				NULL,									// DWORD reserved
				&dwType,								// What kind it is
				(uint8_t *) version, 				// value to set
				&dwLen								// How many bytes to set
			);

			if (lResult == ERROR_SUCCESS) {
				CString str;

				version_num = atoi(strstr(version, ".") + 1);

			} else {
				int val;
				DWORD dwType, dwLen;

				dwLen = 4;
				lResult = RegQueryValueEx(
					hKey,									// Handle to key
					"InstalledVersion",				// The values name
					NULL,									// DWORD reserved
					&dwType,								// What kind it is
					(uint8_t *) &val,					// value to set
					&dwLen								// How many bytes to set
				);

				if (lResult == ERROR_SUCCESS) {
					CString str;

					version_num = val;

				}
			}

			RegCloseKey(hKey);
		}
	}

	return(version_num);
}



typedef int (CALLBACK *LPFN_DIRECTXSETUPGETVERSION)(DWORD *, DWORD *); 

// Attempts to obtain the DirectX version and revision number via the dsetup.dll
bool GetDirectXVersionViaDLL(DWORD *version, DWORD *revision) 
{
	HINSTANCE dsetup_dll_handle=NULL;
	LPFN_DIRECTXSETUPGETVERSION pfn_DirectXSetupGetVersion=NULL;

	version=0;
	revision=0;

	// Check for the DSetup dll, and open it
	dsetup_dll_handle=LoadLibrary("dsetup.dll");
	if(dsetup_dll_handle==NULL) {
		return FALSE;
	}

	// Get the DirectXSetup function
	pfn_DirectXSetupGetVersion = (LPFN_DIRECTXSETUPGETVERSION)GetProcAddress(dsetup_dll_handle,"DirectXSetupGetVersionA");
	if(pfn_DirectXSetupGetVersion==NULL) {
		FreeLibrary(dsetup_dll_handle);
		dsetup_dll_handle=NULL;
		return FALSE;
	}

	int rc=pfn_DirectXSetupGetVersion(version,revision);

	FreeLibrary(dsetup_dll_handle);
	dsetup_dll_handle=NULL;

	if(rc==0) return FALSE;
	return TRUE;
}


// Converts the given string to lower case
void StringToLower(char *string)
{
	int j;

	for(j=0;string[j]!='\0';j++)
		if(isalpha(string[j]))
			string[j]=tolower(string[j]);
}


// Set the launcher dialog title
void SetLauncherTitleString(void)
{
#if defined(DEMO)
	szTitle.Format(IDS_D3LAUNCH_DEMO_DLGTITLE);
#elif defined(DEMO2)
	szTitle.Format(IDS_D3LAUNCH_DEMO2_DLGTITLE);
#elif defined(OEM_GENERIC)
	szTitle.Format(IDS_D3LAUNCH_OEM_DLGTITLE);
#elif (defined(OEM_VOODOO3) && !defined(USE_ALL_VIDEO_OPTIONS))
	szTitle.Format(IDS_D3LAUNCH_OEMV3_DLGTITLE);
#elif (defined(OEM_VOODOO3) && defined(USE_ALL_VIDEO_OPTIONS))
	szTitle.Format(IDS_D3LAUNCH_OEM_DLGTITLE);
#elif defined(OEM_KATMAI)
	szTitle.Format(IDS_D3LAUNCH_OEMKM_DLGTITLE);
#else
	szTitle.Format(IDS_D3LAUNCH_DLGTITLE);
#endif
}


// Starts up the online registration program
void Register(bool wait_until_done)
{
	char original_path[_MAX_PATH+1];
	char ereg_path[_MAX_PATH+1];

	_getcwd(original_path, _MAX_PATH);
	strcpy(ereg_path,original_path);
	strcat(ereg_path,"\\ereg");
	_chdir(ereg_path);

	// launch the ereg app
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	
	memset( &si, 0, sizeof(STARTUPINFO) );
	si.cb = sizeof(si);

	BOOL ret = CreateProcess(	 "reg32a.exe",							// pointer to name of executable module 
										 NULL,									// pointer to command line string
										 NULL,									// pointer to process security attributes 
										 NULL,									// pointer to thread security attributes 
										 FALSE,									// handle inheritance flag 
										 CREATE_DEFAULT_ERROR_MODE,		// creation flags 
										 NULL,									// pointer to new environment block 
										 NULL,									// pointer to current directory name 
										 &si,										// pointer to STARTUPINFO 
										 &pi 										// pointer to PROCESS_INFORMATION  
									);			

	// If it was started ok, wait for it to finish before continuing
	if(ret && wait_until_done) {
		WaitForSingleObject (pi.hProcess, INFINITE);
	}

	SetCurrentDirectory(original_path);
}


// Copies the appropriate help files into the root directory based
// upon what language is currently selected
void CopyHelpFiles(bool should_overwrite)
{
	char src_hlp_fname[_MAX_PATH+1];
	char src_cnt_fname[_MAX_PATH+1];
	char hlp_filename[_MAX_PATH+1];
	char cnt_filename[_MAX_PATH+1];
	char hlp_ext[_MAX_EXT+1];
	char cnt_ext[_MAX_EXT+1];

	// Check if files already exist
	if(!should_overwrite) {
		if(_access(HELP_HLP_FNAME,0x00)!=-1 && _access(HELP_CNT_FNAME,0x00)!=-1)
			return;
	}

	// Build the source help filenames
	_splitpath(HELP_HLP_FNAME,NULL,NULL,hlp_filename,hlp_ext);
	_splitpath(HELP_CNT_FNAME,NULL,NULL,cnt_filename,cnt_ext);

	int lang_type=os_config_read_uint(szSectionName,"LanguageType",LANGUAGE_ENGLISH);
	switch(lang_type) {
		case LANGUAGE_FRENCH:
			sprintf(src_hlp_fname,"%s%s%s%s",LANGUAGE_HELP_PATH,hlp_filename,FRENCH_EXT,hlp_ext);
			sprintf(src_cnt_fname,"%s%s%s%s",LANGUAGE_HELP_PATH,cnt_filename,FRENCH_EXT,cnt_ext);
			break;
		case LANGUAGE_GERMAN:
			sprintf(src_hlp_fname,"%s%s%s%s",LANGUAGE_HELP_PATH,hlp_filename,GERMAN_EXT,hlp_ext);
			sprintf(src_cnt_fname,"%s%s%s%s",LANGUAGE_HELP_PATH,cnt_filename,GERMAN_EXT,cnt_ext);
			break;
		case LANGUAGE_ITALIAN:
			sprintf(src_hlp_fname,"%s%s%s%s",LANGUAGE_HELP_PATH,hlp_filename,ITALIAN_EXT,hlp_ext);
			sprintf(src_cnt_fname,"%s%s%s%s",LANGUAGE_HELP_PATH,cnt_filename,ITALIAN_EXT,cnt_ext);
			break;
		case LANGUAGE_SPANISH:
			sprintf(src_hlp_fname,"%s%s%s%s",LANGUAGE_HELP_PATH,hlp_filename,SPANISH_EXT,hlp_ext);
			sprintf(src_cnt_fname,"%s%s%s%s",LANGUAGE_HELP_PATH,cnt_filename,SPANISH_EXT,cnt_ext);
			break;
		default:
			sprintf(src_hlp_fname,"%s%s%s",LANGUAGE_HELP_PATH,hlp_filename,hlp_ext);
			sprintf(src_cnt_fname,"%s%s%s",LANGUAGE_HELP_PATH,cnt_filename,cnt_ext);
			break;
	}

	// If any of the source files don't exist, get outta here
	if(_access(src_hlp_fname,0x00)==-1 || _access(src_cnt_fname,0x00)==-1) {
		// Try the english files instead
		sprintf(src_hlp_fname,"%s%s%s",LANGUAGE_HELP_PATH,hlp_filename,hlp_ext);
		sprintf(src_cnt_fname,"%s%s%s",LANGUAGE_HELP_PATH,cnt_filename,cnt_ext);
		if(_access(src_hlp_fname,0x00)==-1 || _access(src_cnt_fname,0x00)==-1)
			return;
	}

	// Delete current help files (and their created files)
	DeleteFile(HELP_HLP_FNAME);
	DeleteFile(HELP_CNT_FNAME);
	DeleteFile(HELP_GID_FNAME);
	DeleteFile(HELP_FTS_FNAME);

	// Copy over the correct files
	CopyFile(src_hlp_fname,HELP_HLP_FNAME,FALSE);
	CopyFile(src_cnt_fname,HELP_CNT_FNAME,FALSE);
}


// Returns true if this machine can support the CPUID instruction
bool SupportsCPUID ()
{
	bool enabled=true;

	__try{
		
		_asm{
			pushad
			__emit 0x0f  //CPUID
			__emit 0xa2	 //CPUID
			popad
		}
	}
	__except(1)
	{
		enabled=false;
	}

	return enabled;
}

// Returns true if this machine can support katmai instructions
bool SupportsKatmai ()
{
	int result=0;
	

	if(SupportsCPUID())
	{
		_asm
		{
			pushad
			xor	eax,eax;
			inc eax;
			__emit 0x0f  //CPUID
			__emit 0xa2	 //CPUID
					
			and edx,0x02000000;
			je  THE_END;

			inc result;
					
			THE_END:				
				popad
		}

		if(!result)
			return 0;
	}
	else
		return 0;

	return 1;
}


// Removes any whitespace padding from the end of a string
void RemoveTrailingWhitespace(char *s)
{
	int last_char_pos;

	last_char_pos=strlen(s)-1;
	while(last_char_pos>=0 && isspace(s[last_char_pos])) {
		s[last_char_pos]='\0';
		last_char_pos--;
	}
}

// Returns a pointer to the first non-whitespace char in given string
char *SkipInitialWhitespace(char *s)
{
	while((*s)!='\0' && isspace(*s)) 
		s++;

	return(s);
}

// Parses the launcher config file and sets appropriate values
void ReadConfigFile(void)
{
	char path[_MAX_PATH+1];
	char filebuffer[4096+1];
	char *line, *parm;
	FILE *f;
	bool done;

	// Setup Config Defaults
	LanguageSelectionEnabled=TRUE;
	EregEnabled=FALSE;

	// Try and open the config file
	sprintf(path,"%s%s",LANGUAGE_LAUNCHER_PATH,LAUNCHER_CONFIG_FNAME);
	f = fopen(path, "rt");
	if(f == NULL) return;

	// Read in config file
	done=FALSE;
	while (!done && !feof(f)) {

		strcpy(filebuffer,"");
		fgets(filebuffer, 4096, f);

		// Get rid of whitespace padding
		RemoveTrailingWhitespace(filebuffer);
		line=SkipInitialWhitespace(filebuffer);
		
		// If it's an empty line or a comment, skip it
		if(strlen(line)==0 || strncmp(line,"//",2)==0)
			continue;
		
		// split off line's parameter
		line=strtok(line,"=");
		if(line==NULL) continue;
		parm=strtok(NULL,"");
		if(parm==NULL) continue;

		RemoveTrailingWhitespace(line);
		parm=SkipInitialWhitespace(parm);
		
		// Process config file directives
		if(stricmp(line,"LANGUAGE_SELECTION")==0) {
			if(stricmp(parm,"ON")==0) {
				LanguageSelectionEnabled=TRUE;
			}
			else {
				LanguageSelectionEnabled=FALSE;
			}
		}
		else if(stricmp(line,"EREG")==0) {
			if(stricmp(parm,"ON")==0) {
				EregEnabled=TRUE;
			}
			else {
				EregEnabled=FALSE;
			}
		}
	}
	fclose(f);
}


// Constructs a path in the local file system's syntax
// 	newPath: stores the constructed path
//  absolutePathHeader: absolute path on which the sub directories will be appended
//						(specified in local file system syntax)
//  takes a variable number of subdirectories which will be concatenated on to the path
//		the last argument in the list of sub dirs *MUST* be NULL to terminate the list
void ddio_MakePath(char* newPath, const char* absolutePathHeader, const char* subDir, ...)
{
	const char	delimiter = '\\';
	va_list		args;
	char*		currentDir = NULL;
	int			pathLength = 0;
	
	assert(newPath);
	assert(absolutePathHeader);
	assert(subDir);
		
	if (newPath != absolutePathHeader)
	{
		strcpy(newPath, absolutePathHeader);
	}

	// Add the first sub directory
	pathLength = strlen(newPath);
	if (newPath[pathLength - 1] != delimiter)
	{
		newPath[pathLength] = delimiter;		// add the delimiter
		newPath[pathLength+1] = 0;				// terminate the string
	}
	strcat(newPath, subDir);		
	
	// Add the additional subdirectories
	va_start(args, subDir);
		while ((currentDir = va_arg(args, char*)) != NULL)
		{
			pathLength = strlen(newPath);
			if (newPath[pathLength - 1] != delimiter)
			{
				newPath[pathLength] = delimiter;		// add the delimiter
				newPath[pathLength+1] = 0;				// terminate the string
			}
			strcat(newPath, currentDir);		
		}
	va_end(args);	
}


// Split a pathname into its component parts
void ddio_SplitPath(const char* srcPath, char* path, char* filename, char* ext)
{
	char drivename[_MAX_DRIVE], dirname[_MAX_DIR];

	_splitpath(srcPath, drivename, dirname, filename, ext);

	if (path) 
		sprintf(path, "%s%s", drivename, dirname);
}

