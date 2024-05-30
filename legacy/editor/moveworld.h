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

 * Header for moveworld.cpp
 *
 */

#ifndef _MOVEWORLD_H
#define _MOVEWORLD_H

#include "vecmat.h"

//Read the mouse and move the wireframe view
void MoveWorld();

//Reset the orienation, position, & distance of the wireframe view
void ResetWireframeView();

//Reset the view radius of the wireframe view
void ResetWireframeViewRad();

//Set the wireframe view target to a specific location
//Does not reset the distance or orientation
void SetWireframeView(vector *pos);

//Structure to describe the wireframe view
struct wireframe_view {
	matrix	orient;
	vector	target;
	float		dist;
	float		rad;
};

//The current wireframe view
extern wireframe_view *Wireframe_view;

//Wireframe views for the mine & room views
extern wireframe_view Wireframe_view_mine,Wireframe_view_room;

#endif	//ifndef _MOVEWORLD_H
