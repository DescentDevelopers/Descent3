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

 * $Logfile: /DescentIII/main/multi_external.h $
 * $Revision: 24 $
 * $Date: 9/01/99 4:12p $
 * $Author: Kevin $
 *
 * defines, structs, etc. that can be exported to DLLs
 *
 * $Log: /DescentIII/main/multi_external.h $
 *
 * 24    9/01/99 4:12p Kevin
 * Byte ordering fixes for the macintosh
 *
 * 23    7/08/99 5:45p Jason
 * added some extra netgame flags
 *
 * 22    7/06/99 5:52p Kevin
 * PXO & multiplayer fixes for the patch
 *
 * 21    5/23/99 3:05a Jason
 * fixed bug with player rankings not being updated correctly
 *
 * 20    5/20/99 2:52p Jason
 * made autowaypoints work in coop
 *
 * 19    5/10/99 10:22p Ardussi
 * changes to compile on Mac
 *
 * 18    5/10/99 12:11a Jason
 * added little endian/big endian safeness
 *
 * 17    5/09/99 1:34p Kevin
 * Added diffuculty level system to multiplayer
 *
 * 16    5/07/99 2:51p Jason
 * fixed a bunch of endlevel multiplayer issues
 *
 * 15    4/21/99 11:26a Jason
 * lowered max players
 *
 * 14    4/21/99 11:05a Kevin
 * new ps_rand and ps_srand to replace rand & srand
 *
 * 13    4/17/99 4:34p Jason
 * changes for robot/turret tracking
 *
 * 12    4/15/99 3:36p Kevin
 * Added mouselook UI stuff to the multiplayer options menu
 *
 * 11    4/14/99 3:56a Jeff
 * fixed case mismatch in #includes
 *
 * 10    3/25/99 3:29p Jason
 * added option to randomize powerup respawn points
 *
 * 9     3/22/99 6:22p Jeff
 * added 2 more audio taunts.  a mulitplayer event when someone plays an
 * audio taunt.  option to disable audio taunts.
 *
 * 8     3/17/99 4:08p Kevin
 * Changed the way games appear and timeout in the game list.
 *
 * 7     3/01/99 4:17p Jeff
 * added net flag whether buddy bot allowed
 *
 * 6     2/19/99 5:21p Kevin
 * Fixed some connection DLLs and a Direct Sound bug with threads.
 *
 * 5     2/03/99 4:26p Jason
 * made multiplayer coop actually work!
 *
 * 4     2/03/99 4:20p Kevin
 * Got multiplayer working with .mn3 files, and setup autodownloading
 *
 * 3     2/02/99 7:06p Jason
 * added ranking system
 *
 * 2     1/21/99 11:15p Jeff
 * pulled out some structs and defines from header files and moved them
 * into seperate header files so that multiplayer dlls don't require major
 * game headers, just those new headers.  Side effect is a shorter build
 * time.  Also cleaned up some header file #includes that weren't needed.
 * This affected polymodel.h, object.h, player.h, vecmat.h, room.h,
 * manage.h and multi.h
 *
 * $NoKeywords: $
 */

#ifndef __MULTI_EXTERNAL_H_
#define __MULTI_EXTERNAL_H_

#if defined(__LINUX__)
#include <cstring>
#include <cstdint>
typedef uintptr_t DWORD;
typedef int HANDLE;
#endif

#include "pstypes.h"
#include "manage_external.h"
#include "cfile.h"
#include "networking.h"
#include "descent.h" //for MSN_NAMELEN
#include "byteswap.h"
#include <cassert>

#define NETGAME_NAME_LEN 32
#define NETGAME_SCRIPT_LEN 32

#define LR_CLIENT 0
#define LR_SERVER 1

// Max number of network players
#define MAX_NET_PLAYERS 32

#define MAX_GAME_DATA_SIZE (MAX_PACKET_SIZE - 4)

// Stuff for the connection DLLs
#define MT_EVT_LOGIN 1
#define MT_EVT_FIRST_FRAME 2
#define MT_EVT_FRAME 3
#define MT_EVT_GAME_OVER 4
#define MT_EVT_GET_HELP 5
#define MT_AUTO_LOGIN 6
#define MT_AUTO_START 7
#define MT_RETURN_TO_GAME_LIST 8

// Net sequencing

