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

/*
 * $Logfile: /DescentIII/Main/dmfc/dmfcpackets.cpp $
 * $Revision: 1.1.1.1 $
 * $Date: 2003/08/26 03:57:21 $
 * $Author: kevinb $
 *
 * DMFC Special packet handling functions
 *
 * $Log: dmfcpackets.cpp,v $
 * Revision 1.1.1.1  2003/08/26 03:57:21  kevinb
 * initial 1.5 import
 *
 *
 * 29    8/21/99 12:32a Jeff
 * Changed the name of the GetRealGametime function to
 * GetRealGametimePacket since it was confusing some compilers with the
 * other GetRealGametime function.
 *
 * 28    7/09/99 2:54p Jeff
 * handle gametime better (pause it when needed) if the server is 'waiting
 * for players'
 *
 * 27    7/08/99 6:25p Jeff
 * remote admin in and working
 *
 * 26    7/08/99 2:39a Jeff
 * rough implementation of remote administration checked in.  Still needs
 * some polishing, but should work basically.
 *
 * 25    5/13/99 4:55p Ardussi
 * changes for compiling on the Mac
 *
 * 24    5/12/99 11:05p Jeff
 * dmfc and multiplayer games now have endian friendly packets (*whew*)
 *
 * 23    4/07/99 9:24p Jeff
 * fixed bug in sending team names over
 *
 * 22    3/17/99 12:24p Jeff
 * converted DMFC to be COM interface
 *
 * 21    2/11/99 12:51a Jeff
 * changed names of exported variables
 *
 * 20    12/08/98 4:47p Jeff
 * umm, various changes, fixed pilot pics so that they work for everyone
 * now
 *
 * 19    12/08/98 12:17p Jeff
 * various changes that include an improved Team Control dialog (doesn't
 * switch teams until exit..) and spew/respawn players that change teams
 *
 * 18    11/17/98 12:36p Jeff
 * fixed bug in sending new team name to clients
 *
 * 17    11/16/98 5:35p Jeff
 * removed log functions, added support for changing team names, fixed ctf
 * to work better with different team names
 *
 * 16    11/11/98 7:19p Jeff
 * changes made so that a dedicated server's team is always -1 (team game
 * or not)
 *
 * 15    10/29/98 7:01p Jeff
 * creation of team placement dialog.  Moved TranslateEvent into DMFC
 *
 * 14    10/23/98 11:22a Jeff
 * changes to handle mixcase, and display the client netgame info
 * correctly
 *
 * 13    10/20/98 12:16p Jeff
 * added death message filter, hud callsign filter
 *
 * 12    10/08/98 3:37p Jeff
 * general improvements (Netgame info things, save to registry).  Changes
 * so it would send packets on NETSEQ_OBJECTS
 *
 * 11    9/30/98 4:21p Jeff
 * team changing is handled correctly
 *
 * 10    9/25/98 8:02p Jason
 * added checks for jeff
 *
 * 9     9/21/98 7:11p Jeff
 * made InputCommand interface API and moved existing input commands to
 * the interface.  Changed mprintf/ASSERT so they are valid in DMFC
 *
 * $NoKeywords: $
 */

#include "gamedll_header.h"
#include "DMFC.h"
#include "dmfcinternal.h"


extern char **DMFCStringTable;
extern int DMFCStringTableSize;
extern const char *_DMFCErrorString;
extern DMFCBase *basethis;
const char *DMFCGetString(int d);

// DMFCBase::RegisterPacketReceiver
//
// Sets up a handler for a Special Packet ID.  When a special packet is recieved it's ID is compared
// to the ID's given to this function.  If any match than it calls the handler given to process
// the packet.
// id = ID of the packet
// func = Function handler to handle the packet.  Must be declared like void MyFunction(ubyte *data);
void DMFCBase::RegisterPacketReceiver(ubyte id, void (*func)(ubyte *)) {
  // go to the last handler
  tSPHandler *current, *last;

  if (!SPRoot) {
    // handle special case for first node being added
    SPRoot = (tSPHandler *)malloc(sizeof(tSPHandler));
    if (!SPRoot)
      return;
    current = SPRoot;
  } else {
    last = current = SPRoot;
    while (current) {
      last = current;
      current = current->next;
    }

    // allocate memory for the node
    current = (tSPHandler *)malloc(sizeof(tSPHandler));
    if (!current)
      return;
    // add it to the list
    last->next = current;
  }

  // fill in the struct
  current->next = NULL;
  current->ID = id;
  current->type = SPH_FUNC;
  current->func = func;
}
void DMFCBase::RegisterPacketReceiver(ubyte id, void (DMFCBase::*func)(ubyte *)) {
  // go to the last handler
  tSPHandler *current, *last;

  if (!SPRoot) {
    // handle special case for first node being added
    SPRoot = (tSPHandler *)malloc(sizeof(tSPHandler));
    if (!SPRoot)
      return;
    current = SPRoot;
  } else {
    last = current = SPRoot;
    while (current) {
      last = current;
      current = current->next;
    }

    // allocate memory for the node
    current = (tSPHandler *)malloc(sizeof(tSPHandler));
    if (!current)
      return;
    // add it to the list
    last->next = current;
  }

  // fill in the struct
  current->next = NULL;
  current->ID = id;
  current->type = SPH_DMFCFUNC;
  current->DMFCfunc = func;
}

