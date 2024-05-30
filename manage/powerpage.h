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

#ifndef POWERPAGE_H
#define POWERPAGE_H

#include "manage.h"
#include "cfile.h"
#include "pstypes.h"
#include "powerup.h"

struct mngs_power_page {
  powerup powerup_struct;
  char image_name[PAGENAME_LEN];
  char sound_name[MAX_POWERUP_SOUNDS][PAGENAME_LEN];
};

// Powerup page functions
//---------------------------------------------------------------

// Given an open file pointer and a power_page struct, writes that power page out
void mng_WritePowerPage(CFILE *outfile, mngs_power_page *powpage);

// Reads a powerup page from an open file.  Returns 0 on error.
int mng_ReadPowerPage(CFILE *infile, mngs_power_page *powpage);

// Given a power handle, searches the table file and replaces the powerup with the same name
// If local=1, then does it to the users local copy
// Returns 0 on error, else 1 if all is good
int mng_ReplacePowPage(char *name, int handle, int local);

// Given a powerup name, finds it in the table file and deletes it
// If local is 1, deletes from the local table file
int mng_DeletePowPage(char *name, int local);

// Reads in the powpage named "name" into powpage struct
// Returns 0 on error or couldn't find, else 1 if all is good
int mng_FindSpecificPowPage(char *name, mngs_power_page *powpage, int offset = 0);

// Given a power page, allocs a powerup and calls AssignPowPageToPowerup to actually
// load bitmaps and values. Rturns powerup handle on success, -1 if fail
int mng_SetAndLoadPowerup(mngs_power_page *powpage);

// Given a powpage and a powerup handle, attempts to make powerup n correspond to
// to the powpage.
// Returns 1 on success, 0 otherwise
int mng_AssignPowPageToPowerup(mngs_power_page *powpage, int n);

// Copies values from a powerup into a power_page
void mng_AssignPowerupToPowPage(int n, mngs_power_page *powpage);

// Reads in a power page from the local table file, superseding any powerup
// already in RAM with that same name
void mng_LoadLocalPowerupPage(CFILE *);

// Reads in a page off the net
void mng_LoadNetPowerupPage(CFILE *);

// First searches through the powerup index to see if the powerup is already
// loaded.  If not, searches in the table file and loads it.
// Returns index of powerup if found, -1 if not
int mng_GetGuaranteedPowerupPage(char *name, CFILE *infile = NULL);

#endif
