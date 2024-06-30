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

#include "pstypes.h"
#include "pserror.h"
#include "game.h"
#include "multi.h"
#include "multisafe.h"
#include "multi_server.h"
#include "object.h"
#include "player.h"
#include "demofile.h"

//------------------------
// MULTISAFE FUNCTIONS
//------------------------

#define GET_AND_VERIFY_OBJECT(o)                                                                                       \
  {                                                                                                                    \
    o = VerifyMSafeObject(MultiGetUshort(data, &count));                                                               \
    if (o == OBJECT_HANDLE_NONE)                                                                                       \
      return;                                                                                                          \
  }

// Makes sure the sever objnum passed in is actually real on our machine
// Also turns it into and object handle
int VerifyMSafeObject(int objnum) {
  int objectnum = Server_object_list[objnum];

  if (objectnum == 65535 || !(Objects[objectnum].flags & OF_SERVER_OBJECT)) {
    Int3(); // Get Jason, got a bad object with msafe
    return OBJECT_HANDLE_NONE;
  }

  return Objects[objectnum].handle;
}

// This is a bit ugly, but I didn't want to totally change the way the msafe calls work.
// this bool tells msafe if it's ok to process a call even though it's playing back a demo.
bool Demo_call_ok = false;
// Extracts the multisafe function to be executed on the client machine
void MultiDoMSafeFunction(uint8_t *data) {
  msafe_struct base_mstruct;
  msafe_struct *mstruct = &base_mstruct;
  int count = 0;

  SKIP_HEADER(data, &count);
  uint8_t type = MultiGetUbyte(data, &count);
  switch (type) {
  case MSAFE_WEATHER_RAIN:
    mstruct->state = MultiGetByte(data, &count);
    mstruct->scalar = MultiGetFloat(data, &count);
    break;
  case MSAFE_WEATHER_SNOW:
    mstruct->state = MultiGetByte(data, &count);
    mstruct->scalar = MultiGetFloat(data, &count);
    break;
  case MSAFE_WEATHER_LIGHTNING:
    mstruct->state = MultiGetByte(data, &count);
    mstruct->randval = MultiGetUbyte(data, &count);
    mstruct->scalar = MultiGetFloat(data, &count);
    break;
  case MSAFE_WEATHER_LIGHTNING_BOLT:
    mstruct->roomnum = MultiGetInt(data, &count);
    GET_AND_VERIFY_OBJECT(mstruct->objhandle);

    mstruct->pos.x = MultiGetFloat(data, &count);
    mstruct->pos.y = MultiGetFloat(data, &count);
    mstruct->pos.z = MultiGetFloat(data, &count);

    mstruct->lifetime = MultiGetFloat(data, &count);

    mstruct->pos2.x = MultiGetFloat(data, &count);
    mstruct->pos2.y = MultiGetFloat(data, &count);
    mstruct->pos2.z = MultiGetFloat(data, &count);

    char str[255];
    MultiGetString(str, data, &count);
    mstruct->texnum = FindTextureName(str);
    if (mstruct->texnum < 0)
      mstruct->texnum = 0;

    mstruct->color = MultiGetUshort(data, &count);
    mstruct->size = MultiGetByte(data, &count);
    mstruct->state = MultiGetByte(data, &count);
    mstruct->count = MultiGetByte(data, &count);
    mstruct->interval = MultiGetFloat(data, &count);
    mstruct->index = MultiGetByte(data, &count);
    mstruct->flags = MultiGetByte(data, &count);

    if (mstruct->flags) {
      mstruct->g1 = MultiGetUshort(data, &count);
      mstruct->g2 = MultiGetUshort(data, &count);
    } else {
      GET_AND_VERIFY_OBJECT(mstruct->ithandle);
    }
    break;
  case MSAFE_ROOM_TEXTURE:
    mstruct->roomnum = MultiGetShort(data, &count);
    mstruct->facenum = MultiGetShort(data, &count);
    MultiGetString(mstruct->name, data, &count);
    mstruct->index = FindTextureName(mstruct->name);
    if (mstruct->index == -1)
      mstruct->index = 0;
    break;
  case MSAFE_ROOM_WIND:
    mstruct->roomnum = MultiGetShort(data, &count);
    mstruct->wind.x = MultiGetFloat(data, &count);
    mstruct->wind.y = MultiGetFloat(data, &count);
    mstruct->wind.z = MultiGetFloat(data, &count);
    break;
  case MSAFE_ROOM_FOG:
    mstruct->roomnum = MultiGetShort(data, &count);
    mstruct->fog_r = (float)MultiGetUbyte(data, &count) / 255.0;
    mstruct->fog_g = (float)MultiGetUbyte(data, &count) / 255.0;
    mstruct->fog_b = (float)MultiGetUbyte(data, &count) / 255.0;
    mstruct->fog_depth = MultiGetFloat(data, &count);
    break;
  case MSAFE_ROOM_FOG_STATE:
    mstruct->roomnum = MultiGetShort(data, &count);
    mstruct->state = MultiGetUbyte(data, &count);
    break;
  case MSAFE_ROOM_CHANGING_FOG:
    mstruct->roomnum = MultiGetShort(data, &count);
    mstruct->fog_r = (float)MultiGetUbyte(data, &count) / 255.0;
    mstruct->fog_g = (float)MultiGetUbyte(data, &count) / 255.0;
    mstruct->fog_b = (float)MultiGetUbyte(data, &count) / 255.0;
    mstruct->fog_depth = MultiGetFloat(data, &count);
    mstruct->interval = MultiGetFloat(data, &count);
    break;
  case MSAFE_ROOM_CHANGING_WIND:
    mstruct->roomnum = MultiGetShort(data, &count);
    mstruct->wind.x = MultiGetFloat(data, &count);
    mstruct->wind.y = MultiGetFloat(data, &count);
    mstruct->wind.z = MultiGetFloat(data, &count);
    mstruct->interval = MultiGetFloat(data, &count);
    break;
  case MSAFE_ROOM_LIGHT_PULSE:
    mstruct->roomnum = MultiGetShort(data, &count);
    mstruct->pulse_time = MultiGetUbyte(data, &count);
    mstruct->pulse_offset = MultiGetUbyte(data, &count);
    break;
  case MSAFE_ROOM_LIGHT_STROBE:
  case MSAFE_ROOM_LIGHT_FLICKER:
  case MSAFE_ROOM_REFUEL:
    mstruct->roomnum = MultiGetShort(data, &count);
    mstruct->state = MultiGetUbyte(data, &count);
    break;
  case MSAFE_ROOM_DAMAGE:
    mstruct->roomnum = MultiGetShort(data, &count);
    mstruct->amount = MultiGetFloat(data, &count);
    mstruct->index = MultiGetUbyte(data, &count);
    break;
  case MSAFE_ROOM_PORTAL_RENDER:
    mstruct->roomnum = MultiGetShort(data, &count);
    mstruct->portalnum = MultiGetShort(data, &count);
    mstruct->state = MultiGetUbyte(data, &count);
    mstruct->flags = MultiGetUbyte(data, &count);
    break;
  case MSAFE_ROOM_PORTAL_BLOCK:
    mstruct->roomnum = MultiGetShort(data, &count);
    mstruct->portalnum = MultiGetShort(data, &count);
    mstruct->state = MultiGetUbyte(data, &count);
    mstruct->flags = MultiGetUbyte(data, &count);
    break;
  case MSAFE_OBJECT_SHAKE_AREA:
    GET_AND_VERIFY_OBJECT(mstruct->objhandle);
    mstruct->scalar = MultiGetFloat(data, &count);
    break;
  case MSAFE_OBJECT_SETONFIRE:
    GET_AND_VERIFY_OBJECT(mstruct->objhandle);
    if (MultiGetByte(data, &count)) {
      GET_AND_VERIFY_OBJECT(mstruct->ithandle);
    } else {
      mstruct->ithandle = OBJECT_HANDLE_NONE;
    }
    mstruct->longevity = MultiGetFloat(data, &count);
    mstruct->interval = MultiGetFloat(data, &count);
    break;
  case MSAFE_OBJECT_SHIELDS:
    GET_AND_VERIFY_OBJECT(mstruct->objhandle);
    mstruct->shields = MultiGetShort(data, &count);
    break;
  case MSAFE_OBJECT_ENERGY:
    GET_AND_VERIFY_OBJECT(mstruct->objhandle);
    mstruct->energy = MultiGetShort(data, &count);
    break;
    //		case MSAFE_OBJECT_ANIMATE:
    //			mstruct->objhandle=VerifyMSafeObject(MultiGetUshort (data,&count));
    //			mstruct->start_tick=MultiGetShort (data,&count);
    //			mstruct->end_tick=MultiGetShort (data,&count);
    //			mstruct->cycle_time=MultiGetFloat (data,&count);
    //			mstruct->flags=MultiGetShort (data,&count);
    //			break;
  case MSAFE_OBJECT_MOVEMENT_SCALAR:
  case MSAFE_OBJECT_RECHARGE_SCALAR:
  case MSAFE_OBJECT_WSPEED_SCALAR:
  case MSAFE_OBJECT_ARMOR_SCALAR:
  case MSAFE_OBJECT_DAMAGE_SCALAR:
    GET_AND_VERIFY_OBJECT(mstruct->objhandle);
    mstruct->scalar = MultiGetFloat(data, &count);
    break;
  case MSAFE_SHOW_ENABLED_CONTROLS:
    GET_AND_VERIFY_OBJECT(mstruct->objhandle);
    mstruct->state = MultiGetByte(data, &count);
    break;
  case MSAFE_OBJECT_PLAYER_CMASK:
    GET_AND_VERIFY_OBJECT(mstruct->objhandle);
    mstruct->control_mask = MultiGetUint(data, &count);
    mstruct->control_val = MultiGetByte(data, &count);
    break;
  case MSAFE_OBJECT_ADD_WEAPON:
    GET_AND_VERIFY_OBJECT(mstruct->objhandle);
    mstruct->index = MultiGetByte(data, &count);
    mstruct->ammo = MultiGetFloat(data, &count);
    break;
  case MSAFE_OBJECT_START_SPEW:
    mstruct->id = MultiGetUbyte(data, &count);
    GET_AND_VERIFY_OBJECT(mstruct->objhandle);
    mstruct->random = MultiGetByte(data, &count);
    mstruct->is_real = MultiGetByte(data, &count);
    mstruct->gunpoint = MultiGetByte(data, &count);
    mstruct->effect_type = MultiGetByte(data, &count);
    mstruct->phys_info = MultiGetByte(data, &count);
    mstruct->drag = MultiGetFloat(data, &count);
    mstruct->mass = MultiGetFloat(data, &count);
    mstruct->interval = MultiGetFloat(data, &count);
    mstruct->longevity = MultiGetFloat(data, &count);
    mstruct->lifetime = MultiGetFloat(data, &count);
    mstruct->size = MultiGetFloat(data, &count);
    mstruct->speed = MultiGetFloat(data, &count);
    mprintf(0, "Got a START SPEW.  Index=%d\n", mstruct->id);
    break;
  case MSAFE_OBJECT_STOP_SPEW:
    mstruct->id = MultiGetUbyte(data, &count);
    mprintf(0, "Got STOP SPEW. index=%d\n", mstruct->id);
    mstruct->id = Server_spew_list[mstruct->id];
    ASSERT(mstruct->id != 65535);
    break;
  case MSAFE_OBJECT_NO_RENDER:
    GET_AND_VERIFY_OBJECT(mstruct->objhandle);
    break;
  case MSAFE_OBJECT_DEFORM:
    GET_AND_VERIFY_OBJECT(mstruct->objhandle);
    mstruct->amount = MultiGetFloat(data, &count);
    mstruct->lifetime = MultiGetFloat(data, &count);
    break;
  case MSAFE_OBJECT_VIEWER_SHAKE:
    mstruct->amount = MultiGetFloat(data, &count);
    break;
  case MSAFE_OBJECT_GHOST:
  case MSAFE_OBJECT_UNGHOST:
    GET_AND_VERIFY_OBJECT(mstruct->objhandle);
    break;
  case MSAFE_OBJECT_INVULNERABLE:
  case MSAFE_OBJECT_CLOAK:
    GET_AND_VERIFY_OBJECT(mstruct->objhandle);
    mstruct->state = MultiGetByte(data, &count);
    mstruct->lifetime = MultiGetFloat(data, &count);
    break;
  case MSAFE_OBJECT_CLOAKALLPLAYERS:
    mstruct->state = MultiGetByte(data, &count);
    mstruct->lifetime = MultiGetFloat(data, &count);
    break;
  case MSAFE_OBJECT_LIGHT_DIST:
    GET_AND_VERIFY_OBJECT(mstruct->objhandle);
    mstruct->light_distance = MultiGetFloat(data, &count);
    break;
  case MSAFE_OBJECT_LIGHT_COLOR:
    GET_AND_VERIFY_OBJECT(mstruct->objhandle);
    mstruct->r1 = MultiGetFloat(data, &count);
    mstruct->g1 = MultiGetFloat(data, &count);
    mstruct->b1 = MultiGetFloat(data, &count);
    break;
  case MSAFE_OBJECT_REMOVE:
    GET_AND_VERIFY_OBJECT(mstruct->objhandle);
    mstruct->playsound = MultiGetByte(data, &count);
    break;
  case MSAFE_OBJECT_PLAYER_CONTROLAI:
    mstruct->slot = MultiGetByte(data, &count);
    mstruct->state = MultiGetByte(data, &count);
    break;

  case MSAFE_OBJECT_WORLD_POSITION:
    GET_AND_VERIFY_OBJECT(mstruct->objhandle);
    mstruct->roomnum = MultiGetInt(data, &count);
    mstruct->pos.x = MultiGetFloat(data, &count);
    mstruct->pos.y = MultiGetFloat(data, &count);
    mstruct->pos.z = MultiGetFloat(data, &count);
    mstruct->orient.fvec.x = MultiGetFloat(data, &count);
    mstruct->orient.fvec.y = MultiGetFloat(data, &count);
    mstruct->orient.fvec.z = MultiGetFloat(data, &count);
    mstruct->orient.rvec.x = MultiGetFloat(data, &count);
    mstruct->orient.rvec.y = MultiGetFloat(data, &count);
    mstruct->orient.rvec.z = MultiGetFloat(data, &count);
    mstruct->orient.uvec.x = MultiGetFloat(data, &count);
    mstruct->orient.uvec.y = MultiGetFloat(data, &count);
    mstruct->orient.uvec.z = MultiGetFloat(data, &count);
    break;
  case MSAFE_OBJECT_SPARKS:
    GET_AND_VERIFY_OBJECT(mstruct->objhandle);
    mstruct->amount = MultiGetFloat(data, &count);
    mstruct->lifetime = MultiGetFloat(data, &count);
    break;
  case MSAFE_SOUND_STREAMING: {
    mstruct->state = MultiGetByte(data, &count);
    int objnum = MultiGetUshort(data, &count);
    if (mstruct->state)
      mstruct->objhandle = VerifyMSafeObject(objnum);

    mstruct->volume = MultiGetFloat(data, &count);
    MultiGetString(mstruct->name, data, &count);
    break;
  }
  case MSAFE_SOUND_STOP_OBJ:
    GET_AND_VERIFY_OBJECT(mstruct->objhandle);

    break;
  case MSAFE_SOUND_VOLUME_OBJ:
    GET_AND_VERIFY_OBJECT(mstruct->objhandle);
    mstruct->volume = MultiGetFloat(data, &count);
    break;
  case MSAFE_MUSIC_REGION: {
    mstruct->state = MultiGetByte(data, &count);
    int objnum = MultiGetUshort(data, &count);
    if (mstruct->state)
      mstruct->objhandle = VerifyMSafeObject(objnum);
    mstruct->index = MultiGetInt(data, &count);
    break;
  }
  case MSAFE_SOUND_2D: {
    mstruct->state = MultiGetByte(data, &count);
    int objnum = MultiGetUshort(data, &count);
    if (mstruct->state)
      mstruct->objhandle = VerifyMSafeObject(objnum);
    mstruct->volume = MultiGetFloat(data, &count);
    mstruct->index = MultiGetInt(data, &count);
    break;
  }
  case MSAFE_SOUND_OBJECT:
    GET_AND_VERIFY_OBJECT(mstruct->objhandle);
    mstruct->index = MultiGetInt(data, &count);
    break;
  case MSAFE_MISC_LEVELGOAL:
    MultiGetString(mstruct->message, data, &count);
    mstruct->index = MultiGetInt(data, &count);
    mstruct->type = MultiGetInt(data, &count);
    mstruct->count = MultiGetInt(data, &count);
    break;

  case MSAFE_MISC_ENABLE_SHIP:
    mstruct->state = (bool)(MultiGetByte(data, &count) != 0);
    MultiGetString(mstruct->name, data, &count);
    break;
  case MSAFE_MISC_UPDATE_HUD_ITEM:
    mstruct->color = MultiGetInt(data, &count);
    MultiGetString(mstruct->message, data, &count);
    break;
  case MSAFE_MISC_FILTERED_HUD_MESSAGE:
  case MSAFE_MISC_HUD_MESSAGE: {
    mstruct->state = MultiGetByte(data, &count);
    int objnum = MultiGetUshort(data, &count);
    if (mstruct->state)
      mstruct->objhandle = VerifyMSafeObject(objnum);
    mstruct->color = MultiGetInt(data, &count);
    MultiGetString(mstruct->message, data, &count);
    break;
  }

  case MSAFE_MISC_GAME_MESSAGE: {
    mstruct->state = MultiGetByte(data, &count);
    int objnum = MultiGetUshort(data, &count);
    if (mstruct->state)
      mstruct->objhandle = VerifyMSafeObject(objnum);
    mstruct->color = MultiGetInt(data, &count);
    MultiGetString(mstruct->message, data, &count);
    MultiGetString(mstruct->message2, data, &count);
    break;
  }

  case MSAFE_MISC_WAYPOINT:
    mstruct->index = MultiGetByte(data, &count);
    break;
  case MSAFE_MISC_POPUP_CAMERA:
    GET_AND_VERIFY_OBJECT(mstruct->objhandle);
    mstruct->interval = MultiGetFloat(data, &count);
    mstruct->scalar = MultiGetFloat(data, &count);
    mstruct->gunpoint = MultiGetByte(data, &count);
    break;
  case MSAFE_MISC_CLOSE_POPUP:
    break;
  case MSAFE_MISC_GUIDEBOT_NAME:
    GET_AND_VERIFY_OBJECT(mstruct->objhandle);
    MultiGetString(mstruct->name, data, &count);
    break;
  case MSAFE_DOOR_LOCK_STATE:
    mstruct->state = MultiGetUbyte(data, &count);
    GET_AND_VERIFY_OBJECT(mstruct->objhandle);
    break;
  case MSAFE_DOOR_ACTIVATE:
  case MSAFE_DOOR_STOP:
    GET_AND_VERIFY_OBJECT(mstruct->objhandle);
    break;
  case MSAFE_DOOR_POSITION:
    GET_AND_VERIFY_OBJECT(mstruct->objhandle);
    mstruct->scalar = MultiGetFloat(data, &count);
    break;
  case MSAFE_INVEN_ADD_OBJECT:
    mstruct->flags = MultiGetInt(data, &count);
    MultiGetString(mstruct->message, data, &count);
    // note the fall through
  case MSAFE_INVEN_REMOVE_OBJECT:
    GET_AND_VERIFY_OBJECT(mstruct->objhandle);
    GET_AND_VERIFY_OBJECT(mstruct->ithandle);
    break;
  case MSAFE_INVEN_ADD_TYPE_ID:
    GET_AND_VERIFY_OBJECT(mstruct->objhandle);
    MultiGetTypeID(data, &count, &mstruct->type, &mstruct->id);
    mstruct->flags = MultiGetInt(data, &count);
    break;
  case MSAFE_INVEN_REMOVE:
  case MSAFE_COUNTERMEASURE_REMOVE:
    GET_AND_VERIFY_OBJECT(mstruct->objhandle);
    MultiGetTypeID(data, &count, &mstruct->type, &mstruct->id);
    break;
  case MSAFE_COUNTERMEASURE_ADD:
    mstruct->slot = MultiGetByte(data, &count);
    MultiGetString(mstruct->name, data, &count);
    mstruct->count = MultiGetByte(data, &count);
    MultiGetTypeID(data, &count, &mstruct->type, &mstruct->id);
    break;

  case MSAFE_OBJECT_ROTDRAG:
    GET_AND_VERIFY_OBJECT(mstruct->objhandle);
    mstruct->rot_drag = MultiGetFloat(data, &count);
    break;
  case MSAFE_OBJECT_TYPE:
    GET_AND_VERIFY_OBJECT(mstruct->objhandle);
    mstruct->type = MultiGetByte(data, &count);
    break;
  case MSAFE_OBJECT_ID:
    GET_AND_VERIFY_OBJECT(mstruct->objhandle);
    mstruct->id = MultiGetUshort(data, &count);
    break;
  case MSAFE_OBJECT_CONTROL_TYPE:
    GET_AND_VERIFY_OBJECT(mstruct->objhandle);
    mstruct->control_type = MultiGetByte(data, &count);
    break;
  case MSAFE_OBJECT_FLAGS:
    GET_AND_VERIFY_OBJECT(mstruct->objhandle);
    mstruct->flags = MultiGetInt(data, &count);
    break;
  case MSAFE_OBJECT_MOVEMENT_TYPE:
    GET_AND_VERIFY_OBJECT(mstruct->objhandle);
    mstruct->movement_type = MultiGetByte(data, &count);
    break;
  case MSAFE_OBJECT_CREATION_TIME:
    GET_AND_VERIFY_OBJECT(mstruct->objhandle);
    mstruct->creation_time = MultiGetFloat(data, &count);
    break;
  case MSAFE_OBJECT_PHYSICS_FLAGS:
    GET_AND_VERIFY_OBJECT(mstruct->objhandle);
    mstruct->physics_flags = MultiGetInt(data, &count);
    break;
  case MSAFE_OBJECT_PARENT:
    GET_AND_VERIFY_OBJECT(mstruct->objhandle);
    mstruct->ithandle = MultiGetUshort(data, &count);

    if (mstruct->ithandle == HANDLE_OBJNUM_MASK)
      mstruct->ithandle = OBJECT_HANDLE_NONE;
    else
      mstruct->ithandle = VerifyMSafeObject(mstruct->ithandle);
    break;
  case MSAFE_WEAPON_ADD:
    GET_AND_VERIFY_OBJECT(mstruct->objhandle);
    mstruct->index = MultiGetInt(data, &count);
    mstruct->state = (bool)(MultiGetByte(data, &count) != 0);
    mstruct->count = MultiGetInt(data, &count);
    break;
  default:
    Int3(); // Illegal type passed to multisafe function
    break;
  }
  if (Demo_flags == DF_PLAYBACK) {
    // Yes this is ugly, but it's better than the other alternatives at the moment
    //(changing the prototype of the exported function and breaking all the dlls)
    Demo_call_ok = true;
    msafe_CallFunction(type, mstruct);
    Demo_call_ok = false;
  } else {
    msafe_CallFunction(type, mstruct);
  }
}

