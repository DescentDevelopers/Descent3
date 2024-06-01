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

#ifndef GAME2DLL_H
#define GAME2DLL_H

#include "pstypes.h"
#include "multi.h"

#include "d3events.h"
#include "vecmat.h"

struct tPreJoinData {
  bool is_banned;
  int team;
};

struct game_collide_info {
  vector point, normal;
  float hitspeed, hit_dot;
  int hitseg, hitwall;
};

struct dllinfo {
  int me_handle;
  int it_handle;
  uint8_t *special_data;
  char *input_string;
  int input_key;
  union {
    int iRet;
    float fRet;
  };
  float fParam;
  ptrdiff_t iParam;
  game_collide_info collide_info;
  int newseg, oldseg;
};

#define MAX_GAMENAME_LEN 32
#define MAX_REQUIREMENT_LEN 384
#define DOF_MAXTEAMS 0x0001 // max_teams member is valid
#define DOF_MINTEAMS 0x0002
// this struct is used to return game specific information to Descent 3
struct tDLLOptions {
  // general flags, also specifies what members of the structure are valid
  int flags;

  // 0 or 1 for non-team games...maximum value is 4.  If not specified, than it is assumed 0
  int max_teams;
  // must be less then or equal to max_teams.  If not specified, then it is assumed 0 for
  // non-team games, 2 for team games. (max_teams will always tell you if it's a team game)
  int min_teams;

  // gives the full name of the game (must be set)
  char game_name[MAX_GAMENAME_LEN];

  // this is an array of semicolor seperated 'string identifiers' that serve as requirements
  // the mission needs to support in order for it to be playable with the game.
  // this parameter must be set (even it is just a '\0' for the first character..aka no requirements)
  char requirements[MAX_REQUIREMENT_LEN];

};

extern dllinfo DLLInfo;

// The chokepoint function to call the dll function
void CallGameDLL(int eventnum, dllinfo *data);

// Frees the dll if its in memory
void FreeGameDLL();

// Loads the game dll.  Returns 1 on success, else 0 on failure
int LoadGameDLL(const char *name, int num_teams_to_use = -1);

// If this function is called than the DLL is to be closed, because there was an error running it
// if reason is not NULL than that is the reason why
void DLLFatalError(const char *reason = NULL);

// Call this function right after a player connects to the game to see if a player is banned
bool GameDLLIsAddressBanned(network_address *addr, const char *tracker_id);

// Call this function to get information/options from a unloaded mod
bool GetDLLGameInfo(const char *name, tDLLOptions *options);

// Call this function to get information about the number of teams for the game
// Returns true if it's a team game...false if it's a non-team game.
// If it returns true, then min is filled in with the minumum number of teams needed for the game
// and max is filled in with the maximum number of teams for the game...if they are the same
// value, then it is the only number of teams supported.
bool GetDLLNumTeamInfo(const char *name, int *mint, int *maxt);

// Call this function to get the list of requirements that the given module needs in order
// to be playable.  Returns the number of requirements it needs...-1 on error.
int GetDLLRequirements(const char *name, char *requirements, int buflen);

#endif
