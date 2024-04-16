#ifndef MEGACELL_H
#define MEGACELL_H

#include "pstypes.h"
#include "manage.h"
#include "gametexture.h"

#define MAX_MEGACELLS	100

#define DEFAULT_MEGACELL_WIDTH	8
#define DEFAULT_MEGACELL_HEIGHT	8

#define MAX_MEGACELL_WIDTH		8
#define MAX_MEGACELL_HEIGHT		8

typedef struct
{
	char			name[PAGENAME_LEN];
	sbyte			width;
	sbyte			height;

	short			texture_handles[MAX_MEGACELL_WIDTH*MAX_MEGACELL_HEIGHT];
	int				flags;
	ubyte			used;
} megacell;

extern int Num_megacells;
extern megacell Megacells[MAX_MEGACELLS];

// Sets all MEGACELLs to unused
void InitMegacells ();

// Allocs a MEGACELL for use, returns -1 if error, else index on success
int AllocMegacell ();

// Frees MEGACELL index n
void FreeMegacell (int n);

// Gets next MEGACELL from n that has actually been alloced
int GetNextMegacell (int n);

// Gets previous MEGACELL from n that has actually been alloced
int GetPrevMegacell (int n);

// Searches thru all MEGACELLs for a specific name, returns -1 if not found
// or index of MEGACELL with name
int FindMegacellName (char *name);



#endif


