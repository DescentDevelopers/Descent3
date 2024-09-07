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

 * $Logfile: /DescentIII/Main/newui_filedlg.cpp $
 * $Revision: 22 $
 * $Date: 10/25/99 10:46a $
 * $Author: Matt $
 *
 * File Dialog using NewUI class
 *
 * $Log: /DescentIII/Main/newui_filedlg.cpp $
 *
 * 22    10/25/99 10:46a Matt
 * Code from Duane that allows users to double-click on a directory in the
 * file dialog to open that folder.
 *
 * 21    9/07/99 4:33p Jeff
 * fixed crash due to not setting a pointer to NULL
 *
 * 20    5/10/99 10:23p Ardussi
 * changes to compile on Mac
 *
 * 19    5/10/99 9:21p Samir
 * Ardussi pointed out this small but significant error when checking for
 * valid path.
 *
 * 18    4/20/99 3:11p Jeff
 * handle invalid initial directory
 *
 * 17    4/20/99 3:07p Jeff
 * save last directory
 *
 * 16    4/20/99 11:46a Samir
 * numerous ui 'feel' fixes.
 *
 * 15    4/14/99 3:56a Jeff
 * fixed case mismatch in #includes
 *
 * 14    2/28/99 6:14p Jeff
 * use UID_OK and UID_CANCEL
 *
 * 13    2/28/99 2:57a Jeff
 * got rid of useless hotspots
 *
 * 12    2/28/99 2:23a Jeff
 * use new ui
 *
 * 11    1/29/99 5:22p Jeff
 * localization
 *
 * 10    1/27/99 5:47p Jeff
 * put up a message that files are being gathered...ease the fear of crash
 *
 * 9     1/21/99 11:15p Jeff
 * pulled out some structs and defines from header files and moved them
 * into separate header files so that multiplayer dlls don't require major
 * game headers, just those new headers.  Side effect is a shorter build
 * time.  Also cleaned up some header file #includes that weren't needed.
 * This affected polymodel.h, object.h, player.h, vecmat.h, room.h,
 * manage.h and multi.h
 *
 * 8     10/08/98 4:23p Kevin
 * Changed code to comply with memory library usage. Always use mem_malloc
 * , mem_free and mem_strdup
 *
 * 7     9/28/98 4:35p Jeff
 * general UI changes and improvements
 *
 * 6     9/23/98 6:19p Jeff
 * finished up (hopefully) updating the config/ui dialogs to meet our
 * standard.  Keyboard/joystick config still needs some work
 *
 * 5     9/02/98 2:54p Jeff
 * added defines for text colors to be used throughout the game...fixed up
 * buddy menu too
 *
 * 4     8/19/98 2:18p Jeff
 * changed ddio_CleanPath
 *
 * 3     7/30/98 12:31p Jeff
 *
 * 2     7/29/98 5:39p Jeff
 * initial creation
 *
 * $NoKeywords: $
 */

#include <cstring>
#include <filesystem>

#include "cfile.h"
#include "ddio.h"
#include "game.h"
#include "mono.h"
#include "newui.h"
#include "pstring.h"
#include "renderer.h"
#include "stringtable.h"

newuiListBox *path_list = nullptr;
char *path_edit = nullptr;
std::filesystem::path working_filename;
std::filesystem::path working_listpath;
newuiSheet *fdlg_working_sheet = nullptr;

std::filesystem::path NewuiFileDlg_lastpath;

// border size of the UI window
#define UI_BORDERSIZE 20

// Updates the given listbox with the directories and files that match wildcards (each wildcard separates by a ;)
void UpdateFileList(newuiListBox *lb, const std::filesystem::path &path, const std::vector<std::string> &wildcards);

// Callback function for listbox
void FileSelectCallback(int index);

