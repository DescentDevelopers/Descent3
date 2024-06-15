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

 * $Logfile: /DescentIII/Main/hogedit/HogEditView.h $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:57:56 $
 * $Author: kevinb $
 *
 *
 *
 * $Log: not supported by cvs2svn $
 *
 * 11    10/28/98 11:24a Nate
 * Added command line auto-processing (create hog from a rib)
 *
 * 10    9/17/98 4:29p Nate
 * Added Import Directory option.
 *
 * 9     8/16/98 4:22p Nate
 * Added message deferal and new hog info dialog
 *
 * 8     8/14/98 7:47p Nate
 * Removed "duplicate file" error message (upon File Inserts)
 *
 * 7     8/14/98 6:04p Nate
 * Added number of files field to status bar.
 *
 * 6     8/14/98 4:38p Nate
 * Fixed a few minor bugs and added better error reporting
 *
 * 5     8/14/98 1:01p Nate
 * Added better error reporting for the HogEditor
 *
 * 4     7/22/98 2:38p Nate
 * Added Modified File prompt when exiting
 *
 * 3     7/20/98 3:35p Nate
 * Added more Editing functionality
 *
 * 2     7/15/98 12:31p Nate
 * Initial version
 *
 * $NoKeywords: $
 */

// HogEditView.h : interface of the CHogEditView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_HOGEDITVIEW_H__48BE52C0_172F_11D2_8CBC_00A0C96ED60D__INCLUDED_)
#define AFX_HOGEDITVIEW_H__48BE52C0_172F_11D2_8CBC_00A0C96ED60D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// HogEdit Window Constants
#define FILE_NAME_COLUMN 0
#define FILE_TYPE_COLUMN 1
#define FILE_DIR_COLUMN 2
#define FILE_SIZE_COLUMN 3
#define FILE_TIME_COLUMN 4

#define FILE_NAME_TITLE "Filename"
#define FILE_TYPE_TITLE "Type"
#define FILE_DIR_TITLE "Directory"
#define FILE_SIZE_TITLE "Size"
#define FILE_TIME_TITLE "Timestamp"

#define FILE_NAME_COLUMN_WIDTH 0.20
#define FILE_TYPE_COLUMN_WIDTH 0.18
#define FILE_DIR_COLUMN_WIDTH 0.35
#define FILE_SIZE_COLUMN_WIDTH 0.07
#define FILE_TIME_COLUMN_WIDTH 0.20

// The size of the buffer to hold the list of "to-be-added" filenames
#define MAX_FILENAME_BUFFER 120000

class CHogEditView : public CListViewEx {
protected: // create from serialization only
  CHogEditView();
  DECLARE_DYNCREATE(CHogEditView)

  // Attributes
public:
  CHogEditDoc *GetDocument();

  // Operations
public:
  void SetColumnWidths(void);
  void GetFileType(char *type, char *filename);
  bool InsertItem(int index, hog_library_entry *entry);
  void UpdateList(void);
  void UpdateTitle(CString &path, bool mod_flag);

  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CHogEditView)
public:
  virtual void OnDraw(CDC *pDC); // overridden to draw this view
  virtual BOOL PreCreateWindow(CREATESTRUCT &cs);

protected:
  virtual void OnInitialUpdate(); // called first time after construct
                                  //}}AFX_VIRTUAL

  // Implementation
public:
  virtual ~CHogEditView();
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext &dc) const;
#endif

protected:
  // Generated message map functions
protected:
  //{{AFX_MSG(CHogEditView)
  afx_msg void OnFileNew();
  afx_msg void OnActionInsert();
  afx_msg void OnFileOpen();
  afx_msg void OnFileSave();
  afx_msg void OnFileSaveAs();
  afx_msg void OnActionDelete();
  afx_msg void OnActionCreate();
  afx_msg void OnViewFullRow();
  afx_msg void OnUpdateViewFullRow(CCmdUI *pCmdUI);
  afx_msg void OnActionUpdate();
  afx_msg void OnActionExtract();
  afx_msg void OnUpdateActionDelete(CCmdUI *pCmdUI);
  afx_msg void OnUpdateActionExtract(CCmdUI *pCmdUI);
  afx_msg void OnColumnclick(NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void OnActionImport();
  afx_msg LONG OnStraightToFileOpen(UINT, LONG);
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

// pass this message to myself when initializing if I should go straight to the
// update dialog
#define WM_STRAIGHT_TO_FILE_OPEN (WM_USER + 10)

#ifndef _DEBUG // debug version in HogEditView.cpp
inline CHogEditDoc *CHogEditView::GetDocument() { return (CHogEditDoc *)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HOGEDITVIEW_H__48BE52C0_172F_11D2_8CBC_00A0C96ED60D__INCLUDED_)
