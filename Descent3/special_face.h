#ifndef SPECIAL_FACE_H
#define SPECIAL_FACE_H

#include "pstypes.h"
#include "pserror.h"
#include "vecmat.h"

#define BAD_SPECIAL_FACE_INDEX	-1
#ifdef MACINTOSH
#define MAX_SPECIAL_FACES	5000
#else
#define MAX_SPECIAL_FACES	13000		//made large enough for Josh's Mercenary level 3
#endif

// What this special face is used for:
#define SFT_SPECULAR				1

typedef struct
{
	vector bright_center;
	ushort bright_color;
} specular_instance;


#define SFF_SPEC_OBJECT	1	
#define SFF_SPEC_SMOOTH	2

typedef struct
{
	ubyte type;		// See types (above)
	ubyte num;		// Number of instances
	ubyte used;
	ubyte flags;
	
	specular_instance *spec_instance;
	
	vector *vertnorms;

} special_face;

extern special_face SpecialFaces[];
extern int Num_of_special_faces;

// Sets all the special faces to unused
void InitSpecialFaces();

// Returns an index into the special faces array
int AllocSpecialFace (int type,int num,bool vertnorms=false,int num_vertnorms=0);

// Given a handle, frees the special face
void FreeSpecialFace (int handle);


#endif
