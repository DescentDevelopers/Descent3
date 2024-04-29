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

// aigame2.cpp
// 0.1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "osiris_import.h"
#include "osiris_common.h"
#include "osiris_vector.h"
#include "DallasFuncs.cpp"

#include "module.h"

#ifdef __cplusplus
extern "C" {
#endif
char STDCALL InitializeDLL(tOSIRISModuleInit *func_list);
void STDCALL ShutdownDLL(void);
int STDCALL GetGOScriptID(const char *name, ubyte isdoor);
void STDCALLPTR CreateInstance(int id);
void STDCALL DestroyInstance(int id, void *ptr);
short STDCALL CallInstanceEvent(int id, void *ptr, int event, tOSIRISEventInfo *data);
int STDCALL SaveRestoreState(void *file_ptr, ubyte saving_state);
#ifdef __cplusplus
}
#endif

int String_table_size = 0;
char **String_table = NULL;
static const char *_Error_string = "!!ERROR MISSING STRING!!";
static const char *_Empty_string = "";
const char *GetStringFromTable(int index) {
  if ((index < 0) || (index >= String_table_size))
    return _Error_string;
  if (!String_table[index])
    return _Empty_string;
  return String_table[index];
}
#define TXT(x) GetStringFromTable(x)

///////////////////////////////////////////////////////////////////////////////
// Script IDs

#define NUM_IDS 5 // maximum number of IDs
#define FIRST_ID 50

#define ID_SAMIRPEST 50       // Samir's test pest.
#define ID_BLACKSTORMTROOP 51 // The CED Black Stormtrooper
#define ID_CREEPER 52         // The CREEPER
#define ID_LUKETURRET 53      // Luke's nifty security turret
#define ID_STBLACKBARREL 54   // The Black Stormtrooper's barrel pack

///////////////////////////////////////////////////////////////////////////////
//	Script names

const char *Script_names[NUM_IDS] = {"Samir's Pest", "StormTrooperBlack", "Creeper", "LukeTurret", "STBlackBarrel"};

//	Class definitions

#define N_SNIPE_POINTS 3

#define GOAL_UID_GETTOPOS 0x100

//	ai base class
class aiObjScript {
public:
  short CallEvent(int event, tOSIRISEventInfo *data);

protected:
  // Handles all possible OSIRIS events.
  virtual void OnInit(int me_handle){};                                            //	EVT_AI_INIT
  virtual void OnDestroy(int me_handle, tOSIRISEVTDESTROY *evt){};                 // EVT_DESTROY
  virtual void OnInterval(tOSIRISEventInfo *data){};                               // EVT_AI_FRAME
  virtual void OnCollide(int me_handle, int it_handle){};                          // EVT_COLLIDE
  virtual bool OnNotify(int me_handle, tOSIRISEVTAINOTIFY *data) { return true; }; //	EVT_AI_NOTIFY
  virtual void OnDamaged(int me_handle, tOSIRISEVTDAMAGED *evt){};                 // EVT_DAMAGED
  virtual void OnMemRestore(void *ptr){};                                          // EVT_MEMRESTORE
};

//	ai base class
#define PEST_STATE_ROAM 0
#define PEST_STATE_HIVE 1

class aiSamirPest : public aiObjScript {
public:
  aiSamirPest();
  ~aiSamirPest();

private:
  struct t_pest_memory {
    int tail_handle;
    float last_anim_frame;
    int foot_sounds[3];
    int state;
  };

  t_pest_memory *memory;

protected:
  // Handles all possible OSIRIS events.
  virtual void OnInit(int me_handle);
  virtual void OnDestroy(int me_handle, tOSIRISEVTDESTROY *evt);
  virtual void OnInterval(tOSIRISEventInfo *data);
  virtual void OnCollide(int me_handle, int it_handle);
  virtual bool OnNotify(int me_handle, tOSIRISEVTAINOTIFY *data);
  virtual void OnMemRestore(void *ptr) { memory = (t_pest_memory *)ptr; };
};

#define STATE_IDLE 0   // waiting state
#define STATE_INIT 1   // initial state
#define STATE_BASE 2   // sentry state.
#define STATE_HIDE 3   // hiding, waiting...
#define STATE_SNIPE 4  // will attack player briefly.
#define STATE_ATTACK 5 // regular attack
#define STATE_STALK 6  // stalks player

class aiBlackStormTrooper : public aiObjScript {
public:
  aiBlackStormTrooper();
  ~aiBlackStormTrooper();

private:
  struct t_bst_memory {
    float timer;
    float snipe_timer;
    float last_frame;
    float laser_timer;
    int target_handle;
    int ammo_barrel;
    int gun_sight_obj;
    int camera_obj;
    sbyte state;
    sbyte snipe_points;
    sbyte cur_snipe_point;
    sbyte laser_targeted;
    short backpack_hp; // points before destroy backpack.
    short flee_hp;

    // a Nx(N-1) array with a extra int for number of VALID connecting snipe points per snipe point.
    //	 note: obvious cases ignored (ptA->ptA)
    // used to create a quick map of snipe points for sniper to use.
    int snipe_point_handles[N_SNIPE_POINTS];
    sbyte access_array[N_SNIPE_POINTS][N_SNIPE_POINTS];

    //	used to keep track of targets black storm trooper knows about.  storm trooper must
    // SEE the target first before storing the target in memory.
    int snipe_target_handles[N_SNIPE_POINTS];
  };

  t_bst_memory *memory;

  void set_state(int me_handle, int state);

protected:
  // Handles all possible OSIRIS events.
  virtual void OnInit(int me_handle);
  virtual void OnDestroy(int me_handle, tOSIRISEVTDESTROY *evt);
  virtual void OnInterval(tOSIRISEventInfo *data);
  virtual bool OnNotify(int me_handle, tOSIRISEVTAINOTIFY *data);
  virtual void OnDamaged(int me_handle, tOSIRISEVTDAMAGED *evt);
  virtual void OnMemRestore(void *ptr) { memory = (t_bst_memory *)ptr; };

private:
  void find_targets(int me_handle, bool new_snipe_point);
};

class aiCreeper : public aiObjScript {
public:
  aiCreeper();
  ~aiCreeper();

private:
  struct t_creep_memory {
    float timer;
    float eye_timer;
    sbyte state;
    ubyte melee_flags;
    short energy;
    short hits;
    int eye_obj;
  };

  t_creep_memory *memory;

  void set_state(int me_handle, int state);

protected:
  // Handles all possible OSIRIS events.
  virtual void OnInit(int me_handle);
  virtual void OnDestroy(int me_handle, tOSIRISEVTDESTROY *evt);
  virtual void OnInterval(tOSIRISEventInfo *data);
  virtual bool OnNotify(int me_handle, tOSIRISEVTAINOTIFY *data);
  virtual void OnDamaged(int me_handle, tOSIRISEVTDAMAGED *evt);
  virtual void OnMemRestore(void *ptr) { memory = (t_creep_memory *)ptr; };
};

class aiLukeTurret : public aiObjScript {
  struct t_lturret_memory {
    float timer;
  };

  t_lturret_memory *memory;

public:
  aiLukeTurret(){};
  ~aiLukeTurret(){};

protected:
  virtual void OnInit(int me_handle);
  virtual bool OnNotify(int me_handle, tOSIRISEVTAINOTIFY *data);
  virtual void OnInterval(tOSIRISEventInfo *data);
  virtual void OnMemRestore(void *ptr) { memory = (t_lturret_memory *)ptr; };
};

class aiSTBlackBarrel : public aiObjScript {
  struct t_stblackbarrel_memory {
    matrix orient;
  };

