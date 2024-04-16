/*
 * $Logfile: /DescentIII/Main/Dmfc/dmfcprecord.cpp $
 * $Revision: 1.1.1.1 $
 * $Date: 2003/08/26 03:57:21 $
 * $Author: kevinb $
 *
 * DMFC Player record functions
 *
 * $Log: dmfcprecord.cpp,v $
 * Revision 1.1.1.1  2003/08/26 03:57:21  kevinb
 * initial 1.5 import
 *
 *
 * 18    10/21/99 9:28p Jeff
 * B.A. Macintosh code merge
 *
 * 17    7/09/99 2:54p Jeff
 * handle gametime better (pause it when needed) if the server is 'waiting
 * for players'
 *
 * 16    5/21/99 10:44p Jeff
 * fix PXO crash due to bytes being out of line when joining a game
 *
 * 15    5/13/99 4:55p Ardussi
 * changes for compiling on the Mac
 *
 * 14    5/12/99 11:05p Jeff
 * dmfc and multiplayer games now have endian friendly packets (*whew*)
 *
 * 13    3/17/99 12:25p Jeff
 * converted DMFC to be COM interface
 *
 * 12    2/11/99 12:51a Jeff
 * changed names of exported variables
 *
 * 11    12/08/98 4:47p Jeff
 * umm, various changes, fixed pilot pics so that they work for everyone
 * now
 *
 * 10    11/11/98 7:19p Jeff
 * changes made so that a dedicated server's team is always -1 (team game
 * or not)
 *
 * 9     10/18/98 7:59p Jeff
 * functions added to dmfc for client->server objnum matching.  Banning
 * now uses tracker id when available.
 *
 * 8     10/17/98 7:30p Jeff
 * network_address compares don't compare port on somethings
 *
 * 7     9/30/98 3:50p Jeff
 * general improvements (many)
 *
 * 6     9/29/98 3:04p Jeff
 * added time in game and start_time support
 *
 * 5     9/22/98 5:20p Jason
 * fixed some player bugs
 *
 * 4     9/21/98 7:11p Jeff
 * made InputCommand interface API and moved existing input commands to
 * the interface.  Changed mprintf/ASSERT so they are valid in DMFC
 *
 * $NoKeywords: $
 */

#include "gamedll_header.h"
#include "DMFC.h"
#include "dmfcinternal.h"
#include <stdlib.h>
#include <string.h>

player_record Player_records[MAX_PLAYER_RECORDS];
int Pnum_to_PRec[DLLMAX_PLAYERS];

// callback pointers for packet packing and unpacking
int (*Player_record_pack_cb)(void *user_info, ubyte *data);
int (*Player_record_unpack_cb)(void *user_info, ubyte *data);

//	Initializes the Player records
void PRec_Init(void) {
  int i;

  for (i = 0; i < MAX_PLAYER_RECORDS; i++) {
    memset(&Player_records[i], 0, sizeof(player_record));
    Player_records[i].state = STATE_EMPTY;
    Player_records[i].user_info = NULL;
    Player_records[i].pinfo = NULL;
    Player_records[i].tracker_id = NULL;
    Player_records[i].user_info_size = 0;
    Player_records[i].total_time_in_game = 0;
  }
  for (i = 0; i < DLLMAX_PLAYERS; i++)
    Pnum_to_PRec[i] = -1;

  Player_record_pack_cb = NULL;
  Player_record_unpack_cb = NULL;
}

//	Frees up and closes the Player records
void PRec_Close(void) {
  for (int i = 0; i < MAX_PLAYER_RECORDS; i++) {
    if (Player_records[i].user_info)
      free(Player_records[i].user_info);

    if (Player_records[i].tracker_id)
      free(Player_records[i].tracker_id);

    if (Player_records[i].pinfo) {
      delete Player_records[i].pinfo;
      Player_records[i].pinfo = NULL;
    }
  }
  Player_record_pack_cb = NULL;
  Player_record_unpack_cb = NULL;
}

