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

/******************************************************************************\
* file   : ListCtrl.h
* created: 1997.09.22
*
* description:
* Implements a CListCtrl which can mark full rows.
\******************************************************************************/

#ifndef _CLISTCTRLEX_H_
#define _CLISTCTRLEX_H_

#include "TitleTip.h"

#define LVNU_SELCHANGED 0x1000

/**
 * A super CListControl.
 * <P>features:
 * <UL>
 *   <LI>Title Tip item expantion
 *   <LI>full row selection
 *   <LI>notifies parent selection has changed
 *   <LI>
 * </UL>
 */

class CListCtrlEx : public CListCtrl {
  typedef CListCtrl inherited;

  DECLARE_DYNCREATE(CListCtrlEx)

  // Construction
public:
  CListCtrlEx();
  virtual ~CListCtrlEx();

  // Attributes
protected:
  BOOL m_bFullRowSel;
  BOOL m_bClientWidthSel;

public:
  BOOL SetFullRowSel(BOOL bFillRowSel);
  BOOL GetFullRowSel();

  // Overrides
protected:
  virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CListCtrlEx)
public:
  virtual BOOL PreCreateWindow(CREATESTRUCT &cs);

protected:
  virtual void PreSubclassWindow();
  //}}AFX_VIRTUAL

  // Implementation
public:
#ifdef _DEBUG
  virtual void Dump(CDumpContext &dc) const;
#endif

protected:
  void RepaintSelectedItems();

  // Implementation - list view colors
  COLORREF m_clrText;
  COLORREF m_clrTextBk;
  COLORREF m_clrBkgnd;
  CTitleTip m_titletip;

  afx_msg LRESULT OnSetTextColor(WPARAM wParam, LPARAM lParam);
  afx_msg LRESULT OnSetTextBkColor(WPARAM wParam, LPARAM lParam);
  afx_msg LRESULT OnSetBkColor(WPARAM wParam, LPARAM lParam);

  bool HitTestRowCol(CPoint &point, int &iRow, int &iColumn) const;
  CRect GetCellRect(int iRow, int iColumn) const;

  int GetTrueColumnWidth(int nCurrentPosition) const;
  CString GetTrueItemText(int row, int col) const;
  void HideTitleTip();
  void SendSelChangedNotification();
  void ShowTitleTip(CPoint point);

  // Generated message map functions
protected:
  //{{AFX_MSG(CListCtrlEx)
  afx_msg void OnPaint();
  afx_msg void OnSetFocus(CWnd *pOldWnd);
  afx_msg void OnKillFocus(CWnd *pNewWnd);
  afx_msg void OnMouseMove(UINT nFlags, CPoint point);
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
  afx_msg void OnDestroy();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
#endif