  t_stblackbarrel_memory *memory;

public:
  aiSTBlackBarrel(){};
  ~aiSTBlackBarrel(){};

protected:
  virtual void OnInit(int me_handle);
  virtual void OnDestroy(int me_handle, tOSIRISEVTDESTROY *evt);
  virtual void OnDamaged(int me_handle, tOSIRISEVTDAMAGED *evt);
  virtual void OnMemRestore(void *ptr) { memory = (t_stblackbarrel_memory *)ptr; };
};

///////////////////////////////////////////////////////////////////////////////
//	InitializeDLL
//	Purpose:
//		This function gets called when the DLL first gets loaded.  It will only be called once (until the
//	DLL is unloaded).  Passed in is a struct of data passed from the game needed for the DLL to interact
//	with D3.  Usually this function will just call osicommon_Initialize(), which sets up the imported
//	functions.  However, you can alloc some memory or whatever in this function, and free it in ShutdownDLL().
//	Note: You cannot call any imported functions until osicommon_Initialize() is called.
//	Returns 1 if initialization went ok, 0 if there was an error and the DLL should not be loaded.
char STDCALL InitializeDLL(tOSIRISModuleInit *func_list) {
  osicommon_Initialize((tOSIRISModuleInit *)func_list);
  String_table_size = func_list->string_count;
  String_table = func_list->string_table;
  if (func_list->game_checksum != CHECKSUM) {
    mprintf(0, "Game-Checksum FAIL!!! (%ul!=%ul)\n", func_list->game_checksum, CHECKSUM);
    mprintf(0, "RECOMPILE YOUR SCRIPTS!!!\n");
    return 0;
  }
  return 1;
}

//	ShutdownDLL
//	Purpose:
//		This function is called right before a DLL is about to be unloaded from memory.  You can free
//	any unfree'd memory, or anything else you need to do.  Don't worry about destroying any instances
//	of scripts, as they will all be automatically destroyed before this function is called.  The
//	same goes for any memory allocated with Scrpt_MemAlloc(), as this will automatically be freed
//	when a scripts instance is destroyed.
void STDCALL ShutdownDLL(void) {}

//	GetGOScriptID
//	Purpose:
//		Given the name of the object (from it's pagename), this function will search through it's
//	list of General Object Scripts for a script with a matching name (to see if there is a script
//	for that type/id of object within this DLL).  If a matching scriptname is found, a UNIQUE ID
//	is to be returned back to Descent 3.  This ID will be used from here on out for all future
//	interaction with the DLL.  Since doors are not part of the generic object's, it's possible
//	for a door to have the same name as a generic object (OBJ_POWERUP, OBJ_BUILDING, OBJ_CLUTTER
//	or OBJ_ROBOT), therefore, a 1 is passed in for isdoor if the given object name refers to a
//	door, else it is a 0.  The return value is the unique identifier, else -1 if the script
//	does not exist in the DLL.
int STDCALL GetGOScriptID(const char *name, ubyte isdoor) {
  int i;
  for (i = 0; i < NUM_IDS; i++) {
    if (strcmp(name, Script_names[i]) == 0) {
      return (FIRST_ID + i);
    }
  }

  return -1;
}

//	CreateInstance
//	Purpose:
//		Given an ID from a call to GetGOScriptID(), this function will create a new instance for that
//	particular script (by allocating and initializing memory, etc.).  A pointer to this instance
//	is to be returned back to Descent 3.  This pointer will be passed around, along with the ID
//	for CallInstanceEvent() and DestroyInstance().  Return NULL if there was an error.
void STDCALLPTR CreateInstance(int id) {
  switch (id) {
  case ID_SAMIRPEST:
    return new aiSamirPest;
  case ID_BLACKSTORMTROOP:
    return new aiBlackStormTrooper;
  case ID_CREEPER:
    return new aiCreeper;
  case ID_LUKETURRET:
    return new aiLukeTurret;
  case ID_STBLACKBARREL:
    return new aiSTBlackBarrel;
  }

  return NULL;
}

//	DestroyInstance
//	Purpose:
//		Given an ID, and a pointer to a particular instance of a script, this function will delete and
//	destruct all information associated with that script, so it will no longer exist.
void STDCALL DestroyInstance(int id, void *ptr) {
  switch (id) {
  case ID_SAMIRPEST:
    delete ((aiSamirPest *)ptr);
    break;
  case ID_BLACKSTORMTROOP:
    delete ((aiBlackStormTrooper *)ptr);
    break;
  case ID_CREEPER:
    delete ((aiCreeper *)ptr);
    break;
  case ID_LUKETURRET:
    delete ((aiLukeTurret *)ptr);
    break;
  case ID_STBLACKBARREL:
    delete ((aiSTBlackBarrel *)ptr);
    break;
  }
}

//	CallInstanceEvent
//	Purpose:
//		Given an ID, a pointer to a script instance, an event and a pointer to the struct of
//	information about the event, this function will translate who this event belongs to and
//	passes the event to that instance of the script to be handled.  Return a combination of
//	CONTINUE_CHAIN and CONTINUE_DEFAULT, to give instructions on what to do based on the
//	event. CONTINUE_CHAIN means to continue through the chain of scripts (custom script, level
//	script, mission script, and finally default script).  If CONTINUE_CHAIN is not specified,
//	than the chain is broken and those scripts of lower priority will never get the event.  Return
//	CONTINUE_DEFAULT in order to tell D3 if you want process the normal action that is built into
//	the game for that event.  This only pertains to certain events.  If the chain continues
//	after this script, than the CONTINUE_DEFAULT setting will be overridden by lower priority
//	scripts return value.
short STDCALL CallInstanceEvent(int id, void *ptr, int event, tOSIRISEventInfo *data) {
  return ((aiObjScript *)ptr)->CallEvent(event, data);
}

//	SaveRestoreState
//	Purpose:
//		This function is called when Descent 3 is saving or restoring the game state.  In this function
//	you should save/restore any global data that you want preserved through load/save (which includes
//	demos).  You must be very careful with this function, corrupting the file (reading or writing too
//	much or too little) may be hazardous to the game (possibly making it impossible to restore the
//	state).  It would be best to use version information to keep older versions of saved states still
//	able to be used.  IT IS VERY IMPORTANT WHEN SAVING THE STATE TO RETURN THE NUMBER OF _BYTES_ WROTE
//	TO THE FILE.  When restoring the data, the return value is ignored.  saving_state is 1 when you should
//	write data to the file_ptr, 0 when you should read in the data.
int STDCALL SaveRestoreState(void *file_ptr, ubyte saving_state) { return 0; }

//////////////////////////////////////////////////////////////////////////////
// HELPER FUNCTIONS

// Returns the new child's handle
int CreateAndAttach(int me, const char *child_name, ubyte child_type, char parent_ap, char child_ap, bool f_aligned = true,
                    bool f_set_parent = false) {
  int child_handle = OBJECT_HANDLE_NONE;
  int child_id = Obj_FindID(child_name);
  msafe_struct m;
  m.objhandle = me;
  MSafe_GetValue(MSAFE_OBJECT_POS, &m);
  MSafe_GetValue(MSAFE_OBJECT_ROOMNUM, &m);

  if (child_id >= 0) {
    int parent;
    if (f_set_parent)
      parent = me;
    else
      parent = OBJECT_HANDLE_NONE;

    child_handle = Obj_Create(child_type, child_id, m.roomnum, &m.pos, NULL, parent);
    if (child_handle != OBJECT_HANDLE_NONE) {
      if (!Obj_AttachObjectAP(me, parent_ap, child_handle, child_ap, f_aligned)) {
        // chrishack (we need a way to instantly kill scripted objects)
      }
    }
  }

  return child_handle;
}

void FlushGoal(int me_handle, int goal_priority) {
  bool used;

  AI_GoalValue(me_handle, goal_priority, VF_GET, AIGV_B_USED, &used);
  if (used) {
    int flags;
    AI_GoalValue(me_handle, 3, VF_GET, AIGV_I_FLAGS, &flags);

    if (!(flags & GF_NONFLUSHABLE)) {
      AI_ClearGoal(me_handle, goal_priority);
    }
  }
}

