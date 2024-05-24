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

 * $Logfile: /DescentIII/Main/dmfc/dmfcclient.cpp $
 * $Revision: 1.1.1.1 $
 * $Date: 2003/08/26 03:57:20 $
 * $Author: kevinb $
 *
 * Client Event handlers
 *
 * $Log: dmfcclient.cpp,v $
 * Revision 1.1.1.1  2003/08/26 03:57:20  kevinb
 * initial 1.5 import
 *
 *
 * 92    9/05/01 6:04p Matt
 * Added code to save the user's preferred HUD name level setting even if
 * the server bashes it down.
 *
 * 91    7/15/99 3:33p Jeff
 * play a sound when a player disconnects/leaves the game
 *
 * 90    7/13/99 12:11p Jeff
 * added some specific text taunt token decoding
 *
 * 89    7/13/99 10:05a Jeff
 * text taunt token decoding
 *
 * 88    7/09/99 7:02p Jeff
 * put in countdown timer for when a level is about to end
 *
 * 87    7/09/99 6:17p Jeff
 * added $remoteadminlogout and $wait commands
 *
 * 86    7/09/99 2:53p Jeff
 * handle gametime better (pause it when needed) if the server is 'waiting
 * for players'
 *
 * 85    7/08/99 9:56p Jeff
 * added event handler for weapon fired event
 *
 * 84    7/08/99 2:39a Jeff
 * rough implementation of remote administration checked in.  Still needs
 * some polishing, but should work basically.
 *
 * 83    7/07/99 6:10p Jeff
 * don't count the time that a player is in observer mode for death/kill
 * messages
 *
 * 82    6/22/99 5:41p Jeff
 * removed int3 (causing probs in Linux)
 *
 * 81    6/11/99 5:36p Jeff
 * removed ai_info #ifdefs (better way of doing it)
 *
 * 80    6/10/99 12:41p Jeff
 * no ai_info in non-Outrage builds
 *
 * 79    5/24/99 11:05p Jeff
 * fixed bug for dedicated server team getting reset on endlevel
 *
 * 78    5/11/99 9:42p Jeff
 * fixed bug changing teams while dead
 *
 * 77    5/10/99 2:43a Jeff
 * handle new scheme of player's joining in a team game, where the team is
 * set before player enters game in the main code, but the team is
 * determined via event call to dmfc
 *
 * 76    5/08/99 4:45a Jeff
 * fixed printscreen position
 *
 * 75    5/08/99 4:30a Jeff
 * fixed sequencing bug where clients never got a level end event for the
 * multiplayer games
 *
 * 74    5/07/99 5:16p Jeff
 * fixed player disconnect at level end
 *
 * 73    5/07/99 2:21p Jeff
 * fixed (?) not disconnecting clients on level end
 *
 * 72    5/04/99 8:46p Jeff
 * display icon when someone plays an audio taunt
 *
 * 71    4/03/99 4:06p Jeff
 * added loss/ping gauge
 *
 * 70    3/30/99 7:42p Jeff
 * fixed a misspelling on a function name
 *
 * 69    3/22/99 6:20p Jeff
 * added 2 more audio taunts.  a mulitplayer event when someone plays an
 * audio taunt.  option to disable audio taunts.
 *
 * 68    3/22/99 1:55p Jeff
 * make sure initialization happens (possible crashing)
 *
 * 67    3/17/99 12:23p Jeff
 * converted DMFC to be COM interface
 *
 * 66    2/11/99 12:50a Jeff
 * changed names of exported variables
 *
 * 65    2/09/99 3:32p Jeff
 * table file parser takes quotes strings for force keywords
 *
 * 64    2/07/99 1:19a Jeff
 * added new multiplayer game events EVT_GAMEOBJKILLED and
 * EVT_GAMEOBJDESTROYED
 *
 * 63    2/05/99 8:24p Jeff
 * added table file parser macros
 *
 * 62    2/04/99 7:17p Jeff
 * put in sounds for hud messages
 *
 * 61    2/02/99 8:43a Chris
 * I made buildings with AI work correctly (ie really big robots should be
 * buildings)
 * anim to and from states are now shorts instead of bytes
 *
 * 60    2/01/99 10:52p Jeff
 * added sounds for kills (teammate and regular)
 *
 * 59    1/31/99 7:26p Matt
 * Renamed a bunch of functions to have HUD capitalized
 *
 * 58    1/19/99 3:55a Jeff
 * all strings localized out
 *
 * 56    1/15/99 3:54a Jeff
 * exported a couple more functions.  Added event handlers for Weapon
 * collide event
 *
 * 55    1/12/99 2:55p Jeff
 * added/finished the waiting for player's to join dialog
 *
 * 54    1/10/99 7:41p Jeff
 * added initial version of waitforplayers dialog
 *
 * 53    1/07/99 5:01p Jeff
 * added Int3's and updated all net games to use stats manager...correctly
 * too
 *
 * 52    1/06/99 7:02p Jeff
 * added a multiplayer event for game controls
 *
 * 51    12/23/98 6:38p Kevin
 * All UDP data (except gamespy) now uses one (registered) port number
 *
 * 50    12/08/98 4:47p Jeff
 * umm, various changes, fixed pilot pics so that they work for everyone
 * now
 *
 * 49    12/08/98 3:29p Jeff
 * updated the team control dialog so the server can determine if they
 * want to make the clients wait
 *
 * 48    12/08/98 12:17p Jeff
 * various changes that include an improved Team Control dialog (doesn't
 * switch teams until exit..) and spew/respawn players that change teams
 *
 * 47    12/03/98 7:05p Jeff
 * updated new stats
 *
 * 46    12/01/98 6:56p Jeff
 * put in quick and dirty implementation of pilot pics for testing
 *
 * 45    11/19/98 5:56p Jeff
 * added slider exported and improved Hoard
 *
 * 44    11/17/98 6:29p Jeff
 * mod can specify whether or not to display the team setup dialog on team
 * game start.  Added a menu item to display team setup dialog in mid-game
 *
 * 43    11/16/98 5:35p Jeff
 * removed log functions, added support for changing team names, fixed ctf
 * to work better with different team names
 *
 * 42    11/11/98 7:19p Jeff
 * changes made so that a dedicated server's team is always -1 (team game
 * or not)
 *
 * 41    11/11/98 12:47p Jeff
 * fixed bug where it would give team change messages in a non-team game
 *
 * 40    11/01/98 1:59a Jeff
 * made a $help inputcommand for help in a dedicated server environment
 *
 * 39    10/30/98 12:47p Jeff
 * cut down a couple bytes on memory usage
 *
 * 38    10/29/98 7:01p Jeff
 * creation of team placement dialog.  Moved TranslateEvent into DMFC
 *
 * 37    10/20/98 12:16p Jeff
 * added death message filter, hud callsign filter
 *
 * 36    10/18/98 7:59p Jeff
 * functions added to dmfc for client->server objnum matching.  Banning
 * now uses tracker id when available.
 *
 * 35    10/15/98 6:18p Jeff
 * created the is player banned event, removed prejoin event
 *
 * 34    10/15/98 1:34p Jeff
 * added scrollable onscreen menu.  Remove ban in dmfc.  prejoin event
 *
 * 33    10/13/98 2:15a Jeff
 * created new event for when a player leaves a multiplayer game.  Fixed
 * some 'english' bugs in the network games.
 *
 * 32    10/11/98 2:57a Jeff
 * added new multiplayer event EVT_GAME_INTERVAL, which is to be called on
 * interval, and EVT_HUD_INTERVAL is only to be called when the hud is to
 * be rendered
 *
 * 31    10/08/98 3:37p Jeff
 * general improvements (Netgame info things, save to registry).  Changes
 * so it would send packets on NETSEQ_OBJECTS
 *
 * 30    10/05/98 2:49p Jeff
 *
 * 29    10/01/98 7:02p Jeff
 * implemented colored HUD callsigns
 *
 * 28    10/01/98 11:30a Jeff
 * made the observer mode events into just a client event
 *
 * 27    9/30/98 4:21p Jeff
 * team changing is handled correctly
 *
 * 26    9/30/98 4:07p Jeff
 *
 * 25    9/30/98 3:50p Jeff
 * general improvements (many)
 *
 * 24    9/28/98 5:05p Jeff
 * made the statisitical death messages an option in the menu
 *
 * 23    9/25/98 7:57p Jeff
 * removed CheckPInfo call
 *
 * 22    9/25/98 7:25p Jeff
 *
 * 21    9/25/98 4:50p Jeff
 *
 * 20    9/24/98 6:54p Jeff
 * added DisconnectMe() and made use of it when you get kicked/banned
 *
 * 19    9/24/98 5:52p Jeff
 * starting adding statistical death messages, checked in for testing
 *
 * 18    9/23/98 4:56p Jeff
 * set teams always for 1 team game
 *
 * 17    9/23/98 4:17p Jeff
 * basic changes/improvements, started changing death messages
 *
 * 16    9/21/98 7:11p Jeff
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