void FDlg_EnableWaitMessage(bool enable) {
#define FDWM_HEIGHT 128
#define FDWM_WIDTH 256

  static NewUIGameWindow *msgbox = nullptr;
  static bool opened = false;
  newuiSheet *sheet;

  if (opened == enable) {
    Int3();
  }

  opened = enable;

  if (enable) {
    msgbox = new NewUIGameWindow;
    if (msgbox) {

      msgbox->Create(0, 0, FDWM_WIDTH, FDWM_HEIGHT, UIF_PROCESS_ALL | UIF_CENTER);

      sheet = msgbox->GetSheet();
      sheet->NewGroup(nullptr, 45, 25);
      sheet->AddText(TXT_GETTINGFILES);

      msgbox->Open();
      sheet->Realize();

      // fake a DoUI
      DoUIFrame();
      rend_Flip();
    }
  } else {

    if (msgbox) {
      msgbox->Close();
      msgbox->Destroy();
      delete msgbox;
    }
    msgbox = nullptr;
  }
}

bool IsDirectoryItem(char *item) {
  if (*item != ' ')
    return false;
  if (*(item + 1) != '[')
    return false;
  if (item[strlen(item) - 1] == ']')
    return true;
  return false;
}
// Callback function for listbox
void FileSelectCallback(int index) {
  if ((!path_edit) || (!path_list))
    return;

  char file[_MAX_PATH];
  path_list->GetCurrentItem(file, _MAX_PATH);
  if (IsDirectoryItem(file))
    return; // we don't care about directories

  working_filename = file;

  strncpy(path_edit, (working_listpath / working_filename).u8string().c_str(), _MAX_PATH - 1);
  path_edit[_MAX_PATH - 1] = '\0';
  mprintf(0, "New Path: %s\n", path_edit);
  fdlg_working_sheet->UpdateChanges();
}

#define ED_WILDCARD 0
#define ED_FILENAME 1

#define ID_ROOTPATH 24
#define ID_LISTBOX 25
#define ID_UPDIR 26
#define ID_WILDCARD 27
#define ID_FILENAME 28
#define ID_CANCEL UID_CANCEL
#define ID_OK UID_OK

