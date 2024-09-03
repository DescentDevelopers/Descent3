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

 * $Logfile: /DescentIII/Main/editor/roomkeypaddialog.h $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:57:38 $
 * $Author: kevinb $
 *
 *	Room keypad
 *
 * $Log: not supported by cvs2svn $
 *
 * 35    10/21/99 1:33p Jeff
 * added checkbox to prevent certain rooms from lighting
 *
 * 34    4/29/99 5:44p Chris
 * Added the check for bad center points
 *
 * 33    4/28/99 1:39p Chris
 * Added the ability to block portals
 *
 * 32    4/14/99 7:21p Jason
 * added secret room checkbox
 *
 * 31    4/03/99 7:55p Matt
 * Use new keypad update system instead of timer to update.
 *
 * 30    3/31/99 5:15p Matt
 * Added checkboxes on the room tab to control rendering of faces based on
 * shell flag.
 *
 * 29    3/31/99 11:34a Matt
 * Got rid of unused forcefield checkbox on room tab
 *
 * 28    2/03/99 5:48p Matt
 * Added room damage system
 *
 * 27    1/08/99 5:37p Samir
 * reverb values per room.
 *
 * 26    12/22/98 2:03p Matt
 * Added room names, and made rooms not compress so that room numbers are
 * suitable for persistant uses.
 *
 * 25    10/06/98 6:08p Jason
 * added RF_TRIANGULATE so that special rooms (with non-planar UVS) can be
 * drawn correctly
 *
 * 24    10/04/98 2:35p Matt
 * Added debug options to limit rendering
 *
 * 23    8/28/98 6:21p Matt
 * Added a function to flip a face.
 *
 * 22    8/27/98 7:16p Jason
 * changes to dialog
 *
 * 21    8/17/98 6:40p Matt
 * Added ambient sound system
 *
 * 20    7/20/98 4:46p Jason
 * added BOA vis table stuff
 *
 * 19    7/16/98 12:06p Jason
 * added special flags to room structure
 *
 * 18    3/16/98 6:41p Jason
 * added goal room stuff
 *
 * 17    3/06/98 3:23p Jason
 * added lighting from satellites to indoor rooms
 *
 * 16    2/10/98 7:46p Matt
 * Added roompad check boxes for fuelcen and goals
 *
 * 15    2/10/98 1:12p Jason
 * added forcefields and saturating walls
 *
 * 14    11/05/97 9:31p Jason
 * made render_external_rooms button work
 *
 * 13    10/01/97 7:49p Matt
 * Added "External Room" checkbox on room keypad
 *
 * 12    9/14/97 9:48p Matt
 * Made CEditorView a friend class
 *
 * 11    9/10/97 5:13p Matt
 * Made small portal change to room keypad
 *
 * 10    9/06/97 10:53p Matt
 * Added portal stuff to room keypad
 *
 * 9     9/06/97 3:14p Matt
 * Added button to go to next portal
 * Added separate face & vert count displays for current room & cur
 * palette room
 * Use new PrintToDsgItem() function
 *
 * 8     9/02/97 12:14p Samir
 * Fixed scrolling.
 *
 * 7     8/29/97 3:57p Samir
 * This should add scrolling support.
 *
 * 6     8/21/97 5:59p Matt
 * Added next vertex & combine faces functions
 *
 * 5     8/01/97 4:56p Jason
 * added face moving
 *
 * 4     8/01/97 12:50p Jason
 * added code to support scaling of rooms/faces/edges
 *
 * 3     7/31/97 3:31p Jason
 * added functions to rotate portal rooms
 *
 * 3     6/27/97 4:15p Jason
 * added room stuff
 *
 * 2     6/27/97 3:10p Jason
 * added groovy stuff for rooms
 *
 * 1     6/27/97 11:38a Samir
 * Initial rev.
 *
 * $NoKeywords: $
 */

#if !defined(AFX_ROOMKEYPADDIALOG_H__DDBE5AC1_EEDC_11D0_941E_00000000E8E8__INCLUDED_)
#define AFX_ROOMKEYPADDIALOG_H__DDBE5AC1_EEDC_11D0_941E_00000000E8E8__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// RoomKeypadDialog.h : header file
//

#include "KeypadDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CRoomKeypadDialog dialog

