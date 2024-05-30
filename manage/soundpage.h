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

#ifndef SOUNDPAGE_H
#define SOUNDPAGE_H

#include "manage.h"
#include "soundload.h"
#include "cfile.h"
#include "pstypes.h"

struct mngs_sound_page {
  sound_info sound_struct;
  char raw_name[PAGENAME_LEN];
};

// Sound page functions
//---------------------------------------------------------------

// Given an open file pointer and a sound_page struct, writes that soundpage out
void mng_WriteSoundPage(CFILE *outfile, mngs_sound_page *soundpage);

// Reads a sound page from an open file.  Returns 0 on error.
int mng_ReadSoundPage(CFILE *infile, mngs_sound_page *soundpage);
// Given an open file pointer and a sound_page struct, writes that soundpage out
void mng_WriteNewSoundPage(CFILE *outfile, mngs_sound_page *soundpage);

// Reads a sound page from an open file.  Returns 0 on error.
int mng_ReadNewSoundPage(CFILE *infile, mngs_sound_page *soundpage);

// Reads in the soundpage named "name" into soundpage struct
// Returns 0 on error or couldn't find, else 1 if all is good
int mng_FindSpecificSoundPage(char *name, mngs_sound_page *soundpage, int offset = 0);

// Given a sound page, allocs a sound and calls AssignSoundPageToSound to actually
// load model and values. Rturns sound handle on success, -1 if fail
int mng_SetAndLoadSound(mngs_sound_page *soundpage);

// Given a soundpage and a sound handle, attempts to make sound n correspond to
// to the soundpage.
// Returns 1 on success, 0 otherwise
int mng_AssignSoundPageToSound(mngs_sound_page *soundpage, int n);

// Copies values from a Sound into a sound_page
void mng_AssignSoundToSoundPage(int n, mngs_sound_page *soundpage);

// Reads in a sound page from the local table file, superseding any sound
// already in RAM with that same name
void mng_LoadLocalSoundPage(CFILE *);

// Reads in a page off the net
void mng_LoadNetSoundPage(CFILE *, bool overlay = false);

// First searches through the sound index to see if the sound is already
// loaded.  If not, searches in the table file and loads it.
// Returns index of sound if found, -1 if not
int mng_GetGuaranteedSoundPage(char *name, CFILE *infile = NULL);

#endif
