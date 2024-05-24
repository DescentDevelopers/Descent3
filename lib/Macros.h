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

 * $Logfile: /DescentIII/Main/Lib/Macros.h $
 * $Revision: 11 $
 * $Date: 10/21/99 9:28p $
 * $Author: Jeff $
 *
 *	Macros.
 *
 * $Log: /DescentIII/Main/Lib/Macros.h $
 *
 * 11    10/21/99 9:28p Jeff
 * B.A. Macintosh code merge
 *
 * 10    7/28/99 5:18p Kevin
 * Mac merge fixes
 *
 * 9     7/28/99 3:17p Kevin
 * Mac Changes
 *
 * 8     12/16/98 1:57p Samir
 * Replaced CleanupString2 with CleanupStr
 *
 * 7     12/02/98 11:44a Samir
 * added CHECK_FLAG macro
 *
 * 6     8/10/98 5:53p Samir
 * macro to convert KB to bytes.
 *
 * 5     7/14/98 11:52a Samir
 * added cleanup string 'macro'.
 *
 * 4     5/15/98 3:13p Samir
 * added some uint16_t/byte macros.
 *
 * 3     3/31/98 7:48p Samir
 * fixed horrible makeword macro bug.
 *
 * 2     12/03/97 7:37p Samir
 * Added makeword, hiword and loword macros.
 *
 * $NoKeywords: $
 */
#ifndef _MACROS_H
#define _MACROS_H
#define CHECK_FLAG(_var, _flag) ((_var) & (_flag))
#endif
