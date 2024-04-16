/*
 * $Logfile: /DescentIII/Main/Dmfc/dmfcinterface.cpp $
 * $Revision: 1.1.1.1 $
 * $Date: 2003/08/26 03:57:20 $
 * $Author: kevinb $
 *
 * DMFC COM Interface functions
 *
 * $Log: dmfcinterface.cpp,v $
 * Revision 1.1.1.1  2003/08/26 03:57:20  kevinb
 * initial 1.5 import
 *
 *
 * 10    8/15/99 4:36p Jeff
 * finished exporting all inventory class functions.  export object_info
 * array. added check for -nooutragelogo to display Outrage logo display.
 *
 * 9     8/11/99 1:46p Jeff
 * interface functions
 *
 * 8     7/13/99 5:45p Jeff
 * fixed for Windows compile
 *
 * 7     7/13/99 5:39p Jeff
 * changes for Linux compile
 *
 * 6     7/09/99 2:53p Jeff
 * handle gametime better (pause it when needed) if the server is 'waiting
 * for players'
 *
 * 5     7/08/99 9:55p Jeff
 * added event handler for weapon fired event
 *
 * 4     7/07/99 5:00p Jeff
 * removed vararg functions from interface functions, just made different
 * versions of them
 *
 * 3     7/07/99 12:16p Jeff
 * all mangles symbol names fixed.  Finished making interface functions.
 *
 * 2     7/06/99 7:37p Jeff
 * added IDMFC 1.0 interface functions
 *
 * $NoKeywords: $
 */

#include "gamedll_header.h"
#include "DMFC.h"
#include "dmfcinternal.h"
#include "dmfcinputcommands.h"

#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>

/*
 ***********************************************************
 IDMFC 1.0 COM Interface Functions
 ***********************************************************
 */

void DLLFUNCCALL IDMFC_Delete(IDMFC *instance) {
  assert(instance != NULL);
  instance->Delete();
}

void DLLFUNCCALLPTR IDMFC_Dynamic_Cast(IDMFC *instance, const char *pszType) {
  assert(instance != NULL);
  return instance->Dynamic_Cast(pszType);
}

void DLLFUNCCALL IDMFC_DuplicatePointer(IDMFC *instance) {
  assert(instance != NULL);
  instance->DuplicatePointer();
}

void DLLFUNCCALL IDMFC_DestroyPointer(IDMFC *instance) {
  assert(instance != NULL);
  instance->DestroyPointer();
}

void DLLFUNCCALL IDMFC_LoadFunctions(IDMFC *instance, int *api_func) {
  assert(instance != NULL);
  instance->LoadFunctions(api_func);
}

void DLLFUNCCALL IDMFC_GameInit(IDMFC *instance, int teams) {
  assert(instance != NULL);
  instance->GameInit(teams);
}

void DLLFUNCCALL IDMFC_GameClose(IDMFC *instance) {
  assert(instance != NULL);
  instance->GameClose();
}

void DLLFUNCCALL IDMFC_OnServerPlayerKilled(IDMFC *instance, object *killer_obj, int victim_pnum) {
  assert(instance != NULL);
  instance->OnServerPlayerKilled(killer_obj, victim_pnum);
}

void DLLFUNCCALL IDMFC_OnServerPlayerExploded(IDMFC *instance, int player_num) {
  assert(instance != NULL);
  instance->OnServerPlayerExploded(player_num);
}

void DLLFUNCCALL IDMFC_OnServerCollide(IDMFC *instance, object *me_obj, object *it_obj) {
  assert(instance != NULL);
  instance->OnServerCollide(me_obj, it_obj);
}

void DLLFUNCCALL IDMFC_OnServerCollideComplex(IDMFC *instance, object *me_obj, object *it_obj, vector *point,
                                              vector *normal) {
  assert(instance != NULL);
  instance->OnServerCollide(me_obj, it_obj, point, normal);
}

void DLLFUNCCALL IDMFC_OnServerPlayerChangeSegment(IDMFC *instance, int player_num, int newseg, int oldseg) {
  assert(instance != NULL);
  instance->OnServerPlayerChangeSegment(player_num, newseg, oldseg);
}

void DLLFUNCCALL IDMFC_OnServerObjectChangeSegment(IDMFC *instance, object *obj, int newseg, int oldseg) {
  assert(instance != NULL);
  instance->OnServerObjectChangeSegment(obj, newseg, oldseg);
}

void DLLFUNCCALL IDMFC_OnServerPlayerEntersGame(IDMFC *instance, int player_num) {
  assert(instance != NULL);
  instance->OnServerPlayerEntersGame(player_num);
}

void DLLFUNCCALL IDMFC_OnServerPlayerDisconnect(IDMFC *instance, int player_num) {
  assert(instance != NULL);
  instance->OnServerPlayerDisconnect(player_num);
}

void DLLFUNCCALL IDMFC_OnServerGameCreated(IDMFC *instance) {
  assert(instance != NULL);
  instance->OnServerGameCreated();
}

void DLLFUNCCALL IDMFC_OnServerLevelChange(IDMFC *instance) {
  assert(instance != NULL);
  instance->OnServerLevelChange();
}

void DLLFUNCCALL IDMFC_OnServerLevelStart(IDMFC *instance) {
  assert(instance != NULL);
  instance->OnServerLevelStart();
}

void DLLFUNCCALL IDMFC_OnServerLevelEnd(IDMFC *instance) {
  assert(instance != NULL);
  instance->OnServerLevelEnd();
}

void DLLFUNCCALL IDMFC_OnServerObjectShieldsChanged(IDMFC *instance, object *obj, float amount) {
  assert(instance != NULL);
  instance->OnServerObjectShieldsChanged(obj, amount);
}

bool DLLFUNCCALL IDMFC_OnServerIsAddressBanned(IDMFC *instance, network_address *addr, char *tracker_id) {
  assert(instance != NULL);
  return instance->OnServerIsAddressBanned(addr, tracker_id);
}

void DLLFUNCCALL IDMFC_OnServerWallCollide(IDMFC *instance, object *obj, float hitspeed, int hitseg, int hitwall,
                                           vector *hitpt, vector *wall_normal, float hit_dot) {
  assert(instance != NULL);
  instance->OnServerWallCollide(obj, hitspeed, hitseg, hitwall, hitpt, wall_normal, hit_dot);
}

void DLLFUNCCALL IDMFC_OnServerObjectKilled(IDMFC *instance, object *obj, object *killer) {
  assert(instance != NULL);
  instance->OnServerObjectKilled(obj, killer);
}

void DLLFUNCCALL IDMFC_OnServerObjectDestroyed(IDMFC *instance, object *obj) {
  assert(instance != NULL);
  instance->OnServerObjectDestroyed(obj);
}

void DLLFUNCCALL IDMFC_OnClientPlayerKilled(IDMFC *instance, object *killer_obj, int victim_pnum) {
  assert(instance != NULL);
  instance->OnClientPlayerKilled(killer_obj, victim_pnum);
}

void DLLFUNCCALL IDMFC_OnClientPlayerExploded(IDMFC *instance, int player_num) {
  assert(instance != NULL);
  instance->OnClientPlayerExploded(player_num);
}

void DLLFUNCCALL IDMFC_OnClientCollide(IDMFC *instance, object *me_obj, object *it_obj) {
  assert(instance != NULL);
  instance->OnClientCollide(me_obj, it_obj);
}

void DLLFUNCCALL IDMFC_OnClientCollideComplex(IDMFC *instance, object *me_obj, object *it_obj, vector *point,
                                              vector *normal) {
  assert(instance != NULL);
  instance->OnClientCollide(me_obj, it_obj, point, normal);
}

void DLLFUNCCALL IDMFC_OnClientPlayerChangeSegment(IDMFC *instance, int player_num, int newseg, int oldseg) {
  assert(instance != NULL);
  instance->OnClientPlayerChangeSegment(player_num, newseg, oldseg);
}

void DLLFUNCCALL IDMFC_OnClientObjectChangeSegment(IDMFC *instance, object *obj, int newseg, int oldseg) {
  assert(instance != NULL);
  instance->OnClientObjectChangeSegment(obj, newseg, oldseg);
}

void DLLFUNCCALL IDMFC_OnClientPlayerEntersGame(IDMFC *instance, int player_num) {
  assert(instance != NULL);
  instance->OnClientPlayerEntersGame(player_num);
}

void DLLFUNCCALL IDMFC_OnClientPlayerDisconnect(IDMFC *instance, int player_num) {
  assert(instance != NULL);
  instance->OnClientPlayerDisconnect(player_num);
}

void DLLFUNCCALL IDMFC_OnMeDisconnectFromServer(IDMFC *instance) {
  assert(instance != NULL);
  instance->OnMeDisconnectFromServer();
}

void DLLFUNCCALL IDMFC_OnClientGameCreated(IDMFC *instance) {
  assert(instance != NULL);
  instance->OnClientGameCreated();
}

void DLLFUNCCALL IDMFC_OnClientObjectShieldsChanged(IDMFC *instance, object *obj, float amount) {
  assert(instance != NULL);
  instance->OnClientObjectShieldsChanged(obj, amount);
}

void DLLFUNCCALL IDMFC_OnWeaponFired(IDMFC *instance, object *weapon_obj, object *shooter) {
  assert(instance != NULL);
  instance->OnWeaponFired(weapon_obj, shooter);
}

void DLLFUNCCALL IDMFC_OnClientLevelChange(IDMFC *instance) {
  assert(instance != NULL);
  instance->OnClientLevelChange();
}

void DLLFUNCCALL IDMFC_OnClientLevelStart(IDMFC *instance) {
  assert(instance != NULL);
  instance->OnClientLevelStart();
}

void DLLFUNCCALL IDMFC_OnClientLevelEnd(IDMFC *instance) {
  assert(instance != NULL);
  instance->OnClientLevelEnd();
}

void DLLFUNCCALL IDMFC_OnClientWallCollide(IDMFC *instance, object *obj, float hitspeed, int hitseg, int hitwall,
                                           vector *hitpt, vector *wall_normal, float hit_dot) {
  assert(instance != NULL);
  instance->OnClientWallCollide(obj, hitspeed, hitseg, hitwall, hitpt, wall_normal, hit_dot);
}

void DLLFUNCCALL IDMFC_OnClientObjectKilled(IDMFC *instance, object *obj, object *killer) {
  assert(instance != NULL);
  instance->OnClientObjectKilled(obj, killer);
}

void DLLFUNCCALL IDMFC_OnClientObjectDestroyed(IDMFC *instance, object *obj) {
  assert(instance != NULL);
  instance->OnClientObjectDestroyed(obj);
}

