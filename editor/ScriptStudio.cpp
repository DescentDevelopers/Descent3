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

 * $Logfile: /DescentIII/Main/editor/ScriptStudio.cpp $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:57:39 $
 * $Author: kevinb $
 *
 *	Script Studio New Editor
 *
 * $Log: not supported by cvs2svn $
 *
 * 17    10/13/98 11:42a Samir
 * this may fix things?
 *
 * 16    10/08/98 4:24p Kevin
 * Changed code to comply with memory library usage. Always use mem_malloc
 * , mem_free and mem_strdup
 *
 * 15    7/13/98 11:13a Chris
 * Fixed the 65535 character limit with Kevin's help
 *
 * 14    6/23/98 2:43p Matt
 * Changed calls to OutrageMessageBox() & Debug_MessageBox() to deal with
 * int return value (instead of bool).
 *
 * 13    4/06/98 4:55p Samir
 * no bold text.
 *
 * 12    4/06/98 2:49p Samir
 * Fixed width font.
 *
 * 11    3/24/98 6:35p Samir
 * added script studio exit
 *
 * 10    3/24/98 4:29p Samir
 * report correct line number.
 *
 * 9     3/04/98 5:54p Samir
 * Added line number gauge to script studio.
 *
 * 8     3/02/98 4:03p Samir
 * Created a function to open up a script studio window.
 *
 * 7     10/15/97 3:05p Samir
 * Improved paste a little bit.
 *
 * 6     9/26/97 4:15p Samir
 * Mostly working system to go to a certain script in a file.
 *
 * 5     9/25/97 5:28p Samir
 * Even newer script code due to more changes in ObjCScript.cpp
 *
 * 4     9/04/97 4:39p Matt
 * Added includes needed as a result of removing includes from d3edit.h
 *
 * 3     8/20/97 3:35p Samir
 * Enhanced ScriptStudio to use tabs!  Use new ScriptStudio instead of
 * ScriptEditor.
 *
 * $NoKeywords: $
 */

// ScriptStudio.cpp : implementation file
//

#include "mfc_compatibility.h"
#include "editor.h"
#include "ScriptStudio.h"
#include "OsirisStatusDlg.h"
#include "ObjCScript.h"
#include "mem.h"

#include "pserror.h"
#include "d3x.h"
#include "ddio.h"
#include "cfile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MAX_SCRIPT_LINE_SIZE 800

static UINT WM_FINDREPLACE = RegisterWindowMessage(FINDMSGSTRING);

/////////////////////////////////////////////////////////////////////////////
// CScriptStudio dialog

CScriptStudio::CScriptStudio(CWnd *pParent /*=NULL*/) : CDialog(CScriptStudio::IDD, pParent) {
  //{{AFX_DATA_INIT(CScriptStudio)
  // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT

  m_pdlgFindReplace = NULL;
}

void CScriptStudio::DoDataExchange(CDataExchange *pDX) {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CScriptStudio)
  // NOTE: the ClassWizard will add DDX and DDV calls here
  //}}AFX_DATA_MAP
}

void CScriptStudio::SetText(const CString &text) {
  m_EditText = text;
  m_EditInitial = text;
}

void CScriptStudio::SetD3XObject(tD3XProgram *prog) { m_D3XProgram = prog; }

void CScriptStudio::GetText(CString &text) { text = m_EditText; }

void CScriptStudio::UpdateEditText() {
  CHARRANGE chr;

  m_RichEdit.HideCaret();
  m_RichEdit.GetSel(chr);
  m_RichEdit.SetSel(0, -1);
  m_EditText = m_RichEdit.GetSelText();
  m_RichEdit.SetSel(chr);
  m_RichEdit.ShowCaret();

  //	m_RichEdit.GetWindowText(m_EditText);
}

void CScriptStudio::SelectText(const char *txt) { m_SelectText = txt; }