#define NETSEQ_PREGAME 0           // Not in a game
#define NETSEQ_WAITING_FOR_LEVEL 1 // Client - waiting for level name and checksum
#define NETSEQ_LEVEL_START 2       // Waiting for level object stuff
#define NETSEQ_NEED_GAMETIME 3     // Need to ask for gametime
#define NETSEQ_WAIT_GAMETIME 4     // Waiting for server response in gametime
#define NETSEQ_REQUEST_PLAYERS 5   // Requesting players
#define NETSEQ_PLAYERS 6           // Getting player stuff
#define NETSEQ_REQUEST_BUILDINGS 7 // Requesting buildings
#define NETSEQ_BUILDINGS 8         // Getting buildings
#define NETSEQ_REQUEST_OBJECTS 9   // Getting objects
#define NETSEQ_OBJECTS 10          // Got objects
#define NETSEQ_REQUEST_WORLD 11    // Getting world states
#define NETSEQ_WORLD 12            // Got World
#define NETSEQ_PLAYING 13          // Playing the game
#define NETSEQ_LEVEL_END 14        // At the post-level stage

// Netplayer flags
#define NPF_CONNECTED 1          // This player is connected
#define NPF_SERVER 2             // This player is the server
#define NPF_MASTER_TRACKER 4     // This is a master tracker game
#define NPF_MT_READING_PILOT 8   // Waiting for a response from the mastertracker about this pilot's stats
#define NPF_MT_WRITING_PILOT 16  // Waiting to finish updating the mastertracker with this pilot's stats
#define NPF_MT_HAS_PILOT_DATA 32 // We got data from the mastertracker
#define NPF_WROTE_RANK 64        // We told the clients about this clients rank

typedef struct {
  network_address addr;
  int flags;
  SOCKET reliable_socket;
  float last_packet_time;
  float packet_time; // for making sure we don't get position packets out of order
  unsigned int total_bytes_sent;
  unsigned int total_bytes_rcvd;
  unsigned int secret_net_id;       //	We use this to determine who we are getting packets from
  int file_xfer_flags;              // Are we sending,receiving, or neither
  unsigned int file_xfer_total_len; // Total length of the file we are receiving
  unsigned int file_xfer_pos;       // Position for sending and/or receiving
  unsigned int file_xfer_id;        // File id that we are sending
  unsigned int file_xfer_who;       // Who the file is for
  CFILE *file_xfer_cfile;           // File handle for src/target file
  ushort position_counter;          // for making sure we don't get position packets out of order
  char ship_logo[_MAX_PATH];
  char voice_taunt1[_MAX_PATH];
  char voice_taunt2[_MAX_PATH];
  char voice_taunt3[_MAX_PATH];
  char voice_taunt4[_MAX_PATH];
  ubyte custom_file_seq;
  ubyte sequence; // where we are in the sequence chain
  ubyte pps;
  HANDLE hPlayerEvent;      // player event to use for directplay
  DWORD dpidPlayer; // directplay ID of player created
  float ping_time;
  float last_ping_time;
  ushort pilot_pic_id;
  float percent_loss;
  unsigned char digest[16];
} netplayer;

#define MISSION_NAME_LEN 50

typedef struct {
  network_address addr;
  char name[NETGAME_NAME_LEN];
  char mission[MSN_NAMELEN];
  char mission_name[MISSION_NAME_LEN];
  char scriptname[NETGAME_SCRIPT_LEN];
  ushort level_num;
  ushort curr_num_players;
  ushort max_num_players;
  float server_response_time;
  unsigned int flags;
  float last_update;
  bool dedicated_server;
  ubyte difficulty; // Game difficulty level
  unsigned int handle;
} network_game;

// netgame flags
#define NF_TIMER 0x01               // This level will end when the timer runs out
#define NF_KILLGOAL 0x02            // This level will end when the number of kills reaches a certain point
#define NF_USE_ROBOTS 0x04          // This game uses robots
#define NF_EXIT_NOW 0x08            // This game needs to bail right now
#define NF_PEER_PEER 0x10           // This game is a psuedo peer-peer game, so send position packets to all clients.
#define NF_SENDROTVEL 0x20          // Use low resolution packets
#define NF_ALLOWGUIDEBOT 0x40       // Whether the Guide bot is allowed in the game
#define NF_DIRECTPLAY 0x80          //	This game is a directplay game
#define NF_ATTACK_FRIENDLY 0x100    // Homers and gunboys will attack friendlies
#define NF_DAMAGE_FRIENDLY 0x200    // Friendly fire will cause damage
#define NF_USE_ACC_WEAP 0x400       // Use big weapon spheres against player ships.
#define NF_USE_SMOOTHING 0x800      // Smooth out positional movement via curve interpolation
#define NF_BRIGHT_PLAYERS 0x1000    // Bright players in netgame
#define NF_PERMISSABLE 0x2000       // Clients need server permission to fire
#define NF_RESPAWN_WAYPOINT 0x4000  // Players should use waypoints to respawn
#define NF_RANDOMIZE_RESPAWN 0x8000 // Powerups should move around randomly when respawning
#define NF_ALLOW_MLOOK 0x10000      // Allow mouse lookers
#define NF_TRACK_RANK 0x20000       // Track rankings for PXO
#define NF_COOP 0x40000             // This game is a cooperative game

