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

 * $Logfile: /DescentIII/Main/editor/ScriptWizard.cpp $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:57:39 $
 * $Author: kevinb $
 *
 *	Script Wizard
 *
 * $Log: not supported by cvs2svn $
 *
 * 20    11/06/98 12:34p Jason
 * more speedups for manage system
 *
 * 19    10/08/98 4:24p Kevin
 * Changed code to comply with memory library usage. Always use mem_malloc
 * , mem_free and mem_strdup
 *
 * 18    8/31/98 4:12p Samir
 * select level script by default in listbox.
 *
 * 17    6/23/98 3:04p Samir
 * sped up script list generation.
 *
 * 16    6/23/98 2:43p Matt
 * Changed calls to OutrageMessageBox() & Debug_MessageBox() to deal with
 * int return value (instead of bool).
 *
 * 15    4/14/98 7:31p Matt
 * Changed code to use ddio_MakePath() instead of sprintf() to create file
 * spec
 *
 * 14    1/23/98 4:45p Samir
 * Make level.scr be the default.
 *
 * 13    9/26/97 4:15p Samir
 * Mostly working system to go to a certain script in a file.
 *
 * 12    9/25/97 5:28p Samir
 * Even newer script code due to more changes in ObjCScript.cpp
 *
 * 11    9/24/97 6:18p Samir
 * Fix some memory leakage probs.
 *
 * 10    9/24/97 2:57p Samir
 * New ScriptWizard functionality, without extra commenting in files.
 *
 * 9     9/22/97 5:59p Samir
 * Changed ObjCScript system, so everything is broken, but it shouldn't
 * crash the game.
 *
 * 8     9/10/97 1:58p Samir
 * Fixed up some code to select multiple script files.
 *
 * 7     9/04/97 4:39p Matt
 * Added includes needed as a result of removing includes from d3edit.h
 *
 * 6     9/02/97 3:28p Matt
 * Got rid of warnings
 *
 * 5     8/21/97 6:00p Samir
 * A whole lot of script editing stuff added for ScriptWizard.
 *
 * 4     8/20/97 3:35p Samir
 * Enhanced ScriptStudio to use tabs!  Use new ScriptStudio instead of
 * ScriptEditor.
 *
 * $NoKeywords: $
 */

// ScriptWizard.cpp : implementation file
//

#include <filesystem>
#include <regex>

#include "stdafx.h"
#include "editor.h"
#include "ScriptWizard.h"
#include "ObjCScript.h"
#include "manage.h"
#include "EditLineDialog.h"
#include "ScriptEditorDlg.h"
#include "gamefile.h"
#include "manage\gamefilepage.h"
#include "ScriptStudio.h"
#include "AddScriptDialog.h"
#include "pserror.h"
#include "ddio.h"
#include "mem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MAX_SCREVTS 15

/////////////////////////////////////////////////////////////////////////////
// CScriptWizard dialog

CScriptWizard::CScriptWizard(CWnd *pParent /*=NULL*/) : CDialog(CScriptWizard::IDD, pParent) {
  //{{AFX_DATA_INIT(CScriptWizard)
  // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT

  m_ScriptSource = NULL;
  m_ScriptCode = NULL;
}

void CScriptWizard::DoDataExchange(CDataExchange *pDX) {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CScriptWizard)
  // NOTE: the ClassWizard will add DDX and DDV calls here
  //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CScriptWizard, CDialog)
//{{AFX_MSG_MAP(CScriptWizard)
ON_BN_CLICKED(IDC_ADDSCRIPT, OnAddScript)
ON_BN_CLICKED(IDC_LOCK, OnLock)
ON_BN_CLICKED(IDC_UNLOCK, OnUnlock)
ON_BN_CLICKED(IDC_EDITSCRIPT, OnEditScript)
ON_LBN_SELCHANGE(IDC_SCR_LISTBOX, OnSelChangeScrListbox)
ON_BN_CLICKED(IDC_ADDEVENT, OnAddEvent)
ON_LBN_SELCHANGE(IDC_SCREVT_LISTBOX, OnSelchangeScrevtListbox)
ON_CBN_SELCHANGE(IDC_SCRMOD_BOX, OnSelchangeScrmodBox)
ON_WM_DESTROY()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScriptWizard message handlers