void DLLFUNCCALL IDMFC_OnPlayerEntersObserver(IDMFC *instance, int pnum, object *piggy) {
  assert(instance != NULL);
  instance->OnPlayerEntersObserver(pnum, piggy);
}

void DLLFUNCCALL IDMFC_OnPlayerExitsObserver(IDMFC *instance, int pnum) {
  assert(instance != NULL);
  instance->OnPlayerExitsObserver(pnum);
}

bool DLLFUNCCALL IDMFC_OnCanChangeTeam(IDMFC *instance, int pnum, int newteam) {
  assert(instance != NULL);
  return instance->OnCanChangeTeam(pnum, newteam);
}

void DLLFUNCCALL IDMFC_OnSpecialPacket(IDMFC *instance) {
  assert(instance != NULL);
  instance->OnSpecialPacket();
}

void DLLFUNCCALL IDMFC_OnInterval(IDMFC *instance) {
  assert(instance != NULL);
  instance->OnInterval();
}

void DLLFUNCCALL IDMFC_OnHUDInterval(IDMFC *instance) {
  assert(instance != NULL);
  instance->OnHUDInterval();
}

void DLLFUNCCALL IDMFC_OnPLRInterval(IDMFC *instance) {
  assert(instance != NULL);
  instance->OnPLRInterval();
}

void DLLFUNCCALL IDMFC_OnPLRInit(IDMFC *instance) {
  assert(instance != NULL);
  instance->OnPLRInit();
}

void DLLFUNCCALL IDMFC_OnKeypress(IDMFC *instance, int key) {
  assert(instance != NULL);
  instance->OnKeypress(key);
}

void DLLFUNCCALL IDMFC_OnInputString(IDMFC *instance, char *input_string) {
  assert(instance != NULL);
  instance->OnInputString(input_string);
}

void DLLFUNCCALL IDMFC_OnPlayerChangeTeam(IDMFC *instance, int player_num, int newteam, bool announce,
                                          bool spew_everything) {
  assert(instance != NULL);
  instance->OnPlayerChangeTeam(player_num, newteam, announce, spew_everything);
}

void DLLFUNCCALL IDMFC_OnGameStateRequest(IDMFC *instance, int pnum) {
  assert(instance != NULL);
  instance->OnGameStateRequest(pnum);
}

void DLLFUNCCALL IDMFC_OnSaveStatsToFile(IDMFC *instance) {
  assert(instance != NULL);
  instance->OnSaveStatsToFile();
}

void DLLFUNCCALL IDMFC_OnPlayerReconnect(IDMFC *instance, int player_num) {
  assert(instance != NULL);
  instance->OnPlayerReconnect(player_num);
}

void DLLFUNCCALL IDMFC_OnPlayerConnect(IDMFC *instance, int player_num) {
  assert(instance != NULL);
  instance->OnPlayerConnect(player_num);
}

void DLLFUNCCALL IDMFC_OnControlMessage(IDMFC *instance, ubyte msg, int from_pnum) {
  assert(instance != NULL);
  instance->OnControlMessage(msg, from_pnum);
}

bool DLLFUNCCALL IDMFC_OnAllowObserverChange(IDMFC *instance, bool turnonobserver) {
  assert(instance != NULL);
  return instance->OnAllowObserverChange(turnonobserver);
}

void DLLFUNCCALL IDMFC_OnClientShowUI(IDMFC *instance, int id, void *user_data) {
  assert(instance != NULL);
  instance->OnClientShowUI(id, user_data);
}

void DLLFUNCCALL IDMFC_OnPrintScores(IDMFC *instance, int level) {
  assert(instance != NULL);
  instance->OnPrintScores(level);
}

void DLLFUNCCALL IDMFC_OnDisconnectSaveStatsToFile(IDMFC *instance) {
  assert(instance != NULL);
  instance->OnDisconnectSaveStatsToFile();
}

void DLLFUNCCALL IDMFC_OnLevelEndSaveStatsToFile(IDMFC *instance) {
  assert(instance != NULL);
  instance->OnLevelEndSaveStatsToFile();
}

ddgr_color DLLFUNCCALL IDMFC_OnGetHudCallSignColor(IDMFC *instance, int playernum) {
  assert(instance != NULL);
  return instance->OnGetHudCallSignColor(playernum);
}

void DLLFUNCCALL IDMFC_TranslateEvent(IDMFC *instance, int eventnum, dllinfo *data) {
  assert(instance != NULL);
  instance->TranslateEvent(eventnum, data);
}

void DLLFUNCCALL IDMFC_OnTeamChangeName(IDMFC *instance, int team, char *oldname, char *newname) {
  assert(instance != NULL);
  instance->OnTeamChangeName(team, oldname, newname);
}

void DLLFUNCCALL IDMFC_OnDoControls(IDMFC *instance, game_controls *controls) {
  assert(instance != NULL);
  instance->OnDoControls(controls);
}

void DLLFUNCCALL IDMFC_OnPlayAudioTaunt(IDMFC *instance, int pnum) {
  assert(instance != NULL);
  instance->OnPlayAudioTaunt(pnum);
}

void DLLFUNCCALL IDMFC_DrawMenu(IDMFC *instance) {
  assert(instance != NULL);
  instance->DrawMenu();
}

void DLLFUNCCALL IDMFC_DisplayPlayerInfo(IDMFC *instance, int background_bmp, bool dedicated_server) {
  assert(instance != NULL);
  instance->DisplayPlayerInfo(background_bmp, dedicated_server);
}

void DLLFUNCCALL IDMFC_SwitchPlayerInfoDisplay(IDMFC *instance, int pnum) {
  assert(instance != NULL);
  instance->SwitchPlayerInfoDisplay(pnum);
}

int DLLFUNCCALL IDMFC_DisplayingPlayerInfo(IDMFC *instance) {
  assert(instance != NULL);
  return instance->DisplayingPlayerInfo();
}

int DLLFUNCCALL IDMFC_GetTeamFromString(IDMFC *instance, char *str) {
  assert(instance != NULL);
  return instance->GetTeamFromString(str);
}

int DLLFUNCCALL IDMFC_GetPlayerNum(IDMFC *instance) {
  assert(instance != NULL);
  return instance->GetPlayerNum();
}

int DLLFUNCCALL IDMFC_GetLocalRole(IDMFC *instance) {
  assert(instance != NULL);
  return instance->GetLocalRole();
}

bool DLLFUNCCALL IDMFC_CheckPlayerNum(IDMFC *instance, int player_num) {
  assert(instance != NULL);
  return instance->CheckPlayerNum(player_num);
}

bool DLLFUNCCALL IDMFC_PacketCheckPlayerNum(IDMFC *instance, int player_num) {
  assert(instance != NULL);
  return instance->PacketCheckPlayerNum(player_num);
}

void DLLFUNCCALL IDMFC_CallClientEvent(IDMFC *instance, int event, int me_objnum, int it_objnum, int destination,
                                       bool parms) {
  assert(instance != NULL);
  instance->CallClientEvent(event, me_objnum, it_objnum, destination, parms);
}

bool DLLFUNCCALL IDMFC_GetTimeLeft(IDMFC *instance, float *time) {
  assert(instance != NULL);
  return instance->GetTimeLeft(time);
}

void DLLFUNCCALL IDMFC_EndLevel(IDMFC *instance) {
  assert(instance != NULL);
  instance->EndLevel();
}

void DLLFUNCCALL IDMFC_WarpToLevel(IDMFC *instance, int lev) {
  assert(instance != NULL);
  instance->WarpToLevel(lev);
}

bool DLLFUNCCALL IDMFC_GetScoreLimit(IDMFC *instance, int *limit) {
  assert(instance != NULL);
  return instance->GetScoreLimit(limit);
}

void DLLFUNCCALL IDMFC_AutoTimeLimit(IDMFC *instance, bool turnon) {
  assert(instance != NULL);
  instance->AutoTimeLimit(turnon);
}

void DLLFUNCCALL IDMFC_AutoDeathMessage(IDMFC *instance, bool turnon) {
  assert(instance != NULL);
  instance->AutoDeathMessage(turnon);
}

void DLLFUNCCALL IDMFC_AddDeathMessage(IDMFC *instance, char *string, bool victim_first) {
  assert(instance != NULL);
  instance->AddDeathMessage(string, victim_first);
}

void DLLFUNCCALL IDMFC_AddSuicideMessage(IDMFC *instance, char *string) {
  assert(instance != NULL);
  instance->AddSuicideMessage(string);
}

void DLLFUNCCALL IDMFC_DoRandomDeathMessage(IDMFC *instance, int killernum, int victimnum, uint hash) {
  assert(instance != NULL);
  instance->DoRandomDeathMessage(killernum, victimnum, hash);
}

int DLLFUNCCALL IDMFC_GetItObjNum(IDMFC *instance) {
  assert(instance != NULL);
  return instance->GetItObjNum();
}

int DLLFUNCCALL IDMFC_GetMeObjNum(IDMFC *instance) {
  assert(instance != NULL);
  return instance->GetMeObjNum();
}

void DLLFUNCCALL IDMFC_RegisterPacketReceiver(IDMFC *instance, ubyte id, void (*func)(ubyte *)) {
  assert(instance != NULL);
  instance->RegisterPacketReceiver(id, func);
}

void DLLFUNCCALL IDMFC_StartPacket(IDMFC *instance, ubyte *data, ubyte id, int *count) {
  assert(instance != NULL);
  instance->StartPacket(data, id, count);
}

void DLLFUNCCALL IDMFC_SendPacket(IDMFC *instance, ubyte *data, int size, int destination) {
  assert(instance != NULL);
  instance->SendPacket(data, size, destination);
}

int DLLFUNCCALL IDMFC_GetTeamForNewPlayer(IDMFC *instance, int player_num, int num_teams) {
  assert(instance != NULL);
  return instance->GetTeamForNewPlayer(player_num, num_teams);
}

void DLLFUNCCALL IDMFC_SetNumberOfTeams(IDMFC *instance, int teams) {
  assert(instance != NULL);
  instance->SetNumberOfTeams(teams);
}

void DLLFUNCCALL IDMFC_AutoTeamSelect(IDMFC *instance, bool turnon) {
  assert(instance != NULL);
  instance->AutoTeamSelect(turnon);
}

void DLLFUNCCALL IDMFC_SendTeamAssignment(IDMFC *instance, int playernum, int team, bool spew_on_respawn) {
  assert(instance != NULL);
  instance->SendTeamAssignment(playernum, team, spew_on_respawn);
}