// DMFCBase::StartPacket
//
//	 Initializes a packet so it is ready to be sent out.
//	 data = pointer to buffer of data for packet.  This buffer should be MAX_GAME_DATA_SIZE in size, even
//			if you don't plan on using all that data, there is header information that is added
//			in this function.
//   id = ID of the Special packet.  When the packet is recieved by whomever, this is the ID that determines which
//   handler to
//        call.
//	 count = pointer to your packet index pointer
void DMFCBase::StartPacket(ubyte *data, ubyte id, int *count) {
  *count = 0;

  MultiAddByte(id, data, count); // set the first byte (invisible to coder) as the id
}

// DMFCBase::SendPacket
//
//   Ships a Special Packet away to the destination.  This call must be preceeded by a StartPacket call
//	 data = buffer of data to be sent out (same buffer that was passed to StartPacket
//   size = size (in bytes) of the packet
//   destination = either a player number, SP_ALL for all the players or SP_SERVER to send to the server (if you are a
//   client)
void DMFCBase::SendPacket(ubyte *data, int size, int destination) {
  ASSERT(size < MAX_GAME_DATA_SIZE - 2); // allow for a header by the d3
  if (size >= MAX_GAME_DATA_SIZE - 2)
    return;

  // if we are the server and are trying to send to the server then exit out
  if ((GetLocalRole() == LR_SERVER) && (destination == SP_SERVER)) {
    return;
  }

  // we can safely send this packet out to it's appropriate destination

  // see if we are trying to send to the server, it's specially handled
  if (destination == SP_SERVER) {
    DLLMultiClientSendSpecialPacket(data, size);
    return;
  }

  // server trying to send to clients
  if (GetLocalRole() == LR_CLIENT) {
    // we can't send client to client
    return;
  }

  // if server is trying to send to all the clients then we need to send to each individual
  if (destination == SP_ALL) {
    int pnum;
    for (pnum = 0; pnum < DLLMAX_PLAYERS; pnum++) {

      if ((pnum != GetPlayerNum()) && (PacketCheckPlayerNum(pnum))) {
        // ship it!
        DLLMultiSendSpecialPacket(pnum, data, size);
      }
    }
    return;
  }

  // we are only sending to one client
  if (destination != GetPlayerNum())
    DLLMultiSendSpecialPacket(destination, data, size);
}

// DMFCBase::GetGameStateRequest
//
//		Receiver for the server from a client asking for the state of the game
void DMFCBase::GetGameStateRequest(ubyte *data) {
  int pnum;
  int count = 0;
  pnum = MultiGetInt(data, &count);
  CallOnGameStateRequest(pnum);
}

// DMFCBase::SendTeamAssignment
//
//     A DMFC Special Packet function, this sends a team assignment packet to all the players. Server only.
//     playernum = player to change teams
//     team = new team
void DMFCBase::SendTeamAssignment(int playernum, int team, bool spew_onrespawn) {
  if (GetLocalRole() != LR_SERVER)
    return;

  ubyte data[MAX_GAME_DATA_SIZE];
  int count = 0;

  StartPacket(data, SPID_TEAMASSIGNMENT, &count);

  MultiAddByte((spew_onrespawn) ? 1 : 0, data, &count);
  MultiAddInt(playernum, data, &count);
  MultiAddInt(team, data, &count);

  SendPacket(data, count, SP_ALL);
}

// DMFCBase::GetTeamAssignmentPacket
//
//     Reciever for the team assignment packet.
void DMFCBase::GetTeamAssignmentPacket(ubyte *data) {
  int count = 0;
  bool spew = MultiGetByte(data, &count) ? true : false;

  int pnum = MultiGetInt(data, &count);
  int team = MultiGetInt(data, &count);

  if (CheckPlayerNum(pnum)) {
    Players[pnum].team = team;
    CallOnPlayerChangeTeam(pnum, team, true, spew);
  }
}