#define DALLAS_LOW_PRIORITY_GOAL_SLOT 0
#define DALLAS_HIGH_PRIORITY_GOAL_SLOT 3

// Wipes out all goals except slots 0 and 3 (used by Dallas)
void SafeGoalClearAll(int obj_handle) {
  int i;

  for (i = 0; i < MAX_GOALS; i++) {
    if (i != DALLAS_LOW_PRIORITY_GOAL_SLOT && i != DALLAS_HIGH_PRIORITY_GOAL_SLOT) {
      AI_ClearGoal(obj_handle, i);
    }
  }
}

// Works like AI_SetType(), but won't clear out Dallas' goal slots 0 or 3
void AI_SafeSetType(int obj_handle, int ai_type) {
  float aware = AWARE_BARELY;
  float time = 25.0f;

  SafeGoalClearAll(obj_handle);

  switch (ai_type) {
  case AIT_STALKER: {
    vector goal_pos;
    int goal_room;

    Obj_Value(obj_handle, VF_GET, OBJV_V_POS, &goal_pos);
    Obj_Value(obj_handle, VF_GET, OBJV_I_ROOMNUM, &goal_room);

    AI_AddGoal(obj_handle, AIG_WANDER_AROUND, 1, 1.0f, -1, GF_NONFLUSHABLE | GF_KEEP_AT_COMPLETION, &goal_pos,
               goal_room);
  } break;

  case AIT_EVADER2: {
    vector goal_pos;
    int goal_room;
    float size, circle_distance;

    Obj_Value(obj_handle, VF_GET, OBJV_V_POS, &goal_pos);
    Obj_Value(obj_handle, VF_GET, OBJV_I_ROOMNUM, &goal_room);
    Obj_Value(obj_handle, VF_GET, OBJV_F_SIZE, &size);
    AI_Value(obj_handle, VF_GET, AIV_F_CIRCLE_DIST, &circle_distance);

    AI_AddGoal(obj_handle, AIG_GUARD_AREA, 1, 1.0f, -1, GF_NONFLUSHABLE, &goal_pos, goal_room);
    AI_SetGoalCircleDist(obj_handle, 1, size * 2.0f);
    AI_GoalAddEnabler(obj_handle, 1, AIE_GT_AWARENESS, 1.0, 0.0, &aware);
    AI_GoalAddEnabler(obj_handle, 1, AIE_LTE_LAST_SEE_TARGET_INTERVAL, 1.0, 0.0, &time);

    int gi = AI_AddGoal(obj_handle, AIG_GET_AROUND_OBJ, ACTIVATION_BLEND_LEVEL, 3.0f, -1,
                        GF_NONFLUSHABLE | GF_OBJ_IS_TARGET, OBJECT_HANDLE_NONE);
    AI_GoalAddEnabler(obj_handle, gi, AIE_NEAR, 1.0, 0.0, &circle_distance);
    AI_SetGoalCircleDist(obj_handle, gi, circle_distance);
  } break;

  case AIT_EVADER1: {
    float circle_distance;
    AI_Value(obj_handle, VF_GET, AIV_F_CIRCLE_DIST, &circle_distance);

    AI_AddGoal(obj_handle, AIG_MOVE_RELATIVE_OBJ, 1, 1.0f, -1, GF_OBJ_IS_TARGET | GF_SPEED_ATTACK | GF_NONFLUSHABLE,
               OBJECT_HANDLE_NONE);
    AI_GoalAddEnabler(obj_handle, 1, AIE_GT_AWARENESS, 1.0, 0.0, &aware);
    AI_GoalAddEnabler(obj_handle, 1, AIE_LTE_LAST_SEE_TARGET_INTERVAL, 1.0, 0.0, &time);

    if (circle_distance > 0.0f) {
      int gi = AI_AddGoal(obj_handle, AIG_GET_AROUND_OBJ, ACTIVATION_BLEND_LEVEL, 3.0f, -1,
                          GF_NONFLUSHABLE | GF_OBJ_IS_TARGET, OBJECT_HANDLE_NONE);

      float temp = circle_distance * 0.55f;
      AI_GoalAddEnabler(obj_handle, gi, AIE_NEAR, 1.0, 0.0, &temp);
      AI_SetGoalCircleDist(obj_handle, gi, circle_distance * 0.55f);
    }
  } break;

  case AIT_MELEE1: {
    int flags = AISR_MELEE;
    int m_goal;

    AI_AddGoal(obj_handle, AIG_MOVE_RELATIVE_OBJ, 1, 1.0f, -1,
               GF_NONFLUSHABLE | GF_OBJ_IS_TARGET | GF_KEEP_AT_COMPLETION, OBJECT_HANDLE_NONE);
    AI_GoalAddEnabler(obj_handle, 1, AIE_GT_AWARENESS, 1.0, 0.0, &aware);
    AI_GoalAddEnabler(obj_handle, 1, AIE_LTE_LAST_SEE_TARGET_INTERVAL, 1.0, 0.0, &time);

    AI_AddGoal(obj_handle, AIG_GET_TO_OBJ, 2, 1.0f, -1,
               GF_SPEED_ATTACK | GF_NONFLUSHABLE | GF_OBJ_IS_TARGET | GF_USE_BLINE_IF_SEES_GOAL | GF_KEEP_AT_COMPLETION,
               OBJECT_HANDLE_NONE);
    AI_GoalAddEnabler(obj_handle, 2, AIE_AI_STATUS_FLAG, 1.0, 0.0, &flags);
    AI_SetGoalCircleDist(obj_handle, 2, -100.0f);
    AI_GoalAddEnabler(obj_handle, 2, AIE_GT_AWARENESS, 1.0, 0.0, &aware);
    AI_GoalAddEnabler(obj_handle, 2, AIE_LTE_LAST_SEE_TARGET_INTERVAL, 1.0, 0.0, &time);

    m_goal = AI_AddGoal(obj_handle, AIG_MELEE_TARGET, ACTIVATION_BLEND_LEVEL, 1.0f, -1,
                        GF_NONFLUSHABLE | GF_OBJ_IS_TARGET | GF_KEEP_AT_COMPLETION, OBJECT_HANDLE_NONE);
    AI_GoalAddEnabler(obj_handle, m_goal, AIE_GT_AWARENESS, 1.0, 0.0, &aware);
    AI_GoalAddEnabler(obj_handle, m_goal, AIE_LTE_LAST_SEE_TARGET_INTERVAL, 1.0, 0.0, &time);
  } break;

  case AIT_BIRD_FLOCK1: {
  } break;

  case AIT_HERD1: {
  } break;

  case AIT_STATIONARY_TURRET:
    break;

  case AIT_AIS:
    break;

  case AIT_FLYLANDER:
    break;

  default: {
    mprintf(0, "Invalid AI type passed into AI_SafeSetType()\n");
  }
  }
}

int TurnOnSpew(int objref, int gunpoint, int effect_type, float mass, float drag, int gravity_type, ubyte isreal,
               float lifetime, float interval, float longevity, float size, float speed, ubyte random) {
  msafe_struct mstruct;

  mstruct.objhandle = objref;
  mstruct.gunpoint = gunpoint;
  mstruct.effect_type = effect_type;
  mstruct.mass = mass;
  mstruct.drag = drag;
  mstruct.phys_info = gravity_type;
  mstruct.is_real = isreal;
  mstruct.lifetime = lifetime;
  mstruct.interval = interval;
  mstruct.longevity = longevity;
  mstruct.size = size;
  mstruct.speed = speed;
  mstruct.random = (random) ? SPEW_RAND_SIZE | SPEW_RAND_LIFETIME | SPEW_RAND_SPEED : 0;

  MSafe_CallFunction(MSAFE_OBJECT_START_SPEW, &mstruct);

  return mstruct.id;
}

