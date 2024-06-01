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

#ifndef MEGACELLPAGE_H
#define MEGACELLPAGE_H

#include "manage.h"
#include "megacell.h"
#include "cfile.h"
#include "pstypes.h"

struct mngs_megacell_page {
  megacell megacell_struct;
  char cellname[MAX_MEGACELL_WIDTH * MAX_MEGACELL_HEIGHT][PAGENAME_LEN];
};

// Megacell page functions
//---------------------------------------------------------------

// Given an open file pointer and a megacell_page struct, writes that megacellpage out
void mng_WriteMegacellPage(CFILE *outfile, mngs_megacell_page *megacellpage);

// Reads a megacell page from an open file.  Returns 0 on error.
int mng_ReadMegacellPage(CFILE *infile, mngs_megacell_page *megacellpage);

// Given an open file pointer and a megacell_page struct, writes that megacellpage out
void mng_WriteNewMegacellPage(CFILE *outfile, mngs_megacell_page *megacellpage);

// Reads a megacell page from an open file.  Returns 0 on error.
int mng_ReadNewMegacellPage(CFILE *infile, mngs_megacell_page *megacellpage);

// Reads in the megacellpage named "name" into megacellpage struct
// Returns 0 on error or couldn't find, else 1 if all is good
int mng_FindSpecificMegacellPage(char *name, mngs_megacell_page *megacellpage, int local);

// Given a megacell page, allocs a megacell and calls AssignMegacellPageToMegacell to actually
// load model and values. Rturns megacell handle on success, -1 if fail
int mng_SetAndLoadMegacell(mngs_megacell_page *megacellpage);

// Given a megacellpage and a megacell handle, attempts to make megacell n correspond to
// to the megacellpage.
// Returns 1 on success, 0 otherwise
int mng_AssignMegacellPageToMegacell(mngs_megacell_page *megacellpage, int n);

// Copies values from a Megacell into a megacell_page
void mng_AssignMegacellToMegacellPage(int n, mngs_megacell_page *megacellpage);

// Reads in a megacell page from the local table file, superseding any megacell
// already in RAM with that same name
void mng_LoadLocalMegacellPage(CFILE *);

// Reads in a page off the net
void mng_LoadNetMegacellPage(CFILE *);

#endif