//	Sets up the Player records so they can be used for Multiplayer user defined stats
//	sizeof_user_stats : size of the struct for each player's stats
//	pack_callback :	callback function called when data from the struct needs to be packed
//					into a packet.  It is SUPER important that this function packs the data
//					in little endian format. This function gets a pointer to the struct that
//					needs to be packed (user_info), and a buffer in which to pack it to.  This
//					function is to return the number of bytes it has packed.
//	unpack_callback : callback function called when data from the struct needs to be unpacket
//					from a packet.  This data will be in little endian format. Returns the number of
//					bytes unpacked.
//	returns:	1 if size given was <=0 (if so all previous user stats will be removed)
//				0 all went ok
//				-1 out of memory (all user stats memory will be freed)
//				-2 invalid callbacks
int PRec_SetupUserPRec(int sizeof_user_stats, int (*pack_callback)(void *user_info, ubyte *data),
                       int (*unpack_callback)(void *user_info, ubyte *data)) {
  ASSERT(pack_callback != NULL);
  ASSERT(unpack_callback != NULL);
  if (!pack_callback || !unpack_callback)
    return -2;

  int i;

  // calculate to make sure they don't specify too much
  int size = MAX_GAME_DATA_SIZE - (104 + 5 * MAX_PLAYER_RECORDS);
  if (sizeof_user_stats > size) {
    // the user mod specified too big of a size
    mprintf((0, "DMFC Warning: In PRec_SetupUserPRec(), the size given is too large for a packet\n"));
    Int3();
    goto user_stats_err;
  }

  // check the first item to see whats set for it
  if (Player_records[0].user_info) {
    // we need to free up all the user_info's
    for (i = 0; i < MAX_PLAYER_RECORDS; i++) {
      if (Player_records[i].user_info) {
        free(Player_records[i].user_info);
        Player_records[i].user_info = NULL;
      }
      Player_records[i].user_info_size = 0;
    }
  }

  if (sizeof_user_stats <= 0)
    return 1;

  for (i = 0; i < MAX_PLAYER_RECORDS; i++) {
    Player_records[i].user_info = malloc(sizeof_user_stats);
    if (!Player_records[i].user_info)
      goto user_stats_err;
    Player_records[i].user_info_size = sizeof_user_stats;
    memset(Player_records[i].user_info, 0, sizeof_user_stats);
  }

  Player_record_pack_cb = pack_callback;
  Player_record_unpack_cb = unpack_callback;

  return 0;

user_stats_err:
  for (i = 0; i < MAX_PLAYER_RECORDS; i++) {
    if (Player_records[i].user_info) {
      free(Player_records[i].user_info);
      Player_records[i].user_info = NULL;
    }
    Player_records[i].user_info_size = 0;
  }
  return -1;
}

//	Given a pnum it will return the PRec slot of that player, or -1 if it's not a valid player
int PRec_GetPlayerSlot(int pnum) {
  if ((pnum < 0) || (pnum >= DLLMAX_PLAYERS))
    return -1;
  return Pnum_to_PRec[pnum];
}

//	Given a PRec slot, it returns a pointer to the user stats struct, NULL if error, or PRec_SetupUserPRec
//	hasn't been called yet
void *PRec_GetUserStats(int slot) {
  if ((slot < 0) || (slot >= MAX_PLAYER_RECORDS))
    return NULL;
  return Player_records[slot].user_info;
}

//	Returns a pointer to a player_record for the requested slot, NULL if error
player_record *PRec_GetPRecord(int slot) {
  if ((slot < 0) || (slot >= MAX_PLAYER_RECORDS))
    return NULL;
  return &Player_records[slot];
}

//	Returns a pointer to a player_record of a player, NULL if error
player_record *PRec_GetPRecordByPnum(int pnum) {
  int slot = PRec_GetPlayerSlot(pnum);
  return PRec_GetPRecord(slot);
}

//	Sets a disconnect time for a player
void PRec_SetDisconnectTime(int slot, float time) {
  if ((slot < 0) || (slot >= MAX_PLAYER_RECORDS))
    return;
  Player_records[slot].disconnect_time = time;
}