//////////////////////////////////////////////////////////////////////////////
//	aiObjScript
//		all ai scripts will have this as a base function.
short aiObjScript::CallEvent(int event, tOSIRISEventInfo *data) {
  switch (event) {
  case EVT_AI_INIT:
    OnInit(data->me_handle);
    break;
  case EVT_AI_FRAME:
    OnInterval(data);
    break;
  case EVT_AI_NOTIFY:
    if (!OnNotify(data->me_handle, &data->evt_ai_notify))
      return 0;
    break;
  case EVT_DESTROY:
    OnDestroy(data->me_handle, &data->evt_destroy);
    break;
  case EVT_COLLIDE:
    OnCollide(data->me_handle, data->evt_collide.it_handle);
    break;
  case EVT_DAMAGED:
    OnDamaged(data->me_handle, &data->evt_damaged);
    break;
  case EVT_MEMRESTORE:
    OnMemRestore(data->evt_memrestore.memory_ptr);
    break;
  }

  return CONTINUE_CHAIN | CONTINUE_DEFAULT;
};

//////////////////////////////////////////////////////////////////////////////
//	aiSamirPest
//		The Samir Pest

//	ai base class
aiSamirPest::aiSamirPest() { memory = NULL; }

aiSamirPest::~aiSamirPest() {}

void aiSamirPest::OnInit(int me_handle) {
  tOSIRISMEMCHUNK ch;

  mprintf(0, "OnInit\n");

  ch.id = 4;
  ch.size = sizeof(aiSamirPest::t_pest_memory);
  ch.my_id.type = OBJECT_SCRIPT;
  ch.my_id.objhandle = me_handle;

  memory = (aiSamirPest::t_pest_memory *)Scrpt_MemAlloc(&ch);

  memory->tail_handle = CreateAndAttach(me_handle, "Pesttail", OBJ_ROBOT, 0, 0);
  memory->last_anim_frame = 0;

  memory->foot_sounds[0] = Sound_FindId("RbtPestFoot1");
  memory->foot_sounds[1] = Sound_FindId("RbtPestFoot2");
  memory->foot_sounds[2] = Sound_FindId("RbtPestFoot3");

  memory->state = PEST_STATE_ROAM;

  AI_SetType(me_handle, AIT_EVADER1);
}

void aiSamirPest::OnDestroy(int me_handle, tOSIRISEVTDESTROY *evt) {
  mprintf(0, "OnDestroy\n");

  msafe_struct mstruct;

  mstruct.objhandle = memory->tail_handle;
  mstruct.killer_handle = OBJECT_HANDLE_NONE;
  mstruct.damage_type = GD_SCRIPTED;
  mstruct.amount = 1000.0f;

  MSafe_CallFunction(MSAFE_OBJECT_DAMAGE_OBJECT, &mstruct);
}

void aiSamirPest::OnInterval(tOSIRISEventInfo *data) {
  float frame;
  Obj_Value(data->me_handle, VF_GET, OBJV_F_ANIM_FRAME, &frame);
  if ((frame > 9 && memory->last_anim_frame <= 9) || (frame > 10 && memory->last_anim_frame <= 10) ||
      (frame > 13 && memory->last_anim_frame <= 13)) {
    int index = rand() % 3;

    Sound_Play3d(data->me_handle, memory->foot_sounds[index]);
  }

  memory->last_anim_frame = frame;
}

void aiSamirPest::OnCollide(int me_handle, int it_handle) {
  int type;

  Obj_Value(it_handle, VF_GET, OBJV_I_TYPE, &type);

  if (type == OBJ_PLAYER && memory->state != PEST_STATE_HIVE) {
    // tell pest to return to hive.
    int id = Scrpt_FindObjectName("Hive");
    int room_index;
    vector pos;

    // force scripting for this.
    AI_SetType(me_handle, AIT_AIS);

    // go to hive location!
    Obj_Value(id, VF_GET, OBJV_V_POS, &pos);
    Obj_Value(id, VF_GET, OBJV_I_ROOMNUM, &room_index);
    AI_AddGoal(me_handle, AIG_GET_TO_POS, 1, 1.0, -1, GF_KEEP_AT_COMPLETION | GF_NOTIFIES, &pos, room_index);
    AI_SetGoalCircleDist(me_handle, 1, 10.0f);

    memory->state = PEST_STATE_HIVE;
  }
}

bool aiSamirPest::OnNotify(int me_handle, tOSIRISEVTAINOTIFY *data) { return true; }

//////////////////////////////////////////////////////////////////////////////
//	aiBlackStormTrooper
//		CED Black Stormtrooper

//	ai base class
aiBlackStormTrooper::aiBlackStormTrooper() { memory = NULL; }

aiBlackStormTrooper::~aiBlackStormTrooper() {}

void aiBlackStormTrooper::OnInit(int me_handle) {
  tOSIRISMEMCHUNK ch;
  int n_pts, j, i;

  ch.id = 4;
  ch.size = sizeof(aiBlackStormTrooper::t_bst_memory);
  ch.my_id.type = OBJECT_SCRIPT;
  ch.my_id.objhandle = me_handle;

  memory = (aiBlackStormTrooper::t_bst_memory *)Scrpt_MemAlloc(&ch);

  //	reset timer for robot.
  memory->timer = 0;

  for (i = 0; i < N_SNIPE_POINTS; i++) {
    memory->snipe_target_handles[i] = OBJECT_HANDLE_NONE;
  }

  // get sniper points available for trooper.
  memory->cur_snipe_point = -1;
  for (memory->snipe_points = 0; memory->snipe_points < N_SNIPE_POINTS; memory->snipe_points++) {
    char buf[64];
    char name[48];
    int id;
    Obj_Value(me_handle, VF_GET, OBJV_S_NAME, name);
    sprintf(buf, "Snipe%s-%c", name, 'A' + memory->snipe_points);
    id = Scrpt_FindObjectName(buf);
    if (id == OBJECT_HANDLE_NONE) {
      break;
    }
    memory->snipe_point_handles[memory->snipe_points] = id;
  }

  // determine reachable snipe points for each snipe point!
  for (i = 0; i < memory->snipe_points; i++) {
    for (j = 0, n_pts = 0; j < memory->snipe_points; j++) {
      if (i != j) {
        if (AI_IsObjReachable(memory->snipe_point_handles[i], memory->snipe_point_handles[j])) {
          memory->access_array[i][n_pts++] = (sbyte)j;
        }
      }
    }
    memory->access_array[i][N_SNIPE_POINTS - 1] = (sbyte)n_pts;
  }

  // initialize miscellaneous stuff
  memory->backpack_hp = 200;
  memory->flee_hp = 50;
  memory->gun_sight_obj = CreateAndAttach(me_handle, "STEmitter", OBJ_ROBOT, 0, 0, true, true);
  memory->ammo_barrel = CreateAndAttach(me_handle, "StormTrooperBlackBarrel", OBJ_ROBOT, 1, 0, true, true);
  memory->camera_obj = OBJECT_HANDLE_NONE;

  memory->last_frame = 0.0f;
  memory->target_handle = OBJECT_HANDLE_NONE;
  memory->laser_targeted = false;

  // just wait for a target.
  set_state(me_handle, STATE_INIT);
}

