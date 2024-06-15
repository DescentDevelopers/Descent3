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

 * $Logfile: /DescentIII/Main/D3Launch/D3Launch.h $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:56:51 $
 * $Author: kevinb $
 *
 *
 *
 * $Log: not supported by cvs2svn $
 *
 * 40    6/18/99 4:42p Nate
 * Added non-LaserLock ROW version
 *
 * 39    6/17/99 1:57p Nate
 * Enabled italian for ROW builds
 *
 * 38    6/09/99 2:46p Nate
 * Minor changes for different builds
 *
 * 37    5/21/99 3:38p Nate
 * Added changes for Rest of World release (new update directory path)
 *
 * 36    5/20/99 12:53p Nate
 * Added GLSetup window, EAX mixer support, and CHFlight+Mouseman Mode
 * options
 *
 * 35    5/19/99 12:29p Nate
 * Fixed openGL crash, changed Network speed default, changed OpenGL
 * detection to default to unchecked, and removed config file parsing from
 * US version
 *
 * 34    5/07/99 11:23a Nate
 * Added support for a launcher config file
 *
 * 33    5/05/99 11:38p Nate
 * Disabled Language selection for US release builds
 *
 * 32    5/02/99 12:38p Nate
 * Disabled GLSetup for final release
 *
 * 31    4/27/99 10:42p Nate
 * Added vsync enabling when direct3d is chosen
 *
 * 30    4/15/99 12:03p Nate
 * Added "Descent 3 Demo 2" build type
 *
 * 29    4/08/99 1:13p Nate
 * Added Pentium III detection
 *
 * 28    3/29/99 3:14p Nate
 * Added OEM_GENERIC
 *
 * 27    3/19/99 10:18a Nate
 * Added OEM_GENERIC compile type
 *
 * 26    3/12/99 3:29p Nate
 * Added more multi-language support
 *
 * 25    3/02/99 5:45p Nate
 * Lots of little changes/fixes
 *
 * 24    2/26/99 12:50p Nate
 * Changed OEM_Voodoo3 names
 *
 * 23    2/24/99 8:37p Nate
 * Various little dialog changes, added "Install Drivers" dialog
 *
 * 22    2/24/99 1:46p Nate
 * Added multi-language support
 *
 * 21    2/17/99 2:23p Nate
 * Added some OEM changes
 *
 * 20    2/05/99 3:51p Nate
 * Added conditional compilation directives for OEM support
 *
 * 19    11/30/98 3:00p Nate
 * Added StringToLower()
 *
 * 18    10/15/98 7:30p Nate
 *
 * 17    10/15/98 11:31a Nate
 * Added Launcher Sound toggling
 *
 * 16    10/08/98 6:23p Nate
 * Fixed a few bugs.
 *
 * 15    10/02/98 7:09p Nate
 *
 * 14    9/30/98 1:59p Nate
 * Added Version constants for demo and full builds
 *
 * 13    9/29/98 6:05p Nate
 * Added the functionality to update the game version from a text file.
 *
 * 12    9/22/98 3:33p Nate
 * Added conditional compiling to help system (can toggle between HTML and
 * standard windows help)
 *
 * 11    9/21/98 6:06p Nate
 *
 * 10    9/21/98 5:40p Nate
 * Incorporated the new HTML help system
 *
 * 9     9/16/98 3:23p Nate
 * Added m_straight_to_setup detection
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

// D3Launch.h : main header file for the D3LAUNCH application
//

#if !defined(AFX_D3LAUNCH_H__9149CF38_207D_11D2_8CBD_00A0C96ED60D__INCLUDED_)
#define AFX_D3LAUNCH_H__9149CF38_207D_11D2_8CBD_00A0C96ED60D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file for PCH
#endif

/******************************
** The Launcher Version ID's **
******************************/
#define LAUNCHER_VERSION_ID 1.0
#define LAUNCHER_DEMO_VERSION_ID 1.0

// Uncomment this if you want to use the HTML help system for the launcher
// #define USE_HTML_HELP_SYSTEM

// Uncomment one of the following if you want to do a special build
// #define DEMO
// #define DEMO2
// #define OEM_GENERIC
// #define OEM_VOODOO3
// #define OEM_KATMAI
// #define FULL_US_RELEASE
#define FULL_ROW_RELEASE
// #define FULL_AUSSIE_RELEASE

