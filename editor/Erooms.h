/*
* Descent 3 
* Copyright (C) 2024 Parallax Software
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.

--- HISTORICAL COMMENTS FOLLOW ---

 * $Logfile: /DescentIII/Main/editor/Erooms.h $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:57:38 $
 * $Author: kevinb $
 *
 * Header for ERooms.cpp
 *
 * $Log: not supported by cvs2svn $
 * 
 * 50    10/14/99 4:21p Matt
 * Added code to remove duplicate faces from a room.
 * 
 * 49    7/14/99 9:07a Gwar
 * added a parameter to AllocLoadRoom
 * 
 * 48    7/02/99 2:17p Gwar
 * added a parameter to AllocLoadRoom to disable centering on origin
 * 
 * 47    6/17/99 7:34a Gwar
 * added some function prototypes
 * 
 * 46    5/05/99 3:02a Gwar
 * added a parameter to DeleteRoomFace (by permission)
 * 
 * 45    4/06/99 5:10p Matt
 * Base room shell on all portals in the room, not just the first one.
 * 
 * 44    4/01/99 9:42a Matt
 * Added code to compute room shells, and made verify level do it.
 * 
 * 43    3/30/99 7:57p Matt
 * Added prototype and moved a function to be inline in the header.
 * 
 * 42    3/30/99 7:20p Matt
 * Deleted PointsAreColinear(), since it was never used and worked the
 * wrong way.
 * 
 * 41    2/03/99 1:10p Matt
 * Changed the paletted room current faces to be stored in a seperate
 * array, instead of in the room structure.
 * 
 * 40    10/03/98 8:05p Matt
 * Changed the way CheckIfFaceIsPlanar() works (& renamed it to
 * FaceIsPlanar()) and added planar check to VerifyMine() & VerifyRoom()
 * 
 * 39    9/09/98 4:02p Matt
 * Added default value for parm to CreateNewRoom()
 * 
 * 38    9/08/98 6:12p Matt
 * Added code to check for and fix duplicate and unused points.
 * 
 * 37    9/02/98 4:20p Matt
 * Cleaned up some linking code now that we don't have multi-face portals.
 * 
 * 36    9/02/98 2:28p Matt
 * New function to add a vertex to a face.
 * 
 * 35    8/31/98 4:37p Matt
 * Added improved room & mine error checking.
 * 
 * 34    6/08/98 12:27p Matt
 * Added function to copy from one face to another the flags that should
 * be inherited.
 * 
 * 33    4/27/98 6:43p Matt
 * Cleaned up epsilons some more, and moved some code here from hroom.cpp.
 * 
 * 32    4/16/98 10:41a Matt
 * Added FindSharedEdgeAcrossRooms()
 * 
 * 31    4/02/98 12:23p Jason
 * trimmed some fat from our structures
 * 
 * 30    4/01/98 3:26p Matt
 * Changed NORMALS_SAME_VALUE to have a more reasonable value (was
 * 0.99999999, changed to 0.999f)
 * 
 * 29    2/24/98 2:57p Jason
 * adjust planar tolerance (again)
 * 
 * 28    2/24/98 1:54p Jason
 * added much stricter concavity testing
 * 
 * 27    1/20/98 4:05p Matt
 * Move BuildListOfPortalVerts() from roomkeypaddialog.cpp to erooms.cpp
 * 
 * 26    1/15/98 7:34p Matt
 * Revamped error checking when computing face normals
 * 
 * 25    10/13/97 5:08p Matt
 * Moved ComputeRoomBoundingSphere() & CreateRoomObjects() from editor to
 * main
 * 
 * 
 * 24    10/01/97 7:51p Matt
 * Added code for external rooms
 * 
 * 23    9/24/97 3:20p Matt
 * Added ComputeRoomMinMax()
 * 
 * 22    9/22/97 2:29p Matt
 * Added comment
 * 
 * 21    9/19/97 2:52p Jason
 * changes to fix lightmap seam problem
 * 
 * 20    9/17/97 11:50a Jason
 * ripped out segment engine
 * 
 * 19    9/12/97 1:25p Matt
 * Added ifdef around body to prevent errors when included twice
 * 
 * 18    9/02/97 5:17p Jason
 * changes for dynamic lighting
 * 
 * 17    9/02/97 2:28p Matt
 * Changed PointOutsideEdge() to the more generally-useful
 * CheckPointAgainstEdge()
 * Made CopyFace() clear the portal number 
 * 
 * 16    8/29/97 5:44p Matt
 * Moved a couple functions from hroom.cpp, and added a couple more useful
 * functions.
 * 
 * 15    8/26/97 10:24a Matt
 * Removed debugging mprintf()
 * 
 * 14    8/22/97 1:02p Matt
 * Got CompressMine() working for rooms
 * 
 * 13    8/21/97 5:56p Matt
 * Added a bunch of useful functions, and make CheckFaceConcavity() more
 * generally useful.
 * 
 * 12    8/04/97 6:46p Jason
 * added code for a lightmap system
 * 
 * 11    8/01/97 6:11p Matt
 * Added several functions that I needed for attach room code
 * 
 * 10    8/01/97 12:50p Jason
 * added code to support scaling of rooms/faces/edges
 * 
 * 9     7/31/97 3:31p Jason
 * added functions to rotate portal rooms
 * 
 * 8     7/29/97 1:55p Matt
 * Added some generally useful room/face functions
 * 
 * 7     7/20/97 3:41p Jason
 * got room/terrain links working (at least in the terrain)
 * 
 * 6     7/18/97 4:37p Matt
 * Added function CheckFaceConcavity()
 * 
 * 
 *
 * $NoKeywords: $
 */