class CRoomKeypadDialog : public CKeypadDialog {
  // Construction
public:
  CRoomKeypadDialog(CWnd *pParent = NULL); // standard constructor
  void UpdateKeypad(int mask);

  virtual void RunKeypadFunction(int code);

  // Dialog Data
  //{{AFX_DATA(CRoomKeypadDialog)
  enum { IDD = IDD_ROOMKEYPAD };
  // NOTE: the ClassWizard will add data members here
  //}}AFX_DATA

  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CRoomKeypadDialog)
protected:
  virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
                                                   //}}AFX_VIRTUAL

  // Implementation
protected:
  // Generated message map functions
  //{{AFX_MSG(CRoomKeypadDialog)
  afx_msg void OnLoadRoom();
  afx_msg void OnSaveRoomLocally();
  afx_msg void OnSelendokRoomPulldown();
  afx_msg void OnPaint();
  afx_msg BOOL OnHelpInfo(HELPINFO *pHelpInfo);
  afx_msg void OnMarkRoom();
  afx_msg void OnRoompadRotateLeft();
  afx_msg void OnRoompadRotateRight();
  afx_msg void OnRoompadPitchForward();
  afx_msg void OnRoompadPitchBackward();
  afx_msg void OnRoompadBankRight();
  afx_msg void OnRoompadBankLeft();
  afx_msg void OnRoompadNextEdge();
  afx_msg void OnRoompadExpandRoom();
  afx_msg void OnRoompadContractRoom();
  afx_msg void OnRoompadExpandFace();
  afx_msg void OnRoompadContractFace();
  afx_msg void OnRoompadExpandEdge();
  afx_msg void OnRoompadContractEdge();
  afx_msg void OnRoompadMoveForward();
  afx_msg void OnRoompadMoveBackward();
  afx_msg void OnRoompadMoveRight();
  afx_msg void OnRoompadMoveLeft();
  afx_msg void OnRoompadMoveUp();
  afx_msg void OnRoompadMoveDown();
  afx_msg void OnRoompadNextVertex();
  afx_msg void OnRoompadCombineFaces();
  afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar);
  afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar);
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnRoompadNextPortal();
  virtual BOOL OnInitDialog();
  afx_msg void OnRenderPortalFaces();
  afx_msg void OnEnablePortalFlythrough();
  afx_msg void OnExternalRoom();
  afx_msg void OnRenderExternalRooms();
  afx_msg void OnNextFace();
  afx_msg void OnRoompadGoal1();
  afx_msg void OnRoompadGoal2();
  afx_msg void OnRoompadRefuelingCenter();
  afx_msg void OnTouchesOutside();
  afx_msg void OnRoompadGoal3();
  afx_msg void OnRoompadGoal4();
  afx_msg void OnGoalfaceCheck();
  afx_msg void OnSpecial1();
  afx_msg void OnSpecial2();
  afx_msg void OnSpecial3();
  afx_msg void OnSpecial4();
  afx_msg void OnSpecial5();
  afx_msg void OnSpecial6();
  afx_msg void OnBoaVisButton();
  afx_msg void OnSelendokRoomAmbient();
  afx_msg void OnMirrorButton();
  afx_msg void OnNoMirrorButton();
  afx_msg void OnRoompadFlipFace();
  afx_msg void OnRoompadNoDrawTerrain();
  afx_msg void OnRoompadOneRoomOnly();
  afx_msg void OnTriangulate();
  afx_msg void OnEditName();
  afx_msg void OnSelchangeRoomEnvaudio();
  afx_msg void OnKillfocusRoompadDamage();
  afx_msg void OnSelchangeRoompadDamageType();
  afx_msg void OnRenderShell();
  afx_msg void OnRenderNonShell();
  afx_msg void OnSecretCheck();
  afx_msg void OnPortalBlockedCheck();
  afx_msg void OnPortalBlockedCanGoAwayCheck();
  afx_msg void OnBadCenterpoints();
  afx_msg void OnRoomSkipLighting();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()

private:
  void LoadSettings();
  void SaveSettings();

  static int m_Instances; // number of instances of this keypad.
  void UpdateDialog();

  friend class CEditorView;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ROOMKEYPADDIALOG_H__DDBE5AC1_EEDC_11D0_941E_00000000E8E8__INCLUDED_)
