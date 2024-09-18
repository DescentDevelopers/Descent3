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

// FilePageDialog.cpp : implementation file
//

#include "stdafx.h"
#include "editor.h"
#include "ddio.h"
#include "gamefile.h"
#include "gamefilepage.h"
#include "FilePageDialog.h"
#include "EditLineDialog.h"
#include "pserror.h"
#include "mem.h"
#include "mono.h"
#include "FilePageAddDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

char *DirectoryNames[] = {"Graphics", "Rooms", "Sounds",  "Models", "Levels", "Briefings",
                          "Misc",     "Art",   "Scripts", "Music",  "Voice"};
int NumDirectoryNames = sizeof(DirectoryNames) / sizeof(char *);

/////////////////////////////////////////////////////////////////////////////
// CFilePageDialog dialog

CFilePageDialog::CFilePageDialog(CWnd *pParent /*=NULL*/) : CDialog(CFilePageDialog::IDD, pParent) {
  //{{AFX_DATA_INIT(CFilePageDialog)
  m_DirectoryFilter = _T("");
  //}}AFX_DATA_INIT
}

void CFilePageDialog::DoDataExchange(CDataExchange *pDX) {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CFilePageDialog)
  DDX_Control(pDX, IDC_CHECKEDOUT_LIST, m_CheckedOutListCtrl);
  DDX_Control(pDX, IDC_FILELIST, m_FileListCtrl);
  DDX_CBString(pDX, IDC_DIRECTORY_FILTER, m_DirectoryFilter);
  //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CFilePageDialog, CDialog)
//{{AFX_MSG_MAP(CFilePageDialog)
ON_BN_CLICKED(IDC_ADD_FILE, OnAddFile)
ON_BN_CLICKED(IDC_CHECKIN_FILE, OnCheckinFile)
ON_BN_CLICKED(IDC_DELETE_FILE, OnDeleteFile)
ON_BN_CLICKED(IDC_LOCK_FILE, OnLockFile)
ON_BN_CLICKED(IDC_OVERRIDE, OnOverride)
ON_BN_CLICKED(IDC_FILES_OUT, OnFilesOut)
ON_CBN_SELCHANGE(IDC_DIRECTORY_FILTER, OnSelchangeDirectoryFilter)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFilePageDialog message handlers

void CFilePageDialog::OnAddFile() {
  if (!Network_up) {
    OutrageMessageBox("Sorry babe, the network is down.  This action is a no-no.\n");
    return;
  }

  CFilePageAddDlg dlg;

  dlg.SetInitialPath(LocalD3Dir);
  if (dlg.DoModal() != IDOK)
    return;

  char cur_name[100];
  char pathname[100], name[100], extension[100];
  int gamefile_handle;

  if (dlg.m_NumberOfSelectedFiles > 0) {
    ASSERT(dlg.m_SelectedFiles);
    ASSERT(dlg.m_SelectedDestDir);
  }

  for (int q = 0; q < dlg.m_NumberOfSelectedFiles; q++) {
    ASSERT(dlg.m_SelectedFiles[q]);
    if (!dlg.m_SelectedFiles[q])
      continue;

    //	Okay, we selected a file. Lets do what needs to be done here.
    ddio_SplitPath(dlg.m_SelectedFiles[q], pathname, name, extension);
    sprintf(cur_name, "%s%s", name, extension);

    if ((FindGamefileName(cur_name)) != -1) {
      char buffer[512];
      sprintf(buffer, "%s already exists in the manage system", cur_name);
      OutrageMessageBox(buffer);
      continue;
    }

    gamefile_handle = AllocGamefile();
    strcpy(Gamefiles[gamefile_handle].name, cur_name);
    strcpy(Gamefiles[gamefile_handle].dir_name, dlg.m_SelectedDestDir);

    // Finally, save a local copy of the model/anim and alloc a tracklock
    mprintf(0, "Making a copy of '%s' locally...\n", cur_name);

    char destname[100];
    ddio_MakePath(destname, LocalD3Dir, "data", dlg.m_SelectedDestDir, Gamefiles[gamefile_handle].name, NULL);
    if (stricmp(destname, dlg.m_SelectedFiles[q])) // only copy if they are different
      cf_CopyFile(destname, dlg.m_SelectedFiles[q]);

    mng_AllocTrackLock(cur_name, PAGETYPE_GAMEFILE);

    // D3EditState.current_gamefile=gamefile_handle;
  }

  UpdateDialog();
}

