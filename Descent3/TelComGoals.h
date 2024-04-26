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
 * $Logfile: /DescentIII/main/TelComGoals.h $
 * $Revision: 2 $
 * $Date: 4/14/99 3:57a $
 * $Author: Jeff $
 *
 * TelCom Goal Status screen
 *
 * $Log: /DescentIII/main/TelComGoals.h $
 *
 * 2     4/14/99 3:57a Jeff
 * fixed case mismatch in #includes
 *
 * 1     2/20/99 4:31p Jeff
 *
 * $NoKeywords: $
 */

#ifndef __TCGOALS_H_
#define __TCGOALS_H_
#include "TelCom.h"

// This is the function called by TelCom
//  return true if TelCom should exit to TelCom Main Menu
//  return false if you should exit out of TelCom completly
bool TelComGoalStatus(tTelComInfo *tcs);

#endif