#define SND_MYTEAM_TEAMMATE_KILLED "PlayerDeath"
#define SND_NONTEAM_KILL "PlayerDeath"

/*
$$TABLE_SOUND "PlayerDeath"
*/

// ######################### Client allowed event handlers############################

// DMFCBase::OnClientPlayerKilled (Only called if server tells client to execute)
//
//  Event handler for when a player gets killed, either by another player or some other way.
//  killer_obj = object pointer to the object that killed the player
//  victim_pnum = player number of the player that got killed
void DMFCBase::OnClientPlayerKilled(object *killer_obj, int victim_pnum) {
  uint32_t hash = 0xFFFFFFFF;
  int kpnum;
  player_record *kpr, *vpr;

  if (!killer_obj) {
    kpnum = -1;
  } else {
    if ((killer_obj->type == OBJ_PLAYER) || (killer_obj->type == OBJ_GHOST) || (killer_obj->type == OBJ_OBSERVER)) {
      kpnum = killer_obj->id;

      // now we need to extract out the weapon of the kill
      if (Data->iParam != -1)
        hash = (uint32_t)Data->iParam;
    } else if (killer_obj->type == OBJ_ROBOT || (killer_obj->type == OBJ_BUILDING && killer_obj->ai_info)) {
      // countermeasure kill
      kpnum = GetCounterMeasureOwner(killer_obj);
    } else
      kpnum = -1;
  }

  if (victim_pnum != -1) {
    if (kpnum == victim_pnum) {
      // suicide
      vpr = PRec_GetPRecordByPnum(victim_pnum);
      if (vpr) {
        vpr->dstats.suicides[DSTAT_LEVEL]++;
        vpr->dstats.suicides[DSTAT_OVERALL]++;
      }
      UpdatePInfo(kpnum, kpnum, 1);
    } else {
      // regular death
      vpr = PRec_GetPRecordByPnum(victim_pnum);

      if (vpr) {
        vpr->dstats.deaths[DSTAT_LEVEL]++;
        vpr->dstats.deaths[DSTAT_OVERALL]++;
      }

      if (kpnum != -1) {
        kpr = PRec_GetPRecordByPnum(kpnum);
        if (kpr) {
          kpr->dstats.kills[DSTAT_LEVEL]++;
          kpr->dstats.kills[DSTAT_OVERALL]++;
        }
        UpdatePInfo(victim_pnum, kpnum, 1);
      }
    }
  }

  static int teammate_kill_id = -2;
  static int nonteam_kill_id = -2;

  if (killer_obj && killer_obj->type == OBJ_PLAYER && victim_pnum != -1) {

    if (Players[killer_obj->id].team == Players[victim_pnum].team && (GetNumTeams() >= 2) &&
        (killer_obj->id != victim_pnum)) {
      // a teammate killed another teammate!
      if (teammate_kill_id == -2) {
        teammate_kill_id = DLLFindSoundName(IGNORE_TABLE(SND_MYTEAM_TEAMMATE_KILLED));
      }
      if (teammate_kill_id != -1) {
        DLLPlay2dSound(teammate_kill_id, MAX_GAME_VOLUME / 2);
      }
    } else {
      // normal kill sound
      if (nonteam_kill_id == -2) {
        nonteam_kill_id = DLLFindSoundName(IGNORE_TABLE(SND_NONTEAM_KILL));
      }
      if (nonteam_kill_id != -1) {
        DLLPlay2dSound(nonteam_kill_id, MAX_GAME_VOLUME / 2);
      }
    }
  }

  // if DMFC is supposed to handle Deathmessages, than do one
  if (m_iProtectedFlags & DMFC_PRF_AUTODEATHMSG)
    DoRandomDeathMessage(GetItObjNum(), GetMeObjNum(), hash);
}

