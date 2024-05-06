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

 * $Logfile: /DescentIII/Main/dmfc/dmfcvirtual.cpp $
 * $Revision: 1.1.1.1 $
 * $Date: 2003/08/26 03:57:21 $
 * $Author: kevinb $
 *
 * Handles the interface for virtual calls/overriding event handlers
 *
 * $Log: dmfcvirtual.cpp,v $
 * Revision 1.1.1.1  2003/08/26 03:57:21  kevinb
 * initial 1.5 import
 *
 *
 * 10    7/13/99 10:05a Jeff
 * text taunt token decoding
 *
 * 9     7/08/99 9:55p Jeff
 * added event handler for weapon fired event
 *
 * 8     5/10/99 2:44a Jeff
 * handle new scheme of player's joining in a team game, where the team is
 * set before player enters game in the main code, but the team is
 * determined via event call to dmfc
 *
 * 7     5/08/99 4:31a Jeff
 * fixed sequencing bug where clients never got a level end event for the
 * multiplayer games
 *
 * 6     4/14/99 1:15a Jeff
 * fixed case mismatched #includes
 *
 * 5     3/24/99 1:41p Jeff
 * some dedicated server fixups...ability to set number of teams
 *
 * 4     3/22/99 6:21p Jeff
 * added 2 more audio taunts.  a mulitplayer event when someone plays an
 * audio taunt.  option to disable audio taunts.
 *
 * 3     3/22/99 5:51p Matt
 * Removed some includes from one file and added some to other files, to
 * reduce the amount of rebuilding when headers change.
 *
 * 2     3/17/99 12:25p Jeff
 * converted DMFC to be COM interface
 *
 * 1     3/17/99 11:54a Jeff
 *
 * $NoKeywords: $
 */

#include "DMFC.h"
#include "dmfcinternal.h"
#include "d3events.h"

//	DMFCBase::InitializeVirtualHandles
//		Prepares the virtual handles
void DMFCBase::InitializeVirtualHandles(void) {
  UserOnServerPlayerKilled = NULL;
  UserOnServerPlayerExploded = NULL;
  UserOnServerCollide = NULL;
  UserOnServerCollide2 = NULL;
  UserOnServerPlayerChangeSegment = NULL;
  UserOnServerObjectChangeSegment = NULL;
  UserOnServerPlayerEntersGame = NULL;
  UserOnServerPlayerDisconnect = NULL;
  UserOnServerGameCreated = NULL;
  UserOnServerLevelChange = NULL;
  UserOnServerLevelStart = NULL;
  UserOnServerLevelEnd = NULL;
  UserOnServerObjectShieldsChanged = NULL;
  UserOnServerIsAddressBanned = NULL;
  UserOnServerWallCollide = NULL;
  UserOnServerObjectKilled = NULL;
  UserOnServerObjectDestroyed = NULL;
  UserOnClientPlayerKilled = NULL;
  UserOnClientPlayerExploded = NULL;
  UserOnClientCollide = NULL;
  UserOnClientCollide2 = NULL;
  UserOnClientPlayerChangeSegment = NULL;
  UserOnClientObjectChangeSegment = NULL;
  UserOnClientPlayerEntersGame = NULL;
  UserOnClientPlayerDisconnect = NULL;
  UserOnMeDisconnectFromServer = NULL;
  UserOnClientGameCreated = NULL;
  UserOnClientObjectShieldsChanged = NULL;
  UserOnWeaponFired = NULL;
  UserOnClientLevelChange = NULL;
  UserOnClientLevelStart = NULL;
  UserOnClientLevelEnd = NULL;
  UserOnClientWallCollide = NULL;
  UserOnClientObjectKilled = NULL;
  UserOnClientObjectDestroyed = NULL;
  UserOnPlayerEntersObserver = NULL;
  UserOnPlayerExitsObserver = NULL;
  UserOnCanChangeTeam = NULL;
  UserOnSpecialPacket = NULL;
  UserOnInterval = NULL;
  UserOnHUDInterval = NULL;
  UserOnPLRInterval = NULL;
  UserOnPLRInit = NULL;
  UserOnKeypress = NULL;
  UserOnInputString = NULL;
  UserOnPlayerChangeTeam = NULL;
  UserOnGameStateRequest = NULL;
  UserOnSaveStatsToFile = NULL;
  UserOnPlayerReconnect = NULL;
  UserOnPlayerConnect = NULL;
  UserOnControlMessage = NULL;
  UserOnAllowObserverChange = NULL;
  UserOnClientShowUI = NULL;
  UserOnPrintScores = NULL;
  UserOnDisconnectSaveStatsToFile = NULL;
  UserOnLevelEndSaveStatsToFile = NULL;
  UserOnGetHudCallSignColor = NULL;
  UserOnTeamChangeName = NULL;
  UserOnDoControls = NULL;
  UserOnPlayAudioTaunt = NULL;
  UserOnGetTokenString = NULL;
}

