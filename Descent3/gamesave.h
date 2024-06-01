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

 * $Logfile: /DescentIII/main/gamesave.h $
 * $Revision: 16 $
 * $Date: 7/21/99 7:17p $
 * $Author: Chris $
 *
 *
 *
 * $Log: /DescentIII/main/gamesave.h $
 *
 * 16    7/21/99 7:17p Chris
 * Fixed an attach related crash in the load/save game code
 *
 * 15    4/20/99 11:45a Samir
 * added autosave
 *
 * 14    4/14/99 3:56a Jeff
 * fixed case mismatch in #includes
 *
 * 13    3/22/99 5:12p Samir
 * added snapshot to savegame.
 *
 * 12    3/08/99 11:04a Jason
 * added versioning for texture changes
 *
 * 11    3/04/99 6:30p Jeff
 * saves out current waypoint and matcens
 *
 * 10    1/29/99 12:47p Matt
 * Rewrote the doorway system
 *
 * 9     1/21/99 11:15p Jeff
 * pulled out some structs and defines from header files and moved them
 * into seperate header files so that multiplayer dlls don't require major
 * game headers, just those new headers.  Side effect is a shorter build
 * time.  Also cleaned up some header file #includes that weren't needed.
 * This affected polymodel.h, object.h, player.h, vecmat.h, room.h,
 * manage.h and multi.h
 *
 * 8     1/08/99 2:55p Samir
 * Ripped out OSIRIS1.
 *
 * 7     10/08/98 12:00p Kevin
 * Demo system work
 *
 * 6     10/06/98 5:45p Kevin
 * Added new configuration for demo
 *
 * 5     8/20/98 10:52a Samir
 * fixed some ui probs
 *
 * 4     8/19/98 5:35p Samir
 * saved triggers, spew, viseffects, weapons, room data xcept texture per
 * face changes, one real strange bug where robots in the same room as you
 * don't appear but fire at you.
 *
 * 3     8/18/98 3:05p Samir
 * save game with no weapons or fireballs.
 *
 * 2     8/18/98 1:12a Samir
 * rudimentary savegame.  pretty damn buggy when loading.
 *
 * $NoKeywords: $
 */

#ifndef GAMESAVE_H
#define GAMESAVE_H

#include "pstypes.h"
#include "cfile.h"
#include "object.h"
#include "objinfo.h"

#include "gametexture.h"
#include "bitmap.h"
#include "ddio.h"
#include "door.h"
#include "doorway.h"
#include "ship.h"
#include "weapon.h"
#include "polymodel.h"

#define GAMESAVE_SLOTS 8    // maximum number of savegames
#define GAMESAVE_DESCLEN 31 // gamesave description maximum length.

typedef struct gs_tables {
  int16_t model_handles[MAX_POLY_MODELS];
  int16_t obji_indices[MAX_OBJECT_IDS];
  int16_t bm_handles[MAX_BITMAPS];
  int16_t tex_handles[MAX_TEXTURES];
  int16_t door_handles[MAX_DOORS];
  int16_t ship_handles[MAX_SHIPS];
  int16_t wpn_handles[MAX_WEAPONS];
} gs_tables;

//	savegame version info.
// this should be handled like level file version, as to prevent invalidating old savegames
//	please record a table of changes here.

//	0	samir-initial version
//	1	Added saving/loading of changed textures
// 2  Added correct saving and restoring of attach points

#define GAMESAVE_VERSION 2
#define GAMESAVE_OLDVER 0 // any version before this value is obsolete.

void SaveGameDialog();
bool LoadGameDialog(); // returns true if ok, false if canceled.
void QuickSaveGame();

bool LoadCurrentSaveGame(); // loads savegame as specified from LoadGameDialog (false fails)

extern int Quicksave_game_slot; // externed so gamesequencing can reset this value starting new game.

// internal.

//	loads a game from a given slot.  returns a code below.
#define LGS_OK 0
#define LGS_FILENOTFOUND 1
#define LGS_OUTDATEDVER 2
#define LGS_STARTLVLFAILED 3 // level failed to load or startup
#define LGS_MISSIONFAILED 4  // mission failed to load.
#define LGS_OBJECTSCORRUPT 5 // object list is corrupt (or out of date with level)
#define LGS_CORRUPTLEVEL 6   // either level is out of date, or list is corrupted.

int LoadGameState(const char *pathname);

//	Easy IO routines for repetitive tasks.

#define gs_WriteVector(_f, _v)                                                                                         \
  do {                                                                                                                 \
    cf_WriteFloat((_f), (_v).x);                                                                                       \
    cf_WriteFloat((_f), (_v).y);                                                                                       \
    cf_WriteFloat((_f), (_v).z);                                                                                       \
  } while (0)
#define gs_WriteMatrix(_f, _m)                                                                                         \
  do {                                                                                                                 \
    gs_WriteVector((_f), (_m).rvec);                                                                                   \
    gs_WriteVector((_f), (_m).uvec);                                                                                   \
    gs_WriteVector((_f), (_m).fvec);                                                                                   \
  } while (0)
#define gs_WriteAngle(_f, _a) cf_WriteShort(_f, (int16_t)(_a))
#define gs_WriteByte(_f, _b) cf_WriteByte(_f, _b)
#define gs_WriteShort(_f, _s) cf_WriteShort(_f, _s)
#define gs_WriteInt(_f, _i) cf_WriteInt(_f, _i)
#define gs_WriteFloat(_f, _fl) cf_WriteFloat(_f, _fl)
#define gs_WriteBool(_f, _b) cf_WriteByte(_f, _b)

