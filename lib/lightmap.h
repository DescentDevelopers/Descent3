#ifndef LIGHTMAP_H
#define LIGHTMAP_H

#include "pstypes.h"

#ifdef MACINTOSH
#define MAX_LIGHTMAPS (60000)
#else
#define MAX_LIGHTMAPS (65534)
#endif
#define BAD_LM_INDEX 65535

// lightmap flags
#define LF_CHANGED 1   // this bitmap has changed since last frame (useful for hardware cacheing)
#define LF_LIMITS 2    // This lightmap has a specific area that has changed since last frame
#define LF_WRAP 4      // This lightmap should be drawn with wrapping (not clamping)
#define LF_BRAND_NEW 8 // This lightmap is brand new and hasn't been to the video card yet

typedef struct {
  ubyte width, height; // Width and height in pixels
  ushort *data;        // 16bit data

  ushort used;
  ubyte flags;
  short cache_slot;         // for the renderers use
  ubyte square_res;         // for renderers use
  ubyte cx1, cy1, cx2, cy2; // Change x and y coords
} bms_lightmap;

extern bms_lightmap GameLightmaps[MAX_LIGHTMAPS];

// Sets all the lightmaps to unused
void lm_InitLightmaps();

void lm_ShutdownLightmaps(void);

// Allocs a lightmap of w x h size
// Returns lightmap handle if successful, -1 if otherwise
int lm_AllocLightmap(int w, int h);

// Given a handle, frees the lightmap memory and flags this lightmap as unused
void lm_FreeLightmap(int handle);

// returns a lightmaps width  else -1 if something is wrong
int lm_w(int handle);

// returns a lightmaps height , else -1 if something is wrong
int lm_h(int handle);

// returns a lightmaps data else NULL if something is wrong
ushort *lm_data(int handle);

#endif
