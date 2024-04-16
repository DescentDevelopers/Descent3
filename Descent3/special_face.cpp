#include "pstypes.h"

#include "special_face.h"
#include "mono.h"
#include <stdlib.h>
#include <string.h>
#include "mem.h"

int Num_of_special_faces=0;
special_face SpecialFaces[MAX_SPECIAL_FACES];

static ushort Free_special_face_list[MAX_SPECIAL_FACES];

// Sets all the special faces to unused
void InitSpecialFaces()
{
	int i;

	for (i=0;i<MAX_SPECIAL_FACES;i++)
	{
		SpecialFaces[i].used=0;
		SpecialFaces[i].vertnorms=NULL;
		SpecialFaces[i].spec_instance=NULL;
		Free_special_face_list[i]=i;
	}

}
// Returns an index into the special faces array
int AllocSpecialFace (int type,int num,bool vertnorms,int num_vertnorms)
{
	int n;

	if (Num_of_special_faces>=MAX_SPECIAL_FACES)
	{
		Int3();	// Get Jason, ran out of special faces!
		return BAD_SPECIAL_FACE_INDEX;
	}

	n = Free_special_face_list[Num_of_special_faces++];
	ASSERT (SpecialFaces[n].used==0);

	ASSERT (n>=0 && n<MAX_SPECIAL_FACES);
	
	memset (&SpecialFaces[n],0,sizeof(special_face));

	SpecialFaces[n].spec_instance=(specular_instance *)mem_malloc (num*sizeof(specular_instance));
	ASSERT (SpecialFaces[n].spec_instance);
	
	SpecialFaces[n].type=type;
	SpecialFaces[n].num=num;
	SpecialFaces[n].flags=0;
	SpecialFaces[n].used=1;

	if (vertnorms)
	{
		SpecialFaces[n].vertnorms=(vector *)mem_malloc (num_vertnorms*sizeof(vector));
		ASSERT (SpecialFaces[n].vertnorms);
		SpecialFaces[n].flags|=SFF_SPEC_SMOOTH;
	}
			
	return n;
}

// Given a handle, frees the special face
void FreeSpecialFace (int handle)
{ 
	ASSERT (handle>=0 && handle<=MAX_SPECIAL_FACES);

	if (SpecialFaces[handle].used<1)
		return;

	SpecialFaces[handle].used--;

	if (SpecialFaces[handle].used==0)
	{
		Free_special_face_list[--Num_of_special_faces] = handle;
		if (SpecialFaces[handle].spec_instance)
		{
			mem_free (SpecialFaces[handle].spec_instance);
			SpecialFaces[handle].spec_instance=NULL;
		}


		if (SpecialFaces[handle].vertnorms)
		{
			mem_free (SpecialFaces[handle].vertnorms);
			SpecialFaces[handle].vertnorms=NULL;
		}
	}
}