void DMFCBase::Set_OnServerPlayerKilled(void (*callback)(object *killer_obj, int victim_pnum)) {
  UserOnServerPlayerKilled = callback;
}

void DMFCBase::Set_OnServerPlayerExploded(void (*callback)(int player_num)) { UserOnServerPlayerExploded = callback; }

void DMFCBase::Set_OnServerCollide(void (*callback)(object *me_obj, object *it_obj)) { UserOnServerCollide = callback; }

void DMFCBase::Set_OnServerCollide(void (*callback)(object *me_obj, object *it_obj, vector *point, vector *normal)) {
  UserOnServerCollide2 = callback;
}

void DMFCBase::Set_OnServerPlayerChangeSegment(void (*callback)(int player_num, int newseg, int oldseg)) {
  UserOnServerPlayerChangeSegment = callback;
}

void DMFCBase::Set_OnServerObjectChangeSegment(void (*callback)(object *obj, int newseg, int oldseg)) {
  UserOnServerObjectChangeSegment = callback;
}

void DMFCBase::Set_OnServerPlayerEntersGame(void (*callback)(int player_num)) {
  UserOnServerPlayerEntersGame = callback;
}

void DMFCBase::Set_OnServerPlayerDisconnect(void (*callback)(int player_num)) {
  UserOnServerPlayerDisconnect = callback;
}

void DMFCBase::Set_OnServerGameCreated(void (*callback)(void)) { UserOnServerGameCreated = callback; }

void DMFCBase::Set_OnServerLevelChange(void (*callback)(void)) { UserOnServerLevelChange = callback; }

void DMFCBase::Set_OnServerLevelStart(void (*callback)(void)) { UserOnServerLevelStart = callback; }

void DMFCBase::Set_OnServerLevelEnd(void (*callback)(void)) { UserOnServerLevelEnd = callback; }

void DMFCBase::Set_OnServerObjectShieldsChanged(void (*callback)(object *obj, float amount)) {
  UserOnServerObjectShieldsChanged = callback;
}

void DMFCBase::Set_OnServerIsAddressBanned(bool (*callback)(network_address *addr, const char *tracker_id)) {
  UserOnServerIsAddressBanned = callback;
}

void DMFCBase::Set_OnServerWallCollide(void (*callback)(object *obj, float hitspeed, int hitseg, int hitwall,
                                                        vector *hitpt, vector *wall_normal, float hit_dot)) {
  UserOnServerWallCollide = callback;
}

void DMFCBase::Set_OnServerObjectKilled(void (*callback)(object *obj, object *killer)) {
  UserOnServerObjectKilled = callback;
}

void DMFCBase::Set_OnServerObjectDestroyed(void (*callback)(object *obj)) { UserOnServerObjectDestroyed = callback; }

void DMFCBase::Set_OnClientPlayerKilled(void (*callback)(object *killer_obj, int victim_pnum)) {
  UserOnClientPlayerKilled = callback;
}

void DMFCBase::Set_OnClientPlayerExploded(void (*callback)(int player_num)) { UserOnClientPlayerExploded = callback; }

void DMFCBase::Set_OnClientCollide(void (*callback)(object *me_obj, object *it_obj)) { UserOnClientCollide = callback; }

void DMFCBase::Set_OnClientCollide(void (*callback)(object *me_obj, object *it_obj, vector *point, vector *normal)) {
  UserOnClientCollide2 = callback;
}

void DMFCBase::Set_OnClientPlayerChangeSegment(void (*callback)(int player_num, int newseg, int oldseg)) {
  UserOnClientPlayerChangeSegment = callback;
}

void DMFCBase::Set_OnClientObjectChangeSegment(void (*callback)(object *obj, int newseg, int oldseg)) {
  UserOnClientObjectChangeSegment = callback;
}

void DMFCBase::Set_OnClientPlayerEntersGame(void (*callback)(int player_num)) {
  UserOnClientPlayerEntersGame = callback;
}

void DMFCBase::Set_OnClientPlayerDisconnect(void (*callback)(int player_num)) {
  UserOnClientPlayerDisconnect = callback;
}

void DMFCBase::Set_OnMeDisconnectFromServer(void (*callback)(void)) { UserOnMeDisconnectFromServer = callback; }

void DMFCBase::Set_OnClientGameCreated(void (*callback)(void)) { UserOnClientGameCreated = callback; }

void DMFCBase::Set_OnClientObjectShieldsChanged(void (*callback)(object *obj, float amount)) {
  UserOnClientObjectShieldsChanged = callback;
}

void DMFCBase::Set_OnWeaponFired(void (*callback)(object *weapon_obj, object *shooter)) {
  UserOnWeaponFired = callback;
}

