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
 */

// FilePageAddDlg.cpp : implementation file
//

#include "stdafx.h"
#include "editor.h"
#include "FilePageAddDlg.h"
#include "ddio.h"
#include "mem.h"
#include <direct.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFilePageAddDlg dialog

CFilePageAddDlg::CFilePageAddDlg(CWnd *pParent /*=NULL*/) : CDialog(CFilePageAddDlg::IDD, pParent) {
  //{{AFX_DATA_INIT(CFilePageAddDlg)
  // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT
  m_NumberOfSelectedFiles = 0;
  m_SelectedFiles = NULL;
  m_SelectedDestDir = NULL;
  initial_path = "";
}

CFilePageAddDlg::~CFilePageAddDlg(void) {
  if (m_SelectedFiles) {
    for (int i = 0; i < m_NumberOfSelectedFiles; i++) {
      if (m_SelectedFiles[i])
        mem_free(m_SelectedFiles[i]);
    }
    mem_free(m_SelectedFiles);
  }

  if (m_SelectedDestDir) {
    mem_free(m_SelectedDestDir);
  }
}

void CFilePageAddDlg::SetInitialPath(char *path) {
  char olddir[_MAX_PATH];
  ddio_GetWorkingDir(olddir, _MAX_PATH);
  if (ddio_SetWorkingDir(path)) {
    if (path[1] == ':')
      initial_path = path;
  }
  ddio_SetWorkingDir(olddir);
}

void CFilePageAddDlg::DoDataExchange(CDataExchange *pDX) {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CFilePageAddDlg)
  // NOTE: the ClassWizard will add DDX and DDV calls here
  //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CFilePageAddDlg, CDialog)
//{{AFX_MSG_MAP(CFilePageAddDlg)
ON_CBN_SELCHANGE(IDC_LIST_DRIVES, OnSelchangeListDrives)
ON_LBN_SELCHANGE(IDC_LIST_FOLDERS, OnSelchangeListFolders)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFilePageAddDlg message handlers

// CFileList

CFileList::CFileList() {}

CFileList::~CFileList() {}

BEGIN_MESSAGE_MAP(CFileList, CListBox)
//{{AFX_MSG_MAP(CFileList)
ON_CONTROL_REFLECT(LBN_DBLCLK, OnDblclk)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFileList message handlers

// fills the list-box with the appropriate information
// either files, directories or drives
// in this case system and/or hidden files are not shown
BOOL CFileList::FillBox() {
  WIN32_FIND_DATA FileData;
  HANDLE hSearch;
  CString csString;
  BOOL check_dir;

  SetRedraw(FALSE);

  if (m_nFlag == DRIVES) {
    Dir(0x4000, m_csPath);
  } else {
    // resets the list-box; parent-function
    ResetContent();

    // construct the path and mask for displaying
    csString = m_csPath;
    csString += m_csFileMask;

    hSearch = FindFirstFile(csString, &FileData);
    if (hSearch == INVALID_HANDLE_VALUE)
      return FALSE;

    check_dir = (m_nFlag == DIRECTORIES ? TRUE : FALSE);

    CheckEntry(FileData.cFileName, FileData.dwFileAttributes, check_dir);

    while (1) {
      if (!FindNextFile(hSearch, &FileData)) {

        SetRedraw();
        InvalidateRect(NULL);
        // Close the search handle.
        if (!FindClose(hSearch))
          return FALSE;

        return TRUE;
      } else
        CheckEntry(FileData.cFileName, FileData.dwFileAttributes, check_dir);
    }
  }

  SetRedraw();
  InvalidateRect(NULL);

  return TRUE;
}

BOOL CFileList::InitBox(UINT res, CWnd *parent, LPSTR lpszPath, LPSTR lpszFileMask, UINT flags, UINT msg,
                        void (*cb)(char *)) {
  m_nFlag = flags;
  m_csPath = lpszPath;
  m_csFileMask = lpszFileMask;
  m_nMsg = msg;
  m_cwndParent = parent;
  callback = cb;

  // let's check if a \ is the last letter in the path
  if (m_nFlag == DRIVES || m_nFlag == DIRECTORIES) {
    if (m_csPath.Right(1) != "\\")
      m_csPath += "\\";
  }

  // subclassing the dialog-template listbox
  if (SubclassDlgItem(res, parent) == FALSE)
    return FALSE;

  return FillBox();
}