// DMFCBase::OnClientPlayerExploded (Only called if server tells client to execute)
//
//  Event handler for when a player explodes.  Gets called after a EVT_GAMEPLAYERKILLED event.
//  player_num = player number of the player exploding
void DMFCBase::OnClientPlayerExploded(int player_num) {}

// DMFCBase::OnClientCollide (Only called if server tells client to execute)
//
//   Event handler for when two objects collide.  At least one of the two objects is a player or a robot.
//   Be aware that this event will be called twice per collision, the second time it is called the me and
//   it objects will be flipped.
//	 ALSO NOTE: In order for the OnClientCollide() that passes the point and normal to have valid values
//	 the server must pass true to the parameter to send arguments in CallClientEvent().
//   me_obj = object pointer to the me object
//   it_obj = object pointer to the it object
void DMFCBase::OnClientCollide(object *me_obj, object *it_obj) {}
void DMFCBase::OnClientCollide(object *me_obj, object *it_obj, vector *point, vector *normal) {}

// DMFCBase::OnClientPlayerChangeSegment (Only called if server tells client to execute)
//
//   Event handler for when a player changes rooms or a "large" terrain cell
//   (8x8 Terrain block).
//   player_num = player number of the player who just changed the room/segment
//   newseg = New room/cell location
//	 oldseg = Old room/cell location
void DMFCBase::OnClientPlayerChangeSegment(int player_num, int newseg, int oldseg) {}

// DMFCBase::OnClientObjectChangeSegment (Only called if server tells client to execute)
//
//   Event handler for when a player changes rooms or a "large" terrain cell
//   (8x8 Terrain block).
//   obj = Object pointer of the object who just changed the room/segment
//   newseg = New room/cell location
//	 oldseg = Old room/cell location
void DMFCBase::OnClientObjectChangeSegment(object *obj, int newseg, int oldseg) {}

