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

// ListVwEx.h : interface of the CListViewEx class
//
// This class provedes a full row selection mode for the report
// mode list view control.
//
// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992-1997 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

class CListViewEx : public CListView {
  DECLARE_DYNCREATE(CListViewEx)

  // Construction
public:
  CListViewEx();

  // Attributes
protected:
  BOOL m_bFullRowSel;

public:
  BOOL SetFullRowSel(BOOL bFillRowSel);
  BOOL GetFullRowSel();

  BOOL m_bClientWidthSel;

  // Overrides
protected:
  virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CListViewEx)
public:
  virtual BOOL PreCreateWindow(CREATESTRUCT &cs);

protected:
  //}}AFX_VIRTUAL

  // Implementation
public:
  virtual ~CListViewEx();
#ifdef _DEBUG
  virtual void Dump(CDumpContext &dc) const;
#endif

protected:
  static LPCTSTR MakeShortString(CDC *pDC, LPCTSTR lpszLong, int nColumnLen, int nOffset);
  void RepaintSelectedItems();

  // Implementation - client area width
  int m_cxClient;

  // Implementation - state icon width
  int m_cxStateImageOffset;
  afx_msg LRESULT OnSetImageList(WPARAM wParam, LPARAM lParam);

  // Implementation - list view colors
  COLORREF m_clrText;
  COLORREF m_clrTextBk;
  COLORREF m_clrBkgnd;
  afx_msg LRESULT OnSetTextColor(WPARAM wParam, LPARAM lParam);
  afx_msg LRESULT OnSetTextBkColor(WPARAM wParam, LPARAM lParam);
  afx_msg LRESULT OnSetBkColor(WPARAM wParam, LPARAM lParam);

  // Generated message map functions
protected:
  //{{AFX_MSG(CListViewEx)
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnPaint();
  afx_msg void OnSetFocus(CWnd *pOldWnd);
  afx_msg void OnKillFocus(CWnd *pNewWnd);
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