#ifndef _EROOMS_H
#define _EROOMS_H

#include "room.h"

#define DEFAULT_ROOM_SIZE 20.0

//Values to determine if two points are the same, if a point is on a plane, & if a point is on an edge
#define POINT_TO_POINT_EPSILON	0.1
#define POINT_TO_PLANE_EPSILON	0.1
#define POINT_TO_EDGE_EPSILON		0.1

//List of current faces for the palette rooms
extern int Current_faces[];

//Called at startup to mark all rooms as unused
void InitRooms();

//Returns a free room number, & marks it no longer free.  Returns -1 if none free.
//If palette_room is true, allocate out of the part of the array for the room palette
int GetFreeRoom(bool palette_room);

//Allocates a room & initializes it.  
//All fields except the 3d points and the side types are set to default values.
//Memory is allocated for faces & verts arrays, but the elements are *not* initialized
//If palette_room is true, allocate out of the part of the array for the room palette
//Returns:  pointer to new room, or NULL if no free rooms
room *CreateNewRoom(int nverts,int nfaces,bool palette_room=0);


// Searches thru all rooms for a specific name, returns -1 if not found
// or index of room with name
int FindRoomName (char *name);

// saves a room in our ORF (Outrage room file) format
void SaveRoom (int n,char *filename);

// Allocates a room and then tries to load it
// Returns index into Rooms[] array on success
// -1 on fail
int AllocLoadRoom (char *filename,bool bCenter=true,bool palette_room=1);

// Gets next room from n that has actually been alloced
int GetNextRoom (int n);

//Deterimines whether a face is concave or convex
//Parameters:	num_verts - the number of vertices in the face to be tested
//					face_verts - list of vertex numbers in this face
//					normal - the surface normal of this face
//					verts - array of vertices into which face_verts elements index
//Returns:		If the face is concave, returns the number of the vertex that makes the concavity.
//					If the face is convex, returns -1
//NOTE: A face could have multiple concavities, and this will only find the one with the 
//lowest-numbered vertex
int CheckFaceConcavity(int num_verts,int16_t *face_verts,vector *normal,vector *verts);

// Goes through each face of the passed room and sets the default uvs
void AssignDefaultUVsToRoom (room *rp);

//Computes the center point on a face by averaging the points in the face
void ComputeCenterPointOnFace(vector *vp,room *rp,int facenum);

//Computes the center point on a face by averaging the points in the portal
void ComputeCenterPointOnPortal(vector *vp,room *rp,int portalnum);

//Recompute the surface normals for all the faces in a room
//Parameters:	rp - pointer to the room
//Returns:		true if normals computed ok, false if some normals were bad
bool ResetRoomFaceNormals(room *rp);

//Copies the contents of one face to another.
//Parameters:	dfp - pointer to the destination face.  This face should be uninitialized.
//					sfp - pointer to the source face
void CopyFace(face *dfp,face *sfp);

// Fixes all the concave/nonplanar faces of facelist of room rp
void FixConcaveFaces (room *rp,int *facelist,int facecount);

void AssignDefaultUVsToRoomFace (room *rp,int facenum);

