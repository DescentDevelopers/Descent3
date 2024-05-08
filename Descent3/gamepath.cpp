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

 * $Logfile: /DescentIII/main/gamepath.cpp $
 * $Revision: 15 $
 * $Date: 10/08/98 4:23p $
 * $Author: Kevin $
 *
 *	Game automated path routines.
 *
 * $Log: /DescentIII/main/gamepath.cpp $
 *
 * 15    10/08/98 4:23p Kevin
 * Changed code to comply with memory library usage. Always use mem_malloc
 * , mem_free and mem_strdup
 *
 * 14    2/10/98 10:48a Matt
 * Moved editor code from gamepath.cpp to epath.cpp
 *
 * 13    2/05/98 4:25p Chris
 * Fixed a few bugs in the path system, improved the insert node function,
 * and made the fvec and uvec of the node work (is even loaded and saved)
 * correctly.
 *
 * 12    2/05/98 1:42p Chris
 * Fixed the problems with DrawNumber.  It use to only be able to draw
 * numbers between 0 and 9.
 *
 * 11    2/04/98 6:09p Matt
 * Changed object room number to indicate a terrain cell via a flag.  Got
 * rid of the object flag which used to indicate terrain.
 *
 * 10    2/03/98 5:01p Chris
 * UPdated the path system
 *
 * 9     2/02/98 8:16p Chris
 * FIxed some problems and added some functionality
 *
 * 8     1/27/98 12:01p Chris
 * Game path system now works with new, load, and save in the editor.
 * Also, the game path system is further bug proved.
 *
 * 7     1/26/98 6:32p Chris
 * Incremental fixes to the path system.
 *
 * 6     12/23/97 6:18p Samir
 * Moved all gr.h references to EDITOR block.
 *
 * 5     9/17/97 1:22p Samir
 * BIG SEGMENT RIPOUT
 *
 * $NoKeywords: $
 */

#include "gamepath.h"
#include <stdlib.h>
#include <string.h>
#include "pserror.h"
#include "pstypes.h"

game_path GamePaths[MAX_GAME_PATHS];
int Num_game_paths = 0;

// Frees gamepath n for future use
void FreeGamePath(int n) {
  if (!GamePaths[n].used)
    return;

  mem_free(GamePaths[n].pathnodes);
  mprintf((0, "Path %d lost some\n", n));

  GamePaths[n].num_nodes = 0;
  GamePaths[n].used = 0;
  Num_game_paths--;
}

void InitGamePaths() {
  static bool f_game_paths_init = false;
  int i;

  if (f_game_paths_init) {
    // Clear out the current path info
    for (i = 0; i < MAX_GAME_PATHS; i++) {
      FreeGamePath(i);
    }
  }

  f_game_paths_init = true;

  for (i = 0; i < MAX_GAME_PATHS; i++) {
    GamePaths[i].num_nodes = 0;
    GamePaths[i].used = 0;
  }

  Num_game_paths = 0;
}

// searches through GamePath index and returns index of path matching name
// returns -1 if not found
int FindGamePathName(const char *name) {
  int i;

  for (i = 0; i < MAX_GAME_PATHS; i++) {
    if (GamePaths[i].used && !stricmp(GamePaths[i].name, name))
      return i;
  }
  return -1;
}
