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

#ifndef SHIPPAGE_H
#define SHIPPAGE_H

#include "manage.h"
#include "ship.h"
#include "cfile.h"
#include "pstypes.h"

struct mngs_ship_page {
  ship ship_struct;
  char image_name[PAGENAME_LEN];
  char dying_image_name[PAGENAME_LEN];
  char weapon_name[MAX_PLAYER_WEAPONS][MAX_WB_GUNPOINTS][PAGENAME_LEN];
  char fire_sound_name[MAX_PLAYER_WEAPONS][MAX_WB_FIRING_MASKS][PAGENAME_LEN];
  char med_image_name[PAGENAME_LEN];
  char lo_image_name[PAGENAME_LEN];
  char firing_sound_name[MAX_PLAYER_WEAPONS][PAGENAME_LEN];
  char release_sound_name[MAX_PLAYER_WEAPONS][PAGENAME_LEN];
  char spew_powerup_name[MAX_PLAYER_WEAPONS][PAGENAME_LEN];
};

// Ship page functions
//---------------------------------------------------------------

// Given an open file pointer and a ship_page struct, writes that shippage out
void mng_WriteShipPage(CFILE *outfile, mngs_ship_page *shippage);

// Reads a ship page from an open file.  Returns 0 on error.
int mng_ReadShipPage(CFILE *infile, mngs_ship_page *shippage);

// Given an open file pointer and a ship_page struct, writes that shippage out
void mng_WriteNewShipPage(CFILE *outfile, mngs_ship_page *shippage);

// Reads a ship page from an open file.  Returns 0 on error.
int mng_ReadNewShipPage(CFILE *infile, mngs_ship_page *shippage);

// Reads in the shippage named "name" into shippage struct
// Returns 0 on error or couldn't find, else 1 if all is good
int mng_FindSpecificShipPage(char *name, mngs_ship_page *shippage, int offset = 0);

// Given a ship page, allocs a ship and calls AssignShipPageToShip to actually
// load model and values. Rturns ship handle on success, -1 if fail
int mng_SetAndLoadShip(mngs_ship_page *shippage, CFILE *infile);

// Given a shippage and a ship handle, attempts to make ship n correspond to
// to the shippage.
// Returns 1 on success, 0 otherwise
int mng_AssignShipPageToShip(mngs_ship_page *shippage, int n, CFILE *infile = NULL);

// Copies values from a Ship into a ship_page
void mng_AssignShipToShipPage(int n, mngs_ship_page *shippage);

// Reads in a ship page from the local table file, superseding any ship
// already in RAM with that same name
void mng_LoadLocalShipPage(CFILE *);

// Reads in a page off the net
void mng_LoadNetShipPage(CFILE *, bool overlay = false);

#endif