// DMFCBase::OnClientPlayerEntersGame (Only called if server tells client to execute)
//
//   Event handler for when a player enters the game.  This will only get called once per
//   player, it usually gets called right after they connect to the server to start playing.
//   player_num = player number of the player entering the game
void DMFCBase::OnClientPlayerEntersGame(int player_num) {
  if ((GetLocalRole() == LR_SERVER) || (player_num != GetPlayerNum())) {
    int slot;

    if (IsMasterTrackerGame())
      slot = PRec_FindPlayer(Players[player_num].callsign, NULL, Players[player_num].tracker_id);
    else
      slot = PRec_FindPlayer(Players[player_num].callsign, &NetPlayers[player_num].addr, NULL);

    if (slot == -1) {
      // we have a new player
      slot = PRec_GetFreeSlot();
      if (!PRec_AssignPlayerToSlot(player_num, slot, Players, NetPlayers))
        mprintf(
            (0, "Unable to assign New Player (%s) to Player Record slot #%d\n", Players[player_num].callsign, slot));
      else
        mprintf((0, "(%s) has been assigned to Player Record slot #%d\n", Players[player_num].callsign, slot));
      CallOnPlayerConnect(player_num);
    } else {
      // we have a reconnecting player
      if (!PRec_ReconnectPlayerToSlot(player_num, slot, Players, NetPlayers))
        mprintf((0, "Unable to reassign reconnecting player (%s) to Player Record slot #%d\n",
                 Players[player_num].callsign, slot));
      else
        mprintf((0, "Reconnected player (%s) to Player Record slot #%d\n", Players[player_num].callsign, slot));
      CallOnPlayerReconnect(player_num);
    }
  }

  // If the correct options are set, go into the D1/D2 style wait screen
  if (GetLocalRole() == LR_SERVER && (player_num == GetPlayerNum())) {

    if (GetNumTeams() > 1 && (!(m_iProtectedFlags & DMFC_PRF_NOTEAMSELECTONSTART))) {
      // Display the team placement dialog
      static char enable;
      enable = 1;
      StartUIWindow(UIID_TEAMPLACEMENT, &enable);
    }

    if (GetNumTeams() < 2 && (!(m_iProtectedFlags & DMFC_PRF_NOWAITFORPLAYERS))) {
      StartUIWindow(UIID_WAITFORPLAYERS, NULL);
    }
  }

  // Load up the pilot picture bitmap
  //--------------------------------
  if (player_num == GetPlayerNum()) {
    // since we are entering the game, go through all the players and setup their bitmaps
    for (int w = 0; w < DLLMAX_PLAYERS; w++) {
      if (CheckPlayerNum(w) || (w == 0 && GetLocalRole() == LR_SERVER)) {
        if (PilotPicBmpHandles[w] > BAD_BITMAP_HANDLE) {
          DLLbm_FreeBitmap(PilotPicBmpHandles[w]);
          PilotPicBmpHandles[w] = BAD_BITMAP_HANDLE;
        }

        mprintf((0, "PPIC ID: %d is %d\n", w, NetPlayers[w].pilot_pic_id));
        if (NetPlayers[w].pilot_pic_id != 65535) {
          PilotPicBmpHandles[w] = DLLPPic_GetBitmapHandle(NetPlayers[w].pilot_pic_id);
          if (PilotPicBmpHandles[w] < BAD_BITMAP_HANDLE)
            PilotPicBmpHandles[w] = BAD_BITMAP_HANDLE;
        } else {
          PilotPicBmpHandles[w] = BAD_BITMAP_HANDLE;
        }
      } else {
        PilotPicBmpHandles[w] = BAD_BITMAP_HANDLE;
      }
    }
  } else {
    if (PilotPicBmpHandles[player_num] > BAD_BITMAP_HANDLE) {
      DLLbm_FreeBitmap(PilotPicBmpHandles[player_num]);
      PilotPicBmpHandles[player_num] = BAD_BITMAP_HANDLE;
    }

    mprintf((0, "PPIC ID: %d is %d\n", player_num, NetPlayers[player_num].pilot_pic_id));
    if (NetPlayers[player_num].pilot_pic_id != 65535) {
      PilotPicBmpHandles[player_num] = DLLPPic_GetBitmapHandle(NetPlayers[player_num].pilot_pic_id);
      if (PilotPicBmpHandles[player_num] < BAD_BITMAP_HANDLE)
        PilotPicBmpHandles[player_num] = BAD_BITMAP_HANDLE;
    } else {
      PilotPicBmpHandles[player_num] = BAD_BITMAP_HANDLE;
    }
  }

  // Play the player hud message sound
  static int sound_id = -2;
  if (sound_id == -2) {
    sound_id = DLLFindSoundName("Hudmessage");
  }

  if (sound_id > -1 && GetPlayerNum() != player_num) {
    DLLPlay2dSound(sound_id, MAX_GAME_VOLUME / 2);
  }
}

// DMFCBase::OnClientPlayerDisconnect
//
//   Event handler for when a player disconnects from the server.
//   player_num = player number of the player that just disconnected
void DMFCBase::OnClientPlayerDisconnect(int player_num) {
  // Play the player hud message sound
  static int sound_id = -2;
  if (sound_id == -2) {
    sound_id = DLLFindSoundName("Hudmessage");
  }

  if (sound_id > -1 && GetPlayerNum() != player_num) {
    DLLPlay2dSound(sound_id, MAX_GAME_VOLUME / 2);
  }

  // save the team
  PRec_SetPlayerTeam(player_num, Players[player_num].team);

  if (!PRec_DisconnectPlayer(player_num))
    mprintf((0, "Unable to disconnect player (%s) from Player Records\n", Players[player_num].callsign));
  else
    mprintf((0, "Disconnected player (%s) from Player Records\n", Players[player_num].callsign));
}

// DMFCBase::OnMeDisconnectFromServer
//
//	Event handler that gets called if we disconnect from the server for some reason (not purposesly quit)
void DMFCBase::OnMeDisconnectFromServer(void) {
  // We're disconnecting so save the stats to file
  if (m_iProtectedFlags & DMFC_PRF_AUTOSAVEDISC)
    CallOnDisconnectSaveStatsToFile();
}

// DMFCBase::OnClientGameCreated (Only called if server tells client to execute)
//
//   Event handler when the server's game first gets started.  This will only be called once
//   while the server is running, and that is when the server first starts up.
void DMFCBase::OnClientGameCreated(void) {}

// DMFCBase::OnClientLevelChange (Only called if server tells client to execute)
//
//   Event handler when the server changes levels.  This will get called after a level ends and a server
//   is starting a new level.
void DMFCBase::OnClientLevelChange(void) {}

