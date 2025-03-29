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

 * $Logfile: /DescentIII/Main/editor/ObjMoveManager.cpp $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:57:38 $
 * $Author: kevinb $
 *
 *	Object Move Manager
 *
 * $Log: not supported by cvs2svn $
 *
 * 14    5/11/99 12:07a 3dsmax
 * may have fixed problem with wireframe/object movement conflict.
 *
 * 13    6/17/98 12:30p Samir
 * don't draw wireframe when moving an object.
 *
 * 12    4/16/98 8:22p Sean
 * don't allow movement of doors
 *
 * 11    2/06/98 3:10p Samir
 * modified rotate scale a bit.
 *
 * 10    2/06/98 3:08p Samir
 * Simplified object movement code.
 *
 * 9     2/04/98 12:42a Matt
 * Made objects turn a lot faster with the mouse
 *
 * 8     9/05/97 3:08p Dan
 * fixed some more rotatation
 *
 * 7     9/05/97 2:51p Samir
 * Yet another adjustment on banking objects.
 *
 * 6     9/05/97 2:45p Samir
 * Increased resolution of PHB rotations.
 *
 * 5     9/03/97 8:00p Samir
 * Implemented most of the fixes for the object moving system.
 *
 * 4     9/02/97 5:36p Matt
 * Got rid of warnings
 *
 * 3     9/02/97 3:11p Samir
 * Fixed small bug.
 *
 * 2     9/02/97 3:00p Samir
 * more intuitive object moving for certain axis.
 *
 * 1     8/29/97 1:45p Samir
 * Basic object movement working.
 *
 * $NoKeywords: $
 */

#include "stdafx.h"
#include "ObjMoveManager.h"
#include "TextureGrWnd.h"
#include "editor.h"
#include "d3edit.h"
#include "HObject.h"

#include "ddio.h"

ObjectMoveManager ObjMoveManager;

//////////////////////////////////////////////////////////////////////////////
//	Object Moving Manager

ObjectMoveManager::ObjectMoveManager() {
  m_DragState = 0;
  m_MoveAxis = OBJMOVEAXIS_X;
}

//	This should be called once we want to start moving an object
//	(usually from a mouse down event)
#define CURSOR_BIND_RECT_W2 20

void ObjectMoveManager::Start(CWnd *wnd, int view_width, int view_height, vector *view_pos, matrix *view_mat, int x,
                              int y) {
  if (Objects[Cur_object_index].type == OBJ_DOOR)
    return; // Can't move doors

  m_DragState = 1;

  //	set up window properties.
  m_DragWnd = wnd;
  m_WindowW2 = view_width / 2;
  m_WindowH2 = view_height / 2;
  m_DragWnd->GetWindowRect(&m_DragRect);
  m_ObjNum = Cur_object_index;

  if (m_ObjNum == -1)
    m_DragState = 0; // stop now!

  m_ViewPos = *view_pos;
  m_ViewMat = *view_mat;

  //@@	if (first_poll) {
  //@@	//bind cursor to dragging rectangle
  //@@		POINT pt = {0,0};
  //@@		RECT bind_rect;
  //@@		vector v;
  //@@
  //@@		GetXYAndPointOfObject(&pt, &v);
  //@@		wnd->ClientToScreen(&pt);
  //@@
  //@@		SetRect(&bind_rect, pt.x - CURSOR_BIND_RECT_W2, pt.y - CURSOR_BIND_RECT_W2, pt.x + CURSOR_BIND_RECT_W2,
  //@@					pt.y + CURSOR_BIND_RECT_W2);
  //@@
  //@@		ClipCursor(&bind_rect);
  //@@
  //@@		old_pt = pt;
  //@@		first_poll = false;
  //@@	}
  //@@
  //@@	hOldCursor = GetCursor();
  //@@	SetCursor(LoadCursor(NULL, IDC_CROSS));
}

//	ends current object movement
void ObjectMoveManager::End() {
  //@@	SetCursor(hOldCursor);
  //@@	ClipCursor(NULL);
  m_DragState = 0;
  Object_moved = 0;
  World_changed = 1;
}

#define ROTATE_SCALE (256.0f * 20.0f / ((obj->size < 10.0f) ? 10.0f : obj->size))

//	defers control to the move manager
void ObjectMoveManager::Defer() {
  //	this code will call the object movement routines based off of the current mouse state and
  //	the current movement axis.
  static POINT cur_pt;

  if (m_DragState == 1) {
    int x, y, dsx, dsy;
    float dx, dy;

    Object_moved = 0;

    if (ddio_MouseGetState(&x, &y, &dsx, &dsy) & MOUSE_LB) {
      object *obj = &Objects[m_ObjNum];

      if (!dsx && !dsy)
        return;

      GetObjectDeltas(&dx, &dy, obj, dsx, dsy);

      switch (m_MoveAxis) {
      case OBJMOVEAXIS_X:
        HObjectMove(Cur_object_index, dx, 0, 0);
        break;

      case OBJMOVEAXIS_Y:
        HObjectMove(Cur_object_index, 0, -dy, 0);
        break;

      case OBJMOVEAXIS_Z:
        HObjectMove(Cur_object_index, 0, 0, dy);
        break;

      case OBJMOVEAXIS_XY:
        HObjectMove(Cur_object_index, dx, -dy, 0);
        break;

      case OBJMOVEAXIS_P:
        Object_move_rotation = fabs(-dy) * ROTATE_SCALE;
        if (dy > 0)
          HObjectIncreasePitch();
        else if (dy < 0)
          HObjectDecreasePitch();
        break;

      case OBJMOVEAXIS_H:
        Object_move_rotation = fabs(dx) * ROTATE_SCALE;
        if (dx > 0)
          HObjectIncreaseHeading();
        else if (dx < 0)
          HObjectDecreaseheading();
        break;

      case OBJMOVEAXIS_B:
        Object_move_rotation = fabs(dx) * ROTATE_SCALE;
        if (dx > 0)
          HObjectIncreaseBank();
        else if (dx < 0)
          HObjectDecreaseBank();
        break;

      case OBJMOVEAXIS_PH:
        Object_move_rotation = fabs(-dy) * ROTATE_SCALE;
        if (dy > 0)
          HObjectIncreasePitch();
        else if (dy < 0)
          HObjectDecreasePitch();
        Object_move_rotation = fabs(dx) * ROTATE_SCALE;
        if (dx > 0)
          HObjectIncreaseHeading();
        else if (dx < 0)
          HObjectDecreaseheading();
        break;
      }
    } else {
      //	we are done otherwise, reset drag state to off
      this->End();
    }
  } else {
    Object_moved = 0;
  }
}

//	dx and dy are the return values projected from dsx and dsy, given the object's position
void ObjectMoveManager::GetObjectDeltas(float *dx, float *dy, object *obj, int dsx, int dsy) {
  vector pos;

  //	rotate object to view position
  pos = obj->pos - m_ViewPos;
  pos = pos * m_ViewMat;

  if (pos.z() == 0.0)
    return;

  *dx = ((scalar)dsx * pos.z()) / m_WindowW2;
  *dy = ((scalar)dsy * pos.z()) / m_WindowH2;
}