BEGIN_MESSAGE_MAP(CScriptStudio, CDialog)
//{{AFX_MSG_MAP(CScriptStudio)
ON_COMMAND(ID_OSIRIS_COMPILE_SCRIPT, OnOsirisCompileScript)
ON_COMMAND(ID_OSIRIS_COPY, OnOsirisCopy)
ON_COMMAND(ID_OSIRIS_CUT, OnOsirisCut)
ON_COMMAND(ID_OSIRIS_EXPORT_SCRIPT, OnOsirisExportScript)
ON_COMMAND(ID_OSIRIS_FIND, OnOsirisFind)
ON_COMMAND(ID_OSIRIS_FIND_AND_REPLACE, OnOsirisFindAndReplace)
ON_COMMAND(ID_OSIRIS_IMPORT_SCRIPT, OnOsirisImportScript)
ON_COMMAND(ID_OSIRIS_PASTE, OnOsirisPaste)
ON_COMMAND(ID_OSIRIS_SAVE_EXIT, OnOsirisSaveExit)
ON_COMMAND(ID_OSIRIS_SET_EXTERNAL, OnOsirisSetExternal)
ON_COMMAND(ID_OSIRIS_USE_EXTERNAL, OnOsirisUseExternal)
ON_UPDATE_COMMAND_UI(ID_OSIRIS_PASTE, OnUpdateOsirisPaste)
ON_NOTIFY(EN_MSGFILTER, IDC_SCRIPTVIEW, OnMsgfilterScriptView)
ON_WM_DESTROY()
ON_COMMAND(ID_EXIT, OnExit)
//}}AFX_MSG_MAP
ON_REGISTERED_MESSAGE(WM_FINDREPLACE, OnFindReplace)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScriptStudio message handlers

BOOL CScriptStudio::OnInitDialog() {
  RECT rect;
  CWnd *wnd;
  PARAFORMAT pf;
  CHARFORMAT cf;

  m_RichEditInit = false;

  CDialog::OnInitDialog();

  //	Create our Rich Edit Control add keyboard handler for those SPECIAL keys like TAB.
  wnd = GetDlgItem(IDC_SCRMOD_BOX);
  wnd->GetWindowRect(&rect);
  rect.left += 2;
  rect.right -= 2;
  rect.top += 2;
  rect.bottom -= 2;
  ScreenToClient(&rect);
  m_RichEdit.Create(WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_WANTRETURN | ES_AUTOHSCROLL | ES_AUTOVSCROLL |
                        ES_SAVESEL | ES_NOHIDESEL | WS_VSCROLL,
                    rect, this, IDC_SCRIPTVIEW);
  m_RichEdit.SetEventMask(ENM_KEYEVENTS | ENM_MOUSEEVENTS);

  m_RichEdit.LimitText(0);
  m_RichEdit.GetParaFormat(pf);

  pf.cbSize = sizeof(pf);
  pf.dwMask = PFM_TABSTOPS;
  pf.cTabCount = MAX_TAB_STOPS;
  for (int i = 0; i < pf.cTabCount; i++)
    pf.rgxTabs[i] = 360 * (i + 1);

  m_RichEdit.SetParaFormat(pf);

  m_RichEdit.SetWindowText((LPCSTR)m_EditText);
  FindNext((char *)(LPCSTR)m_SelectText);

  //	sets character format
  ZeroMemory(&cf, sizeof(cf));
  cf.dwMask = CFM_FACE | CFM_CHARSET | CFM_BOLD;
  strcpy(cf.szFaceName, "Courier");
  cf.bCharSet = DEFAULT_CHARSET;
  cf.bPitchAndFamily = FIXED_PITCH | FF_DONTCARE;
  m_RichEdit.SetDefaultCharFormat(cf);

  //	set linenumber
  CStatic *linenum = (CStatic *)GetDlgItem(IDC_LINENUM);
  char buf[8];

  sprintf(buf, "%d", m_RichEdit.LineFromChar(-1) + 1);
  linenum->SetWindowText(buf);

  return TRUE; // return TRUE unless you set the focus to a control
               // EXCEPTION: OCX Property Pages should return FALSE
}

void CScriptStudio::OnDestroy() { CDialog::OnDestroy(); }

//	The Key handler for the editor.   Handles TAB chars.
void CScriptStudio::OnMsgfilterScriptView(NMHDR *pNMHDR, LRESULT *pResult) {
  MSGFILTER *pMsgFilter = reinterpret_cast<MSGFILTER *>(pNMHDR);

  // TODO: The control will not send this notification unless you override the
  // CDialog::OnInitDialog() function to send the EM_SETEVENTMASK message
  // to the control with either the ENM_KEYEVENTS or ENM_MOUSEEVENTS flag
  // ORed into the lParam mask.

  switch (pMsgFilter->msg) {
  case WM_KEYDOWN:
    if (pMsgFilter->wParam == VK_TAB)
      m_RichEdit.ReplaceSel("\t");
    *pResult = 0;
    break;
  }

  CStatic *linenum = (CStatic *)GetDlgItem(IDC_LINENUM);
  char buf[8];

  sprintf(buf, "%d", m_RichEdit.LineFromChar(-1) + 1);
  linenum->SetWindowText(buf);
}