// DMFCBase::GetChangeTeamPacket
//
//     Reciever for the change team packet.(Server Only)
void DMFCBase::GetChangeTeamPacket(ubyte *data) {
  if (GetLocalRole() != LR_SERVER)
    return;
  // We recieved a request for a player to change teams

  int count = 0;

  bool spew_on_respawn = MultiGetByte(data, &count) ? true : false;
  int pnum = MultiGetByte(data, &count);
  int newteam = MultiGetByte(data, &count);
  if (newteam == 255)
    newteam = -1;

  if (!AllowTeamChange()) {
    return;
  }

  if (!CallOnCanChangeTeam(pnum, newteam)) {
    return;
  }

  if (!CheckPlayerNum(pnum))
    return;

  if ((newteam < 0) || (newteam > m_iNumTeams))
    return;

  Players[pnum].team = newteam;
  CallOnPlayerChangeTeam(pnum, newteam, true, spew_on_respawn);
  SendTeamAssignment(pnum, newteam, spew_on_respawn);
}

// DMFCBase::SendChangeTeamRequest
//
//		Sends a request to the server to change teams (Client Only)
void DMFCBase::SendChangeTeamRequest(int newteam, bool spew_onrespawn) {
  ubyte data[MAX_GAME_DATA_SIZE];
  int count = 0;

  StartPacket(data, SPID_CHANGETEAM, &count);

  MultiAddByte((spew_onrespawn) ? 1 : 0, data, &count);
  MultiAddByte(GetPlayerNum(), data, &count);
  MultiAddByte((newteam == -1) ? 255 : newteam, data, &count);

  SendPacket(data, count, SP_SERVER);
}

// DMFCBase::GetDMFCGameInfo
//
//		Receives information about the DMFC game
void DMFCBase::GetDMFCGameInfo(ubyte *data) {
  int count = 0;

  ubyte level = MultiGetByte(data, &count); // server's HUD Callsign max level
  m_bMakeClientsWait = MultiGetByte(data, &count) ? true : false;
  SwitchServerHudCallsignLevel(level);

  int num_teams = MultiGetByte(data, &count);

  char buffer[256];

  if (num_teams > 1) {
    MultiGetString(buffer, data, &count);
    SetTeamName(RED_TEAM, buffer, false);

    MultiGetString(buffer, data, &count);
    SetTeamName(BLUE_TEAM, buffer, false);

    MultiGetString(buffer, data, &count);
    SetTeamName(GREEN_TEAM, buffer, false);

    MultiGetString(buffer, data, &count);
    SetTeamName(YELLOW_TEAM, buffer, false);
  }

  WaitForServer(m_bMakeClientsWait);
}

// DMFCBase::SendDMFCGameInfo
//
//		Sends information about the DMFC game
void DMFCBase::SendDMFCGameInfo(int player) {
  int r_len, b_len, g_len, y_len;
  int count = 0;
  ubyte data[MAX_GAME_DATA_SIZE];

  StartPacket(data, SPID_DMFCGAMEINFO, &count);

  if (GetNumTeams() > 1) {
    // team game
    r_len = strlen(GetTeamString(RED_TEAM)) + 1;
    b_len = strlen(GetTeamString(BLUE_TEAM) + 1);
    g_len = strlen(GetTeamString(GREEN_TEAM) + 1);
    y_len = strlen(GetTeamString(YELLOW_TEAM) + 1);

    MultiAddByte(m_iServerHUDCallsignLevel, data, &count);
    MultiAddByte(m_bMakeClientsWait, data, &count);
    MultiAddByte(GetNumTeams(), data, &count);

    MultiAddString(GetTeamString(RED_TEAM), data, &count);
    MultiAddString(GetTeamString(BLUE_TEAM), data, &count);
    MultiAddString(GetTeamString(GREEN_TEAM), data, &count);
    MultiAddString(GetTeamString(YELLOW_TEAM), data, &count);

  } else {
    // non-team game
    MultiAddByte(m_iServerHUDCallsignLevel, data, &count);
    MultiAddByte(m_bMakeClientsWait, data, &count);
    MultiAddByte(GetNumTeams(), data, &count);
  }

  SendPacket(data, count, player);

  SendRealGametime(player);
}

