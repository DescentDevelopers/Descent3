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

 * $Logfile: /DescentIII/data/scripts/osiris_common.h $
 * $Revision: 1.2 $
 * $Date: 2004/02/09 04:14:52 $
 * $Author: kevinb $
 *
 * Contains code that is common along all modules.  Mostly defines, flags, etc.
 *
 * $Log: osiris_common.h,v $
 * Revision 1.2  2004/02/09 04:14:52  kevinb
 * Added newlines to all headers to reduce number of warnings printed
 *
 * Made some small changes to get everything compiling.
 *
 * All Ready to merge the 1.5 tree.
 *
 * Revision 1.1  2000/05/29 05:48:29  icculus
 * initial import.
 *
 * Revision 1.1  2000/04/18 01:45:56  icculus
 * Needed for compilation.
 *
 *
 * 146   11/03/99 10:20a Chris
 * Added the ability to find out if a room is volatile
 *
 * 145   11/02/99 12:16p Chris
 * More Merc code - added support for ignore_obj_lists for the Osiris
 * RayCast function
 *
 * 144   10/27/99 4:18p 3dsmax
 * added cloak flag for Obj_Value
 *
 * 143   10/23/99 6:16p Nate
 * Added define for Polish Language
 *
 * 142   10/16/99 8:44p Jeff
 * created defines to remove player weapons and all powerups from a room
 *
 * 141   10/12/99 4:50p Jeff
 *
 * 140   9/10/99 11:03a Samir
 * added command to get object's name from Obj_Value.
 *
 * 139   6/23/99 12:49p Samir
 * fixed for Linux builds
 *
 * 138   5/21/99 7:29a Chris
 * More tweaking...
 *
 * 137   5/19/99 11:25a Matt
 * Added multisafe functions & Dallas actions for showing a timer on the
 * screen and adding custom HUD messages.
 *
 * 136   5/17/99 6:22p Jeff
 * added filtered HUD messages
 *
 * 135   5/10/99 9:36p Ardussi
 * changes to compile on Mac
 *
 * 134   5/10/99 12:23a Chris
 * Fixed another hearing/seeing case.  :)  Buddy bot now is in the player
 * ship at respawn
 *
 * 133   5/06/99 6:38a Chris
 *
 * 132   5/06/99 4:11a Jeff
 * created multisafe function to destroy robots in the level (helps boss
 * fights)
 *
 * 131   5/04/99 6:53p Jeff
 * added event for when a player dies
 *
 * 130   5/04/99 12:13p Matt
 * Renamed fire-flare symbol to fire-weapon.
 *
 * 129   4/30/99 4:32p Matt
 * Added a Dallas action to fire a flare from an object.
 *
 * 128   4/29/99 3:25p Chris
 * Added support to find out if an object has wbs
 *
 * 127   4/29/99 1:59a Chris
 * Added the portal blockage support
 *
 * 126   4/27/99 4:27p Jeff
 * added player respawn osiris event
 *
 * 125   4/27/99 4:40a Jeff
 * added msafe member to set/get guidebot name
 *
 * 124   4/26/99 4:27a Jeff
 * added multisafe functions to check a player's weapons, and add/remove
 * weapons from a player
 *
 * 123   4/24/99 6:44p Jeff
 * added functions for theif so he can steal things other than weapons
 *
 * 122   4/24/99 12:09a Jeff
 * added path value functions
 *
 * 121   4/22/99 4:58p Kevin
 * fixed bad merge
 *
 * 120   4/22/99 4:44p Jason
 * fixed level goal stuff for multiplayer
 *
 * 118   4/20/99 7:28p Jeff
 * added guidebot name
 *
 * 117   4/20/99 12:00a Jeff
 * Linux compile
 *
 * 116   4/06/99 11:44p Matt
 * Added action to create a lighting bolt between two gunpoints on the
 * same object, and queries to return an object's original shields and to
 * multiply a percent times a float.
 *
 * 115   4/06/99 8:44p Matt
 * Added a couple actions for Mark's energy/shield collector
 *
 * 114   4/04/99 8:21p Matt
 * Fixed include problems with osiris vector stuff.  Osiris headers now
 * include vecmat_external.h, instead of defining the typedefs, structs,
 * and inlines on the Osiris side.
 *
 * 113   4/04/99 6:20p Matt
 * Added distance-based shake Dallas action.
 *
 * 112   4/04/99 4:55p Jeff
 * gamecinematics_external.h was missing for some reason
 *
 * 111   4/03/99 1:13a Jeff
 * added multisafe/dallas actions to set an object on fire
 *
 * 110   4/02/99 10:12p Jeff
 * made ship permissions multi safe
 *
 * 109   4/02/99 10:18a Chris
 * We can now mess with the Object_info anim stuff
 *
 * 108   3/31/99 11:11a Luke
 * fixed another bad merge (Jeff)
 *
 * 107   3/30/99 8:15p Jeff
 * bad merge
 *
 * 106   3/30/99 7:25p Chris
 * Make the weath_lightning stuff more flexable
 *
 * 104   3/30/99 4:32p Chris
 * Moved subtype to the main goal sturct (from goal_info).  Made move
 * relative object vec finishable.  (Like get behind player)
 *
 * 103   3/28/99 5:56p Matt
 * Added Dallas action to turn on object sparking
 *
 * 102   3/27/99 12:29p Chris
 * Fixed problems with assigned lower priority goals that used paths (it
 * would confuse the bot because the .path field was getting updated for
 * the new goal even though it was lower priority)
 *
 * 101   3/26/99 12:38p Jeff
 * added cloak predefs
 *
 * 100   3/22/99 6:35p Matt
 * Added Dallas action to create a lightning bolt between two objects.
 * Jason will implement this.
 *
 * 99    3/22/99 1:59p Matt
 * Added break glass Dallas action
 *
 * 98    3/22/99 10:56a Chris
 * Awareness code was tweaked.  Multisafe stuff added for objects.
 *
 * 97    3/17/99 3:55p Nate
 * Added language defines
 *
 * 96    3/16/99 11:26a Chris
 * Buddy is now smarter -- gets to trigger level goals
 *
 * 95    3/10/99 1:33p Chris
 * Fixes for multi-line gb messages
 *
 * 94    3/04/99 6:13p Matt
 * Added message2 buffer to msafe_struct, and made the message longer.
 *
 * 93    3/03/99 3:34p Chris
 *
 * 92    3/03/99 3:02p Matt
 * Added Game Message action in Dallas
 *
 * 91    3/03/99 3:01a Chris
 * Exported the difficulty stuff to OSIRIS
 *
 * 90    2/28/99 11:30p Chris
 * FindObjOfType and OSIRIS controllable deaths
 *
 * 89    2/25/99 8:54p Jeff
 * Inventory supports level change persistant items.  Inventory supports
 * time-out objects.  Inventory Reset changed (takes a level of reset
 * now).  Quad lasers stay across level change (single player).  Guidebot
 * bug fixed (now back in ship on level start).  Quads time out when
 * spewed.  Invulnerability and cloak powerups no longer use game
 * event/callbacks, so they can be saved in game saves (moved to
 * MakePlayerInvulnerable and MakeObjectInvisible)
 *
 * 88    2/25/99 5:43p Chris
 * Massive improvement to BOA and AI (again)
 *
 * 87    2/22/99 5:59p Chris
 * Level Goals are now in OSIRIS.  :)
 *
 * 86    2/22/99 1:20a Jeff
 * added support for inventory (simple) in dallas.  Moved end-level
 * sequence to use IGC.  Add position clipboard stuff for dallas.  Fixed
 * some inventory bug with storing object handles
 *
 * 85    2/21/99 7:16p Chris
 * Added the EVT_LEVEL_GOAL_ITEM_COMPLETE event
 *
 * 84    2/21/99 5:51p Chris
 * FIxed a bug with OBJV_C_TYPE (It should have been OBJV_I_TYPE).  Added
 * Obj_FindType()
 *
 * 83    2/21/99 5:49p Matt
 * Added Dallas action to set an object's volume
 *
 * 82    2/20/99 4:28p Chris
 * SOme updates to the system, not done
 *
 * 81    2/19/99 5:16p Chris
 * Added the child died event
 *
 * 80    2/17/99 3:25a Jeff
 * added checksum member to init struct
 *
 * 79    2/15/99 11:09a Matt
 * Added function to count the number of a certain type of object in the
 * level. (Also added HUD actions to print an int and float.)
 *
 * 78    2/11/99 2:54a Jeff
 * improvements to introcam
 *
 * 77    2/09/99 12:43p Matt
 * Added function to check for players in a room
 *
 * 76    2/08/99 2:53p Jeff
 * added matcen event data
 *
 * 75    2/08/99 2:47p Jeff
 * new events
 *
 * 74    2/08/99 3:10a Jeff
 * added a multisafe type for player control type setting.  Improved intro
 * cinematic action function, player now follows path.
 *
 * 73    2/06/99 10:03p Matt
 * Added keys system
 *
 * 72    2/06/99 1:52a Matt
 * Added a bunch of Dallas funcs, mostly for Sean
 *
 * 71    2/05/99 9:06p Jeff
 * added aux events for a couple new ainotify's
 *
 * 70    2/03/99 5:48p Matt
 * Added room damage system
 *
 * 69    1/31/99 11:05p Matt
 * Added music region set
 *
 * 68    1/31/99 8:53p Jeff
 * new in game cinematics system finished
 *
 * 67    1/30/99 3:19p Matt
 * Made Dallas spews work from object centers in addition to gunpoints
 *
 * 66    1/29/99 12:47p Matt
 * Rewrote the doorway system
 *
 * 65    1/27/99 11:41p Jeff
 * added dll identifier
 *
 * 64    1/26/99 5:16p Matt
 * Added STOP_SOUND_OBJ
 *
 * 63    1/25/99 7:06p Matt
 * Ripped out doorway index stuff, since doors are referred to by their
 * door objects.
 *
 * 62    1/25/99 8:13a Chris
 * Made the AUX goal remap more obvious...
 *
 * 61    1/24/99 8:17p Chris
 * Updated AI and OSIRIS.  Externalized fireball constants for spew and
 * sparks.  Added CreateRandomSparks to OSIRIS, renamed a bunch of AI
 * Notify names to use underscores.  Fixed a memory access leak in the
 * matcen effect code.
 *
 * 60    1/23/99 2:56a Jeff
 * added target field to boss intro struct
 *
 * 59    1/22/99 6:53p Chris
 * Fixed LoadandBind Aux notify problems, fixed static path problems,
 * fixed AIF_FORCE_AWARE Problems, improved path code
 *
 * 58    1/22/99 3:34p Kevin
 * Increased the MSAFE_MESSAGE_LENGTH to 200 bytes
 *
 * 57    1/20/99 6:26p Matt
 * Added several actions
 *
 */

