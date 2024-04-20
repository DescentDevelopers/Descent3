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

#ifndef _ATTACH_H_
#define _ATTACH_H_

#include "object.h"

bool AttachDoPosOrient(object *parent, char parent_ap, object *child, char child_ap, bool f_parent,
                       bool f_move_obj = true, vector *pos = NULL, matrix *orient = NULL, bool f_dropping_off = false);
bool AttachDoPosOrientRad(object *parent, char p_ap, object *child, float rad_percent, vector *pos);
void AttachUpdateSubObjects(object *obj);

bool AttachObject(object *parent, char parent_ap, object *child, char child_ap, bool f_use_aligned = false);
bool AttachObject(object *parent, char parent_ap, object *child, float percent_rad);
bool UnattachFromParent(object *child);
bool UnattachChild(object *parent, char parent_ap);
bool UnattachChildren(object *parent);

#endif