//	Process all commands to and from Rich Edit Control here.

BOOL CScriptStudio::OnCommand(WPARAM wParam, LPARAM lParam) {
  int nCode = HIWORD(wParam);
  int nID = LOWORD(wParam);

  switch (nID) {
  case IDC_SCRIPTVIEW:
    HandleEditControl(nID, nCode);
    break;
  }

  return CDialog::OnCommand(wParam, lParam);
}

void CScriptStudio::OnOK() {
  ddio_KeyFlush();
  UpdateEditText();

  CDialog::OnOK();
}

void CScriptStudio::OnCancel() {
  ddio_KeyFlush();

  if (m_EditInitial != m_EditText)
    if (OutrageMessageBox(MBOX_YESNO, "You have made changes to this script that won't be saved.  Are you sure?") !=
        IDYES)
      return;

  CDialog::OnCancel();
}

/////////////////////////////////////////////////////////////////////////////////////////////
//	Menu functions.

void CScriptStudio::OnOsirisSaveExit() {
  m_EditInitial = m_EditText;
  OnOK();
}

void CScriptStudio::OnOsirisSetExternal() {
  //	OnSetExternalViewer();
}

void CScriptStudio::OnOsirisUseExternal() {
  //	OnUseExternal();
}

void CScriptStudio::OnOsirisCompileScript() {
  COsirisStatusDlg progress_status;

  UpdateEditText();

  progress_status.SetScript(m_EditText.GetBuffer(1), m_D3XProgram);

  progress_status.DoModal();
}

void CScriptStudio::OnOsirisImportScript() {
  CString path;
  CString filter = "All Files (*.*)|*.*||";

  CFileDialog dlg(true, 0, 0, OFN_FILEMUSTEXIST, (LPCTSTR)filter, this);
  if (dlg.DoModal() == IDOK) {
    path = dlg.GetPathName();
    ImportScript(path.GetBuffer(1));
  }
}

void CScriptStudio::OnOsirisExportScript() {
  CString path;
  CString filter = "All Files (*.*)|*.*||";

  CFileDialog dlg(false, 0, 0, OFN_OVERWRITEPROMPT, (LPCTSTR)filter, this);
  if (dlg.DoModal() == IDOK) {
    path = dlg.GetPathName();
    ExportScript(path.GetBuffer(1));
  }
}

//////////////////////////////////////////////////////////////////////////////
//	Editing and Clipboard functionality

void CScriptStudio::OnOsirisFind() { InitFindReplace(true); }

void CScriptStudio::OnOsirisFindAndReplace() {
  // TODO: Add your command handler code here
  InitFindReplace(false);
}

void CScriptStudio::OnOsirisPaste() {
  m_RichEdit.Paste();
  m_RichEdit.RedrawWindow();
}

void CScriptStudio::OnOsirisCopy() { m_RichEdit.Copy(); }

void CScriptStudio::OnOsirisCut() { m_RichEdit.Cut(); }

void CScriptStudio::OnUpdateOsirisPaste(CCmdUI *pCmdUI) {
  // this doesn't work right...don't know why
  if (IsClipboardFormatAvailable(CF_TEXT))
    pCmdUI->Enable(true);
  else
    pCmdUI->Enable(false);
}

/////////////////////////////////////////////////////////////////////////////////////////////
//	These Functions deal with user interface issues within the editor like Find-Replace.

// This is the message handler of sorts for the Find/Replace dialog, it interprets the message and passes the control
// to the appropriate function
LONG CScriptStudio::OnFindReplace(WPARAM wParam, LPARAM lParam) {
  // Get a pointer to the calling dialog
  CFindReplaceDialog *pDlg = CFindReplaceDialog::GetNotifier(lParam);
  ASSERT(pDlg != NULL);

  // See what the user is up to out there...
  if (pDlg->IsTerminating()) {
    // Time to kill the dialog box
    return 0;
  }

  if (pDlg->ReplaceAll()) {
    // Put a call to your ReplaceAll() method here...
    CString find, repl;
    find = pDlg->GetFindString();
    repl = pDlg->GetReplaceString();
    if (pDlg->MatchCase())
      m_FindMatchCase = true;
    else
      m_FindMatchCase = false;
    ReplaceAll(find.GetBuffer(1), repl.GetBuffer(1));
    return 0;
  }

  if (pDlg->ReplaceCurrent()) {
    // Put a call to your ReplaceCurrent() method here...
    CString repl;
    repl = pDlg->GetReplaceString();
    if (pDlg->MatchCase())
      m_FindMatchCase = true;
    else
      m_FindMatchCase = false;
    ReplaceCurrent(repl.GetBuffer(1));
    return 0;
  }

  CString str;
  if (pDlg->MatchCase())
    m_FindMatchCase = true;
  else
    m_FindMatchCase = false;
  str = pDlg->GetFindString();

  FindNext(str.GetBuffer(1));

  return 0;
}

