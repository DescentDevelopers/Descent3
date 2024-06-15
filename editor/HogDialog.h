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

// HogDialog.h : header file
//

#ifndef HOGDIALOG_H
#define HOGDIALOG_H

/////////////////////////////////////////////////////////////////////////////
// CHogDialog dialog

struct hog_library; // hogfile!

class CHogDialog : public CDialog {
  // Construction
public:
  CHogDialog(CWnd *pParent, char *filename); // standard constructor

  // Dialog Data
  //{{AFX_DATA(CHogDialog)
  enum { IDD = IDD_HOGDIALOG };
  // NOTE: the ClassWizard will add data members here
  //}}AFX_DATA

  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CHogDialog)
protected:
  virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
                                                   //}}AFX_VIRTUAL

  // Implementation
protected:
  // Generated message map functions
  //{{AFX_MSG(CHogDialog)
  virtual BOOL OnInitDialog();
  afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()

private:
  CListCtrl *m_ListCtrl;
  char *m_HogName;
  hog_library *m_Library;
};

#endif