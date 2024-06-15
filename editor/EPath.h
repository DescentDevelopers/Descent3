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

 * $Logfile: /DescentIII/Main/editor/EPath.h $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:57:38 $
 * $Author: kevinb $
 *
 * Header for epaths.cpp
 *
 * $Log: not supported by cvs2svn $
 *
 * 3     2/03/99 6:57p Chris
 * Added the "Move node to current object" function
 *
 * 2     2/10/98 10:48a Matt
 * Moved editor code from gamepath.cpp to epath.cpp
 *
 * 1     2/10/98 10:13a Matt
 *
 */

#include "vecmat.h"
#include "gamepath.h"

extern uint8_t Show_paths;

// Allocs a gamepath that a robot will follow.  Returns an index into the GamePaths
// array
int AllocGamePath(void);

// Given a path number, and a node number in that path, adds another node after the
// specified node
// Returns the index number of the new node
// If nodenum is -1, this node couldn't be added
// Flags are passed via the flags field
int InsertNodeIntoPath(int pathnum, int nodenum, int flags);

void FreeGamePath(int n);

// Given a pathnum and a node index, deletes that node and moves all the following nodes down
// by one
void DeleteNodeFromPath(int pathnum, int nodenum);

// Given a path number and a node, it moves the node by the change in position (if the new position is valid)
int MovePathNode(int pathnum, int nodenum, vector *delta_pos);

// Given a path number and a node, it moves the node to the position (if the new position is valid)
int MovePathNodeToPos(int pathnum, int nodenum, vector *pos);

// Gets next path from n that has actually been alloced
int GetNextPath(int n);
// Gets previous path from n that has actually been alloced
int GetPrevPath(int n);

// returns the index of the first path (from 0) alloced
// returns -1 if there are no paths
int GetFirstPath();

// draws a vector number
void DrawNumber(int num, vector pos, float size);

class grViewport;
// draws all the paths
void DrawAllPaths(grViewport *vp, vector *viewer_eye, matrix *viewer_orient, float zoom);