//	Returns the PRec slot of a player who matches the given info, -1 if none
//	addr = network address			(can be NULL)
//	tracker_id = Mastertracker ID  (can be NULL)
int PRec_FindPlayer(char *callsign, network_address *addr, char *tracker_id) {
  if (!callsign)
    return -1;

  bool good_to_use;

  for (int i = 0; i < MAX_PLAYER_RECORDS; i++) {
    // check if the slot isn't empty
    if (Player_records[i].state == STATE_DISCONNECTED) {

      if (!strcmp(Player_records[i].callsign, callsign)) {
        // we have a possible match, the callsign matches

        good_to_use = true;

        if (addr) {
          // we need to check the network address
          if (!basethis->CompareNetworkAddress(&Player_records[i].net_addr, addr, false))
            good_to_use = false; // the network address doesn't match
        }

        if (tracker_id && good_to_use) {
          // we need to check the master tracker_id
          if (Player_records[i].tracker_id) {
            if (strcmp(Player_records[i].tracker_id, tracker_id)) {
              good_to_use = false; // the tracker_id doesn't match
            }
          } else {
            good_to_use = false;
          }
        }

        if (good_to_use) {
          // we found the player based on the information passed in
          return i;
        }
      }
    }
  }
  return -1; // no matches
}

//	Gets the first available free slot (STATE_EMPTY), if none are available it clears and returns
//	the oldest disconnected player (STATE_DISCONNECTED)
int PRec_GetFreeSlot(void) {
  int i, old_player = -1;
  float old_time = 9999999.0f;
  for (i = 0; i < MAX_PLAYER_RECORDS; i++) {
    if (Player_records[i].state == STATE_EMPTY)
      return i; // we found a free slot
    if (Player_records[i].state == STATE_DISCONNECTED) {
      if (old_player == -1) {
        // we found our first, so make sure we mark it
        old_player = i;
        old_time = Player_records[i].disconnect_time;
      }
      // check to see if this player has been disconnected longer than any others
      if (Player_records[i].disconnect_time < old_time) {
        // he has
        old_time = Player_records[i].disconnect_time;
        old_player = i;
      }
    }
  }

  ASSERT(old_player != -1);

  if (old_player != -1) {
    // go through all the records and remove this guy as a victim
    for (int p = 0; p < MAX_PLAYER_RECORDS; p++) {
      if ((p != old_player) && (Player_records[p].pinfo)) {
        ((PInfo *)Player_records[p].pinfo)->RemoveKiller(old_player);
      }
    }

    // clear out this player
    if (Player_records[old_player].tracker_id)
      free(Player_records[old_player].tracker_id);
    void *save_ptr = Player_records[old_player].user_info;
    int size = Player_records[old_player].user_info_size;
    if (Player_records[old_player].pinfo) {
      delete Player_records[old_player].pinfo;
      Player_records[old_player].pinfo = NULL;
    }
    memset(&Player_records[old_player], 0, sizeof(player_record));
    Player_records[old_player].state = STATE_EMPTY;
    Player_records[old_player].user_info = save_ptr;
    Player_records[old_player].user_info_size = size;
    Player_records[old_player].total_time_in_game = 0;
  }
  return old_player;
}