BOOL CScriptWizard::OnInitDialog() {
  CDialog::OnInitDialog();

  //	Place all valid script files into the module combo box, then
  //	place all script names relating to the current script file into the script
  // listbox.
  CComboBox *modcbox = (CComboBox *)GetDlgItem(IDC_SCRMOD_BOX);

  // m_ScriptCode = D3XReallocProgram(NULL, 0, 0, 0); // LGT: undefined
  ResetScriptList();

  //	place all filenames of scripts into combo box
  std::filesystem::path dir = std::filesystem::path(LocalLevelsDir);
  ddio_DoForeachFile(dir, std::regex(".+\\.scr"), [&modcbox](const std::filesystem::path& path){
    std::filesystem::path file = path.filename();
    modcbox->AddString(file.u8string().c_str());
  });

  modcbox->SelectString(-1, LEVEL_SCRIPT_NAME);

  //	update listboxes accordingly to current script module
  ScriptFileName[0] = 0;
  ScriptCodeName[0] = 0;
  CScriptWizard::LoadCurrentModule();

  //	Updates the buttons
  UpdateDialogButtons();

  return TRUE; // return TRUE unless you set the focus to a control
               // EXCEPTION: OCX Property Pages should return FALSE
}

void CScriptWizard::OnDestroy() {
  CDialog::OnDestroy();

  if (m_ScriptCode)
    // D3XFreeProgram(m_ScriptCode); // LGT: undefined
    ResetScriptList();
}

//	We save whatever was added to the script.  This does not include text added via edit
//	(that's done in the internal script editor.)  It does include text added from operations like
//	Add Script, Add Event, etc.
void CScriptWizard::OnOK() {
  SaveScript(ScriptFileName, m_ScriptSource);
  FreeScript(m_ScriptSource);

  CDialog::OnOK();
}

//	We just don't save anything.  We free the script, if there is one
void CScriptWizard::OnCancel() {
  FreeScript(m_ScriptSource);

  CDialog::OnCancel();
}

//	Adds a script to the current editable script.  We will not allow this if the script is not locked,
//	so we should allow then to unlock it at this point.
void CScriptWizard::OnAddScript() {
  char scrname[MAX_SCRNAME];

  scrname[0] = 0;

  if (strlen(ScriptFileName) == 0) {
    OutrageMessageBox("There is no script file to add a script to!  You must make one.");
    return;
  }

  //	check if current module is locked by us.
  if (mng_FindTrackLock(ScriptFileName, PAGETYPE_GAMEFILE) == -1)
    OutrageMessageBox("Warning: This script is not locked by you.\nAny changes you make will not be permanent.");

  //	request a name for this new script!
  CAddScriptDialog dlg(this);
  if (dlg.DoModal() == IDOK) {
    //	add script to listbox, then select it, then run editor
    CListBox *lb = (CListBox *)GetDlgItem(IDC_SCR_LISTBOX);
    char *tempstr = AddScriptBlockToScript(m_ScriptSource, (LPCSTR)dlg.m_Name, (LPCSTR)dlg.m_TypeName);
    mem_free(m_ScriptSource);
    m_ScriptSource = tempstr;
    UpdateScriptListbox();
    lb->SelectString(-1, (LPCSTR)dlg.m_Name);
    OnEditScript();
  } else {
    OutrageMessageBox("A script name is required to continue. Cancelling action.");
  }
}

void CScriptWizard::OnAddEvent() {
  CListBox *listbox = (CListBox *)GetDlgItem(IDC_SCREVT_LISTBOX);
  CListBox *scrlb = (CListBox *)GetDlgItem(IDC_SCR_LISTBOX);
  char *tempstr;

  char evtname[MAX_EVTNAME];
  char scrname[MAX_SCRNAME];

  ASSERT(listbox->GetCurSel() != LB_ERR);
  ASSERT(scrlb->GetCurSel() != LB_ERR);

  scrlb->GetText(scrlb->GetCurSel(), scrname);
  listbox->GetText(listbox->GetCurSel(), evtname);

  tempstr = AddEventBlockToScript(m_ScriptSource, evtname, scrname);

  if (!tempstr) {
    OutrageMessageBox("Failed to add event to script.\nScript text possibly corrupted.");
    return;
  }

  mem_free(m_ScriptSource);
  m_ScriptSource = tempstr;
}

