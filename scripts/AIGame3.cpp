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

// AIGame3.cpp
// 0.1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "osiris_import.h"
#include "osiris_common.h"
#include "osiris_vector.h"
#include "DallasFuncs.cpp"

#include "AIGame3_External.h"

#include "module.h"
#include "psrand.h"

#ifdef __cplusplus
extern "C" {
#endif
DLLEXPORT char STDCALL InitializeDLL(tOSIRISModuleInit *func_list);
DLLEXPORT void STDCALL ShutdownDLL(void);
DLLEXPORT int STDCALL GetGOScriptID(const char *name, uint8_t isdoor);
DLLEXPORT void STDCALLPTR CreateInstance(int id);
DLLEXPORT void STDCALL DestroyInstance(int id, void *ptr);
DLLEXPORT int16_t STDCALL CallInstanceEvent(int id, void *ptr, int event, tOSIRISEventInfo *data);
DLLEXPORT int STDCALL SaveRestoreState(void *file_ptr, uint8_t saving_state);
#ifdef __cplusplus
}
#endif

static int String_table_size = 0;
static char **String_table = NULL;
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

//-------------------
// Function prototypes
//-------------------

static int TurnOnSpew(int objref, int gunpoint, int effect_type, float mass, float drag, int gravity_type, uint8_t isreal,
                      float lifetime, float interval, float longevity, float size, float speed, uint8_t random);
// Returns the new child's handle
static int CreateAndAttach(int me, const char *child_name, uint8_t child_type, char parent_ap, char child_ap,
                           bool f_aligned = true, bool f_set_parent = false);

static int FindClosestPlayer(int objhandle);

// Returns true if object current has an active Dallas low priority goal
static bool HasLowPriorityGoal(int obj_handle);

// Returns true if object current has an active Dallas high priority goal
static bool HasHighPriorityGoal(int obj_handle);

// Wipes out all goals except slots 0 and 3 (used by Dallas)
static void SafeGoalClearAll(int obj_handle);

static void AI_SafeSetType(int obj_handle, int ai_type);

//----------------
// Name lookups
//----------------

// Name lookup globals
static uint16_t energy_effect_id;       // weapon ID for the energy charge effect
static uint16_t frag_burst_effect_id;   // weapon ID for the frag burst effect
static uint16_t boss_frag_burst_id;     // weapon ID for the boss frag burst effect
static uint16_t transfer_effect_id;     // texture ID for the energy transfer lightning effect
static uint16_t heal_effect_id;         // texture ID for the heal lightning effect
static uint16_t boss_heal_effect_id;    // texture ID for the boss heal lightning effect
static uint16_t tractor_beam_effect_id; // texture ID for the tractor beam effect
static uint16_t alien_organism_id;      // object type ID for the alien organism robot
static uint16_t shield_blast_id;        // weapon ID for the HT shield blast effect
static uint16_t ht_grenade_id;          // weapon ID for the HT grenade
static uint16_t ht_grenade_effect_id;   // weapon ID for the HT grenade launch effect
static uint16_t lifter_blast_effect_id; // weapon ID for the lifter blast effect
static uint16_t lifter_stick_effect_id; // texture ID for lifter's night-stick lightning effect
static uint16_t teleport_effect_id;     // weapon ID for teleporting effect

static uint16_t ht_grenade_sound_id; // sound ID for firing the grenade

static uint16_t powerup_id; // invisible powerup id

static uint16_t boss_flapping_id; // flapping sound id
static uint16_t boss_turf_id;     // turf id
static uint16_t boss_see_id;
static uint16_t boss_hurt_id;

static uint16_t lifter_pull_sound_id;
static uint16_t lifter_amb_sound_id;

// ==========================
// AI Goal Related Functions
// ==========================

#define DALLAS_LOW_PRIORITY_GOAL_SLOT 0
#define DALLAS_HIGH_PRIORITY_GOAL_SLOT 3

// Returns true if object current has an active Dallas low priority goal
bool HasLowPriorityGoal(int obj_handle) {
  bool used;
  AI_GoalValue(obj_handle, DALLAS_LOW_PRIORITY_GOAL_SLOT, VF_GET, AIGV_B_USED, &used);

  return used;
}

// Returns true if object current has an active Dallas high priority goal
bool HasHighPriorityGoal(int obj_handle) {
  bool used;
  AI_GoalValue(obj_handle, DALLAS_HIGH_PRIORITY_GOAL_SLOT, VF_GET, AIGV_B_USED, &used);

  return used;
}

// Returns True if given index implies that a goal is finished
static inline bool IsGoalFinishedNotify(int index) {
  return (index == AIN_GOAL_COMPLETE || index == AIN_GOAL_INVALID || index == AIN_GOAL_FAIL || index == AIN_GOAL_ERROR);
}

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

// ============================
// Miscellaneous Utility Funcs
// ============================