//	Assigns a player to a specified slot (fills in the info, and prepares the slot for action)
//	Do Not use this for a reconnecting player
bool PRec_AssignPlayerToSlot(int pnum, int slot, player *players_array, netplayer *netplayers_array) {
  if ((slot < 0) || (slot >= MAX_PLAYER_RECORDS))
    return false;
  if ((pnum < 0) || (pnum >= DLLMAX_PLAYERS))
    return false;
  if (!players_array)
    return false;
  if (!netplayers_array)
    return false;
  if (Player_records[slot].state != STATE_EMPTY)
    return false;

  Pnum_to_PRec[pnum] = slot;
  Player_records[slot].state = STATE_INGAME;
  strcpy(Player_records[slot].callsign, players_array[pnum].callsign);
  memcpy(&Player_records[slot].net_addr, &netplayers_array[pnum].addr, sizeof(network_address));
  Player_records[slot].pnum = pnum;
  Player_records[slot].start_time = basethis->GetRealGametime();
  Player_records[slot].total_time_in_game = 0;
  Player_records[slot].team = players_array[pnum].team;

  if (basethis->IsMasterTrackerGame() && (basethis->Players[pnum].tracker_id)) {
    // we are in a master tracker game, so save the tracker ID
    mprintf((0, "PREC: Got a PXO Player ID of %s\n", basethis->Players[pnum].tracker_id));
    Player_records[slot].tracker_id = strdup(basethis->Players[pnum].tracker_id);
  } else {
    // either it's not a tracker game or the tracker_id member of players is void
    Player_records[slot].tracker_id = NULL;
  }

  memset(&Player_records[slot].dstats, 0, sizeof(t_dstat));
  if (Player_records[slot].pinfo) {
    delete Player_records[slot].pinfo;
    Player_records[slot].pinfo = NULL;
  }
  Player_records[slot].pinfo = new PInfo(slot);
  return true;
}

//	Reconnects a player to a STATE_DISCONNECTED slot
bool PRec_ReconnectPlayerToSlot(int pnum, int slot, player *players_array, netplayer *netplayers_array) {
  if ((slot < 0) || (slot >= MAX_PLAYER_RECORDS))
    return false;
  if ((pnum < 0) || (pnum >= DLLMAX_PLAYERS))
    return false;
  if (!players_array)
    return false;
  if (!netplayers_array)
    return false;
  if (Player_records[slot].state != STATE_DISCONNECTED)
    return false;

  Pnum_to_PRec[pnum] = slot;
  Player_records[slot].state = STATE_INGAME;
  strcpy(Player_records[slot].callsign, players_array[pnum].callsign);
  memcpy(&Player_records[slot].net_addr, &netplayers_array[pnum].addr, sizeof(network_address));
  Player_records[slot].pnum = pnum;
  Player_records[slot].start_time = basethis->GetRealGametime();

  if (!Player_records[slot].pinfo) {
    // try to create the pinfo now
    Player_records[slot].pinfo = new PInfo(slot);
  }
  return true;
}

//	Disconnects a player from the Player records
bool PRec_DisconnectPlayer(int pnum) {
  if ((pnum < 0) || (pnum >= DLLMAX_PLAYERS))
    return false;

  int slot = Pnum_to_PRec[pnum];
  if (slot == -1)
    return false; // player was never connected

  Pnum_to_PRec[pnum] = -1;
  Player_records[slot].state = STATE_DISCONNECTED;
  Player_records[slot].pnum = -1;

  float curr_time = basethis->GetRealGametime();
  float time_in = curr_time - Player_records[slot].start_time;
  if (time_in > 0) {
    Player_records[slot].total_time_in_game += time_in;
  }

  PRec_SetDisconnectTime(slot, curr_time);
  return true;
}

//	Clear all the level volatile (things that reset with the change of levels) things
void PRec_LevelReset(void) {
  t_dstat *stat;
  for (int i = 0; i < MAX_PLAYER_RECORDS; i++) {
    stat = &Player_records[i].dstats;
    stat->kills[DSTAT_LEVEL] = 0;
    stat->deaths[DSTAT_LEVEL] = 0;
    stat->suicides[DSTAT_LEVEL] = 0;
    if (Player_records[i].pinfo)
      ((PInfo *)Player_records[i].pinfo)->ResetAll();
  }
}

//	Gets the team of a player given the pnum
int PRec_GetPlayerTeam(int pnum) {
  int slot = PRec_GetPlayerSlot(pnum);
  if (slot == -1)
    return 0;
  return Player_records[slot].team;
}

//	Sets the team of a player given the pnum
void PRec_SetPlayerTeam(int pnum, int team) {
  int slot = PRec_GetPlayerSlot(pnum);
  if (slot == -1)
    return;
  Player_records[slot].team = team;
}