void DMFCBase::Set_OnClientLevelChange(void (*callback)(void)) { UserOnClientLevelChange = callback; }

void DMFCBase::Set_OnClientLevelStart(void (*callback)(void)) { UserOnClientLevelStart = callback; }

void DMFCBase::Set_OnClientLevelEnd(void (*callback)(void)) { UserOnClientLevelEnd = callback; }

void DMFCBase::Set_OnClientWallCollide(void (*callback)(object *obj, float hitspeed, int hitseg, int hitwall,
                                                        vector *hitpt, vector *wall_normal, float hit_dot)) {
  UserOnClientWallCollide = callback;
}

void DMFCBase::Set_OnClientObjectKilled(void (*callback)(object *obj, object *killer)) {
  UserOnClientObjectKilled = callback;
}

void DMFCBase::Set_OnClientObjectDestroyed(void (*callback)(object *obj)) { UserOnClientObjectDestroyed = callback; }

void DMFCBase::Set_OnPlayerEntersObserver(void (*callback)(int pnum, object *piggy)) {
  UserOnPlayerEntersObserver = callback;
}

void DMFCBase::Set_OnPlayerExitsObserver(void (*callback)(int pnum)) { UserOnPlayerExitsObserver = callback; }

void DMFCBase::Set_OnCanChangeTeam(bool (*callback)(int pnum, int newteam)) { UserOnCanChangeTeam = callback; }

void DMFCBase::Set_OnSpecialPacket(void (*callback)(void)) { UserOnSpecialPacket = callback; }

void DMFCBase::Set_OnInterval(void (*callback)(void)) { UserOnInterval = callback; }

void DMFCBase::Set_OnHUDInterval(void (*callback)(void)) { UserOnHUDInterval = callback; }

void DMFCBase::Set_OnPLRInterval(void (*callback)(void)) { UserOnPLRInterval = callback; }

void DMFCBase::Set_OnPLRInit(void (*callback)(void)) { UserOnPLRInit = callback; }

void DMFCBase::Set_OnKeypress(void (*callback)(int key)) { UserOnKeypress = callback; }

void DMFCBase::Set_OnInputString(void (*callback)(char *input_string)) { UserOnInputString = callback; }

void DMFCBase::Set_OnPlayerChangeTeam(void (*callback)(int player_num, int newteam, bool announce,
                                                       bool spew_everything)) {
  UserOnPlayerChangeTeam = callback;
}

void DMFCBase::Set_OnGameStateRequest(void (*callback)(int pnum)) { UserOnGameStateRequest = callback; }

void DMFCBase::Set_OnSaveStatsToFile(void (*callback)(void)) { UserOnSaveStatsToFile = callback; }

void DMFCBase::Set_OnPlayerReconnect(void (*callback)(int player_num)) { UserOnPlayerReconnect = callback; }

void DMFCBase::Set_OnPlayerConnect(void (*callback)(int player_num)) { UserOnPlayerConnect = callback; }

void DMFCBase::Set_OnControlMessage(void (*callback)(ubyte msg, int from_pnum)) { UserOnControlMessage = callback; }

void DMFCBase::Set_OnAllowObserverChange(bool (*callback)(bool turnonobserver)) {
  UserOnAllowObserverChange = callback;
}

void DMFCBase::Set_OnClientShowUI(void (*callback)(int id, void *user_data)) { UserOnClientShowUI = callback; }

void DMFCBase::Set_OnPrintScores(void (*callback)(int level)) { UserOnPrintScores = callback; }

void DMFCBase::Set_OnDisconnectSaveStatsToFile(void (*callback)(void)) { UserOnDisconnectSaveStatsToFile = callback; }

void DMFCBase::Set_OnLevelEndSaveStatsToFile(void (*callback)(void)) { UserOnLevelEndSaveStatsToFile = callback; }

void DMFCBase::Set_OnGetHudCallSignColor(ddgr_color (*callback)(int playernum)) {
  UserOnGetHudCallSignColor = callback;
}

void DMFCBase::Set_OnTeamChangeName(void (*callback)(int team, char *oldname, char *newname)) {
  UserOnTeamChangeName = callback;
}

void DMFCBase::Set_OnDoControls(void (*callback)(game_controls *controls)) { UserOnDoControls = callback; }

void DMFCBase::Set_OnPlayAudioTaunt(void (*callback)(int pnum)) { UserOnPlayAudioTaunt = callback; }

void DMFCBase::Set_OnGetTokenString(void (*callback)(char *src, char *dest, int dest_size)) {
  UserOnGetTokenString = callback;
}
///////////////////////////////////////////////////////

void DMFCBase::CallOnServerPlayerKilled(object *killer_obj, int victim_pnum) {
  if (UserOnServerPlayerKilled)
    (*UserOnServerPlayerKilled)(killer_obj, victim_pnum);
  else
    OnServerPlayerKilled(killer_obj, victim_pnum);
}