void aiBlackStormTrooper::find_targets(int me_handle, bool new_snipe_point) {
  vector pos; // position of trooper
  int room;   // room of trooper

  int nearby_objs[32];                     // objhandles for nearby players (or robots)
  int player_objs[8];                      // nearby players.
  int nobjs, np;                           // number of objects and players
  int closest_player = OBJECT_HANDLE_NONE; //	handle of closest player
  float closest_player_dist = 10000.0f;    // distance of closest player from trooper.
  float current_player_dist = 10000.0f;    // used to store distance of current targeted player
  int i, j;

  //	now determine target, closest player?
  // get nearby objects within 1000 units of trooper.  our substitution for FOV check, since troopers
  //	are usually behind walls.
  Obj_Value(me_handle, VF_GET, OBJV_V_POS, &pos);
  Obj_Value(me_handle, VF_GET, OBJV_I_ROOMNUM, &room);

  nobjs = AI_GetNearbyObjs(&pos, room, 500.0f, nearby_objs, 32,
                           false, // lightmaps, no.
                           true,  // players and robots
                           false, // don't include non collide objects
                           true); // stop at doors, don't check behind them.

  // get only players.
  for (i = 0, np = 0; i < nobjs && np < 8; i++) {
    int objtype = -1;

    Obj_Value(nearby_objs[i], VF_GET, OBJV_I_TYPE, &objtype);
    if (objtype == OBJ_PLAYER) {
      // if a player, check if it's in the target list
      //	(the trooper must have seen the target at least once before to keep track of it)
      for (j = 0; j < N_SNIPE_POINTS; j++) {
        if (nearby_objs[i] == memory->snipe_target_handles[j]) {
          float dist;
          player_objs[np++] = nearby_objs[i];
          dist = AI_GetDistToObj(me_handle, nearby_objs[i]);
          if (memory->target_handle == nearby_objs[i]) {
            current_player_dist = dist;
          }
          if (closest_player_dist > dist) {
            closest_player_dist = dist;
            closest_player = nearby_objs[i];
          }
        }
      }
    }
  }

  //// lost all targets, go back into idle state.
  if (closest_player == OBJECT_HANDLE_NONE) {
    memory->target_handle = OBJECT_HANDLE_NONE;
    return;
  }

  // if we don't have a target yet, make this player a target.
  if (np && (memory->target_handle == OBJECT_HANDLE_NONE ||
             (current_player_dist > closest_player_dist && memory->target_handle != closest_player))) {
    memory->target_handle = closest_player;
    AI_Value(me_handle, VF_SET, AIV_I_TARGET_HANDLE, &memory->target_handle);
    mprintf(0, "BStorm: acquired new target through clairvoyance.\n");
  }

  // do something with snipe point if new_snipe_point is true (like find an ideal one)
}

void aiBlackStormTrooper::OnDestroy(int me_handle, tOSIRISEVTDESTROY *evt) {
  msafe_struct mstruct;

  mstruct.objhandle = memory->ammo_barrel;
  mstruct.killer_handle = OBJECT_HANDLE_NONE;
  mstruct.damage_type = GD_SCRIPTED;
  mstruct.amount = 1000.0f;

  MSafe_CallFunction(MSAFE_OBJECT_DAMAGE_OBJECT, &mstruct);

  if (memory->camera_obj != OBJECT_HANDLE_NONE) {
    mstruct.objhandle = memory->camera_obj;
    MSafe_CallFunction(MSAFE_OBJECT_REMOVE, &mstruct);
  }

  mstruct.objhandle = memory->gun_sight_obj;
  MSafe_CallFunction(MSAFE_OBJECT_REMOVE, &mstruct);
}

void aiBlackStormTrooper::OnInterval(tOSIRISEventInfo *data) {
  msafe_struct mstruct;
  int me_handle = data->me_handle;
  float anim_frame;
  vector pos;
  int room;

  Obj_Value(me_handle, VF_GET, OBJV_F_ANIM_FRAME, &anim_frame);
  Obj_Value(me_handle, VF_GET, OBJV_I_ROOMNUM, &room);
  Obj_Value(me_handle, VF_GET, OBJV_V_POS, &pos);

  // laser gunsight
  if (memory->laser_targeted) {
    ray_info ray;
    matrix orient;

    Obj_Value(me_handle, VF_GET, OBJV_M_ORIENT, &orient);

    // Determine real start pos - room
    vector end_pos = pos;
    end_pos += orient.fvec * 2000.0f;

    int fvi_flags = FQ_CHECK_OBJS | FQ_IGNORE_POWERUPS | FQ_IGNORE_WEAPONS | FQ_IGNORE_MOVING_OBJECTS |
                    FQ_IGNORE_NON_LIGHTMAP_OBJECTS;
    int fate = FVI_RayCast(me_handle, &pos, &end_pos, room, 0.0f, fvi_flags, &ray);

    Obj_Value(memory->camera_obj, VF_SET, OBJV_I_ROOMNUM, &ray.hit_room);
    Obj_Value(memory->camera_obj, VF_SET, OBJV_V_POS, &ray.hit_point);

    if (Game_GetTime() >= memory->laser_timer) {
      mstruct.objhandle = memory->camera_obj;
      MSafe_CallFunction(MSAFE_OBJECT_REMOVE, &mstruct);

      memory->camera_obj = OBJECT_HANDLE_NONE;
      memory->laser_targeted = false;
    }
  }

  // check if the pack is gone
  int attached_handle = Obj_GetAttachChildHandle(me_handle, 1);
  if (attached_handle == OBJECT_HANDLE_NONE) {
    aObjDestroy(me_handle);
    return;
  }

  // state code.
  switch (memory->state) {
  case STATE_HIDE:
    // we didn't find a valid snipe point?  try to find one after a certain amount of time.
    // this will ensure that every so often, if there is no valid snipe point specified,
    //	we will try to find one.
    if (memory->snipe_points == 0) {
      if (Game_GetTime() > memory->timer) {
        set_state(data->me_handle, STATE_SNIPE);
      }
    } else if (memory->cur_snipe_point == -1) {
      if (Game_GetTime() > memory->timer) {
        set_state(data->me_handle, STATE_HIDE);
      }
    }
    break;

  case STATE_SNIPE:
    // snipe for a certain amount of time, then return to base.
    if (memory->laser_targeted == false) {
      int type;

      memory->camera_obj = Obj_Create(OBJ_POWERUP, Obj_FindID("Invisiblepowerup"), room, &pos, NULL, me_handle);
      mstruct.objhandle = memory->camera_obj;
      MSafe_CallFunction(MSAFE_OBJECT_NO_RENDER, &mstruct);

      Obj_Value(me_handle, VF_GET, OBJV_I_ROOMNUM, &mstruct.roomnum);
      Obj_Value(me_handle, VF_GET, OBJV_V_POS, &mstruct.pos);
      Obj_Value(memory->camera_obj, VF_GET, OBJV_V_POS, &mstruct.pos2);

      mstruct.objhandle = memory->gun_sight_obj;
      mstruct.ithandle = memory->camera_obj;
      mstruct.lifetime = 5.0f;
      mstruct.size = 1.0f;

      mstruct.interval = 1.0f;
      mstruct.count = 2;
      mstruct.index = 1;
      mstruct.texnum = Scrpt_FindTextureName("Stlaser");
      mstruct.color = ((128 >> 3) << 10) | ((128 >> 3) << 5) | (128 >> 3);

      mstruct.state = 0;
      mstruct.flags = 0;

      memory->laser_timer = Game_GetTime() + mstruct.lifetime;
      memory->laser_targeted = true;

      MSafe_CallFunction(MSAFE_WEATHER_LIGHTNING_BOLT, &mstruct);
      //	mprintf(0, "BStorm: TARGETING!!!\n");
    }

    if (Game_GetTime() > memory->timer) {
      set_state(data->me_handle, STATE_HIDE);
    }
    break;

  case STATE_BASE:
    if (Game_GetTime() > memory->timer) {
      if (memory->target_handle == OBJECT_HANDLE_NONE) {
        set_state(data->me_handle, STATE_IDLE);
      } else {
        int roll = (rand() % 100);
        if (roll <= 50) {
          find_targets(data->me_handle, false);
          memory->timer = Game_GetTime() + ((memory->target_handle == OBJECT_HANDLE_NONE) ? 5.0f : 2.0f);
        }
      }
    }
    if (Game_GetTime() > memory->snipe_timer) {
      set_state(data->me_handle, STATE_SNIPE);
      memory->snipe_timer = Game_GetTime() + 3.0f + (rand() % 7) * 0.5f;
    }
    break;
  }

  memory->last_frame = anim_frame;
}