// by double-clicking .. in a drive-box the
// path will get longer and longer such as
//  c:\inn\pics\..\html\..\..\windows\system\..\..\det.log
//  the proper path in this example should be
//  c:\det.log
void CFileList::OnDblclk() {
  // TODO: Add your control notification handler code here
  CString temp_string, temp_string2;
  GetText(GetCurSel(), temp_string);

  switch (m_nFlag) {
  case DRIVES:
    // is the drive available
    // temp_string2=temp_string;
    temp_string.MakeUpper();

    if (!_chdrive(temp_string[2] - 'A' + 1)) {
      m_csPath.Format("%c:\\", temp_string[2]);
      m_cwndParent->PostMessage(m_nMsg, DRIVES, 0);
    } else {
      SetCurSel(-1);
      temp_string2.Empty();
      temp_string2.Format("Drive %c is not available", temp_string[2]);
      MessageBox(temp_string2);
    }
    break;
  case DIRECTORIES:
    // if we dblclk a .. then we move up one level
    if (temp_string == "..") {
      m_csPath = m_csPath.Left(m_csPath.ReverseFind('\\'));
      m_csPath = m_csPath.Left(m_csPath.ReverseFind('\\'));
      m_csPath += '\\';
    } else
      // otherwise we just add another sub-dir to the path
      m_csPath = m_csPath + temp_string + '\\';

    FillBox();
    // notify mama about an update
    m_cwndParent->PostMessage(m_nMsg, DIRECTORIES, 0);
    break;
  case FILES:
    m_cwndParent->PostMessage(m_nMsg, FILES, 0);
    break;
  }
}

// get and set functions
inline CString CFileList::getPath() { return m_csPath; }

inline CString CFileList::getFileMask() { return m_csFileMask; }

inline void CFileList::setFileMask(CString mask) { m_csFileMask = (LPCTSTR)mask; }

inline BOOL CFileList::RefreshMask(CString mask) {
  m_csFileMask = mask;
  return FillBox();
}

// let's check if we display the right entries
// we don't want any hidden, system, temporary and/or directories (for a file-box)
inline void CFileList::CheckEntry(LPSTR name, DWORD flag, BOOL dir) {
  // we don't want to display a .; that is the current directory, or
  // a ..(previous directory) in a filebox
  if (name[0] == '.')
    if (!dir)
      return;
  if (name[1] == '\0')
    return;

  // we don't want to show any system and/or hidden files/directories
  if (flag & FILE_ATTRIBUTE_HIDDEN)
    return;

  if (flag & FILE_ATTRIBUTE_SYSTEM)
    return;

  // we don't want any directories in a file-box
  if (dir) {
    if (!(flag & FILE_ATTRIBUTE_DIRECTORY))
      return;
  }
  //... or files in a dir-box
  else {
    if (flag & FILE_ATTRIBUTE_DIRECTORY)
      return;
  }

  AddString(name);
}

inline BOOL CFileList::RefreshBox(CString path, CString mask) {
  setPath(path);
  setFileMask(mask);
  return FillBox();
}

inline BOOL CFileList::RefreshBox(CString path) {
  setPath(path);
  return FillBox();
}

inline void CFileList::setPath(CString path) {
  CString temp = path;
  char *buffer;

  // check for valid entry
  if (temp.Right(1) != "\\")
    temp += "\\";

  buffer = (char *)mem_malloc(temp.GetLength() + 1);

  if (!buffer)
    return;

  // ddio_CleanPath(buffer,temp.GetBuffer(0));
  strcpy(buffer, temp.GetBuffer(0));

  m_csPath = buffer;

  if (callback)
    (*callback)(buffer);

  mem_free(buffer);
}

