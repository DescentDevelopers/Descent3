#ifndef GAMEFILE_H
#define GAMEFILE_H

#include "pstypes.h"
#include "manage.h"

#ifdef MACINTOSH
	#define MAX_GAMEFILES		1300
#else
	#define MAX_GAMEFILES		1500
#endif

typedef struct
{
	char name[PAGENAME_LEN];
	char dir_name[PAGENAME_LEN];
	ubyte used;
} gamefile;


extern int Num_gamefiles;
extern gamefile Gamefiles[MAX_GAMEFILES];

// Sets all gamefiles to unused
void InitGamefiles ();

// Allocs a gamefile for use, returns -1 if error, else index on success
int AllocGamefile ();

// Frees gamefile index n
void FreeGamefile (int n);

// Gets next gamefile from n that has actually been alloced
int GetNextGamefile (int n);

// Gets previous gamefile from n that has actually been alloced
int GetPrevGamefile (int n);

// Searches thru all gamefile for a specific name, returns -1 if not found
// or index of gamefile with name
int FindGamefileName (char *name);


#endif


