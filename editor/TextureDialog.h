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

 * $Logfile: /DescentIII/Main/editor/TextureDialog.h $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:57:39 $
 * $Author: kevinb $
 *
 * Texture dialog keypad
 *
 * $Log: not supported by cvs2svn $
 *
 * 13    4/08/99 5:47p Jason
 * added face finding function
 *
 * 12    12/04/98 1:14p Jason
 * added "Currently Used" texture flag
 *
 * 11    8/20/98 4:09p Jason
 * added facemap feature
 *
 * 10    8/05/98 8:44p Jason
 * added uniform scaling of textures
 *
 * 9     6/15/98 4:00p Jason
 * replaced monochromatic polymodel lighting with rgb lighting
 *
 * 8     3/03/98 5:39p Jason
 * added swatch to texture tab
 *
 * 7     11/14/97 12:41p Jason
 * Added ReplaceTexture function
 *
 * 6     9/15/97 1:18p Jason
 * got selectable tmap2s working
 *
 * 5     9/14/97 9:48p Matt
 * Made CEditorView a friend class
 *
 * 4     8/29/97 1:52p Samir
 * Added bitmap buttons for Texture Keypad.
 *
 * 3     8/25/97 7:39p Samir
 * Using new GrListBox from old editorPictListBox
 *
 * 10    6/03/97 4:55p Mark
 *
 * 9     5/27/97 3:11p Samir
 * Allow 'masking' of textures to be displayed in listbox.
 *
 * 8     4/11/97 4:53p Samir
 * Grab texture button works for tmap 1s
 *
 * 7     2/26/97 4:00p Samir
 * Added most keypad functions.
 *
 * 6     2/21/97 5:09p Samir
 * Uses new PictListBox class for texture listbox here.
 *
 * 5     2/19/97 7:30p Samir
 * Got scrolling texture view to work.
 *
 * 4     1/27/97 11:38a Samir
 * Added horizontal scrolling of keypad modeless dialog.
 *
 * $NoKeywords: $
 */

#ifndef M_TEXTUREDIALOG_H
#define M_TEXTUREDIALOG_H

#include "KeypadDialog.h"
#include "GrListBox.h"

class CGrTextureListBox : public CGrListBox {
protected:
  //	enumerates items maximum in the list box.  this is here to insure that any changes in
  //	the user-managed list.
  virtual int ListEnumerate();

  // Specify the first item to be drawn in the box by DrawItem. returns the item #
  //	return -1 if there is no first item (list is empty, maybe)
  virtual int ListFirstItem(int firsttex);

  // Specify the next/prev item to be drawn in the box by DrawItem.
  //	Returns the item # following or preceding the passed the item #
  virtual int ListNextItem(int curtex);
  virtual int ListPrevItem(int curtex);

  //	draw the item
  virtual void DrawItem(int x, int y, grHardwareSurface *itemsurf, int item);

  //	if item was selected from the list box, this function is invoked.
  virtual void OnItemSelected(int tex);
};

/////////////////////////////////////////////////////////////////////////////
// CTextureDialog dialog

class CTextureDialog : public CKeypadDialog {
  // Construction
public:
  CTextureDialog(CWnd *pParent = NULL); // standard constructor

  //	selects a texture in the texture dialog
  void SelectTexture(int texnum);
  void DrawSwatch(int handle, float r, float g, float b);

  // Dialog Data
  //{{AFX_DATA(CTextureDialog)
  enum { IDD = IDD_TEXTUREKEYPAD };
  // NOTE: the ClassWizard will add data members here
  //}}AFX_DATA

  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CTextureDialog)
protected:
  virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
                                                   //}}AFX_VIRTUAL

public:
  virtual void RunKeypadFunction(int code);

  // Implementation
protected:
  // Generated message map functions
  //{{AFX_MSG(CTextureDialog)
  afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar);
  afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar);
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnPaint();
  virtual BOOL OnInitDialog();
  afx_msg void OnTexpadFlipX();
  afx_msg void OnTexpadFlipY();
  afx_msg void OnTexpadSlideLeft();
  afx_msg void OnTexpadSlideRight();
  afx_msg void OnTexpadRotLeft();
  afx_msg void OnTexpadRotRight();
  afx_msg void OnTexpadRotate90();
  afx_msg void OnTexpadStretchLess();
  afx_msg void OnTexpadStretchMore();
  afx_msg void OnTexpadSlideUp();
  afx_msg void OnTexpadSlideDown();
  afx_msg void OnTexpadSetDefault();
  afx_msg void OnTexpadNextEdge();
  afx_msg void OnTexpadGrab();
  afx_msg void OnShowMineTextures2();
  afx_msg void OnShowObjTextures();
  afx_msg void OnShowLightTextures();
  afx_msg void OnShowTerrainTextures();
  afx_msg BOOL OnHelpInfo(HELPINFO *pHelpInfo);
  afx_msg void OnResetAllTextures();
  afx_msg void OnDestroy();
  afx_msg void OnShowTmap2Textures();
  afx_msg void OnReplaceTexture();
  afx_msg void OnKillfocusTexscaleEdit();
  afx_msg void OnTexpadExpandU();
  afx_msg void OnTexpadExpandV();
  afx_msg void OnTexpadContractU();
  afx_msg void OnTexpadContractV();
  afx_msg void OnFaceMap();
  afx_msg void OnCurrentUsed();
  afx_msg void OnFindTexture();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()

private:
  CGrTextureListBox m_TexLB;

  CBitmap m_bmUp;
  CBitmap m_bmDown;
  CBitmap m_bmLeft;
  CBitmap m_bmRight;
  CBitmap m_bmRotLeft;
  CBitmap m_bmRotRight;
  CBitmap m_bmStretchLess;
  CBitmap m_bmStretchMore;

  friend class CEditorView;
};

#endif