void CFilePageDialog::UpdateDialog() {
  int i;
  CCheckListBox *list;
  list = (CCheckListBox *)GetDlgItem(IDC_FILELIST);
  list->ResetContent();
  bool all_files = false;

  CComboBox *combo = (CComboBox *)GetDlgItem(IDC_DIRECTORY_FILTER);
  combo->GetLBText(combo->GetCurSel(), m_DirectoryFilter);
  if (!stricmp(m_DirectoryFilter.GetBuffer(0), "All Directories")) {
    all_files = true;
  }

  for (i = 0; i < MAX_GAMEFILES; i++) {

    if (Gamefiles[i].used && (mng_FindTrackLock(Gamefiles[i].name, PAGETYPE_GAMEFILE) == -1)) {
      if (all_files) {
        list->AddString(Gamefiles[i].name);
      } else {
        if (!stricmp(Gamefiles[i].dir_name, m_DirectoryFilter.GetBuffer(0))) {
          list->AddString(Gamefiles[i].name);
        }
      }
    }
  }

  list = (CCheckListBox *)GetDlgItem(IDC_CHECKEDOUT_LIST);
  list->ResetContent();

  for (i = 0; i < MAX_TRACKLOCKS; i++) {
    if (GlobalTrackLocks[i].used && GlobalTrackLocks[i].pagetype == PAGETYPE_GAMEFILE) {
      list->AddString(GlobalTrackLocks[i].name);
    }
  }
}

int CFilePageDialog::CreateIndexMap(int **index_map, int listbox_id) {
  int r, n;
  CCheckListBox *list = (CCheckListBox *)GetDlgItem(listbox_id);
  if (!list)
    return 0;

  int listbox_size = list->GetCount();
  int listbox_count = 0;
  for (r = 0; r < listbox_size; r++) {
    if (list->GetCheck(r))
      listbox_count++;
  }
  if (listbox_count == 0)
    return 0;

  int *map;

  map = *index_map = mem_rmalloc<int>(listbox_count);
  if (!map)
    return 0;
  n = 0;
  for (r = 0; r < listbox_size; r++) {
    if (list->GetCheck(r)) {
      map[n] = r;
      n++;
    }
  }
  return listbox_count;
}

int FindGameFile(char *name) {
  for (int i = 0; i < MAX_GAMEFILES; i++) {
    if (Gamefiles[i].used && !stricmp(name, Gamefiles[i].name))
      return i;
  }
  return -1;
}

void CFilePageDialog::OnCheckinFile() {
  int r, n;
  mngs_Pagelock temp_pl;

  if (Num_gamefiles < 1)
    return;

  int *listbox_map;
  int listbox_count = CreateIndexMap(&listbox_map, IDC_CHECKEDOUT_LIST);
  if (!listbox_count)
    return;

  if (!mng_MakeLocker()) {
    mem_free(listbox_map);
    return;
  }

  CCheckListBox *list = (CCheckListBox *)GetDlgItem(IDC_CHECKEDOUT_LIST);
  char buffer[_MAX_PATH], ext[_MAX_EXT];

  for (int i = 0; i < listbox_count; i++) {
    list->GetText(listbox_map[i], buffer);
    ddio_SplitPath(buffer, NULL, NULL, ext);
    if ((!stricmp(ext, ".d3l")) || (!stricmp(ext, ".cpp"))) {
      if (MessageBox("Tisk Tisk, Now there you go, ignoring Jeff when he told you not to check levels and scripts\nin "
                     "through this dialog.  Are you sure you want to ignore Jeff?\n(He doesn't like to be ignored)",
                     buffer, MB_YESNO) != IDYES)
        continue;
    }

    n = FindGameFile(buffer);
    if (n == -1)
      continue;

    // Make sure we own this gamefile
    strcpy(temp_pl.name, Gamefiles[n].name);
    temp_pl.pagetype = PAGETYPE_GAMEFILE;

    r = mng_CheckIfPageOwned(&temp_pl, TableUser);
    if (r < 0)
      OutrageMessageBox(ErrorString);
    else if (r == 0)
      OutrageMessageBox(InfoString);
    else {
      // Change the pagelock state to UNLOCKED
      strcpy(temp_pl.holder, "UNLOCKED");
      if (!mng_ReplacePagelock(temp_pl.name, &temp_pl)) {
        MessageBox(ErrorString, "Error!");
        continue;
      } else {
        // Now actually replace the copy on the net with our local one

        if (!mng_ReplacePage(Gamefiles[n].name, Gamefiles[n].name, n, PAGETYPE_GAMEFILE, 0))
          OutrageMessageBox(ErrorString);
        else {
          // Save this gamefile file to the network for all

          char destname[100], srcname[100];

          ddio_MakePath(srcname, LocalD3Dir, "data", Gamefiles[n].dir_name, Gamefiles[n].name, NULL);
          ddio_MakePath(destname, NetD3Dir, "data", Gamefiles[n].dir_name, Gamefiles[n].name, NULL);

          cf_CopyFile(destname, srcname);

          //@@OutrageMessageBox ("File checked in.");

          // Delete it from local pagefile if it is there
          int dret = mng_DeletePage(Gamefiles[n].name, PAGETYPE_GAMEFILE, 1);
          ASSERT(dret == 1);

          // Free the tracklock
          int p = mng_FindTrackLock(Gamefiles[n].name, PAGETYPE_GAMEFILE);
          ASSERT(p != -1);
          mng_FreeTrackLock(p);
        }
      }
    }
  }

  mng_EraseLocker();
  OutrageMessageBox("File(s) checked in.");
  mem_free(listbox_map);
  UpdateDialog();
}

