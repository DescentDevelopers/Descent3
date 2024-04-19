/*
 * $Logfile: /DescentIII/Main/editor/RoomUVs.h $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:57:38 $
 * $Author: kevinb $
 *
 * Header for RoomUVs.cpp
 *
 * $Log: not supported by cvs2svn $
 * 
 * 4     9/30/99 6:35p Matt
 * Added code to scale the UVs of a texture based on the size of the
 * texture when the texture is applied to a face.  This will cause the
 * applied texture to have the make 3D texel size at the old texture.
 * 
 * 3     4/02/98 12:23p Jason
 * trimmed some fat from our structures
 * 
 * 2     7/18/97 7:38p Jason
 * finished room/terrain texture modifications
 *
 * $NoKeywords: $
 */

#include "room.h"


//	Given u,v coordinates at two vertices, assign u,v coordinates to the other vertices on a face.
//	va, vb = face-relative vertex indices corresponding to uva, uvb.  Ie, they are always in 0..num_verts_in_face
void AssignUVsToFace(room *rp, int facenum, roomUVL *uva, roomUVL *uvb, int va, int vb);

// Stretches the UVS of a face 
// Edge is the vertex number - so the edge is actually edge,edge+1
void StretchRoomUVs(room *rp, int facenum, int edge);

//Scale all the UV values in a face from the center point (as defined by averaging the u & v values)
void ScaleFaceUVs(room *rp,int facenum,float scale);

