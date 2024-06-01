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

 * $Logfile: /DescentIII/main/ambient.h $
 * $Revision: 4 $
 * $Date: 2/12/99 5:33p $
 * $Author: Kevin $
 *
 * Header for ambient sound system
 *
 * $Log: /DescentIII/main/ambient.h $
 *
 * 4     2/12/99 5:33p Kevin
 * Added table file flag thingy
 *
 * 3     2/10/99 2:31p Matt
 * Deleted some unused code and fixed a slightly stupid thing.
 *
 * 2     8/20/98 12:35p Matt
 * Added nice editing for ambient sound patterns
 *
 * 1     8/17/98 7:00p Matt
 * Added ambient sound system
 *
 */

#ifndef _AMBIENT_H
#define _AMBIENT_H

#include "manage.h"

// An Ambient Sound Element, one part of an Ambient Sound Pattern (ASP)
struct ase {
  int handle;                   // the sample's handle
  float min_volume, max_volume; // minimum and maximum volume
  int probability;              // between 0 and 100
};

// An Ambient Sound Element, one part of an Ambient Sound Pattern (ASP)
struct asp {
  char name[PAGENAME_LEN];    // the name of this pattern
  float min_delay, max_delay; // delay time between sounds
  int num_sounds;             // how many sounds in this pattern
  ase *sounds;                // array of sounds
  float delay;                // how long until the next sound
};

extern int Num_ambient_sound_patterns;
extern asp Ambient_sound_patterns[];

#define AMBIENT_FILE_NAME "ambient.dat"
/*
$$TABLE_GENERIC "ambient.dat"
*/

// Initialize the ambient sound system
void InitAmbientSoundSystem();

// Does the ambient sound processing for one frame, maybe playing a sound or two
void DoAmbientSounds();

// Initializes the ambient sounds for the current level
void InitAmbientSounds();

// Return the index of a named ambient sound pattern
// Returns number, or -1 if can't find
int FindAmbientSoundPattern(char *aspname);

// Returns a pointer to the name of the specified ambient sound pattern
char *AmbientSoundPatternName(int n);

// Reads data from the ambient sound data file
void ReadAmbientData();

// Writes data from the ambient sound data file
void WriteAmbientData();

#endif