//	This code invokes the original OSIRIS editor dialog with the currently loaded script file.
void CScriptWizard::OnEditScript() {
  CScriptStudio studio;
  CScriptEditorDlg editor;

  if (strlen(ScriptFileName) == 0) {
    OutrageMessageBox("There is no script file to add a script to!  You must make one.");
    return;
  }

  //	check if current module is locked by us.
  if (FindGamefileName(ScriptFileName) != -1) {
    if (mng_FindTrackLock(ScriptFileName, PAGETYPE_GAMEFILE) == -1) {
      OutrageMessageBox("Warning: This script is not locked by you.\nAny changes you make will not be permanent.");
    }
  }

  //@@**
  //@@**//	Do default external editor stuff.  Jeff Slutter's code.
  //@@**	if(strlen(Default_external_editor)==0)
  //@@**	{
  //@@**		char buffer[256];
  //@@**		CString default_path;
  //@@**		int size=GetWindowsDirectory(buffer,256);
  //@@**		ASSERT(size<256);
  //@@**		buffer[size]='\\';
  //@@**		buffer[size+1]='\0';
  //@@**
  //@@**		default_path=buffer;
  //@@**		default_path+="Notepad.exe";
  //@@**		editor.SetExternalEditor(default_path.GetBuffer(1));
  //@@**	}
  //@@**	else
  //@@**	{
  //@@**		editor.SetExternalEditor(Default_external_editor);
  //@@**	}

  //	Now just set the current source loaded as the editor's source
  //	Also create a program so that the compiler can
  char buf[64];
  CListBox *scrlistbox = (CListBox *)GetDlgItem(IDC_SCR_LISTBOX);

  scrlistbox->GetText(scrlistbox->GetCurSel(), buf);
  studio.SelectText(buf);
  studio.SetD3XObject(m_ScriptCode);
  studio.SetText(CString(m_ScriptSource));

  if (studio.DoModal() == IDOK) {
    // Try and compile now
    CString tempstr;

    FreeScript(m_ScriptSource);
    studio.GetText(tempstr);
    m_ScriptSource = (char *)mem_malloc(tempstr.GetLength() + 1);

    if (m_ScriptSource) {
      strcpy(m_ScriptSource, (LPCSTR)tempstr);
      SaveScript(ScriptFileName, m_ScriptSource);
      FreeScript(m_ScriptSource);
      m_ScriptSource = LoadScript(ScriptFileName); // After this, Source_script has new edited script.

      if (!CompileScript(studio.GetD3XObject(), m_ScriptSource)) {
        OutrageMessageBox("Your script did not compile.  Unless you fix it, the script will not work.");
      } else {
        SaveScriptCode(ScriptCodeName, studio.GetD3XObject());
      }
    } else {
      OutrageMessageBox("Couldn't allocate memory to save script!");
    }
  }

  m_ScriptCode = studio.GetD3XObject();

  //	rebuild list of scripts from changes in source code that may have happened.
  ResetScriptList();
  if (stricmp(ScriptFileName, DEFAULT_SCRIPT_NAME) == 0)
    GenerateScriptWizardInfo(m_ScriptSource, false);
  else
    GenerateScriptWizardInfo(m_ScriptSource, true);

  UpdateScriptListbox();

  //@@**	if(strcmp(editor.m_ExternalEditor,Default_external_editor))
  //@@**	{
  //@@**		strcpy(Default_external_editor,editor.m_ExternalEditor.GetBuffer(1));
  //@@**	}
}

