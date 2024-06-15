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

// TableFileEdit.cpp : implementation file
//

#include "stdafx.h"
#include "editor.h"
#include "TableFileEdit.h"
#include "TableManage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTableFileEdit dialog

CTableFileEdit::CTableFileEdit(CWnd *pParent /*=NULL*/) : CDialog(CTableFileEdit::IDD, pParent) {
  //{{AFX_DATA_INIT(CTableFileEdit)
  //}}AFX_DATA_INIT
}

void CTableFileEdit::DoDataExchange(CDataExchange *pDX) {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CTableFileEdit)
  DDX_Control(pDX, IDC_TABLEEDIT_SCREEN_NAME_EDIT, m_ScreenNameBox);
  DDX_Control(pDX, IDC_TABLEEDIT_DESCRIPTION_EDIT, m_DescriptionBox);
  DDX_Control(pDX, IDC_TABLEEDIT_NAME_LIST, m_NameList);
  //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTableFileEdit, CDialog)
//{{AFX_MSG_MAP(CTableFileEdit)
ON_COMMAND(ID_TABLEEDIT_FILE_LOAD, OnTableEditFileLoad)
ON_COMMAND(ID_TABLEEDIT_FILE_EXIT, OnTableEditFileExit)
ON_CBN_SELCHANGE(IDC_TABLEEDIT_NAME_LIST, OnSelChangeTableEditNameList)
ON_BN_CLICKED(IDC_TABLEEDIT_BTN_NEXT, OnTableEditBtnNext)
ON_BN_CLICKED(IDC_TABLEEDIT_BTN_PREVIOUS, OnTableEditBtnPrevious)
ON_COMMAND(ID_TABLEEDIT_FILE_SAVE, OnTableEditFileSave)
ON_COMMAND(ID_TABLEEDIT_FILE_SAVE_AS, OnTableEditFileSaveAs)
ON_UPDATE_COMMAND_UI(ID_TABLEEDIT_FILE_SAVE, OnUpdateTableEditFileSave)
ON_UPDATE_COMMAND_UI(ID_TABLEEDIT_FILE_SAVE_AS, OnUpdateTableEditFileSaveAs)
ON_WM_CLOSE()
ON_EN_CHANGE(IDC_TABLEEDIT_DESCRIPTION_EDIT, OnChangeTableEditDescriptionEdit)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTableFileEdit message handlers

void CTableFileEdit::OnTableEditFileLoad() {
  // TODO: Add your command handler code here
  if (m_PageList.ModifiedPrompt(this) == IDNO)
    return;

  CString filters = "Outrage Table File (*.loc)|*.loc|All Files (*.*)|*.*||";

  CFileDialog dlg_open(TRUE, NULL, NULL, OFN_FILEMUSTEXIST, (LPCTSTR)filters, this);
  if (dlg_open.DoModal() == IDCANCEL) {
    return;
  }

  if (!m_PageList.LoadTable(dlg_open.GetPathName().GetBuffer(0))) {
    MessageBox("Out of Memory Error: LoadTable()\n", "Load Table Error!");
    m_PageList.ClearList();
    return;
  }

  m_PageList.FillFromList(&m_NameList);
  EnableAll(TRUE);

  m_PageList.DisplaySelected(&m_NameList, &m_DescriptionBox);
  m_PageList.SetTitleString(this);
}

void CTableFileEdit::OnTableEditFileExit() {
  // TODO: Add your command handler code here
  if (m_PageList.ModifiedPrompt(this) == IDYES)
    EndDialog(0);
}

void CTableFileEdit::OnSelChangeTableEditNameList() {
  // TODO: Add your control notification handler code here
  int new_object_index;

  m_PageList.SaveSelected(&m_DescriptionBox);
  new_object_index = m_NameList.GetCurSel();
  m_PageList.SelectNode(new_object_index);
  m_PageList.DisplaySelected(&m_NameList, &m_DescriptionBox);
}

