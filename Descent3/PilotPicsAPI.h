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

 * $Logfile: /DescentIII/Main/PilotPicsAPI.h $
 * $Revision: 2 $
 * $Date: 11/23/98 6:30p $
 * $Author: Jeff $
 *
 * API header file for Pilot Pictures
 *
 * $Log: /DescentIII/Main/PilotPicsAPI.h $
 *
 * 2     11/23/98 6:30p Jeff
 * initial creation
 *
 * $NoKeywords: $
 */

#ifndef _PILOTPICS_H__
#define _PILOTPICS_H__

// ---------------------------------------------------------
// PPic_InitDatabase
//	Purpose:
//		Initializes the database for the pictures.  Call before
//	any other Pilot Picture function
// ---------------------------------------------------------
bool PPic_InitDatabase(void);

// ---------------------------------------------------------
// PPic_CloseDatabase
//	Purpose:
//		Closes down the picture database.
// ---------------------------------------------------------
void PPic_CloseDatabase(void);

// ---------------------------------------------------------
// PPic_QueryPilot
//	Purpose:
//		Given a pilot callsign it will search the database
//	and return the number of pilots that match the name
// ---------------------------------------------------------
ushort PPic_QueryPilot(char *pilot_name);

// ---------------------------------------------------------
// PPic_FindFirst
//	Purpose:
//		Given a pilot callsign it will return the first pilot
//	that matches the search.  Returns true on success, false
//	if it couldn't find any pilots matching.  It returns
//	the pilot id.
// ----------------------------------------------------------
bool PPic_FindFirst(char *pilot_name, ushort *pilot_id);

// ----------------------------------------------------------
// PPic_FindNext
//	Purpose:
//		Call repeatedly until false is returned to get information
//	about all the pilots that match pilot_name passed into
//	an initial call to PPic_FindFirst().  It returns the
//	pilot id.
// ----------------------------------------------------------
bool PPic_FindNext(ushort *pilot_id);

// ----------------------------------------------------------
// PPic_FindClose
//	Purpose:
//		Stops a PPic_FindFirst/PPic_FindNext session
// ----------------------------------------------------------
void PPic_FindClose(void);

// ----------------------------------------------------------
// PPic_GetPilot
//	Purpose:
//		Given a pilot id, it will return the pilot name of
//	the pilot name.  Returns false if it's an invalid pilot id.
// ----------------------------------------------------------
bool PPic_GetPilot(ushort pilot_id, char *pilot_name, int buffersize);

// ----------------------------------------------------------
// PPic_GetBitmapHandle
//	Purpose:
//		Given a pilot id, it will return a handle to the
//	bitmap for the pilot.  MAKE SURE YOU FREE THE BITMAP WITH
//	bm_FreeBitmap().  Returns -1 if it was an illegal pilot id.
//	Returns BAD_BITMAP_HANDLE if it couldn't open the bitmap.
// ----------------------------------------------------------
int PPic_GetBitmapHandle(ushort pilot_id);

#endif
