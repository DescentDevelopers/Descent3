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

* $Logfile: /DescentIII/Main/editor/ScriptLevelInterface.h $
* $Revision: 1.1.1.1 $
* $Date: 2003-08-26 03:57:38 $
* $Author: kevinb $
*
* ScriptLevelInterface.h : implementation file
*
* $Log: not supported by cvs2svn $
 *
 * 10    2/17/99 6:54p Jeff
 * added check box for auto check in/out scripts with levels...fix
 * checksum mprintf bug
 *
 * 9     1/11/99 3:34p Jeff
 * added checks for when going editor->game to see if scripts are out of
 * date, if so give the option of breaking out.  Add some options to mass
 * script compiler, along with a toolbar shortcut.
 *
 * 8     1/05/99 4:54p Jeff
 * save the last script edited and set the on init
 *
 * 7     12/20/98 9:57p Jeff
 * added ability to display scripts not checked out.  don't display
 * message on checkin/out
 *
 * 6     12/18/98 12:48p Jeff
 *
 * 5     12/18/98 10:40a Jeff
 * save out settings to registry
 *
 * 4     12/13/98 3:10a Jeff
 * fixed some bugs.  Automatically check in/out/etc .dll when operating
 * with .cpp.  Hooked in compiler and compiler configuration.  Very
 * complete.
 *
 * 3     12/11/98 10:34p Jeff
 * Updated the script/level interface, all manage system integration is
 * complete.  External Script Editor loading is setup too.
 *
 * 2     12/11/98 5:50p Jeff
 * implemented and added changes regarding Level&Scripting manage system
 * and compiler interface
*
* $NoKeywords: $
*/

#pragma once

// ScriptLevelInterface.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CScriptLevelInterface dialog

class CScriptLevelInterface : public CDialog {
  // Construction
public:
  CScriptLevelInterface(CWnd *pParent = NULL); // standard constructor

  // Dialog Data
  //{{AFX_DATA(CScriptLevelInterface)
  enum { IDD = IDD_SCRIPTINTERFACE };
  CCheckListBox m_ListAvailable;
  CCheckListBox m_ListCheckedOut;
  CComboBox m_ScriptToCompile;
  int m_ViewType;
  int m_ScriptType;
  BOOL m_ShowNonCheckedOut;
  BOOL m_AutoCheckScripts;
  //}}AFX_DATA

  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CScriptLevelInterface)
protected:
  virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
                                                   //}}AFX_VIRTUAL

  // Implementation
protected:
  void SetStatus(const char *str);
  void UpdateAvailableWithLevels();
  void UpdateAvailableWithScripts();
  void UpdateCheckedOutWithLevels();
  void UpdateCheckedOutWithScripts();
  void UpdateScriptListWithLevels();
  void UpdateScriptListWithScripts();
  bool DeleteLevel();
  bool DeleteScript();
  bool CheckInScripts();
  bool CheckInLevels();
  bool CheckOutScripts();
  bool CheckOutLevels();
  bool UndoCheckOutScripts();
  bool UndoCheckOutLevels();

  bool InCritical;

  void UpdateDialog();

  bool AddNewLevel(void);
  bool AddNewScript(void);

  // Generated message map functions
  //{{AFX_MSG(CScriptLevelInterface)
  afx_msg void OnCheckout();
  afx_msg void OnCheckin();
  afx_msg void OnUndocheckout();
  afx_msg void OnRadioScripts();
  afx_msg void OnRadioLevels();
  afx_msg void OnCompile();
  afx_msg void OnEditscript();
  afx_msg void OnCloseupSelectedScript();
  virtual BOOL OnInitDialog();
  afx_msg void OnAddnew();
  afx_msg void OnDelete();
  afx_msg void OnCreatescript();
  afx_msg void OnClose();
  afx_msg void OnDestroy();
  afx_msg void OnConfigcompiler();
  virtual void OnOK();
  afx_msg void OnMasscompile();
  afx_msg void OnSelchangeSelectedScript();
  afx_msg void OnListNoncheckedOut();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

// Returns true if there are out of date script files (only checks those in manage system)
int AreScriptsOutofDate(void);
// returns true if the given .cpp file is has an out of date dll (or non-existant).  Working
//	dir MUST be set to data\scripts before calling this function
bool IsScriptOutofDate(char *name);

bool IsScriptOutofSync(const std::filesystem::path &name);

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
