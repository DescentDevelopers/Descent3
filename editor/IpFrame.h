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

// IpFrame.h : interface of the CInPlaceFrame class
//

#if !defined(AFX_IPFRAME_H__949F57DE_189E_11D1_BD9B_00A0C96ED595__INCLUDED_)
#define AFX_IPFRAME_H__949F57DE_189E_11D1_BD9B_00A0C96ED595__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CInPlaceFrame : public COleIPFrameWnd {
  DECLARE_DYNCREATE(CInPlaceFrame)
public:
  CInPlaceFrame();

  // Attributes
public:
  // Operations
public:
  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CInPlaceFrame)
public:
  virtual BOOL OnCreateControlBars(CFrameWnd *pWndFrame, CFrameWnd *pWndDoc);
  virtual BOOL PreCreateWindow(CREATESTRUCT &cs);
  //}}AFX_VIRTUAL

  // Implementation
public:
  virtual ~CInPlaceFrame();
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext &dc) const;
#endif

protected:
  CToolBar m_wndToolBar;
  COleDropTarget m_dropTarget;
  COleResizeBar m_wndResizeBar;

  // Generated message map functions
protected:
  //{{AFX_MSG(CInPlaceFrame)
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  // NOTE - the ClassWizard will add and remove member functions here.
  //    DO NOT EDIT what you see in these blocks of generated code!
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IPFRAME_H__949F57DE_189E_11D1_BD9B_00A0C96ED595__INCLUDED_)