#define START_VERIFY_SAVEFILE(f) int cur_file_pos = cftell(f)
#define END_VERIFY_SAVEFILE(f, s) mprintf(0, "%s =%d bytes\n", s, cftell(f) - cur_file_pos)

#define gs_ReadVector(_f, _v)                                                                                          \
  do {                                                                                                                 \
    (_v).x = cf_ReadFloat(_f);                                                                                         \
    (_v).y = cf_ReadFloat(_f);                                                                                         \
    (_v).z = cf_ReadFloat(_f);                                                                                         \
  } while (0)
#define gs_ReadMatrix(_f, _m)                                                                                          \
  do {                                                                                                                 \
    gs_ReadVector((_f), (_m).rvec);                                                                                    \
    gs_ReadVector((_f), (_m).uvec);                                                                                    \
    gs_ReadVector((_f), (_m).fvec);                                                                                    \
  } while (0)
#define gs_ReadAngle(_f, _a) ((_a) = (angle)cf_ReadShort(_f))
#define gs_ReadBool(_f, _b) ((_b) = (bool)cf_ReadByte(_f))
#define gs_ReadByte(_f, _b) ((_b) = cf_ReadByte(_f))
#define gs_ReadShort(_f, _s) ((_s) = cf_ReadShort(_f))
#define gs_ReadInt(_f, _i) ((_i) = cf_ReadInt(_f))
#define gs_ReadFloat(_f, _fl) ((_fl) = cf_ReadFloat(_f))

//	writes out translation tables.
void SGSXlateTables(CFILE *fp);

//	initializes rooms
void SGSRooms(CFILE *fp);

//	saves out events
void SGSEvents(CFILE *fp);

//	saves out triggers
void SGSTriggers(CFILE *fp);

// save viseffects
void SGSVisEffects(CFILE *fp);

// players
void SGSPlayers(CFILE *fp);

//	saves out objects
void SGSObjects(CFILE *fp);

//	saves ai
void SGSObjAI(CFILE *fp, const ai_frame *ai);

//	saves fx
void SGSObjEffects(CFILE *fp, const object *op);

//	saves script
//@@void SGSScript(CFILE *fp, const script_info *script);

//	saves wb
void SGSObjWB(CFILE *fp, object *op, int num_wbs);

// saves special object info
void SGSObjSpecial(CFILE *fp, const object *op);

// load spew
void SGSSpew(CFILE *fp);

//@@// saves ai goals
//@@void SGSObjAIGoal(CFILE *fp, const goal *g);

// load matcens
void SGSMatcens(CFILE *fp);

//	give a description and slot number (0 to GAMESAVE_SLOTS-1)
bool SaveGameState(const char *pathname, const char *description);

//	retreive gamesave file header info. description must be a buffer of length GAMESAVE_DESCLEN+1
// returns true if it's a valid savegame file.  false if corrupted somehow
// pointer to bm_handle will return a bitmap handle to the snapshot for game. (*bm_handle) can be invalid.
bool GetGameStateInfo(const char *pathname, char *description, int *bm_handle = NULL);

///////////////////////////////////////////////////////////////////////////////
//	reads in translation tables
int LGSXlateTables(CFILE *fp);

//	loads in level's mission and level.
int LGSMission(const char *msnname, int level);

//	initializes rooms
int LGSRooms(CFILE *fp);

//	loads in and sets these events
int LGSEvents(CFILE *fp);

//	loads in and sets these triggers
int LGSTriggers(CFILE *fp);

// players
int LGSPlayers(CFILE *fp);

//	loads in and sets these objects
int LGSObjects(CFILE *fp, int version);

// load viseffects
int LGSVisEffects(CFILE *fp);

// load spew
int LGSSpew(CFILE *fp);

//	loads ai
int LGSObjAI(CFILE *fp, ai_frame **pai);

// saves ai goals
int LGSObjAIGoal(CFILE *fp, goal *g);

//	loads fx
int LGSObjEffects(CFILE *fp, object *op);

//	loads weapon batter info
int LGSObjWB(CFILE *fp, object *op);

//	loads script
//@@vector *LGSScript(CFILE *fp, script_info *script, uint8_t *is_scripted, int *memsize);

// loads special object info
int LGSObjSpecial(CFILE *fp, object *op);

// load matcens
int LGSMatcens(CFILE *fp);

//////////////////////////////////////////////////////////////////////////////
//	Game Save/Load User interface

#define SAVEGAMEDLG_WIDTH (512)
#define SAVEGAMEDLG_HEIGHT (384)
#define SAVEGAMEDLG_X ((Max_window_w - SAVEGAMEDLG_WIDTH) / 2)
#define SAVEGAMEDLG_Y ((Max_window_h - SAVEGAMEDLG_HEIGHT) / 2)
#define SAVEGAMELB_WIDTH (32 * 14)
#define SAVEGAMELB_HEIGHT (32 * 6)
#define SAVEGAMELB_X ((SAVEGAMEDLG_WIDTH - SAVEGAMELB_WIDTH) / 2)
#define SAVEGAMELB_Y ((SAVEGAMEDLG_HEIGHT - SAVEGAMELB_HEIGHT + 32) / 2)
#define SAVEGAMEBTN_W 96
#define SAVEGAMEBTN_X ((SAVEGAMEDLG_WIDTH / 4) - (SAVEGAMEBTN_W / 2))
#define SAVEGAMEBTN_X2 ((3 * SAVEGAMEDLG_WIDTH / 4) - (SAVEGAMEBTN_W / 2))
#define SAVEGAMEBTN_Y (SAVEGAMEDLG_HEIGHT - 64)

#define UID_SAVELB 100
#define UID_LOADLB 100

#endif
