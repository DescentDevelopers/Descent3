/*
 * $Logfile: /DescentIII/main/soar_helpers.cpp $
 * $Revision: 7 $
 * $Date: 4/16/99 12:33a $
 * $Author: Matt $
 *
 * Functions called by the Soar DLL
 *
 * $Log: /DescentIII/main/soar_helpers.cpp $
 *
 * 7     4/16/99 12:33a Matt
 * Disable Soar on non-Windows systems.
 *
 */

#include "soar.h"

#ifdef SOAR_ENABLED

#include "soar_helpers.h"

#include "object.h"
#include "vecmat.h"
#include "controls.h"
#include "weapon.h"
#include "room.h"
#include "terrain.h"
#include "player.h"
#include "gameloop.h"
#include "doorway.h"

bool DSGetHighestObjIndex(int &n) {
  n = Highest_object_index;
  return true;
}

bool DSGetObjSize(int n, float *size) {
  if (n < 0 || n > Highest_object_index || Objects[n].type == OBJ_NONE)
    return false;

  *size = Objects[n].size;
  return true;
}

bool DSGetObjMass(int n, float *mass) {
  if (n < 0 || n > Highest_object_index || Objects[n].type == OBJ_NONE)
    return false;

  *mass = Objects[n].mtype.phys_info.mass;
  return true;
}

bool DSGetObjShields(int n, float *shields) {
  if (n < 0 || n > Highest_object_index || Objects[n].type == OBJ_NONE)
    return false;

  *shields = Objects[n].shields;
  return true;
}

bool DSGetObjOrient(int n, matrix *m) {
  if (n < 0 || n > Highest_object_index || Objects[n].type == OBJ_NONE)
    return false;

  *m = Objects[n].orient;
  return true;
}

bool DSGetObjVelocity(int n, vector *vel) {
  if (n < 0 || n > Highest_object_index || Objects[n].type == OBJ_NONE)
    return false;

  *vel = Objects[n].mtype.phys_info.velocity;
  return true;
}

bool DSGetObjPos(int n, vector *pos) {
  if (n < 0 || n > Highest_object_index || Objects[n].type == OBJ_NONE)
    return false;

  *pos = Objects[n].pos;
  return true;
}

bool DSGetObjRoomNum(int n, int *roomnum) {
  if (n < 0 || n > Highest_object_index || Objects[n].type == OBJ_NONE)
    return false;

  *roomnum = Objects[n].roomnum;
  return true;
}

bool DSGetObjRotVelocity(int n, vector *rotvel) {
  if (n < 0 || n > Highest_object_index || Objects[n].type == OBJ_NONE)
    return false;

  *rotvel = Objects[n].mtype.phys_info.rotvel;
  return true;
}

bool DSGetObjType(int n, int *type) {
  if (n < 0 || n > Highest_object_index)
    return false;

  *type = Objects[n].type;
  return true;
}

bool DSGetObjId(int n, int *id) {
  if (n < 0 || n > Highest_object_index || Objects[n].type == OBJ_NONE)
    return false;

  *id = Objects[n].id;
  return true;
}

bool DSGetNextObjIndex(int n, int *next) {
  if (n < 0 || n > Highest_object_index || Objects[n].type == OBJ_NONE)
    return false;

  *next = Objects[n].next;
  return true;
}

bool DSGetPrevObjIndex(int n, int *prev) {
  if (n < 0 || n > Highest_object_index || Objects[n].type == OBJ_NONE)
    return false;

  *prev = Objects[n].next;
  return true;
}

bool DSSetControllerInfo(int n, game_controls *controls) {
  object *objp;

  if (n < 0 || n > Highest_object_index || Objects[n].type == OBJ_NONE || Objects[n].control_type != CT_SOAR)
    return false;

  objp = &Objects[n];

  // Update object's physics
  if (objp->movement_type == MT_PHYSICS) {
    objp->mtype.phys_info.rotthrust.x = controls->pitch_thrust * objp->mtype.phys_info.full_rotthrust;
    objp->mtype.phys_info.rotthrust.z = controls->bank_thrust * objp->mtype.phys_info.full_rotthrust;
    objp->mtype.phys_info.rotthrust.y = controls->heading_thrust * objp->mtype.phys_info.full_rotthrust;
    objp->mtype.phys_info.thrust = (objp->orient.fvec * controls->forward_thrust * objp->mtype.phys_info.full_thrust) +
                                   (objp->orient.uvec * controls->vertical_thrust * objp->mtype.phys_info.full_thrust) +
                                   (objp->orient.rvec * controls->sideways_thrust * objp->mtype.phys_info.full_thrust);
  }

  // Process weapon firing
  FireWeaponFromPlayer(objp, PW_PRIMARY, controls->fire_primary_down_count, controls->fire_primary_down_state,
                       controls->fire_primary_down_time);
  FireWeaponFromPlayer(objp, PW_SECONDARY, controls->fire_secondary_down_count, controls->fire_secondary_down_state,
                       controls->fire_secondary_down_time);

  // Deal with the flare
  for (int i = 0; i < controls->fire_flare_down_count; i++)
    FireFlareFromPlayer(objp);

  return true;
}

bool DSGetFirstRoomObject(int n, int *first) {
  if (!ROOMNUM_OUTSIDE(n)) {
    if (n < 0 || n > Highest_room_index || Rooms[n].used == 0)
      return false;

    *first = Rooms[n].objects;
  } else {
    int cell;

    cell = CELLNUM(n);
    if (cell < 0 && cell >= TERRAIN_WIDTH * TERRAIN_DEPTH)
      return false;

    *first = Terrain_seg[n].objects;
  }

  return true;
}

