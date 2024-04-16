/*
 * $Logfile: /DescentIII/Main/Dmfc/dmfcserver.cpp $
 * $Revision: 1.1.1.1 $
 * $Date: 2003/08/26 03:57:21 $
 * $Author: kevinb $
 *
 * DMFC Server event handlers
 *
 * $Log: dmfcserver.cpp,v $
 * Revision 1.1.1.1  2003/08/26 03:57:21  kevinb
 * initial 1.5 import
 *
 *
 * 29    7/09/99 6:18p Jeff
 * added $remoteadminlogout and $wait commands
 *
 * 28    7/09/99 2:54p Jeff
 * handle gametime better (pause it when needed) if the server is 'waiting
 * for players'
 *
 * 27    7/08/99 6:25p Jeff
 * remote admin in and working
 *
 * 26    5/08/99 4:31a Jeff
 * fixed sequencing bug where clients never got a level end event for the
 * multiplayer games
 *
 * 25    5/04/99 8:46p Jeff
 * display icon when someone plays an audio taunt
 *
 * 24    3/22/99 5:51p Matt
 * Removed some includes from one file and added some to other files, to
 * reduce the amount of rebuilding when headers change.
 *
 * 23    3/17/99 12:25p Jeff
 * converted DMFC to be COM interface
 *
 * 22    2/11/99 12:51a Jeff
 * changed names of exported variables
 *
 * 21    2/07/99 1:20a Jeff
 * added new multiplayer game events EVT_GAMEOBJKILLED and
 * EVT_GAMEOBJDESTROYED
 *
 * 20    1/17/99 11:52p Jeff
 * added some new events, and changed a couple event handlers
 *
 * 19    1/15/99 3:54a Jeff
 * exported a couple more functions.  Added event handlers for Weapon
 * collide event
 *
 * 18    11/16/98 5:35p Jeff
 * removed log functions, added support for changing team names, fixed ctf
 * to work better with different team names
 *
 * 17    10/23/98 11:22a Jeff
 * changes to handle mixcase, and display the client netgame info
 * correctly
 *
 * 16    10/18/98 7:59p Jeff
 * functions added to dmfc for client->server objnum matching.  Banning
 * now uses tracker id when available.
 *
 * 15    10/15/98 6:18p Jeff
 * created the is player banned event, removed prejoin event
 *
 * 14    10/15/98 1:34p Jeff
 * added scrollable onscreen menu.  Remove ban in dmfc.  prejoin event
 *
 * 13    10/01/98 11:30a Jeff
 * made the observer mode events into just a client event
 *
 * 12    9/30/98 3:50p Jeff
 * general improvements (many)
 *
 * 11    9/21/98 7:11p Jeff
 * made InputCommand interface API and moved existing input commands to
 * the interface.  Changed mprintf/ASSERT so they are valid in DMFC
 *
 * $NoKeywords: $
 */

#include "gamedll_header.h"
#include "DMFC.h"
#include "dmfcinternal.h"
#include "d3events.h"

extern char **DMFCStringTable;
extern int DMFCStringTableSize;
extern char *_DMFCErrorString;
extern DMFCBase *basethis;
char *DMFCGetString(int d);

//###################Server Only functions################################

// DMFCBase::OnServerPlayerKilled
//
//  Event handler for when a player gets killed, either by another player or some other way
//  killer_obj = object pointer to the object that killed the player
//  victim_pnum = player number of the player that got killed
void DMFCBase::OnServerPlayerKilled(object *killer_obj, int victim_pnum) {
  if (Data->iParam == 255)
    Data->iParam = -1;
  else {
    Data->iParam = (int)DLLMultiGetMatchChecksum(OBJ_WEAPON, Data->iParam);
    if (Data->iParam == -1) {
      mprintf((0, "Unable to generate checksum for weapon id\n"));
    }
  }

  // send this event to the clients so they can handle it
  CallClientEvent(EVT_CLIENT_GAMEPLAYERKILLED, GetMeObjNum(), GetItObjNum(), -1, true);
  CallOnClientPlayerKilled(killer_obj, victim_pnum);
}

// DMFCBase::OnServerPlayerExploded
//
//  Event handler for when a player explodes.  Gets called after a EVT_GAMEPLAYERKILLED event.
//  player_num = player number of the player exploding
void DMFCBase::OnServerPlayerExploded(int player_num) {}

// DMFCBase::OnServerCollide
//
//   Event handler for when two objects collide.  At least one of the two objects is a player or a robot.
//   Be aware that this event will be called twice per collision, the second time it is called the me and
//   it objects will be flipped.
//   me_obj = object pointer to the me object
//   it_obj = object pointer to the it object
void DMFCBase::OnServerCollide(object *me_obj, object *it_obj) {}
void DMFCBase::OnServerCollide(object *me_obj, object *it_obj, vector *point, vector *normal) {}

// DMFCBase::OnServerPlayerChangeSegment
//
//   Event handler for when a player changes rooms or a "large" terrain cell
//   (8x8 Terrain block).
//   player_num = player number of the player who just changed the room/segment
//   newseg = New room/cell location
void DMFCBase::OnServerPlayerChangeSegment(int player_num, int newseg, int oldseg) {}

