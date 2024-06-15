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

 * $Logfile: /DescentIII/Main/editor/ObjectDialog.h $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:57:38 $
 * $Author: kevinb $
 *
 * Object keypad dialog
 *
 * $Log: not supported by cvs2svn $
 *
 * 14    5/08/99 1:39a Matt
 * Added a function to delete all objects of a certain type, and support
 * for placing and attaching groups to the terrain.
 *
 * 13    3/27/99 12:52p Chris
 * The editor object placement code now allows objects to be pushed into
 * wall (when a toggle box is checked)
 *
 * 12    2/03/99 1:20a Matt
 * Made the object combo box on the object tab sort, and replaced the
 * listbox of pictures with a big picture of the current item.
 *
 * 11    1/27/99 11:03a Samir
 * added rotate 90 button to object dialog.
 *
 * 10    10/22/98 11:02a Chris
 * Added object regrounding
 *
 * 9     10/08/98 7:18p Chris
 * Added object swap types
 *
 * 8     8/24/98 12:24p Jason
 * added waypoints and player start position flags
 *
 * 7     5/18/98 2:56p Matt
 * Added code to readjust all ground object (for after the terrain has
 * moved).
 *
 * 6     1/07/98 6:39p Jason
 * Fixed player object number stuff
 *
 * 5     8/29/97 2:49p Samir
 * Added new object movement features.
 *
 * 4     8/25/97 7:39p Samir
 * Using new GrListBox from old editorPictListBox
 *
 * 3     8/12/97 10:57p Matt
 * Made object page work for the generic object  types (robot, powerup,
 * building, clutter)
 *
 * 2     7/24/97 2:54p Matt
 * Got rid of some member variables that kept track of current robot,
 * powerup, & object type, and used variables in D3EditState instead.
 *
 * 19    6/04/97 11:56a Samir
 * Added PowerupProp Dialog.
 *
 * 18    6/03/97 4:55p Mark
 *
 * 17    5/01/97 12:33p Jason
 * added reset objects function
 *
 * 16    4/15/97 11:29a Samir
 * Iimplemented menu item object placement and deletion.
 *
 * 15    4/03/97 5:08p Matt
 * Removed handlers for buttons that have been removed
 *
 * 14    4/03/97 1:18p Matt
 * Added handler code for object movement mode toggle button (on object
 * keypad)
 *
 * 13    4/02/97 12:22p Samir
 * Fixed object box scroll problems by keeping an array of item starts for
 * each obj_type and fixed problem of placing a bad robot (door) when
 * first placing objects.
 *
 * 12    3/25/97 6:31p Samir
 * Ability to select and probably place robots.
 *
 * 11    2/26/97 3:35p Mark
 *
 * 10    2/21/97 6:26p Samir
 * Object palette works (shitty scaling of powerups.)
 *
 * 9     2/20/97 4:26p Mark
 *
 * 8     2/19/97 5:26p Samir
 * When setting focus on objID listbox, update its contents.
 *
 * 7     2/19/97 2:16p Samir
 * Added RunKeypadFunction handler.
 *
 * 6     2/18/97 12:47p Samir
 * Able to select object types and ids from keypad.
 *
 * 5     2/17/97 6:20p Samir
 * Added Move and Placement of objects.
 *
 * 4     1/27/97 11:38a Samir
 * Added horizontal scrolling of keypad modeless dialog.
 *
 * $NoKeywords: $
 */

/////////////////////////////////////////////////////////////////////////////
// CObjectDialog dialog

#include "KeypadDialog.h"
#include "GrListBox.h"
#include "object.h"

class CObjectDialog;

class CObjectDialog : public CKeypadDialog {
  // Construction
public:
  CObjectDialog(CWnd *pParent = NULL); // standard constructor

  void PlaceObject();
  void DeleteObject();

  // Dialog Data
  //{{AFX_DATA(CObjectDialog)
  enum { IDD = IDD_OBJECTKEYPAD };
  // NOTE: the ClassWizard will add data members here
  //}}AFX_DATA

  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CObjectDialog)
protected:
  virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
                                                   //}}AFX_VIRTUAL

public:
  void UpdateIDList();

  // Given an item number, return the object ID
  int ItemToID(int item);

  // Given an object ID number, return the item number
  int IDToItem(int id);

  // Implementation
protected:
  // Generated message map functions
  //{{AFX_MSG(CObjectDialog)
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar);
  afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar);
  afx_msg void OnObjectPlaceObject();
  virtual BOOL OnInitDialog();
  afx_msg void OnSelchangeComboObjType();
  afx_msg void OnSelchangeComboObjID();
  afx_msg void OnObjPadDelobj();
  afx_msg void OnObjpadNextobj();
  afx_msg void OnSetfocusComboObjID();
  afx_msg void OnObjectpadGroup();
  afx_msg void OnObjContPreview();
  afx_msg void OnPaint();
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnObjpadFlagobjmulti();
  afx_msg void OnObjpadFlipobj();
  afx_msg void OnObjpadSetdefault();
  afx_msg void OnDestroy();
  afx_msg void OnResetObjects();
  afx_msg BOOL OnHelpInfo(HELPINFO *pHelpInfo);
  afx_msg void OnObjpadProperties();
  afx_msg void OnSelChangeCoordSysSelect();
  afx_msg void OnObjmoveX();
  afx_msg void OnObjmoveY();
  afx_msg void OnObjmoveXY();
  afx_msg void OnObjmoveZ();
  afx_msg void OnObjmoveP();
  afx_msg void OnObjmoveH();
  afx_msg void OnObjmoveB();
  afx_msg void OnObjmovePH();
  afx_msg void OnObjpadResetAllObjHeights();
  afx_msg void OnObjpadResetCurobjHeight();
  afx_msg void OnPrevStartPositon();
  afx_msg void OnNextStartPos();
  afx_msg void OnJumpToStartPos();
  afx_msg void OnRedCheck();
  afx_msg void OnBlueCheck();
  afx_msg void OnGreenCheck();
  afx_msg void OnYellowCheck();
  afx_msg void OnSelendokSwapSourceCombo();
  afx_msg void OnSelendokSwapDestCombo();
  afx_msg void OnObjectSwapButton();
  afx_msg void OnSelendokRegroundCombo();
  afx_msg void OnRegroundButton();
  afx_msg void OnObjRot90();
  afx_msg void OnObjectPushthroughwalls();
  afx_msg void OnObjpadDeleteAll();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()

private:
  void InitObjIDComboBox(CComboBox *box);
  void InitObjTypeComboBox(CComboBox *box);

private:
  int m_current_start_pos;

private:
  //	loads and saves settings for this keypad
  int GetFreePlayerIndex();
  void UpdateDialog();
  void DrawPicture(CWnd *wnd);
};
