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

 * $Logfile: /DescentIII/main/scorch.h $
 * $Revision: 3 $
 * $Date: 5/25/98 8:36p $
 * $Author: Matt $
 *
 * Header for scorch system
 *
 * $Log: /DescentIII/main/scorch.h $
 *
 * 3     5/25/98 8:36p Matt
 * Added code to set different sizes for different weapon scorch marks.
 * Also, don't leave scorch marks on lights.
 *
 * 2     5/22/98 12:34p Matt
 * Added scorch mark/bullet hole system.
 *
 * 1     5/21/98 11:32p Matt
 *
 */

#include "vecmat.h"

// Called when a new level is started to reset the data
void ResetScorches();

// Add a scorch mark
// Parameters:	roomnum,facenum - the face that the scorch is on
//					pos - where the scorch goes
//					texture_handle - the texture for the scorch mark
//					size - how big the scorch is
void AddScorch(int roomnum, int facenum, vector *pos, int texture_handle, float size);

// Draw the scorch(es) for a given face
void DrawScorches(int roomnum, int facenum);