#ifndef __OSIRIS_COMMON_H_
#define __OSIRIS_COMMON_H_

#include <cstdint>

#include "vecmat_external.h"
#include "aistruct_external.h"
#include "object_external.h"
#include "matcen_external.h"
#include "robotfirestruct_external.h"
#include "findintersection_external.h"
#include "weapon_external.h"
#include "damage_external.h"
#include "fireball_external.h"
#include "levelgoal_external.h"
#include "deathinfo_external.h"
#include "difficulty_external.h"
#include "gamecinematics_external.h"
#include "player_external.h"

// =======================================================================
// Event return values (OR these if needed)
// =======================================================================
#define CONTINUE_CHAIN 0x0100   // continue executing the chain of scripts
#define CONTINUE_DEFAULT 0x0001 // continue executing the 'in-code' action

// =======================================================================
// Events that OSIRIS handles
// =======================================================================
#define EVT_INTERVAL 0x100         // called every frame
#define EVT_AI_FRAME 0x101         // called every frame for AI info
#define EVT_DAMAGED 0x102          // called when object is damaged
#define EVT_COLLIDE 0x103          // called when object collides with something
#define EVT_CREATED 0x104          // called when object created
#define EVT_DESTROY 0x105          // called when object destroyed
#define EVT_TIMER 0x106            // called when a timer event is signalled
#define EVT_USE 0x107              // called when item is selected for use from inventory
#define EVT_AI_NOTIFY 0x110        // called when an AI gets notified
#define EVT_AI_INIT 0x111          // called to initialize SCRIPT AI stuff
#define EVT_LEVELSTART EVT_CREATED //
#define EVT_LEVELEND EVT_DESTROY   // called when level is started and ended.
#define EVT_CHANGESEG 0x115        // called when object changes room
#define EVT_SAVESTATE 0x117        // called when the script should save it's state
#define EVT_RESTORESTATE 0x118     // called when the script should restore it's state
#define EVT_MEMRESTORE                                                                                                 \
  0x119 // called when the script should restore a pointer to the special auto-save memory it allocated
#define EVT_TIMERCANCEL 0x11A // called when a timer is cancels (either by function call or from it's object detonator)
#define EVT_AIN_OBJKILLED 0x11B          // child event of EVT_AI_NOTIFY
#define EVT_AIN_SEEPLAYER 0x11C          // child event of EVT_AI_NOTIFY
#define EVT_AIN_WHITOBJECT 0x11D         // child event of EVT_AI_NOTIFY
#define EVT_AIN_GOALCOMPLETE 0x11E       // child event of EVT_AI_NOTIFY
#define EVT_AIN_GOALFAIL 0x11F           // child event of EVT_AI_NOTIFY
#define EVT_AIN_MELEE_HIT 0x120          // child event of EVT_AI_NOTIFY
#define EVT_AIN_MELEE_ATTACK_FRAME 0x121 // child event of EVT_AI_NOTIFY
#define EVT_AIN_MOVIE_START 0x122        // child event of EVT_AI_NOTIFY
#define EVT_AIN_MOVIE_END 0x123          // child event of EVT_AI_NOTIFY
#define EVT_MATCEN_CREATE 0x124          // level event that a matcen created an object
#define EVT_DOOR_ACTIVATE 0x125          // event to when a door is opening
#define EVT_DOOR_CLOSE 0x126             // event to when a door is closing
#define EVT_CHILD_DIED 0x127
#define EVT_LEVEL_GOAL_COMPLETE 0x128
#define EVT_ALL_LEVEL_GOALS_COMPLETE 0x129
#define EVT_LEVEL_GOAL_ITEM_COMPLETE 0x12A
#define EVT_PLAYER_MOVIE_START 0x12B // event when a IGC focusing on the player starts
#define EVT_PLAYER_MOVIE_END 0x12C   // event when a IGC focusing on the player ends
#define EVT_PLAYER_RESPAWN 0x12D     // event when a player respawns
#define EVT_PLAYER_DIES 0x12E        // event when a player dies

