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

 * $Logfile: /DescentIII/Main/editor/HObject.h $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:57:38 $
 * $Author: kevinb $
 *
 * Editor object handling functions
 *
 * $Log: not supported by cvs2svn $
 *
 * 7     5/18/98 2:56p Matt
 * Added code to readjust all ground object (for after the terrain has
 * moved).
 *
 * 6     2/06/98 3:08p Samir
 * Simplified object movement code.
 *
 * 5     9/17/97 11:55a Samir
 * MORE SEGMENT RIPOUT
 *
 * 4     9/17/97 11:35a Samir
 * BIG SEGMENT RIPOUT
 *
 * 3     9/03/97 8:00p Samir
 * Implemented most of the fixes for the object moving system.
 *
 * 2     8/21/97 7:48p Matt
 * Added code to move player object (like old move player to curseg)
 *
 * 9     6/27/97 7:20p Matt
 * Added function to move player 0 to current segment
 *
 * 8     4/07/97 3:14p Samir
 * Added Flip and SetDefault Object functions.
 *
 * 7     4/04/97 2:45p Matt
 * Removed prototypes for deleted functions
 *
 * 6     3/27/97 11:54a Samir
 * UpdateMineObjects returns number of objects deleted of a type.
 *
 * 5     3/27/97 11:48a Samir
 * Added update objects in mine functions.
 *
 * 4     3/25/97 6:31p Samir
 * Added robot placement.
 *
 * 3     2/19/97 3:37p Samir
 * Added delete and rotation of objects.
 *
 * 2     2/17/97 6:18p Samir
 * Add powerup, move it around WITHIN the segment.
 *
 * 1     2/17/97 3:23p Samir
 * Handler for Object functions
 *
 * $NoKeywords: $
 */

#ifndef HOBJECT_H
#define HOBJECT_H

#include "pstypes.h"
#include "fix.h"
#include "object.h"

const float HOBJECT_SCALE_UNIT = (float)0.5;
const angle HOBJECT_ROTATION_UNIT = 1024;

const int HOBJECT_MOVE_LEFT = 1, HOBJECT_MOVE_RIGHT = 2, HOBJECT_MOVE_FORWARD = 3, HOBJECT_MOVE_BACK = 4,
          HOBJECT_MOVE_UP = 5, HOBJECT_MOVE_DOWN = 6;

//	var determining length of movement.
extern float Object_move_scale;
extern angle Object_move_rotation;

//	moves the specified object in direction specified
void HObjectMove(int objnum, float dx, float dy, float dz);

//	places the object into the center of the current selected segment
//	returns 0 if fail.
bool HObjectPlace(int obj_type, int obj_id);

// Adjusts an object so it's at the ground level (for when the ground has moved)
void ResetGroundObject(object *objp);

//	deletes the currently selected object from the mine.
void HObjectDelete();

//		sets default orientation for object.
void HObjectSetDefault();

// Move the object to in front of the viewer (where a new object would be place
void HObjectMoveToViewer(object *objp);

//	Select objects in the mine or segment
int HObjectSelectNextInMine();
int HObjectSelectPrevInMine();

//	Rotation of objects
void HObjectIncreaseBank();
void HObjectDecreaseBank();
void HObjectIncreasePitch();
void HObjectDecreasePitch();
void HObjectIncreaseHeading();
void HObjectDecreaseheading();

//	Flipping of object
void HObjectFlip();

#endif
