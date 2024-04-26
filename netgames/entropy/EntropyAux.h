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

#ifndef __ENTROPY_AUX_H__
#define __ENTROPY_AUX_H__

/*
=====================================================================
Entropy Special Packet IDs
=====================================================================
*/
#define SPID_NEWPLAYER 0
#define SPID_TAKEOVER 1
#define SPID_MATCHOBJNUM 2
#define SPID_ROOMINFO 3
#define SPID_PICKUPVIRUS 4
#define SPID_VIRUSCREATE 5

/*
=====================================================================
Some general defines for Entropy
=====================================================================
*/
#define MAX_VIRII 16         // maximum number of virii for each team not picked up
#define MAX_VIRII_PER_ROOM 4 // maximum number of virii allowed to be alive in a lab at once
#define NUM_TEAMS 2          // Only 2 teams supported in this game

// the following are control message code (when the server sends status info to the client)
#define VIRUS_NOTENOUGHKILLS 0
#define VIRUS_CANTKILL 1
#define VIRUS_DESTROYED 2

/*
=====================================================================
Some common function prototypes
=====================================================================
*/
void SendRoomInfo(int pnum);
void ReceiveRoomInfo(ubyte *data);
void SendClientPickupVirus(int player_num);
void ReceivePickupVirus(ubyte *data);
void SendGameStartPacket(int pnum);
void GetGameStartPacket(ubyte *data);
void SendTakeOverPacket(int newteam, int oldteam, int room, int victor);
void ReceiveTakeOverPacket(ubyte *data);
void DoVirusCreate(ubyte *data);
void SendVirusCreate(object *obj);
bool IsPlayerInEnemy(int pnum);

// Functions in EntropyRoom.cpp
int SpewObjectInRoom(int type, int id, int roomnum);
bool PaintRoomWithTexture(int texture, int roomnum);

// Take over functions
void TakeOverRoom(int newteam, int oldteam, int room, int victor);
void SendTakeOverPacket(int newteam, int oldteam, int room, int victor);
void ReceiveTakeOverPacket(ubyte *data);

/*
=====================================================================
Externed globals needed throughout Entropy
=====================================================================
*/
extern int NumberOfKillsSinceLastDeath[DLLMAX_PLAYERS]; //'kills in a row' for a player
extern int TeamScore[NUM_TEAMS];                        // team scores
extern int TeamOwnedRooms[NUM_TEAMS];                   // Number of rooms each team has
extern int EnergyText[NUM_TEAMS];                       // texture handles to Energy room textures
extern int RepairText[NUM_TEAMS];                       // texture handles to Repair room textures
extern int LabText[NUM_TEAMS];                          // texture handles to Lab room textures
extern int virus_id;                                    // Object ID of the Virus object
extern int *RoomList; // List of room index's that are special for Entropy (i.e. all the labs, repairs and energy rooms)
extern int RoomCount; // Size of RoomList when it's finally allocated

#endif