// DMFCBase::OnClientLevelStart (Only called if server tells client to execute)
//
//   Event handler for when a multiplayer level is starting up.  This will get called right before the level
//   starts.
void DMFCBase::OnClientLevelStart(void) {
  // Clear this array so we don't get weird anomalies
  ResetPlayersInGame();

  // Reset the timers
  InitTimers();

  // Reset level stats of Player Records
  PRec_LevelReset();

  // reset the pinfo list
  ResetPInfo();

  // Request Player Records
  SendRequestForPlayerRecords();

  DisplayTauntIndicator = false;
}

// DMFCBase::OnClientLevelEnd
//
//   Event handler for when a multiplayer level is ending.
void DMFCBase::OnClientLevelEnd(void) {
  MarkPlayersInGame();

  if (m_iProtectedFlags & DMFC_PRF_AUTOSAVELEVELEND)
    CallOnLevelEndSaveStatsToFile();

  KillAllTimers();
  ENABLE_FLAGS(m_iProtectedFlags, DMFC_PRF_PLRFIRSTTIME);

  // Call PLRInit so data can be initialized
  CallOnPLRInit();

  for (int i = 0; i < DLLMAX_PLAYERS; i++) {
    if ((NetPlayers[i].flags & NPF_CONNECTED) &&
        (NetPlayers[i].sequence >= NETSEQ_PLAYING || NetPlayers[i].sequence == NETSEQ_WAITING_FOR_LEVEL)) {

      // save the team
      PRec_SetPlayerTeam(i, Players[i].team);

      if (!PRec_DisconnectPlayer(i))
        mprintf((0, "Unable to disconnect player (%s) from Player Records\n", Players[i].callsign));
      else
        mprintf((0, "Disconnected player (%s) from Player Records\n", Players[i].callsign));
    }
  }
}

// DMFCBase::OnPlayerEntersObserver
//
//	Event handler when a player becomes an observer mode
//	If they are piggybacking another player than piggy is the object pointer, else it's NULL
void DMFCBase::OnPlayerEntersObserver(int pnum, object *piggy) {
  mprintf((0, "Player %d entering observermode %s\n", pnum, (piggy) ? "Piggyback" : "Roam"));
  player_record *pr = PRec_GetPRecordByPnum(pnum);
  if (pr && pr->state == STATE_INGAME) {
    PInfo *pi = pr->pinfo;
    ASSERT(pi != NULL);
    pi->EnterObserverMode();
  } else {
    Int3();
  }
}

// DMFCBase::OnPlayerExitsObserver
//
//	Event handler when a player is leaving observer mode
void DMFCBase::OnPlayerExitsObserver(int pnum) {
  mprintf((0, "Player %d leaving observer mode\n", pnum));
  player_record *pr = PRec_GetPRecordByPnum(pnum);
  if (pr && pr->state == STATE_INGAME) {
    PInfo *pi = pr->pinfo;
    ASSERT(pi != NULL);
    pi->ExitObserverMode();
  } else {
    Int3();
  }
}

// DMFCBase::OnCanChangeTeam
//
//    Called to determine if a player can change teams, you can override this so the server won't
//    let a player change teams if it is not desired (for that player)
bool DMFCBase::OnCanChangeTeam(int pnum, int newteam) {
  if (Players[pnum].team == -1)
    return false; // no team switching for dedicated server

  // AnnounceTeamChangeDeny(pnum);
  return true;
}

// DMFCBase::OnSaveStatsToFile
//
//
//		The user is requesting the game stats to be saved to file, you must handle this completly on
//	the game's side, nothing is done in DMFC
void DMFCBase::OnSaveStatsToFile(void) { mprintf((0, "User requested stats to be saved!\n")); }

// DMFCBase::OnLevelEndSaveStatsToFile
//
//
//	The game should save the "End of Level" stats to file.  You must handle this completly on
//	the game's side, nothing is done in DMFC
void DMFCBase::OnLevelEndSaveStatsToFile(void) {}

// DMFCBase::OnDisconnectSaveStatsToFile
//
//
//	The game should save the stats because the player (only ourself as the client in the game),
//	has disconnected.
void DMFCBase::OnDisconnectSaveStatsToFile(void) {}

// DMFCBase::OnSpecialPacket
//
//   Event handler for when a special packet arrives and needs to be processed.
//   Both the server and client can get this event, although it is more common for the
//   client to recieve these.
void DMFCBase::OnSpecialPacket(void) {
  if (!SPRoot)
    return;

  // see if we have a handler for the ID, if so, call the handler, else do nothing
  ubyte *data = Data->special_data;
  int id = data[0];

  tSPHandler *current;
  current = SPRoot;
  while (current) {
    if (current->ID == id) {
      // we got a winner
      switch (current->type) {
      case SPH_DMFCFUNC:
        if (current->DMFCfunc)
          (this->*current->DMFCfunc)(data + 1);
        break;
      case SPH_FUNC:
        if (current->func)
          (*current->func)(data + 1);
        break;
      }
      return;
    }
    current = current->next;
  }
}

