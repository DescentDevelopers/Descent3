#ifndef POSTRENDER_H
#define POSTRENDER_H

#define MAX_POSTRENDERS	3000

#define PRT_OBJECT				0
#define PRT_VISEFFECT			1
#define PRT_WALL				2


typedef struct 
{
	ubyte type;		// See types above
	union
	{
		short objnum;
		short visnum;
		short facenum;
	};
	
	short roomnum;
	float z;
} postrender_struct;

extern int Num_postrenders;
extern postrender_struct Postrender_list[];

void ResetPostrenderList ();

// Renders all the objects/viseffects/walls we have in our postrender list
void PostRender(int);

void DrawPostrenderFace (int roomnum,int facenum,bool change_z=true);


#endif