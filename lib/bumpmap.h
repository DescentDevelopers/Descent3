#ifndef BUMPMAP_H
#define BUMPMAP_H

#include "pstypes.h"

#ifdef MACINTOSH
#define MAX_BUMPMAPS 1
#else
#define MAX_BUMPMAPS 500
#endif
#define BAD_BUMP_INDEX 65535

#define BUMPF_USED 1
#define BUMPF_CHANGED 2

typedef struct {
  ushort *data;     // 8bit data
  short cache_slot; // for the renderers use
  ubyte width, height;
  ubyte flags, pad;
} bms_bumpmap;

extern bms_bumpmap GameBumpmaps[MAX_BUMPMAPS];

// Sets all the bumpmaps to unused
void bump_InitBumpmaps();

void bump_ShutdownBumpmaps(void);

// Allocs a bumpmap of BUMP_WIDTH * BUMP_HEIGHT size
// Returns bumpmap handle if successful, -1 if otherwise
int bump_AllocBumpmap(int w, int h);

// Given a handle, frees the bumpmap memory and flags this bumpmap as unused
void bump_FreeBumpmap(int handle);

// returns a bumpmaps data else NULL if something is wrong
ushort *bump_data(int handle);

// returns width or height of the passed in bumpmap
ubyte bump_w(int handle);
ubyte bump_h(int handle);

#endif