void DLLFUNCCALL IDMFC_GetTeamAssignmentPacket(IDMFC *instance, ubyte *data) {
  assert(instance != NULL);
  instance->GetTeamAssignmentPacket(data);
}

void DLLFUNCCALL IDMFC_GetChangeTeamPacket(IDMFC *instance, ubyte *data) {
  assert(instance != NULL);
  instance->GetChangeTeamPacket(data);
}

void DLLFUNCCALL IDMFC_GetGameStateRequest(IDMFC *instance, ubyte *data) {
  assert(instance != NULL);
  instance->GetGameStateRequest(data);
}

void DLLFUNCCALL IDMFC_SendChangeTeamRequest(IDMFC *instance, int newteam, bool spew_onchange) {
  assert(instance != NULL);
  instance->SendChangeTeamRequest(newteam, spew_onchange);
}

void DLLFUNCCALL IDMFC_GetDMFCGameInfo(IDMFC *instance, ubyte *data) {
  assert(instance != NULL);
  instance->GetDMFCGameInfo(data);
}

void DLLFUNCCALL IDMFC_SendDMFCGameInfo(IDMFC *instance, int player) {
  assert(instance != NULL);
  instance->SendDMFCGameInfo(player);
}

void DLLFUNCCALL IDMFC_RequestTeamChange(IDMFC *instance, int team, int pnum, bool spew_on_respawn) {
  assert(instance != NULL);
  instance->RequestTeamChange(team, pnum, spew_on_respawn);
}

void DLLFUNCCALL IDMFC_RequestGameState(IDMFC *instance) {
  assert(instance != NULL);
  instance->RequestGameState();
}

void DLLFUNCCALL IDMFC_AddHUDItemCallback(IDMFC *instance, int type, void (*func)(struct tHUDItem *)) {
  assert(instance != NULL);
  instance->AddHUDItemCallback(type, func);
}

int DLLFUNCCALL IDMFC_GetMyTeam(IDMFC *instance) {
  assert(instance != NULL);
  return instance->GetMyTeam();
}

char DLLFUNCCALLPTR IDMFC_GetTeamString(IDMFC *instance, int team) {
  assert(instance != NULL);
  return (char *)instance->GetTeamString(team);
}

ddgr_color DLLFUNCCALL IDMFC_GetTeamColor(IDMFC *instance, int team) {
  assert(instance != NULL);
  return instance->GetTeamColor(team);
}

int DLLFUNCCALL IDMFC_GetNumTeams(IDMFC *instance) {
  assert(instance != NULL);
  return instance->GetNumTeams();
}

bool DLLFUNCCALL IDMFC_AllowTeamChange(IDMFC *instance) {
  assert(instance != NULL);
  return instance->AllowTeamChange();
}

void DLLFUNCCALL IDMFC_SwitchAllowTeamChange(IDMFC *instance, bool turnon) {
  assert(instance != NULL);
  instance->SwitchAllowTeamChange(turnon);
}

void DLLFUNCCALL IDMFC_GetSortedPlayerSlots(IDMFC *instance, int *sortedindex, int maxsize) {
  assert(instance != NULL);
  instance->GetSortedPlayerSlots(sortedindex, maxsize);
}

void DLLFUNCCALL IDMFC_GetSortedPlayerSlotsByEfficiency(IDMFC *instance, int *sortedindex, int maxsize) {
  assert(instance != NULL);
  instance->GetSortedPlayerSlotsByEfficiency(sortedindex, maxsize);
}

bool DLLFUNCCALL IDMFC_IsMenuUp(IDMFC *instance) {
  assert(instance != NULL);
  return instance->IsMenuUp();
}

float DLLFUNCCALL IDMFC_ConvertHUDAlphaFloat(IDMFC *instance, float normal) {
  assert(instance != NULL);
  return instance->ConvertHUDAlpha(normal);
}

ubyte DLLFUNCCALL IDMFC_ConvertHUDAlphaByte(IDMFC *instance, ubyte normal) {
  assert(instance != NULL);
  return instance->ConvertHUDAlpha(normal);
}

void DLLFUNCCALL IDMFC_ClipString(IDMFC *instance, int width, char *string, bool arrow) {
  assert(instance != NULL);
  instance->ClipString(width, string, arrow);
}

void DLLFUNCCALL IDMFC_DisplayOutrageLogo(IDMFC *instance) {
  assert(instance != NULL);
  instance->DisplayOutrageLogo();
}

void DLLFUNCCALL IDMFC_KillAllTimers(IDMFC *instance) {
  assert(instance != NULL);
  instance->KillAllTimers();
}

int DLLFUNCCALL IDMFC_SetTimerInterval(IDMFC *instance, void (*func)(void), float intval, float longevity,
                                       void (*onkill)(void)) {
  assert(instance != NULL);
  return instance->SetTimerInterval(func, intval, longevity, onkill);
}

void DLLFUNCCALL IDMFC_KillTimer(IDMFC *instance, int handle) {
  assert(instance != NULL);
  instance->KillTimer(handle);
}

void DLLFUNCCALL IDMFC_SwitchShowHudCallsignLevel(IDMFC *instance, ubyte level, bool announce) {
  assert(instance != NULL);
  instance->SwitchShowHudCallsignLevel(level, announce);
}

void DLLFUNCCALL IDMFC_SwitchServerHudCallsignLevel(IDMFC *instance, ubyte level) {
  assert(instance != NULL);
  instance->SwitchServerHudCallsignLevel(level);
}

int DLLFUNCCALL IDMFC_GetCounterMeasureOwner(IDMFC *instance, object *robot) {
  assert(instance != NULL);
  return instance->GetCounterMeasureOwner(robot);
}

int DLLFUNCCALL IDMFC_CFGOpen(IDMFC *instance, char *filename) {
  assert(instance != NULL);
  return instance->CFGOpen(filename);
}

void DLLFUNCCALL IDMFC_CFGClose(IDMFC *instance) {
  assert(instance != NULL);
  instance->CFGClose();
}

int DLLFUNCCALL IDMFC_CFGFlush(IDMFC *instance) {
  assert(instance != NULL);
  return instance->CFGFlush();
}

int DLLFUNCCALL IDMFC_CFGCreateKey(IDMFC *instance, char *name) {
  assert(instance != NULL);
  return instance->CFGCreateKey(name);
}

int DLLFUNCCALL IDMFC_CFGLookupKey(IDMFC *instance, char *name) {
  assert(instance != NULL);
  return instance->CFGLookupKey(name);
}

int DLLFUNCCALL IDMFC_CFGLookupRecord(IDMFC *instance, char *record, void *data) {
  assert(instance != NULL);
  return instance->CFGLookupRecord(record, data);
}

int DLLFUNCCALL IDMFC_CFGCreateRecord(IDMFC *instance, char *name, char type, void *data) {
  assert(instance != NULL);
  return instance->CFGCreateRecord(name, type, data);
}

void DLLFUNCCALL IDMFC_DecryptData(IDMFC *instance, ubyte *data, int size) {
  assert(instance != NULL);
  instance->DecryptData(data, size);
}

void DLLFUNCCALL IDMFC_EncryptData(IDMFC *instance, ubyte *data, int size) {
  assert(instance != NULL);
  instance->EncryptData(data, size);
}

void DLLFUNCCALL IDMFC_VersionCheck(IDMFC *instance, int pnum) {
  assert(instance != NULL);
  instance->VersionCheck(pnum);
}

void DLLFUNCCALL IDMFC_UpdatePInfo(IDMFC *instance, int victim, int killer, int amount) {
  assert(instance != NULL);
  instance->UpdatePInfo(victim, killer, amount);
}

void DLLFUNCCALL IDMFC_ResetPInfo(IDMFC *instance) {
  assert(instance != NULL);
  instance->ResetPInfo();
}

void DLLFUNCCALL IDMFC_WriteDMFCStatsToFile(IDMFC *instance, CFILE *file) {
  assert(instance != NULL);
  instance->WriteDMFCStatsToFile(file);
}

bool DLLFUNCCALL IDMFC_SetWeaponDeathMessage(IDMFC *instance, char *weapon_name, char *message, bool victim_first) {
  assert(instance != NULL);
  return instance->SetWeaponDeathMessage(weapon_name, message, victim_first);
}

char DLLFUNCCALLPTR IDMFC_GetWeaponDeathMessage(IDMFC *instance, int index, bool *victim_first) {
  assert(instance != NULL);
  return instance->GetWeaponDeathMessage(index, victim_first);
}

void DLLFUNCCALL IDMFC_AddWeaponHash(IDMFC *instance, char *parent, int count, char **array) {
  assert(instance != NULL);
  instance->AddWeaponHashArray(parent, count, array);
}

int DLLFUNCCALL IDMFC_SetupPlayerRecord(IDMFC *instance, int sizeof_individual_data,
                                        int (*pack_callback)(void *user_info, ubyte *data),
                                        int (*unpack_callback)(void *user_info, ubyte *data)) {
  assert(instance != NULL);
  return instance->SetupPlayerRecord(sizeof_individual_data, pack_callback, unpack_callback);
}

void DLLFUNCCALLPTR IDMFC_GetPlayerRecordData(IDMFC *instance, int pnum) {
  assert(instance != NULL);
  return instance->GetPlayerRecordData(pnum);
}

player_record DLLFUNCCALLPTR IDMFC_GetPlayerRecord(IDMFC *instance, int slot) {
  assert(instance != NULL);
  return instance->GetPlayerRecord(slot);
}

player_record DLLFUNCCALLPTR IDMFC_GetPlayerRecordByPnum(IDMFC *instance, int pnum) {
  assert(instance != NULL);
  return instance->GetPlayerRecordByPnum(pnum);
}

void DLLFUNCCALL IDMFC_SendRequestForPlayerRecords(IDMFC *instance) {
  assert(instance != NULL);
  instance->SendRequestForPlayerRecords();
}

void DLLFUNCCALL IDMFC_ReceiveRequestForPlayerRecords(IDMFC *instance, ubyte *data) {
  assert(instance != NULL);
  instance->ReceiveRequestForPlayerRecords(data);
}

bool DLLFUNCCALL IDMFC_IsPlayerBanned(IDMFC *instance, int pnum) {
  assert(instance != NULL);
  return instance->IsPlayerBanned(pnum);
}

bool DLLFUNCCALL IDMFC_IsAddressBanned(IDMFC *instance, network_address *addr, char *tracker_id) {
  assert(instance != NULL);
  return instance->IsAddressBanned(addr, tracker_id);
}