// This initializes a Find/Replace dialog box (if bFind==false it sets up a Find and Replace, else it's just Find)
void CScriptStudio::InitFindReplace(BOOL bFind) {
  //
  // If the dialog has been called already, and then closed,
  // the pointer is still non-NULL, but the object is already
  // dead, so reset the pointer!
  //
  m_pdlgFindReplace = NULL;
  m_LastWord = " ";

  // Construct the dialog
  m_pdlgFindReplace = new CFindReplaceDialog;
  ASSERT(m_pdlgFindReplace != NULL);

  // Initialize the dialog
  if (!m_pdlgFindReplace->Create(bFind, 0, 0, FR_DOWN, this)) {
    mprintf(0, "Error allocating find/replace dialog!");
    m_pdlgFindReplace = NULL;
    return;
  }

  // Display the dialog
  ASSERT(m_pdlgFindReplace != NULL);
  m_pdlgFindReplace->SetActiveWindow();
  m_pdlgFindReplace->ShowWindow(SW_SHOW);
}

// This will look for the next series of charaters (given by the null terminated w)
// If it finds the string, it will highlight/select it and move the caret to the beginning of the word
// returns true if it found the string
// else false
bool CScriptStudio::FindNext(char *w) {
  int index, max;
  bool word_found;
  bool new_word;
  static int start = 0;

  word_found = false;

  // See if this is a new word to look for (as compared to the last searched word)
  if (m_LastWord != w) {
    m_LastWord = w;
    start = 0;
    new_word = true;
  } else
    new_word = false;

  // This will update the m_sScript string so it has the current script in it
  UpdateEditText();

  // Make a copy of the script to work with
  char *text;
  text = (char *)mem_malloc(m_EditText.GetLength() + 1);
  if (!text)
    Int3();
  strcpy(text, (LPCSTR)m_EditText);
  max = m_EditText.GetLength();

  // Make sure we aren't gonna try to go past the end of the buffer, start at 0 if we are
  if (start + (signed)strlen(w) >= max - 1)
    start = 0;

  // Here we begin the search
  for (index = start; index < max; index++) {
    // check the character it's at in the buffer to the first character of the find string, if they are the same
    // it's a possibility, so Check the word
    if (toupper(text[index]) == toupper(w[0]))
      if (CheckWord(index, text, w)) {
        // We got a match, so move the caret, and select the word
        SetCurrentIndex(index);
        SetSelection(index, strlen(w));
        word_found = true;
        // adjust start so on the next FindNext() it will start from the next character
        start = index + 1;
        index = max;
      }
  }

  if (text)
    mem_free(text);

  if (!word_found) {
    // This can mean 2 things.  We searched to the end of the file and either never found a word, or we did at one time

    // either way, start back at the beginning next time
    start = 0;

    // if it was a new word then we never found it
    if (new_word) {
      OutrageMessageBox("Word Not Found!");
      m_LastWord = " ";
    } else {
      OutrageMessageBox("Searched to end of file");
    }
    return false;
  }

  return true;
}

// This selects a word/string in the edit box, given the starting zero based character index and the length of the
// string
void CScriptStudio::SetSelection(int index, int length) {
  m_RichEdit.SetActiveWindow();
  m_RichEdit.SetFocus();
  m_RichEdit.ShowCaret();
  m_RichEdit.SetSel(-1, 0);
  m_RichEdit.SetSel(index, index + length);
}

// This function clears all selections
void CScriptStudio::RemoveAllSelections() {
  m_RichEdit.SetFocus();
  m_RichEdit.ShowCaret();
  m_RichEdit.SetSel(-1, 0);
}

