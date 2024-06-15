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

 * $Logfile: /DescentIII/Main/editor/KeypadDialog.cpp $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:57:38 $
 * $Author: kevinb $
 *
 * Generic Keypad Dialog base class
 *
 * $Log: not supported by cvs2svn $
 *
 * 7     3/15/99 5:22p Chris
 * Fixed bug where the last entered data would be lost when a keypad was
 * scrolled
 *
 * 6     10/01/98 10:48a Samir
 * hopefully fixced scrolling problem.
 *
 * 5     10/02/97 6:47p Samir
 * Made m_Active static, so deactivating one keypad deactivates all.
 *
 * 4     8/25/97 12:08p Samir
 * Damn, another fuckup.
 *
 * 3     8/25/97 12:05p Samir
 * Oops, a fuckup fixed.
 *
 * 2     8/25/97 12:03p Samir
 * Fixed a lot of scrolling problems.
 *
 * 6     2/21/97 5:04p Samir
 * Set active flag to false when destroying to prevent corruption of
 * memory for some reason.
 *
 * 5     2/20/97 6:05p Samir
 * Allow for clean activation and deactivation of surfaces in editor when
 * going into fullscreen mode.
 *
 * 4     2/19/97 7:31p Samir
 * now scrolling for child and sibling windows.
 *
 * 3     1/27/97 11:38a Samir
 * Added horizontal scrolling of keypad modeless dialog.
 *
 * $NoKeywords: $
 */

// KeypadDialog.cpp : implementation file
//

#include "stdafx.h"
#include "editor.h"
#include "KeypadDialog.h"
#include "mono.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CKeypadDialog dialog

BOOL CKeypadDialog::m_Active = TRUE;

CKeypadDialog::CKeypadDialog(UINT id, CWnd *pParent /*=NULL*/) : CDialog(id, pParent) {
  //{{AFX_DATA_INIT(CKeypadDialog)
  // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT

  m_IDD = id;
  m_wndParent = pParent;
}

void CKeypadDialog::DoDataExchange(CDataExchange *pDX) {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CKeypadDialog)
  // NOTE: the ClassWizard will add DDX and DDV calls here
  //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CKeypadDialog, CDialog)
//{{AFX_MSG_MAP(CKeypadDialog)
ON_WM_SIZE()
ON_WM_VSCROLL()
ON_WM_HSCROLL()
ON_WM_DESTROY()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CKeypadDialog message handlers

BOOL CKeypadDialog::OnInitDialog() {
  CDialog::OnInitDialog();

  m_hpos = m_vpos = m_fullheight = m_fullwidth = 0;

  //	get dimensions of dialog.
  return TRUE; // return TRUE unless you set the focus to a control
               // EXCEPTION: OCX Property Pages should return FALSE
}

void CKeypadDialog::OnSize(UINT nType, int cx, int cy) {
  RECT rect;
  LONG style;

  CDialog::OnSize(nType, cx, cy);

  if (!m_sized) {
    // first time sizing initialization.
    m_fullwidth = cx;
    m_fullheight = cy;
    m_sized = true;
  }

  rect.left = 0;
  rect.right = cx;
  rect.top = 0;
  rect.bottom = cy;
  style = GetWindowLong(m_hWnd, GWL_STYLE);
  style = style & (WS_VSCROLL + WS_HSCROLL);
  AdjustWindowRect(&rect, style, FALSE);

  InitScrollBars(rect.right - rect.left, rect.bottom - rect.top);
}

void CKeypadDialog::PostNcDestroy() {
  // TODO: Add your specialized code here and/or call the base class

  delete this;
}

void CKeypadDialog::OnCancel() {
  // Do nothing:: So we force the program to manual close this modeless dialog
}

BOOL CKeypadDialog::Create() {
  BOOL res;

  m_sized = false;
  res = CDialog::Create(m_IDD, m_wndParent);

  return res;
}