// DMFCBase::OnHUDInterval
//
//	Event handler that gets called once a frame when it's time to render the HUD images
void DMFCBase::OnHUDInterval(void) {
  if (m_bMakeClientsWait)
    WaitingForServerLoop();
  if (m_iProtectedFlags & DMFC_PRF_DISPPLAYERINFO)
    DisplayPlayerInfo(MenuBackgroundBMP);
  if (m_iProtectedFlags & DMFC_PRF_DISPNETGAMEINFO)
    DisplayNetGameInfo(MenuBackgroundBMP);
  LossGuageFrame();
  DisplayTauntIndicatorFrame();

  if (m_iProtectedFlags & DMFC_PRF_INOPTIONS) {
    // Update all the state submenus so they are reporting the correct value
    if (TeamMenuItem)
      TeamMenuItem->SetState(GetPlayerTeam(GetPlayerNum()));
    if (AutoBalanceItem)
      AutoBalanceItem->SetState((m_iProtectedFlags & DMFC_PRF_AUTOTEAMSELECT) ? 1 : 0);
    if (AllowTeamChangeItem)
      AllowTeamChangeItem->SetState(AllowTeamChange() ? 1 : 0);
    if (ObserverItem)
      ObserverItem->SetState((Objects[Players[GetPlayerNum()].objnum].type == OBJ_OBSERVER) ? 1 : 0);
    if (StatisticMessagesItem)
      StatisticMessagesItem->SetState((m_iProtectedFlags & DMFC_PRF_DISPSTATHUDMSGS) ? 1 : 0);
    if (SaveStatsLevelEndItem)
      SaveStatsLevelEndItem->SetState((m_iProtectedFlags & DMFC_PRF_AUTOSAVELEVELEND) ? 1 : 0);
    if (SaveStatsDisconnectItem)
      SaveStatsDisconnectItem->SetState((m_iProtectedFlags & DMFC_PRF_AUTOSAVEDISC) ? 1 : 0);
    if (MenuBackgroundItem)
      MenuBackgroundItem->SetState((m_iProtectedFlags & DMFC_PRF_DISPMENUBACKGR) ? 1 : 0);
    if (NetGameInfoItem)
      NetGameInfoItem->SetState(IsDisplayingNetGameInfo() ? 1 : 0);
    if (HUDCallsignsItem)
      HUDCallsignsItem->SetState(m_iMyCurrentHUDCallsignLevel);
    if (ServerHUDCallsignsItem)
      ServerHUDCallsignsItem->SetState(m_iServerHUDCallsignLevel);
    if (DeathMessageFilterItem)
      DeathMessageFilterItem->SetState(m_iDeathMessageFilter);
    if (ShipLogosItem)
      ShipLogosItem->SetState(AreLogosEnabled() ? 1 : 0);
    if (HUDIndicatorItem)
      HUDIndicatorItem->SetState(LossGuageEnabled ? 1 : 0);

    // We have an options screen up, so draw it
    DrawMenu();
  }

  // DisplayPilotPicsFrame();

  // Display countdown if needed
  DisplayTimelimitCountdown();

  DLLgrtext_Flush();
}

// DMFCBase::OnInterval
//
//   Event handler that gets called once a frame
void DMFCBase::OnInterval(void) {
  if (!(m_iProtectedFlags & DMFC_PRF_PAUSETIME)) {
    RealGametime += *Frametime; // increment the timer
  }

  // If we are a dedicated server, we are waiting, and there is a wait time
  // check to see if it's time to disable
  if (GetLocalRole() == LR_SERVER && IAmDedicatedServer() && m_bMakeClientsWait) {
    if (DedicatedLevelWait > 0 && DedicatedLevelWait <= (*Gametime)) {
      // time to stop waiting!
      mprintf((0, "Telling clients that they can play!\n"));
      DPrintf("Allowing Clients To Play\n");
      PauseRealGameTime(false);
      MakeClientsWait(false);
    }
  }

  // CheckPInfo();

  // Process remote admin frame
  Remote_ProcessFrame();

  // Process all currently running timers
  ProcessTimers();

  // Handle time limit games
  float tleft;
  if (GetLocalRole() == LR_SERVER) {
    if ((m_iProtectedFlags & DMFC_PRF_AUTOTIMELIMIT) && (GetTimeLeft(&tleft))) {
      if (tleft <= 0) {
        EndLevel();
        return;
      }
    }
  }
}

// DMFCBase::OnPLRInterval
//
//  Event handler that gets called once a frame when the Post Level Results screen is being display
void DMFCBase::OnPLRInterval(void) {
  DLLgrtext_SetFont(Game_fonts[SMALL_UI_FONT_INDEX]);
  DLLgrtext_SetColor(GR_RGB(255, 40, 40));
  DLLgrtext_CenteredPrintf(0, 13, Netgame->scriptname);
  DLLgrtext_CenteredPrintf(0, 461, DTXT_PRESSPRNTSCRN);
}

// DMFCBase::OnPLRInit
//
//  Event handler that gets called the first frame of the PLR screen for each level
void DMFCBase::OnPLRInit(void) { DISABLE_FLAGS(m_iProtectedFlags, DMFC_PRF_PLRFIRSTTIME); }