void DMFCBase::CallOnServerPlayerExploded(int player_num) {
  if (UserOnServerPlayerExploded)
    (*UserOnServerPlayerExploded)(player_num);
  else
    OnServerPlayerExploded(player_num);
}

void DMFCBase::CallOnServerCollide(object *me_obj, object *it_obj) {
  if (UserOnServerCollide)
    (*UserOnServerCollide)(me_obj, it_obj);
  else
    OnServerCollide(me_obj, it_obj);
}

void DMFCBase::CallOnServerCollide(object *me_obj, object *it_obj, vector *point, vector *normal) {
  if (UserOnServerCollide2)
    (*UserOnServerCollide2)(me_obj, it_obj, point, normal);
  else
    OnServerCollide(me_obj, it_obj, point, normal);
}

void DMFCBase::CallOnServerPlayerChangeSegment(int player_num, int newseg, int oldseg) {
  if (UserOnServerPlayerChangeSegment)
    (*UserOnServerPlayerChangeSegment)(player_num, newseg, oldseg);
  else
    OnServerPlayerChangeSegment(player_num, newseg, oldseg);
}

void DMFCBase::CallOnServerObjectChangeSegment(object *obj, int newseg, int oldseg) {
  if (UserOnServerObjectChangeSegment)
    (*UserOnServerObjectChangeSegment)(obj, newseg, oldseg);
  else
    OnServerObjectChangeSegment(obj, newseg, oldseg);
}

void DMFCBase::CallOnServerPlayerEntersGame(int player_num) {
  if (UserOnServerPlayerEntersGame)
    (*UserOnServerPlayerEntersGame)(player_num);
  else
    OnServerPlayerEntersGame(player_num);
}

void DMFCBase::CallOnServerPlayerDisconnect(int player_num) {
  if (UserOnServerPlayerDisconnect)
    (*UserOnServerPlayerDisconnect)(player_num);
  else
    OnServerPlayerDisconnect(player_num);
}

void DMFCBase::CallOnServerGameCreated(void) {
  if (UserOnServerGameCreated)
    (*UserOnServerGameCreated)();
  else
    OnServerGameCreated();
}

void DMFCBase::CallOnServerLevelChange(void) {
  if (UserOnServerLevelChange)
    (*UserOnServerLevelChange)();
  else
    OnServerLevelChange();
}

void DMFCBase::CallOnServerLevelStart(void) {
  if (UserOnServerLevelStart)
    (*UserOnServerLevelStart)();
  else
    OnServerLevelStart();
}

void DMFCBase::CallOnServerLevelEnd(void) {
  if (UserOnServerLevelEnd)
    (*UserOnServerLevelEnd)();
  else
    OnServerLevelEnd();
}

void DMFCBase::CallOnServerObjectShieldsChanged(object *obj, float amount) {
  if (UserOnServerObjectShieldsChanged)
    (*UserOnServerObjectShieldsChanged)(obj, amount);
  else
    OnServerObjectShieldsChanged(obj, amount);
}

bool DMFCBase::CallOnServerIsAddressBanned(network_address *addr, const char *tracker_id) {
  if (UserOnServerIsAddressBanned)
    return (*UserOnServerIsAddressBanned)(addr, tracker_id);
  else
    return OnServerIsAddressBanned(addr, tracker_id);
}

void DMFCBase::CallOnServerWallCollide(object *obj, float hitspeed, int hitseg, int hitwall, vector *hitpt,
                                       vector *wall_normal, float hit_dot) {
  if (UserOnServerWallCollide)
    (*UserOnServerWallCollide)(obj, hitspeed, hitseg, hitwall, hitpt, wall_normal, hit_dot);
  else
    OnServerWallCollide(obj, hitspeed, hitseg, hitwall, hitpt, wall_normal, hit_dot);
}

void DMFCBase::CallOnServerObjectKilled(object *obj, object *killer) {
  if (UserOnServerObjectKilled)
    (*UserOnServerObjectKilled)(obj, killer);
  else
    OnServerObjectKilled(obj, killer);
}

void DMFCBase::CallOnServerObjectDestroyed(object *obj) {
  if (UserOnServerObjectDestroyed)
    (*UserOnServerObjectDestroyed)(obj);
  else
    OnServerObjectDestroyed(obj);
}

void DMFCBase::CallOnClientPlayerKilled(object *killer_obj, int victim_pnum) {
  if (UserOnClientPlayerKilled)
    (*UserOnClientPlayerKilled)(killer_obj, victim_pnum);
  else
    OnClientPlayerKilled(killer_obj, victim_pnum);
}

void DMFCBase::CallOnClientPlayerExploded(int player_num) {
  if (UserOnClientPlayerExploded)
    (*UserOnClientPlayerExploded)(player_num);
  else
    OnClientPlayerExploded(player_num);
}

