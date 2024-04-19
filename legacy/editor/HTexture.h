/*
 * $Logfile: /DescentIII/Main/editor/HTexture.h $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:57:38 $
 * $Author: kevinb $
 *
 * HTexture handler for texture functions
 *
 * $Log: not supported by cvs2svn $
 * 
 * 12    8/30/99 12:55p Gwar
 * added a parameter to HTexturePropagateToFace to not change the texture
 * of the target face
 * 
 * 11    5/19/99 12:21a Gwar
 * 
 * 10    4/27/99 3:30p Gwar
 * added a NEWEDITOR #define
 * 
 * 9     8/15/98 5:04p Matt
 * Added code to copy a texture & UVs from one face to another
 * 
 * 8     4/16/98 10:42a Matt
 * Made texture propagation work across rooms.
 * 
 * 7     9/17/97 11:57a Matt
 * Ripped out segment code
 * 
 * 6     9/17/97 11:17a Samir
 * BIG SEGMENT RIPOUT
 * 
 * 5     8/13/97 9:52a Matt
 * Combined slide left/right/up/down into one function, and the same with
 * rotate left/right
 * 
 * 4     8/01/97 4:53p Jason
 * made a more general PropagateTextureToFace function call
 * 
 * 4     4/17/97 4:03p Samir
 * Apply Tmap 1 and Tmap2 implemented, but no Tmap2 rendering is occurring
 * yet.
 * 
 * 3     3/31/97 3:50p Samir
 * Set default texture uvs should work for side not whole segment.
 * 
 * 2     2/26/97 3:59p Samir
 * Most scaling, rotating, sliding and flipping work.
 * 
 * 1     2/20/97 11:52a Samir
 *
 * $NoKeywords: $
 */

#ifndef HTEXTURE_H
#define HTEXTURE_H

#include "pstypes.h"

#ifndef NEWEDITOR
#include "d3edit.h"
#else
#include "../neweditor/stdafx.h"
#include "../neweditor/neweditor.h"
#include "../neweditor/globals.h"
#endif

struct room;

//	function to apply a texture to a segment side.
void HTextureStretchLess(room *rp, int face, int edge);
void HTextureStretchMore(room *rp, int face, int edge);
void HTextureSetDefault(room *rp, int face=Curface);
void HTextureFlipX();
void HTextureFlipY();
void HTextureSlide(room *rp, int facenum, float right, float up);
void HTextureRotate(room *rp, int facenum, angle ang);
void HTextureNextEdge();

//Apply the specified texture to the specified room:face
void HTextureApplyToRoomFace(room *rp,int facenum,int tnum);

//Copy texture from current face to adjacent face, tiling the UV coordinates
//Parameters:	destrp,destface - room:face that the propagate is based on
//					srcrp,srcface - room:face that is changed
//Return:	1 if success, 0 if faces not adjacent
int HTexturePropagateToFace(room *destrp,int destface,room *srcrp,int srcface,bool tex=true);

//Copy texture UVs from one face to another
//Parameters:	destrp,destface - room:face that is changed
//					srcrp,srcface - room:face to copy from
//					offset - vert 0 on source is assigned to vert offset on dest
//Return:	1 if success, 0 if faces don't have the same number of verts
int HTextureCopyUVsToFace(room *destrp,int destface,room *srcrp,int srcface,int offset);

#endif