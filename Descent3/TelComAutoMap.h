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

 * $Logfile: /DescentIII/main/TelComAutoMap.h $
 * $Revision: 5 $
 * $Date: 4/14/99 3:57a $
 * $Author: Jeff $
 *
 * TelCom Automap
 *
 * $Log: /DescentIII/main/TelComAutoMap.h $
 *
 * 5     4/14/99 3:57a Jeff
 * fixed case mismatch in #includes
 *
 * 4     12/14/98 11:57a Jason
 * changes for visibile automap
 *
 * 3     8/27/98 2:51p Jeff
 * New TelCom finally checked in
 *
 * 2     5/18/98 2:23p Jeff
 * Automap displays stand-in art right now
 *
 * $NoKeywords: $
 */

#ifndef __TCAUTOMAP_H_
#define __TCAUTOMAP_H_

#include "TelCom.h"

// This is the function called by TelCom
//  return true if TelCom should exit to TelCom Main Menu
//  return false if you should exit out of TelCom completly
//  mmonitor = main monitor class
//  lmonitor = local monitor class (the monitor at the top of the screen)
bool TelComAutoMap(tTelComInfo *tcs);

void AutomapClearVisMap();

extern uint8_t AutomapVisMap[];

#endif