void DMFCBase::CallOnClientCollide(object *me_obj, object *it_obj) {
  if (UserOnClientCollide)
    (*UserOnClientCollide)(me_obj, it_obj);
  else
    OnClientCollide(me_obj, it_obj);
}

void DMFCBase::CallOnClientCollide(object *me_obj, object *it_obj, vector *point, vector *normal) {
  if (UserOnClientCollide2)
    (*UserOnClientCollide2)(me_obj, it_obj, point, normal);
  else
    OnClientCollide(me_obj, it_obj, point, normal);
}

void DMFCBase::CallOnClientPlayerChangeSegment(int player_num, int newseg, int oldseg) {
  if (UserOnClientPlayerChangeSegment)
    (*UserOnClientPlayerChangeSegment)(player_num, newseg, oldseg);
  else
    OnClientPlayerChangeSegment(player_num, newseg, oldseg);
}

void DMFCBase::CallOnClientObjectChangeSegment(object *obj, int newseg, int oldseg) {
  if (UserOnClientObjectChangeSegment)
    (*UserOnClientObjectChangeSegment)(obj, newseg, oldseg);
  else
    OnClientObjectChangeSegment(obj, newseg, oldseg);
}

void DMFCBase::CallOnClientPlayerEntersGame(int player_num) {
  if (UserOnClientPlayerEntersGame)
    (*UserOnClientPlayerEntersGame)(player_num);
  else
    OnClientPlayerEntersGame(player_num);
}

void DMFCBase::CallOnClientPlayerDisconnect(int player_num) {
  if (UserOnClientPlayerDisconnect)
    (*UserOnClientPlayerDisconnect)(player_num);
  else
    OnClientPlayerDisconnect(player_num);
}

void DMFCBase::CallOnMeDisconnectFromServer(void) {
  if (UserOnMeDisconnectFromServer)
    (*UserOnMeDisconnectFromServer)();
  else
    OnMeDisconnectFromServer();
}

void DMFCBase::CallOnClientGameCreated(void) {
  if (UserOnClientGameCreated)
    (*UserOnClientGameCreated)();
  else
    OnClientGameCreated();
}

void DMFCBase::CallOnClientObjectShieldsChanged(object *obj, float amount) {
  if (UserOnClientObjectShieldsChanged)
    (*UserOnClientObjectShieldsChanged)(obj, amount);
  else
    OnClientObjectShieldsChanged(obj, amount);
}

void DMFCBase::CallOnWeaponFired(object *weapon_obj, object *shooter) {
  if (UserOnWeaponFired)
    (*UserOnWeaponFired)(weapon_obj, shooter);
  else
    OnWeaponFired(weapon_obj, shooter);
}

void DMFCBase::CallOnClientLevelChange(void) {
  if (UserOnClientLevelChange)
    (*UserOnClientLevelChange)();
  else
    OnClientLevelChange();
}

void DMFCBase::CallOnClientLevelStart(void) {
  if (UserOnClientLevelStart)
    (*UserOnClientLevelStart)();
  else
    OnClientLevelStart();
}

void DMFCBase::CallOnClientLevelEnd(void) {
  if (UserOnClientLevelEnd)
    (*UserOnClientLevelEnd)();
  else
    OnClientLevelEnd();
}

void DMFCBase::CallOnClientWallCollide(object *obj, float hitspeed, int hitseg, int hitwall, vector *hitpt,
                                       vector *wall_normal, float hit_dot) {
  if (UserOnClientWallCollide)
    (*UserOnClientWallCollide)(obj, hitspeed, hitseg, hitwall, hitpt, wall_normal, hit_dot);
  else
    OnClientWallCollide(obj, hitspeed, hitseg, hitwall, hitpt, wall_normal, hit_dot);
}

void DMFCBase::CallOnClientObjectKilled(object *obj, object *killer) {
  if (UserOnClientObjectKilled)
    (*UserOnClientObjectKilled)(obj, killer);
  else
    OnClientObjectKilled(obj, killer);
}

void DMFCBase::CallOnClientObjectDestroyed(object *obj) {
  if (UserOnClientObjectDestroyed)
    (*UserOnClientObjectDestroyed)(obj);
  else
    OnClientObjectDestroyed(obj);
}

void DMFCBase::CallOnPlayerEntersObserver(int pnum, object *piggy) {
  if (UserOnPlayerEntersObserver)
    (*UserOnPlayerEntersObserver)(pnum, piggy);
  else
    OnPlayerEntersObserver(pnum, piggy);
}

void DMFCBase::CallOnPlayerExitsObserver(int pnum) {
  if (UserOnPlayerExitsObserver)
    (*UserOnPlayerExitsObserver)(pnum);
  else
    OnPlayerExitsObserver(pnum);
}