//	Uses gamefile locking code!
//	This is taken from FilePageDialog.cpp, with some modifications to the outputted text, but uses the GameFile
//	page scheme, emulating a script page.
void CScriptWizard::OnLock() {
  int n, r;
  mngs_Pagelock temp_pl;
  mngs_gamefile_page gamefilepage;

  n = FindGamefileName(ScriptFileName);
  if (n == -1) {
    OutrageMessageBox("Unable to find script file page in the table file.\nOperation can't be completed.");
    return;
  }

  //	start the really involved locking process.
  if (!mng_MakeLocker())
    return;

  // Make sure it can be locked
  strcpy(temp_pl.name, Gamefiles[n].name);
  temp_pl.pagetype = PAGETYPE_GAMEFILE;

  r = mng_CheckIfPageLocked(&temp_pl);
  if (r == 2) {
    int answer =
        OutrageMessageBox(MBOX_YESNO, "This script is not even in the table file, or the database maybe corrupt.  "
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
    if (!mng_ReplacePagelock(temp_pl.name, &temp_pl)) {
      MessageBox(ErrorString, "Error!");
      mng_EraseLocker();
      return;
    } else {
      // Search thru the net pagefile and get a new copy in RAM in case anyone
      // changed it since we started the editor
      if (mng_FindSpecificGamefilePage(temp_pl.name, &gamefilepage, 0)) {
        if (mng_AssignGamefilePageToGamefile(&gamefilepage, n)) {
          if (!mng_ReplacePage(Gamefiles[n].name, Gamefiles[n].name, n, PAGETYPE_GAMEFILE, 1)) {
            OutrageMessageBox("There was problem writing that page locally!");
            mng_EraseLocker();
            return;
          }
          OutrageMessageBox("Script locked.");
        } else
          OutrageMessageBox(
              "There was a problem loading this script.  You might encounter problems in dealing with it.  Good luck!");

        mng_AllocTrackLock(Gamefiles[n].name, PAGETYPE_GAMEFILE);
        UpdateDialogButtons();
      } else
        OutrageMessageBox("Couldn't find that script in the table file!");
    }
  }
  mng_EraseLocker();
}

//	Uses gamefile unlocking code!
void CScriptWizard::OnUnlock() {
  int n, r;
  mngs_Pagelock temp_pl;

  n = FindGamefileName(ScriptFileName);
  if (n == -1) {
    OutrageMessageBox("Unable to find script file page in the table file.\nOperation can't be completed.");
    return;
  }

  if (!mng_MakeLocker())
    return;

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
      mng_EraseLocker();
      return;
    } else {
      // Now actually replace the copy on the net with our local one
      if (!mng_ReplacePage(Gamefiles[n].name, Gamefiles[n].name, n, PAGETYPE_GAMEFILE, 0))
        OutrageMessageBox(ErrorString);
      else {
        // Save this gamefile file to the network for all
        // Force the save of the current script file so this actually works
        char destname[100], srcname[100];

        SaveScript(ScriptFileName, m_ScriptSource);

        ddio_MakePath(srcname, LocalD3Dir, "data", Gamefiles[n].dir_name, Gamefiles[n].name, NULL);
        ddio_MakePath(destname, NetD3Dir, "data", Gamefiles[n].dir_name, Gamefiles[n].name, NULL);

        cf_CopyFile(destname, srcname);

        OutrageMessageBox("Script checked in.");

        // Delete it from local pagefile if its there
        int dret = mng_DeletePage(Gamefiles[n].name, PAGETYPE_GAMEFILE, 1);
        ASSERT(dret == 1);
        mng_EraseLocker();

        // Free the tracklock
        int p = mng_FindTrackLock(Gamefiles[n].name, PAGETYPE_GAMEFILE);
        ASSERT(p != -1);
        mng_FreeTrackLock(p);
        UpdateDialogButtons();
      }
    }
  }
}

void CScriptWizard::OnSelChangeScrListbox() {
  CListBox *lb = (CListBox *)GetDlgItem(IDC_SCR_LISTBOX);

  m_CurScriptSel = lb->GetCurSel();
  if (m_CurScriptSel == LB_ERR)
    m_CurScriptSel = -1;
}

void CScriptWizard::OnSelchangeScrevtListbox() {
  CListBox *lb = (CListBox *)GetDlgItem(IDC_SCREVT_LISTBOX);

  m_CurEventSel = lb->GetCurSel();
  UpdateDialogButtons();
}

//	User Interface Update Controllers
//		ListBox
//		ComboBox

void CScriptWizard::LoadCurrentModule() {
  CComboBox *modcbox = (CComboBox *)GetDlgItem(IDC_SCRMOD_BOX);
  char str[MAX_SCRNAME];

  //	Get current module select and load it in.
  if (m_ScriptSource)
    FreeScript(m_ScriptSource);

  m_ScriptSource = NULL;

  if (modcbox->GetLBText(modcbox->GetCurSel(), str) != CB_ERR) {
    char *ptr;
    m_ScriptSource = LoadScript(str);
    strcpy(ScriptFileName, str);
    strcpy(ScriptCodeName, str);
    ptr = strtok(ScriptCodeName, ".");
    strcat(ptr, ".d3x");
  }

  //	new script file, set current script to 0
  m_CurScriptSel = 0;

  //	compile and generate list of script names.
  ResetScriptList();
  if (m_ScriptSource) {
    if (stricmp(ScriptFileName, DEFAULT_SCRIPT_NAME) == 0)
      GenerateScriptWizardInfo(m_ScriptSource, false);
    else
      GenerateScriptWizardInfo(m_ScriptSource, true);
  }

  UpdateScriptListbox();
}