//Changes the number of verts in a face.  Frees and reallocates the face_verts & face_uvls arrays.
//Leaves all other fields the same
void ReInitRoomFace(face *fp,int nverts);

//Determines if two points are close enough together to be considered the same
//Parameters:	v0,v1 - the two points
//Returns:		true if the points are the same or very close; else false
bool PointsAreSame(vector *v0,vector *v1);

//Check to see if a point is in, in front of, or behind a plane
//Parameters:	checkpoint - the point to check
//					planepoint,normal - the plane we're checking against
//Returns:		0 if on the plane, -1 if behind, 1 if in front
int CheckPointToPlane(vector *checkpoint,vector *planepoint,vector *normal);

//Check to see if all the points on a face are in front of a plane
//Parameters:	rp,facenum - the face to check
//					planepoint,normal - define the plane we're checking against
//Returns:		the number of the first point found on the back of the plane, or -1 of all on front
int CheckFaceToPlane(room *rp,int facenum,vector *planepoint,vector *normal);

//Check if a point is inside, outside, or on an edge of a polygon
//Parameters:	checkv - the point to be checked
//					v1,v0 - the edge to check against. Two sequential verts in a clockwise polygon.
//					normal - the surface normal of the polygon
//Returns:	 1 if the point in inside the edge
//				 0 if the point is on the edge
//				-1 if the point is outside the edge
int CheckPointAgainstEdge(vector *checkv,vector *v0,vector *v1,vector *normal);

//Create space for additional vertices in a room.
//Allocates a new array of vertices, copies from the old list, and frees the old list
//The new vertices are at the end of the list, so none of the old vertices change number
//Parameters:	rp - the room
//					n_new_verts - how many vertices are being added to the room
//Returns:		the number of the first new vertex
int RoomAddVertices(room *rp,int num_new_verts);

//Create space for additional faces in a room.
//Allocates a new faces array, copies from the old list, and frees the old list
//The new faces are at the end of the list, so none of the old faces change number
//Parameters:	rp - the room
//					num_new_faces - how many faces are being added to the room
//Returns:		the number of the first new face
int RoomAddFaces(room *rp,int num_new_faces);

//Structure to hold vector and uvl data for clipping
struct vertex {
	vector vec;
	roomUVL	uvl;
};

//Clips on edge of a polygon against another edge
//Parameters:	normal - defines the plane in which these edgs lie
//					v0,v1 - the edge to be clipped
//					v2,v3 - is the edge clipped against
//					newv - filled in with the intersection point
void ClipEdge(vector *normal,vertex *v0,vertex *v1,vector *v2,vector *v3,vertex *newv);

//Finds a shared edge, if one exists, between two faces in the same room
//Parameters:	fp0,fp1 - pointers to the two faces
//					vn0,vn1 - filled in with the vertex numbers of the edge.  These vert numbers are 
//									relative to their own faces.  The shared edge is verts <vn0,vn0+1> on
//									face 0, and <vn1+1,vn1> on face 1
//Returns:		true if a shared edge was found, else false
bool FindSharedEdge(face *fp0,face *fp1,int *vn0,int *vn1);

//Finds a shared edge, if one exists, between two faces in different rooms
//Parameters:	rp0,rp1 - pointers to the two rooms
//					face0,face1 - the face numbers in rp0 & rp1, respectively
//					vn0,vn1 - filled in with the vertex numbers of the edge.  These vert numbers are 
//									relative to their own faces.  The shared edge is verts <vn0,vn0+1> on
//									face 0, and <vn1+1,vn1> on face 1
//Returns:		true if a shared edge was found, else false
bool FindSharedEdgeAcrossRooms(room *rp0,int face0,room *rp1,int face1,int *vn0,int *vn1);

//If two surface normals have a dot product greater than or equal to this value, they are the same
#define NORMALS_SAME_VALUE .999f

//Determines if two normals are the same (or very very close)
//Parameters:	n0,n1 - the two normals
//Returns:		true if they are more-or-less the same, else false
inline bool NormalsAreSame(vector *n0,vector *n1)
{
	float d = *n0 * *n1;

	return (d > NORMALS_SAME_VALUE);
}

//Delete a face from a room
//Parameters:	rp - the room the face is in
//					facenum - the face to be deleted
void DeleteRoomFace(room *rp,int facenum,bool delete_unused_verts = true);

//Deletes a portal from a room.  Does not delete this portal this connects to
//Parameters:	rp - the room the portal is in
//					portalnum - the portal to be deleted
void DeleteRoomPortal(room *rp,int portalnum);

