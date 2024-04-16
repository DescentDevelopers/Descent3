/*
 * $Logfile: /DescentIII/main/scorch.h $
 * $Revision: 3 $
 * $Date: 5/25/98 8:36p $
 * $Author: Matt $
 *
 * Header for scorch system
 *
 * $Log: /DescentIII/main/scorch.h $
 * 
 * 3     5/25/98 8:36p Matt
 * Added code to set different sizes for different weapon scorch marks.
 * Also, don't leave scorch marks on lights.
 * 
 * 2     5/22/98 12:34p Matt
 * Added scorch mark/bullet hole system.
 * 
 * 1     5/21/98 11:32p Matt
 * 
 */

#include "vecmat.h"

//Called when a new level is started to reset the data
void ResetScorches();

//Add a scorch mark
//Parameters:	roomnum,facenum - the face that the scorch is on
//					pos - where the scorch goes
//					texture_handle - the texture for the scorch mark
//					size - how big the scorch is
void AddScorch(int roomnum,int facenum,vector *pos,int texture_handle,float size);

//Draw the scorch(es) for a given face
void DrawScorches(int roomnum,int facenum);


