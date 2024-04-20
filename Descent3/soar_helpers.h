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
 * $Logfile: /DescentIII/main/soar_helpers.h $
 * $Revision: 4 $
 * $Date: 4/16/99 12:33a $
 * $Author: Matt $
 *
 * Header for soar_helpers.cpp
 *
 * $Log: /DescentIII/main/soar_helpers.h $
 *
 * 4     4/16/99 12:33a Matt
 * Disable Soar on non-Windows systems.
 *
 */

#ifndef SOAR_HELPERS_H_
#define SOAR_HELPERS_H_

#include "soar.h"

#ifdef SOAR_ENABLED

#include "object.h"
#include "vecmat.h"
#include "controls.h"
#include "weapon.h"
#include "room.h"
#include "terrain.h"
#include "player.h"

// Functions passed to Soar in this order

bool DSGetHighestObjIndex(int &n);
bool DSGetObjSize(int n, float *size);
bool DSGetObjMass(int n, float *mass);
bool DSGetObjShields(int n, float *shields);
bool DSGetObjPos(int n, vector *pos);
bool DSGetObjOrient(int n, matrix *m);
bool DSGetObjVelocity(int n, vector *vel);
bool DSGetObjRotVelocity(int n, vector *rotvel);
bool DSGetObjRoomNum(int n, int *roomnum);
bool DSGetObjType(int n, int *type);
bool DSGetObjId(int n, int *id);
bool DSGetNextObjIndex(int n, int *next);
bool DSGetPrevObjIndex(int n, int *prev);
bool DSSetControllerInfo(int n, game_controls *controls);
bool DSGetFirstRoomObject(int n, int *first);
bool DSPauseGame(void);
bool DSResumeGame(void);
bool DSGetObjAABB(int n, vector *min_xyz, vector *max_xyz);
bool DSSelectCurrentWeapon(int n);
bool DSGetCurrentPrimary(int *n);
bool DSGetCurrentSecondary(int *n);
bool DSGetAvailableWeapons(int *flags);
bool DSGetNumRoomPortals(int n, int *num_portals);
bool DSGetRoomPortalConnectRoom(int n, int p, int *connect_room);
bool DSIsObjDestroyable(int n, bool *f_is_destroyable);
bool DSIsDoorOpenable(int n, int *is_openable);
bool DSGetDoorStatus(int n, int *status);

// Not externalized to Soar
void DSSoarInit();
void DSSoarEnd();

#else // ifdef SOAR_ENABLED

#define DSSoarInit()                                                                                                   \
  do {                                                                                                                 \
  } while (0)
#define DSSoarEnd()                                                                                                    \
  do {                                                                                                                 \
  } while (0)

#endif // ifdef SOAR_ENABLED

#endif