void CKeypadDialog::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar) {
  SCROLLINFO si;
  int new_pos = 0;

  CDialog::SetFocus();

  if (!pScrollBar) {
    si.cbSize = sizeof(SCROLLINFO);
    si.fMask = SIF_ALL;
    GetScrollInfo(SB_VERT, &si);
    switch (nSBCode) {
    case SB_LINEUP:
      new_pos = si.nPos - 4;
      break;

    case SB_LINEDOWN:
      new_pos = si.nPos + 4;
      break;

    case SB_PAGEUP:
      new_pos = si.nPos - si.nPage;
      break;

    case SB_PAGEDOWN:
      new_pos = si.nPos + si.nPage;
      break;

    case SB_THUMBPOSITION:
      new_pos = nPos;
      break;

    case SB_THUMBTRACK:
      new_pos = nPos;
      break;

    default:
      new_pos = si.nPos;
    }

    //	make sure new_pos is in a valid range
    if (new_pos > (int)((si.nMax - si.nPage) + 1))
      new_pos = (si.nMax - si.nPage) + 1;
    if (new_pos < 0)
      new_pos = 0;

    if (new_pos != si.nPos) {
      //	We will use ScrollWindowEx to scroll the child windows of this dialog box.
      ScrollWindowEx(0, -(new_pos - si.nPos), NULL, NULL, NULL, NULL, SW_SCROLLCHILDREN | SW_INVALIDATE | SW_ERASE);
      UpdateWindow();

      si.cbSize = sizeof(si);
      si.fMask = SIF_POS;
      si.nPos = new_pos;
      SetScrollInfo(SB_VERT, &si, TRUE);
    }

    m_vpos = new_pos;
  }
  CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CKeypadDialog::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar) {
  SCROLLINFO si;
  int new_pos = 0;

  CDialog::SetFocus();

  if (!pScrollBar) {
    si.cbSize = sizeof(SCROLLINFO);
    si.fMask = SIF_ALL;
    GetScrollInfo(SB_HORZ, &si);
    switch (nSBCode) {
    case SB_LINELEFT:
      new_pos = si.nPos - 4;
      break;

    case SB_LINERIGHT:
      new_pos = si.nPos + 4;
      break;

    case SB_PAGELEFT:
      new_pos = si.nPos - si.nPage;
      break;

    case SB_PAGERIGHT:
      new_pos = si.nPos + si.nPage;
      break;

    case SB_THUMBPOSITION:
      new_pos = nPos;
      break;

    case SB_THUMBTRACK:
      new_pos = nPos;
      break;

    default:
      new_pos = si.nPos;
    }

    //	make sure new_pos is in a valid range
    if (new_pos > (int)((si.nMax - si.nPage) + 1))
      new_pos = (si.nMax - si.nPage) + 1;
    if (new_pos < 0)
      new_pos = 0;

    if (new_pos != si.nPos) {
      //	We will use ScrollWindowEx to scroll the child windows of this dialog box.
      ScrollWindowEx(-(new_pos - si.nPos), 0, NULL, NULL, NULL, NULL, SW_SCROLLCHILDREN | SW_INVALIDATE | SW_ERASE);
      UpdateWindow();

      si.cbSize = sizeof(si);
      si.fMask = SIF_POS;
      si.nPos = new_pos;
      SetScrollInfo(SB_HORZ, &si, TRUE);
    }

    m_hpos = new_pos;
  }

  CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CKeypadDialog::OnDestroy() {
  m_Active = FALSE;

  CDialog::OnDestroy();
}

void CKeypadDialog::InitScrollBars(int vis_w, int vis_h) {
  SCROLLINFO si;
  bool hscroll = false, vscroll = false;

  si.cbSize = sizeof(si);
  si.fMask = SIF_RANGE | SIF_POS | SIF_PAGE;
  si.nMin = 0;

  //	mprintf(0, "%d,%d -> %d,%d\n", m_fullwidth, m_fullheight, vis_w, vis_h);

  if (m_fullwidth > vis_w)
    hscroll = true;
  if (m_fullheight > vis_h)
    vscroll = true;

  if (hscroll) {
    ScrollWindowEx(m_hpos, 0, NULL, NULL, NULL, NULL, SW_SCROLLCHILDREN | SW_INVALIDATE | SW_ERASE);
    si.nMax = m_fullwidth - 1;
    si.nPage = vis_w - (vscroll ? GetSystemMetrics(SM_CXVSCROLL) : 0);
    si.nPos = 0;
    SetScrollInfo(SB_HORZ, &si);
    ShowScrollBar(SB_HORZ, TRUE);
  } else {
    ShowScrollBar(SB_HORZ, FALSE);
    ScrollWindowEx(m_hpos, 0, NULL, NULL, NULL, NULL, SW_SCROLLCHILDREN | SW_INVALIDATE | SW_ERASE);
    //		mprintf(0, "hoff=%d\n", m_hpos);
  }

  if (vscroll) {
    ScrollWindowEx(0, m_vpos, NULL, NULL, NULL, NULL, SW_SCROLLCHILDREN | SW_INVALIDATE | SW_ERASE);
    si.nMax = m_fullheight - 1;
    si.nPage = vis_h - (hscroll ? GetSystemMetrics(SM_CYHSCROLL) : 0);
    si.nPos = 0;
    SetScrollInfo(SB_VERT, &si);
    ShowScrollBar(SB_VERT, TRUE);
    m_vpos = 0;
  } else {
    ShowScrollBar(SB_VERT, FALSE);
    ScrollWindowEx(0, m_vpos, NULL, NULL, NULL, NULL, SW_SCROLLCHILDREN | SW_INVALIDATE | SW_ERASE);
    //		mprintf(0, "voff=%d\n", m_vpos);
  }

  m_hpos = 0;
  m_vpos = 0;
}
