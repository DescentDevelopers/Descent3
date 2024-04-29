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
 * $Logfile: /DescentIII/Main/debuggraph.h $
 * $Revision: 2 $
 * $Date: 4/04/99 8:15p $
 * $Author: Jeff $
 *
 * Debug graph visual log
 *
 * $Log: /DescentIII/Main/debuggraph.h $
 *
 * 2     4/04/99 8:15p Jeff
 * added debug graph stuff
 *
 * $NoKeywords: $
 */

#ifndef __DEBUGGRAPH_H_
#define __DEBUGGRAPH_H_
#include "grdefs.h"

#define DGF_MULTIPLAYER 0x01  // used in multiplayer
#define DGF_SINGLEPLAYER 0x02 // used in single player

int DebugGraph_Add(float min, float max, const char *name, int flags = DGF_MULTIPLAYER | DGF_SINGLEPLAYER);
int DebugGraph_Add(int min, int max, const char *name, int flags = DGF_MULTIPLAYER | DGF_SINGLEPLAYER);

void DebugGraph_Update(int id, int value);
void DebugGraph_Update(int id, float value);
void DebugGraph_Render(void);

void DebugGraph_Disable(int id);
void DebugGraph_Enable(int id);

void DebugGraph_DisplayOptions(void);

void DebugGraph_Initialize(void);

#endif
