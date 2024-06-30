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

// StatusDlg.cpp : implementation file
//

#include "stdafx.h"
#include "editor.h"
#include "StatusDlg.h"
#include "pserror.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStatusDlg dialog

CStatusDlg::CStatusDlg(CWnd *pParent /*=NULL*/) : CDialog(CStatusDlg::IDD, pParent) {
  //{{AFX_DATA_INIT(CStatusDlg)
  // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT
}

void CStatusDlg::DoDataExchange(CDataExchange *pDX) {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CStatusDlg)
  // NOTE: the ClassWizard will add DDX and DDV calls here
  //}}AFX_DATA_MAP
}

void CStatusDlg::Init(int min, int max, int delta) {
  CProgressCtrl *progress;

  progress = (CProgressCtrl *)GetDlgItem(IDC_STATUSPROGRESS);
  progress->SetRange(min, max);
  progress->SetPos(min);
  progress->SetStep(delta);

  mprintf(0, "Progress Control Create: Min= %d Max= %d Step= %d\n", min, max, delta);
}

int CStatusDlg::Step() {
  CProgressCtrl *progress;

  progress = (CProgressCtrl *)GetDlgItem(IDC_STATUSPROGRESS);
  return progress->StepIt();
}

void CStatusDlg::Text(char *string) {
  CWnd *text;
  text = GetDlgItem(IDC_STATUSTEXT);
  text->SetWindowText(string);
}

void CStatusDlg::SetTo(int set) {
  CProgressCtrl *progress;

  progress = (CProgressCtrl *)GetDlgItem(IDC_STATUSPROGRESS);
  progress->SetPos(set);
}

BEGIN_MESSAGE_MAP(CStatusDlg, CDialog)
//{{AFX_MSG_MAP(CStatusDlg)
// NOTE: the ClassWizard will add message map macros here
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStatusDlg message handlers

// Initializes the Progress Window.  This MUST MUST MUST be called first, the parent parameter can be
// NULL to set the parent to the main window.
// min/max: these are the range of the progress indicator
// iterations: If you plan on using CProgress:IncreaseProgress() then this is how many times you plan on
//				calling CProgress::IncreaseProgress().  If you are going to use
// CProgress::SetProgressPercentage() 				then you should call the version of InitProgress(CWnd
// *parent) below.
//  Returns true if the progress dialog was created
bool CProgress::InitProgress(fix min, fix max, int32_t iterations, CWnd *parent) {
  int nmin, nmax, Step;
  nmin = FixToInt(min);
  nmax = FixToInt(max);

  float delta;
  delta = (float)(((float)(nmax - nmin)) / ((float)iterations));
  while (delta < 1.0) {
    delta *= 10;
    nmax *= 10;
  }
  Step = (int)delta;
  m_Max = nmax;
  m_Min = nmin;

  if (!iterations)
    return false;
  m_StatusDlg = new CStatusDlg;
  if (!m_StatusDlg)
    return false;
  m_StatusDlg->Create(IDD_STATUSDLG, parent);
  m_StatusDlg->ShowWindow(SW_SHOW);
  m_StatusDlg->Init(nmin, nmax, Step);
  return true;
}

// This is a quick way of initializing the progress indicator, with a range 0-100 step size 1.  This is what you
// should call if you plan on using CProgress::SetProgressPercentage().  Again, parent can be NULL if you want it to
// be the main application as the parent
bool CProgress::InitProgress(CWnd *parent) {
  m_Max = 100;
  m_Min = 0;

  m_StatusDlg = new CStatusDlg;
  if (!m_StatusDlg)
    return false;
  m_StatusDlg->Create(IDD_STATUSDLG, parent);
  m_StatusDlg->ShowWindow(SW_SHOW);
  m_StatusDlg->Init(0, 100, 1);
  return true;
}

// YOU MUST call this after you are done with the progress indicator.  This frees up some allocated memory
void CProgress::DestroyProgress() {
  ASSERT(m_StatusDlg != NULL);
  m_StatusDlg->DestroyWindow();
  if (m_StatusDlg)
    delete m_StatusDlg;
}

// By default the progress indicator says "Please wait...", here you can change that text...make sure you call
// this after InitProgress()
void CProgress::SetProgressText(char *string) {
  ASSERT(m_StatusDlg != NULL);
  m_StatusDlg->Text(string);
}

// This is the iterator function.  Call this in your loop or whatever to increase the progress indicator.  You can also
// use CProgress::SetProgressPercentage() instead
bool CProgress::IncreaseProgress() {
  ASSERT(m_StatusDlg != NULL);
  int ret;
  ret = m_StatusDlg->Step();
  if (ret >= m_Max)
    return true;
  else
    return false;
}

// Sets the progress indicator to a certain percentage
void CProgress::SetProgressPercentage(int percent) {
  ASSERT(m_StatusDlg != NULL);
  float per;
  per = ((float)percent) / ((float)100.0);
  m_StatusDlg->SetTo(m_Min + ((int)(per * (m_Max - m_Min))));
}

void CProgress::SetProgressPercentage(float percent) {
  ASSERT(m_StatusDlg != NULL);
  m_StatusDlg->SetTo(m_Min + ((int)(percent * (m_Max - m_Min))));
}