// =======================================================================
// General defines
// =======================================================================
#define MAX_MODULEFUNCS 256 // maximum number of functions imported to OSIRIS

typedef int OMMSHANDLE;

// =======================================================================
// Language defines
// =======================================================================
#define LANGUAGE_ENGLISH 0
#define LANGUAGE_GERMAN 1
#define LANGUAGE_SPANISH 2
#define LANGUAGE_ITALIAN 3
#define LANGUAGE_FRENCH 4
#define LANGUAGE_POLISH 5

// Player Accessories indices (for Player_Value function)
#define ACCESSORY_CLOAK 0
#define ACCESSORY_INVULNERABILITY 1
#define ACCESSORY_AFTERBURNER 2
#define ACCESSORY_HEADLIGHT 3
#define ACCESSORY_QUAD 4
#define ACCESSORY_RAPID_FIRE 5
#define ACCESSORY_MAP 6

#define MSAFE_ROOM 0
#define MSAFE_OBJ 20
#define MSAFE_WEATHER 100
#define MSAFE_SOUND 110
#define MSAFE_MATCEN 130
#define MSAFE_MISC 150
#define MSAFE_DOOR 170
#define MSAFE_MISSION 180
#define MSAFE_TRIGGER 190
#define MSAFE_INVEN 200
#define MSAFE_COUNTERMEASURE 210
#define MSAFE_WEAPON 250

#define SPEW_RAND_SPEED 2
#define SPEW_RAND_SIZE 4
#define SPEW_RAND_LIFETIME 16

// =======================================================================
// Value Functions
// =======================================================================

#define VF_SET 0
#define VF_GET 1
#define VF_SET_FLAGS 2
#define VF_CLEAR_FLAGS 3

// =======================================================================
// Value - Player only values
// =======================================================================

#define PLYV_F_ENERGY 0
#define PLYV_F_MOVEMENT_SCALAR 1
#define PLYV_F_RECHARGE_SCALAR 2
#define PLYV_F_WSPEED_SCALAR 3
#define PLYV_F_DAMAGE_SCALAR 4
#define PLYV_F_ARMOR_SCALAR 5
#define PLYV_I_BUDDY_HANDLE 6

// These work off the weapon index
#define PLYSV_I_WEAPON 7
#define PLYSV_I_WEAPON_AMMO 8
#define PLYSV_US_WEAPON_POWERUP_ID 9

// These work off the accessory index
#define PLYSV_F_ACCESSORY 10

// This is for setting and getting the guidebot name
#define PLYV_CS_GUIDEBOTNAME 11

// this is for thief access
#define PLYV_B_THIEF_PLAYERHASITEM 12   // VF_GET only (takes a bool for ptr, int for index..which item..see THIEFITEM_)
#define PLYV_I_THIEF_STEALPLAYERITEM 13 // VF_SET only (takes an int for index, ptr is ignored)

#define PLYV_I_FLAGS 14         // Player flags
#define PLYV_I_STRIP_WEAPONS 15 // VF_SET only (removes all weapons but lasers, sets energy to 0)

// These are used to mask out various controls, put in initially for the training system
#define PLYV_CMASK_FORWARD 1
#define PLYV_CMASK_REVERSE 2
#define PLYV_CMASK_LEFT 4
#define PLYV_CMASK_RIGHT 8
#define PLYV_CMASK_UP 16
#define PLYV_CMASK_DOWN 32
#define PLYV_CMASK_PITCHUP 64
#define PLYV_CMASK_PITCHDOWN 128
#define PLYV_CMASK_HEADINGLEFT 256
#define PLYV_CMASK_HEADINGRIGHT 512
#define PLYV_CMASK_BANKLEFT 1024
#define PLYV_CMASK_BANKRIGHT 2048
#define PLYV_CMASK_PRIMARY 4096
#define PLYV_CMASK_SECONDARY 8192
#define PLYV_CMASK_AFTERBURNER 16384

// =======================================================================
// Value - AI Values
// =======================================================================

#define AIV_F_MAX_SPEED 0
#define AIV_F_MAX_DELTA_SPEED 1
#define AIV_F_MAX_TURN_RATE 2
#define AIV_F_MAX_DELTA_TURN_RATE 3
#define AIV_F_ATTACK_VEL_PERCENT 4
#define AIV_F_FLEE_VEL_PERCENT 5
#define AIV_F_DODGE_VEL_PERCENT 6
#define AIV_F_CIRCLE_DIST 7
#define AIV_F_DODGE_PERCENT 8
#define AIV_F_MELEE_DAMAGE1 9
#define AIV_F_MELEE_DAMAGE2 10
#define AIV_F_MELEE_LATENCY1 11
#define AIV_F_MELEE_LATENCY2 12
#define AIV_C_MOVEMENT_TYPE 13
#define AIV_C_MOVEMENT_SUBTYPE 14
#define AIV_C_ANIMATION_TYPE 15
#define AIV_C_NEXT_ANIMATION_TYPE 16
#define AIV_C_NEXT_MOVEMENT 17
#define AIV_C_CURRENT_WB_FIRING 18
#define AIV_I_TARGET_HANDLE 19
#define AIV_F_NEXT_TARGET_UPDATE_TIME 20
#define AIV_F_DIST_TO_TARGET 21
#define AIV_V_VEC_TO_TARGET 22
#define AIV_F_NEXT_CHECK_SEE_TARGET_TIME 23
#define AIV_V_LAST_SEE_TARGET_POS 24
#define AIV_F_LAST_SEE_TARGET_TIME 25
#define AIV_F_WEAPON_SPEED 26
#define AIV_F_NEXT_MELEE_TIME 27
#define AIV_F_LAST_RENDER_TIME 28
#define AIV_F_NEXT_FLINCH_TIME 29
#define AIV_V_MOVEMENT_DIR 30
#define AIV_V_ROT_THRUST_VECTOR 31
#define AIV_F_FOV 32
#define AIV_F_AVOID_FRIENDS_DIST 33
#define AIV_F_FRUSTRATION 34
#define AIV_F_CURIOUSITY 35
#define AIV_F_FIRE_SPREAD 36
#define AIV_F_AGRESSION 37
#define AIV_F_NIGHT_VISION 38
#define AIV_F_FOG_VISION 39
#define AIV_F_LEAD_ACCURACY 40
#define AIV_F_LEAD_VARIENCE 41
#define AIV_F_FIGHT_TEAM 42
#define AIV_F_FIGHT_SAME 43
#define AIV_F_HEARING 44
#define AIV_F_ROAMING 45
#define AIV_F_LIFE_PRESERVATION 46
#define AIV_F_BIASED_FLIGHT_IMPORTANCE 47
#define AIV_F_BIASED_FLIGHT_MIN 48
#define AIV_F_BIASED_FLIGHT_MAX 49
#define AIV_F_AWARENESS 50
#define AIV_I_FLAGS 51
#define AIV_I_STATUS_REG 52
#define AIV_F_LAST_HEAR_TARGET_TIME 53

