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

 * $Logfile: /DescentIII/Main/viseffect.h $
 * $Revision: 33 $
 * $Date: 3/20/00 1:35p $
 * $Author: Matt $
 *
 * Header for viseffect.cpp
 *
 * $Log: /DescentIII/Main/viseffect.h $
 *
 * 33    3/20/00 1:35p Matt
 * Merge of Duane's post-1.3 changes.
 * Moved max_vis_effects (the variable, not the constant) from viseffect.h
 * to viseffect_external.h.
 *
 * 31    10/21/99 9:27p Jeff
 * B.A. Macintosh code merge
 *
 * 30    4/30/99 7:37p Jeff
 * created viseffect_external.h
 *
 * 29    4/19/99 3:17p Jason
 * made real vis effects work
 *
 * 28    3/31/99 11:40a Jason
 * added support for attached thick lightning
 *
 * 27    3/23/99 4:10p Jason
 * more tweaks for line sparks
 *
 * 26    3/23/99 12:50p Jason
 * added line sparks
 *
 * 25    12/11/98 5:08p Jason
 * changed mass driver effect
 *
 * 24    9/29/98 7:15p Jason
 * added axis billboards
 *
 * 23    9/18/98 8:23p Jason
 * fixed insidious vis effect errors
 *
 * 22    9/17/98 3:03p Jason
 * added lightning and invul hit effects
 *
 * 21    6/23/98 3:34p Jason
 * added cool lighting effect to gravity field
 *
 * 20    6/22/98 6:52p Jason
 * changed gravity field effect somewhat
 *
 * 19    6/12/98 5:14p Jason
 * made viseffects deal with zbuffer better
 *
 * 18    4/29/98 11:38a Jason
 * added some weather effects (first pass)
 *
 * 17    4/19/98 5:00p Jason
 * added cool napalm effect, plus made object effects dynamically
 * allocated
 *
 * 16    4/15/98 2:33p Jason
 * changed some smoke trail stuff
 *
 * 15    4/15/98 12:22p Jason
 * lots of miscellaneous stuff pertaining to lighting and vis effects
 *
 * 14    4/10/98 12:39p Jason
 * added expanding explosion bitmaps
 *
 * 13    4/07/98 3:31p Jason
 * got particle effects working with weapons
 *
 * 12    4/07/98 12:54p Jason
 * changes for viseffects and multiplayer dll
 *
 * 11    2/16/98 2:37p Jason
 * added real viseffects
 *
 * 10    2/15/98 1:21a Jason
 * doubled the amount of viseffects
 *
 * 9     2/04/98 9:28p Jason
 * added some new weapons effects
 *
 * 8     2/04/98 6:58p Matt
 * Fixed viseffect rendering on terrain.
 *
 * 7     2/04/98 6:09p Matt
 * Changed object room number to indicate a terrain cell via a flag.  Got
 * rid of the object flag which used to indicate terrain.
 *
 */

#ifndef VIS_EFFECT_H
#define VIS_EFFECT_H

#include "viseffect_external.h"

extern vis_effect *VisEffects;
extern int Highest_vis_effect_index;

// Returns the next free viseffect
int VisEffectAllocate();

// Frees up a viseffect for use
int VisEffectFree(int visnum);

int VisEffectInitType(vis_effect *vis);

// initialize a new viseffect.  adds to the list for the given room
// returns the object number
int VisEffectCreate(ubyte type, ubyte id, int roomnum, vector *pos);

// link the viseffect  into the list for its room
// Does nothing for effects over terrain
void VisEffectLink(int visnum, int roomnum);

// Unlinks a viseffect from a room
// Does nothing for terrain
void VisEffectUnlink(int visnum);

// when an effect has moved into a new room, this function unlinks it
// from its old room and links it into the new room
void VisEffectRelink(int visnum, int newroomnum);

// Frees all the vis effects that are currently in use
void FreeAllVisEffects();

// Goes through our array and clears the slots out
void InitVisEffects();

// remove viseffect from the world
void VisEffectDelete(int visnum);

// Kills all the effects that are dead
void VisEffectDeleteDead();

// Moves our visuals
void VisEffectMoveAll();

// Renders a vis effect
void DrawVisEffect(vis_effect *vis);

// Creates a some sparks that go in random directions
void CreateRandomSparks(int num_sparks, vector *pos, int roomnum, int which_index = -1, float force_scalar = 1);

// Creates a some line sparks that go in random directions
void CreateRandomLineSparks(int num_sparks, vector *pos, int roomnum, ushort color = 0, float force_scalar = 1);

// Creates vis effects but has the caller set their parameters
// initialize a new viseffect.  adds to the list for the given room
// returns the vis number
int VisEffectCreateControlled(ubyte type, object *parent, ubyte id, int roomnum, vector *pos, float lifetime,
                              vector *velocity, int phys_flags = 0, float size = 0, float mass = 0.0f,
                              float drag = 0.0f, bool isreal = 0);

// Creates a some particles that go in random directions
void CreateRandomParticles(int num_sparks, vector *pos, int roomnum, int bm_handle, float size, float life);

// Attaches viseffects that move with an object
void AttachRandomNapalmEffectsToObject(object *obj);

#endif
