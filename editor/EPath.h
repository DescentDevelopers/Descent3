/*
 * $Logfile: /DescentIII/Main/editor/EPath.h $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:57:38 $
 * $Author: kevinb $
 *
 * Header for epaths.cpp
 *
 * $Log: not supported by cvs2svn $
 * 
 * 3     2/03/99 6:57p Chris
 * Added the "Move node to current object" function
 * 
 * 2     2/10/98 10:48a Matt
 * Moved editor code from gamepath.cpp to epath.cpp
 * 
 * 1     2/10/98 10:13a Matt
 * 
 */

#include "vecmat.h"
#include "gamepath.h"

extern ubyte Show_paths;

// Allocs a gamepath that a robot will follow.  Returns an index into the GamePaths
// array 
int AllocGamePath (void);

// Given a path number, and a node number in that path, adds another node after the 
// specified node
// Returns the index number of the new node
// If nodenum is -1, this node couldn't be added
// Flags are passed via the flags field
int InsertNodeIntoPath (int pathnum,int nodenum,int flags);

void FreeGamePath(int n);

// Given a pathnum and a node index, deletes that node and moves all the following nodes down
// by one
void DeleteNodeFromPath (int pathnum,int nodenum);

// Given a path number and a node, it moves the node by the change in position (if the new position is valid)
int MovePathNode(int pathnum, int nodenum, vector *delta_pos);

// Given a path number and a node, it moves the node to the position (if the new position is valid)
int MovePathNodeToPos(int pathnum, int nodenum, vector *pos);

// Gets next path from n that has actually been alloced
int GetNextPath (int n);
// Gets previous path from n that has actually been alloced
int GetPrevPath (int n);

// returns the index of the first path (from 0) alloced
// returns -1 if there are no paths
int GetFirstPath ();

// draws a vector number
void DrawNumber (int num,vector pos,float size);

class grViewport;
// draws all the paths
void DrawAllPaths (grViewport *vp,vector *viewer_eye,matrix *viewer_orient,float zoom);