void DLLFUNCCALL IDMFC_RemoveAllBans(IDMFC *instance) {
  assert(instance != NULL);
  instance->RemoveAllBans();
}

bool DLLFUNCCALL IDMFC_RemoveBan(IDMFC *instance, int index) {
  assert(instance != NULL);
  return instance->RemoveBan(index);
}

char DLLFUNCCALLPTR IDMFC_GetBannedPlayerCallsign(IDMFC *instance, int index) {
  assert(instance != NULL);
  return instance->GetBannedPlayerCallsign(index);
}

int DLLFUNCCALL IDMFC_GetNumBannedPlayers(IDMFC *instance) {
  assert(instance != NULL);
  return instance->GetNumBannedPlayers();
}

void DLLFUNCCALL IDMFC_BanPlayerFromGame(IDMFC *instance, int pnum) {
  assert(instance != NULL);
  instance->BanPlayerFromGame(pnum);
}

bool DLLFUNCCALL IDMFC_IsPlayerAlive(IDMFC *instance, int pnum) {
  assert(instance != NULL);
  return instance->IsPlayerAlive(pnum);
}

void DLLFUNCCALL IDMFC_SendControlMessageToPlayer(IDMFC *instance, int pnum, ubyte msg) {
  assert(instance != NULL);
  instance->SendControlMessageToPlayer(pnum, msg);
}

void DLLFUNCCALL IDMFC_ReceiveControlMessage(IDMFC *instance, ubyte *data) {
  assert(instance != NULL);
  instance->ReceiveControlMessage(data);
}

bool DLLFUNCCALL IDMFC_FindPInfoStatFirst(IDMFC *instance, int slot, tPInfoStat *stat) {
  assert(instance != NULL);
  return instance->FindPInfoStatFirst(slot, stat);
}

bool DLLFUNCCALL IDMFC_FindPInfoStatNext(IDMFC *instance, tPInfoStat *stat) {
  assert(instance != NULL);
  return instance->FindPInfoStatNext(stat);
}

void DLLFUNCCALL IDMFC_FindPInfoStatClose(IDMFC *instance) {
  assert(instance != NULL);
  instance->FindPInfoStatClose();
}

void DLLFUNCCALL IDMFC_DoDamageToPlayer(IDMFC *instance, int pnum, int type, float amount, bool playsound) {
  assert(instance != NULL);
  instance->DoDamageToPlayer(pnum, type, amount, playsound);
}

void DLLFUNCCALL IDMFC_StartUIWindow(IDMFC *instance, int id, void *user_data) {
  assert(instance != NULL);
  instance->StartUIWindow(id, user_data);
}

void DLLFUNCCALL IDMFC_DisconnectMe(IDMFC *instance) {
  assert(instance != NULL);
  instance->DisconnectMe();
}

signed char DLLFUNCCALL IDMFC_AddInputCommand(IDMFC *instance, char *command, char *description,
                                              void (*handler)(char *)) {
  assert(instance != NULL);
  return instance->AddInputCommand(command, description, handler);
}

void DLLFUNCCALL IDMFC_CheckPInfo(IDMFC *instance) {
  assert(instance != NULL);
  instance->CheckPInfo();
}

void DLLFUNCCALL IDMFC_EnableStatisticalMessages(IDMFC *instance, bool on) {
  assert(instance != NULL);
  instance->EnableStatisticalMessages(on);
}

void DLLFUNCCALL IDMFC_EnableOnScreenMenu(IDMFC *instance, bool turn_on) {
  assert(instance != NULL);
  instance->EnableOnScreenMenu(turn_on);
}

void DLLFUNCCALL IDMFC_EnableAutoSaveLevelEnd(IDMFC *instance, bool enable) {
  assert(instance != NULL);
  instance->EnableAutoSaveLevelEnd(enable);
}

void DLLFUNCCALL IDMFC_EnableAutoSaveDisconnect(IDMFC *instance, bool enable) {
  assert(instance != NULL);
  instance->EnableAutoSaveDisconnect(enable);
}

void DLLFUNCCALL IDMFC_GenerateStatFilename(IDMFC *instance, char *filename, char *root, bool end_of_level) {
  assert(instance != NULL);
  instance->GenerateStatFilename(filename, root, end_of_level);
}

bool DLLFUNCCALL IDMFC_IsPlayerObserver(IDMFC *instance, int pnum) {
  assert(instance != NULL);
  return instance->IsPlayerObserver(pnum);
}

void DLLFUNCCALL IDMFC_EnableOnScreenMenuBackground(IDMFC *instance, bool enable) {
  assert(instance != NULL);
  instance->EnableOnScreenMenuBackground(enable);
}

float DLLFUNCCALL IDMFC_GetTimeInGame(IDMFC *instance, int slot) {
  assert(instance != NULL);
  return instance->GetTimeInGame(slot);
}

char DLLFUNCCALLPTR IDMFC_GetTimeString(IDMFC *instance, float sec) {
  assert(instance != NULL);
  return instance->GetTimeString(sec);
}

void DLLFUNCCALL IDMFC_DisplayNetGameInfo(IDMFC *instance, int background_bmp, bool dedicated_server) {
  assert(instance != NULL);
  instance->DisplayNetGameInfo(background_bmp, dedicated_server);
}

void DLLFUNCCALL IDMFC_SwitchNetGameInfoDisplay(IDMFC *instance, int on) {
  assert(instance != NULL);
  instance->SwitchNetGameInfoDisplay(on);
}

bool DLLFUNCCALL IDMFC_IsDisplayingNetGameInfo(IDMFC *instance) {
  assert(instance != NULL);
  return instance->IsDisplayingNetGameInfo();
}

void DLLFUNCCALL IDMFC_DatabaseRegister(IDMFC *instance, char *name) {
  assert(instance != NULL);
  instance->DatabaseRegister(name);
}

bool DLLFUNCCALL IDMFC_DatabaseReadA(IDMFC *instance, const char *label, char *entry, int *entrylen) {
  assert(instance != NULL);
  return instance->DatabaseRead(label, entry, entrylen);
}

bool DLLFUNCCALL IDMFC_DatabaseReadB(IDMFC *instance, const char *label, void *entry, int wordsize) {
  assert(instance != NULL);
  return instance->DatabaseRead(label, entry, wordsize);
}

bool DLLFUNCCALL IDMFC_DatabaseReadC(IDMFC *instance, const char *label, bool *entry) {
  assert(instance != NULL);
  return instance->DatabaseRead(label, entry);
}

bool DLLFUNCCALL IDMFC_DatabaseWriteA(IDMFC *instance, const char *label, const char *entry, int entrylen) {
  assert(instance != NULL);
  return instance->DatabaseWrite(label, entry, entrylen);
}

bool DLLFUNCCALL IDMFC_DatabaseWriteB(IDMFC *instance, const char *label, int entry) {
  assert(instance != NULL);
  return instance->DatabaseWrite(label, entry);
}

void DLLFUNCCALL IDMFC_SendNetGameInfoSync(IDMFC *instance, int to_who) {
  assert(instance != NULL);
  instance->SendNetGameInfoSync(to_who);
}

bool DLLFUNCCALL IDMFC_CompareNetworkAddress(IDMFC *instance, network_address *one, network_address *two,
                                             bool use_port) {
  assert(instance != NULL);
  return instance->CompareNetworkAddress(one, two, use_port);
}

bool DLLFUNCCALL IDMFC_IsMasterTrackerGame(IDMFC *instance) {
  assert(instance != NULL);
  return instance->IsMasterTrackerGame();
}

int DLLFUNCCALL IDMFC_ConvertLocalToServerObjnum(IDMFC *instance, int objnum) {
  assert(instance != NULL);
  return instance->ConvertLocalToServerObjnum(objnum);
}

int DLLFUNCCALL IDMFC_ConvertServerToLocalObjnum(IDMFC *instance, int objnum) {
  assert(instance != NULL);
  return instance->ConvertServerToLocalObjnum(objnum);
}

bool DLLFUNCCALL IDMFC_ShouldIDisplayHUDName(IDMFC *instance, int pnum) {
  assert(instance != NULL);
  return instance->ShouldIDisplayHUDName(pnum);
}

void DLLFUNCCALL IDMFC_SetDeathMessageFilter(IDMFC *instance, int level) {
  assert(instance != NULL);
  instance->SetDeathMessageFilter(level);
}

bool DLLFUNCCALL IDMFC_IAmDedicatedServer(IDMFC *instance) {
  assert(instance != NULL);
  return instance->IAmDedicatedServer();
}

bool DLLFUNCCALL IDMFC_IsPlayerDedicatedServerPnum(IDMFC *instance, int pnum) {
  assert(instance != NULL);
  return instance->IsPlayerDedicatedServer(pnum);
}

bool DLLFUNCCALL IDMFC_IsPlayerDedicatedServer(IDMFC *instance, player_record *pr) {
  assert(instance != NULL);
  return instance->IsPlayerDedicatedServer(pr);
}

void DLLFUNCCALL IDMFC_DisplayInputCommandHelp(IDMFC *instance, char *s) {
  assert(instance != NULL);
  instance->DisplayInputCommandHelp(s);
}

int DLLFUNCCALL IDMFC_GetPlayerTeam(IDMFC *instance, int pnum) {
  assert(instance != NULL);
  return instance->GetPlayerTeam(pnum);
}

bool DLLFUNCCALL IDMFC_SetTeamName(IDMFC *instance, int team, char *name, bool announce) {
  assert(instance != NULL);
  return instance->SetTeamName(team, name, announce);
}

void DLLFUNCCALL IDMFC_ConvertHUDCoord(IDMFC *instance, int x, int y, int *rx, int *ry) {
  assert(instance != NULL);
  instance->ConvertHUDCoord(x, y, rx, ry);
}

int DLLFUNCCALL IDMFC_GetPlayerLogoBmp(IDMFC *instance, int player_num, bool *is_vclip) {
  assert(instance != NULL);
  return instance->GetPlayerLogoBmp(player_num, is_vclip);
}

void DLLFUNCCALL IDMFC_EnableShipLogos(IDMFC *instance, bool enable) {
  assert(instance != NULL);
  instance->EnableShipLogos(enable);
}

bool DLLFUNCCALL IDMFC_AreLogosEnabled(IDMFC *instance) {
  assert(instance != NULL);
  return instance->AreLogosEnabled();
}

void DLLFUNCCALL IDMFC_EnableAudioTaunts(IDMFC *instance, bool enable) {
  assert(instance != NULL);
  instance->EnableAudioTaunts(enable);
}