void CFilePageDialog::OnDeleteFile() {
  int n;
  int answer, tl;
  mngs_Pagelock pl;

  if (Num_gamefiles < 1)
    return;

  int *listbox_map;
  int listbox_count = CreateIndexMap(&listbox_map, IDC_CHECKEDOUT_LIST);
  if (!listbox_count)
    return;

  if (!mng_MakeLocker()) {
    mem_free(listbox_map);
    return;
  }

  CCheckListBox *list = (CCheckListBox *)GetDlgItem(IDC_CHECKEDOUT_LIST);
  char buffer[_MAX_PATH];

  for (int i = 0; i < listbox_count; i++) {
    list->GetText(listbox_map[i], buffer);
    n = FindGameFile(buffer);
    if (n == -1)
      continue;

    // Check to see if we even have it locked
    if ((tl = mng_FindTrackLock(Gamefiles[n].name, PAGETYPE_GAMEFILE)) == -1) {
      OutrageMessageBox("This gamefile is not yours to delete.  Lock first.");
      continue;
    }

    // Make sure it is to be deleted
    answer = MessageBox("Are you sure you want to delete this gamefile?", Gamefiles[n].name, MB_YESNO);
    if (answer == IDNO)
      continue;

    strcpy(pl.name, Gamefiles[n].name);
    pl.pagetype = PAGETYPE_GAMEFILE;

    // Check to see if this is a local gamefile only.  If so, only delete it locally
    if (mng_CheckIfPageOwned(&pl, TableUser) != 1) {
      mng_FreeTrackLock(tl);
      if (!mng_DeletePage(Gamefiles[n].name, PAGETYPE_GAMEFILE, 1)) {
        mprintf(0, ErrorString);
        Int3();
      }
    } else // if it is network, delete it from both the net and local drives
    {

      mng_FreeTrackLock(tl);
      mng_DeletePage(Gamefiles[n].name, PAGETYPE_GAMEFILE, 1);
      mng_DeletePage(Gamefiles[n].name, PAGETYPE_GAMEFILE, 0);
      mng_DeletePagelock(Gamefiles[n].name, PAGETYPE_GAMEFILE);
    }

    FreeGamefile(n);
  }

  mng_EraseLocker();
  OutrageMessageBox("Gamefile(s) deleted.");
  mem_free(listbox_map);
  UpdateDialog();
}