typedef struct {
  ushort server_version; // This is so client and server code matches
  char name[NETGAME_NAME_LEN];
  char mission[MSN_NAMELEN];
  char mission_name[MISSION_NAME_LEN];
  char scriptname[NETGAME_SCRIPT_LEN];
  char server_config_name[PAGENAME_LEN];
  char connection_name[PAGENAME_LEN];
  network_address server_address; // The address of the server that we're talking to - not used if we are the server

  ubyte local_role;
  ubyte server_sequence;
  float last_server_time;   // last time we got a packet from the server
  ubyte packets_per_second; // how many packets per second we'll send out
  int flags;
  int timelimit; // how many minutes to play this level
  int killgoal;  // kill goal for this level
  int respawn_time;
  int max_players;
  ubyte difficulty; // Game difficulty level
  u_char digest[16];
} netgame_info;

// Inline functions for extracting/packing multiplayer data
inline void MultiAddUbyte(ubyte element, ubyte *data, int *count) {
  data[*count] = element;
  *count += sizeof(ubyte);
}

inline void MultiAddByte(ubyte element, ubyte *data, int *count) {
  data[*count] = element;
  *count += sizeof(ubyte);
}

inline void MultiAddSbyte(sbyte element, ubyte *data, int *count) {
  data[*count] = element;
  *count += sizeof(sbyte);
}

inline void MultiAddShort(short element, ubyte *data, int *count) {
  *(short *)(data + *count) = INTEL_SHORT(element);
  *count += sizeof(short);
}

inline void MultiAddUshort(ushort element, ubyte *data, int *count) {
  *(ushort *)(data + *count) = INTEL_SHORT(element);
  *count += sizeof(ushort);
}

inline void MultiAddInt(int element, ubyte *data, int *count) {
  *(int *)(data + *count) = INTEL_INT(element);
  *count += sizeof(int);
}

inline void MultiAddUint(uint element, ubyte *data, int *count) {
  *(uint *)(data + *count) = INTEL_INT(element);
  *count += sizeof(uint);
}

inline void MultiAddFloat(float element, ubyte *data, int *count) {
  *(float *)(data + *count) = INTEL_FLOAT(element);
  *count += sizeof(float);
}

inline void MultiAddString(const char *str, ubyte *data, int *count) {
  size_t len = strlen(str) + 1;
  assert(len <= 0xFF);

  MultiAddByte((ubyte)len, data, count);
  memcpy(&data[*count], str, len);
  *count += len;
}

inline ubyte MultiGetUbyte(ubyte *data, int *count) {
  ubyte element = (*(ubyte *)(data + *count));
  (*count) += sizeof(ubyte);
  return element;
}

inline ubyte MultiGetByte(ubyte *data, int *count) {
  ubyte element = (*(ubyte *)(data + *count));
  (*count) += sizeof(ubyte);
  return element;
}

inline sbyte MultiGetSbyte(ubyte *data, int *count) {
  sbyte element = (*(sbyte *)(data + *count));
  (*count) += sizeof(sbyte);
  return element;
}

inline short MultiGetShort(ubyte *data, int *count) {
  short element = (*(short *)(data + *count));
  *count += sizeof(short);
  return INTEL_SHORT(element);
}

inline ushort MultiGetUshort(ubyte *data, int *count) {
  ushort element = (*(ushort *)(data + *count));
  *count += sizeof(short);
  return INTEL_SHORT(element);
}

inline int MultiGetInt(ubyte *data, int *count) {
  int element = (*(int *)(data + *count));
  *count += sizeof(int);
  return INTEL_INT(element);
}

inline uint MultiGetUint(ubyte *data, int *count) {
  uint element = (*(uint *)(data + *count));
  *count += sizeof(int);
  return INTEL_INT(element);
}

inline float MultiGetFloat(ubyte *data, int *count) {
  float element = (*(float *)(data + *count));
  *count += sizeof(float);
  return INTEL_FLOAT(element);
}

inline void MultiGetString(char *str, ubyte *data, int *count) {
  ubyte len = MultiGetByte(data, count);
  memcpy(str, &data[*count], len);
  *count += len;
}

inline void MultiAddVector(vector v, ubyte *data, int *count) {
  MultiAddFloat(v.x, data, count);
  MultiAddFloat(v.y, data, count);
  MultiAddFloat(v.z, data, count);
}

inline vector MultiGetVector(ubyte *data, int *count) {
  vector v;

  v.x = MultiGetFloat(data, count);
  v.y = MultiGetFloat(data, count);
  v.z = MultiGetFloat(data, count);
  return v;
}
#endif
