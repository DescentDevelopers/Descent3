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

 * $Logfile: /DescentIII/main/gamepath.h $
 * $Revision: 12 $
 * $Date: 10/08/98 4:23p $
 * $Author: Kevin $
 *
 * Header for gamepath.cpp
 *
 * $Log: /DescentIII/main/gamepath.h $
 *
 * 12    10/08/98 4:23p Kevin
 * Changed code to comply with memory library usage. Always use mem_malloc
 * , mem_free and mem_strdup
 *
 * 11    2/10/98 10:48a Matt
 * Moved editor code from gamepath.cpp to epath.cpp
 *
 */

#ifndef GAME_PATH_H
#define GAME_PATH_H

#include "stdlib.h"
#include "pstypes.h"
#include "pserror.h"
#include "vecmat.h"
#include "3d.h"
#include "manage.h"
#include "mem.h"
// chrishack -- this could be dynamically allocated at the beginning of a level
// MAX_NODES_PER_PATH is big and so is MAX_GAME_PATHS

#define MAX_GAME_PATHS 300
#define MAX_NODES_PER_PATH 100

struct node {
  vector pos;  // where this node is in the world
  int roomnum; // what room?
  int flags;   // if this point lives over the terrain, etc
  vector fvec;
  vector uvec;
};

class game_path {
public:
  game_path() {
    num_nodes = 0;
    pathnodes = NULL;
    used = false;
  }
  ~game_path() {
    if (used)
      mem_free(pathnodes);
    used = false;
  }

  node *pathnodes;
  int num_nodes;           // how many nodes in this path?
  char name[PAGENAME_LEN]; // the name of this path
  uint8_t flags;             // special properties of this path
  bool used;               // if this path is in use
};

extern game_path GamePaths[MAX_GAME_PATHS];
extern int Num_game_paths;

void InitGamePaths();

// searches through GamePath index and returns index of path matching name
// returns -1 if not found
int FindGamePathName(const char *name);

#endif
