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

#ifndef MULTI_CLIENT_H

// Do client stuff for this frame
void MultiDoClientFrame();

// Set the local machine as a client
void MultiStartClient(char *scriptname);

#define MAX_SAVED_MOVES 100

struct saved_move {
  float timestamp;
  vector thrust, rotthrust;

};

extern saved_move SavedMoves[];
extern int Current_saved_move;

// Alloc and free saved move
extern int MultiAllocSavedMove();
extern void MultiFreeSavedMove(int);

#endif