void MultiSendMSafeFunction(uint8_t type, msafe_struct *mstruct) {
  int sequence = -1;
  int to_slot = -1;
  uint32_t to_bitmask = 0xFFFFFFFF;

  ASSERT((Demo_flags == DF_RECORDING) || (Netgame.local_role == LR_SERVER));

  uint8_t data[MAX_GAME_DATA_SIZE];
  int count = 0;
  int size_offset;

  size_offset = START_DATA(MP_MSAFE_FUNCTION, data, &count);

  MultiAddUbyte(type, data, &count);

  switch (type) {
  case MSAFE_WEATHER_RAIN:
    sequence = NETSEQ_WORLD;
    MultiAddByte(mstruct->state, data, &count);
    MultiAddFloat(mstruct->scalar, data, &count);
    break;
  case MSAFE_WEATHER_SNOW:
    sequence = NETSEQ_WORLD;
    MultiAddByte(mstruct->state, data, &count);
    MultiAddFloat(mstruct->scalar, data, &count);
    break;
  case MSAFE_WEATHER_LIGHTNING:
    sequence = NETSEQ_WORLD;
    MultiAddByte(mstruct->state, data, &count);
    MultiAddUbyte(mstruct->randval, data, &count);
    MultiAddFloat(mstruct->scalar, data, &count);
    break;
  case MSAFE_WEATHER_LIGHTNING_BOLT:
    sequence = NETSEQ_WORLD;
    MultiAddInt(mstruct->roomnum, data, &count);
    MultiAddUshort(mstruct->objhandle & HANDLE_OBJNUM_MASK, data, &count);

    MultiAddFloat(mstruct->pos.x, data, &count);
    MultiAddFloat(mstruct->pos.y, data, &count);
    MultiAddFloat(mstruct->pos.z, data, &count);

    MultiAddFloat(mstruct->lifetime, data, &count);

    MultiAddFloat(mstruct->pos2.x, data, &count);
    MultiAddFloat(mstruct->pos2.y, data, &count);
    MultiAddFloat(mstruct->pos2.z, data, &count);

    MultiAddString(GameTextures[mstruct->texnum].name, data, &count);
    MultiAddUshort(mstruct->color, data, &count);
    MultiAddByte(mstruct->size, data, &count);
    MultiAddByte(mstruct->state, data, &count);
    MultiAddByte(mstruct->count, data, &count);
    MultiAddFloat(mstruct->interval, data, &count);
    MultiAddByte(mstruct->index, data, &count);

    if (mstruct->flags) // Do attached lightning effect
    {
      MultiAddByte(1, data, &count);

      MultiAddUshort(mstruct->g1, data, &count);
      MultiAddUshort(mstruct->g2, data, &count);
    } else {
      MultiAddByte(0, data, &count);
      MultiAddUshort(mstruct->ithandle & HANDLE_OBJNUM_MASK, data, &count);
    }

    break;
  case MSAFE_ROOM_TEXTURE:
    sequence = NETSEQ_WORLD;
    MultiAddShort(mstruct->roomnum, data, &count);
    MultiAddShort(mstruct->facenum, data, &count);
    MultiAddString(GameTextures[mstruct->index].name, data, &count);
    break;
  case MSAFE_ROOM_WIND:
    sequence = NETSEQ_WORLD;
    MultiAddShort(mstruct->roomnum, data, &count);
    MultiAddFloat(mstruct->wind.x, data, &count);
    MultiAddFloat(mstruct->wind.y, data, &count);
    MultiAddFloat(mstruct->wind.z, data, &count);
    break;
  case MSAFE_ROOM_CHANGING_WIND:
    sequence = NETSEQ_WORLD;
    MultiAddShort(mstruct->roomnum, data, &count);
    MultiAddFloat(mstruct->wind.x, data, &count);
    MultiAddFloat(mstruct->wind.y, data, &count);
    MultiAddFloat(mstruct->wind.z, data, &count);
    MultiAddFloat(mstruct->interval, data, &count);
    break;
  case MSAFE_ROOM_FOG:
    sequence = NETSEQ_WORLD;
    MultiAddShort(mstruct->roomnum, data, &count);
    MultiAddUbyte(mstruct->fog_r * 255.0, data, &count);
    MultiAddUbyte(mstruct->fog_g * 255.0, data, &count);
    MultiAddUbyte(mstruct->fog_b * 255.0, data, &count);
    MultiAddFloat(mstruct->fog_depth, data, &count);
    break;
  case MSAFE_ROOM_FOG_STATE:
    sequence = NETSEQ_WORLD;
    MultiAddShort(mstruct->roomnum, data, &count);
    MultiAddUbyte(mstruct->state, data, &count);
    break;
  case MSAFE_ROOM_CHANGING_FOG:
    sequence = NETSEQ_WORLD;
    MultiAddShort(mstruct->roomnum, data, &count);
    MultiAddUbyte(mstruct->fog_r * 255.0, data, &count);
    MultiAddUbyte(mstruct->fog_g * 255.0, data, &count);
    MultiAddUbyte(mstruct->fog_b * 255.0, data, &count);
    MultiAddFloat(mstruct->fog_depth, data, &count);
    MultiAddFloat(mstruct->interval, data, &count);
    break;
  case MSAFE_ROOM_LIGHT_PULSE:
    sequence = NETSEQ_WORLD;
    MultiAddShort(mstruct->roomnum, data, &count);
    MultiAddUbyte(mstruct->pulse_time, data, &count);
    MultiAddUbyte(mstruct->pulse_offset, data, &count);
    break;
  case MSAFE_ROOM_LIGHT_STROBE:
  case MSAFE_ROOM_LIGHT_FLICKER:
  case MSAFE_ROOM_REFUEL:
    sequence = NETSEQ_WORLD;
    MultiAddShort(mstruct->roomnum, data, &count);
    MultiAddUbyte(mstruct->state, data, &count);
    break;
  case MSAFE_ROOM_DAMAGE:
    sequence = NETSEQ_WORLD;
    MultiAddShort(mstruct->roomnum, data, &count);
    MultiAddFloat(mstruct->amount, data, &count);
    MultiAddUbyte(mstruct->index, data, &count);
    break;
  case MSAFE_ROOM_PORTAL_RENDER:
    sequence = NETSEQ_WORLD;
    MultiAddShort(mstruct->roomnum, data, &count);
    MultiAddShort(mstruct->portalnum, data, &count);
    MultiAddUbyte(mstruct->state, data, &count);

    if (mstruct->flags)
      MultiAddUbyte(1, data, &count);
    else
      MultiAddUbyte(0, data, &count);

    break;
  case MSAFE_ROOM_PORTAL_BLOCK:
    sequence = NETSEQ_WORLD;
    MultiAddShort(mstruct->roomnum, data, &count);
    MultiAddShort(mstruct->portalnum, data, &count);
    MultiAddUbyte(mstruct->state, data, &count);

    if (mstruct->flags)
      MultiAddUbyte(1, data, &count);
    else
      MultiAddUbyte(0, data, &count);

    break;

  case MSAFE_OBJECT_SHAKE_AREA: {
    sequence = NETSEQ_PLAYING;
    MultiAddUshort(mstruct->objhandle & HANDLE_OBJNUM_MASK, data, &count);
    MultiAddFloat(mstruct->scalar, data, &count);

    to_bitmask = 0;

    object *obj = ObjGet(mstruct->objhandle);

    for (int i = 1; i < MAX_PLAYERS; i++) {
      if (!(NetPlayers[i].flags & NPF_CONNECTED))
        continue;

      float dist = vm_VectorDistanceQuick(&Objects[Players[i].objnum].pos, &obj->pos);
      if (dist < mstruct->scalar)
        to_bitmask |= (1 << i);
    }
    break;
  }
  case MSAFE_OBJECT_SETONFIRE:
    sequence = NETSEQ_OBJECTS;
    MultiAddUshort(mstruct->objhandle & HANDLE_OBJNUM_MASK, data, &count);
    if (mstruct->ithandle == OBJECT_HANDLE_NONE) {
      MultiAddByte(0, data, &count);
    } else {
      MultiAddByte(1, data, &count);
      MultiAddUshort(mstruct->ithandle & HANDLE_OBJNUM_MASK, data, &count);
    }
    MultiAddFloat(mstruct->longevity, data, &count);
    MultiAddFloat(mstruct->interval, data, &count);
    break;

  case MSAFE_OBJECT_SHIELDS:
    if (Objects[mstruct->objhandle & HANDLE_OBJNUM_MASK].type == OBJ_PLAYER)
      sequence = NETSEQ_PLAYERS;
    else
      sequence = NETSEQ_OBJECTS;

    MultiAddUshort(mstruct->objhandle & HANDLE_OBJNUM_MASK, data, &count);
    MultiAddShort(mstruct->shields, data, &count);
    break;

  case MSAFE_OBJECT_ROTDRAG:
    if (Objects[mstruct->objhandle & HANDLE_OBJNUM_MASK].type == OBJ_PLAYER)
      sequence = NETSEQ_PLAYERS;
    else
      sequence = NETSEQ_OBJECTS;

    MultiAddUshort(mstruct->objhandle & HANDLE_OBJNUM_MASK, data, &count);
    MultiAddFloat(mstruct->rot_drag, data, &count);
    break;

  case MSAFE_OBJECT_TYPE:
    if (Objects[mstruct->objhandle & HANDLE_OBJNUM_MASK].type == OBJ_PLAYER)
      sequence = NETSEQ_PLAYERS;
    else
      sequence = NETSEQ_OBJECTS;

    MultiAddUshort(mstruct->objhandle & HANDLE_OBJNUM_MASK, data, &count);
    MultiAddByte(mstruct->type, data, &count);
    break;

  case MSAFE_OBJECT_ID:
    if (Objects[mstruct->objhandle & HANDLE_OBJNUM_MASK].type == OBJ_PLAYER)
      sequence = NETSEQ_PLAYERS;
    else
      sequence = NETSEQ_OBJECTS;

    MultiAddUshort(mstruct->objhandle & HANDLE_OBJNUM_MASK, data, &count);
    MultiAddUshort(mstruct->id, data, &count);
    break;

  case MSAFE_OBJECT_CONTROL_TYPE:
    if (Objects[mstruct->objhandle & HANDLE_OBJNUM_MASK].type == OBJ_PLAYER)
      sequence = NETSEQ_PLAYERS;
    else
      sequence = NETSEQ_OBJECTS;

    MultiAddUshort(mstruct->objhandle & HANDLE_OBJNUM_MASK, data, &count);
    MultiAddByte(mstruct->control_type, data, &count);
    break;

  case MSAFE_OBJECT_FLAGS:
    if (Objects[mstruct->objhandle & HANDLE_OBJNUM_MASK].type == OBJ_PLAYER)
      sequence = NETSEQ_PLAYERS;
    else
      sequence = NETSEQ_OBJECTS;

    MultiAddUshort(mstruct->objhandle & HANDLE_OBJNUM_MASK, data, &count);
    MultiAddInt(mstruct->flags, data, &count);
    break;

  case MSAFE_OBJECT_MOVEMENT_TYPE:
    if (Objects[mstruct->objhandle & HANDLE_OBJNUM_MASK].type == OBJ_PLAYER)
      sequence = NETSEQ_PLAYERS;
    else
      sequence = NETSEQ_OBJECTS;

    MultiAddUshort(mstruct->objhandle & HANDLE_OBJNUM_MASK, data, &count);
    MultiAddByte(mstruct->movement_type, data, &count);
    break;

  case MSAFE_OBJECT_CREATION_TIME:
    if (Objects[mstruct->objhandle & HANDLE_OBJNUM_MASK].type == OBJ_PLAYER)
      sequence = NETSEQ_PLAYERS;
    else
      sequence = NETSEQ_OBJECTS;

    MultiAddUshort(mstruct->objhandle & HANDLE_OBJNUM_MASK, data, &count);
    MultiAddFloat(mstruct->creation_time, data, &count);
    break;

  case MSAFE_OBJECT_PHYSICS_FLAGS:
    if (Objects[mstruct->objhandle & HANDLE_OBJNUM_MASK].type == OBJ_PLAYER)
      sequence = NETSEQ_PLAYERS;
    else
      sequence = NETSEQ_OBJECTS;

    MultiAddUshort(mstruct->objhandle & HANDLE_OBJNUM_MASK, data, &count);
    MultiAddInt(mstruct->physics_flags, data, &count);
    break;

  case MSAFE_OBJECT_PARENT:
    if (Objects[mstruct->objhandle & HANDLE_OBJNUM_MASK].type == OBJ_PLAYER)
      sequence = NETSEQ_PLAYERS;
    else
      sequence = NETSEQ_OBJECTS;

    MultiAddUshort(mstruct->objhandle & HANDLE_OBJNUM_MASK, data, &count);
    if (mstruct->ithandle != OBJECT_HANDLE_NONE && mstruct->ithandle != OBJECT_HANDLE_BAD)
      MultiAddUshort(mstruct->ithandle & HANDLE_OBJNUM_MASK, data, &count);
    else
      MultiAddUshort(HANDLE_OBJNUM_MASK, data, &count);
    break;

  case MSAFE_OBJECT_ENERGY:
    sequence = NETSEQ_PLAYERS;
    MultiAddUshort(mstruct->objhandle & HANDLE_OBJNUM_MASK, data, &count);
    MultiAddShort(mstruct->energy, data, &count);
    break;
    //		case MSAFE_OBJECT_ANIMATE:
    //			sequence=NETSEQ_OBJECTS;
    //			MultiAddUshort (mstruct->objhandle & HANDLE_OBJNUM_MASK,data,&count);
    //			MultiAddShort (mstruct->start_tick,data,&count);
    //			MultiAddShort (mstruct->end_tick,data,&count);
    //			MultiAddFloat (mstruct->cycle_time,data,&count);
    //			MultiAddShort (mstruct->flags,data,&count);
    //			break;
  case MSAFE_OBJECT_MOVEMENT_SCALAR:
  case MSAFE_OBJECT_RECHARGE_SCALAR:
  case MSAFE_OBJECT_WSPEED_SCALAR:
  case MSAFE_OBJECT_ARMOR_SCALAR:
  case MSAFE_OBJECT_DAMAGE_SCALAR:
    sequence = NETSEQ_PLAYERS;
    MultiAddUshort(mstruct->objhandle & HANDLE_OBJNUM_MASK, data, &count);
    MultiAddFloat(mstruct->scalar, data, &count);
    break;
  case MSAFE_SHOW_ENABLED_CONTROLS:
    sequence = NETSEQ_PLAYERS;
    MultiAddUshort(mstruct->objhandle & HANDLE_OBJNUM_MASK, data, &count);
    MultiAddByte(mstruct->state, data, &count);
    break;
  case MSAFE_OBJECT_PLAYER_CMASK:
    sequence = NETSEQ_PLAYERS;
    MultiAddUshort(mstruct->objhandle & HANDLE_OBJNUM_MASK, data, &count);
    MultiAddUint(mstruct->control_mask, data, &count);
    MultiAddByte(mstruct->control_val, data, &count);
    break;
  case MSAFE_OBJECT_ADD_WEAPON:
    sequence = NETSEQ_PLAYERS;
    MultiAddUshort(mstruct->objhandle & HANDLE_OBJNUM_MASK, data, &count);
    MultiAddByte(mstruct->index, data, &count);
    MultiAddFloat(mstruct->ammo, data, &count);

    to_slot = Objects[mstruct->objhandle & HANDLE_OBJNUM_MASK].id;
    break;
  case MSAFE_OBJECT_START_SPEW:
    sequence = NETSEQ_WORLD;
    MultiAddUbyte((mstruct->id & 0xFF), data, &count);
    MultiAddUshort(mstruct->objhandle & HANDLE_OBJNUM_MASK, data, &count);
    MultiAddByte(mstruct->random, data, &count);
    MultiAddByte(mstruct->is_real, data, &count);
    MultiAddByte(mstruct->gunpoint, data, &count);
    MultiAddByte(mstruct->effect_type, data, &count);
    MultiAddByte(mstruct->phys_info, data, &count);
    MultiAddFloat(mstruct->drag, data, &count);
    MultiAddFloat(mstruct->mass, data, &count);
    MultiAddFloat(mstruct->interval, data, &count);
    MultiAddFloat(mstruct->longevity, data, &count);
    MultiAddFloat(mstruct->lifetime, data, &count);
    MultiAddFloat(mstruct->size, data, &count);
    MultiAddFloat(mstruct->speed, data, &count);
    break;
  case MSAFE_OBJECT_STOP_SPEW:
    sequence = NETSEQ_WORLD;
    MultiAddUbyte(mstruct->id & 0xFF, data, &count);
    break;
  case MSAFE_OBJECT_NO_RENDER:
    sequence = NETSEQ_OBJECTS;
    MultiAddUshort(mstruct->objhandle & HANDLE_OBJNUM_MASK, data, &count);
    break;
  case MSAFE_OBJECT_DEFORM:
    sequence = NETSEQ_OBJECTS;
    MultiAddUshort(mstruct->objhandle & HANDLE_OBJNUM_MASK, data, &count);
    MultiAddFloat(mstruct->amount, data, &count);
    MultiAddFloat(mstruct->lifetime, data, &count);
    break;
  case MSAFE_OBJECT_VIEWER_SHAKE:
    sequence = NETSEQ_OBJECTS;
    MultiAddFloat(mstruct->amount, data, &count);
    break;
  case MSAFE_OBJECT_GHOST:
  case MSAFE_OBJECT_UNGHOST:
    sequence = NETSEQ_OBJECTS;
    MultiAddUshort(mstruct->objhandle & HANDLE_OBJNUM_MASK, data, &count);
    break;
  case MSAFE_OBJECT_INVULNERABLE:
  case MSAFE_OBJECT_CLOAK:
    sequence = NETSEQ_OBJECTS;
    MultiAddUshort(mstruct->objhandle & HANDLE_OBJNUM_MASK, data, &count);
    MultiAddByte(mstruct->state, data, &count);
    MultiAddFloat(mstruct->lifetime, data, &count);
    break;
  case MSAFE_OBJECT_CLOAKALLPLAYERS:
    sequence = NETSEQ_OBJECTS;
    MultiAddByte(mstruct->state, data, &count);
    MultiAddFloat(mstruct->lifetime, data, &count);
    break;
  case MSAFE_OBJECT_LIGHT_DIST:
    sequence = NETSEQ_OBJECTS;
    MultiAddUshort(mstruct->objhandle & HANDLE_OBJNUM_MASK, data, &count);
    MultiAddFloat(mstruct->light_distance, data, &count);
    break;
  case MSAFE_OBJECT_LIGHT_COLOR:
    sequence = NETSEQ_OBJECTS;
    MultiAddUshort(mstruct->objhandle & HANDLE_OBJNUM_MASK, data, &count);
    MultiAddFloat(mstruct->r1, data, &count);
    MultiAddFloat(mstruct->g1, data, &count);
    MultiAddFloat(mstruct->b1, data, &count);
    break;
  case MSAFE_OBJECT_REMOVE:
    sequence = NETSEQ_OBJECTS;
    MultiAddUshort(mstruct->objhandle & HANDLE_OBJNUM_MASK, data, &count);
    MultiAddByte(mstruct->playsound, data, &count);
    break;
  case MSAFE_OBJECT_PLAYER_CONTROLAI:
    sequence = NETSEQ_OBJECTS;
    MultiAddByte(mstruct->slot, data, &count);
    MultiAddByte(mstruct->state, data, &count);
    break;

  case MSAFE_OBJECT_WORLD_POSITION:
    sequence = NETSEQ_OBJECTS;
    MultiAddUshort(mstruct->objhandle & HANDLE_OBJNUM_MASK, data, &count);
    MultiAddInt(mstruct->roomnum, data, &count);
    MultiAddFloat(mstruct->pos.x, data, &count);
    MultiAddFloat(mstruct->pos.y, data, &count);
    MultiAddFloat(mstruct->pos.z, data, &count);
    MultiAddFloat(mstruct->orient.fvec.x, data, &count);
    MultiAddFloat(mstruct->orient.fvec.y, data, &count);
    MultiAddFloat(mstruct->orient.fvec.z, data, &count);
    MultiAddFloat(mstruct->orient.rvec.x, data, &count);
    MultiAddFloat(mstruct->orient.rvec.y, data, &count);
    MultiAddFloat(mstruct->orient.rvec.z, data, &count);
    MultiAddFloat(mstruct->orient.uvec.x, data, &count);
    MultiAddFloat(mstruct->orient.uvec.y, data, &count);
    MultiAddFloat(mstruct->orient.uvec.z, data, &count);
    break;
  case MSAFE_OBJECT_SPARKS:
    sequence = NETSEQ_PLAYING;
    MultiAddUshort(mstruct->objhandle & HANDLE_OBJNUM_MASK, data, &count);
    MultiAddFloat(mstruct->amount, data, &count);
    MultiAddFloat(mstruct->lifetime, data, &count);
    break;
  case MSAFE_SOUND_STREAMING:
    sequence = NETSEQ_PLAYING;
    MultiAddByte(mstruct->state, data, &count);
    MultiAddUshort(mstruct->objhandle & HANDLE_OBJNUM_MASK, data, &count);
    MultiAddFloat(mstruct->volume, data, &count);
    MultiAddString(mstruct->name, data, &count);
    to_slot = mstruct->slot;
    break;
  case MSAFE_SOUND_STOP_OBJ:
    sequence = NETSEQ_PLAYING;
    MultiAddUshort(mstruct->objhandle & HANDLE_OBJNUM_MASK, data, &count);
    break;
  case MSAFE_SOUND_VOLUME_OBJ:
    sequence = NETSEQ_PLAYING;
    MultiAddUshort(mstruct->objhandle & HANDLE_OBJNUM_MASK, data, &count);
    MultiAddFloat(mstruct->volume, data, &count);
    break;
  case MSAFE_MUSIC_REGION:
    sequence = NETSEQ_PLAYING;
    MultiAddByte(mstruct->state, data, &count);
    MultiAddUshort(mstruct->objhandle & HANDLE_OBJNUM_MASK, data, &count);
    MultiAddInt(mstruct->index, data, &count);
    to_slot = mstruct->slot;
    break;
  case MSAFE_SOUND_2D:
    sequence = NETSEQ_PLAYING;
    MultiAddByte(mstruct->state, data, &count);
    MultiAddUshort(mstruct->objhandle & HANDLE_OBJNUM_MASK, data, &count);
    MultiAddFloat(mstruct->volume, data, &count);
    MultiAddInt(mstruct->index, data, &count);
    to_slot = mstruct->slot;
    break;
  case MSAFE_SOUND_OBJECT:
    sequence = NETSEQ_OBJECTS;
    MultiAddUshort(mstruct->objhandle & HANDLE_OBJNUM_MASK, data, &count);
    MultiAddInt(mstruct->index, data, &count);
    break;
  case MSAFE_MISC_LEVELGOAL:
    sequence = NETSEQ_WORLD;
    MultiAddString(mstruct->message, data, &count);
    MultiAddInt(mstruct->index, data, &count);
    MultiAddInt(mstruct->type, data, &count);
    MultiAddInt(mstruct->count, data, &count);
    break;
  case MSAFE_MISC_ENABLE_SHIP:
    sequence = NETSEQ_PLAYERS;
    MultiAddByte(mstruct->state, data, &count);
    MultiAddString(mstruct->name, data, &count);
    break;
  case MSAFE_MISC_UPDATE_HUD_ITEM:
    sequence = NETSEQ_PLAYING;
    MultiAddInt(mstruct->color, data, &count);
    MultiAddString(mstruct->message, data, &count);
    break;
  case MSAFE_MISC_FILTERED_HUD_MESSAGE:
  case MSAFE_MISC_HUD_MESSAGE:
    sequence = NETSEQ_PLAYING;
    MultiAddByte(mstruct->state, data, &count);

    if (mstruct->state) {
      ASSERT(Objects[mstruct->objhandle & HANDLE_OBJNUM_MASK].type == OBJ_PLAYER ||
             Objects[mstruct->objhandle & HANDLE_OBJNUM_MASK].type == OBJ_OBSERVER ||
             Objects[mstruct->objhandle & HANDLE_OBJNUM_MASK].type == OBJ_GHOST);
    }
    MultiAddUshort(mstruct->objhandle & HANDLE_OBJNUM_MASK, data, &count);
    MultiAddInt(mstruct->color, data, &count);
    MultiAddString(mstruct->message, data, &count);
    to_slot = mstruct->slot;
    break;
  case MSAFE_MISC_GAME_MESSAGE:
    sequence = NETSEQ_PLAYING;
    MultiAddByte(mstruct->state, data, &count);
    MultiAddUshort(mstruct->objhandle & HANDLE_OBJNUM_MASK, data, &count);
    MultiAddInt(mstruct->color, data, &count);
    MultiAddString(mstruct->message, data, &count);
    MultiAddString(mstruct->message2, data, &count);
    to_slot = mstruct->slot;
    break;
  case MSAFE_MISC_WAYPOINT:
    sequence = NETSEQ_WORLD;
    MultiAddByte(mstruct->index, data, &count);
    break;
  case MSAFE_MISC_POPUP_CAMERA:
    sequence = NETSEQ_PLAYING;
    MultiAddUshort(mstruct->objhandle & HANDLE_OBJNUM_MASK, data, &count);
    MultiAddFloat(mstruct->interval, data, &count);
    MultiAddFloat(mstruct->scalar, data, &count);
    MultiAddByte(mstruct->gunpoint, data, &count);
    break;
  case MSAFE_MISC_CLOSE_POPUP:
    sequence = NETSEQ_PLAYING;
    break;
  case MSAFE_MISC_GUIDEBOT_NAME:
    sequence = NETSEQ_PLAYING;
    MultiAddUshort(mstruct->objhandle & HANDLE_OBJNUM_MASK, data, &count);
    MultiAddString(mstruct->name, data, &count);
    break;
  case MSAFE_DOOR_LOCK_STATE:
    sequence = NETSEQ_WORLD;
    MultiAddUbyte(mstruct->state, data, &count);
    MultiAddUshort(mstruct->objhandle & HANDLE_OBJNUM_MASK, data, &count);
    break;
  case MSAFE_DOOR_ACTIVATE:
  case MSAFE_DOOR_STOP:
    sequence = NETSEQ_PLAYING;
    MultiAddUshort(mstruct->objhandle & HANDLE_OBJNUM_MASK, data, &count);
    break;
  case MSAFE_DOOR_POSITION:
    sequence = NETSEQ_PLAYING;
    MultiAddUshort(mstruct->objhandle & HANDLE_OBJNUM_MASK, data, &count);
    MultiAddFloat(mstruct->scalar, data, &count);
    break;
  case MSAFE_INVEN_ADD_OBJECT:
    MultiAddInt(mstruct->flags, data, &count);
    MultiAddString(mstruct->message, data, &count);
    // note the fall through
  case MSAFE_INVEN_REMOVE_OBJECT:
    sequence = NETSEQ_PLAYERS;
    MultiAddUshort(mstruct->objhandle & HANDLE_OBJNUM_MASK, data, &count);
    MultiAddUshort(mstruct->ithandle & HANDLE_OBJNUM_MASK, data, &count);
    break;
  case MSAFE_INVEN_ADD_TYPE_ID:
    sequence = NETSEQ_PLAYERS;
    MultiAddUshort(mstruct->objhandle & HANDLE_OBJNUM_MASK, data, &count);
    MultiAddTypeID(mstruct->type, mstruct->id, data, &count);
    MultiAddInt(mstruct->flags, data, &count);
    break;
  case MSAFE_INVEN_REMOVE:
  case MSAFE_COUNTERMEASURE_REMOVE:
    sequence = NETSEQ_PLAYERS;
    MultiAddUshort(mstruct->objhandle & HANDLE_OBJNUM_MASK, data, &count);
    MultiAddTypeID(mstruct->type, mstruct->id, data, &count);
    break;
  case MSAFE_COUNTERMEASURE_ADD:
    sequence = NETSEQ_PLAYERS;

    MultiAddByte(mstruct->slot, data, &count);
    MultiAddString(mstruct->name, data, &count);
    MultiAddByte(mstruct->count, data, &count);
    MultiAddTypeID(mstruct->type, mstruct->id, data, &count);
    break;

  case MSAFE_WEAPON_ADD:
    sequence = NETSEQ_PLAYERS;
    MultiAddUshort(mstruct->objhandle & HANDLE_OBJNUM_MASK, data, &count);
    MultiAddInt(mstruct->index, data, &count);
    MultiAddByte(mstruct->state, data, &count);
    MultiAddInt(mstruct->count, data, &count);
    break;

  default:
    mprintf(0, "Type %d is not handled in multisafe server...fix me!!\n", type);
    Int3(); // Illegal type passed to multisafe function
    break;
  }

  // Close the packet
  END_DATA(count, data, size_offset);

  // Make sure the sequence got set
  ASSERT(sequence != -1);
  if (sequence == -1)
    return;

  if (Demo_flags == DF_RECORDING) {
    DemoWriteMSafe(data, count);
  }

  if ((Game_mode & GM_MULTI) && (Netgame.local_role == LR_SERVER)) {
    // Ok, send this out to our clients
    if (to_slot == -1) {
      if (to_bitmask == 0xFFFFFFFF)
        MultiSendReliablyToAllExcept(Player_num, data, count, sequence, false);
      else {
        for (int i = 1; i < MAX_PLAYERS; i++) {
          if ((to_bitmask & (1 << i)) && (NetPlayers[i].flags & NPF_CONNECTED) && NetPlayers[i].sequence >= sequence)
            nw_SendReliable(NetPlayers[i].reliable_socket, data, count, false);
        }
      }
    } else {
      if (NetPlayers[to_slot].sequence >= sequence)
        nw_SendReliable(NetPlayers[to_slot].reliable_socket, data, count, false);
    }
  }
}

