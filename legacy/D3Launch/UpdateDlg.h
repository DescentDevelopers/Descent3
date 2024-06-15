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

 * $Logfile: /DescentIII/Main/D3Launch/UpdateDlg.h $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:56:51 $
 * $Author: kevinb $
 *
 *
 *
 * $Log: not supported by cvs2svn $
 *
 * 10    11/03/98 1:04p Nate
 * Added label class for user message.
 *
 * 9     9/29/98 6:06p Nate
 * Moved some of the version related defines and macros to D3Launch.h
 *
 * 8     9/21/98 5:40p Nate
 * Incorporated the new HTML help system
 *
 * 7     9/09/98 1:05p Nate
 * Added Cleanup()
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

#if !defined(AFX_UPDATEDLG_H__FFEFED62_26D1_11D2_8CBD_00A0C96ED60D__INCLUDED_)
#define AFX_UPDATEDLG_H__FFEFED62_26D1_11D2_8CBD_00A0C96ED60D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// UpdateDlg.h : header file
//

#include "PsTypes.h"
#include "MessageWindowDlg.h"
#include "Label.h"

/////////////////////////////////////////////////////////////////////////////
// CUpdateDlg dialog

// Return values for UpdateTheLauncher()
#define LAUNCHER_REPLACE_NONE 0
#define LAUNCHER_REPLACE_ERROR 1
#define LAUNCHER_REPLACE_NEEDED 2
#define LAUNCHER_REPLACE_ACCESS_ERROR 3
#define LAUNCHER_REPLACE_CREATEPROC_ERROR 4

// Return values for GetHTTPFile
#define GETHTTPFILE_OK 0
#define GETHTTPFILE_UNKNOWN_ERROR 1
#define GETHTTPFILE_CANCELLED 2
#define GETHTTPFILE_BADPARMS 3
#define GETHTTPFILE_CANT_WRITE_FILE 4
#define GETHTTPFILE_CANT_PARSE_URL 5
#define GETHTTPFILE_BAD_FILE_OR_DIR 6
#define GETHTTPFILE_HOST_NOT_FOUND 7
#define GETHTTPFILE_NO_MEMORY 8

class CUpdateDlg : public CDialog {
  // Construction
public:
  int m_should_verify; // should actually go through the verification process

  void StatusBar(UINT strID);
  void StatusBar(char *status);
  void LocalizedMessageBox(UINT msgID, UINT titleID, UINT type = MB_OK);
  void AddToList(UINT strID);
  void CloseHTTP();
  BOOL InitHTTP();
  void DebugSpew(char *);
  void DebugSpew(CString &msg);
  int GetHTTPFile(char *remote, char *local);
  BOOL ApplyPatch();
  int UpdateTheLauncher();
  void GetHTTPFileErrorBox(int error_id, char *remote, char *local);
  BOOL ParseMOTDFile(void);
  void DisplayVersions(int maj1, int min1, int bld1, int maj2 = -1, int min2 = -1, int bld2 = -1);
  void Cleanup(void);

  CUpdateDlg(CWnd *pParent = NULL, int should_verify = 1); // standard constructor

  CFont m_listfont; // font for the update feedback list
  CMessageWindowDlg msg_dlg;

  // Dialog Data
  //{{AFX_DATA(CUpdateDlg)
  enum { IDD = IDD_UPDATE_DIALOG };
  CLabel m_UserMsgStatic;
  CEdit m_LatestVersionEdit;
  CEdit m_YourVersionEdit;
  CEdit m_MOTDBox;
  CProgressCtrl m_progress;
  CListBox m_list;
  CString m_statusbar;
  CString m_usermsg;
  //}}AFX_DATA

  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CUpdateDlg)
protected:
  virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
                                                   //}}AFX_VIRTUAL

  // Implementation
protected:
  BOOL GetVersionNfo();
  BOOL GetMOTD();

  // User's current version
  uint32_t UsrMajor, UsrMinor, UsrBuild;

  // Newest version available
  uint32_t NewMajor, NewMinor, NewBuild;

  // Generated message map functions
  //{{AFX_MSG(CUpdateDlg)
  afx_msg void OnYes();
  virtual void OnCancel();
  virtual BOOL OnInitDialog();
  afx_msg void OnClose();
  afx_msg LRESULT OnCommandHelp(WPARAM wParam, LPARAM lParam);
  afx_msg BOOL OnHelpInfo(HELPINFO *pHelpInfo);
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

extern bool CancelPressed; // indicates that the user has pressed cancel button

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UPDATEDLG_H__FFEFED62_26D1_11D2_8CBD_00A0C96ED60D__INCLUDED_)
