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

#include "pstypes.h"
#include "pserror.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "game.h"
#include "gamefile.h"

gamefile Gamefiles[MAX_GAMEFILES];
int Num_gamefiles = 0;

// Sets all gamefiles to unused
void InitGamefiles() {
  for (int i = 0; i < MAX_GAMEFILES; i++) {
    Gamefiles[i].used = 0;
    Gamefiles[i].name[0] = 0;
  }
  Num_gamefiles = 0;
}

// Allocs a gamefile for use, returns -1 if error, else index on success
int AllocGamefile() {
  for (int i = 0; i < MAX_GAMEFILES; i++) {
    if (Gamefiles[i].used == 0) {
      memset(&Gamefiles[i], 0, sizeof(gamefile));
      Gamefiles[i].used = 1;
      Num_gamefiles++;
      return i;
    }
  }

  Int3(); // No gamefiles free!
  return -1;
}

// Frees gamefile index n and all associated images
void FreeGamefile(int n) {
  ASSERT(Gamefiles[n].used > 0);

  Gamefiles[n].used = 0;
  Gamefiles[n].name[0] = 0;
  Num_gamefiles--;
}

// Gets next gamefile from n that has actually been alloced
int GetNextGamefile(int n) {
  int i;

  ASSERT(n >= 0 && n < MAX_GAMEFILES);

  if (Num_gamefiles == 0)
    return -1;

  for (i = n + 1; i < MAX_GAMEFILES; i++)
    if (Gamefiles[i].used)
      return i;
  for (i = 0; i < n; i++)
    if (Gamefiles[i].used)
      return i;

  // this is the only one

  return n;
}

// Gets previous gamefile from n that has actually been alloced
int GetPrevGamefile(int n) {
  int i;

  ASSERT(n >= 0 && n < MAX_GAMEFILES);

  if (Num_gamefiles == 0)
    return -1;

  for (i = n - 1; i >= 0; i--) {
    if (Gamefiles[i].used)
      return i;
  }
  for (i = MAX_GAMEFILES - 1; i > n; i--) {
    if (Gamefiles[i].used)
      return i;
  }

  // this is the only one
  return n;
}
// Searches thru all gamefiles for a specific name, returns -1 if not found
// or index of gamefile with name
int FindGamefileName(char *name) {
  int i;

  ASSERT(name != NULL);

  for (i = 0; i < MAX_GAMEFILES; i++)
    if (Gamefiles[i].used && !stricmp(name, Gamefiles[i].name))
      return i;

  return -1;
}