// Replaces the selected word with the passed word
void CScriptStudio::ReplaceSelected(char *replace_word) {
  long start_index, end_index;

  UpdateData(false);

  m_RichEdit.SetFocus();
  m_RichEdit.GetSel(start_index, end_index);
  m_RichEdit.ReplaceSel(replace_word);

  UpdateData();

  SetCurrentIndex(start_index);
}

// This will start from the beginning and replace all the occurences
void CScriptStudio::ReplaceAll(char *find_word, char *replace_word) {
  SetCurrentIndex(0);
  m_LastWord = " ";
  while (FindNext(find_word)) {
    ReplaceSelected(replace_word);
  }
}

// This only replaces the selected string with the replace_word
void CScriptStudio::ReplaceCurrent(char *replace_word) { ReplaceSelected(replace_word); }

// This function moves the caret to the position specified in index
void CScriptStudio::SetCurrentIndex(int index) {
  CPoint point;
  m_RichEdit.SetFocus();
  m_RichEdit.ShowCaret();
  point = m_RichEdit.GetCharPos(index);
  m_RichEdit.SetCaretPos(point);
}

// This function checks to see if the string w is the same as the string that begins in text[index]
// returns true if it is
bool CScriptStudio::CheckWord(int index, char *text, char *w) {
  int word_length = strlen(w);
  int buffer_size = strlen(text);
  int count;
  if (m_FindMatchCase) {
    for (count = 0; count < word_length; count++) {
      if (index + count >= buffer_size)
        return false;
      if (text[index + count] != w[count])
        return false;
    }
  } else {
    for (count = 0; count < word_length; count++) {
      if (index + count >= buffer_size)
        return false;
      if (toupper(text[index + count]) != toupper(w[count]))
        return false;
    }
  }

  return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//	These function deal with importing and exporting script files.

void CScriptStudio::ExportScript(char *filename) {
  CFILE *file;

  UpdateEditText();
  file = cfopen(filename, "wt");
  mprintf(0, "Exporting Script to file %s\n", filename);
  cf_WriteString(file, m_EditText.GetBuffer(1));
  cfclose(file);
}

void CScriptStudio::ImportScript(char *filename) {
  CFILE *file;
  CString temp;
  int size;
  char buffer[MAX_SCRIPT_LINE_SIZE];

  memset(buffer, 0, MAX_SCRIPT_LINE_SIZE);

  if (!cfexist(filename)) {
    OutrageMessageBox("File not found!");
    return;
  }

  file = cfopen(filename, "rt");
  mprintf(0, "Importing Script from file %s\n", filename);
  do {
    size = cf_ReadString(buffer, MAX_SCRIPT_LINE_SIZE - 2, file);
    buffer[size] = '\xd';
    buffer[size + 1] = '\xa';
    buffer[size + 2] = '\0';
    temp += buffer;
    memset(buffer, 0, size);
  } while (!cfeof(file));

  cfclose(file);

  m_EditText = temp;
  m_RichEdit.SetWindowText((LPCSTR)m_EditText);
}

void CScriptStudio::HandleEditControl(int id, int code) {
  CRichEditCtrl *rec = (CRichEditCtrl *)GetDlgItem(id);

  switch (code) {
  case EN_UPDATE:
    break;
  }
}

bool InvokeScriptStudio(const char *filename, const char *scrname) {
#if 0 // LGT: D3XReallocProgram undefined
	char *source;
	CScriptStudio studio;
	tD3XProgram *script;
	bool ret = false;

//	initialize script studio.
	script = D3XReallocProgram(NULL, 0, 0, 0);					 
	if (!script) {
		OutrageMessageBox("Failure to compile script!   Something is wrong!");
		return false;
	}
	source = LoadScript(filename);
	if (!source) {
		OutrageMessageBox("Unable to find script for level. Something is wrong!");
		return false;
	}
	studio.SelectText(scrname);
	studio.SetD3XObject(script);
	studio.SetText(CString(source));
	FreeScript(source);

	if (studio.DoModal() == IDOK) {
		CString tempstr;
		studio.GetText(tempstr);
		source = (char *) mem_malloc(tempstr.GetLength()+1);
		strcpy(source, tempstr);
		SaveScript(filename, source);
		mem_free(source);
 		ret = true;
	}
	else {
		OutrageMessageBox("The level script could not compile and is invalid until corrected.");
		ret = false;
	}

	D3XFreeProgram(script);

	return ret;
#else
  return false;
#endif
}

void CScriptStudio::OnExit() {
  // TODO: Add your command handler code here

  CDialog::OnOK();
}