bool DLLFUNCCALL IDMFC_AreTauntsEnabled(IDMFC *instance) {
  assert(instance != NULL);
  return instance->AreTauntsEnabled();
}

void DLLFUNCCALL IDMFC_RespawnPlayer(IDMFC *instance, int pnum, bool spew_energy_and_shield, bool spew_everything) {
  assert(instance != NULL);
  instance->RespawnPlayer(pnum, spew_energy_and_shield, spew_everything);
}

void DLLFUNCCALL IDMFC_ReadInHostsAllowDeny(IDMFC *instance) {
  assert(instance != NULL);
  instance->ReadInHostsAllowDeny();
}

void DLLFUNCCALL IDMFC_FreeHostsLists(IDMFC *instance) {
  assert(instance != NULL);
  instance->FreeHostsLists();
}

void DLLFUNCCALL IDMFC_RehashAllowDeny(IDMFC *instance) {
  assert(instance != NULL);
  instance->RehashAllowDeny();
}

void DLLFUNCCALL IDMFC_AnnounceTeamChangeDeny(IDMFC *instance, int pnum) {
  assert(instance != NULL);
  instance->AnnounceTeamChangeDeny(pnum);
}

void DLLFUNCCALL IDMFC_SetMaxPlayerHardLimit(IDMFC *instance, int max) {
  assert(instance != NULL);
  instance->SetMaxPlayerHardLimit(max);
}

int DLLFUNCCALL IDMFC_WasPlayerInGameAtLevelEnd(IDMFC *instance, int prec) {
  assert(instance != NULL);
  return instance->WasPlayerInGameAtLevelEnd(prec);
}

void DLLFUNCCALL IDMFC_Set_OnServerPlayerKilled(IDMFC *instance,
                                                void (*callback)(object *killer_obj, int victim_pnum)) {
  assert(instance != NULL);
  instance->Set_OnServerPlayerKilled(callback);
}

void DLLFUNCCALL IDMFC_Set_OnServerPlayerExploded(IDMFC *instance, void (*callback)(int player_num)) {
  assert(instance != NULL);
  instance->Set_OnServerPlayerExploded(callback);
}

void DLLFUNCCALL IDMFC_Set_OnServerCollideA(IDMFC *instance, void (*callback)(object *me_obj, object *it_obj)) {
  assert(instance != NULL);
  instance->Set_OnServerCollide(callback);
}

void DLLFUNCCALL IDMFC_Set_OnServerCollideB(IDMFC *instance, void (*callback)(object *me_obj, object *it_obj,
                                                                              vector *point, vector *normal)) {
  assert(instance != NULL);
  instance->Set_OnServerCollide(callback);
}

void DLLFUNCCALL IDMFC_Set_OnServerPlayerChangeSegment(IDMFC *instance,
                                                       void (*callback)(int player_num, int newseg, int oldseg)) {
  assert(instance != NULL);
  instance->Set_OnServerPlayerChangeSegment(callback);
}

void DLLFUNCCALL IDMFC_Set_OnServerObjectChangeSegment(IDMFC *instance,
                                                       void (*callback)(object *obj, int newseg, int oldseg)) {
  assert(instance != NULL);
  instance->Set_OnServerObjectChangeSegment(callback);
}

void DLLFUNCCALL IDMFC_Set_OnServerPlayerEntersGame(IDMFC *instance, void (*callback)(int player_num)) {
  assert(instance != NULL);
  instance->Set_OnServerPlayerEntersGame(callback);
}

void DLLFUNCCALL IDMFC_Set_OnServerPlayerDisconnect(IDMFC *instance, void (*callback)(int player_num)) {
  assert(instance != NULL);
  instance->Set_OnServerPlayerDisconnect(callback);
}

void DLLFUNCCALL IDMFC_Set_OnServerGameCreated(IDMFC *instance, void (*callback)(void)) {
  assert(instance != NULL);
  instance->Set_OnServerGameCreated(callback);
}

void DLLFUNCCALL IDMFC_Set_OnServerLevelChange(IDMFC *instance, void (*callback)(void)) {
  assert(instance != NULL);
  instance->Set_OnServerLevelChange(callback);
}

void DLLFUNCCALL IDMFC_Set_OnServerLevelStart(IDMFC *instance, void (*callback)(void)) {
  assert(instance != NULL);
  instance->Set_OnServerLevelStart(callback);
}

void DLLFUNCCALL IDMFC_Set_OnServerLevelEnd(IDMFC *instance, void (*callback)(void)) {
  assert(instance != NULL);
  instance->Set_OnServerLevelEnd(callback);
}

void DLLFUNCCALL IDMFC_Set_OnServerObjectShieldsChanged(IDMFC *instance, void (*callback)(object *obj, float amount)) {
  assert(instance != NULL);
  instance->Set_OnServerObjectShieldsChanged(callback);
}

void DLLFUNCCALL IDMFC_Set_OnServerIsAddressBanned(IDMFC *instance,
                                                   bool (*callback)(network_address *addr, char *tracker_id)) {
  assert(instance != NULL);
  instance->Set_OnServerIsAddressBanned(callback);
}

void DLLFUNCCALL IDMFC_Set_OnServerWallCollide(IDMFC *instance,
                                               void (*callback)(object *obj, float hitspeed, int hitseg, int hitwall,
                                                                vector *hitpt, vector *wall_normal, float hit_dot)) {
  assert(instance != NULL);
  instance->Set_OnServerWallCollide(callback);
}

void DLLFUNCCALL IDMFC_Set_OnServerObjectKilled(IDMFC *instance, void (*callback)(object *obj, object *killer)) {
  assert(instance != NULL);
  instance->Set_OnServerObjectKilled(callback);
}

void DLLFUNCCALL IDMFC_Set_OnServerObjectDestroyed(IDMFC *instance, void (*callback)(object *obj)) {
  assert(instance != NULL);
  instance->Set_OnServerObjectDestroyed(callback);
}

void DLLFUNCCALL IDMFC_Set_OnClientPlayerKilled(IDMFC *instance,
                                                void (*callback)(object *killer_obj, int victim_pnum)) {
  assert(instance != NULL);
  instance->Set_OnClientPlayerKilled(callback);
}

void DLLFUNCCALL IDMFC_Set_OnClientPlayerExploded(IDMFC *instance, void (*callback)(int player_num)) {
  assert(instance != NULL);
  instance->Set_OnClientPlayerExploded(callback);
}

void DLLFUNCCALL IDMFC_Set_OnClientCollideA(IDMFC *instance, void (*callback)(object *me_obj, object *it_obj)) {
  assert(instance != NULL);
  instance->Set_OnClientCollide(callback);
}

void DLLFUNCCALL IDMFC_Set_OnClientCollideB(IDMFC *instance, void (*callback)(object *me_obj, object *it_obj,
                                                                              vector *point, vector *normal)) {
  assert(instance != NULL);
  instance->Set_OnClientCollide(callback);
}

void DLLFUNCCALL IDMFC_Set_OnClientPlayerChangeSegment(IDMFC *instance,
                                                       void (*callback)(int player_num, int newseg, int oldseg)) {
  assert(instance != NULL);
  instance->Set_OnClientPlayerChangeSegment(callback);
}

void DLLFUNCCALL IDMFC_Set_OnClientObjectChangeSegment(IDMFC *instance,
                                                       void (*callback)(object *obj, int newseg, int oldseg)) {
  assert(instance != NULL);
  instance->Set_OnClientObjectChangeSegment(callback);
}

void DLLFUNCCALL IDMFC_Set_OnClientPlayerEntersGame(IDMFC *instance, void (*callback)(int player_num)) {
  assert(instance != NULL);
  instance->Set_OnClientPlayerEntersGame(callback);
}

void DLLFUNCCALL IDMFC_Set_OnClientPlayerDisconnect(IDMFC *instance, void (*callback)(int player_num)) {
  assert(instance != NULL);
  instance->Set_OnClientPlayerDisconnect(callback);
}

void DLLFUNCCALL IDMFC_Set_OnMeDisconnectFromServer(IDMFC *instance, void (*callback)(void)) {
  assert(instance != NULL);
  instance->Set_OnMeDisconnectFromServer(callback);
}

void DLLFUNCCALL IDMFC_Set_OnClientGameCreated(IDMFC *instance, void (*callback)(void)) {
  assert(instance != NULL);
  instance->Set_OnClientGameCreated(callback);
}

void DLLFUNCCALL IDMFC_Set_OnClientObjectShieldsChanged(IDMFC *instance, void (*callback)(object *obj, float amount)) {
  assert(instance != NULL);
  instance->Set_OnClientObjectShieldsChanged(callback);
}

void DLLFUNCCALL IDMFC_Set_OnWeaponFired(IDMFC *instance, void (*callback)(object *weapon_obj, object *shooter)) {
  assert(instance != NULL);
  instance->Set_OnWeaponFired(callback);
}

void DLLFUNCCALL IDMFC_Set_OnClientLevelChange(IDMFC *instance, void (*callback)(void)) {
  assert(instance != NULL);
  instance->Set_OnClientLevelChange(callback);
}

void DLLFUNCCALL IDMFC_Set_OnClientLevelStart(IDMFC *instance, void (*callback)(void)) {
  assert(instance != NULL);
  instance->Set_OnClientLevelStart(callback);
}

void DLLFUNCCALL IDMFC_Set_OnClientLevelEnd(IDMFC *instance, void (*callback)(void)) {
  assert(instance != NULL);
  instance->Set_OnClientLevelEnd(callback);
}

void DLLFUNCCALL IDMFC_Set_OnClientWallCollide(IDMFC *instance,
                                               void (*callback)(object *obj, float hitspeed, int hitseg, int hitwall,
                                                                vector *hitpt, vector *wall_normal, float hit_dot)) {
  assert(instance != NULL);
  instance->Set_OnClientWallCollide(callback);
}

void DLLFUNCCALL IDMFC_Set_OnClientObjectKilled(IDMFC *instance, void (*callback)(object *obj, object *killer)) {
  assert(instance != NULL);
  instance->Set_OnClientObjectKilled(callback);
}

void DLLFUNCCALL IDMFC_Set_OnClientObjectDestroyed(IDMFC *instance, void (*callback)(object *obj)) {
  assert(instance != NULL);
  instance->Set_OnClientObjectDestroyed(callback);
}

void DLLFUNCCALL IDMFC_Set_OnPlayerEntersObserver(IDMFC *instance, void (*callback)(int pnum, object *piggy)) {
  assert(instance != NULL);
  instance->Set_OnPlayerEntersObserver(callback);
}

