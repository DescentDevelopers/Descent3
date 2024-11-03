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

 * $Logfile: /DescentIII/Main/ui/UIObject.cpp $
 * $Revision: 3 $
 * $Date: 4/14/99 1:53a $
 * $Author: Jeff $
 *
 *	Generic object code
 *
 * $Log: /DescentIII/Main/ui/UIObject.cpp $
 *
 * 3     4/14/99 1:53a Jeff
 * fixed case mismatched #includes
 *
 * 2     5/08/98 3:58p Samir
 * added ability to move a UI object.
 *
 * 1     12/30/97 4:36p Samir
 * Initial revision
 *
 * $NoKeywords: $
 */

#include "UIlib.h"
#include "log.h"
#include "pserror.h"

UIObject::UIObject() {}

UIObject::~UIObject() {}

//	obj = NULL, then this object has no parent: it is an independent entity.
void UIObject::Create(int x, int y, int w, int h) {
  ASSERT(!m_Created);

  m_X = x;
  m_Y = y;
  m_W = w;
  m_H = h;
  m_Created = true;
}

//	moves the object.
void UIObject::Move(int x, int y, int w, int h) {
  m_X = x;
  m_Y = y;
  m_W = w;
  m_H = h;
  LOG_DEBUG.printf("new [%d,%d]", m_X, m_Y);
}

//@@
//@@// enables ui draw commands.
//@@void UIObject::StartDraw()
//@@{
//@@	m_OwnerVP->set_clipping_rect(m_X,m_Y,m_X+m_W-1,m_Y+m_H-1);
//@@	ui_StartDraw(m_OwnerVP);
//@@}
//@@
//@@
//@@// disables ui draw commands.
//@@void UIObject::EndDraw()
//@@{
//@@	ui_EndDraw();
//@@}
