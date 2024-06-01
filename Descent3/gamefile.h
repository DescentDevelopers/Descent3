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

#ifndef GAMEFILE_H
#define GAMEFILE_H

#include "pstypes.h"
#include "manage.h"

#define MAX_GAMEFILES 1500

struct gamefile {
  char name[PAGENAME_LEN];
  char dir_name[PAGENAME_LEN];
  uint8_t used;
};

extern int Num_gamefiles;
extern gamefile Gamefiles[MAX_GAMEFILES];

// Sets all gamefiles to unused
void InitGamefiles();

// Allocs a gamefile for use, returns -1 if error, else index on success
int AllocGamefile();

// Frees gamefile index n
void FreeGamefile(int n);

// Gets next gamefile from n that has actually been alloced
int GetNextGamefile(int n);

// Gets previous gamefile from n that has actually been alloced
int GetPrevGamefile(int n);

// Searches thru all gamefile for a specific name, returns -1 if not found
// or index of gamefile with name
int FindGamefileName(char *name);

#endif