// DMFCBase::SendRealGametime
//
//	Updates a player about the 'real' game time
void DMFCBase::SendRealGametime(int pnum) {
  int count = 0;
  ubyte data[MAX_GAME_DATA_SIZE];

  StartPacket(data, SPID_REALGAMETIME, &count);

  MultiAddFloat(RealGametime, data, &count);

  if (m_iProtectedFlags & DMFC_PRF_PAUSETIME) {
    // time is paused
    MultiAddByte(1, data, &count);
  } else {
    // time is not paused
    MultiAddByte(0, data, &count);
  }

  SendPacket(data, count, pnum);
}

// DMFCBase::GetRealGametime
//
//	handles a packet about updating the real game time
void DMFCBase::GetRealGametimePacket(ubyte *data) {
  int count = 0;

  RealGametime = MultiGetFloat(data, &count);

  if (MultiGetByte(data, &count)) {
    // time is paused
    ENABLE_FLAGS(m_iProtectedFlags, DMFC_PRF_PAUSETIME);
  } else {
    // time is not paused
    DISABLE_FLAGS(m_iProtectedFlags, DMFC_PRF_PAUSETIME);
  }
}

// DMFCBase::SendRemoteKey
//
// Sends out the players remote admin key
void DMFCBase::SendRemoteKey(int pnum) {
  // Send this user's remote admin key
  player_record *pr = PRec_GetPRecordByPnum(pnum);
  int precnum = -1;
  if (pr) {
    precnum = translate_precptr_to_index(pr);
  }
  if (precnum != -1) {
    int count = 0;
    ubyte data[MAX_GAME_DATA_SIZE];

    StartPacket(data, SPID_REMOTEKEY, &count);

    ubyte *key = Remote_GetKey(precnum);
    memcpy(&data[count], key, 8);
    count += 8;

    SendPacket(data, count, pnum);
  } else {
    Int3();
  }
}

// DMFCBase::GetRemoteKey
//
// Handles a new remote key from the server
void DMFCBase::GetRemoteKey(ubyte *data) {
  int count = 0;
  ubyte key[8];
  memcpy(key, &data[count], 8);
  count += 8;
  Remote_SetMyKey(key);
}

// DMFCBase::RequestGameState
//
//		Sends a request to the server for the game state
void DMFCBase::RequestGameState(void) {
  if (GetLocalRole() == LR_SERVER)
    return;
  int count = 0;
  ubyte data[MAX_GAME_DATA_SIZE];
  StartPacket(data, SPID_REQGAMESTATE, &count);

  MultiAddInt(GetPlayerNum(), data, &count);

  SendPacket(data, count, SP_SERVER);
}

void DMFCBase::SendVersionToClient(int pnum) {
  if (GetLocalRole() != LR_SERVER)
    return;
  int count = 0;
  ubyte data[MAX_GAME_DATA_SIZE];

  StartPacket(data, SPID_VERSIONID, &count);

  MultiAddInt(DMFC_VERSION_MAJOR, data, &count);
  MultiAddInt(DMFC_VERSION_MINOR, data, &count);

  SendPacket(data, count, pnum);
}

void DMFCBase::GetDMFCVersionCheck(ubyte *data) {
  int major, minor, count = 0;

  major = MultiGetInt(data, &count);
  minor = MultiGetInt(data, &count);

  if ((major != DMFC_VERSION_MAJOR) || (minor != DMFC_VERSION_MINOR)) {
    char buffer[200];
    snprintf(buffer, sizeof(buffer), "Excpecting Version %d.%d, found %d.%d", major, minor, DMFC_VERSION_MAJOR,
             DMFC_VERSION_MINOR);
    FatalError(buffer);
  }
}

// DMFCBase::SendRequestForPlayerRecords
//
//
//	Sends a request to the server to send the player records
void DMFCBase::SendRequestForPlayerRecords(void) {
  if (GetLocalRole() == LR_SERVER)
    return;
  int count = 0;
  ubyte data[MAX_GAME_DATA_SIZE];
  StartPacket(data, SPID_PRECORDREQ, &count);

  MultiAddByte(GetPlayerNum(), data, &count);

  SendPacket(data, count, SP_SERVER);
}

// DMFCBase::ReceiveRequestForPlayerRecords
//
//
//	Recieves and processes a request for a player record
void DMFCBase::ReceiveRequestForPlayerRecords(ubyte *data) {
  if (GetLocalRole() != LR_SERVER)
    return;
  int count = 0;
  int pnum = MultiGetByte(data, &count);
  PRec_SendPRecToPlayer(pnum);

  // send the player his remote admin key
  SendRemoteKey(pnum);
}