void DLLFUNCCALL IDMFC_Set_OnPlayerExitsObserver(IDMFC *instance, void (*callback)(int pnum)) {
  assert(instance != NULL);
  instance->Set_OnPlayerExitsObserver(callback);
}

void DLLFUNCCALL IDMFC_Set_OnCanChangeTeam(IDMFC *instance, bool (*callback)(int pnum, int newteam)) {
  assert(instance != NULL);
  instance->Set_OnCanChangeTeam(callback);
}

void DLLFUNCCALL IDMFC_Set_OnSpecialPacket(IDMFC *instance, void (*callback)(void)) {
  assert(instance != NULL);
  instance->Set_OnSpecialPacket(callback);
}

void DLLFUNCCALL IDMFC_Set_OnInterval(IDMFC *instance, void (*callback)(void)) {
  assert(instance != NULL);
  instance->Set_OnInterval(callback);
}

void DLLFUNCCALL IDMFC_Set_OnHUDInterval(IDMFC *instance, void (*callback)(void)) {
  assert(instance != NULL);
  instance->Set_OnHUDInterval(callback);
}

void DLLFUNCCALL IDMFC_Set_OnPLRInterval(IDMFC *instance, void (*callback)(void)) {
  assert(instance != NULL);
  instance->Set_OnPLRInterval(callback);
}

void DLLFUNCCALL IDMFC_Set_OnPLRInit(IDMFC *instance, void (*callback)(void)) {
  assert(instance != NULL);
  instance->Set_OnPLRInit(callback);
}

void DLLFUNCCALL IDMFC_Set_OnKeypress(IDMFC *instance, void (*callback)(int key)) {
  assert(instance != NULL);
  instance->Set_OnKeypress(callback);
}

void DLLFUNCCALL IDMFC_Set_OnInputString(IDMFC *instance, void (*callback)(char *input_string)) {
  assert(instance != NULL);
  instance->Set_OnInputString(callback);
}

void DLLFUNCCALL IDMFC_Set_OnPlayerChangeTeam(IDMFC *instance, void (*callback)(int player_num, int newteam,
                                                                                bool announce, bool spew_everything)) {
  assert(instance != NULL);
  instance->Set_OnPlayerChangeTeam(callback);
}

void DLLFUNCCALL IDMFC_Set_OnGameStateRequest(IDMFC *instance, void (*callback)(int pnum)) {
  assert(instance != NULL);
  instance->Set_OnGameStateRequest(callback);
}

void DLLFUNCCALL IDMFC_Set_OnSaveStatsToFile(IDMFC *instance, void (*callback)(void)) {
  assert(instance != NULL);
  instance->Set_OnSaveStatsToFile(callback);
}

void DLLFUNCCALL IDMFC_Set_OnPlayerReconnect(IDMFC *instance, void (*callback)(int player_num)) {
  assert(instance != NULL);
  instance->Set_OnPlayerReconnect(callback);
}

void DLLFUNCCALL IDMFC_Set_OnPlayerConnect(IDMFC *instance, void (*callback)(int player_num)) {
  assert(instance != NULL);
  instance->Set_OnPlayerConnect(callback);
}

void DLLFUNCCALL IDMFC_Set_OnControlMessage(IDMFC *instance, void (*callback)(ubyte msg, int from_pnum)) {
  assert(instance != NULL);
  instance->Set_OnControlMessage(callback);
}

void DLLFUNCCALL IDMFC_Set_OnAllowObserverChange(IDMFC *instance, bool (*callback)(bool turnonobserver)) {
  assert(instance != NULL);
  instance->Set_OnAllowObserverChange(callback);
}

void DLLFUNCCALL IDMFC_Set_OnClientShowUI(IDMFC *instance, void (*callback)(int id, void *user_data)) {
  assert(instance != NULL);
  instance->Set_OnClientShowUI(callback);
}

void DLLFUNCCALL IDMFC_Set_OnPrintScores(IDMFC *instance, void (*callback)(int level)) {
  assert(instance != NULL);
  instance->Set_OnPrintScores(callback);
}

void DLLFUNCCALL IDMFC_Set_OnDisconnectSaveStatsToFile(IDMFC *instance, void (*callback)(void)) {
  assert(instance != NULL);
  instance->Set_OnDisconnectSaveStatsToFile(callback);
}

void DLLFUNCCALL IDMFC_Set_OnLevelEndSaveStatsToFile(IDMFC *instance, void (*callback)(void)) {
  assert(instance != NULL);
  instance->Set_OnLevelEndSaveStatsToFile(callback);
}

void DLLFUNCCALL IDMFC_Set_OnGetHudCallSignColor(IDMFC *instance, ddgr_color (*callback)(int playernum)) {
  assert(instance != NULL);
  instance->Set_OnGetHudCallSignColor(callback);
}

void DLLFUNCCALL IDMFC_Set_OnTeamChangeName(IDMFC *instance, void (*callback)(int team, char *oldname, char *newname)) {
  assert(instance != NULL);
  instance->Set_OnTeamChangeName(callback);
}

void DLLFUNCCALL IDMFC_Set_OnDoControls(IDMFC *instance, void (*callback)(game_controls *controls)) {
  assert(instance != NULL);
  instance->Set_OnDoControls(callback);
}

void DLLFUNCCALL IDMFC_Set_OnPlayAudioTaunt(IDMFC *instance, void (*callback)(int pnum)) {
  assert(instance != NULL);
  instance->Set_OnPlayAudioTaunt(callback);
}

void DLLFUNCCALL IDMFC_CallOnServerPlayerKilled(IDMFC *instance, object *killer_obj, int victim_pnum) {
  assert(instance != NULL);
  instance->CallOnServerPlayerKilled(killer_obj, victim_pnum);
}

void DLLFUNCCALL IDMFC_CallOnServerPlayerExploded(IDMFC *instance, int player_num) {
  assert(instance != NULL);
  instance->CallOnServerPlayerExploded(player_num);
}

void DLLFUNCCALL IDMFC_CallOnServerCollideA(IDMFC *instance, object *me_obj, object *it_obj) {
  assert(instance != NULL);
  instance->CallOnServerCollide(me_obj, it_obj);
}

void DLLFUNCCALL IDMFC_CallOnServerCollideB(IDMFC *instance, object *me_obj, object *it_obj, vector *point,
                                            vector *normal) {
  assert(instance != NULL);
  instance->CallOnServerCollide(me_obj, it_obj, point, normal);
}

void DLLFUNCCALL IDMFC_CallOnServerPlayerChangeSegment(IDMFC *instance, int player_num, int newseg, int oldseg) {
  assert(instance != NULL);
  instance->CallOnServerPlayerChangeSegment(player_num, newseg, oldseg);
}

void DLLFUNCCALL IDMFC_CallOnServerObjectChangeSegment(IDMFC *instance, object *obj, int newseg, int oldseg) {
  assert(instance != NULL);
  instance->CallOnServerObjectChangeSegment(obj, newseg, oldseg);
}

void DLLFUNCCALL IDMFC_CallOnServerPlayerEntersGame(IDMFC *instance, int player_num) {
  assert(instance != NULL);
  instance->CallOnServerPlayerEntersGame(player_num);
}

void DLLFUNCCALL IDMFC_CallOnServerPlayerDisconnect(IDMFC *instance, int player_num) {
  assert(instance != NULL);
  instance->CallOnServerPlayerDisconnect(player_num);
}

void DLLFUNCCALL IDMFC_CallOnServerGameCreated(IDMFC *instance) {
  assert(instance != NULL);
  instance->CallOnServerGameCreated();
}

void DLLFUNCCALL IDMFC_CallOnServerLevelChange(IDMFC *instance) {
  assert(instance != NULL);
  instance->CallOnServerLevelChange();
}

void DLLFUNCCALL IDMFC_CallOnServerLevelStart(IDMFC *instance) {
  assert(instance != NULL);
  instance->CallOnServerLevelStart();
}

void DLLFUNCCALL IDMFC_CallOnServerLevelEnd(IDMFC *instance) {
  assert(instance != NULL);
  instance->CallOnServerLevelEnd();
}

void DLLFUNCCALL IDMFC_CallOnServerObjectShieldsChanged(IDMFC *instance, object *obj, float amount) {
  assert(instance != NULL);
  instance->CallOnServerObjectShieldsChanged(obj, amount);
}

bool DLLFUNCCALL IDMFC_CallOnServerIsAddressBanned(IDMFC *instance, network_address *addr, char *tracker_id) {
  assert(instance != NULL);
  return instance->CallOnServerIsAddressBanned(addr, tracker_id);
}

void DLLFUNCCALL IDMFC_CallOnServerWallCollide(IDMFC *instance, object *obj, float hitspeed, int hitseg, int hitwall,
                                               vector *hitpt, vector *wall_normal, float hit_dot) {
  assert(instance != NULL);
  instance->CallOnServerWallCollide(obj, hitspeed, hitseg, hitwall, hitpt, wall_normal, hit_dot);
}

void DLLFUNCCALL IDMFC_CallOnServerObjectKilled(IDMFC *instance, object *obj, object *killer) {
  assert(instance != NULL);
  instance->CallOnServerObjectKilled(obj, killer);
}

void DLLFUNCCALL IDMFC_CallOnServerObjectDestroyed(IDMFC *instance, object *obj) {
  assert(instance != NULL);
  instance->CallOnServerObjectDestroyed(obj);
}

void DLLFUNCCALL IDMFC_CallOnClientPlayerKilled(IDMFC *instance, object *killer_obj, int victim_pnum) {
  assert(instance != NULL);
  instance->CallOnClientPlayerKilled(killer_obj, victim_pnum);
}

void DLLFUNCCALL IDMFC_CallOnClientPlayerExploded(IDMFC *instance, int player_num) {
  assert(instance != NULL);
  instance->CallOnClientPlayerExploded(player_num);
}

void DLLFUNCCALL IDMFC_CallOnClientCollideA(IDMFC *instance, object *me_obj, object *it_obj) {
  assert(instance != NULL);
  instance->CallOnClientCollide(me_obj, it_obj);
}

void DLLFUNCCALL IDMFC_CallOnClientCollideB(IDMFC *instance, object *me_obj, object *it_obj, vector *point,
                                            vector *normal) {
  assert(instance != NULL);
  instance->CallOnClientCollide(me_obj, it_obj, point, normal);
}

void DLLFUNCCALL IDMFC_CallOnClientPlayerChangeSegment(IDMFC *instance, int player_num, int newseg, int oldseg) {
  assert(instance != NULL);
  instance->CallOnClientPlayerChangeSegment(player_num, newseg, oldseg);
}