// DMFCBase::OnServerObjectChangeSegment (Only called if server tells client to execute)
//
//   Event handler for when a player changes rooms or a "large" terrain cell
//   (8x8 Terrain block).
//   obj = Object pointer of the object who just changed the room/segment
//   newseg = New room/cell location
//	 oldseg = Old room/cell location
void DMFCBase::OnServerObjectChangeSegment(object *obj, int newseg, int oldseg) {}

// DMFCBase::OnServerPlayerEntersGame
//
//   Event handler for when a player enters the game.  This will only get called once per
//   player, it usually gets called right after they connect to the server to start playing.
//   player_num = player number of the player entering the game
void DMFCBase::OnServerPlayerEntersGame(int player_num) {
  if (player_num == -1)
    return;

  VersionCheck(player_num);

  // see if they are banned, if so, give em the boot
  if (IsPlayerBanned(player_num)) {
    SendControlMessageToPlayer(player_num, CM_BANNED);
    DLLMultiDisconnectPlayer(player_num);
    return;
  }

  SendDMFCGameInfo(player_num);

  SendNetGameInfoSync(player_num);

  // Since clients won't ever get the OnClientLevelStart, send it here so they get it before PlayerEntersGame
  CallClientEvent(EVT_CLIENT_GAMELEVELSTART, GetMeObjNum(), GetItObjNum(), player_num);

  CallClientEvent(EVT_CLIENT_GAMEPLAYERENTERSGAME, GetMeObjNum(), GetItObjNum(), -1);
  CallOnClientPlayerEntersGame(player_num);
}

// DMFCBase::OnServerPlayerDisconnect
//
//   Event handler for when a player disconnects from the server.
//   player_num = player number of the player that just disconnected
void DMFCBase::OnServerPlayerDisconnect(int player_num) {
  if (player_num == -1)
    return;

  // send this event to the clients so they can handle it
  CallClientEvent(EVT_CLIENT_GAMEPLAYERDISCONNECT, GetMeObjNum(), GetItObjNum(), -1);
  CallOnClientPlayerDisconnect(player_num);
}

// DMFCBase::OnServerGameCreated
//
//   Event handler when the server's game first gets started.  This will only be called once
//   while the server is running, and that is when the server first starts up.
void DMFCBase::OnServerGameCreated(void) {}

// DMFCBase::OnServerLevelChange
//
//   Event handler when the server changes levels.  This will get called after a level ends and a server
//   is starting a new level.
void DMFCBase::OnServerLevelChange(void) {}

// DMFCBase::OnServerLevelStart
//
//   Event handler for when a multiplayer level is starting up.  This will get called right before the level
//   starts.
void DMFCBase::OnServerLevelStart(void) {
  RealGametime = 0.0f; // zero out real gametime

  // Let the server get the OnLevelStart
  CallOnClientLevelStart();

  // send a fake EVT_GAMEPLAYERENTERSGAME to the server so he gets that event
  Data->me_handle = Data->it_handle = Objects[Players[GetPlayerNum()].objnum].handle;
  Data->it_handle = Data->me_handle;
  CallOnServerPlayerEntersGame(GetPlayerNum());

  DisplayTauntIndicator = false;

  // If we are a dedicated server, and there is a wait time make everyone wait
  if (GetLocalRole() == LR_SERVER && IAmDedicatedServer()) {
    if (DedicatedLevelWait > 0) {
      // time to stop waiting!
      mprintf((0, "Telling clients to wait\n"));
      DPrintf("\nMaking Clients Wait %.2f seconds\n", DedicatedLevelWait);
      PauseRealGameTime(true);
      MakeClientsWait(true);
    }
  }
}

// DMFCBase::OnServerLevelEnd
//
//   Event handler for when a multiplayer level is ending.
void DMFCBase::OnServerLevelEnd(void) {
  // send this event to the clients so they can handle it
  // JEFF - since Jason mucked with sequencing, all clients at this point are
  // at some weird sequence, so we can't send packets to them
  // CallClientEvent(EVT_CLIENT_GAMELEVELEND,GetMeObjNum(),GetItObjNum(),-1);
  // CallOnClientLevelEnd();
}

// DMFCBase::OnServerObjectShieldsChanged
//
//	Event handler for when an objects shield's change
void DMFCBase::OnServerObjectShieldsChanged(object *obj, float amount) {}

// DMFCBase::OnServerIsAddressBanned
//
//	Called by the game to determine if the given network address is banned from the game
bool DMFCBase::OnServerIsAddressBanned(network_address *addr, char *tracker_id) {
  return IsAddressBanned(addr, tracker_id);
}

// DMFCBase::OnServerWallCollide
//
//	Called by the game when their is a collision between an object and a wall
void DMFCBase::OnServerWallCollide(object *obj, float hitspeed, int hitseg, int hitwall, vector *hitpt,
                                   vector *wall_normal, float hit_dot) {}

//	DMFCBase::OnServerObjectKilled
//
//	Called when an object is being killed
//	Not automatically sent to clients (OnClientObjectKilled)
//	killer might not be valid (NULL)
void DMFCBase::OnServerObjectKilled(object *obj, object *killer) {}

//	DMFCBase::OnServerObjectDestroyed
//
//	Called when an object is about to be deleted
//	Not automatically sent to clients
void DMFCBase::OnServerObjectDestroyed(object *obj) {}