//Remove holes in the room list
void CompressRooms(void);

// Compress mine by getting rid of holes the the room array
void CompressMine(void);

//Copies the data from one room into another
//Note: Portals are not copied, and the new room will have zero portals
//The destination room is assumed to be uninitialized
//Parameters:	destp - the destination room of the copy
//					srcp - the source room for the copy
void CopyRoom(room *destp,room *srcp);

//Links two rooms, creating portals in each room
//Parameters:	roomlist - pointer to the array of rooms
//					room0,face0 - the room & face numbers of the first room
//					room1,face1 - the room & face numbers of the second room
void LinkRooms(room *roomlist,int room0,int face0,int room1,int face1);

//Back when we had multi-face portals, LinkRoomsSimple() created a pair of 1-face portals
#define LinkRoomsSimple LinkRooms

//Finds the min and max x,y,z values of the vertices in a room
//Parameters:	min,max - filled in with the minimum and maximum x,y, & z values, respectively
//					rp = the room
void ComputeRoomMinMax(vector *min,vector *max,room *rp);

//Builds a list of all the vertices in a room that are part of a portal
//Parameters:	rp - the room to check
//					list - filled in with the list of vert numbers.  List should be MAX_VERTS_PER_ROOM big
//Returns:		the number of verts in the list
int BuildListOfPortalVerts (room *rp,int *list);

//Copy the flags from one face to another derrived from the first (by clipping or splitting)
//Only those flags which are safe to copy are copied
//Parameters:	dfp - pointer to the destination face
//					sfp - pointer to the source face
void CopyFaceFlags(face *dfp,face *sfp);

//Test the mine for validity
void VerifyMine();

//Test the given room for validity
void VerifyRoom(room *rp);

//Inserts a vertex into a face
//Parameters:	rp - the room for this face
//					facenum - the face to which to add the vertex
//					new_v - the number of the vertex to add
//					after_v - the new vert is added *after* this vert (index into face verts)
void AddVertToFace(room *rp,int facenum,int new_v,int after_v);

//Removes all the duplicate points in a level
void RemoveAllDuplicateAndUnusedPoints();

//Checks to see if a face is planar.
//See if all the points are within a certain distance of an average point
//Returns 1 if face is planar, 0 if not
bool FaceIsPlanar(int nv,int16_t *face_verts,vector *normal,vector *verts);

//Checks to see if a face is planar.
//See if all the points are within a certain distance of an average point
//Returns 1 if face is planar, 0 if not
inline bool FaceIsPlanar(room *rp,int facenum)
{
	face *fp = &rp->faces[facenum];

	return FaceIsPlanar(fp->num_verts,fp->face_verts,&fp->normal,rp->verts);
}

//Finds the shell for the specified room.  If the shell is found with no errors, sets
//the non-shell flag for those faces not in the shell.  If there are errors finding the
//shell, all faces have the non-shell flag cleared.
//Returns the number of shell errors (unconnected edges) in the room.
//Writes errors to the error buffer
int ComputeRoomShell(room *rp);

//Finds shells for all rooms.
//Returns the number of rooms with bad shells
int ComputeAllRoomShells();

//Log an error in the room check process
void CheckError(const char *str,...);

//Checks the normals in a room
//Returns the number of bad normals
int CheckNormals(room *rp);

//Checks for concave faces
//Returns the number of concave faces
int CheckConcaveFaces(room *rp);

int CheckDegenerateFaces(room *rp);

//Checks for bad portal connections
//Returns the number of bad portals
int CheckPortals(room *rp);

//Checks for duplicate faces
//Returns the number of duplicate faces
int CheckDuplicateFaces(room *rp);

//Checks for non-planar faces
//Returns the number of non-planar faces
int CheckNonPlanarFaces(room *rp);

//Checks for duplicate points
//Returns the number of duplicate points
int CheckDuplicatePoints(room *rp);

//Find any unused points in a room
int CheckUnusedPoints(room *rp);

//Checks for duplicate points in faces
//Returns the number of duplicate face points
int CheckDuplicateFacePoints(room *rp);

int CheckRoomPortalFaces(room *rp);

//Find t-joints in this room
int FindTJoints(room *rp);

// Counts the number of unique textures in a level, plus gives names of textures used
void CountUniqueTextures ();

//Removes all the duplicate faces in a room
void RemoveDuplicateFaces(room *rp);

#endif	//ifndef _EROOMS_H