void DLLFUNCCALL IDMFC_CallOnClientObjectChangeSegment(IDMFC *instance, object *obj, int newseg, int oldseg) {
  assert(instance != NULL);
  instance->CallOnClientObjectChangeSegment(obj, newseg, oldseg);
}

void DLLFUNCCALL IDMFC_CallOnClientPlayerEntersGame(IDMFC *instance, int player_num) {
  assert(instance != NULL);
  instance->CallOnClientPlayerEntersGame(player_num);
}

void DLLFUNCCALL IDMFC_CallOnClientPlayerDisconnect(IDMFC *instance, int player_num) {
  assert(instance != NULL);
  instance->CallOnClientPlayerDisconnect(player_num);
}

void DLLFUNCCALL IDMFC_CallOnMeDisconnectFromServer(IDMFC *instance) {
  assert(instance != NULL);
  instance->CallOnMeDisconnectFromServer();
}

void DLLFUNCCALL IDMFC_CallOnClientGameCreated(IDMFC *instance) {
  assert(instance != NULL);
  instance->CallOnClientGameCreated();
}

void DLLFUNCCALL IDMFC_CallOnClientObjectShieldsChanged(IDMFC *instance, object *obj, float amount) {
  assert(instance != NULL);
  instance->CallOnClientObjectShieldsChanged(obj, amount);
}

void DLLFUNCCALL IDMFC_CallOnWeaponFired(IDMFC *instance, object *weapon_obj, object *shooter) {
  assert(instance != NULL);
  instance->CallOnWeaponFired(weapon_obj, shooter);
}

void DLLFUNCCALL IDMFC_CallOnClientLevelChange(IDMFC *instance) {
  assert(instance != NULL);
  instance->CallOnClientLevelChange();
}

void DLLFUNCCALL IDMFC_CallOnClientLevelStart(IDMFC *instance) {
  assert(instance != NULL);
  instance->CallOnClientLevelStart();
}

void DLLFUNCCALL IDMFC_CallOnClientLevelEnd(IDMFC *instance) {
  assert(instance != NULL);
  instance->CallOnClientLevelEnd();
}

void DLLFUNCCALL IDMFC_CallOnClientWallCollide(IDMFC *instance, object *obj, float hitspeed, int hitseg, int hitwall,
                                               vector *hitpt, vector *wall_normal, float hit_dot) {
  assert(instance != NULL);
  instance->CallOnClientWallCollide(obj, hitspeed, hitseg, hitwall, hitpt, wall_normal, hit_dot);
}

void DLLFUNCCALL IDMFC_CallOnClientObjectKilled(IDMFC *instance, object *obj, object *killer) {
  assert(instance != NULL);
  instance->CallOnClientObjectKilled(obj, killer);
}

void DLLFUNCCALL IDMFC_CallOnClientObjectDestroyed(IDMFC *instance, object *obj) {
  assert(instance != NULL);
  instance->CallOnClientObjectDestroyed(obj);
}

void DLLFUNCCALL IDMFC_CallOnPlayerEntersObserver(IDMFC *instance, int pnum, object *piggy) {
  assert(instance != NULL);
  instance->CallOnPlayerEntersObserver(pnum, piggy);
}

void DLLFUNCCALL IDMFC_CallOnPlayerExitsObserver(IDMFC *instance, int pnum) {
  assert(instance != NULL);
  instance->CallOnPlayerExitsObserver(pnum);
}

bool DLLFUNCCALL IDMFC_CallOnCanChangeTeam(IDMFC *instance, int pnum, int newteam) {
  assert(instance != NULL);
  return instance->CallOnCanChangeTeam(pnum, newteam);
}

void DLLFUNCCALL IDMFC_CallOnSpecialPacket(IDMFC *instance) {
  assert(instance != NULL);
  instance->CallOnSpecialPacket();
}

void DLLFUNCCALL IDMFC_CallOnInterval(IDMFC *instance) {
  assert(instance != NULL);
  instance->CallOnInterval();
}

void DLLFUNCCALL IDMFC_CallOnHUDInterval(IDMFC *instance) {
  assert(instance != NULL);
  instance->CallOnHUDInterval();
}

void DLLFUNCCALL IDMFC_CallOnPLRInterval(IDMFC *instance) {
  assert(instance != NULL);
  instance->CallOnPLRInterval();
}

void DLLFUNCCALL IDMFC_CallOnPLRInit(IDMFC *instance) {
  assert(instance != NULL);
  instance->CallOnPLRInit();
}

void DLLFUNCCALL IDMFC_CallOnKeypress(IDMFC *instance, int key) {
  assert(instance != NULL);
  instance->CallOnKeypress(key);
}

void DLLFUNCCALL IDMFC_CallOnInputString(IDMFC *instance, char *input_string) {
  assert(instance != NULL);
  instance->CallOnInputString(input_string);
}

void DLLFUNCCALL IDMFC_CallOnPlayerChangeTeam(IDMFC *instance, int player_num, int newteam, bool announce,
                                              bool spew_everything) {
  assert(instance != NULL);
  instance->CallOnPlayerChangeTeam(player_num, newteam, announce, spew_everything);
}

void DLLFUNCCALL IDMFC_CallOnGameStateRequest(IDMFC *instance, int pnum) {
  assert(instance != NULL);
  instance->CallOnGameStateRequest(pnum);
}

void DLLFUNCCALL IDMFC_CallOnSaveStatsToFile(IDMFC *instance) {
  assert(instance != NULL);
  instance->CallOnSaveStatsToFile();
}

void DLLFUNCCALL IDMFC_CallOnPlayerReconnect(IDMFC *instance, int player_num) {
  assert(instance != NULL);
  instance->CallOnPlayerReconnect(player_num);
}

void DLLFUNCCALL IDMFC_CallOnPlayerConnect(IDMFC *instance, int player_num) {
  assert(instance != NULL);
  instance->CallOnPlayerConnect(player_num);
}

void DLLFUNCCALL IDMFC_CallOnControlMessage(IDMFC *instance, ubyte msg, int from_pnum) {
  assert(instance != NULL);
  instance->CallOnControlMessage(msg, from_pnum);
}

bool DLLFUNCCALL IDMFC_CallOnAllowObserverChange(IDMFC *instance, bool turnonobserver) {
  assert(instance != NULL);
  return instance->CallOnAllowObserverChange(turnonobserver);
}

void DLLFUNCCALL IDMFC_CallOnClientShowUI(IDMFC *instance, int id, void *user_data) {
  assert(instance != NULL);
  instance->CallOnClientShowUI(id, user_data);
}

void DLLFUNCCALL IDMFC_CallOnPrintScores(IDMFC *instance, int level) {
  assert(instance != NULL);
  instance->CallOnPrintScores(level);
}

void DLLFUNCCALL IDMFC_CallOnDisconnectSaveStatsToFile(IDMFC *instance) {
  assert(instance != NULL);
  instance->CallOnDisconnectSaveStatsToFile();
}

void DLLFUNCCALL IDMFC_CallOnLevelEndSaveStatsToFile(IDMFC *instance) {
  assert(instance != NULL);
  instance->CallOnLevelEndSaveStatsToFile();
}

ddgr_color DLLFUNCCALL IDMFC_CallOnGetHudCallSignColor(IDMFC *instance, int playernum) {
  assert(instance != NULL);
  return instance->CallOnGetHudCallSignColor(playernum);
}

void DLLFUNCCALL IDMFC_CallOnTeamChangeName(IDMFC *instance, int team, char *oldname, char *newname) {
  assert(instance != NULL);
  instance->CallOnTeamChangeName(team, oldname, newname);
}

void DLLFUNCCALL IDMFC_CallOnDoControls(IDMFC *instance, game_controls *controls) {
  assert(instance != NULL);
  instance->CallOnDoControls(controls);
}

void DLLFUNCCALL IDMFC_CallOnPlayAudioTaunt(IDMFC *instance, int pnum) {
  assert(instance != NULL);
  instance->CallOnPlayAudioTaunt(pnum);
}

dllinfo DLLFUNCCALLPTR IDMFC_GetDLLInfoCallData(IDMFC *instance) {
  assert(instance != NULL);
  return instance->GetDLLInfoCallData();
}

int DLLFUNCCALL IDMFC_GetHighestRoomIndex(IDMFC *instance) {
  assert(instance != NULL);
  return instance->GetHighestRoomIndex();
}

int DLLFUNCCALL IDMFC_GetGameWindowW(IDMFC *instance) {
  assert(instance != NULL);
  return instance->GetGameWindowW();
}

int DLLFUNCCALL IDMFC_GetGameWindowH(IDMFC *instance) {
  assert(instance != NULL);
  return instance->GetGameWindowH();
}

int DLLFUNCCALL IDMFC_GetGameWindowX(IDMFC *instance) {
  assert(instance != NULL);
  return instance->GetGameWindowX();
}

int DLLFUNCCALL IDMFC_GetGameWindowY(IDMFC *instance) {
  assert(instance != NULL);
  return instance->GetGameWindowY();
}

int DLLFUNCCALLPTR IDMFC_GetGameFontTranslateArray(IDMFC *instance) {
  assert(instance != NULL);
  return instance->GetGameFontTranslateArray();
}

int DLLFUNCCALL IDMFC_GetObserverModeBitmap(IDMFC *instance) {
  assert(instance != NULL);
  return instance->GetObserverModeBitmap();
}

float DLLFUNCCALL IDMFC_GetFrametime(IDMFC *instance) {
  assert(instance != NULL);
  return instance->GetFrametime();
}

float DLLFUNCCALL IDMFC_GetGametime(IDMFC *instance) {
  assert(instance != NULL);
  return instance->GetGametime();
}

float DLLFUNCCALL IDMFC_GetRealGametime(IDMFC *instance, bool *ispaused) {
  assert(instance != NULL);
  return instance->GetRealGametime(ispaused);
}

float DLLFUNCCALLPTR IDMFC_GetShieldDeltaArray(IDMFC *instance) {
  assert(instance != NULL);
  return instance->GetShieldDeltaArray();
}

float DLLFUNCCALL IDMFC_GetHudAspectX(IDMFC *instance) {
  assert(instance != NULL);
  return instance->GetHudAspectX();
}

float DLLFUNCCALL IDMFC_GetHudAspectY(IDMFC *instance) {
  assert(instance != NULL);
  return instance->GetHudAspectY();
}