bool DMFCBase::CallOnCanChangeTeam(int pnum, int newteam) {
  if (UserOnCanChangeTeam)
    return (*UserOnCanChangeTeam)(pnum, newteam);
  else
    return OnCanChangeTeam(pnum, newteam);
}

void DMFCBase::CallOnSpecialPacket(void) {
  if (UserOnSpecialPacket)
    (*UserOnSpecialPacket)();
  else
    OnSpecialPacket();
}

void DMFCBase::CallOnInterval(void) {
  if (UserOnInterval)
    (*UserOnInterval)();
  else
    OnInterval();
}

void DMFCBase::CallOnHUDInterval(void) {
  if (UserOnHUDInterval)
    (*UserOnHUDInterval)();
  else
    OnHUDInterval();
}

void DMFCBase::CallOnPLRInterval(void) {
  if (UserOnPLRInterval)
    (*UserOnPLRInterval)();
  else
    OnPLRInterval();
}

void DMFCBase::CallOnPLRInit(void) {
  if (UserOnPLRInit)
    (*UserOnPLRInit)();
  else
    OnPLRInit();
}

void DMFCBase::CallOnKeypress(int key) {
  if (UserOnKeypress)
    (*UserOnKeypress)(key);
  else
    OnKeypress(key);
}

void DMFCBase::CallOnInputString(char *input_string) {
  if (UserOnInputString)
    (*UserOnInputString)(input_string);
  else
    OnInputString(input_string);
}

void DMFCBase::CallOnPlayerChangeTeam(int player_num, int newteam, bool announce, bool spew_everything) {
  if (UserOnPlayerChangeTeam)
    (*UserOnPlayerChangeTeam)(player_num, newteam, announce, spew_everything);
  else
    OnPlayerChangeTeam(player_num, newteam, announce, spew_everything);
}

void DMFCBase::CallOnGameStateRequest(int pnum) {
  if (UserOnGameStateRequest)
    (*UserOnGameStateRequest)(pnum);
  else
    OnGameStateRequest(pnum);
}

void DMFCBase::CallOnSaveStatsToFile(void) {
  if (UserOnSaveStatsToFile)
    (*UserOnSaveStatsToFile)();
  else
    OnSaveStatsToFile();
}

void DMFCBase::CallOnPlayerReconnect(int player_num) {
  if (UserOnPlayerReconnect)
    (*UserOnPlayerReconnect)(player_num);
  else
    OnPlayerReconnect(player_num);
}

void DMFCBase::CallOnPlayerConnect(int player_num) {
  if (UserOnPlayerConnect)
    (*UserOnPlayerConnect)(player_num);
  else
    OnPlayerConnect(player_num);
}

void DMFCBase::CallOnControlMessage(ubyte msg, int from_pnum) {
  if (UserOnControlMessage)
    (*UserOnControlMessage)(msg, from_pnum);
  else
    OnControlMessage(msg, from_pnum);
}

bool DMFCBase::CallOnAllowObserverChange(bool turnonobserver) {
  if (UserOnAllowObserverChange)
    return (*UserOnAllowObserverChange)(turnonobserver);
  else
    return OnAllowObserverChange(turnonobserver);
}

void DMFCBase::CallOnClientShowUI(int id, void *user_data) {
  if (UserOnClientShowUI)
    (*UserOnClientShowUI)(id, user_data);
  else
    OnClientShowUI(id, user_data);
}

void DMFCBase::CallOnPrintScores(int level) {
  if (UserOnPrintScores)
    (*UserOnPrintScores)(level);
  else
    OnPrintScores(level);
}

void DMFCBase::CallOnDisconnectSaveStatsToFile(void) {
  if (UserOnDisconnectSaveStatsToFile)
    (*UserOnDisconnectSaveStatsToFile)();
  else
    OnDisconnectSaveStatsToFile();
}

void DMFCBase::CallOnLevelEndSaveStatsToFile(void) {
  if (UserOnLevelEndSaveStatsToFile)
    (*UserOnLevelEndSaveStatsToFile)();
  else
    OnLevelEndSaveStatsToFile();
}

ddgr_color DMFCBase::CallOnGetHudCallSignColor(int playernum) {
  if (UserOnGetHudCallSignColor)
    return (*UserOnGetHudCallSignColor)(playernum);
  else
    return OnGetHudCallSignColor(playernum);
}

void DMFCBase::CallOnTeamChangeName(int team, char *oldname, char *newname) {
  if (UserOnTeamChangeName)
    (*UserOnTeamChangeName)(team, oldname, newname);
  else
    OnTeamChangeName(team, oldname, newname);
}

void DMFCBase::CallOnDoControls(game_controls *controls) {
  if (UserOnDoControls)
    (*UserOnDoControls)(controls);
  else
    OnDoControls(controls);
}