void MultiPackNetworkAddress(network_address *addr, ubyte *data, int *count) {
  memcpy(&data[*count], addr->address, 6);
  (*count) += 6;
  MultiAddUshort(addr->port, data, count);
  memcpy(&data[*count], addr->net_id, 4);
  (*count) += 4;
  MultiAddByte(addr->connection_type, data, count);
}

void MultiUnpackNetworkAddress(network_address *addr, ubyte *data, int *count) {
  memcpy(addr->address, &data[*count], 6);
  (*count) += 6;
  addr->port = MultiGetUshort(data, count);
  memcpy(addr->net_id, &data[*count], 4);
  (*count) += 4;
  addr->connection_type = (network_protocol)MultiGetByte(data, count);
}

void MultiPackDStats(t_dstat *stat, ubyte *data, int *count) {
  MultiAddInt(stat->kills[0], data, count);
  MultiAddInt(stat->kills[1], data, count);
  MultiAddInt(stat->deaths[0], data, count);
  MultiAddInt(stat->deaths[1], data, count);
  MultiAddInt(stat->suicides[0], data, count);
  MultiAddInt(stat->suicides[1], data, count);
}

void MultiUnpackDStats(t_dstat *stat, ubyte *data, int *count) {
  stat->kills[0] = MultiGetInt(data, count);
  stat->kills[1] = MultiGetInt(data, count);
  stat->deaths[0] = MultiGetInt(data, count);
  stat->deaths[1] = MultiGetInt(data, count);
  stat->suicides[0] = MultiGetInt(data, count);
  stat->suicides[1] = MultiGetInt(data, count);
}

//	Sends the Player records info to the given client
void PRec_SendPRecToPlayer(int pnum) {
  if (basethis->GetLocalRole() != LR_SERVER)
    return;

  ubyte data[MAX_GAME_DATA_SIZE];
  int callsignlen, totalsize, totalcount;
  totalsize = totalcount = 0;
  int pinfo_size, count;

  for (int i = 0; i < MAX_PLAYER_RECORDS; i++) {
    if (Player_records[i].state != STATE_EMPTY) {
      callsignlen = strlen(Player_records[i].callsign);

      if (Player_records[i].pinfo)
        pinfo_size = ((PInfo *)Player_records[i].pinfo)->GetSizeOfData();
      else
        pinfo_size = 0;

      ubyte tracker_len = 0;

      if (Player_records[i].tracker_id) {
        tracker_len = strlen(Player_records[i].tracker_id);
      }

      count = 0;
      basethis->StartPacket(data, SPID_PRECORD, &count);

      MultiAddByte(i, data, &count);                       // pack byte (record num)
      MultiAddByte(Player_records[i].state, data, &count); // pack byte (state)
      MultiAddByte(callsignlen, data, &count);             // pack byte (strlen(callsign))
      memcpy(&data[count], Player_records[i].callsign, callsignlen);
      count += callsignlen;                                               // pack bytes (callsign)
      MultiPackNetworkAddress(&Player_records[i].net_addr, data, &count); // pack bytes (network_address)
      MultiAddByte(Player_records[i].pnum, data, &count);                 // pack byte  (pnum)
      MultiAddByte((Player_records[i].team == -1) ? 255 : Player_records[i].team, data, &count); // pack byte  (team)

      if (basethis->IsMasterTrackerGame()) {
        MultiAddByte(1, data, &count);
        MultiAddByte(tracker_len, data, &count); // pack byte (tracker len)
        if (tracker_len) {
          memcpy(&data[count], Player_records[i].tracker_id, tracker_len);
          count += tracker_len; // pack bytes (tracker_id)
        }
      } else {
        MultiAddByte(0, data, &count);
      }

      MultiAddFloat(Player_records[i].disconnect_time, data, &count);    //	pack float (disconnect_time/start_time)
      MultiAddFloat(Player_records[i].total_time_in_game, data, &count); // pack float (total time in game)

      MultiPackDStats(&Player_records[i].dstats, data, &count); // pack bytes (dstats)

      MultiAddByte((Player_records[i].user_info_size > 0) ? 1 : 0, data, &count); // pack byte (there is user info)
      if (Player_records[i].user_info_size > 0)                                   // if(user_info_size>0)
      {
        if (Player_record_pack_cb) {
          count += (*Player_record_pack_cb)(Player_records[i].user_info, &data[count]);
        } else {
          Int3();
          count += Player_records[i].user_info_size; // attempt to recover
        }
      }

      // PInfo stuff
      MultiAddInt(pinfo_size, data, &count);
      if (pinfo_size > 0) {
        ((PInfo *)Player_records[i].pinfo)->PackData(&data[count]);
        count += pinfo_size;
      }

      totalsize += count;
      totalcount++;
      basethis->SendPacket(data, count, pnum);
    }
  }

  mprintf((0, "*Player Record: Send out %d packets of total %d bytes to %s\n", totalcount, totalsize,
           basethis->Players[pnum].callsign));
}

