/*
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

#define MAX_GAME_PATHS		300
#define MAX_NODES_PER_PATH	100

typedef struct
{
	vector pos;				// where this node is in the world
	int roomnum;				// what room?
	int flags;				// if this point lives over the terrain, etc
	vector fvec;
	vector uvec;
} node;

class game_path
{
	public:
	game_path(){num_nodes = 0; pathnodes = NULL; used = false;}
	~game_path(){if(used) mem_free(pathnodes); used = false;}

	node *pathnodes;		
	int num_nodes;					// how many nodes in this path?
	char name[PAGENAME_LEN];	// the name of this path
	ubyte flags;					// special properties of this path
	bool used;						// if this path is in use
};

extern game_path GamePaths[MAX_GAME_PATHS];
extern int Num_game_paths;

void InitGamePaths();

// searches through GamePath index and returns index of path matching name
// returns -1 if not found
int FindGamePathName (char *name);

#endif