// =======================================================================
// Value - AI Values
// =======================================================================

#define AIGV_I_TYPE 0
#define AIGV_C_ACTIVATION_LEVEL 1
#define AIGV_F_MIN_INFLUENCE 2
#define AIGV_F_MAX_INFLUENCE 3

#define AIGSV_F_INFLUENCE_DIST 4

#define AIGV_I_HANDLE 5
#define AIGV_I_ROOMNUM 6
#define AIGV_I_F_ACTIONS 7
#define AIGV_I_ID 8
#define AIGV_C_SUBTYPE 9
#define AIGV_F_TIME 10
#define AIGV_V_VEC 11
#define AIGV_V_POS 12
#define AIGV_F_STEER_MIN_DIST 13
#define AIGV_F_STEER_MAX_DIST 14
#define AIGV_F_STEER_MAX_STRENGTH 15
#define AIGV_B_ATTACH_F_ALIGNED 16
#define AIGV_B_ATTACH_F_SPHERE 17
#define AIGV_F_ATTACH_RAD 18
#define AIGV_C_ATTACH_CHILD_AP 19
#define AIGV_C_ATTACH_PARENT_AP 20
#define AIGV_I_WANDER_AVOID_HANDLE 21
#define AIGV_C_WANDER_MIN_ROOMS 22
#define AIGV_C_WANDER_MAX_ROOMS 23
#define AIGV_C_WANDER_FLAGS 24
#define AIGV_C_WANDER_MINE_INDEX 25
#define AIGV_F_CIRCLE_DIST 26
#define AIGV_I_STATUS_REG 27
#define AIGV_F_START_TIME 28
#define AIGV_F_NEXT_PATH_TIME 29
#define AIGV_F_DIST_TO_GOAL 30
#define AIGV_V_VEC_TO_TARGET 31
#define AIGV_F_NEXT_CHECK_SEE_TARGET_TIME 32
#define AIGV_V_LAST_SEE_TARGET_POS 33
#define AIGV_F_LAST_SEE_TARGET_TIME 34
#define AIGV_F_NEXT_TARGET_UPDATE_TIME 35
#define AIGV_I_FLAGS 36
#define AIGV_V_ORIENT_FVEC 37
#define AIGV_V_ORIENT_UVEC 38

#define AIGV_C_NUM_ENABLERS 39
#define AIGSV_C_ENABLER_TYPE 40
#define AIGSV_F_ENABLER_TIME 41
#define AIGSV_C_ENABLER_MTYPE 42
#define AIGSV_F_ENABLER_FLOAT_VALUE 43
#define AIGSV_I_ENABLER_FLAGS 44
#define AIGSV_F_ENABLER_DIST 45
#define AIGSV_F_ENABLER_PERCENT 46
#define AIGSV_F_ENABLER_CHECK_INTERVAL 47
#define AIGSV_F_ENABLER_LAST_CHECK_TIME 48
#define AIGSV_C_ENABLER_NEXT_ENABLER_OP 49

#define AIGV_B_USED 50
#define AIGV_I_SCRIPTED_DATA_PTR 51

// =======================================================================
// Value - Object Values
// =======================================================================

#define OBJV_F_SHIELDS 0
#define OBJV_I_TYPE 1
#define OBJV_US_ID 2
#define OBJV_V_POS 3
#define OBJV_M_ORIENT 4
#define OBJV_I_ROOMNUM 5
#define OBJV_V_VELOCITY 6
#define OBJV_V_ROTVELOCITY 7
#define OBJV_V_THRUST 8
#define OBJV_V_ROTTHRUST 9
#define OBJV_I_FLAGS 10
#define OBJV_F_SIZE 11
#define OBJV_C_CONTROL_TYPE 12
#define OBJV_C_MOVEMENT_TYPE 13
#define OBJV_F_CREATION_TIME 14
#define OBJV_F_ROTDRAG 15
#define OBJV_I_PHYSICS_FLAGS 16
#define OBJV_I_PARENT_HANDLE 17
#define OBJV_F_ANIM_FRAME 18
#define OBJV_F_MAX_SHIELDS 19
#define OBJV_PC_MARKER_MESSAGE 20
#define OBJSV_F_ANIM_START 21
#define OBJSV_F_ANIM_END 22
#define OBJSV_F_ANIM_TIME 23
#define OBJV_S_NAME 24           // object's name
#define OBJV_C_VIRUS_INFECTED 25 // the object show signs of virus infection
#define OBJV_C_NEGATIVE_LIGHT 26 // the object casts negative light (black hole)
#define OBJV_C_IS_CLOAKED 27     // is object cloaked?
#define OBJV_PI_HACK_FVI_IGNORE_LIST 28

// =======================================================================
// Value - Object Weapon Battery Values
// =======================================================================

#define WBV_F_ANIM_FRAME 0
#define WBV_F_LAST_FIRE_TIME 1
#define WBV_I_DYNAMIC_FLAGS 2

// WBSV values use the index parameter of Obj_WBValue() for the gun point index
#define WBV_C_NUM_GUNPTS 3
#define WBSV_US_GUNPT_WEAPON_ID 4
#define WBSV_V_GUNPT_POS 5

// WBSV values use the index parameter of Obj_WBValue() for the firing mask index
#define WBV_C_NUM_MASKS 6
#define WBSV_C_MASK 7
#define WBSV_I_FIRE_SOUND 8
#define WBSV_F_LATENCY 9
#define WBSV_F_ANIM_TIME 10
#define WBSV_F_ANIM_START 11
#define WBSV_F_ANIM_FIRE 12
#define WBSV_F_ANIM_END 13

#define WBV_C_NUM_WBS 14

// =======================================================================
// Value - Room Values
// =======================================================================

#define RMV_I_FLAGS 0
#define RMV_V_WIND 1
#define RMV_C_USED 2

#define RMV_I_NUM_PATH_PNTS 3
#define RMSV_V_PATH_PNT 4

#define RMV_I_NUM_FACES 5
#define RMSV_I_FACE_TEXTURE_ID 6
#define RMSV_I_FACE_FLAGS 7
#define RMSV_V_FACE_NORMAL 8
#define RMSV_V_FACE_CENTER_PNT 14 // I didn't want to reorder the other ones as it is late in the build process

#define RMV_I_NUM_PORTALS 9
#define RMSV_V_PORTAL_PATH_PNT 10
#define RMSV_I_PORTAL_CONNECT_ROOM 11
#define RMSV_I_PORTAL_CONNECT_PORTAL 12
#define RMSV_I_PORTAL_FACE 13
#define RMSV_I_PORTAL_FLAGS 15

