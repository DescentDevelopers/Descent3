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

#ifndef MARKER_H
#define MARKER_H

#include "player.h"

#define MAX_MARKER_MESSAGE_LENGTH 40

// Drops a marker at the players current position
void DropMarker(char *message);

// Resets markers before a level stars
void ResetMarkers();

// Inits markers at game load
void InitMarkers();

extern int Marker_polynum;
extern int Marker_message;

extern char MarkerMessages[MAX_PLAYERS * 2][MAX_MARKER_MESSAGE_LENGTH];

#endif