char DLLFUNCCALLPTR IDMFC_GetLocalD3Dir(IDMFC *instance) {
  assert(instance != NULL);
  return (char *)instance->GetLocalD3Dir();
}

tMission DLLFUNCCALLPTR IDMFC_GetCurrentMission(IDMFC *instance) {
  assert(instance != NULL);
  return (tMission *)instance->GetCurrentMission();
}

room DLLFUNCCALLPTR IDMFC_GetRooms(IDMFC *instance) {
  assert(instance != NULL);
  return instance->GetRooms();
}

object DLLFUNCCALLPTR IDMFC_GetObjects(IDMFC *instance) {
  assert(instance != NULL);
  return instance->GetObjects();
}

terrain_segment DLLFUNCCALLPTR IDMFC_GetTerrainSegs(IDMFC *instance) {
  assert(instance != NULL);
  return instance->GetTerrainSegs();
}

netgame_info DLLFUNCCALLPTR IDMFC_GetNetgameInfo(IDMFC *instance) {
  assert(instance != NULL);
  return instance->GetNetgameInfo();
}

player DLLFUNCCALLPTR IDMFC_GetPlayers(IDMFC *instance) {
  assert(instance != NULL);
  return instance->GetPlayers();
}

netplayer DLLFUNCCALLPTR IDMFC_GetNetPlayers(IDMFC *instance) {
  assert(instance != NULL);
  return instance->GetNetPlayers();
}

ship DLLFUNCCALLPTR IDMFC_GetShips(IDMFC *instance) {
  assert(instance != NULL);
  return instance->GetShips();
}

weapon DLLFUNCCALLPTR IDMFC_GetWeapons(IDMFC *instance) {
  assert(instance != NULL);
  return instance->GetWeapons();
}

texture DLLFUNCCALLPTR IDMFC_GetGameTextures(IDMFC *instance) {
  assert(instance != NULL);
  return instance->GetGameTextures();
}

poly_model DLLFUNCCALLPTR IDMFC_GetGamePolyModels(IDMFC *instance) {
  assert(instance != NULL);
  return instance->GetGamePolyModels();
}

vclip DLLFUNCCALLPTR IDMFC_GetGameVClips(IDMFC *instance) {
  assert(instance != NULL);
  return instance->GetGameVClips();
}

ddgr_color DLLFUNCCALLPTR IDMFC_GetPlayerColors(IDMFC *instance) {
  assert(instance != NULL);
  return instance->GetPlayerColors();
}

game_controls DLLFUNCCALL IDMFC_GetLastGameControls(IDMFC *instance) {
  assert(instance != NULL);
  return instance->GetLastGameControls();
}

int DLLFUNCCALLPTR IDMFC_GetPilotPicBitmapHandles(IDMFC *instance) {
  assert(instance != NULL);
  return instance->GetPilotPicBitmapHandles();
}

void DLLFUNCCALL IDMFC_GetViewerObjectPtr(IDMFC *instance, object **v_obj) {
  assert(instance != NULL);
  instance->GetViewerObjectPtr(v_obj);
}

void DLLFUNCCALL IDMFC_SetViewerObjectPtr(IDMFC *instance, object *v_obj) {
  assert(instance != NULL);
  instance->SetViewerObjectPtr(v_obj);
}

float DLLFUNCCALL IDMFC_GetRenderZoom(IDMFC *instance) {
  assert(instance != NULL);
  return instance->GetRenderZoom();
}

IMenuItem DLLFUNCCALLPTR IDMFC_GetOnScreenMenu(IDMFC *instance) {
  assert(instance != NULL);
  return instance->GetOnScreenMenu();
}

tOSIRISModuleInit DLLFUNCCALLPTR IDMFC_GetOsirisModuleData(IDMFC *instance) {
  assert(instance != NULL);
  return instance->GetOsirisModuleData();
}

vis_effect DLLFUNCCALLPTR IDMFC_GetVisEffectArray(IDMFC *instance, int **Highviseptr) {
  assert(instance != NULL);
  return instance->GetVisEffectArray(Highviseptr);
}

level_info DLLFUNCCALLPTR IDMFC_GetLevelInfo(IDMFC *instance) {
  assert(instance != NULL);
  return instance->GetLevelInfo();
}

void DLLFUNCCALL IDMFC_SelectNextCameraView(IDMFC *instance, int window) {
  assert(instance != NULL);
  instance->SelectNextCameraView(window);
}

int DLLFUNCCALL IDMFC_GetCameraViewType(IDMFC *instance, int window) {
  assert(instance != NULL);
  return instance->GetCameraViewType(window);
}

object_info DLLFUNCCALLPTR IDMFC_GetObjectInfo(IDMFC *instance, int objinfo_id) {
  assert(instance != NULL);
  return instance->GetObjectInfo(objinfo_id);
}

/*
 ***********************************************************
 IMenuItem 1.0 COM Interface Functions
 ***********************************************************
 */

void DLLFUNCCALL IMenuItem_Delete(IMenuItem *instance) {
  assert(instance != NULL);
  instance->Delete();
}

void DLLFUNCCALLPTR IMenuItem_Dynamic_Cast(IMenuItem *instance, const char *pszType) {
  assert(instance != NULL);
  return instance->Dynamic_Cast(pszType);
}

void DLLFUNCCALL IMenuItem_DuplicatePointer(IMenuItem *instance) {
  assert(instance != NULL);
  instance->DuplicatePointer();
}

void DLLFUNCCALL IMenuItem_DestroyPointer(IMenuItem *instance) {
  assert(instance != NULL);
  instance->DestroyPointer();
}

bool DLLFUNCCALL IMenuItem_AddSubMenu(IMenuItem *instance, IMenuItem *p) {
  assert(instance != NULL);
  return instance->AddSubMenu(p);
}

bool DLLFUNCCALL IMenuItem_DetachSubMenu(IMenuItem *instance, IMenuItem *p) {
  assert(instance != NULL);
  return instance->DetachSubMenu(p);
}

bool DLLFUNCCALL IMenuItem_Forward(IMenuItem *instance) {
  assert(instance != NULL);
  return instance->Forward();
}

bool DLLFUNCCALL IMenuItem_Back(IMenuItem *instance) {
  assert(instance != NULL);
  return instance->Back();
}

bool DLLFUNCCALL IMenuItem_Up(IMenuItem *instance) {
  assert(instance != NULL);
  return instance->Up();
}

bool DLLFUNCCALL IMenuItem_Down(IMenuItem *instance) {
  assert(instance != NULL);
  return instance->Down();
}

void DLLFUNCCALL IMenuItem_Execute(IMenuItem *instance) {
  assert(instance != NULL);
  instance->Execute();
}

void DLLFUNCCALL IMenuItem_Draw(IMenuItem *instance, int x, int y, int height, int backgroundbmp, float *ratio) {
  assert(instance != NULL);
  instance->Draw(x, y, height, backgroundbmp, ratio);
}

void DLLFUNCCALL IMenuItem_SetInputFocus(IMenuItem *instance) {
  assert(instance != NULL);
  instance->SetInputFocus();
}

void DLLFUNCCALL IMenuItem_LoseInputFocus(IMenuItem *instance) {
  assert(instance != NULL);
  instance->LoseInputFocus();
}

bool DLLFUNCCALL IMenuItem_GetFocus(IMenuItem *instance) {
  assert(instance != NULL);
  return instance->GetFocus();
}

char DLLFUNCCALLPTR IMenuItem_GetTitle(IMenuItem *instance) {
  assert(instance != NULL);
  return instance->GetTitle();
}

int DLLFUNCCALL IMenuItem_GetCustomSubMenuCount(IMenuItem *instance) {
  assert(instance != NULL);
  return instance->GetCustomSubMenuCount();
}

void DLLFUNCCALL IMenuItem_CallFunc(IMenuItem *instance, int value) {
  assert(instance != NULL);
  instance->CallFunc(value);
}

bool DLLFUNCCALL IMenuItem_HasSubMenus(IMenuItem *instance) {
  assert(instance != NULL);
  return instance->HasSubMenus();
}

char DLLFUNCCALL IMenuItem_GetType(IMenuItem *instance) {
  assert(instance != NULL);
  return instance->GetType();
}

void DLLFUNCCALL IMenuItem_SetState(IMenuItem *instance, int state) {
  assert(instance != NULL);
  instance->SetState(state);
}

bool DLLFUNCCALL IMenuItem_SetStateItemList(IMenuItem *instance, int count, char **array) {
  assert(instance != NULL);

  return instance->SetStateItemListArray(count, array);
}

/********************************
IDmfcStats 1.0 Interface functions
*********************************/

void DLLFUNCCALL IDmfcStats_Delete(IDmfcStats *instance) {
  assert(instance != NULL);
  instance->Delete();
}

void DLLFUNCCALLPTR IDmfcStats_Dynamic_Cast(IDmfcStats *instance, const char *pszType) {
  assert(instance != NULL);
  return instance->Dynamic_Cast(pszType);
}

void DLLFUNCCALL IDmfcStats_DuplicatePointer(IDmfcStats *instance) {
  assert(instance != NULL);
  instance->DuplicatePointer();
}

void DLLFUNCCALL IDmfcStats_DestroyPointer(IDmfcStats *instance) {
  assert(instance != NULL);
  instance->DestroyPointer();
}

bool DLLFUNCCALL IDmfcStats_Initialize(IDmfcStats *instance, tDmfcStatsInit *init_info) {
  assert(instance != NULL);
  return instance->Initialize(init_info);
}

void DLLFUNCCALL IDmfcStats_Enable(IDmfcStats *instance, bool enable) {
  assert(instance != NULL);
  instance->Enable(enable);
}

bool DLLFUNCCALL IDmfcStats_IsEnabled(IDmfcStats *instance) {
  assert(instance != NULL);
  return instance->IsEnabled();
}

void DLLFUNCCALL IDmfcStats_DoFrame(IDmfcStats *instance) {
  assert(instance != NULL);
  instance->DoFrame();
}

void DLLFUNCCALL IDmfcStats_ScrollDown(IDmfcStats *instance) {
  assert(instance != NULL);
  instance->ScrollDown();
}

void DLLFUNCCALL IDmfcStats_ScrollUp(IDmfcStats *instance) {
  assert(instance != NULL);
  instance->ScrollUp();
}

bool DLLFUNCCALL IDmfcStats_CanScrollDown(IDmfcStats *instance) {
  assert(instance != NULL);
  return instance->CanScrollDown();
}

bool DLLFUNCCALL IDmfcStats_CanScrollUp(IDmfcStats *instance) {
  assert(instance != NULL);
  return instance->CanScrollUp();
}