// Resolves a powerup that the server is telling us about
void MultiDoMSafePowerup(uint8_t *data) {
  int count = 0;
  msafe_struct base_mstruct;
  msafe_struct *mstruct = &base_mstruct;

  SKIP_HEADER(data, &count);
  int objnum = MultiGetUshort(data, &count);

  mstruct->objhandle = VerifyMSafeObject(objnum);

  objnum = MultiGetUshort(data, &count);
  mstruct->ithandle = VerifyMSafeObject(objnum);

  ASSERT(mstruct->objhandle != -1 && mstruct->ithandle != -1);
  if (mstruct->objhandle == -1 || mstruct->ithandle == -1)
    return;
  object *op = ObjGet(mstruct->objhandle);
  ASSERT(op);
  if (op && (op->type != OBJ_NONE))
    msafe_DoPowerup(mstruct);
}

// Sends a powerup over to be evaluated on the client machine
void MultiSendMSafePowerup(msafe_struct *mstruct) {
  int sequence = -1;
  int to_slot = -1;

  //	ASSERT (Netgame.local_role==LR_SERVER);

  uint8_t data[MAX_GAME_DATA_SIZE];
  int count = 0;
  int size_offset;

  size_offset = START_DATA(MP_MSAFE_POWERUP, data, &count);

  mprintf(0, "Sending powerup objnum of %d\n", mstruct->objhandle & HANDLE_OBJNUM_MASK);
  MultiAddUshort(mstruct->objhandle & HANDLE_OBJNUM_MASK, data, &count);
  MultiAddUshort(mstruct->ithandle & HANDLE_OBJNUM_MASK, data, &count);

  // Close the packet
  END_DATA(count, data, size_offset);

  if (Demo_flags == DF_RECORDING) {
    DemoWritePowerup(data, count);
  }

  // Ok, send this out to our clients
  if ((Game_mode & GM_MULTI) && (Netgame.local_role == LR_SERVER))
    MultiSendReliablyToAllExcept(Player_num, data, count, NETSEQ_OBJECTS, false);
}