// Uncomment this if you are doing an initial UK version build
// #define INITIAL_UK_RELEASE

// Uncomment this if you are doing a non-LaserLock version (full ROW variation)
// #define ROW_NLL_RELEASE

// Uncomment this is you are doing an OEM_VOODOO3 build but want it to support all API's
// #define USE_ALL_VIDEO_OPTIONS

// Uncomment this if you want multi-language support
#define USE_MULTI_LANGUAGES

// Uncomment this if you want Italian to be a selectable language
#define ENABLE_ITALIAN

// Uncomment this if you want GLSetup to be a non-selectable option
// #define DISABLE_GLSETUP

// Macro for computing a "comparable" version ID
#define VER(major, minor, build) (100 * 100 * major + 100 * minor + build)

// Comment char to be used in version files
#define VERSION_FILE_COMMENT_CHAR ';'

#include "resource.h" // main symbols

// Help Files
#define MAIN_MENU_HELP "D3L_hlp1.htm"
#define UPDATE_HELP "D3L_hlp2.htm"
#define DIRECTXTAB_HELP "D3L_hlp3.htm"
#define VIDEOTAB_HELP "D3L_hlp4.htm"
#define AUDIOTAB_HELP "D3L_hlp5.htm"
#define JOYSTICKTAB_HELP "D3L_hlp6.htm"
#define SPEEDTAB_HELP "D3L_hlp7.htm"
#define NETWORKTAB_HELP "D3L_hlp8.htm"
#define KEYBOARDTAB_HELP "D3L_hlp9.htm"

/////////////////////////////////////////////////////////////////////////////
// CD3LaunchApp:
// See D3Launch.cpp for the implementation of this class
//

class CD3LaunchApp : public CWinApp {
public:
  CD3LaunchApp();

  HINSTANCE m_hResInst;
  HINSTANCE m_hDefResInst;

  CBitmap m_bkBmap;
  CPalette m_palette;

  CBitmap m_bkBmapBig;
  int m_straight_to_update; // NOTE :	this is for going right to the autoupdate feature from the PXO screen
                            //			it should only ever be set from the command line parser

  int m_straight_to_setup; // NOTE :	this is for going straight to the setup if this is
                           //			the first time (after D3 is installed) that the
                           //			launcher has been run

  BOOL FirstInstance();

  // parse for special (outrage) command-line arguments
  void OutrageParseCommandLine();

  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CD3LaunchApp)
public:
  virtual BOOL InitInstance();
  virtual int ExitInstance();
  //}}AFX_VIRTUAL

  // Implementation

  //{{AFX_MSG(CD3LaunchApp)
  // NOTE - the ClassWizard will add and remove member functions here.
  //    DO NOT EDIT what you see in these blocks of generated code !
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

extern CD3LaunchApp theApp;

void help_launch(char *help_filename);
void url_launch(char *url);
BOOL PlayResource(LPSTR lpName, bool WaitUntilDone);
BOOL GetBitmapAndPalette(UINT nIDResource, CBitmap *bitmap, CPalette *pal);
BOOL GetBitmapAndPalette(LPCTSTR lpszResourceName, CBitmap *bitmap, CPalette *pal);
void DeferMessages(void);
int GetDirectXVersion(void);
bool GetDirectXVersionViaDLL(DWORD *version, DWORD *revision);
void StringToLower(char *string);
void SetLauncherTitleString(void);
void Register(bool wait_until_done);
void CopyHelpFiles(bool should_overwrite);
void ReadConfigFile(void);

bool SupportsCPUID();
bool SupportsKatmai();

extern bool LanguageSelectionEnabled;
extern bool EregEnabled;

extern int LauncherSoundEnabled;
extern bool VideoCardsDetected;
extern bool DetailLevelConfigured;
extern bool NewLanguageSelected;
extern bool RenderersDetected;
extern bool GlideInited;
extern int Dx_version;
extern HINSTANCE Dd_dll_handle;
extern HINSTANCE opengl_dll_handle;

extern double DlgWidthModifier;
extern double DlgHeightModifier;

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_D3LAUNCH_H__9149CF38_207D_11D2_8CBD_00A0C96ED60D__INCLUDED_)