void aiBlackStormTrooper::OnDamaged(int me_handle, tOSIRISEVTDAMAGED *evt) {
  if (memory->state == STATE_BASE || memory->state == STATE_SNIPE) {
    memory->flee_hp -= (short)evt->damage;
    if (memory->flee_hp <= 0) {
      memory->flee_hp = 50;
      set_state(me_handle, STATE_HIDE);
    }
  }
}

bool aiBlackStormTrooper::OnNotify(int me_handle, tOSIRISEVTAINOTIFY *data) {
  switch (data->notify_type) {
  case AIN_FIRED_WEAPON:
    // Create gun barrel smoke
    TurnOnSpew(me_handle, 0, 7, 0.0f, 0.0f, 65536, false, 0.5f, 0.1f, 0.7f, 1.0f, 15.0f, true);
    break;

  case AIN_SEE_TARGET:
    memory->snipe_target_handles[0] = data->it_handle;
    //	mprintf (0, "BStorm: See target!!\n");
    if (memory->state == STATE_IDLE) {
      //	mprintf (0, "BStorm: Found target!!\n");
      memory->target_handle = memory->snipe_target_handles[0];
      set_state(me_handle, STATE_HIDE);

      mprintf(0, "Black Stormtrooper Saw target!\n");
    }
    break;

    //@@	case AIN_SEE_TARGET:
    //@@	// snipe!!
    //@@		if (memory->state == STATE_IDLE) {
    //@@			mprintf (0, "Acquired!\n");
    //@@			set_state(me_handle, STATE_HIDE);
    //@@		}
    //@@		else if (memory->state == STATE_BASE && memory->timer < Game_GetTime()) {
    //@@			if ((rand() % 100) < 60) {
    //@@				set_state(me_handle, STATE_SNIPE);
    //@@			}
    //@@			memory->timer = Game_GetTime() + 2.0f + (rand() % 6)*0.5f;
    //// use timer to switch states.
    //@@		}
    break;

  case AIN_NEAR_TARGET:
    if (memory->state == STATE_BASE) {
      //	mprintf (0, "BStorm: Near target, must flee.\n");
      set_state(me_handle, STATE_HIDE);
    }
    break;

  case AIN_GOAL_COMPLETE:
    if (data->goal_uid == GOAL_UID_GETTOPOS) {
      //	mprintf(0, "BStorm: At hiding position %d.\n", memory->cur_snipe_point);
      set_state(me_handle, STATE_BASE);
    }
    break;

  case AIN_HIT_BY_WEAPON:
    // check if backpack hit.  if so, deduct hitpoints, if 0, frag it.
    //	also, if hit enough, force another hide action.
    break;
  }

  return true;
}

void aiBlackStormTrooper::set_state(int me_handle, int state) {
  int room_number, id, i;
  vector pos;
  char buf[64];
  char name[48];
  float dist;

  // SafeGoalClearAll(me_handle);

retry_set_state:
  switch (state) {
  case STATE_INIT:
    mprintf(0, "Black Stormtrooper set to init mode.\n");
    state = STATE_IDLE;
    goto retry_set_state;

  case STATE_IDLE:
    mprintf(0, "Black Stormtrooper set to idle mode.\n");

    // AI_SafeSetType(me_handle, AIT_EVADER2);
    ///*
    FlushGoal(me_handle, 1);
    FlushGoal(me_handle, 2);
    Obj_Value(me_handle, VF_GET, OBJV_V_POS, &pos);
    Obj_Value(me_handle, VF_GET, OBJV_I_ROOMNUM, &room_number);

    dist = 10.0f;
    AI_AddGoal(me_handle, AIG_GUARD_AREA, 0, 1.0f, -1, GF_NOTIFIES | GF_KEEP_AT_COMPLETION, &pos, room_number);
    AI_GoalValue(me_handle, 0, VF_SET, AIGV_F_CIRCLE_DIST, &dist);
    //*/

    //	mprintf (0, "BStorm: Idling around.\n");
    break;

  case STATE_BASE:
    mprintf(0, "Black Stormtrooper set to base mode.\n");

    // AI_SafeSetType(me_handle, AIT_EVADER2);
    ///*
    // okay, we will stand still, wait to snipe, face target, try to acquire targets, etc.
    FlushGoal(me_handle, 1);
    FlushGoal(me_handle, 2);
    if (memory->cur_snipe_point == -1) {
      //	mprintf (0, "BStorm: Eek! We should be hiding, not basing!");
      state = STATE_HIDE;
      goto retry_set_state;
    }
    id = memory->snipe_point_handles[memory->cur_snipe_point];
    Obj_Value(id, VF_GET, OBJV_V_POS, &pos);
    Obj_Value(id, VF_GET, OBJV_I_ROOMNUM, &room_number);

    dist = 0.0f;
    AI_AddGoal(me_handle, AIG_GUARD_AREA, 2, 1.0f, -1, GF_KEEP_AT_COMPLETION, &pos, room_number);
    AI_GoalValue(me_handle, 2, VF_SET, AIGV_F_CIRCLE_DIST, &dist);
    //*/

    /*
    AI_AddGoal(me_handle, AIG_FIRE_AT_OBJ, ACTIVATION_BLEND_LEVEL, 1.0f, -1, GF_ORIENT_TARGET |
                                            GF_SPEED_ATTACK |
                                            GF_KEEP_AT_COMPLETION |
                                            GF_OBJ_IS_TARGET,
                                            NULL, GST_NEG_FVEC);
    */

    memory->timer = Game_GetTime() + 2.0f; // use timer to reacquire targets.
    memory->snipe_timer = Game_GetTime() + 2.0f + (rand() % 7) * 0.5f;

    find_targets(me_handle, false);
    //	mprintf(0, "BStorm: Base\n");
    break;

  case STATE_HIDE:
    mprintf(0, "Black Stormtrooper set to hide mode.\n");
    // find a snipe point that's different from current one.
    FlushGoal(me_handle, 1);
    FlushGoal(me_handle, 2);
    if (memory->snipe_points) {
      if (memory->cur_snipe_point == -1) {
        // find a snipe point now!
        // bot is either fresh or has been wandering, looking for a place to snipe
        for (i = 0; i < memory->snipe_points; i++) {
          if (AI_IsObjReachable(me_handle, memory->snipe_point_handles[i])) {
            break;
          }
        }
        if (i == memory->snipe_points) {
          //	 hmm, the bot is not in a good place.  have him wander until he finds a snipe point.
          memory->timer = Game_GetTime() + 4.0f;
          Obj_Value(me_handle, VF_GET, OBJV_V_POS, &pos);
          Obj_Value(me_handle, VF_GET, OBJV_I_ROOMNUM, &room_number);
          AI_AddGoal(me_handle, AIG_WANDER_AROUND, 1, 1.0f, -1, GF_SPEED_FLEE | GF_NOTIFIES | GF_KEEP_AT_COMPLETION,
                     &pos, room_number);
          mprintf(0, "BStorm: WARNING-0 no valid snipe point nearby. Design problem?\n");
        } else {
          memory->cur_snipe_point = i;
        }
      } else {
        // this will get an index into the reachable snipe points.
        int n_valid_pts = memory->access_array[memory->cur_snipe_point][N_SNIPE_POINTS - 1];

        if (n_valid_pts > 0) {
          i = rand() % n_valid_pts;
          memory->cur_snipe_point = memory->access_array[memory->cur_snipe_point][i];
        } else {
          memory->cur_snipe_point = -1; // force a periodic check for a snipe point.
          memory->timer = Game_GetTime() + 4.0f;
          mprintf(0, "BStorm: WARNING-1 no valid snipe point nearby. Design problem?\n");
        }
      }

      // by now, we either have a new valid snipe point, or no snipe point.
      if (memory->cur_snipe_point != -1) {
        id = memory->snipe_point_handles[memory->cur_snipe_point];
        Obj_Value(id, VF_GET, OBJV_V_POS, &pos);
        Obj_Value(id, VF_GET, OBJV_I_ROOMNUM, &room_number);
        AI_AddGoal(me_handle, AIG_GET_TO_POS, 2, 1.0, GOAL_UID_GETTOPOS, GF_NOTIFIES | GF_SPEED_FLEE | GF_NONFLUSHABLE,
                   &pos, room_number);
        /*
        AI_AddGoal(me_handle, AIG_FIRE_AT_OBJ, ACTIVATION_BLEND_LEVEL, 1.0f, -1, GF_ORIENT_TARGET |
                                GF_SPEED_ATTACK |
                                GF_KEEP_AT_COMPLETION |
                                GF_OBJ_IS_TARGET,
                                NULL, GST_NEG_FVEC);
        */

        //	mprintf(0, "BStorm: Hide to point %d\n", memory->cur_snipe_point+1);
      }
    } else {
      // bot will track you with no snipe points..
      memory->timer = Game_GetTime() + 3.0f;
      dist = 80.0f + ((float)rand() / (float)RAND_MAX) * 10.0f;
      AI_AddGoal(me_handle, AIG_MOVE_RELATIVE_OBJ, 1, 1.0f, -1, GF_OBJ_IS_TARGET | GF_KEEP_AT_COMPLETION,
                 OBJECT_HANDLE_NONE);
      AI_GoalValue(me_handle, 1, VF_SET, AIGV_F_CIRCLE_DIST, &dist);
    }
    break;

  case STATE_SNIPE:
    mprintf(0, "Black Stormtrooper set to snipe mode.\n");

    // AI_SafeSetType(me_handle, AIT_EVADER1);
    ///*
    FlushGoal(me_handle, 2);

    //		dist = 80.0f + ((float)rand()/(float)RAND_MAX)*10.0f;
    AI_AddGoal(me_handle, AIG_MOVE_RELATIVE_OBJ, 1, 1.0f, -1, GF_OBJ_IS_TARGET | GF_KEEP_AT_COMPLETION,
               OBJECT_HANDLE_NONE);
    //		AI_GoalValue(me_handle, 1, VF_SET, AIGV_F_CIRCLE_DIST, &dist);
    //*/

    /*
    AI_AddGoal(me_handle, AIG_FIRE_AT_OBJ, ACTIVATION_BLEND_LEVEL, 1.0f, -1, GF_ORIENT_TARGET |
                                            GF_SPEED_ATTACK |
                                            GF_KEEP_AT_COMPLETION |
                                            GF_OBJ_IS_TARGET,
                                            NULL, GST_NEG_FVEC);
    */

    memory->timer = Game_GetTime() + 8.0f;
    //	mprintf(0, "BStorm: Snipe!\n");
    break;
  }

  memory->state = state;
}

