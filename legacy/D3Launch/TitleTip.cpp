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

////////////////////////////////////////////////////////////////////////////
// TitleTip.cpp : implementation file
//

#include "stdafx.h"
#include "TitleTip.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTitleTip

CTitleTip::CTitleTip() {
  m_iHorzSpace = 2;
  m_brshBackground = (HBRUSH)0;

  m_clrBackground = ::GetSysColor(COLOR_HIGHLIGHT);
  m_clrText = ::GetSysColor(COLOR_HIGHLIGHTTEXT);

  RegisterWindowClass(); // Register window class if not already registered.
}

CTitleTip::~CTitleTip() {}

// static
void CTitleTip::RegisterWindowClass() {
  WNDCLASS wndcls;
  HINSTANCE hInst = AfxGetInstanceHandle();
  if (!(::GetClassInfo(hInst, TITLETIP_CLASSNAME, &wndcls))) {
    // otherwise we need to register a new class
    wndcls.style = CS_SAVEBITS | CS_DBLCLKS;
    wndcls.lpfnWndProc = ::DefWindowProc;
    wndcls.cbClsExtra = wndcls.cbWndExtra = 0;
    wndcls.hInstance = hInst;
    wndcls.hIcon = NULL;
    wndcls.hCursor = LoadCursor(hInst, IDC_ARROW);
    wndcls.hbrBackground = (HBRUSH)(COLOR_INFOBK + 1);
    wndcls.lpszMenuName = NULL;
    wndcls.lpszClassName = TITLETIP_CLASSNAME;
    if (!AfxRegisterClass(&wndcls))
      AfxThrowResourceException();
  }
}

BEGIN_MESSAGE_MAP(CTitleTip, CWnd)
//{{AFX_MSG_MAP(CTitleTip)
ON_WM_MOUSEMOVE()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CTitleTip::SetBackground(HBRUSH brshBackground) { m_brshBackground = brshBackground; }

void CTitleTip::SetBkColor(COLORREF crColor) { m_clrBackground = crColor; }

void CTitleTip::SetTextColor(COLORREF crColor) { m_clrText = crColor; }

/////////////////////////////////////////////////////////////////////////////
// CTitleTip message handlers

/*
 * styles:
 * <UL>
 *   <LI>WS_BORDER: draws a border around the titletip window
 *   <LI>WS_POPUP:  needed so that the TitleTip window is able to extend
 *     beyond the boundary of the control
 *   <LI>WS_EX_TOOLWINDOW: stops the window from appearing in the task bar
 *   <LI>WS_EX_TOPMOST:    ensures the titletip is visible
 *   <LI>COLOR_INFOBK:     the same color used by ToolTip
 * </UL>
 */

BOOL CTitleTip::Create(CWnd *pParentWnd) {
  ASSERT_VALID(pParentWnd);

  DWORD dwStyle = WS_BORDER | WS_POPUP;
  DWORD dwExStyle = WS_EX_TOOLWINDOW | WS_EX_TOPMOST;
  m_pParentWnd = pParentWnd;
  return CreateEx(dwExStyle, TITLETIP_CLASSNAME, NULL, dwStyle, 0, 0, 0, 0, NULL, NULL, NULL);
}

/**
 * This gets called repeatedly by its parent control.
 * Determines whether the text extent of pszTitleText is too big for the
 * original rectangle (rectTitle) and displays a TitleTip if required.
 *
 * @memo Displays the titletip if required.
 *
 * @param rectTitle  rectangle for the original title - in client coordinates
 * @param sTitleText text to be displayed
 * @param xoffset    number of pixel the text is offset from left border of the cell
 */

