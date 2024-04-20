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
 * $Logfile: /DescentIII/main/screens.h $
 * $Revision: 3 $
 * $Date: 4/07/99 12:31p $
 * $Author: Matt $
 *
 *
 *
 * $Log: /DescentIII/main/screens.h $
 *
 * 3     4/07/99 12:31p Matt
 * Added code for failed missions.
 *
 * 2     4/18/98 2:08a Samir
 * update.
 *
 * 1     4/14/98 8:19p Samir
 * initial revision.
 *
 * $NoKeywords: $
 */

#ifndef SCREENS_H
#define SCREENS_H

//	returns true if contine, returns false if player escaped out.
// Parameter:	success - true if the level was completed ok, false if failed
bool PostLevelResults(bool success);

#endif