bool DoPathFileDialog(bool save_dialog, std::filesystem::path &path, const char *title,
                      const std::vector<std::string> &wildc, int flags) {
  newuiTiledWindow window;
  newuiSheet *sheet;
  newuiListBox *listbox, *rootpathlistbox;
  char *edits[2];

  std::filesystem::path working_path;
  std::filesystem::path working_file;
  std::filesystem::path working_root;

  if (!title)
    return false;
  if (wildc.empty())
    return false;

  std::vector<std::string> wildcards = wildc;

  working_root = path.root_name();
  // figure out the correct path
  if (!std::filesystem::is_directory(path)) {
    // try to use last good path
    if (!std::filesystem::is_directory(NewuiFileDlg_lastpath)) {
      path = LocalD3Dir;
    } else {
      path = NewuiFileDlg_lastpath;
    }
  }

  // Create all the UI Items
  window.Create(title, 0, 0, 380, 280);
  fdlg_working_sheet = sheet = window.GetSheet();

  sheet->NewGroup(nullptr, 0, 0);
  rootpathlistbox = sheet->AddListBox(60, 124, ID_ROOTPATH);
  for (auto const &root : ddio_GetSysRoots()) {
    rootpathlistbox->AddItem(root.u8string().c_str());
  }

  sheet->NewGroup(nullptr, 80, 0);
  listbox = sheet->AddListBox(224, 124, ID_LISTBOX);

  sheet->NewGroup(nullptr, 0, 145);
  edits[ED_FILENAME] = sheet->AddEditBox(nullptr, _MAX_PATH, 300, ID_FILENAME);
  edits[ED_WILDCARD] = sheet->AddEditBox(nullptr, 256, 150, ID_WILDCARD);
  sheet->AddLongButton(TXT_UPTOPARENTDIR, ID_UPDIR);

  sheet->NewGroup(nullptr, 200, 160);
  sheet->AddButton((save_dialog) ? TXT_SAVE : TXT_OPEN, ID_OK);
  sheet->AddButton(TXT_CANCEL, ID_CANCEL);

  path_edit = edits[ED_FILENAME];
  path_list = listbox;

  listbox->SetSelectChangeCallback(FileSelectCallback);

  working_path = std::filesystem::canonical(path);
  UpdateFileList(listbox, working_path, wildcards);

  working_listpath = working_path;
  working_filename = working_file;

  strncpy(edits[ED_FILENAME], working_path.u8string().c_str(), _MAX_PATH - 1);
  edits[ED_FILENAME][_MAX_PATH - 1] = '\0';

  std::string wildc1 = StringJoin(wildcards, ";");
  strncpy(edits[ED_WILDCARD], wildc1.c_str(), 255);
  edits[ED_WILDCARD][255] = '\0';

  bool ret = false;
  bool exit_menu = false;

  sheet->SetInitialFocusedGadget(ID_LISTBOX);

  window.Open();
  while (!exit_menu) {
    int res = window.DoUI();

    // handle all UI results.
    switch (res) {
    case ID_OK: {
      path = edits[ED_FILENAME];

      if (path.empty()) {
        DoMessageBox(TXT_ERROR, TXT_ERRCHOOSEFILE, MSGBOX_OK);
      } else {
        if (std::filesystem::is_directory(path)) {
          std::filesystem::path newpath;
          char file[_MAX_PATH];
          int selection = listbox->GetCurrentIndex();
          if ((selection >= 0) && (listbox->GetCurrentItem(file, _MAX_PATH))) {
            if (*file) {
              working_file.clear();

              // ok the user double-clicked on a directory
              // convert file without the brackets
              file[strlen(file) - 1] = '\0';
              if (!working_path.empty())
                newpath = working_path / (file + 2);
              else
                newpath = (file + 2);

              UpdateFileList(listbox, newpath, wildcards);
              working_path = std::filesystem::canonical(newpath);

              strncpy(edits[ED_FILENAME], working_path.u8string().c_str(), _MAX_PATH - 1);
              edits[ED_FILENAME][_MAX_PATH - 1] = '\0';
            }
          }
        } else {
          if (flags & PFDF_FILEMUSTEXIST) {
            if (!cfexist(path)) {
              DoMessageBox(TXT_ERROR, TXT_ERRFILENOTEXIST, MSGBOX_OK);
            } else {
              NewuiFileDlg_lastpath = path.parent_path();
              ret = exit_menu = true;
            }
          } else {
            NewuiFileDlg_lastpath = path.parent_path();
            ret = exit_menu = true;
          }
        }
      }
    } break;
    case ID_CANCEL:
      ret = false;
      exit_menu = true;
      break;
    case ID_ROOTPATH: {
      char root[_MAX_PATH];
      int selection = rootpathlistbox->GetCurrentIndex();
      if ((selection >= 0) && (rootpathlistbox->GetCurrentItem(root, _MAX_PATH))) {
        if (*root) {
          if (std::filesystem::path(root) != working_root) {
            // root has been changed, update file list
            working_root = std::filesystem::path(root);
            working_path = working_root / "/";
            UpdateFileList(listbox, working_path, wildcards);
          }
        }
      }
      break;
    }
    case ID_LISTBOX: {
      std::filesystem::path newpath;
      char file[_MAX_PATH];
      // check to see if the double-clicked on a dir
      int selection = listbox->GetCurrentIndex();

      if ((selection >= 0) && (listbox->GetCurrentItem(file, _MAX_PATH))) {
        if (*file) {
          if (IsDirectoryItem(file)) {
            working_file.clear();

            // ok the user double-clicked on a directory
            // convert file without the brackets
            file[strlen(file) - 1] = '\0';
            if (!working_path.empty())
              newpath = std::filesystem::canonical(working_path / (file + 2));
            else
              newpath = std::filesystem::canonical(file + 2);

            UpdateFileList(listbox, newpath, wildcards);
            working_path = newpath;

            strncpy(edits[ED_FILENAME], working_path.u8string().c_str(), _MAX_PATH - 1);
            edits[ED_FILENAME][_MAX_PATH - 1] = '\0';
          } else {
            // double-click on a file
            working_file = file;
            FileSelectCallback(selection);
            strcpy(file, edits[ED_FILENAME]);

            // the user wants a file
            if (flags & PFDF_FILEMUSTEXIST) {
              if (cfexist(file)) {
                exit_menu = true;
                ret = true;
                path = file;
                NewuiFileDlg_lastpath = path.parent_path();
              } else {
                // invalid file
                DoMessageBox(TXT_ERROR, TXT_ERRCHOOSEFILE, MSGBOX_OK);
              }
            } else {
              path = file;
              exit_menu = true;
              ret = true;
              NewuiFileDlg_lastpath = path.parent_path();
            }
          }
        }
      }
    } break;
    case ID_UPDIR: {
      working_file.clear();
      working_path = working_path.parent_path();

      UpdateFileList(listbox, working_path, wildcards);
      strncpy(edits[ED_FILENAME], working_path.u8string().c_str(), _MAX_PATH - 1);
      edits[ED_FILENAME][_MAX_PATH - 1] = '\0';
    } break;
    case ID_WILDCARD: {
      wildcards = StringSplit(edits[ED_WILDCARD], ";");
      UpdateFileList(listbox, working_path, wildcards);
    } break;
    case ID_FILENAME: {
      std::filesystem::path c = edits[ED_FILENAME];

      if (!c.empty()) {
        if (!std::filesystem::is_directory(c)) {
          // the user wants a file
          if (flags & PFDF_FILEMUSTEXIST) {
            if (cfexist(c)) {
              path = c;
              exit_menu = true;
              ret = true;
              NewuiFileDlg_lastpath = path.parent_path();
            } else {
              // invalid file
              DoMessageBox(TXT_ERROR, TXT_ERRFILENOTEXIST, MSGBOX_OK);
            }
          } else {
            path = c;
            NewuiFileDlg_lastpath = path.parent_path();
            exit_menu = true;
            ret = true;
          }
        } else {
          // just change directories
          working_path = c;
          UpdateFileList(listbox, working_path, wildcards);
        }
      }
    } break;
    default:
      mprintf(0, "No operation in DoPathFileDialog()!\n");
    }
  }

  if (ret) {
    mprintf(0, "Selected Filename: %s\n", path.u8string().c_str());
  } else {
    mprintf(0, "Cancel!\n");
  }

  window.Close();
  window.Destroy();

  working_filename.clear();
  working_listpath.clear();
  path_edit = nullptr;
  path_list = nullptr;

  return ret;
}