int TurnOnSpew(int objref, int gunpoint, int effect_type, float mass, float drag, int gravity_type, uint8_t isreal,
               float lifetime, float interval, float longevity, float size, float speed, uint8_t random) {
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

// Returns the new child's handle
int CreateAndAttach(int me, const char *child_name, uint8_t child_type, char parent_ap, char child_ap, bool f_aligned,
                    bool f_set_parent) {
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

int FindClosestPlayer(int objhandle) {
  vector objpos, playerpos;
  float closest_dist = FLT_MAX;
  int closest_player = OBJECT_HANDLE_NONE;
  msafe_struct mstruct;

  // Get the object position
  mstruct.objhandle = objhandle;
  MSafe_GetValue(MSAFE_OBJECT_POS, &mstruct);
  objpos = mstruct.pos;

  // Loop though all possible players and compute distance
  for (int p = 0; p < MAX_PLAYERS; p++) {

    // Get player position
    mstruct.slot = p;
    MSafe_GetValue(MSAFE_OBJECT_PLAYER_HANDLE, &mstruct);

    // See if this player active
    if (mstruct.objhandle == OBJECT_HANDLE_NONE)
      continue;

    // Get the target position
    MSafe_GetValue(MSAFE_OBJECT_POS, &mstruct);

    // Get the normalized vector from the source to the target
    float dist = vm_VectorDistanceQuick(&objpos, &mstruct.pos);

    if (dist < closest_dist) {
      closest_dist = dist;
      closest_player = mstruct.objhandle;
    }
  }

  return closest_player;
}

// ==================
// Clear Shot System
// ==================

// Clear shot constants
#define MAX_SCAN_SHOT_OBJECTS 30
#define MAX_SHOT_PATH_POSITIONS 256

// Reasons why not a clear shot
#define CS_ALL_CLEAR 0
#define CS_SHOOTER_IN_WAY 1
#define CS_FRIEND_IN_WAY 2
#define CS_NO_ENEMY 3
#define CS_NO_PATH 4

// Shot data structure
struct tShotData {
  int object_handle;   // IN: the object taking the shot
  vector start_point;  // IN: the starting point of the shot
  int start_room;      //	IN: the room that the starting point is in
  vector dir;          //	IN: the direction the shot is aimed
  float max_dist;      //	IN: the maximum distance of the shot
  float shot_radius;   // IN: the radius of the projectile (used for the ray thickness)
  float danger_radius; // IN: the initial danger radius for the scans
  float radius_inc;    // IN: the danger radius increase multiplier
  float risk_factor;   // IN: the amount of risk a robot is willing to take
};

// Shot scan position data structure
struct tShotPathPositionData {
  int num_friends;       // Number of friends found at this path position
  int num_enemies;       // Number of enemies found at this path position
  int num_neutrals;      // Number of neutrals found at this path position
  bool contains_shooter; // Whether or not this path position contains the shooter
  float path_dist;       // The shot path distance of this scan position from the start
  float scan_radius;     // The radius of the scan that was done at this shot position
};

// Clear shot globals
static tShotPathPositionData ShotPathPositions[MAX_SHOT_PATH_POSITIONS];
static int num_shot_path_positions;

static bool ScanShotPathPosition(int obj_handle, vector *pos, int room, float radius, float dist);
static float TraverseShotPath(tShotData *shot_data);
static int ShotIsClear(float risk_factor);
static int HasClearShot(tShotData *shot_data);

// Scans area around given shot position for object data
bool ScanShotPathPosition(int obj_handle, vector *pos, int room, float radius, float dist) {
  int scan_objs[MAX_SCAN_SHOT_OBJECTS];
  int n_scan;
  int n, i;
  int type;

  // Make sure there is still a shot scan position available
  if (num_shot_path_positions >= MAX_SHOT_PATH_POSITIONS) {
    return false;
  }

  // Init this shot path position's data
  n = num_shot_path_positions;
  ShotPathPositions[n].contains_shooter = false;
  ShotPathPositions[n].path_dist = dist;
  ShotPathPositions[n].scan_radius = radius;
  ShotPathPositions[n].num_enemies = 0;
  ShotPathPositions[n].num_friends = 0;
  ShotPathPositions[n].num_neutrals = 0;

  // Scan for any nearby objects
  // NOTE: check the func below to see what other params do!!!
  n_scan = AI_GetNearbyObjs(pos, room, radius, scan_objs, MAX_SCAN_SHOT_OBJECTS, false, true, false, true);

  // Go through the objects that have been found and store tallies
  for (i = 0; i < n_scan; i++) {
    Obj_Value(scan_objs[i], VF_GET, OBJV_I_TYPE, &type);
    if (type == OBJ_ROBOT || type == OBJ_PLAYER) {
      // Make sure it's not cloaked
      if (qObjectCloakTime(scan_objs[i]) == 0.0f) {
        // Tallie the object accordingly
        if (scan_objs[i] == obj_handle) {
          // It's the shooter
          ShotPathPositions[n].contains_shooter = true;
        } else if (AI_IsObjEnemy(obj_handle, scan_objs[i])) {
          // It's an enemy
          ShotPathPositions[n].num_enemies++;
        } else if (AI_IsObjFriend(obj_handle, scan_objs[i])) {
          // It's a friend
          ShotPathPositions[n].num_friends++;
        } else {
          // It's a neutral
          ShotPathPositions[n].num_neutrals++;
        }
      }
    }
  }
  num_shot_path_positions++;

  return true;
}

// Traverses the straight-line shot path and gathers object data
float TraverseShotPath(tShotData *shot_data) {
  float next_scan_dist, scan_radius;
  float current_path_dist, dist;
  vector start_pos, end_pos, target_pos;
  vector shot_dir;
  int start_room, end_room;
  int flags, fate;
  ray_info ray;
  bool done, max_distance_reached;

  // Clear out any existing shot scan positions
  num_shot_path_positions = 0;

  // Setup the shot's starting data
  start_pos = shot_data->start_point;
  start_room = shot_data->start_room;
  shot_dir = shot_data->dir;
  next_scan_dist = shot_data->danger_radius;
  scan_radius = shot_data->danger_radius;

  // Start scanning
  current_path_dist = 0.0f;
  max_distance_reached = false;
  done = false;

  while (!done) {
    // Check the max shot distance
    if ((current_path_dist + next_scan_dist) >= shot_data->max_dist) {
      next_scan_dist = shot_data->max_dist - current_path_dist;
      max_distance_reached = true;
    }

    // Calculate shot's target position
    target_pos = start_pos + (shot_dir * next_scan_dist);

    // Cast a ray to see if a wall or any clutter objects lie in the target path
    flags = FQ_CHECK_OBJS | FQ_IGNORE_POWERUPS | FQ_IGNORE_WEAPONS | FQ_IGNORE_MOVING_OBJECTS |
            FQ_IGNORE_NON_LIGHTMAP_OBJECTS;
    fate =
        FVI_RayCast(shot_data->object_handle, &start_pos, &target_pos, start_room, shot_data->shot_radius, flags, &ray);

    // Store the ray hit point and room
    end_pos = ray.hit_point;
    end_room = ray.hit_room;

    // If necessary, calculate the distance from start point to the hit point
    if (fate == HIT_NONE) {
      dist = next_scan_dist;
    } else {
      dist = vm_VectorDistance(&start_pos, &end_pos);
    }

    // Add the new distance to our total shot path distance
    current_path_dist += dist;

    // If it hit a wall, change the direction (perfect reflection)
    if (fate == HIT_WALL) {
      // Calculate the bounce dir from the current dir and face normal
      vector new_dir = (-2.0f * (shot_dir * ray.hit_wallnorm)) * ray.hit_wallnorm + shot_dir;
      shot_dir = new_dir;

      // Now, if necessary, cast another ray to finish off the scan dist
      next_scan_dist -= dist;
      if (next_scan_dist > 0.0f) {
        start_pos = end_pos;
        start_room = end_room;
        continue;
      }
    }

    // Do an object scan at this point
    if (!ScanShotPathPosition(shot_data->object_handle, &end_pos, end_room, scan_radius, current_path_dist)) {
      done = true;
      continue;
    }

    // Obj_Create(OBJ_WEAPON, energy_effect_id, end_room, &end_pos, NULL, shot_data->object_handle);

    // If max distance was reached or we hit a clutter object, we're done
    if (max_distance_reached || fate == HIT_OBJECT) {
      done = true;
      continue;
    }

    // Update data for next run
    next_scan_dist = scan_radius * 2.0f;
    scan_radius *= shot_data->radius_inc;
    start_pos = end_pos;
    start_room = end_room;
  }

  return true;
}

// Analyzes data to determine if shot is clear
int ShotIsClear(float risk_factor) {
  int j;

  if (num_shot_path_positions == 0)
    return CS_NO_PATH;

  // If final shot path position contains shooter, don't shoot!
  if (ShotPathPositions[num_shot_path_positions - 1].contains_shooter)
    return CS_SHOOTER_IN_WAY;

  for (j = 0; j < num_shot_path_positions; j++) {
    // Check and see if shooter is in the way (ok if it's the first position)
    if (ShotPathPositions[j].contains_shooter && j != 0)
      return CS_SHOOTER_IN_WAY;

    // Check if there are any friendlies or neutrals in the way
    if ((ShotPathPositions[j].num_friends + ShotPathPositions[j].num_neutrals) > 0)
      return CS_FRIEND_IN_WAY;

    // Check if any enemies are in the way
    if (ShotPathPositions[j].num_enemies > 0 && !ShotPathPositions[j].contains_shooter)
      return CS_ALL_CLEAR;
  }

  return CS_NO_ENEMY;
}

// Checks to see if object has a clear shot
int HasClearShot(tShotData *shot_data) {
  // Traverse the shot trajectory, gathering data
  TraverseShotPath(shot_data);

  // Analyze the shot data
  return (ShotIsClear(shot_data->risk_factor));
}

// ==========================
// Script class definitions
// ==========================
#define NUM_IDS 6 // maximum number of IDs

#define ID_ALIENORGANISM 1  // Alien organism robot
#define ID_HEAVYTROOPER 2   // CED Heavy Trooper robot
#define ID_LIFTER 3         // Lifter robot
#define ID_ALIENBOSS 4      // Alien Mini-boss
#define ID_SECURITYCAMERA 5 // Security Camera
#define ID_CROWDCONTROL 6   // Crowd Control for Merc6 Barges

struct tScriptInfo {
  int id;
  const char *name;
};

static tScriptInfo ScriptInfo[NUM_IDS] = {
    {ID_ALIENORGANISM, "AlienOrganism"}, {ID_HEAVYTROOPER, "HeavyTrooper"},     {ID_LIFTER, "Lifter"},
    {ID_ALIENBOSS, "AlienBoss"},         {ID_SECURITYCAMERA, "SecurityCamera"}, {ID_CROWDCONTROL, "CrowdControl"}};

class BaseObjScript {
public:
  BaseObjScript();
  ~BaseObjScript();
  virtual int16_t CallEvent(int event, tOSIRISEventInfo *data);
};

// Priority Constants
#define LOW_PRIORITY 0
#define MEDIUM_PRIORITY 1
#define HIGH_PRIORITY 2

//---------------------
// AlienOrganism class
//---------------------

// Alien Mode Types
#define ALIEN_LANDED 0
#define ALIEN_LANDED_RESTING 1
#define ALIEN_LANDED_HEALING 2
#define ALIEN_LANDED_DEPOSITING 3
#define ALIEN_LANDED_WITHDRAWING 4

#define ALIEN_LANDING_AT_HOME 10
#define ALIEN_GOING_HOME 11
#define ALIEN_FLEEING 12
#define ALIEN_FINDING_BIRTHPLACE 13
#define ALIEN_GIVING_BIRTH 14

#define ALIEN_HUNTING 20
#define ALIEN_SCAVENGING 21

#define ALIEN_ATTACKING_MELEE 31
#define ALIEN_ATTACKING_RANGED 32

#define ALIEN_MODE_NONE 50

// General Alien Constants
#define ALIEN_MEMORY_ID 4
#define ALIEN_TYPE_NAME "AlienOrganism"

#define ALIEN_MAX_ENERGY_CHARGES 20.0f
#define ALIEN_ENERGY_SUCK_CHARGE 20.0f
#define ALIEN_ENERGY_BOLT_COST 2.0f

#define ALIEN_HEAL_PERCENTAGE 0.50f
#define ALIEN_LANDED_ACTION_TIME 2.0f

#define ALIEN_BEAM_UPDATE_TIME 0.5f
#define ALIEN_SQUAD_RECRUIT_RADIUS 100.0f

#define ALIEN_MAX_TAKEOFF_TIME 0.7f

// Alien Lookup Names
#define ALIEN_ENERGY_EFFECT_NAME "AlienEnergyEffect"
#define ALIEN_TRANSFER_EFFECT_NAME "FunkyEffect1"
#define ALIEN_HEAL_EFFECT_NAME "FunkyEffectGreen"

// Alien Flee Constants
#define ALIEN_ON_FIRE_FLEE_CHANCE 80
#define ALIEN_NEAR_FIRE_FLEE_CHANCE 25
#define ALIEN_SQUADIE_FLEE_SAVING_THROW 5
#define ALIEN_LEADER_FLEE_SAVING_THROW 10

// Alien goal complete IDs
#define ALIEN_GUID_GOT_HOME 0x10100001
#define ALIEN_GUID_LANDED 0x10100002
#define ALIEN_GUID_FOUND_BIRTHPLACE 0x10100003
#define ALIEN_GUID_RAN_AWAY 0x10100004
#define ALIEN_GUID_GOT_TO_DEST_OBJ 0x10100005

// Alien Communication IDs
#define ALIEN_COM_JOIN_ME 0
#define ALIEN_COM_LEAVE_ME 1
#define ALIEN_COM_PROTECT_ME 2
#define ALIEN_COM_BREAK_AND_ATTACK 3
#define ALIEN_COM_REGROUP 4
#define ALIEN_COM_SET_MODE 5
#define ALIEN_COM_SET_PENDING_MODE 6
#define ALIEN_COM_GET_MODE 7
#define ALIEN_COM_SET_SQUAD_FLAGS 8
#define ALIEN_COM_CLEAR_SQUAD_FLAGS 9

#define ALIEN_COM_REQUEST_LEAVE 10
#define ALIEN_COM_LEAVING_YOU 11
#define ALIEN_COM_GET_GOAL_POS 12
#define ALIEN_COM_GET_GOAL_ROOM 13
#define ALIEN_COM_I_WAS_HIT 14
#define ALIEN_COM_CAN_YOU_SEE_ME 15
#define ALIEN_COM_LEAVE_ME_FOR_MODE 16

// Squad Constants
#define ALIEN_MAX_TEAMMATES 2
#define ALIEN_CATCHUP_DIST 4.0f
#define ALIEN_APPROACH_DIST 10.0f
#define ALIEN_ENGAGE_DIST 200.0f
#define ALIEN_BREAKUP_DIST 50.0f
#define ALIEN_REGROUP_DIST 120.0f

// Alien Squad Flags
#define ALIEN_LEADER 0x01
#define ALIEN_SQUADIE 0x02
#define ALIEN_CATCHUP 0x04
#define ALIEN_BROKEN 0x08

// Energy Beam Flags
#define ALIEN_BEAM1 0x01
#define ALIEN_BEAM2 0x02
#define ALIEN_BEAM3 0x04

// Auto avoid friends distances
#define ALIEN_NORMAL_AVOID_DIST 4.0f
#define ALIEN_SQUAD_AVOID_DIST 0.0f

// Circle Distance (also for avoiding friends)
#define ALIEN_SQUAD_CIRC_DIST_MOD 0.4f

// Formation Offset Constants
#define ALIEN_FO_HUNT_RVEC 8.0f
#define ALIEN_FO_HUNT_FVEC 10.0f
#define ALIEN_FO_SCAV_RVEC 3.0f
#define ALIEN_FO_SCAV_FVEC 15.0f
#define ALIEN_FO_ATTK_RVEC 7.0f
#define ALIEN_FO_ATTK_UVECP 7.0f
#define ALIEN_FO_ATTK_UVECN 5.0f
#define ALIEN_FO_ATTK_FVEC 3.0f

// Speed Modifier Constants
#define ALIEN_ENGAGE_SPEED_MOD 1.4f
#define ALIEN_ATTACK_SPEED_MOD 1.8f
#define ALIEN_RETURN_SPEED_MOD 1.0f
#define ALIEN_LAND_SPEED_MOD 0.5f
#define ALIEN_HUNT_SPEED_MOD 1.0f
#define ALIEN_SCAV_SPEED_MOD 0.7f
#define ALIEN_FLEE_SPEED_MOD 2.0f

#define ALIEN_CATCHUP_SPEED_MOD 0.23f
#define ALIEN_SQUADIE_INTERVAL 0.25f

// Teammate data struct
struct teammate_data {
  int handle;      // the object handle for this teammate
  bool is_visible; // whether this teammate can see me or not
};

// Alien memory data structure
struct alienorganism_data {
  // Mode Data
  int mode;          // indicates what mode alien is currently in
  int pending_mode;  // mode to switch to when current task(s) are completed
  int previous_mode; // mode alien was in before starting current task(s)

  float mode_time;                // time spent in current mode
  float next_activity_time;       // next time resting alien should think about doing something
  float max_wander_time;          // maximum time to be spent out hunting/scavenging before returning
  float next_update_squad_time;   // the next time to do a squad update
  float next_update_energy_time;  // the next time to update the energy effect
  float next_update_beam_time;    // the next time to update the energy beams
  float next_generic_check_time;  // the next time to do generic checks
  float next_squadie_update_time; // the next time to update the squadie's catchup speed
  float next_special_damage_time; // the next time alien can take special damage
  float next_vis_check_time;      // the next time to do a squad visibility check

  // General Information
  float base_speed;           // the robot's base speed
  float base_circle_distance; // the robot's base circle distance
  float base_shields;         // the robot's base shields

  float energy_charges; // the number of energy charges currently stored
  float ok_to_deposit;  // indicates whether alien is allowed to deposit charge

  float takeoff_time; // time since takeoff (used for clearing axes lock, etc.)

  int dest_object_handle; // destination handle for object to go to instead of wander

  // Indicators
  bool done_turning;
  bool done_moving;
  bool hit_by_player;
  bool energy_beams_on;

  // Home Data
  vector home_pos;
  int home_room;
  vector home_fvec;
  vector home_uvec;

  // Energy effect props
  int pos1_handle;
  int pos2_handle;
  int pos3_handle;
  int enabled_beams;

  // Squad Data
  int squad_flags;
  int leader_handle;
  int num_teammates;
  teammate_data teammate[ALIEN_MAX_TEAMMATES];
  vector squad_goal_pos;
  int squad_goal_room;

};

// Alien Organism class definition
class AlienOrganism : public BaseObjScript {
private:
  alienorganism_data *memory;

  void SetMode(int me, char mode);
  void DoTakeoff(int me, float takeoff_speed, float speed_variance);

  void DoInit(int me_handle);
  void DoFrame(int me_handle);
  bool DoNotify(int me, tOSIRISEventInfo *data);
  void DoDamage(int me, tOSIRISEVTDAMAGED *damage_data);
  void DoCollide(int me, tOSIRISEVTCOLLIDE *collide_data);

  bool SendCommand(int me, int it, char command, void *ptr);
  bool ReceiveCommand(int me, int it, char command, void *ptr);

  bool IssueSquadOrder(int leader_handle, char command, void *ptr);

  bool IsLandedMode(int mode);
  bool IsEnergyRelatedLandedMode(int mode);
  bool ModeIsAttackSensitive(int mode, int pending_mode);
  bool ModeSupportsSquadEnlistment(int mode, int pending_mode, int recruiter_mode);
  bool ModeSupportsSquadRecruitment(int mode);
  bool ModeSupportsSquadMovement(int mode);
  void DisperseSquad(int me);
  void DisperseSquadToMode(int me, int mode);
  bool LeaveLeader(int me);

  bool Flee(int me, bool force_flee = false);
  bool Heal(int me);
  bool MeleeAttack(int me, bool force_attack = false);
  bool RangedAttack(int me);
  bool Deposit(int me);
  bool Withdraw(int me);

  bool WillJoinLeader(int me, int leader_handle, int priority);

  int FindTeammateID(int tm_handle);
  int CalcFormationPosition(int me, int tm_ID, vector *pos);
  void SetSquadieFormationGoal(int me);
  void UpdateSquadieFormationGoal(int me);
  void SetWanderGoal(int me);
  void RemoveTeammate(int tm_id);

  bool IsNearFire(int me);
  bool IsOnFire(int me);

  bool FindHome(int me);
  void ProjectBeam(int me, int pos_handle, vector *start_pos, int start_room, vector *beam_dir, matrix *orient,
                   int beam_flag);
  void CalcEnergyBeamPositions(int me);
  void UpdateEnergyBeams(int me);
  void UpdateEnergyEffect(int me);
  void SetMaxSpeed(int me, float speed);
  void SetCircleDist(int me, float dist);

  void UpdateSquadList(int me);
  void UpdateSquad(int me);
  void UpdateSquadVisibility(int me);
  void DoSquadieFrame(int me);

public:
  AlienOrganism() {}
  int16_t CallEvent(int event, tOSIRISEventInfo *data);
};

//---------------------
// Heavy Trooper class
//---------------------

// Heavy Trooper Mode Types
#define HT_ATTACK_NORMAL 0
#define HT_PREPARING_GRENADE 1
#define HT_FIRING_GRENADE 2
#define HT_RECOVERING 3

#define HT_PREPARING_FOR_CURL 10
#define HT_CURLING_UP 11
#define HT_UNCURLING 12
#define HT_ARMORED_ATTACK 13
#define HT_ARMORED_CIRCLE_BACK 14

// General Heavy Trooper Constants
#define HT_MEMORY_ID 4

#define HT_PARTICLE_SPEW_TIME 0.4f
#define HT_MAX_PARTICLE_TIME 0.5f

#define HT_CURLUP_DIST 10.0f
#define HT_CURLUP_DIST_VARIANCE 20.0f
#define HT_CURLUP_DAMAGE 0.2f

#define HT_GRENADE_DIST 40.0f
#define HT_GRENADE_DIST_MAX 130.0f

#define HT_RECHARGE_TIME 1.0f
#define HT_DECHARGE_TIME 0.8f
#define HT_MAX_CHARGE 8.0f
#define HT_MAX_CHARGE_VARIANCE 6.0f
#define HT_CHARGE_INC 1.0f

#define HT_CIRCLE_BACK_TIME 1.0f

#define HT_SHIELD_BLAST_DELAY 1.5f
#define HT_ARMOR_DAMAGED_MOD 0.02f
#define HT_CURLING_DAMAGED_MOD 0.2f

#define HT_GRENADE_CHECK_INTERVAL 1.0f
#define HT_GRENADE_CHECK_VARIANCE 1.5f
#define HT_GRENADE_RELOAD_TIME 3.0f

#define HT_GRENADE_GUNPOINT 2

// Heavy Trooper Custom Anim Frames
#define HT_CURLUP_START_FRAME 202.0f
#define HT_CURLUP_END_FRAME 216.0f
#define HT_CURLUP_ANIM_TIME 2.0f

#define HT_UNCURL_START_FRAME 146.0f
#define HT_UNCURL_END_FRAME 173.0f
#define HT_UNCURL_ANIM_TIME 3.5f

#define HT_ROLL_START_FRAME 185.0f
#define HT_ROLL_END_FRAME 195.0f
#define HT_ROLL_ANIM_TIME 0.7f

#define HT_GUNFIRE_START_FRAME 95.0f
#define HT_GUNFIRE_END_FRAME 105.0f

#define HT_GRENADE_START_FRAME 105.0f
#define HT_GRENADE_FIRE_FRAME 119.0f
#define HT_GRENADE_END_FRAME 126.0f

#define HT_GRENADE_FIRE_ANIM_TIME 1.2f
#define HT_GRENADE_RECOIL_ANIM_TIME 1.3f
#define HT_GRENADE_MISFIRE_ANIM_TIME 1.0f

// Heavy Trooper Speed modifiers
#define HT_GRENADE_FIRING_SPEED_MOD 0.2f
#define HT_GRENADE_AIMING_SPEED_MOD 0.6f
#define HT_CURLING_SPEED_MOD 0.8f
#define HT_RAMMING_SPEED_MOD 2.5f
#define HT_CIRCLE_BACK_SPEED_MOD 2.5f

#define HT_RAMMING_DELTA_SPEED_MOD 3.0f

#define HT_RAMMING_TURN_RATE_MOD 1.1f
#define HT_RAMMING_DELTA_TURN_RATE_MOD 3.0f

// Heavy Trooper ID's
#define HT_SHIELD_BLAST_NAME "HTShieldBlast"
#define HT_GRENADE_NAME "Impact Mortar"
#define HT_GRENADE_EFFECT_NAME "AlienEnergyEffect"

// Heavy Trooper memory data structure
struct heavytrooper_data {
  // Mode Data
  int mode;        // indicates what mode trooper is currently in
  float mode_time; // time spent in current mode

  float base_shields;         // original shields
  float base_speed;           // original max speed
  float base_delta_speed;     // original max delta speed
  float base_turn_rate;       // original turn rate
  float base_delta_turn_rate; // original delta turn rate

  float charge; // stores energy used by curl-up mode

  float curlup_dist; // stores this robot's max curlup dist
  float max_charge;  // stores this robot's max charge

  float last_grenade_time;       // last time a grenade was fired
  float next_grenade_check_time; // next time to check if we have a clear grenade shot
  float next_charge_time;        // next time to re-charge or deplete charge
  float next_blast_time;         // next time a shield collision blast can be created
  float next_particle_time;      // next time to create a casing particle spew

};

// Heavy Trooper class definition
class HeavyTrooper : public BaseObjScript {
private:
  heavytrooper_data *memory;

  void SetMode(int me, char mode);

  void DoInit(int me_handle);
  void DoFrame(int me_handle);
  bool DoNotify(int me, tOSIRISEventInfo *data);
  void DoDamage(int me, tOSIRISEVTDAMAGED *damage_data);
  void DoCollide(int me, tOSIRISEVTCOLLIDE *collide_data);

  void LaunchGrenade(int me);
  bool HasAClearGrenadeShot(int me, bool use_grenade_gunpoint = false);

  void SetMaxSpeed(int me, float speed, bool accel_faster = false);
  void EnableGunAttack(int me, bool enable = true);

public:
  HeavyTrooper() {}
  int16_t CallEvent(int event, tOSIRISEventInfo *data);
};

//---------------------
// Lifter class
//---------------------

// Lifter Mode Types
#define LIFTER_PASSIVE 0
#define LIFTER_AGITATED 1
#define LIFTER_HOSTILE 2
#define LIFTER_PULLING 3
#define LIFTER_PASSIFYING 4

// General Lifter Constants
#define LIFTER_MEMORY_ID 4

#define LIFTER_MAX_PULL_TIME 8.0f
#define LIFTER_PULL_CHECK_INTERVAL 0.5f
#define LIFTER_PULL_VALIDATE_INTERVAL 0.25f
#define LIFTER_PULL_TARGET_INTERVAL 0.15f

#define LIFTER_NEXT_PULL_DELAY 3.0f
#define LIFTER_NEXT_PULL_VARIANCE 5.0f

#define LIFTER_MAX_PULLING_DIST 250.0f
#define LIFTER_MIN_PULLING_DIST 20.0f

#define LIFTER_MIN_HOSTILE_MODE_TIME 8.0f
#define LIFTER_MIN_AGITATED_MODE_TIME 2.0f

#define LIFTER_AGITATED_INTEREST_TIME 8.0f
#define LIFTER_HOSTILE_INTEREST_TIME 18.0f

#define LIFTER_AGITATED_SHIELD_PERCENT 0.95f
#define LIFTER_HOSTILE_SHIELD_PERCENT 0.85f

#define LIFTER_LOSE_PULL_SHIELD_PERCENT 0.20f

#define LIFTER_TARGET_PULL_SPEED 35.0f

#define LIFTER_LIFT_BEAM_UPDATE_TIME 0.25f

// Lifter speed constants
#define LIFTER_AGITATED_SPEED_MOD 1.2f
#define LIFTER_HOSTILE_SPEED_MOD 2.0f
#define LIFTER_PULLING_SPEED_MOD 0.6f

// Lifter memory data structure
struct lifter_data {
  // Mode Data
  int mode;        // indicates what mode lifter is currently in
  float mode_time; // time spent in current mode

  float base_shields; // original shields
  float base_speed;   // original max speed

  float shield_loss_margin;    // amount of shields that must be lost before pull is lost
  float shields_at_pull_start; // shield total at start of pull

  int pull_target;      // the target being pulled
  int pull_target_prop; // invisible powerup target pos
  int pull_source_prop; // invisible powerup source pos

  bool not_hostile_yet; // whether or not it's been in hostile mode yet

  float last_damaged_time;     // last time we were damaged
  float next_pull_check_time;  // the next time we can check if a pull can be done
  float last_target_pull_time; // the last time the target was pulled

  float next_lift_beam_time; // the next time to update the lift beam

};

// Lifter class definition
class Lifter : public BaseObjScript {
private:
  lifter_data *memory;

  void SetMode(int me, char mode);

  void DoInit(int me_handle);
  void DoFrame(int me_handle);
  bool DoNotify(int me, tOSIRISEventInfo *data);
  void DoDamage(int me, tOSIRISEVTDAMAGED *damage_data);
  void DoCollide(int me, tOSIRISEVTCOLLIDE *collide_data);
  void DoCleanUp(int me);

  void SetMaxSpeed(int me, float speed);
  bool OkToPull(int me, bool initial_check = true);
  void PullTarget(int me);
  void UpdateTractorBeam(int me);

  void ConfineTarget(int me);
  void ReleaseTarget(int me);
  void StopPulling(int me);
  void MoveTargetProp(int target_handle);
  void SwitchToAlert(int me);
  void UpdateLiftBeam(int me);

public:
  Lifter() {}
  int16_t CallEvent(int event, tOSIRISEventInfo *data);
};

//---------------------
// AlienBoss class
//---------------------

// Alien Boss Mode Types
#define AB_WANDERING 0
#define AB_HUNTING 1
#define AB_ATTACKING 2
#define AB_PREPARE_SPECIAL_ATTACK 3
#define AB_SPECIAL_ATTACK 4
#define AB_SPECIAL_ATTACK_RECOIL 5
#define AB_MELEE_ATTACK 6

#define AB_GOING_TO_NEST 10
#define AB_LANDING_AT_NEST 11
#define AB_HEALING 12
#define AB_FLEEING 13

#define AB_HIDING_FROM_THRUSTER 20
#define AB_GOING_TO_PROTECT_NEST 21
#define AB_DYING 22
#define AB_WAITING 23
#define AB_TELEPORTING 24

// Alien Boss State Types
#define AB_NORMAL 0
#define AB_PROTECTIVE 1
#define AB_ANGRY 2

// General Alien Boss Constants
#define AB_MEMORY_ID 4

#define AB_HEALING_ACTION_TIME 1.0f
#define AB_HEAL_AMOUNT_PERCENT 0.05f
#define AB_BEAM_UPDATE_TIME 0.25f

#define AB_NORMAL_DAMAGE_LEVEL 0.25f
#define AB_PROTECTIVE_DAMAGE_LEVEL 0.4f
#define AB_ANGRY_DAMAGE_LEVEL 0.6f

#define AB_MAX_FLEE_TIME 10.0f
#define AB_STARTING_FLEE_CHANCE 90
#define AB_FLEE_CHANCE_DECREMENT 10

#define AB_ATTACK_CHECK_INTERVAL 0.5f

#define AB_DAMAGE_EFFECT_INTERVAL 0.5f
#define AB_MAX_DAMAGE_SPARK_RATE 20.0f

#define AB_MAX_SPECIAL_ATTACK_DIST 70.0f
#define AB_MAX_STING_DIST 40.0f

#define AB_STING_DAMAGE 20.0f

#define AB_SPECIAL_ATTACK_DELAY 6.0f
#define AB_SPECIAL_ATTACK_VARIANCE 14.0f

#define AB_MELEE_ATTACK_DELAY 5.0f
#define AB_MELEE_ATTACK_VARIANCE 5.0f
#define AB_MELEE_DIST 40.0f
#define AB_MAX_MELEE_TIME 5.0f

#define AB_CHECK_IF_STUCK_INTERVAL 10.0f
#define AB_CHECK_IF_STUCK_DIST 10.0f

// Alien Boss goal complete IDs
#define AB_GUID_GOT_TO_NEST 0x10100001
#define AB_GUID_LANDED 0x10100002
#define AB_GUID_GOT_TO_DEST_OBJ 0x10100003
#define AB_GUID_GOT_TO_WANDER_ROOM 0x10100004
#define AB_GUID_GOT_TO_NEST_ROOM 0x10100005
#define AB_GUID_GOT_TO_HIDE_ROOM 0x10100006

// Alien Boss Custom Anim Frames
#define AB_HEALING_START_FRAME 1.0f
#define AB_HEALING_END_FRAME 10.0f
#define AB_HEALING_ANIM_TIME 2.0f

#define AB_LANDING_START_FRAME 50.0f
#define AB_LANDING_END_FRAME 53.0f
#define AB_LANDING_ANIM_TIME 1.0f

#define AB_TAKEOFF_START_FRAME 10.0f
#define AB_TAKEOFF_END_FRAME 25.0f
#define AB_TAKEOFF_ANIM_TIME 1.0f

#define AB_ATTACK_START_FRAME 36.0f
#define AB_ATTACK_END_FRAME 45.0f
#define AB_ATTACK_ANIM_TIME 0.5f

#define AB_RECOIL_START_FRAME 45.0f
#define AB_RECOIL_END_FRAME 50.0f
#define AB_RECOIL_ANIM_TIME 0.5f

// Speed Modifier Constants
#define AB_WANDER_SPEED_MOD 1.0f
#define AB_ATTACK_SPEED_MOD 0.9f
#define AB_MELEE_ATTACK_SPEED_MOD 1.1f
#define AB_SPEC_ATTACK_SPEED_MOD 0.3f
#define AB_RETURN_SPEED_MOD 1.2f
#define AB_LAND_SPEED_MOD 0.4f
#define AB_FLEE_SPEED_MOD 1.0f
#define AB_GUARD_SPEED_MOD 0.9f
#define AB_HIDE_SPEED_MOD 1.0f
#define AB_DIE_SPEED_MOD 1.0f

// Alien Boss Lookup Globals
#define AB_NUM_WANDER_ROOMS 11

static const char *const AB_WanderRoomNames[AB_NUM_WANDER_ROOMS] = {
    "BossRoomA", "BossTunnelAB", "BossRoomB", "BossTunnelBC", "BossRoomC", "BossTunnelCE",
    "BossRoomD", "BossTunnelAE", "BossRoomE", "BossTunnelCE", "BossRoomD"};

// Alien Boss memory data structure
struct alienboss_data {
  // Mode Data
  int mode;  // indicates what mode alien boss is currently in
  int state; // indicates the boss's current state

  float mode_time;                // time spent in current mode
  float next_activity_time;       // next time to do something
  float max_wander_time;          // maximum time to be spent wandering
  float next_update_beam_time;    // the next time to update the energy beams
  float next_generic_check_time;  // the next time to do generic checks
  float next_special_damage_time; // the next time alien boss can take special damage
  float next_special_attack_time; // the next time boss can do a special attack
  float next_melee_attack_time;   // the next time boss can do a melee attack
  float next_damage_effect_time;  // the next time to do the damage effect
  float intro_end_time;           // the time intro ends

  // General Information
  float base_speed;       // the robot's base speed
  float base_shields;     // the robot's base shields
  int dest_object_handle; // destination handle for object to go to instead of wander
  float damage;           // the amount of damage currently sustained
  float damage_threshold; // the damage that can be sustained before returning to heal
  int fire_flee_chance;   // the chance to flee when caught on fire

  int wander_rooms[AB_NUM_WANDER_ROOMS]; // the rooms alien wanders through
  int curr_wander_room_index;            // the room currently wandering to
  bool wander_forward;                   // whether to follow list normally or in reverse
  bool waiting_effect_enabled;           // whether waiting effect should be done yet
  bool intro_ended;                      // whether intro is over yet

  // Indicators
  bool done_turning;
  bool done_moving;
  bool hit_by_player;
  bool protected_nest;

  // Home Data
  vector home_pos;
  int home_room;
  vector home_fvec;
  vector home_uvec;

  // Energy effect props
  int pos1_handle;
  int pos2_handle;
  int pos3_handle;

  int tail_pos_handle;

  // Scenario objects
  int nest_handle;

  // Hiding from thruster rooms
  int left_hide_room;
  int right_hide_room;

  // Teleport values
  vector dest_pos;
  int dest_room;
  int mode_prior_teleporting;
  bool did_transfer;

  // Getting Stuck Prevention
  float squared_dist_moved;
  float next_check_if_stuck_time;
  vector last_pos;

};

// Alien Boss class definition
class AlienBoss : public BaseObjScript {
private:
  alienboss_data *memory;

  void SetMode(int me, char mode);
  void DoTakeoff(int me, float takeoff_speed, float speed_variance);

  void DoInit(int me_handle);
  void DoFrame(int me_handle);
  bool DoNotify(int me, tOSIRISEventInfo *data);
  void DoDamage(int me, tOSIRISEVTDAMAGED *damage_data);
  void DoCollide(int me, tOSIRISEVTCOLLIDE *collide_data);
  void DoCleanUp(int me);

  bool SendCommand(int me, int it, char command, void *ptr);
  bool ReceiveCommand(int me, int it, char command, void *ptr);

  bool IsOnFire(int me);
  void SetMaxSpeed(int me, float speed);
  void SetWanderGoal(int me);
  void SetWanderRoomGoal(int me);
  int DetermineClosestRoom(int me);
  void SetProtectNestGoal(int me);
  void SetHideRoomGoal(int me);

  bool OkToStartSpecialAttack(int me);
  bool DoStingAttack(int me);
  void AbortCurrentMode(int me);
  void UpdateHealingEnergyBeams(int me);
  void UpdateDamageEffect(int me);
  void StartHealing(int me);
  void StopHealing(int me);

  bool ModeIsStuckSensitive(int mode);

  void DoCustomLookups(void);

public:
  AlienBoss() {}
  int16_t CallEvent(int event, tOSIRISEventInfo *data);
};

//---------------------
// Security Camera class
//---------------------

// Security Camera Mode Types
#define SC_PANNING 0
#define SC_TRACKING 1

// Security Camera Panning types
#define SC_LOOPING 0
#define SC_WARMING_UP 1

// General Security Camera Constants
#define SC_MEMORY_ID 4

#define SC_TRACK_TIME_TO_ALERT 3.0f
#define SC_TIME_TO_LOSE_TARGET 1.0f
#define SC_TIME_TO_GAIN_TARGET 1.0f

#define SC_PAN_SPEED 26.0f   // frames per second
#define SC_TRACK_SPEED 52.0f // frames per second

#define SC_START_FRAME 0.0f
#define SC_MID_FRAME 90.0f
#define SC_END_FRAME 180.0f
#define SC_MAX_FRAME 360.0f // end frame to max frame are reverse of start to end
#define SC_MAX_FRAME_OFFSET 90.0f

#define SC_FRAME_DELTA_ERROR 1.5f

#define SC_TRACK_INTERVAL 0.25f

// Security camera memory data structure
struct securitycamera_data {
  // Mode Data
  int mode;        // indicates what mode security camera is currently in
  float mode_time; // time spent in current mode

  bool alerted; // whether camera is on alert

  int panning_state; // whether it is looping or warming up

  float next_activity_time;    // the next time to do tracking
  float last_update_anim_time; // the last time tracking was done
  float curr_track_anim_frame; // the current tracking animation frame

};

// Security Camera class definition
class SecurityCamera : public BaseObjScript {
private:
  securitycamera_data *memory;

  void SetMode(int me, char mode);

  void DoInit(int me_handle);
  void DoFrame(int me_handle);
  bool DoNotify(int me, tOSIRISEventInfo *data);
  void DoDamage(int me, tOSIRISEVTDAMAGED *damage_data);
  void DoCollide(int me, tOSIRISEVTCOLLIDE *collide_data);

  bool SendCommand(int me, int it, char command, void *ptr);
  bool ReceiveCommand(int me, int it, char command, void *ptr);

public:
  SecurityCamera() {}
  int16_t CallEvent(int event, tOSIRISEventInfo *data);
};

//---------------------
// CrowdControl Class
//---------------------

// General Crowd Control Constants
#define CC_MEMORY_ID 4

#define CC_CHECK_INTERVAL 0.2f
#define CC_MIN_STOPPING_SPEED 0.0000001f // we want to preserve the direction of velocity

// Crowd Control memory data structure
struct crowdcontrol_data {
  int follow_handle;     // handle of object to not crowd
  float stop_dist;       // dist at which to stop completely
  float slowdown_offset; // dist at which to start slowing down (added to stop dist)

  bool disable_check; // allows slowdown check to be turned on/off dynamically

  float base_speed;      // the base speed of the object
  float next_check_time; // the next time to do closeness check

};

// Security Camera class definition
class CrowdControl : public BaseObjScript {
private:
  crowdcontrol_data *memory;

  void DoInit(int me_handle);
  void DoFrame(int me_handle);
  bool DoNotify(int me, tOSIRISEventInfo *data);

  bool SendCommand(int me, int it, char command, void *ptr);
  bool ReceiveCommand(int me, int it, char command, void *ptr);

public:
  CrowdControl() {}
  int16_t CallEvent(int event, tOSIRISEventInfo *data);
};

//-----------------------
// Name lookup functions
//-----------------------

// Looks up any names and stores the returned ID's
void DoNameLookups(void) {
  energy_effect_id = Wpn_FindID(ALIEN_ENERGY_EFFECT_NAME);
  frag_burst_effect_id = Wpn_FindID("FragBurstEffect");
  boss_frag_burst_id = Wpn_FindID("AlienBossFragBurst");
  transfer_effect_id = Scrpt_FindTextureName(ALIEN_TRANSFER_EFFECT_NAME);
  heal_effect_id = Scrpt_FindTextureName(ALIEN_HEAL_EFFECT_NAME);
  boss_heal_effect_id = Scrpt_FindTextureName("FunkyEffect4");
  tractor_beam_effect_id = Scrpt_FindTextureName("FunkyEffectGreen");
  alien_organism_id = Obj_FindID(ALIEN_TYPE_NAME);
  shield_blast_id = Wpn_FindID(HT_SHIELD_BLAST_NAME);
  ht_grenade_id = Wpn_FindID(HT_GRENADE_NAME);
  ht_grenade_effect_id = Wpn_FindID(HT_GRENADE_EFFECT_NAME);
  lifter_blast_effect_id = Wpn_FindID("LifterHitBlast");
  lifter_stick_effect_id = Scrpt_FindTextureName("FunkyEffect2");
  teleport_effect_id = Wpn_FindID("TeleportEffect");

  ht_grenade_sound_id = Sound_FindId("Drop weapon");

  powerup_id = Obj_FindID("Invisiblepowerup");

  boss_flapping_id = Sound_FindId("Merc4BossBuzz");
  boss_turf_id = Sound_FindId("Merc4BossTurf");
  boss_see_id = Sound_FindId("Merc4BossSee");
  boss_hurt_id = Sound_FindId("Merc4BossHurt");

  lifter_pull_sound_id = Sound_FindId("RbtLifterBeam");
  lifter_amb_sound_id = Sound_FindId("RbtLifterAmb1");
}

//----------------
// Standard stuff
//----------------

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

  // Do name lookups
  DoNameLookups();

  InitMathTables();

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
int STDCALL GetGOScriptID(const char *name, uint8_t isdoor) {
  for (int i = 0; i < NUM_IDS; i++) {
    if (!stricmp(name, ScriptInfo[i].name)) {
      return ScriptInfo[i].id;
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
  case ID_ALIENORGANISM:
    return new AlienOrganism;
    break;
  case ID_HEAVYTROOPER:
    return new HeavyTrooper;
    break;
  case ID_LIFTER:
    return new Lifter;
    break;
  case ID_ALIENBOSS:
    return new AlienBoss;
    break;
  case ID_SECURITYCAMERA:
    return new SecurityCamera;
    break;
  case ID_CROWDCONTROL:
    return new CrowdControl;
    break;
  default:
    mprintf(0, "SCRIPT: Illegal ID (%d)\n", id);
    break;
  };
  return NULL;
}

//	DestroyInstance
//	Purpose:
//		Given an ID, and a pointer to a particular instance of a script, this function will delete and
//	destruct all information associated with that script, so it will no longer exist.
void STDCALL DestroyInstance(int id, void *ptr) {
  switch (id) {
  case ID_ALIENORGANISM:
    delete ((AlienOrganism *)ptr);
    break;
  case ID_HEAVYTROOPER:
    delete ((HeavyTrooper *)ptr);
    break;
  case ID_LIFTER:
    delete ((Lifter *)ptr);
    break;
  case ID_ALIENBOSS:
    delete ((AlienBoss *)ptr);
    break;
  case ID_SECURITYCAMERA:
    delete ((SecurityCamera *)ptr);
    break;
  case ID_CROWDCONTROL:
    delete ((CrowdControl *)ptr);
    break;
  default:
    mprintf(0, "SCRIPT: Illegal ID (%d)\n", id);
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
int16_t STDCALL CallInstanceEvent(int id, void *ptr, int event, tOSIRISEventInfo *data) {
  return ((BaseObjScript *)ptr)->CallEvent(event, data);
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
int STDCALL SaveRestoreState(void *file_ptr, uint8_t saving_state) { return 0; }

//============================================
// Script Implementation
//============================================
BaseObjScript::BaseObjScript() {}

BaseObjScript::~BaseObjScript() {}

int16_t BaseObjScript::CallEvent(int event, tOSIRISEventInfo *data) { return CONTINUE_CHAIN | CONTINUE_DEFAULT; }

//---------------------
// AlienOrganism class
//---------------------

// Sends a command out to another alien
bool AlienOrganism::SendCommand(int me, int it, char command, void *ptr) {
  bot_com com;

  com.action = command;
  com.ptr = ptr;

  tOSIRISEventInfo ei;

  ei.me_handle = it;
  ei.extra_info = (void *)&com;
  ei.evt_ai_notify.notify_type = AIN_USER_DEFINED;
  ei.evt_ai_notify.it_handle = me;

  return Obj_CallEvent(it, EVT_AI_NOTIFY, &ei);
}

// Processes a command from another alien
bool AlienOrganism::ReceiveCommand(int me, int it, char command, void *ptr) {
  switch (command) {
  // Leader is asking alien to join squad
  case ALIEN_COM_JOIN_ME:
    if (WillJoinLeader(me, it, *(int *)ptr)) {
      return true;
    }
    break;

  // Leader is telling squadie to leave the squad
  case ALIEN_COM_LEAVE_ME:
    memory->squad_flags = 0;
    memory->leader_handle = OBJECT_HANDLE_NONE;
    memory->num_teammates = 0;
    SetMode(me, memory->mode);
    return true;
    break;

  // Leader is telling squadie to leave the squad for the given mode
  case ALIEN_COM_LEAVE_ME_FOR_MODE:
    memory->squad_flags = 0;
    memory->leader_handle = OBJECT_HANDLE_NONE;
    memory->num_teammates = 0;
    SetMode(me, *(int *)ptr);
    return true;
    break;

  case ALIEN_COM_PROTECT_ME:
    break;

  // Leader is telling squadie to break formation and attack
  case ALIEN_COM_BREAK_AND_ATTACK:
    memory->squad_flags |= ALIEN_BROKEN;
    SetMode(me, memory->mode);
    return true;
    break;

  // Leader is telling squadie to resume formation
  case ALIEN_COM_REGROUP:
    memory->squad_flags &= ~ALIEN_BROKEN;
    SetMode(me, memory->mode);
    return true;
    break;

  // Leader is telling squadie to switch to given mode
  case ALIEN_COM_SET_MODE:
    SetMode(me, *(int *)ptr);
    return true;
    break;

  // Leader is telling squadie to set pending mode
  case ALIEN_COM_SET_PENDING_MODE:
    memory->pending_mode = *(int *)ptr;
    return true;
    break;

  // Alien is asked to return current mode
  case ALIEN_COM_GET_MODE:
    *(int *)ptr = memory->mode;
    return true;
    break;

  // Alien is told to set the given squad flags
  case ALIEN_COM_SET_SQUAD_FLAGS: {
    int flags = *(int *)ptr;
    memory->squad_flags |= flags;
    return true;
  } break;

  // Alien is told to clear the given squad flags
  case ALIEN_COM_CLEAR_SQUAD_FLAGS: {
    int flags = *(int *)ptr;
    memory->squad_flags &= ~flags;
    return true;
  } break;

  // Squadie is asking leader if it can leave
  case ALIEN_COM_REQUEST_LEAVE:
    break;

  // Squadie is telling leader that he's leaving
  case ALIEN_COM_LEAVING_YOU: {
    int tm_id = FindTeammateID(it);
    if (tm_id >= 0) {
      RemoveTeammate(tm_id);
      if (memory->num_teammates == 0) {
        memory->squad_flags = 0;
        SetMode(me, memory->mode);
      }
      return true;
    }
  } break;

  // Squadie is asking for it's formation position
  case ALIEN_COM_GET_GOAL_POS: {
    int tm_id = FindTeammateID(it);
    if (tm_id >= 0) {
      CalcFormationPosition(me, tm_id, (vector *)ptr);
      return true;
    }
  } break;

  // Squadie is asking for it's formation goal room
  case ALIEN_COM_GET_GOAL_ROOM: {
    Obj_Value(me, VF_GET, OBJV_I_ROOMNUM, (int *)ptr);
    return true;
  } break;

  // I'm being informed that a squadmate has been hit
  case ALIEN_COM_I_WAS_HIT: {
    // mprintf(0,"I was hit message received!\n");

    if (MeleeAttack(me)) {
      return true;
    }
  } break;

  // I'm being asked if I can see a squadie or not
  case ALIEN_COM_CAN_YOU_SEE_ME: {
    int tm_id = FindTeammateID(it);
    if (tm_id >= 0) {
      *(bool *)ptr = memory->teammate[tm_id].is_visible;
      return true;
    }
  } break;

  // I'm being told to go hunt to a certain object
  case ALIEN_COM_HUNT_TO_OBJ: {
    // Make sure we're in a mode that is ok to switch from
    if (memory->mode != ALIEN_LANDED_RESTING)
      return false;

    // If we're a squadie, forward this to the leader and let him take care of it
    if (memory->squad_flags & ALIEN_SQUADIE) {
      return SendCommand(me, memory->leader_handle, ALIEN_COM_HUNT_TO_OBJ, ptr);
    }

    // Otherwise, set the destination object
    memory->dest_object_handle = *(int *)ptr;

    // Switch to hunting mode
    SetMode(me, ALIEN_HUNTING);

    // If leading, tell the squad to do the same
    if (memory->squad_flags & ALIEN_LEADER) {
      int mode = ALIEN_HUNTING;
      IssueSquadOrder(me, ALIEN_COM_SET_MODE, &mode);
    }

    return true;
  } break;

  // I'm being told to go hunt to a certain object
  case ALIEN_COM_SCAV_TO_OBJ: {
    // Make sure we're in a mode that is ok to switch from
    if (memory->mode != ALIEN_LANDED_RESTING)
      return false;

    // If we're a squadie, forward this to the leader and let him take care of it
    if (memory->squad_flags & ALIEN_SQUADIE) {
      return SendCommand(me, memory->leader_handle, ALIEN_COM_SCAV_TO_OBJ, ptr);
    }

    // Otherwise, set the destination object
    memory->dest_object_handle = *(int *)ptr;

    // Switch to hunting mode
    SetMode(me, ALIEN_SCAVENGING);

    // If leading, tell the squad to do the same
    if (memory->squad_flags & ALIEN_LEADER) {
      int mode = ALIEN_SCAVENGING;
      IssueSquadOrder(me, ALIEN_COM_SET_MODE, &mode);
    }

    return true;
  } break;
  }

  //	mprintf(0, "AlienOrganism action %d failed\n");
  return false;
}

// Sends a command (rhetorical) to all of a leader's teammates
bool AlienOrganism::IssueSquadOrder(int leader_handle, char command, void *ptr) {
  for (int j = 0; j < memory->num_teammates; j++)
    SendCommand(leader_handle, memory->teammate[j].handle, command, ptr);

  return true;
}

// Returns true if mode is one of the landed modes
bool AlienOrganism::IsLandedMode(int mode) {
  if (mode == ALIEN_LANDED || mode == ALIEN_LANDED_RESTING || mode == ALIEN_LANDED_HEALING ||
      mode == ALIEN_LANDED_DEPOSITING || mode == ALIEN_LANDED_WITHDRAWING)
    return true;

  return false;
}

// Returns true if mode is one of the energy related landed modes
bool AlienOrganism::IsEnergyRelatedLandedMode(int mode) {
  if (mode == ALIEN_LANDED_HEALING || mode == ALIEN_LANDED_DEPOSITING || mode == ALIEN_LANDED_WITHDRAWING)
    return true;

  return false;
}

// Returns true if current mode warrants fighting back when hit by player
bool AlienOrganism::ModeIsAttackSensitive(int mode, int pending_mode) {
  // Make sure we're not already in an attack mode
  if (mode != ALIEN_ATTACKING_MELEE && mode != ALIEN_ATTACKING_RANGED) {
    // Make sure we're not healing, giving birth, or fleeing
    if (mode != ALIEN_FLEEING && mode != ALIEN_LANDING_AT_HOME && mode != ALIEN_LANDED_HEALING &&
        (mode != ALIEN_LANDED_DEPOSITING || rand() % 100 < 2) &&
        (mode != ALIEN_LANDED_WITHDRAWING || rand() % 100 < 5) && pending_mode != ALIEN_LANDED_HEALING &&
        pending_mode != ALIEN_LANDED_DEPOSITING && pending_mode != ALIEN_LANDED_WITHDRAWING &&
        mode != ALIEN_FINDING_BIRTHPLACE && mode != ALIEN_GIVING_BIRTH) {
      return true;
    }
  }

  return false;
}

// These are the modes from which a leader may attempt to recruit squadies
bool AlienOrganism::ModeSupportsSquadRecruitment(int mode) {
  if (mode == ALIEN_LANDED_RESTING || mode == ALIEN_HUNTING || mode == ALIEN_SCAVENGING ||
      mode == ALIEN_ATTACKING_MELEE)
    return true;

  return false;
}

// These are the modes from which an alien may enlist in a squad
bool AlienOrganism::ModeSupportsSquadEnlistment(int mode, int pending_mode, int recruiter_mode) {
  // If recruiter is landed resting, only join if we are too
  if (recruiter_mode == ALIEN_LANDED_RESTING && mode != ALIEN_LANDED_RESTING)
    return false;

  if (mode == ALIEN_LANDED_RESTING || (mode == ALIEN_GOING_HOME && pending_mode == ALIEN_LANDED_RESTING) ||
      mode == ALIEN_HUNTING || mode == ALIEN_SCAVENGING || mode == ALIEN_ATTACKING_MELEE)
    return true;

  return false;
}

// These are the modes in which a squadie may follow formation movement
bool AlienOrganism::ModeSupportsSquadMovement(int mode) {
  if (mode == ALIEN_HUNTING || mode == ALIEN_SCAVENGING || mode == ALIEN_ATTACKING_MELEE)
    return true;

  return false;
}

// Tells all current teammates that they are on their own again
void AlienOrganism::DisperseSquad(int me) {
  if (!(memory->squad_flags & ALIEN_LEADER))
    return;

  for (int i = 0; i < memory->num_teammates; i++) {
    SendCommand(me, memory->teammate[i].handle, ALIEN_COM_LEAVE_ME, NULL);
  }

  memory->squad_flags = 0;
  memory->leader_handle = OBJECT_HANDLE_NONE;
  memory->num_teammates = 0;
}

// Tells all current teammates that they are to leave the squad and enter the given mode
void AlienOrganism::DisperseSquadToMode(int me, int mode) {
  if (!(memory->squad_flags & ALIEN_LEADER))
    return;

  for (int i = 0; i < memory->num_teammates; i++) {
    SendCommand(me, memory->teammate[i].handle, ALIEN_COM_LEAVE_ME_FOR_MODE, &mode);
  }

  memory->squad_flags = 0;
  memory->leader_handle = OBJECT_HANDLE_NONE;
  memory->num_teammates = 0;
}

bool AlienOrganism::LeaveLeader(int me) {
  if (!(memory->squad_flags & ALIEN_SQUADIE))
    return true;

  SendCommand(me, memory->leader_handle, ALIEN_COM_LEAVING_YOU, NULL);
  memory->squad_flags = 0;
  memory->leader_handle = OBJECT_HANDLE_NONE;
  memory->num_teammates = 0;

  return true;
}

// Attempts to make an alien flee (returns true if it does, false otherwise)
bool AlienOrganism::Flee(int me, bool force_flee /*=false*/) {
  // If we're a squadie, tell the leader we are leaving
  if (memory->squad_flags & ALIEN_SQUADIE) {
    if (!force_flee) {
      // do squadie saving throw
      if ((rand() % 100) < ALIEN_SQUADIE_FLEE_SAVING_THROW)
        return false;
    }

    LeaveLeader(me);
  }

  // If we're a leader, disperse the squad
  if (memory->squad_flags & ALIEN_LEADER) {
    if (!force_flee) {
      // do leader saving throw
      if ((rand() % 100) < ALIEN_LEADER_FLEE_SAVING_THROW)
        return false;
    }

    DisperseSquad(me);
  }

  // Ok, let's flee
  SetMode(me, ALIEN_FLEEING);
  // mprintf(0,"Alien is fleeing!\n");

  return true;
}

// Attempts to make an alien attack (and his squad if leading one)
bool AlienOrganism::MeleeAttack(int me, bool force_attack /*=false*/) {
  // If leading squad, tell squad to switch to melee mode
  if (memory->squad_flags & ALIEN_LEADER) {
    int mode = ALIEN_ATTACKING_MELEE;
    IssueSquadOrder(me, ALIEN_COM_SET_MODE, &mode);
  }

  // Attack target!
  SetMode(me, ALIEN_ATTACKING_MELEE);

  // mprintf(0,"Alien is switching to Melee Attack mode!\n");

  return true;
}

// Makes an alien go into ranged attack mode
bool AlienOrganism::RangedAttack(int me) {
  // If we're a squadie, tell the leader we are leaving
  if (memory->squad_flags & ALIEN_SQUADIE) {
    LeaveLeader(me);
  }

  // If we're a leader, disperse the squad
  if (memory->squad_flags & ALIEN_LEADER) {
    DisperseSquad(me);
  }

  SetMode(me, ALIEN_ATTACKING_RANGED);
  // mprintf(0,"Alien is switching to ranged attack mode!\n");

  return true;
}

// Makes an alien go into ranged attack mode
bool AlienOrganism::Heal(int me) {

  // If we're a squadie, tell the leader we are leaving
  if (memory->squad_flags & ALIEN_SQUADIE) {
    LeaveLeader(me);
  }

  // If we're a leader, disperse the squad
  if (memory->squad_flags & ALIEN_LEADER) {
    DisperseSquad(me);
  }

  // mprintf(0,"Heading home to heal...\n");

  // Ok, let's go home and heal
  memory->pending_mode = ALIEN_LANDED_HEALING;
  SetMode(me, ALIEN_GOING_HOME);

  return true;
}

// Makes an alien go into ranged attack mode
bool AlienOrganism::Deposit(int me) {
  // If we're a squadie, tell the leader we are leaving
  if (memory->squad_flags & ALIEN_SQUADIE) {
    LeaveLeader(me);
  }

  // If we're a leader, disperse the squad
  if (memory->squad_flags & ALIEN_LEADER) {
    DisperseSquad(me);
  }

  // mprintf(0,"Alien is going home to deposit...\n");

  memory->pending_mode = ALIEN_LANDED_DEPOSITING;
  SetMode(me, ALIEN_GOING_HOME);

  return true;
}

// Makes an alien go into ranged attack mode
bool AlienOrganism::Withdraw(int me) {
  // If we're a squadie, tell the leader we are leaving
  if (memory->squad_flags & ALIEN_SQUADIE) {
    LeaveLeader(me);
  }

  // If we're a leader, disperse the squad
  if (memory->squad_flags & ALIEN_LEADER) {
    DisperseSquad(me);
  }

  // mprintf(0,"Alien is going home to withdraw...\n");

  memory->pending_mode = ALIEN_LANDED_WITHDRAWING;
  SetMode(me, ALIEN_GOING_HOME);

  return true;
}

// Checks to see if we can join the leader
bool AlienOrganism::WillJoinLeader(int me, int leader_handle, int priority) {
  // Get the recruiter's current mode
  int recruiter_mode;
  SendCommand(me, leader_handle, ALIEN_COM_GET_MODE, &recruiter_mode);

  // If can't join a squad in current mode, decline offer
  if (!ModeSupportsSquadEnlistment(memory->mode, memory->pending_mode, recruiter_mode))
    return false;

  // If not currently in a squad, possibly join up
  if (memory->squad_flags == 0) {
    // Only join 60% of the time (to help prevent mass join slowdowns)
    if (rand() % 100 > 60)
      return false;

    memory->leader_handle = leader_handle;
    memory->squad_flags |= ALIEN_SQUADIE;
    return true;
  }

  return false;
}

// Returns the Teammate ID for a given handle (returns -1 if not on team)
int AlienOrganism::FindTeammateID(int tm_handle) {
  for (int i = 0; i < memory->num_teammates; i++) {
    if (memory->teammate[i].handle == tm_handle)
      return i;
  }

  return -1;
}

// Calculates a squad position for given teammate based on leader's mode
int AlienOrganism::CalcFormationPosition(int me, int tm_ID, vector *pos) {
  vector me_pos, goal_pos;
  matrix orient;

  Obj_Value(me, VF_GET, OBJV_V_POS, &me_pos);
  Obj_Value(me, VF_GET, OBJV_M_ORIENT, &orient);

  goal_pos = me_pos;

  switch (memory->mode) {
  // When hunting, form a horizontal diamond
  case ALIEN_HUNTING: {
    switch (tm_ID) {
    case 0:
      goal_pos -= orient.fvec * ALIEN_FO_HUNT_FVEC;
      goal_pos += orient.rvec * ALIEN_FO_HUNT_RVEC;
      break;
    case 1:
      goal_pos -= orient.fvec * ALIEN_FO_HUNT_FVEC;
      goal_pos -= orient.rvec * ALIEN_FO_HUNT_RVEC;
      break;
    case 2:
      goal_pos -= orient.fvec * ALIEN_FO_HUNT_FVEC * 2.0f;
      break;
    }
  } break;

  // When scavenging, form a scavenge line
  case ALIEN_SCAVENGING: {
    switch (tm_ID) {
    case 0:
      goal_pos -= orient.fvec * ALIEN_FO_SCAV_FVEC;
      goal_pos -= orient.rvec * ALIEN_FO_SCAV_RVEC;
      break;
    case 1:
      goal_pos -= orient.fvec * ALIEN_FO_SCAV_FVEC * 2.0f;
      goal_pos += orient.rvec * ALIEN_FO_SCAV_RVEC;
      break;
    case 2:
      goal_pos -= orient.fvec * ALIEN_FO_SCAV_FVEC * 3.0f;
      break;
    }
  } break;

  // When attacking, form a vertical triangle
  case ALIEN_ATTACKING_MELEE:
  case ALIEN_ATTACKING_RANGED: {
    switch (tm_ID) {
    case 0:
      goal_pos += orient.fvec * ALIEN_FO_ATTK_FVEC;
      goal_pos -= orient.uvec * ALIEN_FO_ATTK_UVECN;
      goal_pos += orient.rvec * ALIEN_FO_ATTK_RVEC;
      break;
    case 1:
      goal_pos += orient.fvec * ALIEN_FO_ATTK_FVEC;
      goal_pos -= orient.uvec * ALIEN_FO_ATTK_UVECN;
      goal_pos -= orient.rvec * ALIEN_FO_ATTK_RVEC;
      break;
    case 2:
      goal_pos += orient.fvec * ALIEN_FO_ATTK_FVEC;
      goal_pos += orient.uvec * ALIEN_FO_ATTK_UVECP;
      break;
    }
  } break;
  }

  // Return the goal position
  (*pos) = goal_pos;
  return true;
}

void AlienOrganism::SetSquadieFormationGoal(int me) {
  float cd = .1f;
  float temp = 0.0f;

  // Do the get to position goal for when squadie can't see leader
  SendCommand(me, memory->leader_handle, ALIEN_COM_GET_GOAL_POS, &memory->squad_goal_pos);
  AI_GoalValue(me, 1, VF_SET, AIGV_V_POS, &memory->squad_goal_pos);

  SendCommand(me, memory->leader_handle, ALIEN_COM_GET_GOAL_ROOM, &memory->squad_goal_room);
  AI_GoalValue(me, 1, VF_SET, AIGV_I_ROOMNUM, &memory->squad_goal_room);

  AI_AddGoal(me, AIG_GET_TO_POS, 1, 1.0f, -1, GF_ORIENT_VELOCITY | GF_KEEP_AT_COMPLETION, &memory->squad_goal_pos,
             memory->squad_goal_room);
  AI_GoalValue(me, 1, VF_SET, AIGV_F_CIRCLE_DIST, &cd);

  // Do the custom goal for when squadie can see leader (similar to use bline when sees goal)
  AI_AddGoal(me, AIG_SCRIPTED, 2, 1.0f, -1, GF_ORIENT_VELOCITY | GF_KEEP_AT_COMPLETION, NULL);
  AI_GoalAddEnabler(me, 2, AIE_SCRIPTED, 1.0, 0.0, &temp);
}

void AlienOrganism::UpdateSquadieFormationGoal(int me) {
  // Update the squadie positions
  SendCommand(me, memory->leader_handle, ALIEN_COM_GET_GOAL_POS, &memory->squad_goal_pos);
  AI_GoalValue(me, 1, VF_SET, AIGV_V_POS, &memory->squad_goal_pos);

  SendCommand(me, memory->leader_handle, ALIEN_COM_GET_GOAL_ROOM, &memory->squad_goal_room);
  AI_GoalValue(me, 1, VF_SET, AIGV_I_ROOMNUM, &memory->squad_goal_room);
}

void AlienOrganism::SetWanderGoal(int me) {
  vector pos;
  int room = 0;
  float dist = 15.0f;

  // If the destination object is valid, set the goals to go to that object
  if (memory->dest_object_handle != OBJECT_HANDLE_NONE) {
    int type;
    Obj_Value(memory->dest_object_handle, VF_GET, OBJV_I_TYPE, &type);
    if (type != OBJ_NONE) {
      AI_AddGoal(me, AIG_GET_TO_OBJ, 2, 1.0f, ALIEN_GUID_GOT_TO_DEST_OBJ,
                 GF_ORIENT_VELOCITY | GF_USE_BLINE_IF_SEES_GOAL | GF_KEEP_AT_COMPLETION | GF_NONFLUSHABLE,
                 memory->dest_object_handle);
      AI_GoalValue(me, 2, VF_SET, AIGV_F_CIRCLE_DIST, &dist);

      return;
    }
    memory->dest_object_handle = OBJECT_HANDLE_NONE;
  }

  // Set the goal to wander
  AI_AddGoal(me, AIG_WANDER_AROUND, 2, 1.0f, -1, GF_ORIENT_VELOCITY | GF_KEEP_AT_COMPLETION, room, room);
  AI_GoalValue(me, 2, VF_SET, AIGV_F_CIRCLE_DIST, &dist);
}

// Removes a teammate from the squad
void AlienOrganism::RemoveTeammate(int tm_id) {
  for (int j = tm_id; j < memory->num_teammates - 1; j++) {
    memory->teammate[j] = memory->teammate[j + 1];
  }

  memory->num_teammates--;
}

// Called by a leader to update his squad list (removes any that have died)
void AlienOrganism::UpdateSquadList(int me) {
  if (!(memory->squad_flags & ALIEN_LEADER))
    return;

  for (int i = 0; i < memory->num_teammates; i++) {
    int type;
    Obj_Value(memory->teammate[i].handle, VF_GET, OBJV_I_TYPE, &type);
    if (type == OBJ_NONE) {
      RemoveTeammate(i);
      i--;
    }
  }
}

// Updates the squad (recruitment, etc.)
void AlienOrganism::UpdateSquad(int me) {
  // If me's a squadie, get outta here
  if (memory->squad_flags & ALIEN_SQUADIE)
    return;

  // Remove any dead teammates
  if (memory->squad_flags & ALIEN_LEADER) {
    UpdateSquadList(me);

    if (memory->num_teammates == 0)
      memory->squad_flags = 0;

    if (memory->num_teammates >= ALIEN_MAX_TEAMMATES)
      return;
  }

  // If can't recruit in current mode get outta here
  if (!ModeSupportsSquadRecruitment(memory->mode))
    return;

  // Try to recruit some teammates
  int scan_objs[25];
  int n_scan;
  int room;
  vector pos;
  int i;

  Obj_Value(me, VF_GET, OBJV_V_POS, &pos);
  Obj_Value(me, VF_GET, OBJV_I_ROOMNUM, &room);

  n_scan = AI_GetNearbyObjs(&pos, room, ALIEN_SQUAD_RECRUIT_RADIUS, scan_objs, 25, false, true, false, true);

  for (i = 0; i < n_scan; i++) {
    uint16_t id;
    Obj_Value(scan_objs[i], VF_GET, OBJV_US_ID, &id);

    // this is more rare than the types matching; so, do it first
    if (id == alien_organism_id) {
      int type;
      Obj_Value(scan_objs[i], VF_GET, OBJV_I_TYPE, &type);
      if (type == OBJ_ROBOT && scan_objs[i] != me) {
        // Make sure this guy isn't already a squadmate
        bool already_on_team = false;
        if (memory->squad_flags & ALIEN_LEADER) {
          if (FindTeammateID(scan_objs[i]) >= 0)
            already_on_team = true;
        }

        if (!already_on_team) {
          // Try and recruit him
          int priority = MEDIUM_PRIORITY;
          if (SendCommand(me, scan_objs[i], ALIEN_COM_JOIN_ME, &priority)) {
            // If not currently a leader, it's time to become one
            if (!(memory->squad_flags & ALIEN_LEADER)) {
              memory->squad_flags = ALIEN_LEADER;
              memory->num_teammates = 0;

              // Reset the current mode to take squad grouping into account
              SetMode(me, memory->mode);
            }
            memory->teammate[memory->num_teammates++].handle = scan_objs[i];

            // Tell squadie whether he should be in formation or not
            int sflags = ALIEN_BROKEN;
            if (memory->squad_flags & ALIEN_BROKEN)
              SendCommand(me, scan_objs[i], ALIEN_COM_SET_SQUAD_FLAGS, &sflags);
            else
              SendCommand(me, scan_objs[i], ALIEN_COM_CLEAR_SQUAD_FLAGS, &sflags);

            // Tell squadie to set his mode to the leader's mode
            int mode = memory->mode;
            SendCommand(me, scan_objs[i], ALIEN_COM_SET_MODE, &mode);
          }
        }
      }
    }

    // If squad is full get outta here
    if (memory->num_teammates >= ALIEN_MAX_TEAMMATES)
      return;
  }
}

// Checks which of my boys I can see
void AlienOrganism::UpdateSquadVisibility(int me) {
  int i, flags, fate;
  vector start_pos, end_pos;
  int start_room;
  ray_info ray;

  // First, remove any dead squadies
  UpdateSquadList(me);

  // Get my room and position
  Obj_Value(me, VF_GET, OBJV_V_POS, &start_pos);
  Obj_Value(me, VF_GET, OBJV_I_ROOMNUM, &start_room);

  // Do the visibility check for each teammate
  for (i = 0; i < memory->num_teammates; i++) {
    // Get teammate's position
    Obj_Value(memory->teammate[i].handle, VF_GET, OBJV_V_POS, &end_pos);

    flags = FQ_CHECK_OBJS | FQ_IGNORE_POWERUPS | FQ_IGNORE_WEAPONS | FQ_IGNORE_MOVING_OBJECTS |
            FQ_IGNORE_NON_LIGHTMAP_OBJECTS;
    fate = FVI_RayCast(me, &start_pos, &end_pos, start_room, 1.0f, flags, &ray);

    // Set whether or not this guy is visible
    memory->teammate[i].is_visible = (fate == HIT_NONE) ? true : false;
  }
}

// Checks to see if any fire blobs are nearby
bool AlienOrganism::IsNearFire(int me) { return false; }

// Checks to see if alien is currently on fire
bool AlienOrganism::IsOnFire(int me) { return (Obj_IsEffect(me, EF_NAPALMED)); }

#define ALIEN_MAX_BEAM_DIST 30.0f
#define ALIEN_ATTEMPT_BEAM_DIST 40.0f

void AlienOrganism::ProjectBeam(int me, int pos_handle, vector *start_pos, int start_room, vector *beam_dir,
                                matrix *orient, int beam_flag) {
  ray_info ray;
  int flags, fate;
  vector end_pos, landing_pos;
  int landing_room;
  float dist;

  // Calculate beam's destination
  end_pos = (*start_pos) + ((*beam_dir) * ALIEN_ATTEMPT_BEAM_DIST);

  flags = FQ_CHECK_OBJS | FQ_IGNORE_POWERUPS | FQ_IGNORE_WEAPONS | FQ_IGNORE_MOVING_OBJECTS |
          FQ_IGNORE_NON_LIGHTMAP_OBJECTS;
  fate = FVI_RayCast(me, start_pos, &end_pos, start_room, 0.0f, flags, &ray);

  landing_pos = ray.hit_point;
  landing_room = ray.hit_room;

  // If beam is within certain distance, move the destination object to destination position
  dist = vm_VectorDistance(start_pos, &landing_pos);
  if (dist < ALIEN_MAX_BEAM_DIST) {
    msafe_struct mstruct;
    mstruct.objhandle = pos_handle;
    mstruct.pos = landing_pos;
    mstruct.orient = (*orient);
    mstruct.roomnum = landing_room;
    MSafe_CallFunction(MSAFE_OBJECT_WORLD_POSITION, &mstruct);

    // Mark this beam as enabled
    memory->enabled_beams |= beam_flag;
  }
}

// Calculates positions and moves invisible powerups to them
void AlienOrganism::CalcEnergyBeamPositions(int me) {
  int start_room;
  vector beam1_dir, beam2_dir, beam3_dir;
  vector start_pos;
  matrix orient;

  // Get our object's pos and orientation
  Obj_Value(me, VF_GET, OBJV_V_POS, &start_pos);
  Obj_Value(me, VF_GET, OBJV_M_ORIENT, &orient);
  Obj_Value(me, VF_GET, OBJV_I_ROOMNUM, &start_room);

  // If we're in a beam mode, get the appropriate beam directions
  if (memory->mode == ALIEN_LANDED_DEPOSITING) {
    beam1_dir = -0.40f * orient.uvec - orient.fvec;
    // beam1_dir += orient.rvec*(0.5f-((float)rand()/(float)D3_RAND_MAX)*1.0f);
    vm_VectorNormalize(&beam1_dir);

    beam2_dir = -0.40f * orient.uvec + orient.fvec - orient.rvec;
    // beam2_dir += orient.fvec*(0.5f-((float)rand()/(float)D3_RAND_MAX)*1.0f);
    vm_VectorNormalize(&beam2_dir);

    beam3_dir = -0.40f * orient.uvec + orient.fvec + orient.rvec;
    // beam3_dir += orient.fvec*(0.5f-((float)rand()/(float)D3_RAND_MAX)*1.0f);
    vm_VectorNormalize(&beam3_dir);
  } else if (memory->mode == ALIEN_LANDED_WITHDRAWING) {
    beam1_dir = -0.3f * orient.uvec + orient.fvec;
    // beam1_dir += orient.rvec*(0.5f-((float)rand()/(float)D3_RAND_MAX)*1.0f);
    vm_VectorNormalize(&beam1_dir);

    beam2_dir = -0.3f * orient.uvec - orient.fvec - orient.rvec;
    // beam2_dir -= orient.fvec*(0.5f-((float)rand()/(float)D3_RAND_MAX)*1.0f);
    vm_VectorNormalize(&beam2_dir);

    beam3_dir = -0.3f * orient.uvec - orient.fvec + orient.rvec;
    // beam3_dir -= orient.fvec*(0.5f-((float)rand()/(float)D3_RAND_MAX)*1.0f);
    vm_VectorNormalize(&beam3_dir);
  } else if (memory->mode == ALIEN_LANDED_HEALING) {
    beam2_dir = -0.4f * orient.uvec + orient.rvec * 2.0f;
    vm_VectorNormalize(&beam2_dir);

    beam3_dir = -0.4f * orient.uvec - orient.rvec * 2.0f;
    vm_VectorNormalize(&beam3_dir);
  } else {
    return;
  }

  // All beams off initially
  memory->enabled_beams = 0;

  // Project the three beams
  if (memory->mode != ALIEN_LANDED_HEALING) {
    ProjectBeam(me, memory->pos1_handle, &start_pos, start_room, &beam1_dir, &orient, ALIEN_BEAM1);
  }
  ProjectBeam(me, memory->pos2_handle, &start_pos, start_room, &beam2_dir, &orient, ALIEN_BEAM2);
  ProjectBeam(me, memory->pos3_handle, &start_pos, start_room, &beam3_dir, &orient, ALIEN_BEAM3);

  // Set flag so that this calculation is only done once
  memory->energy_beams_on = true;
}

// Creates the different lightning beam effects
void AlienOrganism::UpdateEnergyBeams(int me) {
  int b1_obj1, b1_obj2;
  int b2_obj1, b2_obj2;
  int b3_obj1, b3_obj2;
  int b1_texture_id;
  int b2_texture_id;
  int b3_texture_id;

  if (!memory->energy_beams_on)
    return;

  // See if it's time to create the next energy effect
  if (memory->next_update_beam_time <= Game_GetTime()) {
    float next_duration = ALIEN_BEAM_UPDATE_TIME /*+ ((float)rand()/(float)D3_RAND_MAX)*0.2f*/;
    memory->next_update_beam_time = Game_GetTime() + next_duration;

    if (memory->mode == ALIEN_LANDED_DEPOSITING) {
      b1_obj1 = memory->pos1_handle;
      b1_obj2 = me;
      b1_texture_id = transfer_effect_id;

      b2_obj1 = memory->pos2_handle;
      b2_obj2 = me;
      b2_texture_id = transfer_effect_id;

      b3_obj1 = memory->pos3_handle;
      b3_obj2 = me;
      b3_texture_id = transfer_effect_id;
    } else if (memory->mode == ALIEN_LANDED_WITHDRAWING) {
      b1_obj1 = me;
      b1_obj2 = memory->pos1_handle;
      b1_texture_id = transfer_effect_id;

      b2_obj1 = me;
      b2_obj2 = memory->pos2_handle;
      b2_texture_id = transfer_effect_id;

      b3_obj1 = me;
      b3_obj2 = memory->pos3_handle;
      b3_texture_id = transfer_effect_id;
    } else if (memory->mode == ALIEN_LANDED_HEALING) {
      b1_obj1 = memory->pos1_handle;
      b1_obj2 = me;
      b1_texture_id = heal_effect_id;

      b2_obj1 = me;
      b2_obj2 = memory->pos2_handle;
      b2_texture_id = heal_effect_id;

      b3_obj1 = me;
      b3_obj2 = memory->pos3_handle;
      b3_texture_id = heal_effect_id;
    } else {
      mprintf(0, "Invalid energy beam effect mode!\n");
      return;
    }

    // Now create the beams
    if (memory->enabled_beams & ALIEN_BEAM1) {
      aLightningCreate(b1_obj1, b1_obj2, next_duration, 1.0f, 2, b1_texture_id, next_duration, 1, 255, 255, 255, false);
    }

    if (memory->enabled_beams & ALIEN_BEAM2) {
      aLightningCreate(b2_obj1, b2_obj2, next_duration, 1.0f, 2, b2_texture_id, next_duration, 1, 255, 255, 255, false);
    }

    if (memory->enabled_beams & ALIEN_BEAM3) {
      aLightningCreate(b3_obj1, b3_obj2, next_duration, 1.0f, 2, b3_texture_id, next_duration, 1, 255, 255, 255, false);
    }
  }
}

// Sets the correct energy effect for an alien
void AlienOrganism::UpdateEnergyEffect(int me) {
  int room;
  vector pos;
  int weapon_id;

  // See if this object has an energy charge
  if (memory->energy_charges > 0.0f) {
    // See if it's time to create the next energy effect
    if (memory->next_update_energy_time <= Game_GetTime()) {
      float slow_down = 0.5f * (ALIEN_MAX_ENERGY_CHARGES - memory->energy_charges) / ALIEN_MAX_ENERGY_CHARGES;
      memory->next_update_energy_time = Game_GetTime() + ((float)rand() / (float)D3_RAND_MAX) * 0.1f + 0.05f + slow_down;

      Obj_Value(me, VF_GET, OBJV_V_POS, &pos);
      Obj_Value(me, VF_GET, OBJV_I_ROOMNUM, &room);

      Obj_Create(OBJ_WEAPON, energy_effect_id, room, &pos, NULL, me);
    }
  }
}

// Sets the alien's max speed
void AlienOrganism::SetMaxSpeed(int me, float speed) { AI_Value(me, VF_SET, AIV_F_MAX_SPEED, &speed); }

// Sets the alien's circle distance
void AlienOrganism::SetCircleDist(int me, float dist) { AI_Value(me, VF_GET, AIV_F_CIRCLE_DIST, &dist); }

// Sets the Current Mode
void AlienOrganism::SetMode(int me, char mode) {
  // mprintf(0, "From mode %d, ",memory->mode);

  int flags;
  char movement_type;
  vector vel;

  // Clear out any goals
  AI_SafeSetType(me, AIT_AIS);

  // Set FOV to 360
  float fov = -1.0f;
  AI_Value(me, VF_SET, AIV_F_FOV, &fov);

  // Turn off ranged firing
  flags = AIF_DISABLE_FIRING;
  AI_Value(me, VF_SET_FLAGS, AIV_I_FLAGS, &flags);

  // Make it persistant
  flags = AIF_PERSISTANT;
  AI_Value(me, VF_SET_FLAGS, AIV_I_FLAGS, &flags);

  // Turn off the energy beams
  memory->energy_beams_on = false;

  // Do mode specific setups
  switch (mode) {
  case ALIEN_GOING_HOME: {
    // mprintf(0,"Going home mode set.\n");

    ray_info ray;
    int fate;
    vector end_pos, landing_pos;
    int end_room, landing_room;

    // If currently landed, set take off velocity
    DoTakeoff(me, 8.0f, 3.0f);

    // Turn on friend avoidance
    flags = AIF_AUTO_AVOID_FRIENDS;
    AI_Value(me, VF_SET_FLAGS, AIV_I_FLAGS, &flags);

    // Set friend avoidance distance
    float avoid_dist = ALIEN_NORMAL_AVOID_DIST;
    AI_Value(me, VF_SET, AIV_F_AVOID_FRIENDS_DIST, &avoid_dist);
    SetCircleDist(me, memory->base_circle_distance);

    // Make it aware
    flags = AIF_FORCE_AWARENESS;
    AI_Value(me, VF_SET_FLAGS, AIV_I_FLAGS, &flags);

    // Clear wall point collision and axes lock
    flags = PF_POINT_COLLIDE_WALLS;
    Obj_Value(me, VF_CLEAR_FLAGS, OBJV_I_PHYSICS_FLAGS, &flags);

    // Set movement type back to physics for flying around
    movement_type = MT_PHYSICS;
    Obj_Value(me, VF_SET, OBJV_C_MOVEMENT_TYPE, &movement_type);

    // Make sure squad broken flag isn't set
    memory->squad_flags &= ~ALIEN_BROKEN;

    // Calculate destination home point a int16_t distance from actual landing point
    end_pos = memory->home_pos + (memory->home_uvec * 20.0f);

    flags = FQ_CHECK_OBJS | FQ_IGNORE_POWERUPS | FQ_IGNORE_WEAPONS | FQ_IGNORE_MOVING_OBJECTS |
            FQ_IGNORE_NON_LIGHTMAP_OBJECTS;
    fate = FVI_RayCast(me, &memory->home_pos, &end_pos, memory->home_room, 0.0f, flags, &ray);

    landing_pos = ray.hit_point;
    landing_room = ray.hit_room;

    // Set goal to head home
    AI_AddGoal(me, AIG_GET_TO_POS, 2, 1.0f, ALIEN_GUID_GOT_HOME, GF_ORIENT_VELOCITY, &landing_pos, landing_room);
    AI_SetGoalCircleDist(me, 2, 2.0f);

    // Set returning home speed
    SetMaxSpeed(me, memory->base_speed * ALIEN_RETURN_SPEED_MOD);

    // Clear the next activity time
    memory->next_activity_time = 0.0f;

    // Clear the next wander time
    memory->max_wander_time = 0.0f;
  } break;

  case ALIEN_LANDING_AT_HOME: {
    // mprintf(0,"Landing at home mode set.\n");

    // If currently landed, set take off velocity
    DoTakeoff(me, 8.0f, 3.0f);

    // Turn off dodging
    flags = AIF_DODGE;
    AI_Value(me, VF_CLEAR_FLAGS, AIV_I_FLAGS, &flags);

    // Turn off friend avoidance
    flags = AIF_AUTO_AVOID_FRIENDS;
    AI_Value(me, VF_CLEAR_FLAGS, AIV_I_FLAGS, &flags);
    SetCircleDist(me, memory->base_circle_distance);

    // Turn on point-collide-walls and lock axes
    flags = PF_POINT_COLLIDE_WALLS | PF_LOCK_MASK;
    Obj_Value(me, VF_SET_FLAGS, OBJV_I_PHYSICS_FLAGS, &flags);

    // Turn off auto-level
    flags = PF_LEVELING;
    Obj_Value(me, VF_CLEAR_FLAGS, OBJV_I_PHYSICS_FLAGS, &flags);

    // Make it aware
    flags = AIF_FORCE_AWARENESS;
    AI_Value(me, VF_SET_FLAGS, AIV_I_FLAGS, &flags);

    // Set movement type back to physics for flying around
    movement_type = MT_PHYSICS;
    Obj_Value(me, VF_SET, OBJV_C_MOVEMENT_TYPE, &movement_type);

    // Make sure squad broken flag isn't set
    memory->squad_flags &= ~ALIEN_BROKEN;

    // Make him land at his home point
    AI_AddGoal(me, AIG_GET_TO_POS, 2, 100.0f, ALIEN_GUID_LANDED,
               GF_NONFLUSHABLE | GF_KEEP_AT_COMPLETION | GF_ORIENT_SCRIPTED, &memory->home_pos, memory->home_room);
    AI_SetGoalCircleDist(me, 2, 0.0f);

    // Play his landing animation
    Obj_SetCustomAnim(me, 50, 72, 2.5f, 0, -1, -1);

    // Set landing speed
    SetMaxSpeed(me, memory->base_speed * ALIEN_LAND_SPEED_MOD);

    // Clear the next activity time
    memory->next_activity_time = 0.0f;

    // Clear the next wander time
    memory->max_wander_time = 0.0f;
  } break;

  case ALIEN_LANDED:
  case ALIEN_LANDED_RESTING:
  case ALIEN_LANDED_HEALING:
  case ALIEN_LANDED_DEPOSITING:
  case ALIEN_LANDED_WITHDRAWING: {
    // mprintf(0,"Landed mode set.\n");

    // Turn off dodging
    flags = AIF_DODGE;
    AI_Value(me, VF_CLEAR_FLAGS, AIV_I_FLAGS, &flags);

    // Turn off friend avoidance
    flags = AIF_AUTO_AVOID_FRIENDS;
    AI_Value(me, VF_CLEAR_FLAGS, AIV_I_FLAGS, &flags);
    SetCircleDist(me, memory->base_circle_distance);

    // Turn off auto-level
    flags = PF_LEVELING;
    Obj_Value(me, VF_CLEAR_FLAGS, OBJV_I_PHYSICS_FLAGS, &flags);

    // Turn on point-collide-walls and lock axes
    flags = PF_POINT_COLLIDE_WALLS | PF_LOCK_MASK;
    Obj_Value(me, VF_SET_FLAGS, OBJV_I_PHYSICS_FLAGS, &flags);

    // Get rid of forced awareness
    flags = AIF_FORCE_AWARENESS;
    AI_Value(me, VF_CLEAR_FLAGS, AIV_I_FLAGS, &flags);

    // Get rid of persistance
    flags = AIF_PERSISTANT;
    AI_Value(me, VF_CLEAR_FLAGS, AIV_I_FLAGS, &flags);

    // Set to barely aware
    float awareness = AWARE_BARELY;
    AI_Value(me, VF_SET, AIV_F_AWARENESS, &awareness);

    // No movement allowed
    movement_type = MT_NONE;
    Obj_Value(me, VF_SET, OBJV_C_MOVEMENT_TYPE, &movement_type);

    // Set FOV to nearly 0
    // float sleep_fov = 1.0f;
    // AI_Value(me, VF_SET, AIV_F_FOV, &sleep_fov);

    // Make sure squad broken flag isn't set
    memory->squad_flags &= ~ALIEN_BROKEN;

    // If just landed, set it to the pending landed mode, and play looping idle anim
    if (mode == ALIEN_LANDED) {
      mode = memory->pending_mode;
      if (!IsLandedMode(mode)) {
        mode = ALIEN_LANDED_RESTING;
      }

      // Play his landed animation
      Obj_SetCustomAnim(me, 1, 10, 1, 0, -1, -1);
    }
    memory->pending_mode = ALIEN_LANDED_RESTING;
    memory->previous_mode = ALIEN_GOING_HOME;

    // Set landed speed
    SetMaxSpeed(me, 0.0f);

    // Set the next activity time
    if (mode == ALIEN_LANDED_RESTING) {
      memory->next_activity_time = Game_GetTime() + ((float)rand() / (float)D3_RAND_MAX) * 3.0f + 2.0f;
      memory->ok_to_deposit = true;
    } else {
      memory->next_activity_time = Game_GetTime() + ALIEN_LANDED_ACTION_TIME;
    }

    // Clear the next wander time
    memory->max_wander_time = 0.0f;
  } break;

  case ALIEN_HUNTING: {
    // mprintf(0,"Hunting mode set.\n");

    // If currently landed, set take off velocity
    DoTakeoff(me, 8.0f, 3.0f);

    // Turn on friend avoidance
    flags = AIF_AUTO_AVOID_FRIENDS;
    AI_Value(me, VF_SET_FLAGS, AIV_I_FLAGS, &flags);

    // Only avoid friends alot if not in a squad
    if (memory->squad_flags == 0) {
      float avoid_dist = ALIEN_NORMAL_AVOID_DIST;
      AI_Value(me, VF_SET, AIV_F_AVOID_FRIENDS_DIST, &avoid_dist);
      SetCircleDist(me, memory->base_circle_distance);
    } else {
      float avoid_dist = ALIEN_SQUAD_AVOID_DIST;
      AI_Value(me, VF_SET, AIV_F_AVOID_FRIENDS_DIST, &avoid_dist);
      SetCircleDist(me, memory->base_circle_distance * ALIEN_SQUAD_CIRC_DIST_MOD);
    }

    // Make it aware
    flags = AIF_FORCE_AWARENESS;
    AI_Value(me, VF_SET_FLAGS, AIV_I_FLAGS, &flags);

    // Clear wall point collision and axes lock
    flags = PF_POINT_COLLIDE_WALLS;
    Obj_Value(me, VF_CLEAR_FLAGS, OBJV_I_PHYSICS_FLAGS, &flags);

    // Set movement type back to physics for flying around
    movement_type = MT_PHYSICS;
    Obj_Value(me, VF_SET, OBJV_C_MOVEMENT_TYPE, &movement_type);

    // Make sure squad broken flag isn't set
    memory->squad_flags &= ~ALIEN_BROKEN;

    // Only set goals if not currently in squad formation
    if (!(memory->squad_flags & ALIEN_SQUADIE)) {
      SetWanderGoal(me);
    }

    // Set previous mode so alien can return to this if it needs to
    memory->previous_mode = ALIEN_HUNTING;

    // Set returning home speed
    SetMaxSpeed(me, memory->base_speed * ALIEN_HUNT_SPEED_MOD);

    // Set the next activity check to happen within .4 to .8 seconds
    memory->next_activity_time = 0.0f;

    // Set the max time to wander between 20 and 40 seconds
    memory->max_wander_time = ((float)rand() / (float)D3_RAND_MAX) * 20.0f + 40.0f;
  } break;

  case ALIEN_SCAVENGING: {
    // mprintf(0,"Scavenge mode set.\n");

    // If currently landed, set take off velocity
    DoTakeoff(me, 8.0f, 3.0f);

    // Turn on friend avoidance
    flags = AIF_AUTO_AVOID_FRIENDS;
    AI_Value(me, VF_SET_FLAGS, AIV_I_FLAGS, &flags);

    // Only avoid friends alot if not in a squad
    if (memory->squad_flags == 0) {
      float avoid_dist = ALIEN_NORMAL_AVOID_DIST;
      AI_Value(me, VF_SET, AIV_F_AVOID_FRIENDS_DIST, &avoid_dist);
      SetCircleDist(me, memory->base_circle_distance);
    } else {
      float avoid_dist = ALIEN_SQUAD_AVOID_DIST;
      AI_Value(me, VF_SET, AIV_F_AVOID_FRIENDS_DIST, &avoid_dist);
      SetCircleDist(me, memory->base_circle_distance * ALIEN_SQUAD_CIRC_DIST_MOD);
    }

    // Make it aware
    flags = AIF_FORCE_AWARENESS;
    AI_Value(me, VF_SET_FLAGS, AIV_I_FLAGS, &flags);

    // Clear wall point collision and axes lock
    flags = PF_POINT_COLLIDE_WALLS;
    Obj_Value(me, VF_CLEAR_FLAGS, OBJV_I_PHYSICS_FLAGS, &flags);

    // Set movement type back to physics for flying around
    movement_type = MT_PHYSICS;
    Obj_Value(me, VF_SET, OBJV_C_MOVEMENT_TYPE, &movement_type);

    // Make sure squad broken flag isn't set
    memory->squad_flags &= ~ALIEN_BROKEN;

    // Only set goals if not currently in squad formation
    if (!(memory->squad_flags & ALIEN_SQUADIE)) {
      SetWanderGoal(me);
    }

    // Set previous mode so alien can return to this if it needs to
    memory->previous_mode = ALIEN_SCAVENGING;

    // Set returning home speed
    SetMaxSpeed(me, memory->base_speed * ALIEN_SCAV_SPEED_MOD);

    // Set the next activity check to happen within .4 to .8 seconds
    memory->next_activity_time = Game_GetTime() + ((float)rand() / (float)D3_RAND_MAX) * 3.0f + 3.0f;

    // Set the max time to wander between 20 and 40 seconds
    memory->max_wander_time = ((float)rand() / (float)D3_RAND_MAX) * 10.0f + 10.0f;
  } break;

  case ALIEN_ATTACKING_MELEE: {
    // mprintf(0,"Melee mode set.\n");

    // If currently landed, set take off velocity
    DoTakeoff(me, 8.0f, 3.0f);

    // Turn on friend avoidance
    flags = AIF_AUTO_AVOID_FRIENDS;
    AI_Value(me, VF_SET_FLAGS, AIV_I_FLAGS, &flags);

    // Only avoid friends alot if not in a squad or if in a broken squad
    if ((memory->squad_flags == 0) || (memory->squad_flags & ALIEN_BROKEN)) {
      float avoid_dist = ALIEN_NORMAL_AVOID_DIST;
      AI_Value(me, VF_SET, AIV_F_AVOID_FRIENDS_DIST, &avoid_dist);
      SetCircleDist(me, memory->base_circle_distance);
    } else {
      float avoid_dist = ALIEN_SQUAD_AVOID_DIST;
      AI_Value(me, VF_SET, AIV_F_AVOID_FRIENDS_DIST, &avoid_dist);
      SetCircleDist(me, memory->base_circle_distance * ALIEN_SQUAD_CIRC_DIST_MOD);
    }

    // Make it aware
    flags = AIF_FORCE_AWARENESS;
    AI_Value(me, VF_SET_FLAGS, AIV_I_FLAGS, &flags);

    // Clear wall point collision and axes lock
    flags = PF_POINT_COLLIDE_WALLS;
    Obj_Value(me, VF_CLEAR_FLAGS, OBJV_I_PHYSICS_FLAGS, &flags);

    // Set movement type back to physics for flying around
    movement_type = MT_PHYSICS;
    Obj_Value(me, VF_SET, OBJV_C_MOVEMENT_TYPE, &movement_type);

    // Only set goals if not currently in squad formation
    if (!(memory->squad_flags & ALIEN_SQUADIE) || (memory->squad_flags & ALIEN_BROKEN)) {
      AI_SafeSetType(me, AIT_MELEE1);
    }

    // Set Engage and Attack Speeds
    if ((memory->squad_flags == 0) || (memory->squad_flags & ALIEN_BROKEN)) {
      SetMaxSpeed(me, memory->base_speed * ALIEN_ATTACK_SPEED_MOD);
    } else {
      SetMaxSpeed(me, memory->base_speed * ALIEN_ENGAGE_SPEED_MOD);
    }

    // Clear the next activity time
    memory->next_activity_time = Game_GetTime() + ((float)rand() / (float)D3_RAND_MAX) * 0.5f + 0.7f;

    // Clear the next wander time
    memory->max_wander_time = 0.0f;
  } break;

  case ALIEN_ATTACKING_RANGED: {
    // mprintf(0,"Ranged mode set.\n");

    // If currently landed, set take off velocity
    DoTakeoff(me, 8.0f, 3.0f);

    // Turn on friend avoidance
    flags = AIF_AUTO_AVOID_FRIENDS;
    AI_Value(me, VF_SET_FLAGS, AIV_I_FLAGS, &flags);

    // Set friend avoidance distance
    float avoid_dist = ALIEN_NORMAL_AVOID_DIST;
    AI_Value(me, VF_SET, AIV_F_AVOID_FRIENDS_DIST, &avoid_dist);
    SetCircleDist(me, memory->base_circle_distance);

    // Make it aware
    flags = AIF_FORCE_AWARENESS;
    AI_Value(me, VF_SET_FLAGS, AIV_I_FLAGS, &flags);

    // Clear wall point collision and axes lock
    flags = PF_POINT_COLLIDE_WALLS;
    Obj_Value(me, VF_CLEAR_FLAGS, OBJV_I_PHYSICS_FLAGS, &flags);

    // Set movement type back to physics for flying around
    movement_type = MT_PHYSICS;
    Obj_Value(me, VF_SET, OBJV_C_MOVEMENT_TYPE, &movement_type);

    // Make sure squad broken flag isn't set
    memory->squad_flags &= ~ALIEN_BROKEN;

    // Enable ranged firing
    flags = AIF_DISABLE_FIRING;
    AI_Value(me, VF_CLEAR_FLAGS, AIV_I_FLAGS, &flags);

    // Only set goals if not currently in squad formation
    AI_SafeSetType(me, AIT_EVADER1);

    // Set Engage and Attack Speeds
    SetMaxSpeed(me, memory->base_speed * ALIEN_ATTACK_SPEED_MOD);

    // Clear the next activity time
    memory->next_activity_time = 0.0f;

    // Clear the next wander time
    memory->max_wander_time = 0.0f;
  } break;

  case ALIEN_FLEEING: {
    // mprintf(0,"Fleeing mode set.\n");

    // If currently landed, set take off velocity
    DoTakeoff(me, 8.0f, 3.0f);

    // avoid friends while fleeing
    flags = AIF_AUTO_AVOID_FRIENDS;
    AI_Value(me, VF_SET_FLAGS, AIV_I_FLAGS, &flags);

    // Set friend avoidance distance
    float avoid_dist = ALIEN_NORMAL_AVOID_DIST;
    AI_Value(me, VF_SET, AIV_F_AVOID_FRIENDS_DIST, &avoid_dist);
    SetCircleDist(me, memory->base_circle_distance);

    // Make it aware
    flags = AIF_FORCE_AWARENESS;
    AI_Value(me, VF_SET_FLAGS, AIV_I_FLAGS, &flags);

    // Clear wall point collision and axes lock
    flags = PF_POINT_COLLIDE_WALLS;
    Obj_Value(me, VF_CLEAR_FLAGS, OBJV_I_PHYSICS_FLAGS, &flags);

    // Set movement type back to physics for flying around
    movement_type = MT_PHYSICS;
    Obj_Value(me, VF_SET, OBJV_C_MOVEMENT_TYPE, &movement_type);

    // Make sure squad broken flag isn't set
    memory->squad_flags &= ~ALIEN_BROKEN;

    // Set Goal to go somewhere else
    SetWanderGoal(me);

    // Set Fleeing Speed
    SetMaxSpeed(me, memory->base_speed * ALIEN_FLEE_SPEED_MOD);

    // Clear the next activity time
    memory->next_activity_time = 0.0f;

    // Set the max time to flee (base time somewhat off of damage)
    float curr_shields, percent_damaged;
    Obj_Value(me, VF_GET, OBJV_F_SHIELDS, &curr_shields);
    percent_damaged = (memory->base_shields - curr_shields) / memory->base_shields;

    memory->max_wander_time = ((float)rand() / (float)D3_RAND_MAX) * 3.0f + 3.0f + (percent_damaged * 6.0f);
  } break;

  default:
    mprintf(0, "AlienOrganism is all messed up!\n");
  }

  // If following leader in formation and mode supports squads, assign follow goals
  if ((memory->squad_flags & ALIEN_SQUADIE) && !(memory->squad_flags & ALIEN_BROKEN) &&
      ModeSupportsSquadMovement(mode)) {
    SetSquadieFormationGoal(me);
  }

  // Reset the landing flags
  memory->done_moving = false;
  memory->done_turning = false;

  // Set the current mode and reset the mode time
  memory->mode = mode;
  memory->mode_time = 0.0f;
}

// Handles alien takeoffs
void AlienOrganism::DoTakeoff(int me, float takeoff_speed, float speed_variance) {
  // If currently landed, set take off velocity
  if (IsLandedMode(memory->mode)) {
    vector vel;

    Obj_Value(me, VF_GET, OBJV_V_VELOCITY, &vel);
    vel += (memory->home_uvec * (takeoff_speed + ((float)rand() / (float)D3_RAND_MAX) * speed_variance));
    Obj_Value(me, VF_SET, OBJV_V_VELOCITY, &vel);

    // Play the takeoff anim and tell it to go to alert next
    Obj_SetCustomAnim(me, 10, 31, 1, AIAF_NOTIFY, -1, AS_ALERT);

    // Reset the takeoff time
    memory->takeoff_time = 0.0f;
  }
}

// Processes the AI Initialize event
void AlienOrganism::DoInit(int me) {
  int flags;
  msafe_struct m;
  m.objhandle = me;

  // Setup memory data
  tOSIRISMEMCHUNK ch;
  ch.id = ALIEN_MEMORY_ID;
  ch.size = sizeof(alienorganism_data);
  ch.my_id.type = OBJECT_SCRIPT;
  ch.my_id.objhandle = me;

  memory = (alienorganism_data *)Scrpt_MemAlloc(&ch);

  // Init base values
  AI_Value(me, VF_GET, AIV_F_MAX_SPEED, &memory->base_speed);
  AI_Value(me, VF_GET, AIV_F_CIRCLE_DIST, &memory->base_circle_distance);
  Obj_Value(me, VF_GET, OBJV_F_SHIELDS, &memory->base_shields);

  // Init energy charge
  memory->energy_charges = 0.0f;
  memory->ok_to_deposit = true;

  // All beams off initially
  memory->enabled_beams = 0;

  // Clear the destination object handle
  memory->dest_object_handle = OBJECT_HANDLE_NONE;

  // Alien hasn't been hit by player yet
  memory->hit_by_player = false;

  // Alien hasn't taken off yet
  memory->takeoff_time = ALIEN_MAX_TAKEOFF_TIME;

  // Setup Squad Information
  memory->squad_flags = 0;
  memory->leader_handle = OBJECT_HANDLE_NONE;
  memory->num_teammates = 0;

  // .1 to 1.1 seconds into the level, do the first squad matching
  memory->next_update_squad_time = Game_GetTime() + ((float)rand() / (float)D3_RAND_MAX) * 2.0f + 0.1f;

  // Update the energy effect as soon as charge exists
  memory->next_update_energy_time = Game_GetTime();
  memory->next_update_beam_time = Game_GetTime();

  // Update the squadie catchup speed as soon as possible
  memory->next_squadie_update_time = Game_GetTime();

  // Do special damage as soon as relevant
  memory->next_special_damage_time = Game_GetTime();

  // Set the next generic check time
  memory->next_generic_check_time = Game_GetTime() + ((float)rand() / (float)D3_RAND_MAX) * 1.0f + 0.5f;
  memory->next_vis_check_time = Game_GetTime() + ((float)rand() / (float)D3_RAND_MAX) * 2.0f + 1.0f;

  // Init other times
  memory->next_activity_time = Game_GetTime();
  memory->max_wander_time = 0.0f;

  // Store Home Information
  Obj_Value(me, VF_GET, OBJV_V_POS, &memory->home_pos);
  Obj_Value(me, VF_GET, OBJV_I_ROOMNUM, &memory->home_room);

  matrix orient;
  Obj_Value(me, VF_GET, OBJV_M_ORIENT, &orient);
  memory->home_fvec = orient.fvec;
  vm_VectorNormalize(&memory->home_fvec);
  memory->home_uvec = orient.uvec;
  vm_VectorNormalize(&memory->home_uvec);

  // Check and see whether we've been spawned from a matcen or not
  bool spawned_from_matcen = false;
  char type;
  AI_Value(me, VF_GET, AIV_C_ANIMATION_TYPE, &type);
  if (type == AS_BIRTH) {
    mprintf(0, "Alien organism produced by matcen.\n");
    // Note that we came from a matcen
    spawned_from_matcen = true;

    // Find a new home
    FindHome(me);
  }

  // Init squadie goal positions
  memory->squad_goal_pos = memory->home_pos;
  memory->squad_goal_room = memory->home_room;

  // Create and setup the energy effect props
  msafe_struct mstruct;

  memory->pos1_handle = Obj_Create(OBJ_POWERUP, powerup_id, memory->home_room, &memory->home_pos, NULL, me);
  mstruct.objhandle = memory->pos1_handle;
  MSafe_CallFunction(MSAFE_OBJECT_NO_RENDER, &mstruct);

  memory->pos2_handle = Obj_Create(OBJ_POWERUP, powerup_id, memory->home_room, &memory->home_pos, NULL, me);
  mstruct.objhandle = memory->pos2_handle;
  MSafe_CallFunction(MSAFE_OBJECT_NO_RENDER, &mstruct);

  memory->pos3_handle = Obj_Create(OBJ_POWERUP, powerup_id, memory->home_room, &memory->home_pos, NULL, me);
  mstruct.objhandle = memory->pos3_handle;
  MSafe_CallFunction(MSAFE_OBJECT_NO_RENDER, &mstruct);

  // Set starting mode
  if (spawned_from_matcen) {
    // Set our starting mode to landed-resting
    memory->previous_mode = ALIEN_GOING_HOME;
    memory->pending_mode = ALIEN_LANDED_RESTING;
    SetMode(me, ALIEN_ATTACKING_MELEE);
  } else {
    // Set our starting mode to landed-resting
    memory->pending_mode = ALIEN_LANDED_RESTING;
    SetMode(me, ALIEN_LANDED);
  }
}

// Tries to find a home for an alien (returns false if this fails)
#define MAX_HOME_FINDING_ATTEMPTS 20
#define MAX_HOME_FINDING_DIST 1000
#define HOME_LOOK_OFFSET 0.18f // for a 40 degree search pyramid
#define HOME_PLACE_DISTANCE 1.0f
bool AlienOrganism::FindHome(int me) {
  int num_attempts;
  bool home_found;
  vector start_pos, target_pos, landed_pos, home_dir;
  int start_room, landed_room;
  matrix start_orient;
  int flags, fate;
  ray_info ray;
  float size;

  // Find start point
  Obj_Value(me, VF_GET, OBJV_V_POS, &start_pos);
  Obj_Value(me, VF_GET, OBJV_I_ROOMNUM, &start_room);
  Obj_Value(me, VF_GET, OBJV_M_ORIENT, &start_orient);
  Obj_Value(me, VF_GET, OBJV_F_SIZE, &size);

  home_found = false;
  num_attempts = 0;
  while (!home_found && num_attempts < MAX_HOME_FINDING_ATTEMPTS) {
    // Determine ray angle
    home_dir = start_orient.fvec;
    home_dir += start_orient.uvec * (HOME_LOOK_OFFSET - ((float)rand() / (float)D3_RAND_MAX) * 2.0f * HOME_LOOK_OFFSET);
    home_dir += start_orient.rvec * (HOME_LOOK_OFFSET - ((float)rand() / (float)D3_RAND_MAX) * 2.0f * HOME_LOOK_OFFSET);

    // Cast home-finding ray
    target_pos = start_pos + (home_dir * MAX_HOME_FINDING_DIST);
    flags = FQ_CHECK_OBJS | FQ_IGNORE_POWERUPS | FQ_IGNORE_WEAPONS | FQ_IGNORE_MOVING_OBJECTS |
            FQ_IGNORE_NON_LIGHTMAP_OBJECTS;
    fate = FVI_RayCast(me, &start_pos, &target_pos, start_room, size, flags, &ray);

    // Check if it hit a face
    if (fate == HIT_WALL) {
      matrix temp;

      memory->home_pos = ray.hit_face_pnt;
      memory->home_room = ray.hit_face_room;
      memory->home_uvec = ray.hit_wallnorm;

      vm_VectorToMatrix(&temp, &memory->home_uvec, NULL, NULL);
      if (rand() % 100 > 50) {
        memory->home_fvec = temp.uvec;
      } else {
        memory->home_fvec = temp.rvec;
      }
      if (rand() % 100 > 50)
        memory->home_fvec = -memory->home_fvec;

      // Place the object a little away from the wall
      target_pos = memory->home_pos + (memory->home_uvec * HOME_PLACE_DISTANCE);
      flags = FQ_CHECK_OBJS | FQ_IGNORE_POWERUPS | FQ_IGNORE_WEAPONS | FQ_IGNORE_MOVING_OBJECTS |
              FQ_IGNORE_NON_LIGHTMAP_OBJECTS;
      FVI_RayCast(me, &memory->home_pos, &target_pos, memory->home_room, 0.0f, flags, &ray);

      memory->home_pos = ray.hit_point;
      memory->home_room = ray.hit_room;

      home_found = true;
    }

    num_attempts++;
  }

  return home_found;
}

void AlienOrganism::DoSquadieFrame(int me) {
  vector my_pos;
  int type;

  // Check if the leader is still alive
  Obj_Value(memory->leader_handle, VF_GET, OBJV_I_TYPE, &type);
  if (type == OBJ_NONE) {
    memory->squad_flags = 0;
    memory->leader_handle = OBJECT_HANDLE_NONE;
    memory->num_teammates = 0;

    SetMode(me, memory->mode);
    return;
  }

  // If current mode disallows formation following, get outta here
  if (!ModeSupportsSquadMovement(memory->mode))
    return;

  // If squad is currently broken, get outta here
  if (memory->squad_flags & ALIEN_BROKEN)
    return;

  // The leader is alive, so set new goal pos
  UpdateSquadieFormationGoal(me);

  // Check if it's time to update the catchup speed
  if (Game_GetTime() >= memory->next_squadie_update_time) {
    memory->next_squadie_update_time = Game_GetTime() + ALIEN_SQUADIE_INTERVAL;

    // Get the distance to leader
    Obj_Value(me, VF_GET, OBJV_V_POS, &my_pos);
    float dist = vm_VectorDistance(&my_pos, &memory->squad_goal_pos);

    // If squadie is in leader's path, slow down until leader passes
    float slowdown = 1.0f;
    if (dist <= ALIEN_APPROACH_DIST) {
      matrix leader_orient;
      vector dir, leader_pos;

      // See if we are in front of leader
      Obj_Value(memory->leader_handle, VF_GET, OBJV_M_ORIENT, &leader_orient);
      dir = my_pos - memory->squad_goal_pos;
      vm_VectorNormalize(&dir);
      float dot_prod;
      if ((dot_prod = leader_orient.fvec * dir) < 0.0f) {
        slowdown = dist / ALIEN_APPROACH_DIST;
      }
    }

    // If squadie is falling behind, let him speed up so he can catch up to the squad
    if (dist <= ALIEN_CATCHUP_DIST) {
      float catchup_speed;
      AI_Value(memory->leader_handle, VF_GET, AIV_F_MAX_SPEED, &catchup_speed);

      float scaled_speed = dist / ALIEN_CATCHUP_DIST * ALIEN_CATCHUP_SPEED_MOD * catchup_speed;
      catchup_speed += scaled_speed;
      catchup_speed *= slowdown;

      AI_Value(me, VF_SET, AIV_F_MAX_SPEED, &catchup_speed);
    } else if (dist > ALIEN_CATCHUP_DIST) {
      float catchup_speed;
      AI_Value(memory->leader_handle, VF_GET, AIV_F_MAX_SPEED, &catchup_speed);

      float scaled_speed = ALIEN_CATCHUP_SPEED_MOD * catchup_speed;
      catchup_speed += scaled_speed;
      catchup_speed *= slowdown;

      AI_Value(me, VF_SET, AIV_F_MAX_SPEED, &catchup_speed);
    }
  }
}

// Processes the AI Frame Interval Event
void AlienOrganism::DoFrame(int me) {
  vector uvec;
  int flags;
  float anim_frame;
  int new_mode;
  vector vel;
  float last_see_time, last_see_game_time;
  float last_hear_time, last_hear_game_time;
  float last_perceive_time;

  /*
  float shields;
  Obj_Value(me, VF_GET, OBJV_F_SHIELDS, &shields);
  mprintf(0,"Shields: %.2f\n",shields);
  */

  // Get the last see and hear time
  AI_Value(me, VF_GET, AIV_F_LAST_SEE_TARGET_TIME, &last_see_game_time);
  AI_Value(me, VF_GET, AIV_F_LAST_HEAR_TARGET_TIME, &last_hear_game_time);

  // Calculate the time since target was seen and heard
  last_hear_time = Game_GetTime() - last_hear_game_time;
  last_see_time = Game_GetTime() - last_see_game_time;

  // Perceive time is last time target was seen or heard (whichever is smalled)
  if (last_hear_time <= last_see_time) {
    last_perceive_time = last_hear_time;
  } else {
    last_perceive_time = last_see_time;
  }

  // Check if takeoff time is active
  if (memory->takeoff_time < ALIEN_MAX_TAKEOFF_TIME) {
    memory->takeoff_time += Game_GetFrameTime();

    // If the time is up, clear the landing lock flags
    if (memory->takeoff_time >= ALIEN_MAX_TAKEOFF_TIME) {
      // Make sure we're in a mode that wants these flags cleared
      if (memory->mode != ALIEN_LANDING_AT_HOME && !IsLandedMode(memory->mode)) {
        // Clear wall point collision and axes lock
        flags = PF_LOCK_MASK | PF_POINT_COLLIDE_WALLS;
        Obj_Value(me, VF_CLEAR_FLAGS, OBJV_I_PHYSICS_FLAGS, &flags);

        // Turn on dodging
        flags = AIF_DODGE;
        AI_Value(me, VF_SET_FLAGS, AIV_I_FLAGS, &flags);

        // enable auto-leveling
        flags = PF_LEVELING;
        Obj_Value(me, VF_SET_FLAGS, OBJV_I_PHYSICS_FLAGS, &flags);

        // mprintf(0,"Alien landing flags cleared...\n");
      }
    }
  }

  // If alien was hit by player, take action if necessary
  if (memory->hit_by_player) {
    // See if the mode we're in is sensitive to attacks
    if (ModeIsAttackSensitive(memory->mode, memory->pending_mode)) {
      // mprintf(0,"Sensitive to Attack!\n");
      if (memory->squad_flags & ALIEN_SQUADIE) {
        SendCommand(me, memory->leader_handle, ALIEN_COM_I_WAS_HIT, NULL);
      } else {
        MeleeAttack(me);
      }
    }

    // reset the hit by player flag
    memory->hit_by_player = false;
  }

  // Periodically update the squad information (remove dead teammates and/or recruit new ones)
  if (memory->next_update_squad_time <= Game_GetTime()) {
    memory->next_update_squad_time = Game_GetTime() + ((float)rand() / (float)D3_RAND_MAX) * 3.0f + 3.0f;
    UpdateSquad(me);
  }

  // Periodically update the squad visibility information
  if (memory->next_vis_check_time <= Game_GetTime()) {
    memory->next_vis_check_time = Game_GetTime() + ((float)rand() / (float)D3_RAND_MAX) * 1.0f + 1.0f;

    // If we're leading, update the squad's visibility flags
    if (memory->squad_flags & ALIEN_LEADER) {
      UpdateSquadVisibility(me);
    }
  }

  // Update squadies
  if (memory->squad_flags & ALIEN_SQUADIE) {
    DoSquadieFrame(me);
  }

  // Get the current animation frame
  Obj_Value(me, VF_GET, OBJV_F_ANIM_FRAME, &anim_frame);

  // Handle generic checks (pertains to more than 1 mode)
  if (memory->next_generic_check_time <= Game_GetTime()) {
    memory->next_generic_check_time = Game_GetTime() + ((float)rand() / (float)D3_RAND_MAX) * 1.4f + 0.6f;

    // If we're not already landed, or heading home to heal, see if we should
    if (!IsLandedMode(memory->mode) && memory->mode != ALIEN_LANDING_AT_HOME &&
        memory->pending_mode != ALIEN_LANDED_HEALING) {
      float current_shields;
      Obj_Value(me, VF_GET, OBJV_F_SHIELDS, &current_shields);

      // If we're pretty heavily damaged, see if we should return home to heal
      if ((current_shields < memory->base_shields * ALIEN_HEAL_PERCENTAGE) && (rand() % 100 < 80)) {
        Heal(me);
      }
    }

    // If we're not already fleeing or going home to heal, check to see if we should flee
    if (memory->mode != ALIEN_FLEEING && memory->mode != ALIEN_LANDING_AT_HOME &&
        memory->mode != ALIEN_LANDED_HEALING && memory->pending_mode != ALIEN_LANDED_HEALING) {
      if (IsOnFire(me)) {
        // If we're on fire, there's a good chance that we will try to flee
        if ((rand() % 100) < ALIEN_ON_FIRE_FLEE_CHANCE)
          Flee(me);
      } else if (IsNearFire(me)) {
        // If we're near fire, there's a chance that we will try to flee
        if ((rand() % 100) < ALIEN_NEAR_FIRE_FLEE_CHANCE)
          Flee(me);
      }
    }

    // If we're landed, see if we want to play the landed animation
    if (IsLandedMode(memory->mode) && anim_frame == 10 && (rand() % 100) < 15) {
      Obj_SetCustomAnim(me, 1, 10, 1, 0, -1, -1);
    }
  }

  // Handle mode specific operations
  switch (memory->mode) {
  case ALIEN_LANDED_RESTING: {
    // See if it should consider doing something spontaneously
    if (Game_GetTime() >= memory->next_activity_time) {
      // Set the next time to think about doing something (4-6 seconds)
      memory->next_activity_time = Game_GetTime() + ((float)rand() / (float)D3_RAND_MAX) * 1.0f + 0.5f;

      // If alien has a target, decide whether it should go hunt it down
      if (last_perceive_time < 3.0f) {
        int target_handle;
        int die_roll = 0;

        AI_Value(me, VF_GET, AIV_I_TARGET_HANDLE, &target_handle);
        if (target_handle != OBJECT_HANDLE_NONE) {
          die_roll = (rand() % 90) + 1;

          float dist;
          AI_Value(me, VF_GET, AIV_F_DIST_TO_TARGET, &dist);

          // Adjust the random value by distance to target
          // (ie the closer the target, the more likely it will be attacked)
          if (dist > 300.0f)
            die_roll -= 50;
          else if (dist > 200.0f)
            die_roll -= 45;
          else if (dist > 100.0f)
            die_roll -= 40;
          else if (dist > 70.0f)
            die_roll -= 30;
          else if (dist > 50.0f)
            die_roll -= 20;
          else if (dist > 25.0f)
            die_roll += 5;
          else
            die_roll += 50;

          // Grant bonuses for recent seeing and hearing
          if (last_see_time < 3.0)
            die_roll += 5;

          if (last_hear_time < 4.0f)
            die_roll += 50;

          // mprintf(0,"hear: %.1f, see: %.1f, perceive: %.1f\n",last_hear_time,last_see_time,last_perceive_time);
          // mprintf(0,"Doing check...%d - %.1f\n",die_roll,dist);
        }

        if (die_roll > 70) {
          // If we're a squadie, lie and tell the leader we got hit (so he orders the attack)
          if (memory->squad_flags & ALIEN_SQUADIE) {
            SendCommand(me, memory->leader_handle, ALIEN_COM_I_WAS_HIT, NULL);
          } else {
            if (MeleeAttack(me))
              break;
          }
        }
      }

      // If we currently have any charges, go ahead and deposit them
      if (memory->energy_charges > 0.0f) {
        // If we're a squadie, tell the leader we are leaving
        if (memory->squad_flags & ALIEN_SQUADIE) {
          LeaveLeader(me);
        }

        // If we're a leader, disperse the squad
        if (memory->squad_flags & ALIEN_LEADER) {
          DisperseSquad(me);
        }

        SetMode(me, ALIEN_LANDED_DEPOSITING);
        break;
      }

      /*
      // Only think if it's not a squadie (its leader will decide when/whether to do something)
      if(!(memory->squad_flags & ALIEN_SQUADIE))
      {
              int die_roll = (rand()%100) + 1;

              // See what to do
              if(die_roll <= 20)	// Go hunting
              {
                      // Start hunting
                      SetMode(me, ALIEN_HUNTING);

                      // If it's a leader, order squad to switch to hunting mode
                      if(memory->squad_flags & ALIEN_LEADER) {
                              int mode=ALIEN_HUNTING;
                              IssueSquadOrder(me, ALIEN_COM_SET_MODE, &mode);
                      }
              }
              else if(die_roll <= 30)	// Go scavenging
              {
                      // Start scavenging
                      SetMode(me, ALIEN_SCAVENGING);

                      // If it's a leader, order squad to switch to scavenging mode
                      if(memory->squad_flags & ALIEN_LEADER) {
                              int mode=ALIEN_SCAVENGING;
                              IssueSquadOrder(me, ALIEN_COM_SET_MODE, &mode);
                      }
              }
      }
      */
    }
  } break;

  case ALIEN_LANDED_HEALING: {
    // See if we should do a round of healing
    if (Game_GetTime() >= memory->next_activity_time) {
      // Set the next time to do something
      memory->next_activity_time = Game_GetTime() + ALIEN_LANDED_ACTION_TIME;

      // Restore some shields
      float current_shields;
      Obj_Value(me, VF_GET, OBJV_F_SHIELDS, &current_shields);
      current_shields += (memory->base_shields * 0.2f);

      bool done_healing = false;
      if (current_shields >= memory->base_shields) {
        current_shields = memory->base_shields;
        done_healing = true;
      }

      // Set the current shields
      Obj_Value(me, VF_SET, OBJV_F_SHIELDS, &current_shields);
      // mprintf(0,"Healing...\n");

      // If we're all healed up, go to landed resting
      if (done_healing) {
        SetMode(me, ALIEN_LANDED_RESTING);
        // mprintf(0,"Done healing.\n");
      }
    }

    // Handle the energy beam effects
    if (!memory->energy_beams_on)
      CalcEnergyBeamPositions(me);
    UpdateEnergyBeams(me);
  } break;

  case ALIEN_LANDED_DEPOSITING: {
    // See if we should do a round of energy depositing
    if (Game_GetTime() >= memory->next_activity_time) {
      // Set the next time to do something
      memory->next_activity_time = Game_GetTime() + ALIEN_LANDED_ACTION_TIME;

      // Deposit some charges
      memory->energy_charges -= (ALIEN_MAX_ENERGY_CHARGES * 0.25f);

      bool done_depositing = false;
      if (memory->energy_charges <= 0.0f) {
        memory->energy_charges = 0.0f;
        done_depositing = true;
      }

      // mprintf(0,"Depositing...\n");

      // If we're all healed up, go to landed resting
      if (done_depositing) {
        SetMode(me, ALIEN_LANDED_RESTING);
        // mprintf(0,"Done depositing.\n");
      }
    }

    // Handle the energy beam effects
    if (!memory->energy_beams_on)
      CalcEnergyBeamPositions(me);
    UpdateEnergyBeams(me);
  } break;

  case ALIEN_LANDED_WITHDRAWING: {
    // See if we should do a round of energy withdrawing
    if (Game_GetTime() >= memory->next_activity_time) {
      // Set the next time to do something
      memory->next_activity_time = Game_GetTime() + ALIEN_LANDED_ACTION_TIME;

      // Deposit some charges
      memory->energy_charges += (ALIEN_MAX_ENERGY_CHARGES * 0.25f);

      bool done_withdrawing = false;
      if (memory->energy_charges >= ALIEN_MAX_ENERGY_CHARGES) {
        memory->energy_charges = ALIEN_MAX_ENERGY_CHARGES;
        done_withdrawing = true;
      }

      // mprintf(0,"Withdrawing...\n");

      // If we're all healed up, go to melee mode
      if (done_withdrawing) {
        memory->ok_to_deposit = false;
        memory->previous_mode = ALIEN_GOING_HOME;
        memory->pending_mode = ALIEN_LANDED_RESTING;
        SetMode(me, ALIEN_ATTACKING_MELEE);
        // mprintf(0,"Done withdrawing.\n");
      }
    }

    // Handle the energy beam effects
    if (!memory->energy_beams_on)
      CalcEnergyBeamPositions(me);
    UpdateEnergyBeams(me);
  } break;

  case ALIEN_LANDING_AT_HOME: {
    //	mprintf(0, "AlienOrganism Landed at Home\n");
    if (memory->done_turning == true && memory->done_moving == true) {
      SetMode(me, ALIEN_LANDED);
    }
  } break;

  case ALIEN_GOING_HOME: {
    char type, next_type;

    // If we're simply going home to rest, and a target is present, attack it!
    if (memory->pending_mode == ALIEN_LANDED_RESTING && last_perceive_time < 3.0f) {
      if (MeleeAttack(me))
        break;
    }

    // mprintf(0, "AlienOrganism going home\n");

    // If we're in alert, or the next anim mode is alert, and we're done moving, land at home
    if (memory->done_moving == true) {
      AI_Value(me, VF_GET, AIV_C_ANIMATION_TYPE, &type);
      AI_Value(me, VF_GET, AIV_C_NEXT_ANIMATION_TYPE, &next_type);

      if (type == AS_ALERT || type == AS_IDLE || next_type == AS_ALERT || next_type == AS_IDLE) {
        SetMode(me, ALIEN_LANDING_AT_HOME);
      }
    }
  } break;

  case ALIEN_FLEEING: {
    // See if it is time to stop running
    if (memory->mode_time >= memory->max_wander_time) {
      // Go back to melee mode for now
      SetMode(me, ALIEN_ATTACKING_MELEE);
    }
  } break;

  case ALIEN_FINDING_BIRTHPLACE:
    break;

  case ALIEN_GIVING_BIRTH:
    break;

  case ALIEN_HUNTING: {
    // If a target has been spotted, attack it
    if (!(memory->squad_flags & ALIEN_SQUADIE) && last_perceive_time < 3.0f) {
      if (MeleeAttack(me))
        break;
    }

    // See if it is time to go home
    if (memory->mode_time >= memory->max_wander_time) {
      // Set the next max wander time
      memory->max_wander_time = ((float)rand() / (float)D3_RAND_MAX) * 10.0f + 10.0f;

      // If not a squadie, decide to go home
      if (!(memory->squad_flags & ALIEN_SQUADIE)) {
        // If leading squad, tell squad to go home and rest
        if (memory->squad_flags & ALIEN_LEADER) {
          int pending_mode = ALIEN_LANDED_RESTING;
          IssueSquadOrder(me, ALIEN_COM_SET_PENDING_MODE, &pending_mode);

          int mode = ALIEN_GOING_HOME;
          DisperseSquadToMode(me, mode);
        }

        // Go home and rest
        memory->pending_mode = ALIEN_LANDED_RESTING;
        SetMode(me, ALIEN_GOING_HOME);
        break;
      }
    }
  } break;

  case ALIEN_SCAVENGING: {
    // See if it is time to go home
    if (memory->mode_time >= memory->max_wander_time) {
      // Set the next max wander time
      memory->max_wander_time = ((float)rand() / (float)D3_RAND_MAX) * 10.0f + 10.0f;

      // If not a squadie, decide to go home
      if (!(memory->squad_flags & ALIEN_SQUADIE)) {
        // If leading squad, tell squad to go home and rest
        if (memory->squad_flags & ALIEN_LEADER) {
          int pending_mode = ALIEN_LANDED_RESTING;
          IssueSquadOrder(me, ALIEN_COM_SET_PENDING_MODE, &pending_mode);

          int mode = ALIEN_GOING_HOME;
          DisperseSquadToMode(me, mode);
        }

        // Go home and rest
        memory->pending_mode = ALIEN_LANDED_RESTING;
        SetMode(me, ALIEN_GOING_HOME);
        break;
      }
    }
  } break;

  case ALIEN_ATTACKING_MELEE: {
    // If a target hasn't been spotted for a while, return to previous task
    if (!(memory->squad_flags & ALIEN_SQUADIE) && last_perceive_time > 7.0f) {
      int mode = memory->previous_mode;

      // If leading squad, tell squad to return to previous task's mode
      if (memory->squad_flags & ALIEN_LEADER) {
        // If leader is going home, disperse the squad
        if (mode == ALIEN_GOING_HOME) {
          int pending_mode = ALIEN_LANDED_RESTING;
          IssueSquadOrder(me, ALIEN_COM_SET_PENDING_MODE, &pending_mode);
          DisperseSquadToMode(me, mode);
        } else {
          // Otherwise just tell them to switch to the new mode (hunt or scavenge)
          IssueSquadOrder(me, ALIEN_COM_SET_MODE, &mode);
        }
      }

      // Return to previous mode
      SetMode(me, mode);
      break;
    }

    // If leading, check target distance for grouping orders
    if (memory->squad_flags & ALIEN_LEADER) {
      int target_handle;
      AI_Value(me, VF_GET, AIV_I_TARGET_HANDLE, &target_handle);
      if (target_handle != OBJECT_HANDLE_NONE) {
        float dist;
        AI_Value(me, VF_GET, AIV_F_DIST_TO_TARGET, &dist);

        // See if we need to regroup or break up and attack
        if ((dist >= ALIEN_REGROUP_DIST) && (memory->squad_flags & ALIEN_BROKEN)) {
          // Tell Squad to regroup
          IssueSquadOrder(me, ALIEN_COM_REGROUP, NULL);

          // Set up leader for regrouping
          memory->squad_flags &= ~ALIEN_BROKEN;
          SetMode(me, memory->mode);
          break;
        } else if ((dist <= ALIEN_BREAKUP_DIST) && !(memory->squad_flags & ALIEN_BROKEN)) {
          // Tell Squad to breakup and attack!
          IssueSquadOrder(me, ALIEN_COM_BREAK_AND_ATTACK, NULL);

          // Set up leader for regrouping
          memory->squad_flags |= ALIEN_BROKEN;
          SetMode(me, memory->mode);
          break;
        }
      }
    }

    /*
    // If we're a non-broken squadie, and target is closer than leader, break on our own
    if(memory->squad_flags & ALIEN_SQUADIE && !(memory->squad_flags & ALIEN_BROKEN))
    {
            int target_handle;
            AI_Value(me, VF_GET, AIV_I_TARGET_HANDLE, &target_handle);
            if(target_handle != OBJECT_HANDLE_NONE)
            {
            }
    }
    */

    // See if we should decide whether to do any charge related activities
    if (Game_GetTime() >= memory->next_activity_time) {
      // Set the next time to do something
      memory->next_activity_time = Game_GetTime() + ((float)rand() / (float)D3_RAND_MAX) * 0.6f + 0.8f;

      // If we have some charges, see if we should do something with it
      if (memory->energy_charges > 0.0f) {
        int die_roll = rand() % 100;
        if (die_roll < 70) {
          // Switch to ranged mode
          RangedAttack(me);
          break;
        } else if (die_roll < 90 && memory->ok_to_deposit) {
          // Return home and deposit it
          Deposit(me);
          break;
        }
      } else {
        // See if we want to return home and withdrawal some
        int die_roll = rand() % 1000;
        // mprintf(0,"Doing withdrawal check... %d\n",die_roll);
        if (die_roll < 5) // 0.5% chance
        {
          // Possibly factor in a distance to home check here?

          Withdraw(me);
          break;
        }
      }
    }
  } break;

  case ALIEN_ATTACKING_RANGED: {
    // If we've lost target, or if we're out of charges, go back to melee mode
    if (last_perceive_time > 6.0f || memory->energy_charges <= 0.0f) {
      // Return to melee mode
      SetMode(me, ALIEN_ATTACKING_MELEE);
      break;
    }
  } break;
  }

  // Update the energy effect
  UpdateEnergyEffect(me);

  // Increment the current mode time
  memory->mode_time += Game_GetFrameTime();

  /*
  char type, next_type;
  AI_Value(me, VF_GET, AIV_C_ANIMATION_TYPE, &type);
  AI_Value(me, VF_GET, AIV_C_NEXT_ANIMATION_TYPE, &next_type);
  mprintf(0,"Curr anim type: %d, Next anim type: %d\n",type,next_type);
  if(next_type==AS_CUSTOM) mprintf(0,"NEXT TYPE IS CUSTOM!\n\n\n\n\n\n\n");
  */
}

// Process all AI Notify events
bool AlienOrganism::DoNotify(int me, tOSIRISEventInfo *data) {
  if (IsGoalFinishedNotify(data->evt_ai_notify.notify_type)) {
    // Handle goal complete events
    switch (data->evt_ai_notify.goal_uid) {
    case ALIEN_GUID_GOT_HOME:
      memory->done_moving = true;
      break;
    case ALIEN_GUID_LANDED:
      memory->done_moving = true;
      break;
    case ALIEN_GUID_GOT_TO_DEST_OBJ:
      memory->dest_object_handle = OBJECT_HANDLE_NONE;
      SetMode(me, memory->mode);
      mprintf(0, "Alien got to destination object.\n");
      break;
    }
  } else if (data->evt_ai_notify.notify_type == AIN_SCRIPTED_ORIENT) {
    // Handle custom orientation events
    if (memory->mode == ALIEN_LANDING_AT_HOME) {
      memory->done_turning = (AI_TurnTowardsVectors(me, &memory->home_fvec, &memory->home_uvec) != 0);
    }
  } else if (data->evt_ai_notify.notify_type == AIN_FIRED_WEAPON) {
    mprintf(0, "Energy weapon fired...\n");

    // Subtract energy it took to fire energy bolt
    memory->energy_charges -= ALIEN_ENERGY_BOLT_COST;
    if (memory->energy_charges < 0.0f)
      memory->energy_charges = 0.0f;
  } else if (data->evt_ai_notify.notify_type == AIN_SCRIPTED_GOAL) {
    // mprintf(0,"Custom goal called.\n");
    vector my_pos, dir;

    Obj_Value(me, VF_GET, OBJV_V_POS, &my_pos);
    SendCommand(me, memory->leader_handle, ALIEN_COM_GET_GOAL_POS, &memory->squad_goal_pos);
    dir = memory->squad_goal_pos - my_pos;
    vm_VectorNormalize(&dir);

    AI_Value(me, VF_SET, AIV_V_MOVEMENT_DIR, &dir);
  } else if (data->evt_ai_notify.notify_type == AIN_SCRIPTED_ENABLER) {
    // mprintf(0,"Custom enabler called.\n");
    bool is_visible;

    // Ask leader if he can see me
    SendCommand(me, memory->leader_handle, ALIEN_COM_CAN_YOU_SEE_ME, &is_visible);

    // If I'm visible, do straight line to position, otherwise deactivate this goal
    return is_visible;
  } else if (data->evt_ai_notify.notify_type == AIN_MELEE_HIT) {
    // Make sure we are doing the energy suck attack
    if (data->evt_ai_notify.attack_num == 0) {
      int target = data->evt_ai_notify.it_handle;

      // Check if target has energy (if not a player, assume it does have energy)
      bool target_has_energy = true;
      int type;
      Obj_Value(target, VF_GET, OBJV_I_TYPE, &type);
      if (type == OBJ_PLAYER) {
        if (qObjEnergy(target) <= 0.0f) {
          target_has_energy = false;
        }
      }

      // If target still has energy, give alien an energy charge
      if (target_has_energy && memory->energy_charges < ALIEN_MAX_ENERGY_CHARGES) {
        memory->energy_charges += ALIEN_ENERGY_SUCK_CHARGE;
        if (memory->energy_charges > ALIEN_MAX_ENERGY_CHARGES) {
          memory->energy_charges = ALIEN_MAX_ENERGY_CHARGES;
        }
        memory->ok_to_deposit = true;
      }
    }
  } else if (data->evt_ai_notify.notify_type == AIN_USER_DEFINED) {
    bot_com *com = (bot_com *)data->extra_info;

    return ReceiveCommand(me, data->evt_ai_notify.it_handle, com->action, com->ptr);
  }

  return true;
}

// Handles application of damage to an alien
void AlienOrganism::DoDamage(int me, tOSIRISEVTDAMAGED *damage_data) {
  /*
  int wpn_handle=damage_data->weapon_handle;
  int type;
  uint16_t id;

  wpn_handle=damage_data->weapon_handle;
  Obj_Value(wpn_handle, VF_GET, OBJV_US_ID, &id);
  Obj_Value(wpn_handle, VF_GET, OBJV_I_TYPE, &type);

  if(type == OBJ_WEAPON && (id == Wpn_FindID("Napalm") || id == Wpn_FindID("NapalmBlob")))
  */

  // Handle fire damage
  if (damage_data->damage_type == GD_FIRE) {
    damage_data->damage *= 0.7f;
    // mprintf(0, "Damaged by fire!\n");

    return;
  }

  // Handle damage by matter weapons
  if (damage_data->damage_type == GD_MATTER) {
    damage_data->damage *= 0.9f;

    if (Game_GetTime() >= memory->next_special_damage_time) {
      memory->next_special_damage_time = Game_GetTime() + 0.8f + ((float)rand() / (float)D3_RAND_MAX) * 0.5f;

      // Create a frag burst effect
      vector pos;
      int room;
      Obj_Value(me, VF_GET, OBJV_V_POS, &pos);
      Obj_Value(me, VF_GET, OBJV_I_ROOMNUM, &room);
      Obj_Create(OBJ_WEAPON, frag_burst_effect_id, room, &pos, NULL, me);
    }

    return;
  }

  // Handle damage by energy weapons
  if ((damage_data->damage_type == GD_ENERGY || damage_data->damage_type == GD_ELECTRIC) &&
      Game_GetTime() >= memory->next_special_damage_time) {
    // Check if we're currently doing energy transfer or healing
    if (IsEnergyRelatedLandedMode(memory->mode)) {
      memory->next_special_damage_time = Game_GetTime() + 0.1f + ((float)rand() / (float)D3_RAND_MAX) * 0.2f;

      // Do double damage
      damage_data->damage *= 3.0f;

      // Create sparks
      int room;
      vector pos;
      Obj_Value(me, VF_GET, OBJV_V_POS, &pos);
      Obj_Value(me, VF_GET, OBJV_I_ROOMNUM, &room);
      Game_CreateRandomSparks(30 + int(((float)rand() / (float)D3_RAND_MAX) * 10.0f), &pos, room);

      return;
    }
  }

  // Just make it do little damage
  damage_data->damage *= 0.15f;
  // mprintf(0, "Not damaged!\n");
}

// Handles attacks on aliens
void AlienOrganism::DoCollide(int me, tOSIRISEVTCOLLIDE *collide_data) {
  // See if it's a player or player weapon that hit us
  if (qObjIsPlayerOrPlayerWeapon(collide_data->it_handle)) {
    // mprintf(0,"Hit by player!\n");
    memory->hit_by_player = true;
  }
}

// Receives all basic events and calls processesing functions
int16_t AlienOrganism::CallEvent(int event, tOSIRISEventInfo *data) {
  switch (event) {
  case EVT_AI_INIT:
    DoInit(data->me_handle);
    break;
  case EVT_AI_FRAME:
    DoFrame(data->me_handle);
    break;
  case EVT_AI_NOTIFY:
    return (DoNotify(data->me_handle, data) != false) ? CONTINUE_CHAIN | CONTINUE_DEFAULT : 0;
    break;
  case EVT_DAMAGED:
    DoDamage(data->me_handle, &data->evt_damaged);
    break;
  case EVT_DESTROY:
    msafe_struct mo;
    mo.objhandle = memory->pos1_handle;
    MSafe_CallFunction(MSAFE_OBJECT_REMOVE, &mo);
    mo.objhandle = memory->pos2_handle;
    MSafe_CallFunction(MSAFE_OBJECT_REMOVE, &mo);
    mo.objhandle = memory->pos3_handle;
    MSafe_CallFunction(MSAFE_OBJECT_REMOVE, &mo);
    break;
  case EVT_COLLIDE:
    DoCollide(data->me_handle, &data->evt_collide);
    break;
  case EVT_MEMRESTORE:
    memory = (alienorganism_data *)data->evt_memrestore.memory_ptr;
    break;
  }

  return CONTINUE_CHAIN | CONTINUE_DEFAULT;
}

//---------------------
// Heavy Trooper class
//---------------------

// Sets the trooper's max speed
void HeavyTrooper::SetMaxSpeed(int me, float speed, bool accel_faster /*=false*/) {
  AI_Value(me, VF_SET, AIV_F_MAX_SPEED, &speed);

  float delta_speed = memory->base_delta_speed;
  float turn_rate = memory->base_turn_rate;
  float delta_turn_rate = memory->base_delta_turn_rate;
  if (accel_faster) {
    delta_speed *= HT_RAMMING_DELTA_SPEED_MOD;
    turn_rate *= HT_RAMMING_TURN_RATE_MOD;
    delta_turn_rate *= HT_RAMMING_DELTA_TURN_RATE_MOD;
  }
  AI_Value(me, VF_SET, AIV_F_MAX_DELTA_SPEED, &delta_speed);
  AI_Value(me, VF_SET, AIV_F_MAX_TURN_RATE, &turn_rate);
  AI_Value(me, VF_SET, AIV_F_MAX_DELTA_TURN_RATE, &delta_turn_rate);
}

// Enable the trooper's gun attack
void HeavyTrooper::EnableGunAttack(int me, bool enable /*=true*/) {
  int flags = AIF_DISABLE_FIRING;
  if (enable) {
    AI_Value(me, VF_CLEAR_FLAGS, AIV_I_FLAGS, &flags);
  } else {
    AI_Value(me, VF_SET_FLAGS, AIV_I_FLAGS, &flags);
  }
}

// Launches a grenade
void HeavyTrooper::LaunchGrenade(int me) {
  msafe_struct mstruct;

  mstruct.objhandle = me;
  mstruct.index = ht_grenade_id;
  if (mstruct.index == -1)
    return;
  mstruct.gunpoint = HT_GRENADE_GUNPOINT;
  MSafe_CallFunction(MSAFE_OBJECT_FIRE_WEAPON, &mstruct);

  // Play the impact mortar launch sound
  aSoundPlayObject(ht_grenade_sound_id, me, 1.0f);

  /*
  // Create blast effect
  vector pos;
  int room;
  Obj_Value(me, VF_GET, OBJV_I_ROOMNUM, &room);
  Obj_GetGunPos(me, HT_GRENADE_GUNPOINT, &pos);
  Obj_Create(OBJ_WEAPON, ht_grenade_effect_id, room, &pos, NULL, me);
  */

  // Create smoke spew
  TurnOnSpew(me, 2, 7, 0.0f, 0.0f, 65536, false, 0.5f, 0.1f, 0.7f, 1.0f, 15.0f, true);
  TurnOnSpew(me, 2, 3, 0.0f, 0.0f, 65536, false, 0.2f, 0.05f, 0.3f, 2.0f, 40.0f, true);
}

// Checks whether the trooper has a clear shot
bool HeavyTrooper::HasAClearGrenadeShot(int me, bool use_grenade_gunpoint /*=false*/) {
  vector pos, dir;
  matrix orient;
  int room;

  // Get the current object's room number
  Obj_Value(me, VF_GET, OBJV_I_ROOMNUM, &room);

  // See where we should aim from
  if (use_grenade_gunpoint) {
    // aim directly from the grenade launcher's gunpoint
    Obj_GetGunPos(me, HT_GRENADE_GUNPOINT, &pos, &dir);
  } else {
    // since we don't want to aim with the gunpoint, just use the forward vector
    Obj_Value(me, VF_GET, OBJV_V_POS, &pos);
    Obj_Value(me, VF_GET, OBJV_M_ORIENT, &orient);
    dir = orient.fvec;
  }

  // See if should check for a clear shot
  tShotData shot_data;

  shot_data.object_handle = me;
  shot_data.danger_radius = 20.0f;
  shot_data.radius_inc = 1.2f;
  shot_data.max_dist = 120.0f;
  shot_data.risk_factor = 0.0f;
  shot_data.start_point = pos;
  shot_data.start_room = room;
  shot_data.dir = dir;
  shot_data.shot_radius = 5.0f;

  /*
  // If we're not checking during actual launch, make check a little bigger
  if(!use_grenade_gunpoint)
  {
          shot_data.danger_radius *= 1.3f;
  }
  */

  int result = HasClearShot(&shot_data);

  if (result == CS_ALL_CLEAR) {
    return true;
  } else {
    mprintf(0, "Didn't shoot because: %d\n", result);
    return false;
  }
}

// Sets the Current Mode
void HeavyTrooper::SetMode(int me, char mode) {
  int flags;

  // mprintf(0, "From mode %d, ",memory->mode);

  // Clear out any non-Dallas assigned goals
  AI_SafeSetType(me, AIT_AIS);

  // Turn on dodging (only off when curled up)
  flags = AIF_DODGE;
  AI_Value(me, VF_SET_FLAGS, AIV_I_FLAGS, &flags);

  // Do mode specific setups
  switch (mode) {
  case HT_ATTACK_NORMAL: {
    mprintf(0, "Normal attack mode set.\n");

    // Set firing capabilities
    EnableGunAttack(me);

    // Set speed
    SetMaxSpeed(me, memory->base_speed);

    // Set to default ranged attack goals
    AI_SafeSetType(me, AIT_EVADER1);
  } break;

  case HT_PREPARING_GRENADE: {
    mprintf(0, "Preparing grenade mode set.\n");

    // Disable firing capabilities
    EnableGunAttack(me, false);

    // Set speed
    SetMaxSpeed(me, memory->base_speed * HT_GRENADE_AIMING_SPEED_MOD);

    // Set to default ranged attack goals
    AI_SafeSetType(me, AIT_EVADER1);
  } break;

  case HT_FIRING_GRENADE: {
    mprintf(0, "Firing grenade mode set.\n");

    // Disable firing capabilities
    EnableGunAttack(me, false);

    // Set speed
    SetMaxSpeed(me, memory->base_speed * HT_GRENADE_FIRING_SPEED_MOD);

    // Set to default ranged attack goals
    AI_SafeSetType(me, AIT_EVADER1);

    // Play the fire grenade anim
    Obj_SetCustomAnim(me, HT_GRENADE_START_FRAME, HT_GRENADE_FIRE_FRAME, HT_GRENADE_FIRE_ANIM_TIME, 0, -1, -1);
  } break;

  case HT_RECOVERING: {
    mprintf(0, "Recovering mode set.\n");

    // Disable firing capabilities
    EnableGunAttack(me, false);

    // Set speed
    SetMaxSpeed(me, memory->base_speed * HT_GRENADE_AIMING_SPEED_MOD);

    // Set to default ranged attack goals
    AI_SafeSetType(me, AIT_EVADER1);
  } break;

  case HT_PREPARING_FOR_CURL: {
    mprintf(0, "Preparing for curl mode set.\n");

    // Disable firing capabilities
    EnableGunAttack(me, false);

    // Set speed
    SetMaxSpeed(me, memory->base_speed * HT_CURLING_SPEED_MOD);

    // Set to default ranged attack goals
    AI_SafeSetType(me, AIT_EVADER1);
  } break;

  case HT_CURLING_UP: {
    mprintf(0, "Curling up mode set.\n");

    // Disable firing capabilities
    EnableGunAttack(me, false);

    // Set speed
    SetMaxSpeed(me, memory->base_speed * HT_CURLING_SPEED_MOD);

    // Play the curling up animation
    Obj_SetCustomAnim(me, HT_CURLUP_START_FRAME, HT_CURLUP_END_FRAME, HT_CURLUP_ANIM_TIME, 0, -1, -1);

    // Set to default ranged attack goals
    AI_SafeSetType(me, AIT_EVADER1);
  } break;

  case HT_UNCURLING: {
    mprintf(0, "Uncurling mode set.\n");

    // Disable firing capabilities
    EnableGunAttack(me, false);

    // Set speed
    SetMaxSpeed(me, memory->base_speed * HT_CURLING_SPEED_MOD);

    // Play the uncurling animation and then switch back to alert
    Obj_SetCustomAnim(me, HT_UNCURL_START_FRAME, HT_UNCURL_END_FRAME, HT_UNCURL_ANIM_TIME, AIAF_NOTIFY, -1, AS_ALERT);

    // Set to default ranged attack goals
    AI_SafeSetType(me, AIT_EVADER1);
  } break;

  case HT_ARMORED_ATTACK: {
    mprintf(0, "Armored mode set.\n");

    // Disable firing capabilities
    EnableGunAttack(me, false);

    // Turn off dodging
    flags = AIF_DODGE;
    AI_Value(me, VF_CLEAR_FLAGS, AIV_I_FLAGS, &flags);

    // Set speed
    SetMaxSpeed(me, memory->base_speed * HT_RAMMING_SPEED_MOD, true);

    // Set goal to ram into player
    AI_AddGoal(me, AIG_GET_TO_OBJ, 1, 1.0f, -1,
               GF_ORIENT_VELOCITY | GF_OBJ_IS_TARGET | GF_USE_BLINE_IF_SEES_GOAL | GF_KEEP_AT_COMPLETION |
                   GF_NONFLUSHABLE,
               OBJECT_HANDLE_NONE);
    AI_SetGoalCircleDist(me, 1, 0.1f);
  } break;

  case HT_ARMORED_CIRCLE_BACK: {
    mprintf(0, "Armored circle-back mode set.\n");

    vector pos;
    int room;

    // Disable firing capabilities
    EnableGunAttack(me, false);

    // Turn off dodging
    flags = AIF_DODGE;
    AI_Value(me, VF_CLEAR_FLAGS, AIV_I_FLAGS, &flags);

    // Set speed
    SetMaxSpeed(me, memory->base_speed * HT_CIRCLE_BACK_SPEED_MOD, true);

    // Set the goal to wander away for a few seconds
    Obj_Value(me, VF_GET, OBJV_V_POS, &pos);
    Obj_Value(me, VF_GET, OBJV_I_ROOMNUM, &room);

    float dist = 10.0f;

    AI_AddGoal(me, AIG_WANDER_AROUND, 1, 1.0f, -1, GF_KEEP_AT_COMPLETION | GF_NONFLUSHABLE, &pos, room);
    AI_GoalValue(me, 1, VF_SET, AIGV_F_CIRCLE_DIST, &dist);

    int g_index = AI_AddGoal(me, AIG_GET_AROUND_OBJ, ACTIVATION_BLEND_LEVEL, 1.0f, -1,
                             GF_OBJ_IS_TARGET | GF_KEEP_AT_COMPLETION | GF_NONFLUSHABLE, OBJECT_HANDLE_NONE);
    AI_GoalValue(me, g_index, VF_SET, AIGV_F_CIRCLE_DIST, &dist);
  } break;

  default:
    mprintf(0, "Heavy Trooper is all messed up!\n");
    break;
  }

  // Set the current mode and reset the mode time
  memory->mode = mode;
  memory->mode_time = 0.0f;
}

// Processes the AI Initialize event
void HeavyTrooper::DoInit(int me) {
  // Setup memory data
  tOSIRISMEMCHUNK ch;
  ch.id = HT_MEMORY_ID;
  ch.size = sizeof(heavytrooper_data);
  ch.my_id.type = OBJECT_SCRIPT;
  ch.my_id.objhandle = me;

  memory = (heavytrooper_data *)Scrpt_MemAlloc(&ch);

  // Init base values
  AI_Value(me, VF_GET, AIV_F_MAX_SPEED, &memory->base_speed);
  AI_Value(me, VF_GET, AIV_F_MAX_DELTA_SPEED, &memory->base_delta_speed);
  AI_Value(me, VF_GET, AIV_F_MAX_TURN_RATE, &memory->base_turn_rate);
  AI_Value(me, VF_GET, AIV_F_MAX_DELTA_TURN_RATE, &memory->base_delta_turn_rate);
  Obj_Value(me, VF_GET, OBJV_F_SHIELDS, &memory->base_shields);

  // Init to no charge
  memory->charge = 0.0f;

  memory->max_charge = HT_MAX_CHARGE + ((float)rand() / (float)D3_RAND_MAX) * HT_MAX_CHARGE_VARIANCE;
  memory->curlup_dist = HT_CURLUP_DIST + ((float)rand() / (float)D3_RAND_MAX) * HT_CURLUP_DIST_VARIANCE;

  // Check for a grenade shot as soon as possible
  memory->next_grenade_check_time = Game_GetTime();
  memory->last_grenade_time = Game_GetTime();

  memory->next_charge_time = Game_GetTime();
  memory->next_blast_time = Game_GetTime();

  // Do next particle effect as soon as possible
  memory->next_particle_time = Game_GetTime();

  // Start out in normal attack mode
  SetMode(me, HT_ATTACK_NORMAL);
}

// Processes the AI Frame Interval Event
void HeavyTrooper::DoFrame(int me) {
  float last_see_time, last_see_game_time;
  float last_hear_time, last_hear_game_time;
  float curr_anim_frame;

  // Get the last see and hear time
  AI_Value(me, VF_GET, AIV_F_LAST_SEE_TARGET_TIME, &last_see_game_time);
  AI_Value(me, VF_GET, AIV_F_LAST_HEAR_TARGET_TIME, &last_hear_game_time);

  // Calculate the time since target was seen or heard
  last_hear_time = Game_GetTime() - last_hear_game_time;
  last_see_time = Game_GetTime() - last_see_game_time;

  // Get the current animation frame
  Obj_Value(me, VF_GET, OBJV_F_ANIM_FRAME, &curr_anim_frame);

  // Handle charge increase and decrease
  if (memory->mode == HT_ATTACK_NORMAL || memory->mode == HT_PREPARING_GRENADE || memory->mode == HT_FIRING_GRENADE ||
      memory->mode == HT_RECOVERING) {
    if (Game_GetTime() >= memory->next_charge_time) {
      // Set the next recharge time
      memory->next_charge_time = Game_GetTime() + HT_RECHARGE_TIME;

      // Increment the charge
      memory->charge += HT_CHARGE_INC;
      if (memory->charge > memory->max_charge) {
        memory->charge = memory->max_charge;
      }
    }
  } else if (memory->mode == HT_ARMORED_ATTACK || memory->mode == HT_ARMORED_CIRCLE_BACK) {
    if (Game_GetTime() >= memory->next_charge_time) {
      // Set the next decharge time
      memory->next_charge_time = Game_GetTime() + HT_DECHARGE_TIME;

      // Decrement the charge
      memory->charge -= HT_CHARGE_INC;
      if (memory->charge < 0.0f) {
        memory->charge = 0.0f;
      }
    }
  }

  // mprintf(0,"Charge: %.1f\n",memory->charge);

  // Do mode specific checks
  switch (memory->mode) {
  case HT_ATTACK_NORMAL: {
    // Get the current target handle
    int target_handle;
    AI_Value(me, VF_GET, AIV_I_TARGET_HANDLE, &target_handle);

    ///*
    // If we are fully charged, decide if we should curl up
    if (memory->charge >= memory->max_charge) {
      // If low on hitpoints
      float shields;
      Obj_Value(me, VF_GET, OBJV_F_SHIELDS, &shields);
      if (shields < (memory->base_shields * HT_CURLUP_DAMAGE)) {
        SetMode(me, HT_PREPARING_FOR_CURL);
        break;
      }

      // If close to target
      if (target_handle != OBJECT_HANDLE_NONE) {
        float dist;
        AI_Value(me, VF_GET, AIV_F_DIST_TO_TARGET, &dist);
        if (dist < memory->curlup_dist) {
          SetMode(me, HT_PREPARING_FOR_CURL);
          break;
        }
      }
    }
    //*/

    // Decide if we should switch into grenade mode
    if (Game_GetTime() >= memory->next_grenade_check_time) {
      memory->next_grenade_check_time =
          Game_GetTime() + HT_GRENADE_CHECK_INTERVAL + ((float)rand() / (float)D3_RAND_MAX) * HT_GRENADE_CHECK_VARIANCE;

      // Make sure enough time has gone by since we last fired
      if ((Game_GetTime() - memory->last_grenade_time) >= HT_GRENADE_RELOAD_TIME) {
        // Make sure we have a target
        if (target_handle != OBJECT_HANDLE_NONE) {
          float dist;
          AI_Value(me, VF_GET, AIV_F_DIST_TO_TARGET, &dist);

          // Make sure the target is within range
          if (dist > HT_GRENADE_DIST && dist < HT_GRENADE_DIST_MAX) {
            // Make sure we currently have a clear shot
            if (HasAClearGrenadeShot(me, false)) {
              SetMode(me, HT_PREPARING_GRENADE);
              break;
            }
          }
        }
      }
    }

    // See if we're currently in the firing animation
    if (curr_anim_frame > HT_GUNFIRE_START_FRAME && curr_anim_frame < HT_GUNFIRE_END_FRAME) {
      // Check the time to see if we need to generate a new particle spew
      if (Game_GetTime() >= memory->next_particle_time) {
        memory->next_particle_time = Game_GetTime() + HT_MAX_PARTICLE_TIME;

        // Create the casing particle effect
        TurnOnSpew(me, 3, 15, 2.0f, 0.0f, 65536 | 128, false, 0.1f, 0.15f, HT_PARTICLE_SPEW_TIME, 0.2f, 50, true);

        /*
        msafe_struct mstruct;
        mstruct.objhandle = me;
        mstruct.index = Wpn_FindID("ChaffSpark")	;
        if (mstruct.index == -1)
                return;
        mstruct.gunpoint = 3;
        MSafe_CallFunction(MSAFE_OBJECT_FIRE_WEAPON,&mstruct);
        */
      }
    }
  } break;

  case HT_PREPARING_GRENADE: {
    char type, next_type;
    AI_Value(me, VF_GET, AIV_C_ANIMATION_TYPE, &type);
    AI_Value(me, VF_GET, AIV_C_NEXT_ANIMATION_TYPE, &next_type);

    // Wait until the current or next anim type is alert, then try and fire grenade
    if (type == AS_ALERT || next_type == AS_ALERT) {
      SetMode(me, HT_FIRING_GRENADE);
      break;
    }
  } break;

  case HT_FIRING_GRENADE: {
    // Wait until the fire frame is up
    if (curr_anim_frame == HT_GRENADE_FIRE_FRAME) {
      float anim_time;

      // Start the last grenade time count now
      memory->last_grenade_time = Game_GetTime();

      // Do another aim check, but this time from the actual grenade gunpoint
      if (HasAClearGrenadeShot(me, true)) {
        // Fire grenade from gunpoint
        LaunchGrenade(me);

        // Set anim time to recoil
        anim_time = HT_GRENADE_RECOIL_ANIM_TIME;
      } else {
        // Set time to misfire
        anim_time = HT_GRENADE_MISFIRE_ANIM_TIME;
      }

      // Play the recover animation
      Obj_SetCustomAnim(me, HT_GRENADE_FIRE_FRAME, HT_GRENADE_END_FRAME, anim_time, AIAF_NOTIFY, -1, AS_ALERT);

      // Go to recover mode
      SetMode(me, HT_RECOVERING);
      break;
    }
  } break;

  case HT_RECOVERING: {
    char type;
    AI_Value(me, VF_GET, AIV_C_ANIMATION_TYPE, &type);

    // Wait until the current anim type is alert, then return to normal
    if (type == AS_ALERT) {
      SetMode(me, HT_ATTACK_NORMAL);
      break;
    }
  } break;

  case HT_PREPARING_FOR_CURL: {
    char type, next_type;
    AI_Value(me, VF_GET, AIV_C_ANIMATION_TYPE, &type);
    AI_Value(me, VF_GET, AIV_C_NEXT_ANIMATION_TYPE, &next_type);

    // Wait until the current or next anim type is alert, then curl up
    if (type == AS_ALERT || next_type == AS_ALERT) {
      SetMode(me, HT_CURLING_UP);
      break;
    }
  } break;

  case HT_CURLING_UP: {
    // If we're done curling up, switch into armored attack mode
    if (curr_anim_frame == HT_CURLUP_END_FRAME) {
      Obj_SetCustomAnim(me, HT_ROLL_START_FRAME, HT_ROLL_END_FRAME, HT_ROLL_ANIM_TIME, AIAF_LOOPING, -1, -1);
      SetMode(me, HT_ARMORED_ATTACK);
      break;
    }
  } break;

  case HT_UNCURLING: {
    char type;
    AI_Value(me, VF_GET, AIV_C_ANIMATION_TYPE, &type);

    // Wait until the current anim type is alert, then return to normal
    if (type == AS_ALERT) {
      SetMode(me, HT_ATTACK_NORMAL);
      break;
    }
  } break;

  case HT_ARMORED_ATTACK: {
    // If we're out of charge time to uncurl
    if (memory->charge <= 0.0f) {
      SetMode(me, HT_UNCURLING);
      break;
    }
  } break;

  case HT_ARMORED_CIRCLE_BACK: {
    // If we're out of charge or have circled back long enough, get outta here
    if (memory->charge <= 0.0f || memory->mode_time >= HT_CIRCLE_BACK_TIME) {
      SetMode(me, HT_ARMORED_ATTACK);
      break;
    }
  } break;

  default:
    mprintf(0, "Heavy Trooper is all messed up!\n");
    break;
  }

  // Increment the current mode time
  memory->mode_time += Game_GetFrameTime();
}

// Process all AI Notify events
bool HeavyTrooper::DoNotify(int me, tOSIRISEventInfo *data) { return true; }

// Processes incoming damage reports
void HeavyTrooper::DoDamage(int me, tOSIRISEVTDAMAGED *damage_data) {
  // If trooper is currently in armored mode, reduce damage significantly
  if (memory->mode == HT_ARMORED_ATTACK || memory->mode == HT_ARMORED_CIRCLE_BACK) {
    damage_data->damage *= HT_ARMOR_DAMAGED_MOD;
  } else if (memory->mode == HT_CURLING_UP || memory->mode == HT_UNCURLING) {
    damage_data->damage *= HT_CURLING_DAMAGED_MOD;
  }
}

// Handle smashing into target in armored mode
void HeavyTrooper::DoCollide(int me, tOSIRISEVTCOLLIDE *collide_data) {
  vector pos;
  int room;

  // Make sure we're in armored attack mode and still have charge left
  if (memory->mode == HT_ARMORED_ATTACK && memory->charge > 0.0f) {
    // Make sure it's ok to create the attack if necessary
    if (Game_GetTime() >= memory->next_blast_time) {
      // Make sure we have a target
      int target_handle;
      AI_Value(me, VF_GET, AIV_I_TARGET_HANDLE, &target_handle);
      if (target_handle != OBJECT_HANDLE_NONE) {
        // See if the object we collided with is the target
        if (collide_data->it_handle == target_handle) {
          // Create the shield blast
          Obj_Value(me, VF_GET, OBJV_V_POS, &pos);
          Obj_Value(me, VF_GET, OBJV_I_ROOMNUM, &room);
          Obj_Create(OBJ_WEAPON, shield_blast_id, room, &pos, NULL, me);

          // Set the next valid shield blast time
          memory->next_blast_time = Game_GetTime() + HT_SHIELD_BLAST_DELAY;

          // Tell him to run away
          SetMode(me, HT_ARMORED_CIRCLE_BACK);
        }
      }
    }
  }
}

// Receives all basic events and calls processesing functions
int16_t HeavyTrooper::CallEvent(int event, tOSIRISEventInfo *data) {
  switch (event) {
  case EVT_AI_INIT:
    DoInit(data->me_handle);
    break;
  case EVT_AI_FRAME:
    DoFrame(data->me_handle);
    break;
  case EVT_AI_NOTIFY:
    return (DoNotify(data->me_handle, data) != false) ? CONTINUE_CHAIN | CONTINUE_DEFAULT : 0;
    break;
  case EVT_DAMAGED:
    DoDamage(data->me_handle, &data->evt_damaged);
    break;
  case EVT_DESTROY:
    break;
  case EVT_COLLIDE:
    DoCollide(data->me_handle, &data->evt_collide);
    break;
  case EVT_MEMRESTORE:
    memory = (heavytrooper_data *)data->evt_memrestore.memory_ptr;
    break;
  }

  return CONTINUE_CHAIN | CONTINUE_DEFAULT;
}

//--------------
// Lifter class
//--------------

// Sets the trooper's max speed
void Lifter::SetMaxSpeed(int me, float speed) { AI_Value(me, VF_SET, AIV_F_MAX_SPEED, &speed); }

// Checks to see if tractor beam pulling can commence/continue
bool Lifter::OkToPull(int me, bool initial_check /*=true*/) {
  matrix orient;
  vector vec_to_target;
  int target_handle;
  float fov_angle;

  // If it's the initial check, make sure we're in alert mode
  if (initial_check) {
    char type;
    AI_Value(me, VF_GET, AIV_C_ANIMATION_TYPE, &type);

    // Wait until the current anim type is alert, then return to normal
    if (type != AS_ALERT)
      return false;
  }

  // Make sure we still have a target
  AI_Value(me, VF_GET, AIV_I_TARGET_HANDLE, &target_handle);
  if (target_handle == OBJECT_HANDLE_NONE)
    return false;

  // Make sure it's a pullable target
  int type;
  Obj_Value(target_handle, VF_GET, OBJV_I_TYPE, &type);
  if (type != OBJ_PLAYER)
    return false;

  // If it's not the initial pull, make sure the target hasn't changed
  if (!initial_check && target_handle != memory->pull_target)
    return false;

  // Make sure the target isn't cloaked
  if (qObjectCloakTime(target_handle) != 0.0f)
    return false;

  // check distance to target
  float dist;
  AI_Value(me, VF_GET, AIV_F_DIST_TO_TARGET, &dist);
  if (dist > LIFTER_MAX_PULLING_DIST)
    return false;

  // Only check min distance if it's the initial decision check (whether to start pulling)
  if (initial_check && dist < LIFTER_MIN_PULLING_DIST)
    return false;

  // check FOV position of target (greater than 45 degrees is too much)
  Obj_Value(me, VF_GET, OBJV_M_ORIENT, &orient);

  // NOTE: do my own vec to target here! (chris' doesn't update quick enough)
  AI_Value(me, VF_GET, AIV_V_VEC_TO_TARGET, &vec_to_target);
  vm_VectorNormalize(&vec_to_target);
  fov_angle = orient.fvec * vec_to_target;
  if (fov_angle < 0.7f)
    return false;

  // see if anything is in the way
  ray_info ray;
  int flags, fate;
  vector start_pos, end_pos, landing_pos;
  int start_room, landing_room;
  float target_size;

  Obj_Value(me, VF_GET, OBJV_V_POS, &start_pos);
  Obj_Value(me, VF_GET, OBJV_I_ROOMNUM, &start_room);
  Obj_Value(target_handle, VF_GET, OBJV_V_POS, &end_pos);
  Obj_Value(target_handle, VF_GET, OBJV_F_SIZE, &target_size);

  flags = FQ_CHECK_OBJS | FQ_IGNORE_POWERUPS | FQ_IGNORE_WEAPONS | FQ_IGNORE_MOVING_OBJECTS |
          FQ_IGNORE_NON_LIGHTMAP_OBJECTS;
  fate = FVI_RayCast(me, &start_pos, &end_pos, start_room, target_size, flags, &ray);
  if (fate != HIT_NONE)
    return false;

  return true;
}

// Attempts to pull the target towards us
void Lifter::PullTarget(int me) {
  // see if anything is in the way
  ray_info ray;
  int flags, fate;
  vector dir, start_pos, my_pos, end_pos, landing_pos;
  int start_room, landing_room;
  float target_size, dist;
  int target_handle;
  matrix target_orient;
  msafe_struct mstruct;

  // Make sure we still have a target
  AI_Value(me, VF_GET, AIV_I_TARGET_HANDLE, &target_handle);
  if (target_handle == OBJECT_HANDLE_NONE || target_handle != memory->pull_target)
    return;

  // get the target's info
  Obj_Value(target_handle, VF_GET, OBJV_V_POS, &start_pos);
  Obj_Value(target_handle, VF_GET, OBJV_I_ROOMNUM, &start_room);
  Obj_Value(target_handle, VF_GET, OBJV_M_ORIENT, &target_orient);
  Obj_Value(me, VF_GET, OBJV_V_POS, &my_pos);
  Obj_Value(target_handle, VF_GET, OBJV_F_SIZE, &target_size);

  // Calc the direction to pull the target
  dir = my_pos - start_pos;
  vm_VectorNormalize(&dir);

  // Calc the distance to pull the target
  dist = (Game_GetTime() - memory->last_target_pull_time) * LIFTER_TARGET_PULL_SPEED;

  // Calc the point to move to
  end_pos = start_pos + (dir * dist);

  // See where it is safe to move to
  flags = FQ_CHECK_OBJS | FQ_IGNORE_POWERUPS | FQ_IGNORE_WEAPONS;
  fate = FVI_RayCast(target_handle, &start_pos, &end_pos, start_room, target_size, flags, &ray);
  landing_pos = ray.hit_point;
  landing_room = ray.hit_room;

  // Now move the target to this position
  mstruct.objhandle = target_handle;
  mstruct.pos = landing_pos;
  mstruct.orient = target_orient;
  mstruct.roomnum = landing_room;
  MSafe_CallFunction(MSAFE_OBJECT_WORLD_POSITION, &mstruct);

  // Move the invisible target powerup to its appropriate position
  MoveTargetProp(target_handle);

  // Set the last pull time
  memory->last_target_pull_time = Game_GetTime();
}

// Moves the target prop to the appropriate
void Lifter::MoveTargetProp(int target_handle) {
  vector target_pos;
  int target_room;
  matrix target_orient;
  msafe_struct mstruct;

  Obj_Value(target_handle, VF_GET, OBJV_V_POS, &target_pos);
  Obj_Value(target_handle, VF_GET, OBJV_I_ROOMNUM, &target_room);
  Obj_Value(target_handle, VF_GET, OBJV_M_ORIENT, &target_orient);

  mstruct.objhandle = memory->pull_target_prop;
  mstruct.pos = target_pos - (target_orient.uvec * 1.4f);
  mstruct.orient = target_orient;
  mstruct.roomnum = target_room;
  MSafe_CallFunction(MSAFE_OBJECT_WORLD_POSITION, &mstruct);
}

// Creates the beam between me and the target
void Lifter::UpdateTractorBeam(int me) {
  float next_duration = LIFTER_PULL_VALIDATE_INTERVAL;
  aLightningCreate(memory->pull_source_prop, memory->pull_target_prop, next_duration, 1.0f, 4, tractor_beam_effect_id,
                   next_duration, 1, 255, 255, 255, false);
}

// All the controls that will be taken away while being force pulled
#define PLAYER_CONTROL_MASK                                                                                            \
  (PLYV_CMASK_FORWARD | PLYV_CMASK_REVERSE | PLYV_CMASK_LEFT | PLYV_CMASK_RIGHT | PLYV_CMASK_UP | PLYV_CMASK_DOWN |    \
   PLYV_CMASK_AFTERBURNER)

// Takes away the target's movement
void Lifter::ConfineTarget(int me) {
  int target_handle;

  // Make sure we still have a target
  AI_Value(me, VF_GET, AIV_I_TARGET_HANDLE, &target_handle);
  if (target_handle == OBJECT_HANDLE_NONE)
    return;

  // Save the current target handle
  AI_Value(me, VF_GET, AIV_I_TARGET_HANDLE, &memory->pull_target);

  // Make sure it's a pullable target
  int type;
  Obj_Value(target_handle, VF_GET, OBJV_I_TYPE, &type);
  if (type == OBJ_PLAYER) {
    aTogglePlayerObjControl(0, PLAYER_CONTROL_MASK, memory->pull_target);
  }

  // Store our start-of-pull shield level
  Obj_Value(me, VF_GET, OBJV_F_SHIELDS, &memory->shields_at_pull_start);

  // Clear the next pull check time
  memory->next_pull_check_time = Game_GetTime();

  mprintf(0, "Target confined...\n");
}

// Restores the target's movement
void Lifter::ReleaseTarget(int me) {
  // Make sure we still have a target
  if (memory->pull_target != OBJECT_HANDLE_NONE && qObjExists(memory->pull_target)) {
    // Check the type
    int type;
    Obj_Value(memory->pull_target, VF_GET, OBJV_I_TYPE, &type);
    if (type == OBJ_PLAYER) {
      aTogglePlayerObjControl(1, PLAYER_CONTROL_MASK, memory->pull_target);
    }
  }

  // Release the pull target
  memory->pull_target = OBJECT_HANDLE_NONE;

  // Set the next pull check time for a nice delay
  memory->next_pull_check_time =
      Game_GetTime() + LIFTER_NEXT_PULL_DELAY + (((float)rand() / (float)D3_RAND_MAX) * LIFTER_NEXT_PULL_VARIANCE);

  mprintf(0, "Target released.\n");
}

// Stops pulling and returns to hostile mode
void Lifter::StopPulling(int me) {
  ReleaseTarget(me);
  SetMode(me, LIFTER_HOSTILE);
}

// Switches lifter to alert animation from idle
void Lifter::SwitchToAlert(int me) { Obj_SetCustomAnim(me, 10, 12, 0.3f, AIAF_NOTIFY, -1, AS_ALERT); }

// Sets the Current Mode
void Lifter::SetMode(int me, char mode) {
  int flags;

  // mprintf(0, "From mode %d, ",memory->mode);

  // Clear out any non-Dallas assigned goals
  AI_SafeSetType(me, AIT_AIS);

  // Do mode specific setups
  switch (mode) {
  case LIFTER_PASSIVE: {
    mprintf(0, "Passive mode set.\n");

    // Disable auto-targeting
    flags = AIF_DETERMINE_TARGET;
    AI_Value(me, VF_CLEAR_FLAGS, AIV_I_FLAGS, &flags);

    // Set to team neutral
    aAISetTeam(AIF_TEAM_NEUTRAL, me);

    // Set speed for this mode
    SetMaxSpeed(me, memory->base_speed);

    // Play the looping idle animation
    Obj_SetCustomAnim(me, 1, 10, 1.6f, AIAF_LOOPING, lifter_amb_sound_id, -1);
  } break;

  case LIFTER_AGITATED: {
    mprintf(0, "Agitated mode set.\n");

    // Disable auto-targeting
    flags = AIF_DETERMINE_TARGET;
    AI_Value(me, VF_CLEAR_FLAGS, AIV_I_FLAGS, &flags);

    // Set to team neutral
    aAISetTeam(AIF_TEAM_NEUTRAL, me);

    // Set speed for this mode
    SetMaxSpeed(me, memory->base_speed * LIFTER_AGITATED_SPEED_MOD);
  } break;

  case LIFTER_HOSTILE: {
    mprintf(0, "Hostile mode set.\n");

    // Set hostile melee attack
    flags = AIF_MELEE1;
    AI_Value(me, VF_SET_FLAGS, AIV_I_FLAGS, &flags);
    flags = AIF_MELEE2;
    AI_Value(me, VF_CLEAR_FLAGS, AIV_I_FLAGS, &flags);

    // Enable auto-targeting
    flags = AIF_DETERMINE_TARGET;
    AI_Value(me, VF_SET_FLAGS, AIV_I_FLAGS, &flags);

    // Set to team PTMC
    aAISetTeam(AIF_TEAM_PTMC, me);

    // Set melee mode
    AI_SafeSetType(me, AIT_MELEE1);

    // Set speed for this mode
    SetMaxSpeed(me, memory->base_speed * LIFTER_HOSTILE_SPEED_MOD);

    // Set the hostile flag
    memory->not_hostile_yet = false;
  } break;

  case LIFTER_PULLING: {
    mprintf(0, "Pulling mode set.\n");

    // Set pull melee attack
    flags = AIF_MELEE2;
    AI_Value(me, VF_SET_FLAGS, AIV_I_FLAGS, &flags);
    flags = AIF_MELEE1;
    AI_Value(me, VF_CLEAR_FLAGS, AIV_I_FLAGS, &flags);

    // Disable auto-targeting so it can't acquire a new target
    flags = AIF_DETERMINE_TARGET;
    AI_Value(me, VF_CLEAR_FLAGS, AIV_I_FLAGS, &flags);

    // NOTE: don't change teams here (needs to keep current target)!

    // Set melee mode
    AI_SafeSetType(me, AIT_MELEE1);

    // Set speed for this mode
    SetMaxSpeed(me, memory->base_speed * LIFTER_PULLING_SPEED_MOD);

    // Confine the target's movement
    ConfineTarget(me);

    // Move the target prop
    MoveTargetProp(memory->pull_target);

    // Clear the last pull time
    memory->last_target_pull_time = Game_GetTime();
  } break;

  case LIFTER_PASSIFYING: {
    mprintf(0, "Passifying mode set.\n");

    // Disable auto-targeting
    flags = AIF_DETERMINE_TARGET;
    AI_Value(me, VF_CLEAR_FLAGS, AIV_I_FLAGS, &flags);

    // Set to team neutral
    aAISetTeam(AIF_TEAM_NEUTRAL, me);

    // Set speed for this mode
    SetMaxSpeed(me, memory->base_speed);

    // Play the to idle animation
    Obj_SetCustomAnim(me, 50, 52, 1.0f, 0, -1, -1);
  } break;

  default:
    mprintf(0, "Lifter is all messed up!\n");
    break;
  }

  // Set the current mode and reset the mode time
  memory->mode = mode;
  memory->mode_time = 0.0f;
}

// Processes the AI Initialize event
void Lifter::DoInit(int me) {
  // Setup memory data
  tOSIRISMEMCHUNK ch;
  ch.id = HT_MEMORY_ID;
  ch.size = sizeof(lifter_data);
  ch.my_id.type = OBJECT_SCRIPT;
  ch.my_id.objhandle = me;

  memory = (lifter_data *)Scrpt_MemAlloc(&ch);

  // Init base values
  AI_Value(me, VF_GET, AIV_F_MAX_SPEED, &memory->base_speed);
  Obj_Value(me, VF_GET, OBJV_F_SHIELDS, &memory->base_shields);
  memory->shield_loss_margin = memory->base_shields * LIFTER_LOSE_PULL_SHIELD_PERCENT;

  memory->pull_target = OBJECT_HANDLE_NONE;

  // Create and setup the beam pulling target prop object
  msafe_struct mstruct;
  vector pos;
  int room;

  Obj_Value(me, VF_GET, OBJV_V_POS, &pos);
  Obj_Value(me, VF_GET, OBJV_I_ROOMNUM, &room);

  memory->pull_target_prop = Obj_Create(OBJ_POWERUP, powerup_id, room, &pos, NULL, me);
  mstruct.objhandle = memory->pull_target_prop;
  MSafe_CallFunction(MSAFE_OBJECT_NO_RENDER, &mstruct);

  // Create the source tractor beam prop
  memory->pull_source_prop = CreateAndAttach(me, "STEmitter", OBJ_ROBOT, 1, 0, true, true);

  // Init time values
  memory->last_damaged_time = Game_GetTime();
  memory->next_pull_check_time = Game_GetTime();
  memory->last_target_pull_time = Game_GetTime();
  memory->next_lift_beam_time = Game_GetTime();

  // Set the attacked flag
  memory->not_hostile_yet = true;

  // Create the lifter's stick effect
  aLightningCreateGunpoints(0, 1, me, 1000000000.0f, 2.0f, 2, lifter_stick_effect_id, 0.5f, 1, 255, 255, 255, false);

  // Start out in normal attack mode
  SetMode(me, LIFTER_PASSIVE);
}

// Process all AI Notify events
bool Lifter::DoNotify(int me, tOSIRISEventInfo *data) {
  if (data->evt_ai_notify.notify_type == AIN_TARGET_DIED) {
    if (memory->mode == LIFTER_PULLING) {
      StopPulling(me);
    }
  } else if (data->evt_ai_notify.notify_type == AIN_MELEE_HIT) {
    // If we are pulling
    if (memory->mode == LIFTER_PULLING) {
      // Create hit sparks
      int room;
      vector pos;
      Obj_Value(me, VF_GET, OBJV_I_ROOMNUM, &room);
      Obj_GetGunPos(me, 1, &pos);
      Game_CreateRandomSparks(20, &pos, room);

      // Stop pulling
      StopPulling(me);

      // Create the concussive hit blast
      Obj_Value(me, VF_GET, OBJV_I_ROOMNUM, &room);
      Obj_Value(me, VF_GET, OBJV_V_POS, &pos);
      // Obj_GetGunPos(me, 2, &pos);
      Obj_Create(OBJ_WEAPON, lifter_blast_effect_id, room, &pos, NULL, me);
    }
  }

  return true;
}

// Processes the AI Frame Interval Event
void Lifter::DoFrame(int me) {
  float last_see_time, last_see_game_time;
  float last_hear_time, last_hear_game_time;
  float last_perceive_time;
  float curr_anim_frame;

  // Get the last see and hear time
  AI_Value(me, VF_GET, AIV_F_LAST_SEE_TARGET_TIME, &last_see_game_time);
  AI_Value(me, VF_GET, AIV_F_LAST_HEAR_TARGET_TIME, &last_hear_game_time);

  // Calculate the time since target was seen or heard
  last_hear_time = Game_GetTime() - last_hear_game_time;
  last_see_time = Game_GetTime() - last_see_game_time;

  // Perceive time is last time target was seen or heard (whichever is smalled)
  if (last_hear_time <= last_see_time) {
    last_perceive_time = last_hear_time;
  } else {
    last_perceive_time = last_see_time;
  }

  // Get the current animation frame
  Obj_Value(me, VF_GET, OBJV_F_ANIM_FRAME, &curr_anim_frame);

  // Do mode specific checks
  switch (memory->mode) {
  case LIFTER_PASSIVE: {
  } break;

  case LIFTER_AGITATED: {
    // If we've been agitated for a little while, and are damaged enough, switch to hostile
    if (memory->mode_time > LIFTER_MIN_AGITATED_MODE_TIME) {
      float curr_shields;
      Obj_Value(me, VF_GET, OBJV_F_SHIELDS, &curr_shields);
      if (curr_shields < (memory->base_shields * LIFTER_HOSTILE_SHIELD_PERCENT)) {
        SetMode(me, LIFTER_HOSTILE);
        break;
      }

      // If we haven't been damaged in a while, become passive again
      float time_since_damaged;
      time_since_damaged = Game_GetTime() - memory->last_damaged_time;
      if (time_since_damaged >= LIFTER_AGITATED_INTEREST_TIME) {
        SetMode(me, LIFTER_PASSIFYING);
        break;
      }
    }
  } break;

  case LIFTER_HOSTILE: {
    // If hostile for a while, and haven't been damaged in a while, become passive
    if (memory->mode_time > LIFTER_MIN_HOSTILE_MODE_TIME) {
      float time_since_damaged;

      time_since_damaged = Game_GetTime() - memory->last_damaged_time;
      if (time_since_damaged >= LIFTER_HOSTILE_INTEREST_TIME) {
        char type;
        AI_Value(me, VF_GET, AIV_C_ANIMATION_TYPE, &type);

        // Wait until the current anim type is alert, then return to passive
        if (type == AS_ALERT) {
          SetMode(me, LIFTER_PASSIFYING);
          break;
        }
      }
    }

    // See if we should do a tractor beam pull
    if (Game_GetTime() >= memory->next_pull_check_time) {
      memory->next_pull_check_time = Game_GetTime() + LIFTER_PULL_CHECK_INTERVAL;

      // See if it's possible to start pulling target
      if (OkToPull(me)) {
        aSoundPlayObject(lifter_pull_sound_id, me, 1.0f);

        // Switch to pulling mode
        SetMode(me, LIFTER_PULLING);
        break;
      }
    }
  } break;

  case LIFTER_PULLING: {
    // See if we should update the tractor beam and validate the current pull situation
    if (Game_GetTime() >= memory->next_pull_check_time) {
      memory->next_pull_check_time = Game_GetTime() + LIFTER_PULL_VALIDATE_INTERVAL;

      // See if the pulling operation is still valid
      if (OkToPull(me, false)) {
        UpdateTractorBeam(me);
      } else {
        // Otherwise, quit pulling
        StopPulling(me);
        break;
      }
    }

    // Pull the target forward
    bool not_multiplayer = true;
    if (not_multiplayer || (Game_GetTime() - memory->last_target_pull_time) >= LIFTER_PULL_TARGET_INTERVAL) {
      PullTarget(me);
    }

    // Have we reached the maximum pulling time?
    if (memory->mode_time > LIFTER_MAX_PULL_TIME) {
      StopPulling(me);
      break;
    }
  } break;

  case LIFTER_PASSIFYING: {
    float curr_anim_frame;

    // Get the current animation frame
    Obj_Value(me, VF_GET, OBJV_F_ANIM_FRAME, &curr_anim_frame);

    // See if we can return to passive yet
    if (curr_anim_frame == 52) {
      SetMode(me, LIFTER_PASSIVE);
      break;
    }
  } break;

  default:
    mprintf(0, "Lifter is all messed up!\n");
    break;
  }

  // If we're carrying anything, display the carry beam
  UpdateLiftBeam(me);

  // Increment the current mode time
  memory->mode_time += Game_GetFrameTime();
}

// Handles the healing effects
void Lifter::UpdateLiftBeam(int me) {
  // See if it's time to create the next energy effect
  if (memory->next_lift_beam_time <= Game_GetTime()) {
    float next_duration = LIFTER_LIFT_BEAM_UPDATE_TIME;

    memory->next_lift_beam_time = Game_GetTime() + next_duration;

    // See if we're carrying an object
    int attached_handle = Obj_GetAttachChildHandle(me, 0);
    if (attached_handle != OBJECT_HANDLE_NONE) {
      aLightningCreate(me, attached_handle, next_duration, 2.0f, 2, boss_heal_effect_id, next_duration, 1, 255, 255,
                       255, false);
    }
  }
}

// Processes incoming damage reports
void Lifter::DoDamage(int me, tOSIRISEVTDAMAGED *damage_data) {
  memory->last_damaged_time = Game_GetTime();

  if (memory->mode == LIFTER_PASSIVE) {
    if (memory->not_hostile_yet) {
      // If not hostile yet, check to see if we should be agitated
      float curr_shields;
      Obj_Value(me, VF_GET, OBJV_F_SHIELDS, &curr_shields);
      if (curr_shields < (memory->base_shields * LIFTER_AGITATED_SHIELD_PERCENT)) {
        SwitchToAlert(me);
        SetMode(me, LIFTER_AGITATED);
        return;
      }
    } else {
      // If we've been hostile already, just switch right back into hostile mode
      SwitchToAlert(me);
      SetMode(me, LIFTER_HOSTILE);
      return;
    }
  } else if (memory->mode == LIFTER_PULLING) {
    float curr_shields, damage_done_during_pull;

    Obj_Value(me, VF_GET, OBJV_F_SHIELDS, &curr_shields);
    damage_done_during_pull = memory->shields_at_pull_start - curr_shields;
    if (damage_done_during_pull >= memory->shield_loss_margin) {
      // Create tractor beam device sparks
      int room;
      vector pos;
      Obj_Value(memory->pull_source_prop, VF_GET, OBJV_I_ROOMNUM, &room);
      Obj_Value(memory->pull_source_prop, VF_GET, OBJV_V_POS, &pos);
      Game_CreateRandomSparks(15, &pos, room);

      // Stop pulling
      StopPulling(me);
    }
  }
}

// Something collided with lifter
void Lifter::DoCollide(int me, tOSIRISEVTCOLLIDE *collide_data) {}

// Does any final cleanup when object is destroyed
void Lifter::DoCleanUp(int me) {
  // remove the powerup props
  msafe_struct mo;
  mo.objhandle = memory->pull_source_prop;
  MSafe_CallFunction(MSAFE_OBJECT_REMOVE, &mo);
  mo.objhandle = memory->pull_target_prop;
  MSafe_CallFunction(MSAFE_OBJECT_REMOVE, &mo);

  // if we're pulling a target, release it
  if (memory->pull_target != OBJECT_HANDLE_NONE) {
    ReleaseTarget(me);
  }
}

// Receives all basic events and calls processesing functions
int16_t Lifter::CallEvent(int event, tOSIRISEventInfo *data) {
  switch (event) {
  case EVT_AI_INIT:
    DoInit(data->me_handle);
    break;
  case EVT_AI_FRAME:
    DoFrame(data->me_handle);
    break;
  case EVT_AI_NOTIFY:
    return (DoNotify(data->me_handle, data) != false) ? CONTINUE_CHAIN | CONTINUE_DEFAULT : 0;
    break;
  case EVT_DAMAGED:
    DoDamage(data->me_handle, &data->evt_damaged);
    break;
  case EVT_DESTROY:
    DoCleanUp(data->me_handle);
    break;
  case EVT_COLLIDE:
    DoCollide(data->me_handle, &data->evt_collide);
    break;
  case EVT_PLAYER_DIES:
    if (memory->mode == LIFTER_PULLING && data->evt_player_dies.it_handle == memory->pull_target) {
      StopPulling(data->me_handle);
    }
    break;
  case EVT_MEMRESTORE:
    memory = (lifter_data *)data->evt_memrestore.memory_ptr;
    break;
  }

  return CONTINUE_CHAIN | CONTINUE_DEFAULT;
}

//-----------------
// AlienBoss class
//-----------------

// Sends a command out to alien boss
bool AlienBoss::SendCommand(int me, int it, char command, void *ptr) {
  bot_com com;

  com.action = command;
  com.ptr = ptr;

  tOSIRISEventInfo ei;

  ei.me_handle = it;
  ei.extra_info = (void *)&com;
  ei.evt_ai_notify.notify_type = AIN_USER_DEFINED;
  ei.evt_ai_notify.it_handle = me;

  return Obj_CallEvent(it, EVT_AI_NOTIFY, &ei);
}

// Processes a command from another alien
bool AlienBoss::ReceiveCommand(int me, int it, char command, void *ptr) {
  switch (command) {
  case AB_COM_THRUSTER_IS_FIRING: {
    if (memory->mode == AB_HEALING) {
      // Set to the death mode for final dallas-controlled cinematic
      SetMode(me, AB_DYING);

      // Let dallas know that we were healing
      *(bool *)ptr = true;
    } else {
      // Run away from flame if necessary
      SetMode(me, AB_HIDING_FROM_THRUSTER);

      // Let dallas know that we weren't healing
      *(bool *)ptr = false;
    }
  } break;

  case AB_COM_THRUSTER_MISFIRED: {
    // Time to go protect the nest
    SetMode(me, AB_GOING_TO_PROTECT_NEST);
  } break;

  case AB_COM_ITS_SHOWTIME: {
    // Make it start out in reverse
    memory->wander_forward = false;

    // Set the intro end time
    memory->intro_end_time = *(float *)ptr;
    memory->intro_ended = false;

    // Time to go protect the nest
    SetMode(me, AB_WANDERING);
  } break;

  case AB_COM_START_WAITING_EFFECT: {
    // Make it start out in reverse
    memory->waiting_effect_enabled = true;
  } break;

  case AB_COM_THRUSTER_IGNITED: {
    // This makes us angry
    memory->state = AB_ANGRY;

    // Set the new damage level
    memory->damage_threshold = AB_ANGRY_DAMAGE_LEVEL * memory->base_shields;

    // Make it slightly faster
    memory->base_speed *= 1.05f;

    mprintf(0, "Alien Boss is now angry.\n");
  } break;

  case AB_COM_SET_SCENARIO_IDS: {
    ab_scenario_ids *ids = (ab_scenario_ids *)ptr;

    memory->nest_handle = ids->nest_handle;
    memory->left_hide_room = ids->left_hide_room;
    memory->right_hide_room = ids->right_hide_room;

    mprintf(0, "Set scenario ids.\n");
  } break;

  case AB_COM_IS_NESTING: {
    if (memory->mode == AB_HEALING) {
      *(bool *)ptr = true;
    } else {
      *(bool *)ptr = false;
    }
  } break;
  }

  //	mprintf(0, "AlienOrganism action %d failed\n");
  return false;
}

// Sets the alien boss's max speed
void AlienBoss::SetMaxSpeed(int me, float speed) { AI_Value(me, VF_SET, AIV_F_MAX_SPEED, &speed); }

// Checks to see if alien is currently on fire
bool AlienBoss::IsOnFire(int me) { return (Obj_IsEffect(me, EF_NAPALMED)); }

// Sets wandering destination goal for boss
void AlienBoss::SetWanderGoal(int me) {
  vector pos;
  int room = 0;
  float dist = 15.0f;

  // If the destination object is valid, set the goals to go to that object
  if (memory->dest_object_handle != OBJECT_HANDLE_NONE) {
    int type;
    Obj_Value(memory->dest_object_handle, VF_GET, OBJV_I_TYPE, &type);
    if (type != OBJ_NONE) {
      AI_AddGoal(me, AIG_GET_TO_OBJ, 2, 1.0f, ALIEN_GUID_GOT_TO_DEST_OBJ,
                 GF_ORIENT_VELOCITY | GF_USE_BLINE_IF_SEES_GOAL | GF_KEEP_AT_COMPLETION | GF_NONFLUSHABLE,
                 memory->dest_object_handle);
      AI_GoalValue(me, 2, VF_SET, AIGV_F_CIRCLE_DIST, &dist);

      return;
    }
    memory->dest_object_handle = OBJECT_HANDLE_NONE;
  }

  // Set the goal to wander
  AI_AddGoal(me, AIG_WANDER_AROUND, 2, 1.0f, -1, GF_ORIENT_VELOCITY | GF_KEEP_AT_COMPLETION, room, room);
  AI_GoalValue(me, 2, VF_SET, AIGV_F_CIRCLE_DIST, &dist);
}

// Sets goal to go to the current wander room
void AlienBoss::SetWanderRoomGoal(int me) {
  vector pos;
  int roomnum;

  roomnum = memory->wander_rooms[memory->curr_wander_room_index];

  Room_Value(roomnum, VF_GET, RMSV_V_PATH_PNT, &pos, 0);
  AI_AddGoal(me, AIG_GET_TO_POS, 2, DEFAULT_INFLUENCE, AB_GUID_GOT_TO_WANDER_ROOM, GF_ORIENT_VELOCITY, &pos, roomnum);

  // Save values for teleport
  memory->dest_pos = pos;
  memory->dest_room = roomnum;

  float dist = 35.0f;
  AI_GoalValue(me, 2, VF_SET, AIGV_F_CIRCLE_DIST, &dist);
}

// Sets goal to go to the current wander room
void AlienBoss::SetProtectNestGoal(int me) {
  vector pos;
  int roomnum;

  // Go to the nest room (first room is list)
  roomnum = memory->wander_rooms[0];

  Room_Value(roomnum, VF_GET, RMSV_V_PATH_PNT, &pos, 0);
  AI_AddGoal(me, AIG_GET_TO_POS, 2, DEFAULT_INFLUENCE, AB_GUID_GOT_TO_NEST_ROOM, GF_ORIENT_VELOCITY, &pos, roomnum);

  // Save values for teleport
  memory->dest_pos = pos;
  memory->dest_room = roomnum;

  float dist = 35.0f;
  AI_GoalValue(me, 2, VF_SET, AIGV_F_CIRCLE_DIST, &dist);
}

// Sets goal to go to the current wander room
void AlienBoss::SetHideRoomGoal(int me) {
  vector pos, left_room_pos, right_room_pos, my_pos;
  int roomnum;
  float left_room_dist;
  float right_room_dist;

  // Get current position
  Obj_Value(me, VF_GET, OBJV_V_POS, &my_pos);

  // Get positions of hide rooms
  Room_Value(memory->left_hide_room, VF_GET, RMSV_V_PATH_PNT, &left_room_pos, 0);
  Room_Value(memory->right_hide_room, VF_GET, RMSV_V_PATH_PNT, &right_room_pos, 0);

  // Get the distance to each hide room
  left_room_dist = vm_VectorDistance(&my_pos, &left_room_pos);
  right_room_dist = vm_VectorDistance(&my_pos, &right_room_pos);

  // Determine the closest hide room, and go to it
  if (left_room_dist < right_room_dist) {
    roomnum = memory->left_hide_room;
    pos = left_room_pos;
  } else {
    roomnum = memory->right_hide_room;
    pos = right_room_pos;
  }

  Room_Value(roomnum, VF_GET, RMSV_V_PATH_PNT, &pos, 0);
  AI_AddGoal(me, AIG_GET_TO_POS, 2, DEFAULT_INFLUENCE, AB_GUID_GOT_TO_HIDE_ROOM,
             GF_ORIENT_VELOCITY | GF_KEEP_AT_COMPLETION, &pos, roomnum);

  // Save values for teleport
  memory->dest_pos = pos;
  memory->dest_room = roomnum;

  float dist = 30.0f;
  AI_GoalValue(me, 2, VF_SET, AIGV_F_CIRCLE_DIST, &dist);
}

// Returns the index of the wander room closest to current position
int AlienBoss::DetermineClosestRoom(int me) {
  int j, closest_room;
  float closest_dist, dist;
  vector my_pos, room_pos;

  // Get current position
  Obj_Value(me, VF_GET, OBJV_V_POS, &my_pos);

  // Find the room closest to
  closest_room = 0;
  closest_dist = 10000;
  for (j = 0; j < AB_NUM_WANDER_ROOMS; j++) {
    // Get room's position
    Room_Value(memory->wander_rooms[j], VF_GET, RMSV_V_PATH_PNT, &room_pos, 0);

    // Get the distance to this room
    dist = vm_VectorDistanceQuick(&my_pos, &room_pos);

    // See if this room is closer than the currently closest room
    if (dist < closest_dist) {
      closest_room = j;
      closest_dist = dist;
    }
  }

  mprintf(0, "Alien Boss - Determined closest wander room: %d\n", closest_room);

  return closest_room;
}

// Returns any current animations to alert
void AlienBoss::AbortCurrentMode(int me) {
  switch (memory->mode) {
  case AB_HEALING: {
    // Stop the healing fog protection
    StopHealing(me);
  } break;

  case AB_LANDING_AT_NEST: {
    // Play the takeoff anim and tell it to go to alert next
    Obj_SetCustomAnim(me, AB_TAKEOFF_START_FRAME, AB_TAKEOFF_END_FRAME, AB_TAKEOFF_ANIM_TIME, AIAF_NOTIFY, -1,
                      AS_ALERT);
  } break;

  case AB_SPECIAL_ATTACK: {
    // Play rest of attack animation and return to alert
    Obj_SetCustomAnim(me, AB_RECOIL_START_FRAME, AB_RECOIL_END_FRAME, AB_RECOIL_ANIM_TIME, AIAF_NOTIFY, -1, AS_ALERT);
  } break;
  }
}

// Handles the healing effects
void AlienBoss::UpdateHealingEnergyBeams(int me) {
  // See if it's time to create the next energy effect
  if (memory->next_update_beam_time <= Game_GetTime()) {
    float next_duration = AB_BEAM_UPDATE_TIME;

    memory->next_update_beam_time = Game_GetTime() + next_duration;

    aLightningCreate(memory->tail_pos_handle, memory->nest_handle, next_duration, 1.0f, 2, boss_heal_effect_id,
                     next_duration, 1, 255, 255, 255, false);

    // Create healing sparks
    int room;
    vector pos;
    Obj_Value(memory->tail_pos_handle, VF_GET, OBJV_I_ROOMNUM, &room);
    Obj_Value(memory->tail_pos_handle, VF_GET, OBJV_V_POS, &pos);
    Game_CreateRandomSparks(5, &pos, room);
  }
}

// Handles the healing effects
void AlienBoss::UpdateDamageEffect(int me) {
  // See if it's time to create the next energy effect
  if (memory->next_damage_effect_time <= Game_GetTime()) {
    memory->next_damage_effect_time = Game_GetTime() + AB_DAMAGE_EFFECT_INTERVAL;

    float curr_damage;
    curr_damage = memory->damage;
    if (curr_damage > memory->damage_threshold)
      curr_damage = memory->damage_threshold;

    if (curr_damage > 0.0f && memory->damage_threshold != 0.0f) {
      float spark_rate;

      spark_rate = (curr_damage / memory->damage_threshold) * AB_MAX_DAMAGE_SPARK_RATE;
      aObjSpark(me, spark_rate, AB_DAMAGE_EFFECT_INTERVAL);
    }

    // do the following for debugging purposes
    // Obj_Create(OBJ_WEAPON, teleport_effect_id, memory->dest_room, &memory->dest_pos, NULL, me);
  }
}

// Start healing fog protection
void AlienBoss::StartHealing(int me) {
  // Set the green healing fog
  aRoomChangeFog(memory->wander_rooms[0], 0.0f, 1.0f, 0.0f, 800.0f, 2.0f);

  // Set the room damage
  aRoomSetDamage(memory->wander_rooms[0], 5.0f, 1);
}

// End healing fog protection
void AlienBoss::StopHealing(int me) {
  // Set the green healing fog
  aRoomChangeFog(memory->wander_rooms[0], 0.0f, 1.0f, 0.0f, 9000.0f, 4.0f);

  // Set the room damage
  aRoomSetDamage(memory->wander_rooms[0], 0.0f, 1);
}

// Checks if the given mode should do stuck prevention
bool AlienBoss::ModeIsStuckSensitive(int mode) {
  if (mode == AB_WANDERING || mode == AB_GOING_TO_NEST || mode == AB_LANDING_AT_NEST ||
      mode == AB_GOING_TO_PROTECT_NEST) {
    return true;
  }

  return false;
}

// Sets the Current Mode
void AlienBoss::SetMode(int me, char mode) {
  mprintf(0, "From mode %d, ", memory->mode);

  int flags;
  char movement_type;
  vector vel;

  // Clear out any goals
  AI_SafeSetType(me, AIT_AIS);

  // Set normal FOV to 360
  float fov = -1.0f;
  AI_Value(me, VF_SET, AIV_F_FOV, &fov);

  // Turn off ranged firing
  flags = AIF_DISABLE_FIRING;
  AI_Value(me, VF_SET_FLAGS, AIV_I_FLAGS, &flags);

  // Turn off melee attacks
  flags = AIF_MELEE1 | AIF_MELEE2;
  AI_Value(me, VF_CLEAR_FLAGS, AIV_I_FLAGS, &flags);

  // Make it always aware
  flags = AIF_FORCE_AWARENESS;
  AI_Value(me, VF_SET_FLAGS, AIV_I_FLAGS, &flags);

  // Do mode specific setups
  switch (mode) {
  case AB_GOING_TO_NEST: {
    mprintf(0, "Going to Nest mode set.\n");

    ray_info ray;
    int fate;
    vector end_pos, landing_pos;
    int end_room, landing_room;

    // If currently landed, set take off velocity
    DoTakeoff(me, 8.0f, 3.0f);

    // Set movement type back to physics for flying around
    movement_type = MT_PHYSICS;
    Obj_Value(me, VF_SET, OBJV_C_MOVEMENT_TYPE, &movement_type);

    // Calculate destination home point a int16_t distance from actual landing point
    end_pos = memory->home_pos + (memory->home_uvec * 20.0f);

    flags = FQ_CHECK_OBJS | FQ_IGNORE_POWERUPS | FQ_IGNORE_WEAPONS | FQ_IGNORE_MOVING_OBJECTS |
            FQ_IGNORE_NON_LIGHTMAP_OBJECTS;
    fate = FVI_RayCast(me, &memory->home_pos, &end_pos, memory->home_room, 0.0f, flags, &ray);

    landing_pos = ray.hit_point;
    landing_room = ray.hit_room;

    // Set goal to head home
    AI_AddGoal(me, AIG_GET_TO_POS, 2, 1.0f, AB_GUID_GOT_TO_NEST, GF_ORIENT_VELOCITY, &landing_pos, landing_room);
    AI_SetGoalCircleDist(me, 2, 2.0f);

    // Save values for teleport
    memory->dest_pos = landing_pos;
    memory->dest_room = landing_room;

    // Set returning home speed
    SetMaxSpeed(me, memory->base_speed * AB_RETURN_SPEED_MOD);

    // Clear the next activity time
    memory->next_activity_time = 0.0f;

    // Clear the next wander time
    memory->max_wander_time = 0.0f;
  } break;

  case AB_LANDING_AT_NEST: {
    mprintf(0, "Landing at Nest mode set.\n");

    // If currently landed, set take off velocity
    DoTakeoff(me, 8.0f, 3.0f);

    // Turn off dodging
    flags = AIF_DODGE;
    AI_Value(me, VF_CLEAR_FLAGS, AIV_I_FLAGS, &flags);

    // Turn on point-collide-walls and lock axes
    flags = PF_POINT_COLLIDE_WALLS | PF_LOCK_MASK;
    Obj_Value(me, VF_SET_FLAGS, OBJV_I_PHYSICS_FLAGS, &flags);

    // Turn off auto-level
    flags = PF_LEVELING;
    Obj_Value(me, VF_CLEAR_FLAGS, OBJV_I_PHYSICS_FLAGS, &flags);

    // Set movement type back to physics for flying around
    movement_type = MT_PHYSICS;
    Obj_Value(me, VF_SET, OBJV_C_MOVEMENT_TYPE, &movement_type);

    // Make him land at his home point
    AI_AddGoal(me, AIG_GET_TO_POS, 2, 100.0f, AB_GUID_LANDED,
               GF_NONFLUSHABLE | GF_KEEP_AT_COMPLETION | GF_ORIENT_SCRIPTED, &memory->home_pos, memory->home_room);
    AI_SetGoalCircleDist(me, 2, 0.0f);

    // Save values for teleport
    memory->dest_pos = memory->home_pos;
    memory->dest_room = memory->home_room;

    // Play his landing animation
    Obj_SetCustomAnim(me, AB_LANDING_START_FRAME, AB_LANDING_END_FRAME, AB_LANDING_ANIM_TIME, 0, -1, -1);

    // Set landing speed
    SetMaxSpeed(me, memory->base_speed * AB_LAND_SPEED_MOD);

    // Clear the next activity time
    memory->next_activity_time = 0.0f;

    // Clear the next wander time
    memory->max_wander_time = 0.0f;
  } break;

  case AB_HEALING: {
    mprintf(0, "Healing mode set.\n");

    // Turn off dodging
    flags = AIF_DODGE;
    AI_Value(me, VF_CLEAR_FLAGS, AIV_I_FLAGS, &flags);

    // Turn off auto-level
    flags = PF_LEVELING;
    Obj_Value(me, VF_CLEAR_FLAGS, OBJV_I_PHYSICS_FLAGS, &flags);

    // Turn on point-collide-walls and lock axes
    flags = PF_POINT_COLLIDE_WALLS | PF_LOCK_MASK;
    Obj_Value(me, VF_SET_FLAGS, OBJV_I_PHYSICS_FLAGS, &flags);

    // No movement allowed
    movement_type = MT_NONE;
    Obj_Value(me, VF_SET, OBJV_C_MOVEMENT_TYPE, &movement_type);

    // Play his landing animation
    Obj_SetCustomAnim(me, AB_HEALING_START_FRAME, AB_HEALING_END_FRAME, AB_HEALING_ANIM_TIME, AIAF_LOOPING, -1, -1);

    // Set landed speed
    SetMaxSpeed(me, 0.0f);

    // Set the next activity time
    memory->next_activity_time = Game_GetTime() + ALIEN_LANDED_ACTION_TIME;

    // Start healing protection
    StartHealing(me);
  } break;

  case AB_WAITING: {
    mprintf(0, "Waiting mode set.\n");

    // Turn off dodging
    flags = AIF_DODGE;
    AI_Value(me, VF_CLEAR_FLAGS, AIV_I_FLAGS, &flags);

    // Turn off auto-level
    flags = PF_LEVELING;
    Obj_Value(me, VF_CLEAR_FLAGS, OBJV_I_PHYSICS_FLAGS, &flags);

    // Turn on point-collide-walls and lock axes
    flags = PF_POINT_COLLIDE_WALLS | PF_LOCK_MASK;
    Obj_Value(me, VF_SET_FLAGS, OBJV_I_PHYSICS_FLAGS, &flags);

    // No movement allowed
    movement_type = MT_NONE;
    Obj_Value(me, VF_SET, OBJV_C_MOVEMENT_TYPE, &movement_type);

    // Play his landing animation
    Obj_SetCustomAnim(me, AB_HEALING_START_FRAME, AB_HEALING_END_FRAME, AB_HEALING_ANIM_TIME, AIAF_LOOPING, -1, -1);

    // Set landed speed
    SetMaxSpeed(me, 0.0f);
  } break;

  case AB_WANDERING: {
    mprintf(0, "Wandering mode set.\n");

    // If currently landed, set take off velocity
    DoTakeoff(me, 8.0f, 3.0f);

    // Set movement type back to physics for flying around
    movement_type = MT_PHYSICS;
    Obj_Value(me, VF_SET, OBJV_C_MOVEMENT_TYPE, &movement_type);

    // Get Closest wander room
    memory->curr_wander_room_index = DetermineClosestRoom(me);
    SetWanderRoomGoal(me);

    // Set wander speed
    if (memory->intro_ended) {
      SetMaxSpeed(me, memory->base_speed * AB_WANDER_SPEED_MOD);
    } else {
      // wander at fixed speed for the intro
      SetMaxSpeed(me, 30.0f);
    }

    // Set the next activity check to happen within .4 to .8 seconds
    memory->next_activity_time = Game_GetTime() + ((float)rand() / (float)D3_RAND_MAX) * 3.0f + 3.0f;
  } break;

  case AB_ATTACKING: {
    mprintf(0, "Attacking mode set.\n");

    // If currently landed, set take off velocity
    DoTakeoff(me, 8.0f, 3.0f);

    // Set attack FOV to 360
    float fov = -1.0f;
    AI_Value(me, VF_SET, AIV_F_FOV, &fov);

    // Set movement type back to physics for flying around
    movement_type = MT_PHYSICS;
    Obj_Value(me, VF_SET, OBJV_C_MOVEMENT_TYPE, &movement_type);

    // Only set goals if not currently in squad formation
    AI_SafeSetType(me, AIT_EVADER1);

    // Turn on ranged firing
    flags = AIF_DISABLE_FIRING;
    AI_Value(me, VF_CLEAR_FLAGS, AIV_I_FLAGS, &flags);

    // Turn on melee attacks
    flags = AIF_MELEE1 | AIF_MELEE2;
    AI_Value(me, VF_SET_FLAGS, AIV_I_FLAGS, &flags);

    // Set Engage and Attack Speeds
    SetMaxSpeed(me, memory->base_speed * AB_ATTACK_SPEED_MOD);

    // Set the next activity time
    memory->next_activity_time = Game_GetTime() + ((float)rand() / (float)D3_RAND_MAX) * 1.0f;

    // Clear the next wander time
    memory->max_wander_time = 0.0f;
  } break;

  case AB_MELEE_ATTACK: {
    mprintf(0, "Melee Attack mode set.\n");

    // Set attack FOV to 360
    float fov = -1.0f;
    AI_Value(me, VF_SET, AIV_F_FOV, &fov);

    // Turn on melee attacks
    flags = AIF_MELEE1 | AIF_MELEE2;
    AI_Value(me, VF_SET_FLAGS, AIV_I_FLAGS, &flags);

    // Only set goals if not currently in squad formation
    AI_SafeSetType(me, AIT_MELEE1);

    // Set Engage and Attack Speeds
    SetMaxSpeed(me, memory->base_speed * AB_MELEE_ATTACK_SPEED_MOD);
  } break;

  case AB_PREPARE_SPECIAL_ATTACK: {
    mprintf(0, "Prepare Special Attack mode set.\n");

    // Set attack FOV to 360
    float fov = -1.0f;
    AI_Value(me, VF_SET, AIV_F_FOV, &fov);

    // Only set goals if not currently in squad formation
    AI_SafeSetType(me, AIT_EVADER1);

    // Set Engage and Attack Speeds
    SetMaxSpeed(me, memory->base_speed * AB_SPEC_ATTACK_SPEED_MOD);
  } break;

  case AB_SPECIAL_ATTACK: {
    mprintf(0, "Special Attack mode set.\n");

    // Set attack FOV to 360
    float fov = -1.0f;
    AI_Value(me, VF_SET, AIV_F_FOV, &fov);

    // Only set goals if not currently in squad formation
    AI_SafeSetType(me, AIT_EVADER1);

    // Play special attack animation
    Obj_SetCustomAnim(me, AB_ATTACK_START_FRAME, AB_ATTACK_END_FRAME, AB_ATTACK_ANIM_TIME, 0, -1, -1);

    // Set Engage and Attack Speeds
    SetMaxSpeed(me, memory->base_speed * AB_SPEC_ATTACK_SPEED_MOD);
  } break;

  case AB_SPECIAL_ATTACK_RECOIL: {
    mprintf(0, "Special Attack Recoil mode set.\n");

    // Set attack FOV to 360
    float fov = -1.0f;
    AI_Value(me, VF_SET, AIV_F_FOV, &fov);

    // Only set goals if not currently in squad formation
    AI_SafeSetType(me, AIT_EVADER1);

    // Play special attack animation
    Obj_SetCustomAnim(me, AB_RECOIL_START_FRAME, AB_RECOIL_END_FRAME, AB_RECOIL_ANIM_TIME, AIAF_NOTIFY, -1, AS_ALERT);

    // Set Engage and Attack Speeds
    SetMaxSpeed(me, memory->base_speed * AB_SPEC_ATTACK_SPEED_MOD);
  } break;

  case AB_FLEEING: {
    mprintf(0, "Fleeing mode set.\n");

    // If currently landed, set take off velocity
    DoTakeoff(me, 8.0f, 3.0f);

    // Set movement type back to physics for flying around
    movement_type = MT_PHYSICS;
    Obj_Value(me, VF_SET, OBJV_C_MOVEMENT_TYPE, &movement_type);

    // Set Goal to go somewhere else
    SetWanderGoal(me);

    // Set Fleeing Speed
    SetMaxSpeed(me, memory->base_speed * AB_FLEE_SPEED_MOD);

    // Set the max time to flee (base time somewhat off of damage)
    float time_percent = (float(memory->fire_flee_chance) / 100.0f);
    memory->max_wander_time = ((float)rand() / (float)D3_RAND_MAX) * 2.0f + AB_MAX_FLEE_TIME * time_percent;

    // Decrease the flee chance for next time
    memory->fire_flee_chance -= AB_FLEE_CHANCE_DECREMENT;
    if (memory->fire_flee_chance < 0.0f)
      memory->fire_flee_chance = 0.0f;
  } break;

  case AB_HIDING_FROM_THRUSTER: {
    mprintf(0, "Hiding from Thruster mode set.\n");

    // Abort whatever mode we are in
    AbortCurrentMode(me);

    // If currently landed, set take off velocity
    DoTakeoff(me, 8.0f, 3.0f);

    // Set movement type back to physics for flying around
    movement_type = MT_PHYSICS;
    Obj_Value(me, VF_SET, OBJV_C_MOVEMENT_TYPE, &movement_type);

    // Determine closest hide room and go to it
    SetHideRoomGoal(me);

    // Set returning home speed
    SetMaxSpeed(me, memory->base_speed * AB_RETURN_SPEED_MOD);

  } break;

  case AB_GOING_TO_PROTECT_NEST: {
    mprintf(0, "Going to Protect Nest mode set.\n");

    // Abort whatever mode we are in
    AbortCurrentMode(me);

    // Set to protective state
    memory->state = AB_PROTECTIVE;

    // Set the new damage level
    memory->damage_threshold = AB_PROTECTIVE_DAMAGE_LEVEL * memory->base_shields;

    // If currently landed, set take off velocity
    DoTakeoff(me, 8.0f, 3.0f);

    // Set movement type back to physics for flying around
    movement_type = MT_PHYSICS;
    Obj_Value(me, VF_SET, OBJV_C_MOVEMENT_TYPE, &movement_type);

    // Return to nest room
    SetProtectNestGoal(me);

    // Set returning home speed
    SetMaxSpeed(me, memory->base_speed * AB_RETURN_SPEED_MOD);
  } break;

  case AB_DYING: {
    mprintf(0, "Dying mode set.\n");

    // Set Engage and Attack Speeds
    SetMaxSpeed(me, memory->base_speed);

    // Stop the healing room effects
    aRoomChangeFog(memory->wander_rooms[0], 0.0f, 1.0f, 0.0f, 9000.0f, 6.0f);

    // Set the room damage
    aRoomSetDamage(memory->wander_rooms[0], 0.0f, 1);
  } break;

  case AB_TELEPORTING: {
    mprintf(0, "Teleporting mode set.\n");

    // Start cloaking
    aCloakObject(me, 0.0f);

    // Save current mode so we can return to it once teleport is done
    memory->mode_prior_teleporting = memory->mode;

    // set transfer flag
    memory->did_transfer = false;
  } break;

  default:
    mprintf(0, "Alien Boss is all messed up!\n");
  }

  // Reset the landing flags
  memory->done_moving = false;
  memory->done_turning = false;

  // Set the current mode and reset the mode time
  memory->mode = mode;
  memory->mode_time = 0.0f;

  // If we just switched to a stuck sensitive mode, clear out the values
  if (ModeIsStuckSensitive(memory->mode)) {
    memory->next_check_if_stuck_time = Game_GetTime() + AB_CHECK_IF_STUCK_INTERVAL;
    memory->squared_dist_moved = 0.0f;
    Obj_Value(me, VF_GET, OBJV_V_POS, &memory->last_pos);
  }
}

// Handles alien takeoffs
void AlienBoss::DoTakeoff(int me, float takeoff_speed, float speed_variance) {
  // If currently landed, set take off velocity
  if (memory->mode == AB_HEALING || memory->mode == AB_WAITING) {
    vector vel;
    int flags;

    Obj_Value(me, VF_GET, OBJV_V_VELOCITY, &vel);
    vel += (memory->home_uvec * (takeoff_speed + ((float)rand() / (float)D3_RAND_MAX) * speed_variance));
    Obj_Value(me, VF_SET, OBJV_V_VELOCITY, &vel);

    // Play the takeoff anim and tell it to go to alert next
    Obj_SetCustomAnim(me, AB_TAKEOFF_START_FRAME, AB_TAKEOFF_END_FRAME, AB_TAKEOFF_ANIM_TIME, AIAF_NOTIFY, -1,
                      AS_ALERT);

    // Clear wall point collision and axes lock
    flags = PF_LOCK_MASK | PF_POINT_COLLIDE_WALLS;
    Obj_Value(me, VF_CLEAR_FLAGS, OBJV_I_PHYSICS_FLAGS, &flags);

    // Turn on dodging
    flags = AIF_DODGE;
    AI_Value(me, VF_SET_FLAGS, AIV_I_FLAGS, &flags);

    // enable auto-leveling
    flags = PF_LEVELING;
    Obj_Value(me, VF_SET_FLAGS, OBJV_I_PHYSICS_FLAGS, &flags);
  }
}

// Processes the AI Initialize event
void AlienBoss::DoInit(int me) {
  int flags;
  msafe_struct m;
  m.objhandle = me;

  // Setup memory data
  tOSIRISMEMCHUNK ch;
  ch.id = ALIEN_MEMORY_ID;
  ch.size = sizeof(alienboss_data);
  ch.my_id.type = OBJECT_SCRIPT;
  ch.my_id.objhandle = me;

  memory = (alienboss_data *)Scrpt_MemAlloc(&ch);

  // Init base values
  AI_Value(me, VF_GET, AIV_F_MAX_SPEED, &memory->base_speed);
  Obj_Value(me, VF_GET, OBJV_F_SHIELDS, &memory->base_shields);

  // Clear the destination object handle
  memory->dest_object_handle = OBJECT_HANDLE_NONE;

  // Alien hasn't been hit by player yet
  memory->hit_by_player = false;
  memory->damage = 0.0f;
  memory->damage_threshold = AB_NORMAL_DAMAGE_LEVEL * memory->base_shields;
  memory->fire_flee_chance = AB_STARTING_FLEE_CHANCE;

  memory->curr_wander_room_index = 0;
  memory->wander_forward = true;
  memory->protected_nest = false;
  memory->waiting_effect_enabled = false;
  memory->intro_ended = false;

  // Update the energy effect as soon as charge exists
  memory->next_update_beam_time = Game_GetTime();

  // Do special damage as soon as relevant
  memory->next_special_damage_time = Game_GetTime();
  memory->next_damage_effect_time = Game_GetTime();

  // Do special attack as soon as relevant
  memory->next_special_attack_time = Game_GetTime();
  memory->next_melee_attack_time = Game_GetTime();

  // Set the next generic check time
  memory->next_generic_check_time = Game_GetTime() + ((float)rand() / (float)D3_RAND_MAX) * 1.0f + 0.5f;

  // Init other times
  memory->next_activity_time = Game_GetTime();
  memory->max_wander_time = 0.0f;

  // Store Home Information
  Obj_Value(me, VF_GET, OBJV_V_POS, &memory->home_pos);
  Obj_Value(me, VF_GET, OBJV_I_ROOMNUM, &memory->home_room);

  matrix orient;
  Obj_Value(me, VF_GET, OBJV_M_ORIENT, &orient);
  memory->home_fvec = orient.fvec;
  vm_VectorNormalize(&memory->home_fvec);
  memory->home_uvec = orient.uvec;
  vm_VectorNormalize(&memory->home_uvec);

  // Save values for teleport
  memory->dest_pos = memory->home_pos;
  memory->dest_room = memory->home_room;

  // stuck check stuff
  memory->squared_dist_moved = 0.0f;
  memory->next_check_if_stuck_time = Game_GetTime();
  memory->last_pos = memory->home_pos;

  // Init the props
  memory->pos1_handle = OBJECT_HANDLE_NONE;
  memory->pos2_handle = OBJECT_HANDLE_NONE;
  memory->pos3_handle = OBJECT_HANDLE_NONE;

  // Create and setup the energy effect props
  msafe_struct mstruct;
  memory->tail_pos_handle = CreateAndAttach(me, "STEmitter", OBJ_ROBOT, 0, 0, true, true);
  mstruct.objhandle = memory->tail_pos_handle;
  MSafe_CallFunction(MSAFE_OBJECT_NO_RENDER, &mstruct);

  // Init the scenario ID's
  memory->nest_handle = OBJECT_HANDLE_NONE;
  memory->left_hide_room = -1;
  memory->right_hide_room = -1;

  // Do Lookups
  DoCustomLookups();

  // Set the healing fog in room
  aRoomSetFog(memory->wander_rooms[0], 0.0f, 1.0f, 0.0f, 9000.0f);

  // Set to normal state
  memory->state = AB_NORMAL;

  // Set starting mode
  // SetMode(me, AB_GOING_TO_NEST);  // for debugging
  SetMode(me, AB_WAITING);

  // Play looping boss sound
  aSoundPlayObject(boss_flapping_id, me, 1.0f);

  mprintf(0, "Alien boss initialized.\n");
}

// Does custom name lookups
void AlienBoss::DoCustomLookups(void) {
  int j;

  // Lookup the wander room names
  for (j = 0; j < AB_NUM_WANDER_ROOMS; j++) {
    memory->wander_rooms[j] = Scrpt_FindRoomName(AB_WanderRoomNames[j]);

    if (memory->wander_rooms[j] < 0)
      mprintf(0, "Alien Boss Error: Room not found - %s", AB_WanderRoomNames[j]);
  }
}

// Checks to see if tractor beam pulling can commence/continue
bool AlienBoss::OkToStartSpecialAttack(int me) {
  matrix orient;
  vector vec_to_target;
  int target_handle;
  float fov_angle;

  // Make sure we still have a target
  AI_Value(me, VF_GET, AIV_I_TARGET_HANDLE, &target_handle);
  if (target_handle == OBJECT_HANDLE_NONE)
    return false;

  // Make sure the target isn't cloaked
  if (qObjectCloakTime(target_handle) != 0.0f)
    return false;

  // check distance to target
  float dist;
  AI_Value(me, VF_GET, AIV_F_DIST_TO_TARGET, &dist);
  if (dist > AB_MAX_SPECIAL_ATTACK_DIST)
    return false;

  // check FOV position of target (greater than 45 degrees is too much)
  Obj_Value(me, VF_GET, OBJV_M_ORIENT, &orient);

  // NOTE: do my own vec to target here! (chris' doesn't update quick enough)
  AI_Value(me, VF_GET, AIV_V_VEC_TO_TARGET, &vec_to_target);
  vm_VectorNormalize(&vec_to_target);
  fov_angle = orient.fvec * vec_to_target;
  if (fov_angle < 0.7f)
    return false;

  return true;
}

// Checks to see if tractor beam pulling can commence/continue
bool AlienBoss::DoStingAttack(int me) {
  matrix orient;
  vector vec_to_target;
  int target_handle;
  float fov_angle;

  // Make sure we still have a target
  AI_Value(me, VF_GET, AIV_I_TARGET_HANDLE, &target_handle);
  if (target_handle == OBJECT_HANDLE_NONE)
    return false;

  // check distance to target
  float dist;
  AI_Value(me, VF_GET, AIV_F_DIST_TO_TARGET, &dist);
  if (dist > AB_MAX_STING_DIST)
    return false;

  // check FOV position of target (greater than 45 degrees is too much)
  Obj_Value(me, VF_GET, OBJV_M_ORIENT, &orient);

  // NOTE: do my own vec to target here! (chris' doesn't update quick enough)
  AI_Value(me, VF_GET, AIV_V_VEC_TO_TARGET, &vec_to_target);
  vm_VectorNormalize(&vec_to_target);
  fov_angle = orient.fvec * vec_to_target;
  if (fov_angle < 0.7f)
    return false;

  // see if anything is in the way
  ray_info ray;
  int flags, fate;
  vector start_pos, end_pos, landing_pos;
  int start_room, landing_room;
  float target_size;

  Obj_Value(me, VF_GET, OBJV_V_POS, &start_pos);
  Obj_Value(me, VF_GET, OBJV_I_ROOMNUM, &start_room);
  Obj_Value(target_handle, VF_GET, OBJV_V_POS, &end_pos);
  Obj_Value(target_handle, VF_GET, OBJV_F_SIZE, &target_size);

  flags = FQ_CHECK_OBJS | FQ_IGNORE_POWERUPS | FQ_IGNORE_WEAPONS | FQ_IGNORE_MOVING_OBJECTS |
          FQ_IGNORE_NON_LIGHTMAP_OBJECTS;
  fate = FVI_RayCast(me, &start_pos, &end_pos, start_room, target_size, flags, &ray);
  if (fate != HIT_NONE)
    return false;

  // Create the lightning effect for the sting attack
  aLightningCreate(target_handle, memory->tail_pos_handle, 0.4f, 1.0f, 3, transfer_effect_id, 0.4f, 1, 255, 255, 255,
                   false);

  // Damage the target
  aObjApplyDamage(target_handle, AB_STING_DAMAGE);

  // Play damaged sound for target

  mprintf(0, "Did sting attack.\n");

  return true;
}

// Processes the AI Frame Interval Event
void AlienBoss::DoFrame(int me) {
  int flags;
  float anim_frame;
  float last_see_time, last_see_game_time;
  float last_hear_time, last_hear_game_time;
  float last_perceive_time;

  // If we're waiting, bail outta here
  if (memory->mode == AB_WAITING) {
    // display healing effects for intro
    if (memory->waiting_effect_enabled) {
      // Handle the healing energy beam effects
      UpdateHealingEnergyBeams(me);
    }
    return;
  }

  // Get the last see and hear time
  AI_Value(me, VF_GET, AIV_F_LAST_SEE_TARGET_TIME, &last_see_game_time);
  AI_Value(me, VF_GET, AIV_F_LAST_HEAR_TARGET_TIME, &last_hear_game_time);

  // Calculate the time since target was seen and heard
  last_hear_time = Game_GetTime() - last_hear_game_time;
  last_see_time = Game_GetTime() - last_see_game_time;

  // Perceive time is last time target was seen or heard (whichever is smalled)
  if (last_hear_time <= last_see_time) {
    last_perceive_time = last_hear_time;
  } else {
    last_perceive_time = last_see_time;
  }

  // If alien boss was hit by player, take action if necessary
  if (memory->hit_by_player) {
    // reset the hit by player flag
    memory->hit_by_player = false;
  }

  // Get the current animation frame
  Obj_Value(me, VF_GET, OBJV_F_ANIM_FRAME, &anim_frame);

  // Handle generic checks (pertains to more than 1 mode)
  if (memory->next_generic_check_time <= Game_GetTime()) {
  }

  // Handle mode specific operations
  switch (memory->mode) {

  case AB_HEALING: {
    // See if we should do a round of healing
    if (Game_GetTime() >= memory->next_activity_time) {
      // Set the next time to do something
      memory->next_activity_time = Game_GetTime() + AB_HEALING_ACTION_TIME;

      // Heal Some Damage
      float heal_amount = memory->base_shields * AB_HEAL_AMOUNT_PERCENT;
      memory->damage -= heal_amount;

      mprintf(0, "Healing %.1f by %.1f...\n", memory->damage, heal_amount);

      bool done_healing = false;
      if (memory->damage <= 0.0f) {
        memory->damage = 0.0f;
        done_healing = true;
      }

      // If we're all healed up, return to attack mode
      if (done_healing) {
        StopHealing(me);

        SetMode(me, AB_ATTACKING);
        break;
      }
    }

    // Handle the healing energy beam effects
    UpdateHealingEnergyBeams(me);
  } break;

  case AB_DYING: {
    // Allow the healing effects to play for a few seconds
    if (memory->mode_time < 4.5f) {
      UpdateHealingEnergyBeams(me);
    }
  } break;

  case AB_HIDING_FROM_THRUSTER: {
    // Don't hide for more than a few seconds at most
    if (memory->mode_time > 6.0f) {
      SetMode(me, AB_ATTACKING);
      break;
    }
  } break;

  case AB_GOING_TO_PROTECT_NEST: {
    // If we've seen a target, switch into attack mode
    if (memory->protected_nest && last_perceive_time <= 2.0f) {
      SetMode(me, AB_ATTACKING);
      break;
    }
  } break;

  case AB_TELEPORTING: {
    if (memory->mode_time > 2.0f) {
      // Set to previous mode
      SetMode(me, memory->mode_prior_teleporting);
      break;
    } else if (!memory->did_transfer && memory->mode_time > 1.0f) {
      vector pos;
      int room;

      // Do effect before
      Obj_Value(me, VF_GET, OBJV_V_POS, &pos);
      Obj_Value(me, VF_GET, OBJV_I_ROOMNUM, &room);
      Obj_Create(OBJ_WEAPON, teleport_effect_id, room, &pos, NULL, me);

      // Move self
      Obj_Value(me, VF_SET, OBJV_V_POS, &memory->dest_pos);
      Obj_Value(me, VF_SET, OBJV_I_ROOMNUM, &memory->dest_room);

      // Do effect after
      Obj_Value(me, VF_GET, OBJV_V_POS, &pos);
      Obj_Value(me, VF_GET, OBJV_I_ROOMNUM, &room);
      Obj_Create(OBJ_WEAPON, teleport_effect_id, room, &pos, NULL, me);

      memory->did_transfer = true;
    }
  } break;

  case AB_LANDING_AT_NEST: {
    // Play his landed animation if done with landing anim
    if (anim_frame == AB_LANDING_END_FRAME) {
      Obj_SetCustomAnim(me, AB_HEALING_START_FRAME, AB_HEALING_END_FRAME, AB_HEALING_ANIM_TIME, AIAF_LOOPING, -1, -1);
    }

    if (memory->done_turning == true && memory->done_moving == true) {
      SetMode(me, AB_HEALING);
      break;
    }
  } break;

  case AB_GOING_TO_NEST: {
    char type, next_type;

    // If we're in alert, or the next anim mode is alert, and we're done moving, land at home
    if (memory->done_moving == true) {
      AI_Value(me, VF_GET, AIV_C_ANIMATION_TYPE, &type);
      AI_Value(me, VF_GET, AIV_C_NEXT_ANIMATION_TYPE, &next_type);

      if (type == AS_ALERT || type == AS_IDLE || next_type == AS_ALERT || next_type == AS_IDLE) {
        SetMode(me, AB_LANDING_AT_NEST);
        break;
      }
    }
  } break;

  case AB_WANDERING: {
    // If we've seen a target, switch into attack mode
    if (memory->intro_ended && last_perceive_time <= 2.0f) {
      // Play the saw player shriek
      aSoundPlayObject(boss_see_id, me, 1.0f);

      SetMode(me, AB_ATTACKING);
      break;
    }

    // See if intro has ended yet
    if (!memory->intro_ended) {
      if (memory->mode_time > memory->intro_end_time) {
        // Set the normal wander speed
        SetMaxSpeed(me, memory->base_speed * AB_WANDER_SPEED_MOD);

        // set the intro ended flag
        memory->intro_ended = true;

        mprintf(0, "Intro ended for alien boss.\n");
      }
    }
  } break;

  case AB_FLEEING: {
    // See if it is time to stop running
    if (memory->mode_time >= memory->max_wander_time) {
      // Go back to attack mode for now
      SetMode(me, AB_ATTACKING);
    }
  } break;

  case AB_ATTACKING: {
    // Do we need to heal?
    if (memory->damage > memory->damage_threshold) {
      aSoundPlayObject(boss_hurt_id, me, 1.0f);

      SetMode(me, AB_GOING_TO_NEST);
      break;
    }

    // See if we should do a round of healing
    if (Game_GetTime() >= memory->next_activity_time) {
      // Set the next time to do something
      memory->next_activity_time = Game_GetTime() + AB_ATTACK_CHECK_INTERVAL;

      // Should we flee?
      if (memory->state == AB_NORMAL && IsOnFire(me) && (rand() % 100) < memory->fire_flee_chance) {
        aSoundPlayObject(boss_hurt_id, me, 1.0f);

        SetMode(me, AB_FLEEING);
        break;
      }

      // Should we do a special attack?
      if (Game_GetTime() >= memory->next_special_attack_time) {
        if (OkToStartSpecialAttack(me)) {
          aSoundPlayObject(boss_see_id, me, 1.0f);

          memory->next_special_attack_time =
              Game_GetTime() + AB_SPECIAL_ATTACK_DELAY + ((float)rand() / (float)D3_RAND_MAX) * AB_SPECIAL_ATTACK_VARIANCE;

          SetMode(me, AB_PREPARE_SPECIAL_ATTACK);
          break;
        }
      }

      // Should we switch to melee mode?
      if (Game_GetTime() >= memory->next_melee_attack_time) {
        int target_handle;

        // Make sure we still have a target
        AI_Value(me, VF_GET, AIV_I_TARGET_HANDLE, &target_handle);
        if (target_handle != OBJECT_HANDLE_NONE) {
          // check distance to target
          float dist;
          AI_Value(me, VF_GET, AIV_F_DIST_TO_TARGET, &dist);
          if (dist < AB_MELEE_DIST) {
            SetMode(me, AB_MELEE_ATTACK);
            break;
          }
        }
      }
    }

    // If haven't perceived target for a while, return to wander mode
    if (last_perceive_time >= 10.0f) {
      // If we're not in a protective state, go ahead and wander
      if (memory->state == AB_PROTECTIVE) {
        // See if we're in the nest room
        int curr_room;
        Obj_Value(me, VF_GET, OBJV_I_ROOMNUM, &curr_room);
        if (curr_room != memory->wander_rooms[0]) {
          // Go protect nest
          SetMode(me, AB_GOING_TO_PROTECT_NEST);
          break;
        }
      } else {
        SetMode(me, AB_WANDERING);
        break;
      }
    }
  } break;

  case AB_MELEE_ATTACK: {
    // Wait until the melee attack time has expired
    if (memory->mode_time > AB_MAX_MELEE_TIME) {
      memory->next_melee_attack_time =
          Game_GetTime() + AB_MELEE_ATTACK_DELAY + ((float)rand() / (float)D3_RAND_MAX) * AB_MELEE_ATTACK_VARIANCE;

      SetMode(me, AB_ATTACKING);
      break;
    }
  } break;

  case AB_PREPARE_SPECIAL_ATTACK: {
    char type, next_type;
    AI_Value(me, VF_GET, AIV_C_ANIMATION_TYPE, &type);
    AI_Value(me, VF_GET, AIV_C_NEXT_ANIMATION_TYPE, &next_type);

    // Wait until the current or next anim type is alert, then try special attack
    if (type == AS_ALERT || next_type == AS_ALERT) {
      SetMode(me, AB_SPECIAL_ATTACK);
      break;
    }
  } break;

  case AB_SPECIAL_ATTACK: {
    // Wait until the fire frame is up
    if (anim_frame == AB_ATTACK_END_FRAME) {
      // Create a frag burst effect
      vector pos;
      int room;
      matrix orient;

      Obj_Value(me, VF_GET, OBJV_V_POS, &pos);
      Obj_Value(me, VF_GET, OBJV_I_ROOMNUM, &room);
      Obj_Value(me, VF_GET, OBJV_M_ORIENT, &orient);
      Obj_Create(OBJ_WEAPON, boss_frag_burst_id, room, &pos, &orient, me);

      // Go to recover mode
      SetMode(me, AB_SPECIAL_ATTACK_RECOIL);
      break;
    }
  } break;

  case AB_SPECIAL_ATTACK_RECOIL: {
    char type;
    AI_Value(me, VF_GET, AIV_C_ANIMATION_TYPE, &type);

    // Wait until the current anim type is alert, then return to normal
    if (type == AS_ALERT) {
      SetMode(me, AB_ATTACKING);
      break;
    }
  } break;
  }

  // Do the damage effect if necessary
  UpdateDamageEffect(me);

  // If necessary, check if we're stuck
  if (ModeIsStuckSensitive(memory->mode)) {
    vector curr_pos, dist_vec;

    // Update the values
    Obj_Value(me, VF_GET, OBJV_V_POS, &curr_pos);
    dist_vec = curr_pos - memory->last_pos;
    memory->squared_dist_moved += (dist_vec.x * dist_vec.x + dist_vec.y * dist_vec.y + dist_vec.z * dist_vec.z);
    memory->last_pos = curr_pos;

    // See if we need to do a stuck check
    if (Game_GetTime() >= memory->next_check_if_stuck_time) {
      // Calc the next time to check for stuckitude
      memory->next_check_if_stuck_time = Game_GetTime() + AB_CHECK_IF_STUCK_INTERVAL;

      mprintf(0, "Checking to see if Alien Boss is stuck (dist*dist=%f)...\n", memory->squared_dist_moved);

      // Check if the distance moved indicates being stuck
      if (memory->squared_dist_moved < (AB_CHECK_IF_STUCK_DIST * AB_CHECK_IF_STUCK_DIST)) {
        mprintf(0, "Alien Boss is stuck... now teleporting to destination.\n");

        // Teleport boss to new location
        SetMode(me, AB_TELEPORTING);
      } else {
        // Clear the distance moved
        memory->squared_dist_moved = 0.0f;
      }
    }
  }

  // Increment the current mode time
  memory->mode_time += Game_GetFrameTime();
}

// Process all AI Notify events
bool AlienBoss::DoNotify(int me, tOSIRISEventInfo *data) {
  if (IsGoalFinishedNotify(data->evt_ai_notify.notify_type)) {
    // Handle goal complete events
    switch (data->evt_ai_notify.goal_uid) {
    case AB_GUID_GOT_TO_NEST:
      memory->done_moving = true;
      break;

    case AB_GUID_LANDED:
      memory->done_moving = true;
      break;

    case AB_GUID_GOT_TO_NEST_ROOM: {
      // if we're going to protect nest, switch into attack mode
      if (memory->mode == AB_GOING_TO_PROTECT_NEST) {
        mprintf(0, "Alien Boss got to nest room.\n");

        // If we haven't protected nest yet, do special stuff
        if (!memory->protected_nest) {
          int minion1_handle, minion2_handle, player_handle;

          minion1_handle = Scrpt_FindObjectName("BossMinion1");
          minion2_handle = Scrpt_FindObjectName("BossMinion2");

          // Unghost the minions
          aObjGhostSet(0, minion1_handle);
          aObjGhostSet(0, minion2_handle);

          // Turn on their AI
          aAISetState(1, minion1_handle);
          aAISetState(1, minion2_handle);

          // Send them to get the closest player
          player_handle = FindClosestPlayer(minion1_handle);
          SendCommand(minion1_handle, minion1_handle, ALIEN_COM_HUNT_TO_OBJ, &player_handle);
          SendCommand(minion2_handle, minion2_handle, ALIEN_COM_HUNT_TO_OBJ, &player_handle);

          // Play Shriek
          aSoundPlayObject(boss_see_id, me, 1.0f);

          // Set the protected nest flag
          memory->protected_nest = true;
        }

        SetMode(me, AB_ATTACKING);
        break;
      }
    } break;

    case AB_GUID_GOT_TO_HIDE_ROOM: {
      // if we're going to protect nest, switch into attack mode
      if (memory->mode == AB_HIDING_FROM_THRUSTER) {
        mprintf(0, "Alien boss got to hide room.\n");
        // SetMode(me,AB_ATTACKING);
        // break;
      }
    } break;

    case AB_GUID_GOT_TO_WANDER_ROOM: {
      // Make sure we're in wander mode
      if (memory->mode == AB_WANDERING) {
        // Decide whether or not to reverse directions
        if ((rand() % 100) < 4) {
          memory->wander_forward = (memory->wander_forward) ? false : true;

          mprintf(0, "Alien Boss - Reversing wander direction\n");
        }

        // Check whether to follow list in reverse or not
        if (memory->wander_forward) {
          memory->curr_wander_room_index++;
          if (memory->curr_wander_room_index >= AB_NUM_WANDER_ROOMS) {
            memory->curr_wander_room_index = 0;
          }
        } else {
          memory->curr_wander_room_index--;
          if (memory->curr_wander_room_index < 0) {
            memory->curr_wander_room_index = AB_NUM_WANDER_ROOMS - 1;
          }
        }

        // Set the goal to go to the next room
        SetWanderRoomGoal(me);

        // Play the wandering sound
        aSoundPlayObject(boss_turf_id, me, 1.0f);

        mprintf(0, "Alien Boss - Wandering to room: %d\n", memory->curr_wander_room_index);
      }
    } break;

    case AB_GUID_GOT_TO_DEST_OBJ:
      memory->dest_object_handle = OBJECT_HANDLE_NONE;
      SetMode(me, memory->mode);
      mprintf(0, "Alien Boss got to destination object.\n");
      break;
    }
  } else if (data->evt_ai_notify.notify_type == AIN_SCRIPTED_ORIENT) {
    // Handle custom orientation events
    if (memory->mode == AB_LANDING_AT_NEST) {
      memory->done_turning = (AI_TurnTowardsVectors(me, &memory->home_fvec, &memory->home_uvec) != 0);
    }
  } else if (data->evt_ai_notify.notify_type == AIN_MELEE_HIT) {
    // Create hit sparks
    int room;
    vector pos;
    Obj_Value(me, VF_GET, OBJV_I_ROOMNUM, &room);
    Obj_GetGunPos(me, 0, &pos);
    Game_CreateRandomSparks(30, &pos, room);

    // Create the concussive hit blast
    Obj_Value(me, VF_GET, OBJV_I_ROOMNUM, &room);
    Obj_Value(me, VF_GET, OBJV_V_POS, &pos);
    Obj_Create(OBJ_WEAPON, lifter_blast_effect_id, room, &pos, NULL, me);
  } else if (data->evt_ai_notify.notify_type == AIN_USER_DEFINED) {
    bot_com *com = (bot_com *)data->extra_info;

    return ReceiveCommand(me, data->evt_ai_notify.it_handle, com->action, com->ptr);
  }

  return true;
}

// Handles application of damage to an alien
void AlienBoss::DoDamage(int me, tOSIRISEVTDAMAGED *damage_data) {
  // Handle fire damage
  if (damage_data->damage_type == GD_FIRE) {
    memory->damage += (damage_data->damage * 0.6f);
    return;
  }

  // Handle damage by matter weapons
  if (damage_data->damage_type == GD_MATTER) {
    memory->damage += (damage_data->damage * 0.7f);

    if (Game_GetTime() >= memory->next_special_damage_time) {
      memory->next_special_damage_time = Game_GetTime() + 0.8f + ((float)rand() / (float)D3_RAND_MAX) * 0.5f;

      // Create a frag burst effect
      vector pos;
      int room;
      Obj_Value(me, VF_GET, OBJV_V_POS, &pos);
      Obj_Value(me, VF_GET, OBJV_I_ROOMNUM, &room);
      Obj_Create(OBJ_WEAPON, frag_burst_effect_id, room, &pos, NULL, me);
    }

    return;
  }

  // Handle damage by energy weapons
  if ((damage_data->damage_type == GD_ENERGY || damage_data->damage_type == GD_ELECTRIC) &&
      Game_GetTime() >= memory->next_special_damage_time) {
    // Check if we're currently susceptible to energy damage
    if (false) {
      memory->next_special_damage_time = Game_GetTime() + 0.1f + ((float)rand() / (float)D3_RAND_MAX) * 0.2f;

      // Do double damage
      memory->damage += (damage_data->damage * 1.5f);

      // Create sparks
      int room;
      vector pos;
      Obj_Value(me, VF_GET, OBJV_V_POS, &pos);
      Obj_Value(me, VF_GET, OBJV_I_ROOMNUM, &room);
      Game_CreateRandomSparks(30 + int(((float)rand() / (float)D3_RAND_MAX) * 10.0f), &pos, room);

      return;
    } else {
      // Do standard energy damage
      memory->damage += (damage_data->damage * 0.3f);
      return;
    }
  }

  // Apply standard damage mod
  memory->damage += (damage_data->damage * 0.1f);
}

// Handles attacks on aliens
void AlienBoss::DoCollide(int me, tOSIRISEVTCOLLIDE *collide_data) {
  // See if it's a player or player weapon that hit us
  if (qObjIsPlayerOrPlayerWeapon(collide_data->it_handle)) {
    // mprintf(0,"Hit by player!\n");
    memory->hit_by_player = true;
  }
}

// Does any final cleanup when object is destroyed
void AlienBoss::DoCleanUp(int me) {
  // remove the powerup props
  msafe_struct mo;
  mo.objhandle = memory->tail_pos_handle;
  MSafe_CallFunction(MSAFE_OBJECT_REMOVE, &mo);
}

// Receives all basic events and calls processesing functions
int16_t AlienBoss::CallEvent(int event, tOSIRISEventInfo *data) {
  switch (event) {
  case EVT_AI_INIT:
    DoInit(data->me_handle);
    break;
  case EVT_AI_FRAME:
    DoFrame(data->me_handle);
    break;
  case EVT_AI_NOTIFY:
    return (DoNotify(data->me_handle, data) != false) ? CONTINUE_CHAIN | CONTINUE_DEFAULT : 0;
    break;
  case EVT_DAMAGED: {
    DoDamage(data->me_handle, &data->evt_damaged);

    // If boss is slightly damaged, restore it's hitpoints (so it can never die!)
    float curr_shields;
    Obj_Value(data->me_handle, VF_GET, OBJV_F_SHIELDS, &curr_shields);
    if (curr_shields < memory->base_shields * 0.75f) {
      Obj_Value(data->me_handle, VF_SET, OBJV_F_SHIELDS, &memory->base_shields);
    }

    // Only allow it to take very little damage (must take some or it doesn't catch on fire)
    data->evt_damaged.damage = 0.000001f;
  } break;
  case EVT_DESTROY:
    DoCleanUp(data->me_handle);
    break;
  case EVT_COLLIDE:
    DoCollide(data->me_handle, &data->evt_collide);
    break;
  case EVT_MEMRESTORE:
    memory = (alienboss_data *)data->evt_memrestore.memory_ptr;
    DoCustomLookups();
    break;
  }

  return CONTINUE_CHAIN | CONTINUE_DEFAULT;
}

//-----------------------
// Security Camera class
//-----------------------

// Sends a command out to another alien
bool SecurityCamera::SendCommand(int me, int it, char command, void *ptr) {
  bot_com com;

  com.action = command;
  com.ptr = ptr;

  tOSIRISEventInfo ei;

  ei.me_handle = it;
  ei.extra_info = (void *)&com;
  ei.evt_ai_notify.notify_type = AIN_USER_DEFINED;
  ei.evt_ai_notify.it_handle = me;

  return Obj_CallEvent(it, EVT_AI_NOTIFY, &ei);
}

// Processes a command from another alien
bool SecurityCamera::ReceiveCommand(int me, int it, char command, void *ptr) {
  switch (command) {
  // Alien is asked to return current mode
  case SC_COM_GET_ALERT_STATUS:
    *(bool *)ptr = memory->alerted;
    return true;
    break;
  }

  return false;
}

// Sets the Current Mode
void SecurityCamera::SetMode(int me, char mode) {
  int flags;
  float curr_anim_frame;

  // Get the current animation frame
  Obj_Value(me, VF_GET, OBJV_F_ANIM_FRAME, &curr_anim_frame);

  // mprintf(0, "From mode %d, ",memory->mode);

  // Do mode specific setups
  switch (mode) {
  case SC_PANNING: {
    float time;

    // Get the camera ready for looping
    memory->panning_state = SC_WARMING_UP;
    if (curr_anim_frame > SC_MAX_FRAME)
      curr_anim_frame = SC_MAX_FRAME;

    time = (SC_MAX_FRAME - curr_anim_frame) / SC_PAN_SPEED;
    Obj_SetCustomAnim(me, curr_anim_frame, SC_MAX_FRAME, time, AIAF_IMMEDIATE, -1, -1);
    mprintf(0, "Warming up...\n");
  } break;

  case SC_TRACKING: {
    memory->next_activity_time = Game_GetTime();
    // memory->last_update_anim_time = Game_GetTime();

    mprintf(0, "Camera spotted you!\n");
  } break;

  default:
    mprintf(0, "Security Camera is all messed up!\n");
    break;
  }

  // Set the current mode and reset the mode time
  memory->mode = mode;
  memory->mode_time = 0.0f;
}

// Processes the AI Initialize event
void SecurityCamera::DoInit(int me) {
  // Setup memory data
  tOSIRISMEMCHUNK ch;
  ch.id = SC_MEMORY_ID;
  ch.size = sizeof(securitycamera_data);
  ch.my_id.type = OBJECT_SCRIPT;
  ch.my_id.objhandle = me;

  memory = (securitycamera_data *)Scrpt_MemAlloc(&ch);

  memory->alerted = false;

  SetMode(me, SC_PANNING);
}

// Processes the AI Frame Interval Event
void SecurityCamera::DoFrame(int me) {
  float last_see_time, last_see_game_time;
  float curr_anim_frame;

  // Get the last see time
  AI_Value(me, VF_GET, AIV_F_LAST_SEE_TARGET_TIME, &last_see_game_time);

  // Calculate the time since target was seen or heard
  last_see_time = Game_GetTime() - last_see_game_time;

  // Get the current animation frame
  Obj_Value(me, VF_GET, OBJV_F_ANIM_FRAME, &curr_anim_frame);

  // Do mode specific checks
  switch (memory->mode) {
  case SC_PANNING: {
    // If we have a target switch to tracking mode
    if (last_see_time < SC_TIME_TO_GAIN_TARGET) {
      SetMode(me, SC_TRACKING);
      break;
    }

    // If we're warming up, see if we should start looping yet
    if (memory->panning_state == SC_WARMING_UP) {
      // See if we're at the final frame
      if (curr_anim_frame >= SC_MAX_FRAME) {
        memory->panning_state = SC_LOOPING;

        float time = (SC_MAX_FRAME - SC_START_FRAME) / SC_PAN_SPEED;
        Obj_SetCustomAnim(me, SC_START_FRAME, SC_MAX_FRAME, time, AIAF_IMMEDIATE | AIAF_LOOPING, -1, -1);
        mprintf(0, "Looping pan started...\n");
      }
    }
  } break;

  case SC_TRACKING: {
    // If we haven't seen target for a while, return to panning
    if (last_see_time > SC_TIME_TO_LOSE_TARGET) {
      SetMode(me, SC_PANNING);
      break;
    }

    // If we've been tracking target long enough, switch to alert mode
    if (Game_GetTime() >= memory->next_activity_time) {
      memory->next_activity_time = Game_GetTime() + SC_TRACK_INTERVAL;

      // Track the current target
      vector local_vec_to_target, world_vec_to_target, dir, local_fvec;
      matrix orient;
      double theta;
      float frame_offset, dot, curr_frame, dest_frame, anim_time, next_frame;

      // Get the vec to target in local security camera space
      Obj_Value(me, VF_GET, OBJV_M_ORIENT, &orient);
      AI_Value(me, VF_GET, AIV_V_VEC_TO_TARGET, &world_vec_to_target);
      local_vec_to_target = world_vec_to_target * orient;

      // Make it track target on a 2D plane
      dir = local_vec_to_target;
      dir.y = 0.0f;
      if ((dir.x + dir.y + dir.z) == 0.0f)
        break;
      vm_VectorNormalize(&dir);

      local_fvec.x = 0.0f;
      local_fvec.y = 0.0f;
      local_fvec.z = 1.0f;
      dot = dir * local_fvec;
      if (dot < -1.0f)
        dot = -1.0f;
      if (dot > 1.0f)
        dot = 1.0f;
      theta = acos(dot);
      theta = theta / PI * 4.0;

      frame_offset = float(theta) * SC_MAX_FRAME_OFFSET;
      if (frame_offset > SC_MAX_FRAME_OFFSET)
        frame_offset = SC_MAX_FRAME_OFFSET;

      // mprintf(0,"x=%.3f, y=%.3f, z=%.3f - dot=%.3f, theta=%.1f\n",dir.x,dir.y,dir.z,dot,theta);
      // mprintf(0,"Frame offset: %.1f\n",frame_offset);

      // Get the dest frame between start and end frames for reference
      if (dir.x < 0.0f)
        frame_offset = -frame_offset;
      dest_frame = SC_MID_FRAME + frame_offset;

      // Get the current frame between start and end frames for reference
      curr_frame = curr_anim_frame;
      if (curr_frame > SC_END_FRAME)
        curr_frame = SC_MAX_FRAME - curr_frame;

      // Prevent wobbling between two frames
      if (abs(dest_frame - curr_frame) <= SC_FRAME_DELTA_ERROR) {
        dest_frame = curr_frame;
      }

      // Check if frames are reversed, and adjust accordingly
      if (dest_frame < curr_frame) {
        dest_frame = SC_MAX_FRAME - dest_frame;
        curr_frame = SC_MAX_FRAME - curr_frame;
      }

      // Calculate the animation time to go from curr_frame to dest_frame
      anim_time = (dest_frame - curr_frame) / SC_TRACK_SPEED;
      /*
      // Calculate the max distance we can move it this frame
      anim_time = Game_GetTime() - memory->last_update_anim_time;
      next_frame = curr_frame + (anim_time*SC_PAN_SPEED);
      if(next_frame > dest_frame) next_frame=dest_frame;
      Obj_SetCustomAnim(me, next_frame, next_frame, 0.0f, AIAF_IMMEDIATE, -1, -1);
      */

      // Set the next custom animation
      Obj_SetCustomAnim(me, curr_frame, dest_frame, anim_time, AIAF_IMMEDIATE, -1, -1);
      // mprintf(0,"Curr frame: %.6f, Dest frame: %.6f, Anim time: %.1f\n",curr_frame,dest_frame,anim_time);

      // Record the last update anim time
      // memory->last_update_anim_time = Game_GetTime();
    }

    // If we've successfully tracked target for a while, then we are alerted
    if (memory->mode_time >= SC_TRACK_TIME_TO_ALERT && !memory->alerted) {
      memory->alerted = true;
      mprintf(0, "Alerted!\n");
    }
  } break;
  }

  // mprintf(0,"Anim frame: %.2f\n",curr_anim_frame);

  // if(last_see_time < 3.0f)
  //	mprintf(0,"Camera saw target %.2f seconds ago!\n",last_see_time);

  // Increment the current mode time
  memory->mode_time += Game_GetFrameTime();
}

// Process all AI Notify events
bool SecurityCamera::DoNotify(int me, tOSIRISEventInfo *data) {
  if (data->evt_ai_notify.notify_type == AIN_USER_DEFINED) {
    bot_com *com = (bot_com *)data->extra_info;

    return ReceiveCommand(me, data->evt_ai_notify.it_handle, com->action, com->ptr);
  }

  return true;
}

// Receives all basic events and calls processesing functions
int16_t SecurityCamera::CallEvent(int event, tOSIRISEventInfo *data) {
  switch (event) {
  case EVT_AI_INIT:
    DoInit(data->me_handle);
    break;
  case EVT_AI_FRAME:
    DoFrame(data->me_handle);
    break;
  case EVT_AI_NOTIFY:
    return (DoNotify(data->me_handle, data) != false) ? CONTINUE_CHAIN | CONTINUE_DEFAULT : 0;
    break;
  case EVT_DAMAGED:
    // DoDamage(data->me_handle, &data->evt_damaged);
    break;
  case EVT_DESTROY:
    break;
  case EVT_COLLIDE:
    // DoCollide(data->me_handle, &data->evt_collide);
    break;
  case EVT_MEMRESTORE:
    memory = (securitycamera_data *)data->evt_memrestore.memory_ptr;
    break;
  }

  return CONTINUE_CHAIN | CONTINUE_DEFAULT;
}

//-----------------------
// Crowd Control class
//-----------------------

// Sends a command out to another alien
bool CrowdControl::SendCommand(int me, int it, char command, void *ptr) {
  bot_com com;

  com.action = command;
  com.ptr = ptr;

  tOSIRISEventInfo ei;

  ei.me_handle = it;
  ei.extra_info = (void *)&com;
  ei.evt_ai_notify.notify_type = AIN_USER_DEFINED;
  ei.evt_ai_notify.it_handle = me;

  return Obj_CallEvent(it, EVT_AI_NOTIFY, &ei);
}

// Processes a command from another alien
bool CrowdControl::ReceiveCommand(int me, int it, char command, void *ptr) {
  switch (command) {
  // Alien is asked to return current mode
  case CC_COM_SET_DATA: {
    cc_packet *data = (cc_packet *)ptr;

    memory->follow_handle = data->follow_handle;
    memory->stop_dist = data->stop_dist;
    memory->slowdown_offset = data->slowdown_offset;

    mprintf(0, "Set Crowd Control Data.\n");
    return true;
  } break;

  case CC_COM_DISABLE_CHECK: {
    memory->disable_check = *(bool *)ptr;

    mprintf(0, "Crowd control disable check set to: %d\n", memory->disable_check);
  } break;
  }

  return false;
}

// Processes the AI Initialize event
void CrowdControl::DoInit(int me) {
  // Setup memory data
  tOSIRISMEMCHUNK ch;
  ch.id = CC_MEMORY_ID;
  ch.size = sizeof(crowdcontrol_data);
  ch.my_id.type = OBJECT_SCRIPT;
  ch.my_id.objhandle = me;

  memory = (crowdcontrol_data *)Scrpt_MemAlloc(&ch);

  AI_Value(me, VF_GET, AIV_F_MAX_SPEED, &memory->base_speed);

  memory->follow_handle = OBJECT_HANDLE_NONE;
  memory->stop_dist = 0.0f;
  memory->slowdown_offset = 0.0f;

  memory->disable_check = false;

  memory->next_check_time = Game_GetTime();

  mprintf(0, "CrowdControl Initialized.\n");
}

// Processes the AI Frame Interval Event
void CrowdControl::DoFrame(int me) {
  // If data hasn't been set yet, just bail
  if (memory->follow_handle == OBJECT_HANDLE_NONE)
    return;

  // See if we need to do a distance check yet
  if (Game_GetTime() >= memory->next_check_time) {
    memory->next_check_time = Game_GetTime() + CC_CHECK_INTERVAL;

    float my_max_speed;
    bool ok_to_move_normally = false;

    // Get my current max speed
    AI_Value(me, VF_GET, AIV_F_MAX_SPEED, &my_max_speed);

    // Make sure the object we're following still exists
    if (!memory->disable_check && qObjExists(memory->follow_handle)) {
      vector my_vel, my_pos, leader_pos, leader_dir;
      matrix my_orient;
      float my_max_speed, dist, new_speed;

      Obj_Value(me, VF_GET, OBJV_V_VELOCITY, &my_vel);
      Obj_Value(me, VF_GET, OBJV_V_POS, &my_pos);
      Obj_Value(me, VF_GET, OBJV_M_ORIENT, &my_orient);
      Obj_Value(memory->follow_handle, VF_GET, OBJV_V_POS, &leader_pos);

      // It's ok to move in reverse, or away from leader
      ok_to_move_normally = ((my_vel * my_orient.fvec) <= 0.0f) ? true : false;
      if (!ok_to_move_normally) {
        leader_dir = leader_pos - my_pos;
        ok_to_move_normally = ((my_vel * leader_dir) <= 0.0f) ? true : false;
      }

      // Check to see if we're too close to leader (so we can slow down and stop)
      dist = vm_VectorDistance(&my_pos, &leader_pos);
      if (!ok_to_move_normally && dist < (memory->stop_dist + memory->slowdown_offset)) {
        if (dist <= memory->stop_dist || memory->slowdown_offset == 0.0f) {
          new_speed = CC_MIN_STOPPING_SPEED;
        } else {
          float curr_diff;
          curr_diff = dist - memory->stop_dist;

          new_speed = CC_MIN_STOPPING_SPEED + ((curr_diff / memory->slowdown_offset) * memory->base_speed);
        }

        AI_Value(me, VF_SET, AIV_F_MAX_SPEED, &new_speed);
      }
    } else {
      ok_to_move_normally = true;
    }

    // Restore the max speed to the original value if necessary
    if (ok_to_move_normally && my_max_speed != memory->base_speed) {
      AI_Value(me, VF_SET, AIV_F_MAX_SPEED, &memory->base_speed);
    }
  }
}

// Process all AI Notify events
bool CrowdControl::DoNotify(int me, tOSIRISEventInfo *data) {
  if (data->evt_ai_notify.notify_type == AIN_USER_DEFINED) {
    bot_com *com = (bot_com *)data->extra_info;

    return ReceiveCommand(me, data->evt_ai_notify.it_handle, com->action, com->ptr);
  }

  return true;
}

// Receives all basic events and calls processesing functions
int16_t CrowdControl::CallEvent(int event, tOSIRISEventInfo *data) {
  switch (event) {
  case EVT_AI_INIT:
    DoInit(data->me_handle);
    break;
  case EVT_AI_FRAME:
    DoFrame(data->me_handle);
    break;
  case EVT_AI_NOTIFY:
    return (DoNotify(data->me_handle, data) != false) ? CONTINUE_CHAIN | CONTINUE_DEFAULT : 0;
    break;
  case EVT_DAMAGED:
    // DoDamage(data->me_handle, &data->evt_damaged);
    break;
  case EVT_DESTROY:
    break;
  case EVT_COLLIDE:
    // DoCollide(data->me_handle, &data->evt_collide);
    break;
  case EVT_MEMRESTORE:
    memory = (crowdcontrol_data *)data->evt_memrestore.memory_ptr;
    break;
  }

  return CONTINUE_CHAIN | CONTINUE_DEFAULT;
}
