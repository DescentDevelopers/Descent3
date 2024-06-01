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
*/

#ifndef GAMEFILEPAGE_H
#define GAMEFILEPAGE_H

#include "manage.h"
#include "cfile.h"
#include "pstypes.h"
#include "gamefile.h"

struct mngs_gamefile_page {
  gamefile gamefile_struct;
};

// Gamefile page functions
//---------------------------------------------------------------

// Given an open file pointer and a gamefile_page struct, writes that gamefile page out
void mng_WriteGamefilePage(CFILE *outfile, mngs_gamefile_page *gamefilepage);

// Reads a gamefile page from an open file.  Returns 0 on error.
int mng_ReadGamefilePage(CFILE *infile, mngs_gamefile_page *gamefilepage);

// Given an open file pointer and a gamefile_page struct, writes that gamefile page out
void mng_WriteNewGamefilePage(CFILE *outfile, mngs_gamefile_page *gamefilepage);

// Reads a gamefile page from an open file.  Returns 0 on error.
int mng_ReadNewGamefilePage(CFILE *infile, mngs_gamefile_page *gamefilepage);

// Reads in the gamefilepage named "name" into gamefilepage struct
// Returns 0 on error or couldn't find, else 1 if all is good
int mng_FindSpecificGamefilePage(char *name, mngs_gamefile_page *gamefilepage, int offset = 0);

// Given a gamefile page, allocs a gamefile and calls AssignGamefilePageToGamefile to actually
// load bitmaps and values. Rturns gamefile handle on success, -1 if fail
int mng_SetAndLoadGamefile(mngs_gamefile_page *gamefilepage);

// Given a gamefilepage and a gamefile handle, attempts to make gamefile n correspond to
// to the gamefilepage.
// Returns 1 on success, 0 otherwise
int mng_AssignGamefilePageToGamefile(mngs_gamefile_page *gamefilepage, int n);

// Copies values from a gamefile into a gamefile_page
void mng_AssignGamefileToGamefilePage(int n, mngs_gamefile_page *gamefilepage);

// Reads in a gamefile page from the local table file, superseding any gamefile
// already in RAM with that same name
void mng_LoadLocalGamefilePage(CFILE *);

// Reads in a page off the net
void mng_LoadNetGamefilePage(CFILE *, bool overlay = false);

// First searches through the gamefile index to see if the gamefile is already
// loaded.  If not, searches in the table file and loads it.
// Returns index of gamefile if found, -1 if not
int mng_GetGuaranteedGamefilePage(char *name, CFILE *infile = NULL);

#endif