#define RMV_F_DAMAGE 16

// =======================================================================
// Value - Level Goal Values
// =======================================================================

#define LGV_I_STATUS 0

#define LGV_I_NUM_ACTIVE_PRIMARIES 1
#define LGSV_I_ACTIVE_PRIMARY_GOAL 2

#define LGV_I_NUM_ACTIVE_SECONDARIES 3
#define LGSV_I_ACTIVE_SECONDARY_GOAL 4

#define LGV_I_NUM_GOALS 5
#define LGSV_PC_GOAL_NAME 6
#define LGSV_PC_LOCATION_NAME 7
#define LGSV_PC_DESCRIPTION 8
#define LGSV_PC_COMPLETION_MESSAGE 9
#define LGSV_I_PRIORITY 10
#define LGSV_C_GOAL_LIST 11
#define LGSV_I_STATUS 12

#define LGSV_I_NUM_ITEMS 13
#define LGSSV_C_ITEM_TYPE 14
#define LGSSV_I_ITEM_HANDLE 15
#define LGSSV_B_ITEM_DONE 16

// =======================================================================
// Value - Matcen Values
// =======================================================================

#define MTNV_C_ATTACH_TYPE 0
#define MTNV_C_CONTROL_TYPE 1
#define MTNV_I_ATTACH 2
#define MTNV_V_CREATE_POINT 3
#define MTNV_I_CREATE_ROOM 4
#define MTNV_PC_NAME 5
#define MTNV_I_MAX_PROD 6
#define MTNV_F_PROD_MULTIPLIER 7
#define MTNV_I_STATUS 8
#define MTNV_C_CREATION_EFFECT 9
#define MTNV_I_MAX_ALIVE_CHILDREN 10
#define MTNV_F_PRE_PROD_TIME 11
#define MTNV_F_POST_PROD_TIME 12
#define MTNV_S_CREATION_TEXTURE 13

#define MTNSV_I_SOUND 14

#define MTNV_C_NUM_SPAWN_PTS 15
#define MTNSV_I_SPAWN_POINT 16

#define MTNV_C_NUM_PROD_TYPES 17
#define MTNSV_I_PROD_ITEM_ID 18
#define MTNSV_I_PROD_ITEM_PRIORITY 19
#define MTNSV_F_PROD_ITEM_TIME 20
#define MTNSV_I_PROD_ITEM_MAX_PROD 21

// =======================================================================
// inventory add flags
// =======================================================================
#define INVAF_NOTSPEWABLE 0x0001
#define INVAF_TIMEOUTONSPEW 0x0002
#define INVAF_LEVELLAST 0x0004

// =======================================================================
// thief stealable items (see PLYV_ actions)
// =======================================================================
#define THIEFITEM_AUTOMAP 1
#define THIEFITEM_HEADLIGHT 2
#define THIEFITEM_ETOSCONV 3
#define THIEFITEM_CLOAK 4
#define THIEFITEM_INVULN 5
#define THIEFITEM_RAPIDFIRE 6
#define THIEFITEM_QUADFIRE 7
#define THIEFITEM_MAXCOUNT 8

// =======================================================================
// Multiplayer safe function type defines
// =======================================================================

#define MSAFE_ROOM_TEXTURE MSAFE_ROOM + 0
#define MSAFE_ROOM_WIND MSAFE_ROOM + 1
#define MSAFE_ROOM_FOG MSAFE_ROOM + 2
#define MSAFE_ROOM_LIGHT_PULSE MSAFE_ROOM + 3
#define MSAFE_ROOM_PORTAL_RENDER MSAFE_ROOM + 4
#define MSAFE_ROOM_FOG_STATE MSAFE_ROOM + 5
#define MSAFE_ROOM_CHANGING_FOG MSAFE_ROOM + 6
#define MSAFE_ROOM_CHANGING_WIND MSAFE_ROOM + 7
#define MSAFE_ROOM_DAMAGE MSAFE_ROOM + 8
#define MSAFE_ROOM_LIGHT_FLICKER MSAFE_ROOM + 9
#define MSAFE_ROOM_LIGHT_STROBE MSAFE_ROOM + 10
#define MSAFE_ROOM_REFUEL MSAFE_ROOM + 11
#define MSAFE_ROOM_HAS_PLAYER MSAFE_ROOM + 12
#define MSAFE_ROOM_BREAK_GLASS MSAFE_ROOM + 13
#define MSAFE_ROOM_PORTAL_BLOCK MSAFE_ROOM + 14
#define MSAFE_ROOM_REMOVE_ALL_POWERUPS MSAFE_ROOM + 15

#define MSAFE_OBJECT_SHIELDS MSAFE_OBJ + 0
#define MSAFE_OBJECT_ENERGY MSAFE_OBJ + 1
#define MSAFE_OBJECT_BLANK MSAFE_OBJ + 2
#define MSAFE_OBJECT_LIGHT_COLOR MSAFE_OBJ + 3
#define MSAFE_OBJECT_MOVEMENT_SCALAR MSAFE_OBJ + 4
#define MSAFE_OBJECT_RECHARGE_SCALAR MSAFE_OBJ + 5
#define MSAFE_OBJECT_WSPEED_SCALAR MSAFE_OBJ + 6
#define MSAFE_OBJECT_DAMAGE_SCALAR MSAFE_OBJ + 7
#define MSAFE_OBJECT_ARMOR_SCALAR MSAFE_OBJ + 8
#define MSAFE_OBJECT_DAMAGE_OBJECT MSAFE_OBJ + 9
#define MSAFE_OBJECT_TYPE MSAFE_OBJ + 10
#define MSAFE_OBJECT_ID MSAFE_OBJ + 11
#define MSAFE_OBJECT_ADD_WEAPON MSAFE_OBJ + 12
#define MSAFE_OBJECT_START_SPEW MSAFE_OBJ + 13
#define MSAFE_OBJECT_STOP_SPEW MSAFE_OBJ + 14
#define MSAFE_OBJECT_GHOST MSAFE_OBJ + 15
#define MSAFE_OBJECT_UNGHOST MSAFE_OBJ + 16
#define MSAFE_OBJECT_REMOVE MSAFE_OBJ + 17
#define MSAFE_OBJECT_POS MSAFE_OBJ + 18
#define MSAFE_OBJECT_ORIENT MSAFE_OBJ + 19
#define MSAFE_OBJECT_ROOMNUM MSAFE_OBJ + 20
#define MSAFE_OBJECT_VELOCITY MSAFE_OBJ + 21
#define MSAFE_OBJECT_ROTVELOCITY MSAFE_OBJ + 22
#define MSAFE_OBJECT_THRUST MSAFE_OBJ + 23
#define MSAFE_OBJECT_ROTTHRUST MSAFE_OBJ + 24
#define MSAFE_OBJECT_FLAGS MSAFE_OBJ + 25
#define MSAFE_OBJECT_SIZE MSAFE_OBJ + 26
#define MSAFE_OBJECT_CONTROL_TYPE MSAFE_OBJ + 27
#define MSAFE_OBJECT_MOVEMENT_TYPE MSAFE_OBJ + 28
#define MSAFE_OBJECT_CREATION_TIME MSAFE_OBJ + 29
#define MSAFE_OBJECT_CLOAKALLPLAYERS MSAFE_OBJ + 30
#define MSAFE_OBJECT_PHYSICS_FLAGS MSAFE_OBJ + 31
#define MSAFE_OBJECT_PARENT MSAFE_OBJ + 32
#define MSAFE_OBJECT_ROTDRAG MSAFE_OBJ + 33
#define MSAFE_OBJECT_INVULNERABLE MSAFE_OBJ + 34
#define MSAFE_OBJECT_CLOAK MSAFE_OBJ + 35
#define MSAFE_OBJECT_PLAYER_HANDLE MSAFE_OBJ + 36
#define MSAFE_OBJECT_PLAYER_CMASK MSAFE_OBJ + 37
#define MSAFE_OBJECT_NO_RENDER MSAFE_OBJ + 38
#define MSAFE_OBJECT_LIGHT_DIST MSAFE_OBJ + 39
#define MSAFE_OBJECT_DEFORM MSAFE_OBJ + 40
#define MSAFE_OBJECT_VIEWER_SHAKE MSAFE_OBJ + 41 // or is this player shake?
#define MSAFE_OBJECT_PLAYER_KEY MSAFE_OBJ + 42
#define MSAFE_OBJECT_PLAYER_CONTROLAI MSAFE_OBJ + 43
#define MSAFE_OBJECT_WORLD_POSITION MSAFE_OBJ + 44 // room+pos+orient
#define MSAFE_OBJECT_COUNT_TYPE MSAFE_OBJ + 45
#define MSAFE_OBJECT_SPARKS MSAFE_OBJ + 46
#define MSAFE_OBJECT_SETONFIRE                                                                                         \
  MSAFE_OBJ + 47 // objhandle, longevity = how long, interval = damage per sec, ithandle = killer (or
                 // OBJECT_HANDLE_NONE)