void CScriptWizard::UpdateScriptListbox() {
  CListBox *scrlistbox = (CListBox *)GetDlgItem(IDC_SCR_LISTBOX);
  int i;

  //	Add script names to list box
  scrlistbox->ResetContent();
  for (i = 0; i < MAX_SCRIPTS; i++) {
    if (Script_names[i].used)
      if (!stricmp(ScriptFileName, DEFAULT_SCRIPT_NAME) && !Script_names[i].iscustom)
        scrlistbox->AddString(Script_names[i].name);
      else if (stricmp(ScriptFileName, DEFAULT_SCRIPT_NAME) && Script_names[i].iscustom)
        scrlistbox->AddString(Script_names[i].name);
  }

  //	make sure we reselect the current string.
  if (scrlistbox->GetCount() > 0)
    scrlistbox->SetCurSel(m_CurScriptSel);
  else
    m_CurScriptSel = -1;

  m_CurEventSel = 0;

  UpdateEventListbox();
}

void CScriptWizard::UpdateEventListbox() {
  CListBox *listbox = (CListBox *)GetDlgItem(IDC_SCREVT_LISTBOX);
  CListBox *scrlistbox = (CListBox *)GetDlgItem(IDC_SCR_LISTBOX);
  int i, id;
  char scrname[MAX_SCRNAME];

  //	Add script names to list box
  listbox->ResetContent();

  if (m_CurScriptSel > -1) {
    //	get text from listbox, then get script ID from that name, since names are sorted alphabetically
    //	in the listbox.
    scrlistbox->GetText(m_CurScriptSel, scrname);
    id = FindScriptIDFromName(scrname);

    for (i = 0; i < MAX_SCREVTS; i++)
      listbox->AddString(Script_evt_names[i]);

    listbox->SetCurSel(m_CurEventSel);
  }
}

void CScriptWizard::OnSelchangeScrmodBox() {
  LoadCurrentModule();
  UpdateDialogButtons();
}

void CScriptWizard::UpdateDialogButtons() {
  CButton *btnunlock = (CButton *)GetDlgItem(IDC_UNLOCK);
  CButton *btnlock = (CButton *)GetDlgItem(IDC_LOCK);
  CButton *btnedit = (CButton *)GetDlgItem(IDC_EDITSCRIPT);
  CButton *btnevt = (CButton *)GetDlgItem(IDC_ADDEVENT);
  CButton *btnundolock = (CButton *)GetDlgItem(IDC_UNDOLOCK);
  CListBox *evtlb = (CListBox *)GetDlgItem(IDC_SCREVT_LISTBOX);

  int n;
  char str[128];

  //	take care of script locking and unlocking buttons
  if (stricmp(ScriptFileName, DEFAULT_SCRIPT_NAME) == 0)
    n = FindGamefileName(ScriptFileName);
  else
    n = -1;

  if (n == -1) {
    btnunlock->EnableWindow(FALSE);
    btnlock->EnableWindow(FALSE);
    btnundolock->EnableWindow(FALSE);
  } else if (mng_FindTrackLock(Gamefiles[n].name, PAGETYPE_GAMEFILE) == -1) {
    btnunlock->EnableWindow(FALSE);
    btnlock->EnableWindow(TRUE);
    btnundolock->EnableWindow(FALSE);
  } else {
    btnunlock->EnableWindow(TRUE);
    btnlock->EnableWindow(FALSE);
    btnundolock->EnableWindow(TRUE);
  }

  //	take care of Editing buttons
  if (evtlb->GetCurSel() == LB_ERR)
    btnevt->EnableWindow(FALSE);
  else {
    evtlb->GetText(m_CurEventSel, str);
    btnevt->EnableWindow(TRUE);
  }

  if (strlen(ScriptFileName) > 0)
    btnedit->EnableWindow(TRUE);
  else
    btnedit->EnableWindow(FALSE);
}