// DMFCBase::OnKeypress
//
//   Event handler for when a user presses a key while in the game
//   key = Key code of the key being pressed
void DMFCBase::OnKeypress(int key) {
  if (m_iProtectedFlags & DMFC_PRF_INOPTIONS) {
    Data->iRet = 1;
    // Handle keyboard input for an options menu
    if (key == K_F6 || key == K_ESC) { // escape out of options
      DISABLE_FLAGS(m_iProtectedFlags, DMFC_PRF_INOPTIONS);
    }
    if (key == K_ENTER) {
      Menu.Execute();
    }
    switch (key) {
    case K_DOWN: {
      Menu.Down();
    } break;
    case K_UP: {
      Menu.Up();
    } break;
    case K_RIGHT: {
      Menu.Forward();
    } break;
    case K_LEFT: {
      Menu.Back();
    } break;
    }
  } else if (key == K_F6) {
    ENABLE_FLAGS(m_iProtectedFlags, DMFC_PRF_INOPTIONS);
  }
}

// DMFCBase::OnInputString
//
//   Event handler for when the user types a message at the console (F8) that begins with a $
//   input_string = string that was typed
void DMFCBase::OnInputString(char *input_string) {
  if (!InputCommandHandle(input_string)) {
    // The input command wasn't handled!
    mprintf((0, "DMFC Warning: Input Command '%s' wasn't handled\n", input_string));
  }
}

// DMFCBase::OnPlayerChangeTeam
//
//  Called when a player changes team
void DMFCBase::OnPlayerChangeTeam(int player_num, int newteam, bool announce, bool spew_everything) {
  player_record *pr = GetPlayerRecordByPnum(player_num);
  if (pr) {
    if (GetNumTeams() > 1 && announce && newteam != -1)
      DLLAddHUDMessage(DTXT_CHANGETEAM, pr->callsign, GetTeamString(newteam));
    pr->team = newteam;

    // only respawn them if they are not dead
    if (!((Players[player_num].flags & PLAYER_FLAGS_DYING) || (Players[player_num].flags & PLAYER_FLAGS_DEAD))) {
      RespawnPlayer(player_num, false, spew_everything);
    }
  }
}

// DMFCBase::OnClientObjectShieldsChanged
//
//	Event handler for when an objects shield's change
void DMFCBase::OnClientObjectShieldsChanged(object *obj, float amount) {}

// DMFCBase::OnWeaponFired
//
//	Event handler for when an object fires a weapon
void DMFCBase::OnWeaponFired(object *weapon_obj, object *shooter) {}

// DMFCBase::OnPlayerReconnect
//
//
//	This player is reconnecting to the game
void DMFCBase::OnPlayerReconnect(int player_num) {
  if (GetLocalRole() == LR_SERVER) {
    // restore their team
    int team = PRec_GetPlayerTeam(player_num);
    if (team != Players[player_num].team) {
      // hey! Jason hasn't set the correct team for this player
      ASSERT(player_num == 0); // this is only going to happen for the server
    }

    SendTeamAssignment(player_num, team, false);
    Players[player_num].team = team;
    CallOnPlayerChangeTeam(player_num, team, false, false);
    mprintf((0, "Reassigning (%s) to %s team\n", Players[player_num].callsign, GetTeamString(team)));
  }
}

// DMFCBase::OnPlayerConnect
//
//
//	This player is connecting to the game for the first time
void DMFCBase::OnPlayerConnect(int player_num) {
  if (GetLocalRole() == LR_SERVER) {

    // a dedicated server MUST be given a -1 team
    //------------------------------------------
    if (player_num == 0 && IAmDedicatedServer()) {
      Players[0].team = -1;
    }

    // get team assignment
    if (m_iProtectedFlags & DMFC_PRF_AUTOTEAMSELECT) {
      // JEFF: Commented out since team's are now determined right when a client connects
      //@@int team = GetTeamForNewPlayer(player_num,m_iNumTeams);
      int team = Players[player_num].team;
      SendTeamAssignment(player_num, team, false);
      //@@Players[player_num].team = team;
      CallOnPlayerChangeTeam(player_num, team, false, false);
      mprintf((0, "Assigning (%s) to %s team\n", Players[player_num].callsign, GetTeamString(team)));
    }
  }
}

// DMFCBase::OnControlMessage
//
//
//	There is a control message sent from someone
void DMFCBase::OnControlMessage(ubyte msg, int from_pnum) {
  switch (msg) {
  case CM_KICKED: {
    DLLAddHUDMessage(DTXT_KICKEDMSG);
    DisconnectMe();
  } break;
  case CM_BANNED: {
    DLLAddHUDMessage(DTXT_BANNEDMSG);
    DisconnectMe();
  } break;
  case CM_TEAMCHANGEDENIED: {
    // if we get this, the server is telling us that the request for a
    // team change has been denied
    AnnounceTeamChangeDeny(*Player_num);
  } break;
  case CM_SIGNALSTART: {
    if (from_pnum == 0) {
      // we can move!!
      WaitForServer(false);
    }
  } break;
  case CM_SIGNALWAIT: {
    if (from_pnum == 0) {
      // we got to wait
      WaitForServer(true);
    }
  } break;
  case CM_PAUSETIME: {
    ENABLE_FLAGS(m_iProtectedFlags, DMFC_PRF_PAUSETIME);
  } break;
  case CM_UNPAUSETIME: {
    DISABLE_FLAGS(m_iProtectedFlags, DMFC_PRF_PAUSETIME);
  } break;
  };
}

