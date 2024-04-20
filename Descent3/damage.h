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

/*
 * $Logfile: /DescentIII/Main/damage.h $
 * $Revision: 20 $
 * $Date: 4/25/99 10:38p $
 * $Author: Matt $
 *
 *	Applies and handles damage to game entities.
 *
 * $Log: /DescentIII/Main/damage.h $
 *
 * 20    4/25/99 10:38p Matt
 * Made the Osiris kill object function work on players.
 *
 * 19    4/25/99 10:19p Matt
 * Fixed multiplayer and demo problems will killing an object from script,
 * and cleaned up the death code a bit in the process.
 *
 * 18    4/24/99 4:32p Jason
 * made fusion damage sound not be irritating
 *
 * 17    4/02/99 11:23a Matt
 * Made KillObject not take a death_info struct, but rather the death info
 * as individual parameters.  Moved death_info into objinfo.h, since it's
 * only used for generic objects.  Took out fade-away death hack, now that
 * fade-away deaths can be explicitely set.
 *
 * 16    3/22/99 2:00p Matt
 * Moved breaking glass code from collide.cpp to damage.cpp
 *
 * 15    2/25/99 11:01a Matt
 * Added new explosion system.
 *
 * 14    1/20/99 2:13a Chris
 * It is now possible for robots to have special immunities, resistances,
 * and vunerabilities
 *
 * 13    11/28/98 3:11p Jason
 * made damage glow much more apparent
 *
 * 12    11/19/98 5:40p Kevin
 * Demo system
 *
 * 11    10/19/98 11:20p Matt
 * Changed player damage types to start at 0 instead of -1 to avoid -1 ->
 * 255 problem in multiplayer.
 *
 * 10    10/19/98 7:19p Matt
 * Added system to support different types of damage to the player and
 * have these different types make different sounds.
 *
 * 9     5/04/98 4:12p Chris
 *
 * 8     5/04/98 4:04p Chris
 * Minestone checkin' -- energy effect and more AI functionality
 *
 * 7     4/17/98 1:59p Jason
 * added cool object effects
 *
 * 6     2/18/98 3:43a Mark
 *
 * 5     1/20/98 6:01p Jason
 * first pass at getting multiplayer deaths working
 *
 * 4     1/15/98 6:49p Jason
 * incremental checkin for multiplayer
 *
 * 3     12/01/97 11:02a Jason
 * made camera shake from explosions
 *
 * 2     12/01/97 9:54a Chris
 * Added support for concussive forces, generalized robot collisions to
 * generic collisions
 *
 * 1     10/01/97 4:50p Samir
 * Damage for players and robots.
 *
 * $NoKeywords: $
 */

#ifndef DAMAGE_H
#define DAMAGE_H

#include "damage_external.h"
#include "vecmat.h"
#include "DeathInfo.h"

// Maximum damage magnitude
#define MAX_DAMAGE_MAG 20.0f
#define MAX_EDRAIN_MAG 18.0f

struct object;
struct room;

//	Applies damage to a player object, returns true if damage is applied.
bool ApplyDamageToPlayer(object *playerobj, object *killer, int damage_type, float damage_amount, int server_says = 0,
                         int weapon_id = 255, bool playsound = 1);

//	Applies damage to a robot object, returns true if damage is applied.
bool ApplyDamageToGeneric(object *robotobj, object *killer, int damage_type, float damage, int server_says = 0,
                          int weapon_id = 255);

// Starts on object on fire
void SetNapalmDamageEffect(object *obj, object *killer, int weapon_id);

// Chrishack -- milestone
void DecreasePlayerEnergy(int slot, float energy);

// Adds a bit of shake to the camera
void AddToShakeMagnitude(float delta);

// This function sortof replaces ExplodeObject()
// Parameters:	objp - the object to destroy
//					killer - the object who is killing it, or NULL
//					damage - how much damage was applied in the death blow?
//					death_flags - how the object dies
//					delay_time - how long to delay, if a timed delay
void KillObject(object *objp, object *killer, float damage, int death_flags, float delay_time);

// Applies a default death to an object
// Figures out what sort of death to do, then calls the other KillObject()
// Parameters:	objp - the object to destroy
//					killer - the object who is killing it, or NULL
//					damage - how much damage was applied in the death blow?
void KillObject(object *objp, object *killer, float damage);

// Kills the player
// weapon_id can be -1 for no weapon
void KillPlayer(object *playerobj, object *killer, float damage_amount, int weapon_id);

// Shakes player by some random amount
void ShakePlayer();

// Restores the player orientation matrix after shaking
void UnshakePlayer();

// Break the specified (glass) face into shards
// Parameters:	rp, facenum - the face to break
//					hitpnt - the point on the face where the face shatters.  If NULL, uses center
// point of
// face 					hitvec - the direction in which the thing that's breaking the glass is
// moving.  If NULL, 						uses the negative of the surface normal
void BreakGlassFace(room *rp, int facenum, vector *hitpnt = NULL, vector *hitvec = NULL);

#endif