// Updates the given listbox with the directories and files that match wildcards (each wildcard separates by a ;)
void UpdateFileList(newuiListBox *lb, const std::filesystem::path &path, const std::vector<std::string> &wildcards) {
  FDlg_EnableWaitMessage(true);
  lb->RemoveAll();

  if (!std::filesystem::is_directory(path)) {
    // invalid directory/path
    DoMessageBox(TXT_ERROR, TXT_ERRPATHNOTVALID, MSGBOX_OK);
    FDlg_EnableWaitMessage(false);
    return;
  }

  std::vector<std::filesystem::path> dirs = {".."};
  std::vector<std::filesystem::path> files;

  try {
    for (auto const &dir_entry : std::filesystem::directory_iterator{path}) {
      if (std::filesystem::is_directory(dir_entry)) {
        dirs.push_back(dir_entry.path().filename());
      }
      if (std::filesystem::is_regular_file(dir_entry)) {
        for (const auto &wildcard : wildcards) {
          if (stricmp(dir_entry.path().extension().u8string().c_str(),
                      std::filesystem::path(wildcard).extension().u8string().c_str()) == 0) {
            files.push_back(dir_entry.path().filename());
          }
        }
      }
    }
  } catch (std::exception &e) {
    DoMessageBox(TXT_ERROR, TXT_ERRPATHNOTVALID, MSGBOX_OK);
    mprintf(0, "Error iterating directory %s: %s\n", path.u8string().c_str(), e.what());
  }

  if (dirs.size() + files.size() == 0) {
    FDlg_EnableWaitMessage(false);
    return;
  }

  char tempbuffer[_MAX_PATH + 1];

  for (auto const &dir : dirs) {
    snprintf(tempbuffer, sizeof(tempbuffer), " [%s]", dir.u8string().c_str());
    lb->AddItem(tempbuffer);
  }
  for (auto const &file : files) {
    lb->AddItem(file.u8string().c_str());
  }

  FDlg_EnableWaitMessage(false);
}