void CTableFileEdit::OnTableEditBtnNext() {
  // TODO: Add your control notification handler code here
  m_PageList.SaveSelected(&m_DescriptionBox);
  m_PageList.SelectNext();
  m_PageList.DisplaySelected(&m_NameList, &m_DescriptionBox);
}

void CTableFileEdit::OnTableEditBtnPrevious() {
  // TODO: Add your control notification handler code here
  m_PageList.SaveSelected(&m_DescriptionBox);
  m_PageList.SelectPrev();
  m_PageList.DisplaySelected(&m_NameList, &m_DescriptionBox);
}

BOOL CTableFileEdit::OnInitDialog() {
  CDialog::OnInitDialog();

  // TODO: Add extra initialization here
  m_ScreenNameBox.LimitText(0);
  m_DescriptionBox.LimitText(0);

  // Disable all items until a table file has been loaded
  EnableAll(FALSE);

  return TRUE; // return TRUE unless you set the focus to a control
               // EXCEPTION: OCX Property Pages should return FALSE
}

void CTableFileEdit::OnTableEditFileSave() {
  // TODO: Add your command handler code here
  m_PageList.SaveSelected(&m_DescriptionBox);

  if (!m_PageList.SaveTable(NULL)) {
    MessageBox("The save was unsuccessfull", "Save Table Error!");
    return;
  }
  m_PageList.SetTitleString(this);
}

void CTableFileEdit::OnTableEditFileSaveAs() {
  // TODO: Add your command handler code here
  m_PageList.SaveSelected(&m_DescriptionBox);

  CString filters = "Outrage Table File (*.loc)|*.loc|All Files (*.*)|*.*||";

  CFileDialog dlg_open(FALSE, NULL, NULL, 0, (LPCTSTR)filters, this);
  if (dlg_open.DoModal() == IDCANCEL) {
    return;
  }

  if (!m_PageList.SaveTable(dlg_open.GetPathName().GetBuffer(0))) {
    MessageBox("The save was unsuccessfull", "Save Table Error!");
    return;
  }
  m_PageList.SetTitleString(this);
}

void CTableFileEdit::OnUpdateTableEditFileSave(CCmdUI *pCmdUI) {
  // TODO: Add your command update UI handler code here
  pCmdUI->Enable(m_PageList.IsLoaded() && m_PageList.IsModified());
}

void CTableFileEdit::OnUpdateTableEditFileSaveAs(CCmdUI *pCmdUI) {
  // TODO: Add your command update UI handler code here
  pCmdUI->Enable(m_PageList.IsLoaded());
}

void CTableFileEdit::OnClose() {
  // TODO: Add your message handler code here and/or call default
  if (m_PageList.ModifiedPrompt(this) == IDYES)
    CDialog::OnClose();
}

void CTableFileEdit::OnChangeTableEditDescriptionEdit() {
  // TODO: If this is a RICHEDIT control, the control will not
  // send this notification unless you override the CDialog::OnInitDialog()
  // function to send the EM_SETEVENTMASK message to the control
  // with the ENM_CHANGE flag ORed into the lParam mask.

  // TODO: Add your control notification handler code here
  m_PageList.SetModified(TRUE);
  m_PageList.SetTitleString(this);
}

void CTableFileEdit::EnableAll(bool flag) {
  ((CButton *)GetDlgItem(IDC_TABLEEDIT_NAME_LIST))->EnableWindow(flag);
  ((CButton *)GetDlgItem(IDC_TABLEEDIT_BTN_PREVIOUS))->EnableWindow(flag);
  ((CButton *)GetDlgItem(IDC_TABLEEDIT_BTN_NEXT))->EnableWindow(flag);
  ((CButton *)GetDlgItem(IDC_TABLEEDIT_SCREEN_NAME_EDIT))->EnableWindow(flag);
  ((CButton *)GetDlgItem(IDC_TABLEEDIT_DESCRIPTION_EDIT))->EnableWindow(flag);
}
