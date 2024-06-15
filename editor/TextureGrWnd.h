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

* $Logfile: /DescentIII/Main/editor/TextureGrWnd.h $
* $Revision: 1.1.1.1 $
* $Date: 2003-08-26 03:57:39 $
* $Author: kevinb $
*
* Texture View Window
*
* $Log: not supported by cvs2svn $
 *
 * 5     3/02/99 6:03p Jason
 * added opengl prototypes
 *
 * 4     12/21/98 11:30p Matt
 * Added names for objects
 *
 * 3     1/06/98 1:27p Matt
 * Cleaned up interfaces to rendering routines, deleted unused code, etc.
 *
 * 2     9/03/97 8:00p Samir
 * Implemented most of the fixes for the object moving system.
 *
 * 14    6/30/97 12:48p Jeff
 * Added right-click popup menu stufff
 *
 * 13    6/30/97 11:34a Jeff
 *
 * 12    6/05/97 6:58p Jason
 * added more megacell functionality
 *
 * 11    4/09/97 2:00p Jason
 * moved DrawAllPaths function to gamepath where it belongs
 *
 * 10    4/08/97 10:42a Jason
 * implemented path following interface
 *
 * 9     3/20/97 11:55a Jason
 * changes for terrain editing/drawing
 *
 * 8     2/19/97 11:06a Samir
 * Only flip screen if we rendered but did not hit the left button down
 * key to prevent green screen flash.
 *
 * 7     2/18/97 7:57p Matt
 * Added code to select a segment by clicking on it in the texture-mapped
 * window
 *
 * 6     1/30/97 6:44p Samir
 * Added handlers for Caption bar drawing
*
* $NoKeywords: $
*/

// TextureGrWnd.h : header file
//

#ifndef M_TEXTUREGRWND_H
#define M_TEXTUREGRWND_H

#include "GrWnd.h"
#include "vecmat.h"

/////////////////////////////////////////////////////////////////////////////
// CTextureGrWnd window

class CTextureGrWnd : public CGrWnd {
  // Construction
public:
  CTextureGrWnd();

  // Attributes
public:
  // Operations
public:
  //	note RECT is not const.  This means that the rect we get back will be changed
  //	to the actual rect encompassing the window
  BOOL Create(RECT &rect, BOOL movable, CWnd *pParent);
  void Render();
  bool SetupPopup(CMenu *popup, char *title);

  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CTextureGrWnd)
protected:
  virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
  //}}AFX_VIRTUAL

  // Implementation
public:
  virtual ~CTextureGrWnd();

private:
  int CTextureGrWnd::TGWFindRoomFace(int x, int y, int *roomnum, int *facenum);
  int CTextureGrWnd::TGWFindLightmapFace(int x, int y, int *roomnum, int *facenum, int *lumel_num);
  void CTextureGrWnd::TGWRenderMine(vector *pos, matrix *orient, float zoom, int start_roomnum);

  // Generated message map functions
protected:
  //{{AFX_MSG(CTextureGrWnd)
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnPaint();
  afx_msg void OnDestroy();
  afx_msg void OnMove(int x, int y);
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
  afx_msg BOOL OnNcActivate(BOOL bActive);
  afx_msg void OnNcPaint();
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnMouseMove(UINT nFlags, CPoint point);
  afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()

private:
  BOOL m_Movable;   // is this floating?
  BOOL m_StartFlip; // shall we flip the screen when getting to OnPaint.

  vector m_ViewPos;
  matrix m_ViewMatrix; // these are used by the object mover.

  // for FindRoomFace()
  vector m_last_viewer_eye;
  matrix m_last_viewer_orient;
  float m_last_zoom;
  int m_last_start_room;
  void TexGrStartOpenGL();
  void TexGrStopOpenGL();
};

/////////////////////////////////////////////////////////////////////////////
#endif