// DMFCBase::SendControlMessageToPlayer
//
//
//	Sends a [1 byte] control message to a player
void DMFCBase::SendControlMessageToPlayer(int pnum, ubyte msg) {
  int count = 0;
  ubyte data[MAX_GAME_DATA_SIZE];
  StartPacket(data, SPID_CONTROLMSG, &count);

  MultiAddByte(msg, data, &count);
  MultiAddByte(GetPlayerNum(), data, &count);
  MultiAddByte(pnum, data, &count);

  if (GetLocalRole() != LR_SERVER) {
    // send it to the server to forward to a client
    SendPacket(data, count, SP_SERVER);
  } else
    SendPacket(data, count, pnum);
}

// DMFCBase::ReceiveControlMessage
//
//
//	Handles a control message
void DMFCBase::ReceiveControlMessage(ubyte *data) {
  int count = 0;
  ubyte msg = MultiGetByte(data, &count);
  ubyte src = MultiGetByte(data, &count);
  ubyte dst = MultiGetByte(data, &count);

  if ((GetLocalRole() == LR_SERVER) && (dst != SP_SERVER || dst != GetPlayerNum())) {
    // forward this packet to the correct player
    ubyte fdata[MAX_GAME_DATA_SIZE];
    count = 0;

    StartPacket(fdata, SPID_CONTROLMSG, &count);
    MultiAddByte(msg, fdata, &count);
    MultiAddByte(src, fdata, &count);
    MultiAddByte(dst, fdata, &count);
    SendPacket(fdata, count, dst);
  } else {
    // handle the packet
    CallOnControlMessage(msg, src);
  }
}

// DMFCBase::SendNetGameInfoSync
//
//	Sends out a NetGame info sync packet to all the players
void DMFCBase::SendNetGameInfoSync(int to_who) {
  if (GetLocalRole() != LR_SERVER)
    return;

  ubyte data[MAX_GAME_DATA_SIZE];
  int count = 0;
  StartPacket(data, SPID_NETGAMESYNC, &count);

  MultiAddByte(Netgame->packets_per_second, data, &count);
  MultiAddByte((Netgame->flags & NF_KILLGOAL) ? 1 : 0, data, &count);
  MultiAddByte((Netgame->flags & NF_TIMER) ? 1 : 0, data, &count);

  MultiAddUshort(Netgame->timelimit, data, &count);
  MultiAddUshort(Netgame->killgoal, data, &count);
  MultiAddUshort(Netgame->respawn_time, data, &count);
  MultiAddUshort(Netgame->max_players, data, &count);

  SendPacket(data, count, to_who);
}

// DMFCBase::ReceiveNetGameInfoSync
//
//  Receives a NetGame info sync packet from the server
void DMFCBase::ReceiveNetGameInfoSync(ubyte *data) {
  int count = 0;

  Netgame->packets_per_second = MultiGetByte(data, &count);
  if (MultiGetByte(data, &count))
    Netgame->flags |= NF_KILLGOAL;
  else
    Netgame->flags &= ~NF_KILLGOAL;
  if (MultiGetByte(data, &count))
    Netgame->flags |= NF_TIMER;
  else
    Netgame->flags &= ~NF_TIMER;

  Netgame->timelimit = MultiGetUshort(data, &count);
  Netgame->killgoal = MultiGetUshort(data, &count);
  Netgame->respawn_time = MultiGetUshort(data, &count);
  Netgame->max_players = MultiGetUshort(data, &count);
}

// DMFCBase::SendNewTeamName
//
//	Tells the clients about a team's new name
void DMFCBase::SendNewTeamName(int team) {
  if (GetLocalRole() != LR_SERVER)
    return;

  int count = 0;
  ubyte data[MAX_GAME_DATA_SIZE];
  StartPacket(data, SPID_NEWTEAMNAME, &count);

  MultiAddByte(team, data, &count);
  MultiAddString(DMFC_team_names[team], data, &count);

  SendPacket(data, count, SP_ALL);
}

// DMFCBase::ReceiveNewTeamName
//
//	The server is telling us about a new team name
void DMFCBase::ReceiveNewTeamName(ubyte *data) {
  int count = 0;
  int team = MultiGetByte(data, &count);

  ASSERT(team >= 0 && team < DLLMAX_TEAMS);
  if (team < 0 || team >= DLLMAX_TEAMS)
    return; // invalid team

  char old_teamname[MAX_TEAMNAME_LEN];
  strcpy(old_teamname, DMFC_team_names[team]);

  MultiGetString(DMFC_team_names[team], data, &count);

  CallOnTeamChangeName(team, old_teamname, DMFC_team_names[team]);
}