BOOL CFilePageAddDlg::OnInitDialog() {
  CDialog::OnInitDialog();

  // TODO: Add extra initialization here

  // register a personal, user-defined msg so that our boxes can post msg's for
  // interaction
  m_nBoxMsg = RegisterWindowMessage("FilePageDlgAdd");
  // init the  boxes
  // the definition of:
  // BOOL InitBox(UINT res_from_template,
  //	       CWnd *parent,
  //	       LPSTR lpszPath,
  //	       LPSTR lpszFileMask,
  //	       UINT flags,
  //	       UINT msg);

  if (initial_path.IsEmpty())
    initial_path = "c:\\";

  m_cflDrive.InitBox(IDC_LIST_DRIVES, this, initial_path.GetBuffer(0), "*.*", DRIVES, m_nBoxMsg);
  m_cflDir.InitBox(IDC_LIST_FOLDERS, this, initial_path.GetBuffer(0), "*.*", DIRECTORIES, m_nBoxMsg);
  m_cflFile.InitBox(IDC_LIST_FILES, this, initial_path.GetBuffer(0), "*.*", FILES, m_nBoxMsg);

  // Add the drives to the drive list
  CComboBox *combo = (CComboBox *)GetDlgItem(IDC_LIST_DRIVES);
  char *root_drives[256];
  int root_count = ddio_GetFileSysRoots(root_drives, 256);
  int i, c_drive = 0;

  char initial_d[3];
  strncpy(initial_d, initial_path.GetBuffer(0), 2);
  initial_d[2] = '\0';

  for (i = 0; i < root_count; i++) {
    if (!strnicmp(root_drives[i], initial_d, 2))
      c_drive = i;
    combo->AddString(root_drives[i]);
    mem_free(root_drives[i]);
  }
  combo->SetCurSel(c_drive);

  combo = (CComboBox *)GetDlgItem(IDC_DIRS);
  for (i = 0; i < NumDirectoryNames; i++) {
    combo->AddString(DirectoryNames[i]);
  }
  combo->SetCurSel(0);

  return TRUE;
}

LRESULT CFilePageAddDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) {
  // TODO: Add your specialized code here and/or call the base class    ...
  // process the user-defined msg
  if (message == m_nBoxMsg) {
    // update the dir-box and file-box if we change drive
    if (wParam == DRIVES) {
      m_cflDir.RefreshBox(m_cflDrive.getPath());
      m_cflFile.RefreshBox(m_cflDrive.getPath());
    }

    // update the file-box if we change directory
    else if (wParam == DIRECTORIES) {
      m_cflFile.RefreshBox(m_cflDir.getPath());
    }
  }

  return CDialog::WindowProc(message, wParam, lParam);
}

void CFilePageAddDlg::OnSelchangeListDrives() {
  CComboBox *combo = (CComboBox *)GetDlgItem(IDC_LIST_DRIVES);
  int sel = combo->GetCurSel();
  CString drive;

  combo->GetLBText(sel, drive);

  m_cflDrive.setPath(drive);
  m_cflDir.RefreshBox(m_cflDrive.getPath());
  m_cflFile.RefreshBox(m_cflDrive.getPath());
}

void CFilePageAddDlg::OnSelchangeListFolders() {}

bool CFilePageAddDlg::Quit(void) {
  char buffer[_MAX_PATH];
  int cur_sel;
  CComboBox *combo = (CComboBox *)GetDlgItem(IDC_DIRS);
  cur_sel = combo->GetCurSel();
  combo->GetLBText(cur_sel, buffer);

  char temp[1024];
  sprintf(temp, "Are you sure you want to add these files to the data\\%s directory?", buffer);
  if (MessageBox(temp, NULL, MB_YESNO) != IDYES) {
    return false;
  }

  m_SelectedDestDir = mem_strdup(buffer);

  CListBox *list = (CListBox *)GetDlgItem(IDC_LIST_FILES);
  m_NumberOfSelectedFiles = list->GetSelCount();

  if (m_NumberOfSelectedFiles == 0) {
    m_SelectedFiles = NULL;
  } else {

    m_SelectedFiles = (char **)mem_malloc(m_NumberOfSelectedFiles * sizeof(char *));
    if (!m_SelectedFiles) {
      m_SelectedFiles = NULL;
      m_NumberOfSelectedFiles = 0;
      return true;
    }

    int *sel_items = mem_rmalloc<int>(m_NumberOfSelectedFiles);
    if (!sel_items) {
      mem_free(m_SelectedFiles);
      m_SelectedFiles = NULL;
      m_NumberOfSelectedFiles = 0;
      return true;
    }

    list->GetSelItems(m_NumberOfSelectedFiles, sel_items);
    char temp[_MAX_PATH];

    char drive[_MAX_PATH];
    strcpy(drive, m_cflFile.getPath());

    for (int i = 0; i < m_NumberOfSelectedFiles; i++) {
      cur_sel = sel_items[i];

      list->GetText(cur_sel, temp);

      // ummm, figure out real path here
      ddio_MakePath(buffer, drive, temp, NULL);

      m_SelectedFiles[i] = mem_strdup(buffer);
    }
  }

  return true;
}

void CFilePageAddDlg::OnOK() {
  if (!Quit())
    return;

  CDialog::OnOK();
}