#define MSAFE_OBJECT_SHAKE_AREA MSAFE_OBJ + 48
#define MSAFE_OBJECT_ENERGY_WEAPON MSAFE_OBJ + 49
#define MSAFE_OBJECT_DAMAGE_AMOUNT MSAFE_OBJ + 50
#define MSAFE_OBJECT_SHIELDS_ORIGINAL MSAFE_OBJ + 51
#define MSAFE_SHOW_ENABLED_CONTROLS MSAFE_OBJ + 52
#define MSAFE_OBJECT_FIRE_WEAPON MSAFE_OBJ + 53
#define MSAFE_OBJECT_DESTROY_ROBOTS_EXCEPT MSAFE_OBJ + 54

#define MSAFE_WEATHER_RAIN MSAFE_WEATHER + 0
#define MSAFE_WEATHER_SNOW MSAFE_WEATHER + 1
#define MSAFE_WEATHER_LIGHTNING MSAFE_WEATHER + 2
#define MSAFE_WEATHER_LIGHTNING_BOLT MSAFE_WEATHER + 3
#define MSAFE_WEATHER_LIGHTNING_BOLT_POS MSAFE_WEATHER + 4

#define MSAFE_SOUND_2D MSAFE_SOUND + 0
#define MSAFE_SOUND_OBJECT MSAFE_SOUND + 1
#define MSAFE_SOUND_STREAMING MSAFE_SOUND + 2
#define MSAFE_SOUND_STOP MSAFE_SOUND + 3
#define MSAFE_SOUND_STOP_OBJ MSAFE_SOUND + 4
#define MSAFE_SOUND_VOLUME_OBJ MSAFE_SOUND + 5

#define MSAFE_DOOR_LOCK_STATE MSAFE_DOOR + 0
#define MSAFE_DOOR_ACTIVATE MSAFE_DOOR + 1
#define MSAFE_DOOR_POSITION MSAFE_DOOR + 2
#define MSAFE_DOOR_STOP MSAFE_DOOR + 3
#define MSAFE_DOOR_OPENABLE MSAFE_DOOR + 4

#define MSAFE_TRIGGER_SET MSAFE_TRIGGER + 0

#define MSAFE_MISC_HUD_MESSAGE MSAFE_MISC + 0
#define MSAFE_MISC_WAYPOINT MSAFE_MISC + 1
#define MSAFE_MISC_END_LEVEL MSAFE_MISC + 2
#define MSAFE_MISC_POPUP_CAMERA MSAFE_MISC + 3
#define MSAFE_MISC_CLOSE_POPUP MSAFE_MISC + 4
#define MSAFE_MISC_GAME_MESSAGE MSAFE_MISC + 5
#define MSAFE_MUSIC_REGION MSAFE_MISC + 6
#define MSAFE_MISC_ENABLE_SHIP MSAFE_MISC + 7
#define MSAFE_MISC_LEVELGOAL MSAFE_MISC + 8
#define MSAFE_MISC_GUIDEBOT_NAME MSAFE_MISC + 9
#define MSAFE_MISC_FILTERED_HUD_MESSAGE MSAFE_MISC + 10
#define MSAFE_MISC_START_TIMER MSAFE_MISC + 11
#define MSAFE_MISC_ADD_HUD_ITEM MSAFE_MISC + 12
#define MSAFE_MISC_UPDATE_HUD_ITEM MSAFE_MISC + 13

#define MSAFE_INVEN_ADD_TYPE_ID MSAFE_INVEN + 0
#define MSAFE_INVEN_CHECK MSAFE_INVEN + 1
#define MSAFE_INVEN_COUNT MSAFE_INVEN + 2
#define MSAFE_INVEN_REMOVE MSAFE_INVEN + 3
#define MSAFE_INVEN_SIZE MSAFE_INVEN + 4
#define MSAFE_INVEN_GET_TYPE_ID MSAFE_INVEN + 5
#define MSAFE_INVEN_ADD_OBJECT MSAFE_INVEN + 6
#define MSAFE_INVEN_CHECK_OBJECT MSAFE_INVEN + 7
#define MSAFE_INVEN_REMOVE_OBJECT MSAFE_INVEN + 8

#define MSAFE_COUNTERMEASURE_ADD MSAFE_COUNTERMEASURE + 0
#define MSAFE_COUNTERMEASURE_COUNT MSAFE_COUNTERMEASURE + 1
#define MSAFE_COUNTERMEASURE_CHECK MSAFE_COUNTERMEASURE + 2
#define MSAFE_COUNTERMEASURE_REMOVE MSAFE_COUNTERMEASURE + 3
#define MSAFE_COUNTERMEASURE_SIZE MSAFE_COUNTERMEASURE + 4
#define MSAFE_COUNTERMEASURE_GET MSAFE_COUNTERMEASURE + 5

#define MSAFE_WEAPON_CHECK MSAFE_WEAPON + 0
#define MSAFE_WEAPON_ADD MSAFE_WEAPON + 1

// =======================================================================
// Structures shared between OSIRIS and the game
// =======================================================================

// Since this header is shared within D3, in order to keep redefinition errors
// quiet, the following section will only be included if this header file
// is included by a DLL
#ifndef INCLUDED_FROM_D3