void CTitleTip::Show(CRect rectTitle, CString sTitleText, int xoffset /*=0*/) {
  ASSERT(::IsWindow(m_hWnd));
  ASSERT(!rectTitle.IsRectEmpty());

  if (GetFocus() == NULL) // only display titletip if app has focus
    return;

  // Define the rectangle outside which the titletip will be hidden.
  m_rectTitle.top = 0;
  m_rectTitle.left = -xoffset;
  m_rectTitle.right = rectTitle.Width() - xoffset;
  m_rectTitle.bottom = rectTitle.Height();

  m_pParentWnd->ClientToScreen(rectTitle); // Determine the width of the text

  CClientDC dc(this);
  int iSavedDC = dc.SaveDC(); // Save DC state

  dc.SelectObject(m_pParentWnd->GetFont()); // use same font as ctrl

  CSize size = dc.GetTextExtent(sTitleText);
  CRect rectDisplay = rectTitle;
  rectDisplay.left += xoffset - GetHorzSpace();
  rectDisplay.right = rectDisplay.left + size.cx + 2 + 2 * GetHorzSpace();

  // Do not display if the text fits within available space
  if (size.cx <= rectTitle.Width() - 2 * xoffset - 2)
    return;

  // Show the titletip
  SetWindowPos(&wndTop, rectDisplay.left, rectDisplay.top, rectDisplay.Width(), rectDisplay.Height(),
               SWP_SHOWWINDOW | SWP_NOACTIVATE);

  if (m_brshBackground) {
    dc.SetTextColor(m_clrText);
    dc.SetBkColor(m_clrBackground);
    dc.SetBkMode(OPAQUE);
    dc.SelectObject(CBrush::FromHandle(m_brshBackground));
    dc.FillSolidRect(0, 0, rectDisplay.Width(), rectDisplay.Height(), m_clrBackground);
  } else {
    dc.SetBkMode(TRANSPARENT);
  }

  dc.TextOut(GetHorzSpace(), 0, sTitleText);

  dc.RestoreDC(iSavedDC); // Restore DC.

  SetCapture();
}

/*
 * Hide if the mouse is outside the original rectangle. Note that this is
 * smaller than the actual window rectangle.
 */
void CTitleTip::OnMouseMove(UINT nFlags, CPoint point) {
  if (!m_rectTitle.PtInRect(point)) {
    ReleaseCapture();
    ShowWindow(SW_HIDE);

    // Forward the message
    ClientToScreen(&point);
    CWnd *pWnd = WindowFromPoint(point);
    if (pWnd == this)
      pWnd = m_pParentWnd;
    int hittest = (int)pWnd->SendMessage(WM_NCHITTEST, 0, MAKELONG(point.x, point.y));
    if (hittest == HTCLIENT) {
      pWnd->ScreenToClient(&point);
      pWnd->PostMessage(WM_MOUSEMOVE, nFlags, MAKELONG(point.x, point.y));
    } else {
      pWnd->PostMessage(WM_NCMOUSEMOVE, hittest, MAKELONG(point.x, point.y));
    }
  }
}

BOOL CTitleTip::PreTranslateMessage(MSG *pMsg) {
  CWnd *pWnd;
  int hittest;
  switch (pMsg->message) {
  case WM_LBUTTONDBLCLK:
  case WM_RBUTTONDBLCLK:
  case WM_MBUTTONDBLCLK:
  case WM_LBUTTONUP:
  case WM_RBUTTONUP:
  case WM_MBUTTONUP:
  case WM_LBUTTONDOWN:
  case WM_RBUTTONDOWN:
  case WM_MBUTTONDOWN:
    POINTS pts = MAKEPOINTS(pMsg->lParam);
    POINT point;
    point.x = pts.x;
    point.y = pts.y;
    ClientToScreen(&point);
    pWnd = WindowFromPoint(point);
    if (pWnd == this)
      pWnd = m_pParentWnd;

    hittest = (int)pWnd->SendMessage(WM_NCHITTEST, 0, MAKELONG(point.x, point.y));
    if (hittest == HTCLIENT) {
      pWnd->ScreenToClient(&point);
      pMsg->lParam = MAKELONG(point.x, point.y);
    } else {
      switch (pMsg->message) {
      case WM_LBUTTONDOWN:
        pMsg->message = WM_NCLBUTTONDOWN;
        break;
      case WM_RBUTTONDOWN:
        pMsg->message = WM_NCRBUTTONDOWN;
        break;
      case WM_MBUTTONDOWN:
        pMsg->message = WM_NCMBUTTONDOWN;
        break;
      }
      pMsg->wParam = hittest;
      pMsg->lParam = MAKELONG(point.x, point.y);
      ShowWindow(SW_HIDE);
    }
    pWnd->SendMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
    if (GetCapture() == NULL)
      SetCapture();
    return TRUE;
  case WM_KEYDOWN:
  case WM_SYSKEYDOWN:
    ReleaseCapture();
    ShowWindow(SW_HIDE);
    m_pParentWnd->PostMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
    return TRUE;
  }

  if (GetFocus() == NULL) {
    ReleaseCapture();
    ShowWindow(SW_HIDE);
    return TRUE;
  }

  return CWnd::PreTranslateMessage(pMsg);
}
