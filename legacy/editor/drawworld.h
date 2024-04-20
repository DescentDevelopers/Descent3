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
 * $Logfile: /DescentIII/Main/editor/drawworld.h $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:57:37 $
 * $Author: kevinb $
 *
 * Header for drawworld.cpp
 *
 * $Log: not supported by cvs2svn $
 * 
 * 6     10/04/98 1:25p Matt
 * 
 * 5     5/20/98 3:48p Matt
 * Added suport for variable-depth face selection in wireframe view.
 * 
 * 4     9/17/97 11:34a Matt
 * Ripped out segment code
 * 
 * 3     7/29/97 12:16p Matt
 * Made wireframe seach work for rooms & faces
 * 
 * 5     3/17/97 11:10a Jason
 * added wireframe terrain view
 * 
 * 4     2/28/97 4:00p Matt
 * Added code to handle the list of selected segments
 * 
 * 3     2/19/97 5:37p Matt
 * Added code to set curseg from clicking in wireframe window
 * 
 * 2     2/07/97 5:47p Matt
 * Renamed vector.h to vecmat.h to fix DevStudio problem
 *
 * $NoKeywords: $
 */

#include "gr.h"

#include "vecmat.h"

void DrawWorld(grViewport *vp,vector *view_target,matrix *view_orient,float view_dist,int start_room,float rad);
void DrawWorldRoom(grViewport *vp,vector *view_target,matrix *view_orient,float view_dist,int start_room,int depth);

void DrawTerrainWorld(grViewport *vp,vector *view_target,matrix *view_orient,float view_dist);

//Find mode values
#define FM_CLOSEST	0	//Finds the closest face
#define FM_SPECIFIC	1	//Finds the passed face
#define FM_NEXT		2	//Find the next-farthest face from the last-found face

//Finds the closest room:face to the viewer at a given x,y screen position in the wireframe view
//Parameters:	vp - the viewport we're checking.  Must be the same viewport as the last wireframe view rendered
//					x,y - the screen coordinates clicked on
//					roomnum,facenum - pointers to variables to be filled in
//					ingore_previous - if this is true, ignore the previous face & find the next farther face
//Returns:		true if found a room/face
bool WireframeFindRoomFace(grViewport *vp,int x,int y,int *roomnum,int *facenum,int find_mode);

//Adds all the rooms that have a vertex inside of a given screen box to the selected list
//Parameters:	vp - the viewport we're checking.  Must be the same viewport as the last wireframe view rendered
//					left,top,right,bot - the screen coordinates of the box
//Returns:		the number of rooms found
int SelectRoomsInBox(grViewport *vp,int left,int top,int right,int bot);


