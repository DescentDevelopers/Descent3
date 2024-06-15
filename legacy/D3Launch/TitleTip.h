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

#if !defined(AFX_TITLETIP_H__FB05F243_E98F_11D0_82A3_20933B000000__INCLUDED_)
#define AFX_TITLETIP_H__FB05F243_E98F_11D0_82A3_20933B000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// TitleTip.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTitleTip window

#define TITLETIP_CLASSNAME _T("ZTitleTip")

/**
 * Titletips similar to tooltips. For ListView controls, TitleTips are used to
 * display text of those cells which are not wide enough to display the text
 * completely. TitleTips show up as soon as the mouse moves over the cell.
 */

class CTitleTip : public CWnd {
  // Construction
public:
  CTitleTip();
  virtual ~CTitleTip();

  // Attributes
public:
  // Operations
public:
  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CTitleTip)
public:
  virtual BOOL PreTranslateMessage(MSG *pMsg);
  virtual BOOL Create(CWnd *pParentWnd);
  //}}AFX_VIRTUAL

  // Implementation
public:
  int GetHorzSpace() { return m_iHorzSpace; }
  void SetBackground(HBRUSH brshBackground);
  void SetBkColor(COLORREF crColor);
  void SetTextColor(COLORREF crColor);
  void Show(CRect rectTitle, CString sTitleText, int xoffset = 0);

  static void RegisterWindowClass();

protected:
  CWnd *m_pParentWnd;
  CRect m_rectTitle;
  HBRUSH m_brshBackground;
  COLORREF m_clrBackground;
  COLORREF m_clrText;
  int m_iHorzSpace;

  // Generated message map functions
protected:
  //{{AFX_MSG(CTitleTip)
  afx_msg void OnMouseMove(UINT nFlags, CPoint point);
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations
// immediately before the previous line.

#endif // !defined(AFX_TITLETIP_H__FB05F243_E98F_11D0_82A3_20933B000000__INCLUDED_)