//	Receives the Player records info from the server
void PRec_ReceivePRecFromServer(ubyte *data) {
  int count = 0;
  int callsignlen;
  int slot = MultiGetByte(data, &count); // unpack byte (record num)
  int pinfo_size;
  player_record *pr;
  ASSERT((slot >= 0) && (slot < MAX_PLAYER_RECORDS));
  pr = &Player_records[slot];

  pr->state = (slot_state)MultiGetByte(data, &count); // unpack byte (state)
  callsignlen = MultiGetByte(data, &count);           // unpack byte (callsign length)
  memcpy(pr->callsign, &data[count], callsignlen);
  count += callsignlen; // unpack bytes (callsign)
  pr->callsign[callsignlen] = '\0';
  MultiUnpackNetworkAddress(&pr->net_addr, data, &count); // unpack bytes (network address)
  pr->pnum = MultiGetByte(data, &count);                  // unpack byte (pnum)
  pr->team = MultiGetByte(data, &count);
  if (pr->team == 255) // unpack byte (team)
    pr->team = -1;

  if (MultiGetByte(data, &count)) {
    ASSERT(basethis->IsMasterTrackerGame() != 0);
    // there is tracker_id data coming in
    ubyte len = MultiGetByte(data, &count);
    pr->tracker_id = (char *)malloc(len + 1);

    if (pr->tracker_id) {
      memcpy(pr->tracker_id, &data[count], len);
      pr->tracker_id[len] = '\0';
    } else {
      mprintf((0, "PREC: UH OH!!! OUT OF MEMORY\n"));
    }
    count += len;
  } else {
    ASSERT(basethis->IsMasterTrackerGame() == 0);
  }

  pr->disconnect_time = MultiGetFloat(data, &count);    // unpack float (disconnect time/start time)
  pr->total_time_in_game = MultiGetFloat(data, &count); // unpack float (total time in game)
  MultiUnpackDStats(&pr->dstats, data, &count);         // unpack bytes (dstats)

  if (MultiGetByte(data, &count)) {
    ASSERT(pr->user_info != NULL);
    if (Player_record_unpack_cb) {
      count += (*Player_record_unpack_cb)(pr->user_info, &data[count]);
    } else {
      Int3();
      count += pr->user_info_size; // attempt to recover
    }
  } else {
    ASSERT(pr->user_info == NULL);
  }

  if (pr->pinfo) {
    delete pr->pinfo;
    pr->pinfo = NULL;
  }
  pr->pinfo = new PInfo(slot);

  // PInfo stuff
  pinfo_size = MultiGetInt(data, &count);
  if (pinfo_size > 0) {
    if (pr->pinfo)
      ((PInfo *)pr->pinfo)->UnpackData(&data[count], pinfo_size);
    count += pinfo_size;
  } else {
    if (pr->pinfo)
      ((PInfo *)pr->pinfo)->ResetAll();
  }

  if (pr->pnum != 255) {
    Pnum_to_PRec[pr->pnum] = slot;
  }
}
