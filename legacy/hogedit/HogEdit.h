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

 * $Logfile: /DescentIII/Main/hogedit/HogEdit.h $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:57:56 $
 * $Author: kevinb $
 *
 *
 *
 * $Log: not supported by cvs2svn $
 *
 * 10    10/30/98 11:15a Nate
 * Added support for modification of hog files.
 *
 * 9     10/28/98 11:24a Nate
 * Added command line auto-processing (create hog from a rib)
 *
 * 8     9/17/98 4:29p Nate
 * Added Import Directory option.
 *
 * 7     8/16/98 4:22p Nate
 * Added message deferal and new hog info dialog
 *
 * 6     8/14/98 6:04p Nate
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

// HogEdit.h : main header file for the HOGEDIT application
//

#if !defined(AFX_HOGEDIT_H__48BE52B8_172F_11D2_8CBC_00A0C96ED60D__INCLUDED_)
#define AFX_HOGEDIT_H__48BE52B8_172F_11D2_8CBC_00A0C96ED60D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h" // main symbols

#define HOG_EDIT_TITLE "The Hog Farmer v1.0"

int ModifiedPrompt(CWnd *caller);
void ProcessMessages(void);
void UpdateTotalFiles(int num_files);
void UpdateStatusBar(char *msg);

extern bool DocModified;          // is document modified.
extern CStatusBar *MainStatusBar; // pointer to the main status bar
extern CWaitCursor *WaitCursor;   // pointer to a remote wait cursor

// Globals for automated hog file creation (from a given rib file)
extern CString AutoRibFilename;
extern CString AutoHogFilename;
extern bool DoAutoProcess;
extern bool UseCurrentHogFilename;

/////////////////////////////////////////////////////////////////////////////
// CHogEditApp:
// See HogEdit.cpp for the implementation of this class
//

class CHogEditApp : public CWinApp {
public:
  CHogEditApp();

  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CHogEditApp)
public:
  virtual BOOL InitInstance();
  //}}AFX_VIRTUAL

  // Implementation

  //{{AFX_MSG(CHogEditApp)
  afx_msg void OnAppAbout();
  // NOTE - the ClassWizard will add and remove member functions here.
  //    DO NOT EDIT what you see in these blocks of generated code !
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HOGEDIT_H__48BE52B8_172F_11D2_8CBC_00A0C96ED60D__INCLUDED_)