// Use this handle when you want a handle that will never be a valid object
#ifndef INCLUDED_FROM_PLAYER_H
#define MAX_PLAYERS 32
#endif

// define unsigned types;
typedef uint8_t uint8_t;
typedef int8_t int8_t;
typedef uint16_t uint16_t;
typedef uint32_t uint32_t;
typedef uint32_t ddgr_color;

#ifndef NULL
#define NULL 0
#endif

inline ddgr_color GR_RGB(int r, int g, int b) { return ((r << 16) + (g << 8) + b); }

#endif // end #ifndef INCLUDED_FROM_D3

// =======================================================================
// GB Menu structure
// =======================================================================

#define MAX_GB_COMMANDS 60

#define GBCT_EXIT_IMMEDIATELY 0
#define GBCT_STRING_DIALOG 1

#define COM_GET_MENU 0
#define COM_DO_ACTION 1
#define COM_REINIT 2

struct gb_com {
  int8_t action;
  int8_t index;
  void *ptr;
};

struct gb_menu {
  char title[256];
  int32_t num_commands;
  char command_text[MAX_GB_COMMANDS][256];
  char command_id[MAX_GB_COMMANDS];
  char command_type[MAX_GB_COMMANDS];

  char dialog_text[MAX_GB_COMMANDS][256];
};

// =======================================================================
//  Osiris structures
// =======================================================================

struct tOSIRISModuleInit {
  int32_t *fp[MAX_MODULEFUNCS];
  char **string_table;
  int32_t string_count;
  int32_t module_identifier;

  bool module_is_static; // if this is set to true after initialization
                         // then the module will not unload if it's reference
                         // count is 0....only when the level ends.
                         // this is for Game modules ONLY.

  char *script_identifier; // Contains a unique script identifier (string), which
                           // can be used for the OMMS system. While this pointer
                           // will always be valid during the lifetime of the module
                           // DO NOT ALTER THE STRING IT POINTS TO.

  uint32_t game_checksum; // Checksum of game structures, if this doesn't match
                          // the checksum at compile time, it is very likely that
                          // bad things can happen, and this module shouldn't initialize.

}; // contains the necessary data to initialize

// an OSIRIS module
struct tOSIRISEVTINTERVAL {
  float frame_time;
  float game_time;
}; // struct for EVT_INTERVAL data

struct tOSIRISIEVTAIFRAME {
}; // struct for EVT_AI_FRAME data

struct tOSIRISEVTDAMAGED {
  float damage;
  int32_t it_handle;
  int32_t weapon_handle;
  int32_t damage_type;
}; // struct for EVT_DAMAGED data

struct tOSIRISEVTCOLLIDE {
  int32_t it_handle;
}; // struct for EVT_COLLIDE data

struct tOSIRISEVTCREATED {
}; // struct for EVT_CREATED data

struct tOSIRISEVTDESTROY {
  uint8_t is_dying;  // if this is !=0 than the event is coming because it is
                     // really being destroyed.  Else it is due to the level ending.
}; // struct for EVT_DESTROY data

struct tOSIRISEVTTIMER {
  int32_t id;
  float game_time;
}; // struct for EVT_TIMER data

struct tOSIRISEVTUSE {
  int32_t it_handle;
}; // struct for EVT_USE data

struct tOSIRISEVTSAVESTATE {
  void *fileptr;
}; // struct for EVT_SAVESTATE data

struct tOSIRISEVTRESTORESTATE {
  void *fileptr;
}; // struct for EVT_RESTORESTATE data

struct tOSIRISEVTAINOTIFY {
  int32_t notify_type;
  int32_t it_handle;
  int32_t goal_num;
  int32_t goal_uid;
  union {
    int32_t enabler_num;
    int32_t attack_num;
  };
}; // struct for EVT_AI_NOTIFY data

struct tOSIRISEVTAIINIT {
}; // struct for EVT_AI_INIT data

struct tOSIRISEVTLEVELSTART {
}; // struct for EVT_LEVELSTART data

struct tOSIRISEVTLEVELEND {
}; // struct for EVT_LEVELEND data

struct tOSIRISEVTCHANGESEG {
  int32_t room_num;
}; // struct for EVT_CHANGESEG data

struct tOSIRISEVTMEMRESTORE {
  int32_t id;
  void *memory_ptr;
}; // struct for EVT_MEMRESTORE data

struct tOSIRISEVTTIMERCANCEL {
  int32_t handle;
  uint8_t detonated;
}; // struct for EVT_TIMERCANCEL data

struct tOSIRISEVTCHILDDIED {
  int32_t it_handle;
};

struct tOSIRISEVTMATCENCREATE {
  int32_t it_handle;
  int32_t id;
}; // struct for EVT_MATCEN_CREATE data

struct tOSIRISEVTDOORACTIVATE {
}; // struct for EVT_DOOR_ACTIVATE data

struct tOSIRISEVTDOORCLOSE {
}; // struct for EVT_DOOR_CLOSE data

struct tOSIRISEVTLEVELGOALCOMPLETE {
  int32_t level_goal_index;
};

struct tOSIRISEVTLEVELGOALITEMCOMPLETE {
  int32_t level_goal_index;
};

struct tOSIRISEVTALLLEVELGOALSCOMPLETE {
};

struct tOSIRISEVTPLAYERMOVIESTART {
};

struct tOSIRISEVTPLAYERMOVIEEND {
};

struct tOSIRISEVTPLAYERRESPAWN {
  int32_t it_handle; // player respawning
};

struct tOSIRISEVTPLAYERDIES {
  int32_t it_handle; // player dead
};

struct tOSIRISEventInfo {
  union {
    tOSIRISEVTINTERVAL evt_interval;
    tOSIRISIEVTAIFRAME evt_ai_frame;
    tOSIRISEVTDAMAGED evt_damaged;
    tOSIRISEVTCOLLIDE evt_collide;
    tOSIRISEVTCREATED evt_created;
    tOSIRISEVTDESTROY evt_destroy;
    tOSIRISEVTTIMER evt_timer;
    tOSIRISEVTUSE evt_use;
    tOSIRISEVTAINOTIFY evt_ai_notify;
    tOSIRISEVTAINOTIFY evt_ain_objkilled;
    tOSIRISEVTAINOTIFY evt_ain_seeplayer;
    tOSIRISEVTAINOTIFY evt_ain_whitobject;
    tOSIRISEVTAINOTIFY evt_ain_goalcomplete;
    tOSIRISEVTAINOTIFY evt_ain_goalinvalid;
    tOSIRISEVTAINOTIFY evt_ain_goalfail;
    tOSIRISEVTAINOTIFY evt_ain_goalerror;
    tOSIRISEVTAINOTIFY evt_ain_goalforcedone;
    tOSIRISEVTAINOTIFY evt_ain_melee_hit;
    tOSIRISEVTAINOTIFY evt_ain_melee_attack_frame;
    tOSIRISEVTAINOTIFY evt_ain_movie_start;
    tOSIRISEVTAINOTIFY evt_ain_movie_end;
    tOSIRISEVTAIINIT evt_ai_init;
    tOSIRISEVTLEVELSTART evt_levelstart;
    tOSIRISEVTLEVELEND evt_levelend;
    tOSIRISEVTCHANGESEG evt_changeseg;
    tOSIRISEVTSAVESTATE evt_savestate;
    tOSIRISEVTRESTORESTATE evt_restorestate;
    tOSIRISEVTMEMRESTORE evt_memrestore;
    tOSIRISEVTTIMERCANCEL evt_timercancel;
    tOSIRISEVTMATCENCREATE evt_matcen_create;
    tOSIRISEVTDOORACTIVATE evt_door_activate;
    tOSIRISEVTDOORCLOSE evt_door_close;
    tOSIRISEVTCHILDDIED evt_child_died;
    tOSIRISEVTLEVELGOALCOMPLETE evt_level_goal_complete;
    tOSIRISEVTALLLEVELGOALSCOMPLETE evt_all_level_goals_complete;
    tOSIRISEVTLEVELGOALITEMCOMPLETE evt_level_goal_item_complete;
    tOSIRISEVTPLAYERMOVIESTART evt_player_movie_start;
    tOSIRISEVTPLAYERMOVIEEND evt_player_movie_end;
    tOSIRISEVTPLAYERRESPAWN evt_player_respawn;
    tOSIRISEVTPLAYERDIES evt_player_dies;
  };