//////////////////////////////////////////////////////////////////////////////
//	aiCreeper

//	ai base class
aiCreeper::aiCreeper() { memory = NULL; }

aiCreeper::~aiCreeper() {}

void aiCreeper::OnInit(int me_handle) {
  tOSIRISMEMCHUNK ch;
  int flags;

  ch.id = 4;
  ch.size = sizeof(aiCreeper::t_creep_memory);
  ch.my_id.type = OBJECT_SCRIPT;
  ch.my_id.objhandle = me_handle;

  memory = (aiCreeper::t_creep_memory *)Scrpt_MemAlloc(&ch);

  //	reset timer for robot.
  memory->timer = 0;

  // initialize the creeper eye
  memory->eye_obj = CreateAndAttach(me_handle, "CreeperEye", OBJ_ROBOT, 0, 0);
  memory->eye_timer = 0.0f;

  // get sniper points available for trooper.		 n
  memory->state = -1;
  memory->energy = 10;
  memory->hits = 50;

  AI_Value(me_handle, VF_GET, AIV_I_FLAGS, &flags);
  memory->melee_flags = (ubyte)(flags & (AIF_MELEE1 | AIF_MELEE2));

  // set initial state of creeper
  set_state(me_handle, STATE_IDLE);
}

void aiCreeper::OnDamaged(int me_handle, tOSIRISEVTDAMAGED *evt) {
  if (memory->state == STATE_ATTACK) {
    if (memory->hits > -25) {
      memory->hits = memory->hits - (sbyte)evt->damage;
      //	mprintf(0, "[Creeper] hits = %d\n", memory->hits);
    }
    if (memory->hits < 0) {
      set_state(me_handle, STATE_STALK);
    }
  }
}

void aiCreeper::OnDestroy(int me_handle, tOSIRISEVTDESTROY *evt) {
  msafe_struct mstruct;

  mstruct.objhandle = memory->eye_obj;
  mstruct.killer_handle = OBJECT_HANDLE_NONE;
  mstruct.damage_type = GD_SCRIPTED;
  mstruct.amount = 1000.0f;

  MSafe_CallFunction(MSAFE_OBJECT_DAMAGE_OBJECT, &mstruct);
}

void aiCreeper::OnInterval(tOSIRISEventInfo *data) {
  int flags;

  switch (memory->state) {
  case STATE_IDLE:
  case STATE_STALK:
    if (memory->timer < Game_GetTime()) {
      if (memory->energy < 10) {
        memory->energy += (memory->state == STATE_IDLE) ? 2 : 1;
        memory->timer = Game_GetTime() + 1.0f;
        if (memory->energy > 10) {
          memory->energy = 10;
        }
      }
      if (memory->hits < 25) {
        memory->hits += (memory->state == STATE_IDLE) ? 10 : 7;
      }

      if (memory->state == STATE_STALK && memory->eye_timer != 0.0f) {
        // now cloak rest of body 1 sec after eye cloaked.
        msafe_struct m;
        m.objhandle = data->me_handle;
        m.lifetime = memory->eye_timer;
        m.state = 1;

        memory->eye_timer = 0.0f;

        MSafe_CallFunction(MSAFE_OBJECT_CLOAK, &m);
      }
    }

    if (memory->state == STATE_STALK && memory->eye_timer == 0.0f) {
      if (!Obj_IsEffect(data->me_handle, EF_FADING_OUT | EF_CLOAKED)) {
        set_state(data->me_handle, STATE_ATTACK);
      }
    }
    break;

  case STATE_ATTACK:
    if (memory->timer < Game_GetTime()) {
      // recloak and recharge.
      set_state(data->me_handle, STATE_STALK);
    } else {
      flags = AIF_MELEE2;
      if (memory->energy <= 5 && (memory->melee_flags & AIF_MELEE2)) {
        AI_Value(data->me_handle, VF_CLEAR_FLAGS, AIV_I_FLAGS, &flags);
        memory->melee_flags &= (~AIF_MELEE2);
      } else if (memory->energy > 5 && !(memory->melee_flags & AIF_MELEE2)) {
        AI_Value(data->me_handle, VF_SET_FLAGS, AIV_I_FLAGS, &flags);
        memory->melee_flags |= AIF_MELEE2;
      }
    }
  }
}