void CFilePageDialog::OnLockFile() {
  int r, n;
  mngs_Pagelock temp_pl;
  mngs_gamefile_page gamefilepage;

  if (Num_gamefiles < 1)
    return;

  int *listbox_map;
  int listbox_count = CreateIndexMap(&listbox_map, IDC_FILELIST);
  if (!listbox_count)
    return;

  if (!mng_MakeLocker()) {
    mem_free(listbox_map);
    return;
  }

  CCheckListBox *list = (CCheckListBox *)GetDlgItem(IDC_FILELIST);
  char buffer[_MAX_PATH], ext[_MAX_EXT];

  for (int i = 0; i < listbox_count; i++) {
    list->GetText(listbox_map[i], buffer);
    ddio_SplitPath(buffer, NULL, NULL, ext);
    if ((!stricmp(ext, ".d3l")) || (!stricmp(ext, ".cpp"))) {
      if (MessageBox("Tisk Tisk, Now there you go, ignoring Jeff when he told you not to check levels and scripts\nout "
                     "through this dialog.  Are you sure you want to ignore Jeff?\n(He doesn't like to be ignored)",
                     buffer, MB_YESNO) != IDYES)
        continue;
    }

    n = FindGameFile(buffer);
    if (n == -1)
      continue;

    // Make sure it can be locked
    strcpy(temp_pl.name, Gamefiles[n].name);
    temp_pl.pagetype = PAGETYPE_GAMEFILE;

    r = mng_CheckIfPageLocked(&temp_pl);
    if (r == 2) {
      int answer;
      answer = OutrageMessageBox(MBOX_YESNO, "This page is not even in the table file, or the database maybe corrupt.  "
                                             "Override to 'Unlocked'? (Select NO if you don't know what you're doing)");
      if (answer == IDYES) {
        strcpy(temp_pl.holder, "UNLOCKED");
        if (!mng_ReplacePagelock(temp_pl.name, &temp_pl))
          MessageBox(ErrorString, "Error!");
      }
    } else if (r < 0)
      OutrageMessageBox(ErrorString);
    else if (r == 1)
      OutrageMessageBox(InfoString);
    else {

      // Everything is ok.  Tell the network we're locking it and get a copy to
      // our local drive
      strcpy(temp_pl.holder, TableUser);

      // Search thru the net pagefile and get a new copy in RAM in case anyone
      // changed it since we started the editor
      if (mng_FindSpecificGamefilePage(temp_pl.name, &gamefilepage, 0)) {
        if (mng_AssignGamefilePageToGamefile(&gamefilepage, n)) {
          if (!mng_ReplacePage(Gamefiles[n].name, Gamefiles[n].name, n, PAGETYPE_GAMEFILE, 1)) {
            OutrageMessageBox("There was problem writing that page locally!");
            continue;
          } else {

            if (!mng_ReplacePagelock(temp_pl.name, &temp_pl)) {
              MessageBox(ErrorString, "Error!");
              continue;
            }
          }

        } else
          OutrageMessageBox("There was a problem loading this gamefile.  You might encounter problems in dealing with "
                            "it.  Good luck!");

        mng_AllocTrackLock(Gamefiles[n].name, PAGETYPE_GAMEFILE);

      } else
        OutrageMessageBox("Couldn't find that gamefile in the table file!");
    }
  }

  mng_EraseLocker();
  OutrageMessageBox("File(s) locked.");
  mem_free(listbox_map);
  UpdateDialog();
}

BOOL CFilePageDialog::OnInitDialog() {
  CDialog::OnInitDialog();

  CComboBox *combo = (CComboBox *)GetDlgItem(IDC_DIRECTORY_FILTER);
  combo->ResetContent();
  combo->AddString("All Directories");
  for (int i = 0; i < NumDirectoryNames; i++) {
    combo->AddString(DirectoryNames[i]);
  }

  combo->SetCurSel(0);
  UpdateData(true);

  UpdateDialog();

  return TRUE; // return TRUE unless you set the focus to a control
               // EXCEPTION: OCX Property Pages should return FALSE
}

void CFilePageDialog::OnOverride() {
  int n;
  mngs_Pagelock temp_pl;

  if (Num_gamefiles < 1)
    return;

  int *listbox_map;
  int listbox_count = CreateIndexMap(&listbox_map, IDC_FILELIST);
  if (!listbox_count)
    return;

  CCheckListBox *list = (CCheckListBox *)GetDlgItem(IDC_FILELIST);
  char buffer[_MAX_PATH];

  for (int i = 0; i < listbox_count; i++) {
    list->GetText(listbox_map[i], buffer);
    n = FindGameFile(buffer);
    if (n == -1)
      continue;

    strcpy(temp_pl.name, Gamefiles[n].name);
    temp_pl.pagetype = PAGETYPE_GAMEFILE;

    mng_OverrideToUnlocked(&temp_pl);
  }

  mem_free(listbox_map);
}

void CFilePageDialog::OnFilesOut() {
  char str[10000];
  int total = 0;

  mng_DisplayLockList(TableUser);

  sprintf(str, "User %s has these files held locally:\n\n", TableUser);
  for (int i = 0; i < MAX_TRACKLOCKS; i++) {
    if (GlobalTrackLocks[i].used && GlobalTrackLocks[i].pagetype == PAGETYPE_GAMEFILE) {
      char temp[90];

      sprintf(temp, "%s", GlobalTrackLocks[i].name);
      strcat(str, temp);
      strcat(str, "\n");
      total++;
    }
  }

  if (total != 0) {
    // Display that string
    MessageBox(str, "Game files", MB_OK);
  }
}

void CFilePageDialog::OnSelchangeDirectoryFilter() {
  UpdateData(true);
  UpdateDialog();
}

////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
