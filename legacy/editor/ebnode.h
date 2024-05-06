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

* $Logfile: /DescentIII/Main/editor/ebnode.h $
* $Revision: 1.1.1.1 $
* $Date: 2003-08-26 03:57:37 $
* $Author: kevinb $
*
* BOA Helper Node functions
*
* $Log: not supported by cvs2svn $
 * 
 * 7     4/27/99 11:37p Chris
 * Improving the BNode system
 * 
 * 6     4/26/99 10:31p Chris
 * Improving the BNode system
 * 
 * 5     4/25/99 9:02p Chris
 * Improving the Bnode system
 * 
 * 4     4/18/99 5:39a Chris
 * Vastly improved the path node system
 * 
 * 3     4/15/99 5:49p Chris
 * Fixed a bug with rendering the BOAPath nodes
 * 
 * 2     4/14/99 3:13p Chris
 * Beginning to add BoaNode stuff
*
* $NoKeywords: $
*/

#ifndef EBNODE_H_
#define EBNODE_H_

#include "bnode.h"
#include "room.h"
#include "mem.h"
#include "vecmat.h"
#include "3d.h"
#include "gr.h"

#define EBDRAW_NONE                0
#define EBDRAW_ROOM                1
#define EBDRAW_ROOM_AND_NEXT_ROOMS 2
#define EBDRAW_LEVEL					  3

extern char EBN_draw_type;

extern void EBNode_MakeDefaultIntraRoomNodes(int roomnum);
extern void EBNode_MakeDefaultInterRoomEdges(int roomnum);
extern void EBNode_MakeFirstPass(void);
extern void EBNode_Draw(char draw_type, grViewport *vp,vector *viewer_eye,matrix *viewer_orient,float zoom);
extern void EBNode_Move(bool f_offset, int roomnum, int pnt, vector *pos);
extern int EBNode_AddNode(int roomnum, vector *pnt, bool f_from_editor, bool f_check_for_close_nodes);
// Note the 2 last parameters are for internal use ONLY never set them
extern void EBNode_AddEdge(int spnt, int sroom, int epnt, int eroom, bool f_make_reverse = true, float computed_max_rad = -1.0f);
extern void EBNode_RemoveEdge(int spnt, int sroom, int epnt, int eroom, bool f_remove_reverse = true);
int EBNode_InsertNodeOnEdge(int spnt, int sroom, int epnt, int eroom);
void EBNode_RemoveNode(int roomnum, int pnt);
void EBNode_ClearLevel();
bool EBNode_VerifyGraph();
void EBNode_AutoEdgeNode(int spnt, int sroom);

#endif
