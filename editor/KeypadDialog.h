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

 * $Logfile: /DescentIII/Main/editor/KeypadDialog.h $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:57:38 $
 * $Author: kevinb $
 *
 * Generic keypad dialog base class
 *
 * $Log: not supported by cvs2svn $
 *
 * 8     5/02/99 9:23p Matt
 * Notify the level keypad when the level has changed, and then re-init
 * the combo boxes.
 *
 * 7     4/03/99 7:54p Matt
 * Finished implementing spiffy keypad update system.
 *
 * 6     3/10/99 7:17p Samir
 * added function that updates keypads.
 *
 * 5     10/01/98 10:48a Samir
 * hopefully fixced scrolling problem.
 *
 * 4     4/16/98 6:39p Kevin
 * made CKeypadDialog::Activate static.
 *
 * 3     10/02/97 6:47p Samir
 * Made m_Active static, so deactivating one keypad deactivates all.
 *
 * 2     8/25/97 12:04p Samir
 * Fixed some scrolling problems.
 *
 * 7     2/21/97 5:04p Samir
 * Set active flag to false when destroying to prevent corruption of
 * memory for some reason.
 *
 * 6     2/20/97 6:05p Samir
 * Allow for clean activation and deactivation of surfaces in editor when
 * going into fullscreen mode.
 *
 * 5     2/20/97 5:39p Samir
 * Added Deactivate virtual function.
 *
 * 4     2/19/97 2:15p Samir
 * Added a RunKeypadFunction where children can inherits this function to
 * add keypad control.
 *
 * 3     1/27/97 11:38a Samir
 * Added horizontal scrolling of keypad modeless dialog.
 *
 * $NoKeywords: $
 */

#ifndef M_KEYPAD_DIALOG_H
#define M_KEYPAD_DIALOG_H

/////////////////////////////////////////////////////////////////////////////
// CKeypadDialog dialog

class CKeypadDialog : public CDialog {
  // Construction
public:
  CKeypadDialog(UINT id, CWnd *pParent = NULL); // standard constructor

  BOOL Create();

  // Dialog Data
  //{{AFX_DATA(CKeypadDialog)
  enum { IDD = IDD_KEYPAD_DLGBAR };
  // NOTE: the ClassWizard will add data members here
  //}}AFX_DATA

  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CKeypadDialog)
protected:
  virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
  virtual void PostNcDestroy();
  //}}AFX_VIRTUAL

protected:
  static BOOL m_Active; // is keypad active?

public:
  virtual void RunKeypadFunction(int code) {};
  virtual void UpdateKeypad(int mask) {};

  static void Deactivate() { CKeypadDialog::m_Active = FALSE; }; // called when shutting down keypad.
  static void Activate() { CKeypadDialog::m_Active = TRUE; };

  // Implementation
protected:
  CWnd *m_wndParent; // Parent window to dialog

  virtual void OnCancel();

  // Generated message map functions
  //{{AFX_MSG(CKeypadDialog)
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar);
  afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar);
  afx_msg void OnDestroy();
  virtual BOOL OnInitDialog();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()

private:
  UINT m_IDD;
  bool m_sized;
  int m_fullwidth, m_fullheight, m_hpos, m_vpos;
  void InitScrollBars(int vis_w, int vis_h);
};

// Keypad update flags
#define KUF_CURROOM_CHANGED (1 << 0)
#define KUF_CURFACE_CHANGED (1 << 1)
#define KUF_CURPORTAL_CHANGED (1 << 2)
#define KUF_CUROBJ_CHANGED (1 << 3)
#define KUF_CUROBJ_NAME_CHANGED (1 << 4)
#define KUF_LEVEL_LOADED (1 << 5)

#endif
