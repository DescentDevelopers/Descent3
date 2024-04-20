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

/*
 * $Logfile: /DescentIII/Main/editor/WireframeGrWnd.h $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:57:39 $
 * $Author: kevinb $
 *
 * Wireframe view of mine
 *
 * $Log: not supported by cvs2svn $
 * 
 * 2     2/05/98 3:18p Matt
 * Renamed GetSegmentFromPoint() to GetCellFromPoint()
 * 
 * 13    5/22/97 5:16p Jason
 * added "drawing" on the topo map
 * 
 * 12    5/15/97 3:27a Jason
 * implemented terrain magnification
 * 
 * 11    4/11/97 4:20p Jason
 * now draws the player on the wireframe terrain window
 * 
 * 10    3/20/97 3:02p Samir
 * Implemented Selection Manager in Wireframe window.
 * 
 * 9     2/28/97 2:21p Samir
 * You can drag a box within the wireframe window.
 * 
 * 8     2/19/97 5:37p Matt
 * Added code to set curseg from clicking in wireframe window
 * 
 * 7     1/30/97 6:48p Samir
 * Added some NC_xxx message handlers
 * 
 * 6     1/23/97 4:56p Samir
 * Handles saving screen positions of windows and tileability.
 * 
 * 5     1/21/97 12:44p Samir
 * Floatable-resizable views
 *
 * $NoKeywords: $
 */


#ifndef M_WIREFRAMEGRWND_H
#define M_WIREFRAMEGRWND_H

#include "GrWnd.h"
#include "3d.h"

/////////////////////////////////////////////////////////////////////////////
// CWireframeGrWnd window

class CWireframeGrWnd : public CGrWnd
{
// Construction
public:
	CWireframeGrWnd();

// Attributes
public:

// Operations
public:
	BOOL Create(RECT &rect, BOOL movable, CWnd *pParent);
	void Render();

	void GetHighlightedRect(int *left, int *top, int *right, int *bottom);
	void DrawPlayerOnTerrain();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWireframeGrWnd)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CWireframeGrWnd();
	void GetMagCoords (int *,int*,int *,int*);
	void SetMagCoords (int,int,int,int);
	int m_x1,m_x2,m_y1,m_y2;	// min,max of topographical map magnification
	void DrawTerrainWorld(grViewport *vp,vector *view_target,matrix *view_orient,float view_dist);
	int GetCellFromPoint(int x,int y);
	void DrawTerrainCell(int seg);

	// Generated message map functions
protected:
	//{{AFX_MSG(CWireframeGrWnd)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg BOOL OnNcActivate(BOOL bActive);
	afx_msg void OnNcPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	BOOL m_Movable;						// is this a floating window?
	RECT m_HighlightedRect;

	void draw_tile_caption(CDC *dc);	// special caption for tiled windows.
};

/////////////////////////////////////////////////////////////////////////////
#endif
