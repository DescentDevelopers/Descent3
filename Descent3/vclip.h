#ifndef VCLIP_H

#define VCLIP_H

#include "pstypes.h"
#include "fix.h"
#include "manage.h"


#define MAX_VCLIPS		200
#define VCLIP_MAX_FRAMES 50

#define VCF_NOT_RESIDENT		1
#define VCF_WANTS_MIPPED		2

typedef struct 
{
	char name[PAGENAME_LEN];
	short	num_frames;
	short *frames;	// bitmap indices
	float frame_time;   //time (in seconds) of each frame
	int flags;
	ubyte target_size;	// what size this vclip should use (texture wise)
	ubyte used;			// Is this vclip in use?
} vclip;

extern vclip GameVClips[MAX_VCLIPS];
extern int Num_vclips;

// Simply sets all vclips to unused
void InitVClips();

// Allocs a vclip for use
// Returns -1 on error
int AllocVClip ();

// Frees a vclip
void FreeVClip (int num);

// Saves a given video clip to a file
// Returns 1 if everything ok, 0 otherwise
// "num" is index into GameVClip array
int SaveVClip (char *filename,int num);

// Allocs and loads a vclip from the file named "filename"
// Returns -1 on error, index into GameVClip array on success
// Argument texture means that this vclip is an animated texture and
// needs to have an 8bit version 
int AllocLoadVClip (char *filename,int texture_size,int mipped,int pageable=0,int format=0);

// Allocs and loads a vclip from a 3DS ILS file
// Returns -1 on error, else index into GameVClips on success
// Argument texture means that this vclip is an animated texture and
// needs to have an 8bit version 
int AllocLoadIFLVClip (char *filename,int texture_size,int mipped,int format=0);

// gets the filename from a path, plus appends our .oaf extension
void ChangeVClipName (char *src,char *dest);

// Searches thru all vclips for a specific name, returns -1 if not found
// or index of vclip with name
int FindVClipName (char *name);


// Returns frame "frame" of vclip "vclip".  Will mod the frame so that there
// is no overflow
int GetVClipBitmap (int vclip,int frame);

// Loads an animation from an IFF ANIM file
int AllocLoadIFFAnimClip (char *filename,int texture);

// Pages in a vclip if it needs to be
void PageInVClip (int vcnum);


// Frees up the bitmaps used by a vclip
void FreeVClipResidency (int vcnum);

#endif











	