  int32_t me_handle;
  void *extra_info;
}; // contains the necessary data for all events
                    // to pass what they need to their event handlers.

#define OTF_REPEATER 0x0001 // this timer is to repeat repeat_count times
#define OTF_TRIGGER 0x0002  // this timer is for a trigger, use trigger_number
#define OTF_LEVEL 0x0004    // this timer is for the level
#define OTF_CANCELONDEAD                                                                                               \
  0x008 // this timer should be auto-cancelled if object_handle_detonator dies, than the
        // timer auto-cancels
struct tOSIRISTIMER {
  uint16_t flags;
  int32_t id;           // an optional id you can use to store, will be passed back on EVT_TIMER signal
  int32_t repeat_count; // if OTF_REPEATER is set, this is how many times to repeat the signal (every
                        // interval).  -1 for infinite.
  union {
    int32_t object_handle;  // handle to the object to recieve the EVT_TIMER signal
    int32_t trigger_number; // trigger number to recieve the EVT_TIMER signal (OTF_TRIGGER)
  };

  int32_t object_handle_detonator; // if OTF_CANCELONDEAD is specified, this is the object handle to
                                   // use for checking, if this object dies, than auto-cancel the timer

  float timer_interval; // the timeframe in between EVT_TIMER signals (for non-repeaters, this is how
                        // long until the single EVT_TIMER gets signaled).
};

enum script_type { OBJECT_SCRIPT = 0, TRIGGER_SCRIPT, LEVEL_SCRIPT };

struct tOSIRISSCRIPTID {
  script_type type;
  union {
    int32_t objhandle;
    int32_t triggernum;
  };
};

struct tOSIRISMEMCHUNK {
  tOSIRISSCRIPTID my_id;
  uint16_t id;
  int32_t size;
};

// =======================================================================
// Multiplayer safe function structs
// =======================================================================

#define KOI_ID 0     // this item is specifying a type/id of object NOT to kill
#define KOI_HANDLE 1 // this item is specifying an object handle of an object NOT to kill
struct tKillObjectItem {
  uint8_t info_type;
  int32_t id;
  int32_t objhandle;
};

#define MSAFE_MESSAGE_LENGTH 255
#define MSAFE_NAME_LENGTH 32
struct msafe_struct {
  // Rooms
  int32_t roomnum;
  int16_t facenum;
  int16_t texnum;
  int16_t portalnum;
  float fog_r, fog_g, fog_b, fog_depth;
  vector wind;
  uint8_t pulse_time;
  uint8_t pulse_offset;

  // Objects/Players
  uint32_t objhandle;
  uint32_t ithandle;
  float shields, energy;
  int16_t start_tick;
  int16_t end_tick;
  float cycle_time;
  int32_t type, id, aux_type, aux_id;
  uint32_t checksum;
  int32_t path_id;
  float amount;
  uint8_t damage_type;
  uint32_t killer_handle;
  float ammo;
  uint8_t playsound;
  uint8_t remove;
  uint8_t do_powerup;
  vector velocity;
  vector rot_velocity;
  float rot_drag;
  vector thrust;
  vector rot_thrust;
  int8_t control_type;
  int8_t movement_type;
  float creation_time;
  int32_t physics_flags;
  vector pos;
  matrix orient;
  float anim_frame;

  // Spew
  uint8_t is_real;
  uint8_t random;
  uint8_t unused2; // was use_gunpoint
  int8_t gunpoint;
  uint8_t effect_type;
  uint8_t phys_info;
  float drag, mass;

  // Unused
  uint16_t unused; // was doorway_index

  // Weather
  int32_t randval;

  // Triggers
  uint8_t trigger_num;

  // sounds
  int32_t sound_handle;
  float volume;

  // Shared/misc
  int32_t index;
  float scalar;
  float interval;
  uint8_t state;
  int8_t slot;
  char message[MSAFE_MESSAGE_LENGTH];
  char name[MSAFE_NAME_LENGTH];
  int32_t color;
  float longevity;
  float lifetime;
  float size;
  float speed;
  int32_t count;
  int32_t flags;
  void *list;
  uint32_t control_mask;
  uint8_t control_val;

  // Object lighting stuff
  float light_distance;
  float r1, g1, b1; // light 1 color
  float r2, g2, b2; // light 2 color
  float time_interval;
  float flicker_distance;
  float directional_dot;
  int32_t timebits;

  vector pos2;

  // Second message
  char message2[MSAFE_MESSAGE_LENGTH];

};

struct ray_info {
  int32_t fate;
  int32_t hit_room;
  vector hit_point;

  int32_t hit_object;
  int32_t hit_subobject;

  int32_t hit_face;
  vector hit_face_pnt;
  int32_t hit_face_room;
  vector hit_wallnorm;
};

#define PV_POS 0x00000001        // set/get pos member (pass in an osiris_path_node_info ptr)
#define PV_ROOMNUM 0x00000002    // set/get roomnum member (pass in an osiris_path_node_info ptr)
#define PV_FLAGS 0x00000004      // set/get flags member (pass in an osiris_path_node_info ptr)
#define PV_FVEC 0x00000008       // set/get fvec member (pass in an osiris_path_node_info ptr)
#define PV_UVEC 0x00000010       // set/get uvec member (pass in an osiris_path_node_info ptr)
#define PV_ALL 0x0000001F        // set/get all members (above..pass in an osiris_path_node_info ptr)
#define PV_I_NUMNODES 0x00010000 // get number of nodes in this path
#define PV_CS_NAME 0x00020000    // get name of path
#define PV_I_NUMPATHS 0x00040000 // get number of paths (path_id & node_id are ignored in this case)

struct osiris_path_node_info {
  vector pos;      // where this node is in the world
  int32_t roomnum; // what room?
  int32_t flags;   // if this point lives over the terrain, etc
  vector fvec;
  vector uvec;
};

#endif