bool DSPauseGame(void) {
  PauseGame();

  return true;
}

bool DSResumeGame(void) {
  ResumeGame();

  return true;
}

bool DSGetObjAABB(int n, vector *min_xyz, vector *max_xyz) {
  if (n < 0 || n > Highest_object_index || Objects[n].type == OBJ_NONE)
    return false;

  *min_xyz = Objects[n].min_xyz;
  *max_xyz = Objects[n].max_xyz;

  return true;
}

// CHRISHACK --  THIS WEAPON STUFF SUCKS.  The current weapon slots are from 0 to 9.
// slots 0 - 4 are primaries  (mapped twice for a total of 10 primaries (weapons 0 - 9))
// Note: Weapon 0 and 5 use slot 0, weapon 1 and 6 use slot 1, etc.
// slots 5 - 9 are secondaries  (mapped twice for a total of 10 secondaries (weapons 10 - 19))
// if you currently are using the lower mapping, and you call the function with that slot
// number, you use the higher mapping.  If available at that slot, it selects that one
bool DSSelectCurrentWeapon(int n) {
  if (n < 0 || n > 9)
    return false;

  SelectWeapon(n);
  return true;
}

// These are from 0 to 9
bool DSGetCurrentPrimary(int *n) {
  *n = Players[Player_num].weapon[PW_PRIMARY].index;
  return true;
}

// These are from 10 to 19
bool DSGetCurrentSecondary(int *n) {
  *n = Players[Player_num].weapon[PW_SECONDARY].index;
  return true;
}

// Bit field using the first 20 bits.  If set, that weapon is available
bool DSGetAvailableWeapons(int *flags) {
  *flags = Players[Player_num].weapon_flags;

  return true;
}

bool DSGetNumRoomPortals(int n, int *num_portals) {
  if (n < 0 || n > Highest_room_index || Rooms[n].used == 0)
    return false;

  *num_portals = Rooms[n].num_portals;

  return true;
}

bool DSGetRoomPortalConnectRoom(int n, int p, int *connect_room) {
  if (n < 0 || n > Highest_room_index || Rooms[n].used == 0)
    return false;
  if (p < 0 || p >= Rooms[n].num_portals)
    return false;

  *connect_room = Rooms[n].portals[p].croom;

  return true;
}

bool DSIsObjDestroyable(int n, bool *f_is_destroyable) {
  if (n < 0 || n > Highest_object_index || Objects[n].type == OBJ_NONE)
    return false;

  *f_is_destroyable = (Objects[n].flags & OF_DESTROYABLE) != 0;

  return true;
}

bool DSIsDoorOpenable(int n, int *is_openable) {
  if (n < 0 || n > Highest_object_index || Objects[n].type != OBJ_DOOR)
    return false;
  *is_openable = !DoorwayLocked(Objects[n].handle);
  return true;
}

//	doorway state
//#define DOORWAY_CLOSED			0	// door is closed
//#define DOORWAY_OPENING			1	// door is opening
//#define DOORWAY_WAITING			2	// door is waiting to be closed
//#define DOORWAY_CLOSING			3	// door is closing
//#define DOORWAY_OPEN				4	// door is open, that's it.

bool DSGetDoorStatus(int n, int *status) {
  if (n < 0 || n > Highest_object_index || Objects[n].type != OBJ_DOOR)
    return false;
  *status = DoorwayState(Objects[n].handle);
  return true;
}

void DSSoarInit() {
  void *dsfunc[27];

  dsfunc[0] = (void *)DSGetHighestObjIndex;
  dsfunc[1] = (void *)DSGetObjSize;
  dsfunc[2] = (void *)DSGetObjMass;
  dsfunc[3] = (void *)DSGetObjShields;
  dsfunc[4] = (void *)DSGetObjPos;
  dsfunc[5] = (void *)DSGetObjOrient;
  dsfunc[6] = (void *)DSGetObjVelocity;
  dsfunc[7] = (void *)DSGetObjRotVelocity;
  dsfunc[8] = (void *)DSGetObjRoomNum;
  dsfunc[9] = (void *)DSGetObjType;
  dsfunc[10] = (void *)DSGetObjId;
  dsfunc[11] = (void *)DSGetNextObjIndex;
  dsfunc[12] = (void *)DSGetPrevObjIndex;
  dsfunc[13] = (void *)DSSetControllerInfo;
  dsfunc[14] = (void *)DSGetFirstRoomObject;
  dsfunc[15] = (void *)DSPauseGame;
  dsfunc[16] = (void *)DSResumeGame;
  dsfunc[17] = (void *)DSGetObjAABB;
  dsfunc[18] = (void *)DSSelectCurrentWeapon;
  dsfunc[19] = (void *)DSGetCurrentPrimary;
  dsfunc[20] = (void *)DSGetCurrentSecondary;
  dsfunc[21] = (void *)DSGetAvailableWeapons;
  dsfunc[22] = (void *)DSGetNumRoomPortals;
  dsfunc[23] = (void *)DSGetRoomPortalConnectRoom;
  dsfunc[24] = (void *)DSIsObjDestroyable;
  dsfunc[25] = (void *)DSIsDoorOpenable;
  dsfunc[26] = (void *)DSGetDoorStatus;

  SoarInit((void *)Terrain_seg, sizeof(Terrain_seg[0]), (void *)Rooms, sizeof(Rooms[0]), (void *)Objects,
           sizeof(Objects[0]), (void *)dsfunc);

  Soar_active = true;
}

void DSSoarEnd() {
  SoarEnd();
  Soar_active = false;
}

#endif // ifdef SOAR_ENABLED
