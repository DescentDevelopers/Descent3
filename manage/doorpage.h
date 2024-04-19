#ifndef DOORPAGE_H
#define DOORPAGE_H

#include "manage.h"
#include "door.h"
#include "cfile/cfile.h"
#include "pstypes.h"

typedef struct {
  door door_struct;
  char image_name[PAGENAME_LEN];
  char open_sound_name[PAGENAME_LEN];
  char close_sound_name[PAGENAME_LEN];
} mngs_door_page;

// Door page functions
//---------------------------------------------------------------

// Given an open file pointer and a door_page struct, writes that doorpage out
void mng_WriteDoorPage(CFILE *outfile, mngs_door_page *doorpage);

// Reads a door page from an open file.  Returns 0 on error.
int mng_ReadDoorPage(CFILE *infile, mngs_door_page *doorpage);

// Given an open file pointer and a door_page struct, writes that doorpage out
void mng_WriteNewDoorPage(CFILE *outfile, mngs_door_page *doorpage);

// Reads a door page from an open file.  Returns 0 on error.
int mng_ReadNewDoorPage(CFILE *infile, mngs_door_page *doorpage);

// Reads in the doorpage named "name" into doorpage struct
// Returns 0 on error or couldn't find, else 1 if all is good
int mng_FindSpecificDoorPage(char *name, mngs_door_page *doorpage, int offset = 0);

// Given a door page, allocs a door and calls AssignDoorPageToDoor to actually
// load model and values. Rturns door handle on success, -1 if fail
int mng_SetAndLoadDoor(mngs_door_page *doorpage);

// Given a doorpage and a door handle, attempts to make door n correspond to
// to the doorpage.
// Returns 1 on success, 0 otherwise
int mng_AssignDoorPageToDoor(mngs_door_page *doorpage, int n);

// Copies values from a Door into a door_page
void mng_AssignDoorToDoorPage(int n, mngs_door_page *doorpage);

// Reads in a door page from the local table file, superseding any door
// already in RAM with that same name
void mng_LoadLocalDoorPage(CFILE *);

// Reads in a page off the net
void mng_LoadNetDoorPage(CFILE *, bool overlay = false);

#endif