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
 * $Logfile: /DescentIII/Main/editor/ObjMoveManager.h $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:57:38 $
 * $Author: kevinb $
 *
 *	Object Move Manager
 *
 * $Log: not supported by cvs2svn $
 * 
 * 3     2/06/98 3:08p Samir
 * Simplified object movement code.
 * 
 * 2     9/03/97 8:00p Samir
 * Implemented most of the fixes for the object moving system.
 * 
 * 1     8/29/97 1:45p Samir
 * Basic object movement working.
 *
 * $NoKeywords: $
 */

#ifndef _OBJ_MOVE_MANAGER_H
#define _OBJ_MOVE_MANAGER_H

#include "vecmat.h"

struct object;

const int OBJMOVEAXIS_X = 1,
			OBJMOVEAXIS_Y = 2,
			OBJMOVEAXIS_Z = 3,
			OBJMOVEAXIS_XY = 4,
			OBJMOVEAXIS_P = 5,
			OBJMOVEAXIS_H = 6,
			OBJMOVEAXIS_B = 7,
			OBJMOVEAXIS_PH = 8;

class ObjectMoveManager
{
	int m_DragState;					// 0 if not dragging, 1 if dragging. 2 if no drag.
	int m_MoveAxis;					// defined to a value from above.
	int m_ObjNum;						// object index to be moved.
	float m_WindowW2, m_WindowH2;	// window x and y centers, from which we translate mouse coords to 3d coords

	CWnd *m_DragWnd;					// window where we can drag the mouse
	RECT m_DragRect;

	matrix m_ViewMat;
	vector m_ViewPos;

//	dx and dy are the return values projected from dsx and dsy, given the object's position
	void GetObjectDeltas(float *dx, float *dy, object *obj, int dsx, int dsy);

public:
	ObjectMoveManager();

//	starts motion
	void Start(CWnd *wnd, int view_width, int view_height, vector *view_pos, matrix *view_mat, int x, int y);

//	ends current object movement
	void End();

//	defers control to the move manager
	void Defer();

//	tell whether we are drawing a selection box (if drag state == 1)
	bool IsMoving() const { return (m_DragState == 1); };

//	sets the moving axis
	void SetMoveAxis(int axis) { m_MoveAxis = axis; };
};

extern ObjectMoveManager ObjMoveManager;

#endif