bool aiCreeper::OnNotify(int me_handle, tOSIRISEVTAINOTIFY *data) {
  vector pos;
  int room_number;

  switch (data->notify_type) {
  case AIN_SEE_TARGET:
  case AIN_NEAR_TARGET:
    // rules: leave's base state if recharged enough-->cloaks
    if (memory->state == STATE_IDLE) {
      set_state(me_handle, STATE_STALK);
    }
    //	mprintf(0, "[Creeper] see notify %d\n", data->notify_type);
    break;

  case AIN_GOAL_COMPLETE:
  case AIN_PLAYER_SEES_YOU:
  case AIN_ANIM_COMPLETE:
  case AIN_BUMPED_OBJ:
    break;

  case AIN_MELEE_ATTACK_FRAME:
    // reduce energy accordingly depending on attack.
    if (data->attack_num == 1) {
      // blast ring attack
      int wpnid;

      Obj_Value(me_handle, VF_GET, OBJV_I_ROOMNUM, &room_number);
      Obj_GetGunPos(me_handle, 0, &pos);

      wpnid = Wpn_FindID("CreeperBlastRing");
      Obj_Create(OBJ_WEAPON, wpnid, room_number, &pos, NULL, me_handle);

      memory->energy -= 5;
      //	mprintf(0, "[Creeper] Special attack.\n");
    } else {
      // standard attack
      memory->energy -= 3;
    }

    if (memory->energy < 3) {
      set_state(me_handle, STATE_STALK);
    }
    break;

    // default:
    //	mprintf(0, "[Creeper] notify %d\n", data->notify_type);
  }

  return true;
}

void aiCreeper::set_state(int me_handle, int state) {
  int room_number = 0, id, i, flags;
  vector pos;
  char buf[64];
  char name[48];
  msafe_struct m;
  float circle_dist;
  char cenable;

retry_set_state:
  switch (state) {
  case STATE_IDLE:
    // stay at one position until ready.  note that if attacked , it will flee in this state.
    FlushGoal(me_handle, 2);

    memory->timer = Game_GetTime() + 1.0f;

    Obj_Value(me_handle, VF_GET, OBJV_V_POS, &pos);
    Obj_Value(me_handle, VF_GET, OBJV_I_ROOMNUM, &room_number);

    AI_AddGoal(me_handle, AIG_GET_TO_POS, 1, 1.0f, -1, GF_KEEP_AT_COMPLETION, &pos, room_number);
    //		mprintf(0, "[Creeper] Idle\n");
    break;

  case STATE_STALK:
    // cloak and stalk player.
    //	FlushGoal(me_handle, 3);				// take out any priority goals set.
    FlushGoal(me_handle, 2); // take out any goals previously set

    memory->timer = Game_GetTime() + 1.0f; // set timer for recharge!
                                           //		memory->timer = Game_GetTime() + 1.0f + (rand()%4) * 0.5f;

    m.objhandle = memory->eye_obj;
    m.lifetime = 5.0f + (rand() % 20) * 0.5f;
    m.state = 1;

    memory->eye_timer = m.lifetime;

    MSafe_CallFunction(MSAFE_OBJECT_CLOAK, &m);

    circle_dist = 40.0f;
    AI_Value(me_handle, VF_SET, AIV_F_CIRCLE_DIST, &circle_dist);
    //		AI_AddGoal(me_handle, AIG_WANDER_AROUND, 3, 1.0f, -1, GF_ORIENT_VELOCITY | GF_NOTIFIES |
    // GF_KEEP_AT_COMPLETION, &pos, room_number);
    AI_AddGoal(me_handle, AIG_MOVE_RELATIVE_OBJ, 2, 1.0f, -1, GF_KEEP_AT_COMPLETION | GF_OBJ_IS_TARGET, NULL, 0);

    flags = AIF_DISABLE_MELEE;
    AI_Value(me_handle, VF_SET_FLAGS, AIV_I_FLAGS, &flags);

    cenable = 1;
    Obj_Value(me_handle, VF_SET, OBJV_C_NEGATIVE_LIGHT, &cenable);
    //		mprintf(0, "[Creeper] Stalk\n");
    break;

  case STATE_ATTACK:
    //	FlushGoal(me_handle, 3);				// we've decloaked, attack.
    FlushGoal(me_handle, 2);

    memory->timer = Game_GetTime() + 4.0f + (float)(rand() % 3);
    flags = AIF_DISABLE_MELEE;
    AI_Value(me_handle, VF_CLEAR_FLAGS, AIV_I_FLAGS, &flags);

    cenable = 1;
    Obj_Value(me_handle, VF_SET, OBJV_C_NEGATIVE_LIGHT, &cenable);
    //		mprintf(0, "[Creeper] Attack\n");
    break;
  }

  memory->state = state;
}

//	aiLukeTurret
//		Luke's funky turret

void aiLukeTurret::OnInit(int me_handle) {
  tOSIRISMEMCHUNK ch;

  ch.id = 4;
  ch.size = sizeof(aiLukeTurret::t_lturret_memory);
  ch.my_id.type = OBJECT_SCRIPT;
  ch.my_id.objhandle = me_handle;

  memory = (aiLukeTurret::t_lturret_memory *)Scrpt_MemAlloc(&ch);
  memory->timer = -1.0f;

  // turrets only activate when they 'hear' a player fire a weapon.
  int flags = AIF_DISABLE_FIRING;

  AI_SetType(me_handle, AIT_AIS);
  AI_Value(me_handle, VF_SET_FLAGS, AIV_I_FLAGS, &flags);
}

bool aiLukeTurret::OnNotify(int me_handle, tOSIRISEVTAINOTIFY *data) {
  int flags;

  switch (data->notify_type) {
  case AIN_OBJ_FIRED:
    // set 5 seconds for turret awareness before it becomes inert.
    memory->timer = Game_GetTime() + 12.0f;
    AI_SetType(me_handle, AIT_STATIONARY_TURRET);
    flags = AIF_DISABLE_FIRING;
    AI_Value(me_handle, VF_CLEAR_FLAGS, AIV_I_FLAGS, &flags);
    break;
  }

  return true;
}

void aiLukeTurret::OnInterval(tOSIRISEventInfo *data) {
  if (memory->timer == -1.0f)
    return;

  if (memory->timer < Game_GetTime()) {
    int flags = AIF_DISABLE_FIRING;
    AI_SetType(data->me_handle, AIT_AIS);
    AI_Value(data->me_handle, VF_SET_FLAGS, AIV_I_FLAGS, &flags);
    memory->timer = -1.0f;
  }
}

//	aiSTBlackBarrel
//		Black Stormtrooper's ammo pack

void aiSTBlackBarrel::OnInit(int me_handle) {
  tOSIRISMEMCHUNK ch;

  ch.id = 4;
  ch.size = sizeof(aiSTBlackBarrel::t_stblackbarrel_memory);
  ch.my_id.type = OBJECT_SCRIPT;
  ch.my_id.objhandle = me_handle;

  memory = (aiSTBlackBarrel::t_stblackbarrel_memory *)Scrpt_MemAlloc(&ch);

  Obj_Value(me_handle, VF_GET, OBJV_M_ORIENT, &memory->orient);
}

void aiSTBlackBarrel::OnDestroy(int me_handle, tOSIRISEVTDESTROY *evt) {
  // Create a frag burst effect
  vector pos, new_dir;
  int room;
  matrix orient, new_orient;

  Obj_Value(me_handle, VF_GET, OBJV_V_POS, &pos);
  Obj_Value(me_handle, VF_GET, OBJV_I_ROOMNUM, &room);
  Obj_Value(me_handle, VF_GET, OBJV_M_ORIENT, &orient);

  new_dir = -orient.fvec;
  vm_VectorToMatrix(&new_orient, &new_dir, NULL, NULL);

  Obj_Create(OBJ_WEAPON, Wpn_FindID("AlienBossFragBurst"), room, &pos, &new_orient, me_handle);

  // mprintf(0,"Barrel destroyed.\n");
}

void aiSTBlackBarrel::OnDamaged(int me_handle, tOSIRISEVTDAMAGED *evt) {
  // mprintf(0,"Barrel damaged...\n");
}