// DMFCBase::OnAllowObserverChange
//
//
//	returns true if the requested change for observer mode should go through
bool DMFCBase::OnAllowObserverChange(bool turnonobserver) { return true; }

// DMFCBase::OnClientShowUI
//
//
//	The game is saying it's ok to do any UI
void DMFCBase::OnClientShowUI(int id, void *user_data) {
  switch (id) {
  case UIID_TEAMPLACEMENT: {
    bool clients_wait;
    bool call_from_game;
    ubyte d = *(ubyte *)user_data;

    clients_wait = (d & 0x01) ? true : false;
    call_from_game = (d & 0x02) ? false : true;

    DoDMFCUITeamPlacement(clients_wait, call_from_game);
  } break;
  case UIID_WAITFORPLAYERS: {
    DoDMFCUIWaitForPlayers(true);
  } break;
  };
}

// DMFCBase::OnPrintScores
//
//
//	The user is requesting that the scores be printed out (For Dedicated Server use).  Use DPrintf
//	level:	-1	Requesting all the available scores
//			n	Print only the top n scores
//	Override this to how you see fit, but it should conform to the above.
void DMFCBase::OnPrintScores(int level) {}

// DMFCBase::OnGetHudCallSignColor
//
//	This is an event sent in by the game requesting what color it should draw the HUD callsign
//	of the passed in playernum.  Using GR_RGB return the color from the function.  This function
//	gets called every frame that the player is on the clients HUD.
ddgr_color DMFCBase::OnGetHudCallSignColor(int playernum) {
  if (!CheckPlayerNum(playernum))
    return GR_RGB(255, 255, 255);

  if (GetNumTeams() <= 1)
    return GR_RGB(0, 255, 0);

  return GetTeamColor(GetPlayerTeam(playernum));
}

// DMFCBase::OnTeamChangeName
//
//	This event occurs on the client when a team's name has just changed
void DMFCBase::OnTeamChangeName(int team, char *oldname, char *newname) {
  char names[DLLMAX_TEAMS][MAX_TEAMNAME_LEN];
  strcpy(names[RED_TEAM], GetTeamString(RED_TEAM));
  strcpy(names[BLUE_TEAM], GetTeamString(BLUE_TEAM));
  if (TeamMenuItem) {
    switch (m_iNumTeams) {
    case 2:
      TeamMenuItem->SetStateItemList(m_iNumTeams, names[RED_TEAM], names[BLUE_TEAM]);
      break;
    case 3:
      strcpy(names[GREEN_TEAM], GetTeamString(GREEN_TEAM));
      TeamMenuItem->SetStateItemList(m_iNumTeams, names[RED_TEAM], names[BLUE_TEAM], names[GREEN_TEAM]);
      break;
    case 4:
      strcpy(names[GREEN_TEAM], GetTeamString(GREEN_TEAM));
      strcpy(names[YELLOW_TEAM], GetTeamString(YELLOW_TEAM));
      TeamMenuItem->SetStateItemList(m_iNumTeams, names[RED_TEAM], names[BLUE_TEAM], names[GREEN_TEAM],
                                     names[YELLOW_TEAM]);
      break;
    }
  }
}

// DMFCBase::OnDoControls
//
//	This event occurs when there is a new back of game controls data
void DMFCBase::OnDoControls(game_controls *controls) {
  memcpy(&Last_game_controls, controls, sizeof(Last_game_controls));
}

// DMFCBase::OnClientWallCollide
//
//	Called by the game when their is a collision between an object and a wall
void DMFCBase::OnClientWallCollide(object *obj, float hitspeed, int hitseg, int hitwall, vector *hitpt,
                                   vector *wall_normal, float hit_dot) {}

//	DMFCBase::OnClientObjectKilled
//
//	Called when an object is being killed
//	Not automatically sent to clients (OnClientObjectKilled)
//	killer might not be valid (NULL)
void DMFCBase::OnClientObjectKilled(object *obj, object *killer) {}

//	DMFCBase::OnClientObjectDestroyed
//
//	Called when an object is about to be deleted
//	Not automatically sent to clients
void DMFCBase::OnClientObjectDestroyed(object *obj) {}

// DMFCBase::OnPlayAudioTaunt
//
//	This event occurs when a player plays an audio taunt
void DMFCBase::OnPlayAudioTaunt(int pnum) {
  mprintf((0, "%s plays an audio taunt\n", Players[pnum].callsign));
  SetPlayerTauntIndicator(pnum);
}

//	DMFCBase::OnGetTokenString
//
//	This event occurs when a text macro is being used that has tokens
//	in it, if you have a specific token, override this event to handle it.
void DMFCBase::OnGetTokenString(char *src, char *dest, int dest_size) {
  *dest = '\0';

  if (!stricmp(src, "e")) {
    // energy
    char buffer[64];
    int mypnum = GetPlayerNum();
    snprintf(buffer, sizeof(buffer), "%.0f", Players[mypnum].energy);
    strncpy(dest, buffer, dest_size - 1);
    dest[dest_size - 1] = '\0';
    return;
  }

  if (!stricmp(src, "s")) {
    // shields
    char buffer[64];
    int mypnum = GetPlayerNum();
    snprintf(buffer, sizeof(buffer), "%.0f", Objects[Players[mypnum].objnum].shields);
    strncpy(dest, buffer, dest_size - 1);
    dest[dest_size - 1] = '\0';
    return;
  }
}