void DMFCBase::CallOnPlayAudioTaunt(int pnum) {
  if (UserOnPlayAudioTaunt)
    (*UserOnPlayAudioTaunt)(pnum);
  else
    OnPlayAudioTaunt(pnum);
}

void DMFCBase::CallOnGetTokenString(char *src, char *dest, int dest_size) {
  if (UserOnGetTokenString)
    (*UserOnGetTokenString)(src, dest, dest_size);
  else
    OnGetTokenString(src, dest, dest_size);
}

// DMFCBase::TranslateEvent
//
//   Translates the event passed in to handle, calls the appropriate handler function.  If a function isn't
//   created to handle the event, then it calls the default handler.  Handler functions should call the default
//   handler unless it absolutly shouldn't
void DMFCBase::TranslateEvent(int eventnum, dllinfo *data) {
  // set the Data pointer to point to the current event's data
  Data = data;

  // extract this info out here since it has to be done through most of the events
  int me_objnum;
  int it_objnum;
  object *me_objp;
  object *it_objp;

  if (data->me_handle > OBJECT_HANDLE_NONE) {
    me_objnum = data->me_handle & HANDLE_OBJNUM_MASK;
    me_objp = &Objects[me_objnum];
  } else {
    me_objnum = -1;
    me_objp = NULL;
  }

  if (data->it_handle > OBJECT_HANDLE_NONE) {
    it_objnum = data->it_handle & HANDLE_OBJNUM_MASK;
    it_objp = &Objects[it_objnum];
  } else {
    it_objnum = -1;
    it_objp = NULL;
  }

  // handle the event
  switch (eventnum) {
  case EVT_GAMEPLAYERKILLED: {
    CallOnServerPlayerKilled(it_objp, (me_objp) ? me_objp->id : -1);
  } break;

  case EVT_GAMEPLAYEREXPLODED: {
    CallOnServerPlayerExploded((me_objp) ? me_objp->id : -1);
  } break;

  case EVT_GAMECOLLIDE: {
    CallOnServerCollide(me_objp, it_objp);
    CallOnServerCollide(me_objp, it_objp, &data->collide_info.point, &data->collide_info.normal);
  } break;

  case EVT_GAMEPLAYERCHANGESEG: {
    CallOnServerPlayerChangeSegment((me_objp) ? me_objp->id : -1, data->newseg, data->oldseg);
  } break;

  case EVT_GAMEOBJCHANGESEG: {
    CallOnServerObjectChangeSegment(me_objp, data->newseg, data->oldseg);
  } break;

  case EVT_GAMEPLAYERENTERSGAME: {
    CallOnServerPlayerEntersGame((me_objp) ? me_objp->id : -1);
  } break;

  case EVT_GAMEPLAYERDISCONNECT: {
    CallOnServerPlayerDisconnect((me_objp) ? me_objp->id : -1);
  } break;

  case EVT_GAMECREATED: {
    CallOnServerGameCreated();
  } break;

  case EVT_GAMELEVELCHANGE: {
    CallOnServerLevelChange();
  } break;

  case EVT_GAMELEVELSTART: {
    CallOnServerLevelStart();
  } break;

  case EVT_GAMEOBJECTSHIELDSCHANGED: {
    CallOnServerObjectShieldsChanged(me_objp, data->fParam);
  } break;

  case EVT_GAMEOBJDESTROYED: {
    CallOnServerObjectDestroyed(me_objp);
  } break;

  case EVT_GAMEOBJKILLED: {
    CallOnServerObjectKilled(me_objp, it_objp);
  } break;

  case EVT_CLIENT_GAMELEVELEND: {
    if (GetLocalRole() == LR_SERVER)
      CallOnServerLevelEnd();
    CallOnClientLevelEnd();
  } break;

  case EVT_CLIENT_GAMEPLAYERKILLED: {
    CallOnClientPlayerKilled(it_objp, (me_objp) ? me_objp->id : -1);
  } break;

  case EVT_CLIENT_GAMEPLAYEREXPLODED: {
    CallOnClientPlayerExploded((me_objp) ? me_objp->id : -1);
  } break;

  case EVT_CLIENT_GAMECOLLIDE: {
    CallOnClientCollide(me_objp, it_objp);
    CallOnClientCollide(me_objp, it_objp, &data->collide_info.point, &data->collide_info.normal);
  } break;

  case EVT_CLIENT_GAMEPLAYERCHANGESEG: {
    CallOnClientPlayerChangeSegment((me_objp) ? me_objp->id : -1, data->newseg, data->oldseg);
  } break;

  case EVT_CLIENT_GAMEOBJCHANGESEG: {
    CallOnClientObjectChangeSegment(me_objp, data->newseg, data->oldseg);
  } break;

  case EVT_CLIENT_GAMEPLAYERENTERSGAME: {
    CallOnClientPlayerEntersGame((me_objp) ? me_objp->id : -1);
  } break;

  case EVT_CLIENT_GAMEPLAYERDISCONNECT: {
    CallOnClientPlayerDisconnect((me_objp) ? me_objp->id : -1);
  } break;

  case EVT_CLIENT_GAMECREATED: {
    CallOnClientGameCreated();
  } break;

  case EVT_CLIENT_GAMELEVELCHANGE: {
    CallOnClientLevelChange();
  } break;

  case EVT_CLIENT_GAMESPECIALPACKET: {
    CallOnSpecialPacket();
  } break;

  case EVT_CLIENT_HUD_INTERVAL: {
    CallOnHUDInterval();
  } break;

  case EVT_GAME_INTERVAL: {
    CallOnInterval();
  } break;

  case EVT_CLIENT_KEYPRESS: {
    CallOnKeypress(data->input_key);
  } break;

  case EVT_CLIENT_INPUT_STRING: {
    CallOnInputString(data->input_string);
  } break;

  case EVT_CLIENT_GAMELEVELSTART: {
    CallOnClientLevelStart();
  } break;

  case EVT_CLIENT_GAMEPOSTLEVELRESULTS: {
    CallOnPLRInterval();
  } break;

  case EVT_CLIENT_GAMEOBJECTSHIELDSCHANGED: {
    CallOnClientObjectShieldsChanged(me_objp, data->fParam);
  } break;

  case EVT_WEAPONFIRED: {
    CallOnWeaponFired(me_objp, it_objp);
  } break;

  case EVT_CLIENT_GAMEPLAYERENTERSOBSERVER:
    if (me_objp) {
      CallOnPlayerEntersObserver(me_objp->id, it_objp);
    }
    break;

  case EVT_CLIENT_GAMEPLAYEREXITSOBSERVER:
    if (me_objp) {
      CallOnPlayerExitsObserver(me_objp->id);
    }
    break;

  case EVT_CLIENT_GETCOLOREDNAME: {
    ddgr_color color = GR_RGB(255, 255, 255);

    if (me_objp) {
      if (ShouldIDisplayHUDName(me_objp->id)) {
        // ok we should display the hud name
        color = CallOnGetHudCallSignColor(me_objp->id);

      } else {
        // don't display HUD name
        color = -1;
      }
    }
    data->iRet = (int)color;
  } break;

  case EVT_GAME_DISCONNECTED: {
    CallOnMeDisconnectFromServer();
  } break;

  case EVT_GAMECHECKBAN: {
    network_address *addr = (network_address *)data->special_data;
    const char *tracker_id = (const char *) data->iParam;

    data->iRet = (CallOnServerIsAddressBanned(addr, tracker_id)) ? 1 : 0;
  } break;

  case EVT_GAME_GET_PLAYER_TEAM: {
    data->iRet = GetConnectingPlayerTeam(data->iParam);
  } break;

  case EVT_CLIENT_SHOWUI: {
    if (m_iUIWindowID != -1) {
      CallOnClientShowUI(m_iUIWindowID, m_UIUserData);
    }

    m_iUIWindowID = -1;
    m_UIUserData = NULL;
  } break;

  case EVT_GAMEDOCONTROLS: {
    CallOnDoControls((game_controls *)data->special_data);
  } break;

  case EVT_GAMEWALLCOLLIDE: {
    CallOnServerWallCollide(me_objp, data->collide_info.hitspeed, data->collide_info.hitseg, data->collide_info.hitwall,
                            &data->collide_info.point, &data->collide_info.normal, data->collide_info.hit_dot);
  } break;

  case EVT_CLIENT_GAMEWALLCOLLIDE: {
    CallOnClientWallCollide(me_objp, data->collide_info.hitspeed, data->collide_info.hitseg, data->collide_info.hitwall,
                            &data->collide_info.point, &data->collide_info.normal, data->collide_info.hit_dot);
  } break;

  case EVT_CLIENT_GAMEOBJDESTROYED: {
    CallOnClientObjectDestroyed(me_objp);
  } break;
  case EVT_CLIENT_GAMEOBJKILLED: {
    CallOnClientObjectKilled(me_objp, it_objp);
  } break;
  case EVT_CLIENT_PLAYERPLAYSAUDIOTAUNT: {
    CallOnPlayAudioTaunt(me_objp->id);
  } break;
  case EVT_CLIENT_DECODETEXTMACRO: {
    char *dest_string = (char *)data->special_data;
    char *src_string = data->input_string;
    const ptrdiff_t dest_size = data->iParam;

    TranslateTextMacro(src_string, dest_string, dest_size);
  } break;
  default:
    DLLmprintf((0, "DMFC Warning: Unhandled Event #%X\n", eventnum));
    break;
  }
}
