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

 * $Logfile: /DescentIII/Main/aiterrain.h $
 * $Revision: 5 $
 * $Date: 8/05/97 12:17p $
 * $Author: Chris $
 *
 * Terrain specific AI stuff
 *
 * $Log: /DescentIII/Main/aiterrain.h $
 *
 * 5     8/05/97 12:17p Chris
 *
 * 4     7/30/97 1:31p Chris
 * Made helicopters slightly more interesting.
 *
 * 3     7/29/97 12:20p Chris
 * Incremental improvements.  Fixed a memory bug.
 *
 * 2     7/28/97 1:19p Chris
 * Expanding the AI system
 *
 * $NoKeywords: $
 */

#ifndef AITERRAIN_H_
#define AITERRAIN_H_

struct ground_information {
  float highest_y;
  float lowest_y;
};

bool ait_GetGroundInfo(ground_information *ground_info, vector *p0, vector *p1, float rad = 0.0f, angle fov = 0);
void ait_Init(void);

#endif
