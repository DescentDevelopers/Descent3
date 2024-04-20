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

#ifndef MULTI_SERVER_H

#include "pstypes.h"
#include "multi.h"
#include "game2dll.h"
// #include "gametrack.h"

extern int Join_response_strings[];

// extern d3_net_game_data D3_tracker_info;

#define MAX_CHANGED_OBJECTS 700

extern int Changed_anim[MAX_CHANGED_OBJECTS][MAX_NET_PLAYERS];
extern unsigned short Num_changed_anim[MAX_NET_PLAYERS];

extern int Changed_wb_anim[MAX_CHANGED_OBJECTS][MAX_NET_PLAYERS];
extern unsigned short Num_changed_wb_anim[MAX_NET_PLAYERS];

extern int Changed_turret[MAX_CHANGED_OBJECTS][MAX_NET_PLAYERS];
extern unsigned short Num_changed_turret[MAX_NET_PLAYERS];

extern unsigned int Secret_net_id;
#define JOIN_ANSWER_OK 0
#define JOIN_ANSWER_NOT_SERVER 1
#define JOIN_ANSWER_REJECTED 2
#define JOIN_ANSWER_NO_ROOM 3
#define JOIN_ANSWER_FULL 4

// Does whatever the server needs to do for this frame
void MultiDoServerFrame();

// Set the local machine as a server
void MultiStartServer(int playing, char *scriptname, int dedicated_server_num_teams = -1);

// Disconnects all players that haven't been heard from in a while
// Server only
void MultiDisconnectDeadPlayers();

// Disconnect a player for whatever reason
// Server only
void MultiDisconnectPlayer(int slot);

// Sends existing players to a joining player
// Server only
void MultiSendPlayer(int slot, int which);

// Sends this reliable packet to everyone except the server and the named slot
void MultiSendReliablyToAllExcept(int except, ubyte *data, int size, int seq_threshold = 0, bool urgent = 1);

// Tells clients to execute dlls on their machines
void MultiSendClientExecuteDLL(int eventnum, int me_objnum, int it_objnum, int to, dllinfo *info = NULL);

// Resets the settings that a server uses
void MultiResetSettings();

// Given a killer and killed player, calculates their new rankings
void GetNewRankings(object *killed, object *killer);

// Returns a ranking index based on the player rating
// If rankbuf is non-null, fills in the string corresponding to that rank
// Returns -1 if not a pxo game (ie no rankings in this game)
int GetRankIndex(int pnum, char *rankbuf = NULL);

#endif