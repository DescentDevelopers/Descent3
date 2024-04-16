/*
 * $Logfile: /DescentIII/Main/door.h $
 * $Revision: 11 $
 * $Date: 6/10/99 1:11p $
 * $Author: Gwar $
 *
 * doorway structures and side info.
 *
 * $Log: /DescentIII/Main/door.h $
 *
 * 11    6/10/99 1:11p Gwar
 * adding door support for NEWEDITOR
 *
 * 10    2/25/99 1:26p Sean
 * Increased MAX_DOORS from 50 to 60 (MattT on Sean's machine)
 *
 * 9     2/04/99 2:05p Matt
 * Added blastable doors
 *
 * 8     1/21/99 11:15p Jeff
 * pulled out some structs and defines from header files and moved them
 * into seperate header files so that multiplayer dlls don't require major
 * game headers, just those new headers.  Side effect is a shorter build
 * time.  Also cleaned up some header file #includes that weren't needed.
 * This affected polymodel.h, object.h, player.h, vecmat.h, room.h,
 * manage.h and multi.h
 *
 * 7     12/18/98 6:00p Jeff
 * added support for door scripts in new osiris
 *
 * 6     5/22/98 8:50p Matt
 * Added sounds for doors
 *
 * 5     4/06/98 4:53p Jason
 * got pageable polymodels working correctly with editor
 *
 * 4     2/02/98 7:07p Matt
 * Added support for doors that can be seen through even when closed
 *
 * 3     10/06/97 1:01p Jason
 * made doors work with scripts (sort of)
 *
 * 2     9/17/97 11:16a Matt
 * Ripped out segment code
 *
 * 11    3/13/97 6:13p Jason
 * got poly doors working
 *
 * 10    3/12/97 2:15p Samir
 * Removed global references to doorways.
 *
 * 9     3/12/97 12:37p Samir
 * added timing elements to door structure.
 *
 * 8     3/07/97 2:19p Jason
 * added DrawDoorSegment function
 *
 * 7     3/05/97 2:59p Samir
 * Made segnum, sidenum shorts and added prorotype for RemapDoors.
 *
 * 6     3/05/97 1:01p Samir
 * Used doornum instead of polyobj in doorway struct
 *
 * 5     3/05/97 12:16p Jason
 * added code to support our new 3d doors
 *
 * 4     2/27/97 5:09p Samir
 * Key flags defined in door code.
 *
 * 3     2/26/97 4:25p Samir
 * Call GetDoorFlags to get door info.
 *
 * 2     2/25/97 4:35p Samir
 * Added SIDE_FLAGS macro.
 *
 * $NoKeywords: $
 */

#ifndef DOOR_H
#define DOOR_H

#ifdef NEWEDITOR /* only include tablefile header (manage stuff for NEWEDITOR) */
#include "..\neweditor\ned_TableFile.h"
#include "..\neweditor\ned_Door.h"
#include "pstypes.h"
#include "object.h"
#else

#include "pstypes.h"
#include "manage.h"
#include "object.h"

// IMPORTANT!!!!!!!!!!!
// "Doors" refers to a predefined door that is in memory
// "Doorways" are specific doors that are in the mine
// So, there can be several Doorways that all point to the same Door
// Get it?  If not, talk to Samir or Jason

// Door flags

#define DF_BLASTABLE 1  // this door can be destroyed
#define DF_SEETHROUGH 2 // this door can be seen through even when closed

//	DOOR STRUCTURES

typedef struct {
  char name[PAGENAME_LEN]; // name of the door
  ubyte used;              // if this door is in use
  ubyte flags;             // flags for this door
  ubyte pad;               // keep alignment (pagename is 35 chars long)
  short hit_points;        // for blastable doors
  float total_open_time;   // time of animation to open door
  float total_close_time;  // time of animation to close door
  float total_time_open;   // how much time to stay open
  int model_handle;        // an index into the polymodels array
  int open_sound;          // sound to play when closing
  int close_sound;         // sound to play when closing

  // OSIRIS information
  char module_name[MAX_MODULENAME_LEN];

} door;

// The max number of predefined doors
#define MAX_DOORS 60
extern int Num_doors; // number of actual doors in game.
extern door Doors[];

// Sets all doors to unused
void InitDoors();

// Allocs a door for use, returns -1 if error, else index on success
int AllocDoor();

// Frees door index n
void FreeDoor(int n);

// Gets next door from n that has actually been alloced
int GetNextDoor(int n);
// Gets previous door from n that has actually been alloced
int GetPrevDoor(int n);
// Searches thru all doors for a specific name, returns -1 if not found
// or index of door with name
int FindDoorName(char *name);

// Given a filename, loads the model found in that file
int LoadDoorImage(char *filename, int pageable = 1);
// Given a door handle, returns an index to that doors model
int GetDoorImage(int handle);

//	Remaps the doors
void RemapDoors();

#endif

#endif