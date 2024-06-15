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

 * $Logfile: /DescentIII/Main/editor/FloatingKeypadDialog.h $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:57:38 $
 * $Author: kevinb $
 *
 * Floating Keypad dialog
 *
 * $Log: not supported by cvs2svn $
 *
 * 4     8/27/98 6:42p Chris
 * Added the goal keypad and the start of the matcen keypad
 *
 * 3     9/17/97 12:58p Samir
 * Got rid of Curve Keypad.
 *
 * 2     8/29/97 2:49p Samir
 * Nuked SegmentKeypad.
 *
 * 10    6/27/97 12:19p Samir
 * Added room keypad dialog
 *
 * 9     6/05/97 3:37p Samir
 * Added megacell keypad.
 *
 * 8     4/08/97 10:42a Jason
 * implemented path following interface
 *
 * 7     4/04/97 3:15p Samir
 * Added paths keypad, removed player keypad since it's joined with object
 * keypad.
 *
 * 6     3/20/97 11:55a Jason
 * changes for terrain editing/drawing
 *
 * 5     3/05/97 11:49a Samir
 * Added doorway and terrain keypad tabs.
 *
 * 4     2/20/97 6:05p Samir
 * Allow for clean activation and deactivation of surfaces in editor when
 * going into fullscreen mode.
 *
 * 3     2/19/97 2:14p Samir
 * Added a RunKeypadFunction function.
 *
 * 2     1/23/97 4:16p Samir
 * Saves width and height, position of floating keypad.
 *
 * $NoKeywords: $
 */

// FloatingKeypadDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFloatingKeypadDialog dialog

class CTextureDialog;
class CObjectDialog;
class CLightingDialog;
class CPathPadDialog;
class CTriggerDialog;
class CRoomKeypadDialog;
class CDoorwayDialog;
class CKeypadDialog;
class CTerrainDialog;
class CMegacellKeypad;
class levelkeypad;
class matcenkeypad;

class CFloatingKeypadDialog : public CDialog {
  // Construction
public:
  CFloatingKeypadDialog(CWnd *pParent = NULL); // standard constructor

  BOOL Create();

  // Dialog Data
  //{{AFX_DATA(CFloatingKeypadDialog)
  enum { IDD = IDD_FLOATING_KEYPAD };
  // NOTE: the ClassWizard will add data members here
  //}}AFX_DATA

  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CFloatingKeypadDialog)
protected:
  virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
  virtual void PostNcDestroy();
  virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT *pResult);
  //}}AFX_VIRTUAL

public:
  void RunKeypadFunction(int code);
  void Deactivate(); // deactivates current tab
  void Activate();   // activates current tab.

  // Implementation
protected:
  // Generated message map functions
  //{{AFX_MSG(CFloatingKeypadDialog)
  virtual BOOL OnInitDialog();
  afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnMove(int x, int y);
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()

  virtual void OnCancel();

private:
  void DoKeypadTabNotify(NMHDR *); // take care of tab control notifications

private:
  CWnd *m_Parent; // parent window to this dialog

  CTabCtrl *m_KeypadTabCtrl; // this is the tab control in this dialog

  CKeypadDialog *m_KeypadTabDialog; // modeless dialog in tab control
  CTextureDialog *m_TextureDialog;
  CObjectDialog *m_ObjectDialog;
  CLightingDialog *m_LightingDialog;
  CPathPadDialog *m_PathPadDialog;
  CTriggerDialog *m_TriggerDialog;
  CDoorwayDialog *m_DoorwayDialog;
  CTerrainDialog *m_TerrainDialog;
  CMegacellKeypad *m_MegacellKeypad;
  CRoomKeypadDialog *m_RoomDialog;

  BOOL m_Initializing; // Are we done initializing?
};
