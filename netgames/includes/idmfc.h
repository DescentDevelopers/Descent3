/*
* $Logfile: /DescentIII/main/idmfc.h $
* $Revision: 1.1.1.1 $
* $Date: 2003/08/26 03:56:29 $
* $Author: kevinb $
*
* DMFC COM Interface header
*
* $Log: idmfc.h,v $
* Revision 1.1.1.1  2003/08/26 03:56:29  kevinb
* initial 1.5 import
*
 * 
 * 33    11/10/99 3:03p Jeff
 * fixed c interface for register packet receiver
 * 
 * 32    8/15/99 4:36p Jeff
 * finished exporting all inventory class functions.  export object_info
 * array. added check for -nooutragelogo to display Outrage logo display.
 * 
 * 31    8/11/99 1:46p Jeff
 * interface functions
 * 
 * 30    8/11/99 1:22p Jeff
 * exported needed functions for camera windows
 * 
 * 29    7/16/99 2:43p Jeff
 * changed CreateMenuItemWArgs function
 * 
 * 28    7/13/99 5:38p Jeff
 * fixes for Linux compile
 * 
 * 27    7/13/99 10:03a Jeff
 * text taunt token decoding
 * 
 * 26    7/11/99 3:32p Jeff
 * exported game arguments, made command line option to specify
 * autoexec.dmfc
 * 
 * 25    7/09/99 7:02p Jeff
 * put in countdown timer for when a level is about to end
 * 
 * 24    7/09/99 2:53p Jeff
 * handle gametime better (pause it when needed) if the server is 'waiting
 * for players'
 * 
 * 23    7/08/99 9:56p Jeff
 * added event handler for weapon fired event
 * 
 * 22    7/08/99 6:25p Jeff
 * remote admin in and working
 * 
 * 21    7/07/99 5:00p Jeff
 * removed vararg functions from interface functions, just made different
 * versions of them
 * 
 * 20    7/07/99 12:16p Jeff
 * all mangles symbol names fixed.  Finished making interface functions.
 * 
 * 19    7/06/99 7:37p Jeff
 * added IDMFC 1.0 interface functions
 * 
 * 18    6/10/99 11:10a Jeff
 * don't display the Outrage logo for non-Outrage games
 * 
 * 17    5/22/99 1:12a Jeff
 * correctly handle Viewer_object
 * 
 * 16    5/12/99 11:04p Jeff
 * dmfc and multiplayer games now have endian friendly packets (*whew*)
 * 
 * 15    5/09/99 6:20a Jeff
 * improved Entropy (added sounds, max virii per room).  Fixed rendering
 * bugs for other multiplayer dlls.
 * 
 * 14    5/08/99 11:06a Jeff
 * 
 * 13    5/07/99 12:52p Jeff
 * audio taunt icon is ppic if available.  coop has hard max team set of 4
 * 
 * 12    5/02/99 8:40a Jeff
 * fixed syntax error
 * 
 * 11    4/30/99 10:52p Jeff
 * added $warp command
 * 
 * 10    4/30/99 7:36p Jeff
 * exported vis_effects to dmfc
 * 
 * 9     4/14/99 3:56a Jeff
 * fixed case mismatch in #includes
 * 
 * 8     4/04/99 4:55p Jeff
 * added functionality to call osiris functions from multiplayer d3ms
 * 
 * 7     3/30/99 9:01p Jeff
 * exported polymodels
 * 
 * 6     3/30/99 7:42p Jeff
 * fixed a misspelling on a function name
 * 
 * 5     3/22/99 6:21p Jeff
 * added 2 more audio taunts.  a mulitplayer event when someone plays an
 * audio taunt.  option to disable audio taunts.
 * 
 * 4     3/22/99 5:51p Matt
 * Removed some includes from one file and added some to other files, to
 * reduce the amount of rebuilding when headers change.
 * 
 * 3     3/22/99 1:55p Jeff
 * make sure initialization happens (possible crashing)
 * 
 * 2     3/17/99 12:23p Jeff
 * converted DMFC to be COM interface
 * 
 * 1     3/17/99 11:54a Jeff
*
* $NoKeywords: $
*/

#ifndef __IDMFC_H_
#define __IDMFC_H_

#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include "gamedll_header.h"
#include "DMFC.h"
#include "controls.h"
#include "Mission.h"
#include "weapon.h"
#include "player.h"
#include "ship.h"
#include "vclip.h"

#ifdef __cplusplus
// C++ compiler, should we use classes?
#define EXTERN extern "C"
#define USECLASSES
#else
// C compiler, no classes supported
#define EXTERN
#endif

#ifdef USECLASSES
class IDMFC;
class IMenuItem;
class IDmfcStats;
#else
typedef void IDMFC;
typedef void IMenuItem;
typedef void IDmfcStats;
#endif


#ifdef USECLASSES
/*
IObject
	This is the base class that all DMFC classes are derived from.
*/
class IObject
{
public:
	virtual void Delete(void) = 0;
	virtual void *Dynamic_Cast(const char *pszType) = 0;
	virtual void DuplicatePointer(void) = 0;
	virtual void DestroyPointer(void) = 0;
};

//===========================================================================================

/*
IDMFC
	The interface class to Version 1.0 of DMFC
*/
class IDMFC : public IObject
{
public:
	// DMGFBase::LoadFunctions
	//
	//	Initializes all the functions for a multiplayer game...must be first thing called
	virtual void LoadFunctions(int *api_func) = 0;

	// DMFCBase::GameInit
	//   
	//   Sets up all the DLL functions and pointers and preps the class for use.  This ABSOLUTLY must be 
	//   called, so if you override DMFCApp::GameInit, make sure that you put a call to this somewhere in
	//   the override.
	virtual void GameInit(int teams) = 0;

	// DMFCBase::GameClose
	//
	//   Closes up any needed DLL and DMFC tasks.  This ABSOLUTLY must be called, so if you override
	//   DMFCApp::GameClose, make sure that you put a call to this somewhere in the override
	virtual void GameClose(void) = 0;
	
	//###################Server Only functions################################

	// DMFCBase::OnServerPlayerKilled
	//
	//  Event handler for when a player gets killed, either by another player or some other way
	//  killer_obj = object pointer to the object that killed the player
	//  victim_pnum = player number of the player that got killed
	virtual void OnServerPlayerKilled(object *killer_obj,int victim_pnum) = 0;

	// DMFCBase::OnServerPlayerExploded
	//
	//  Event handler for when a player explodes.  Gets called after a EVT_GAMEPLAYERKILLED event.
	//  player_num = player number of the player exploding
	virtual void OnServerPlayerExploded(int player_num) = 0;

	// DMFCBase::OnServerCollide
	//
	//   Event handler for when two objects collide.  At least one of the two objects is a player or a robot.
	//   Be aware that this event will be called twice per collision, the second time it is called the me and
	//   it objects will be flipped.
	//   me_obj = object pointer to the me object
	//   it_obj = object pointer to the it object
	virtual void OnServerCollide(object *me_obj,object *it_obj) = 0;
	virtual void OnServerCollide(object *me_obj,object *it_obj,vector *point,vector *normal) = 0;

	// DMFCBase::OnServerPlayerChangeSegment
	//
	//   Event handler for when a player changes rooms or a "large" terrain cell 
	//   (8x8 Terrain block).
	//   player_num = player number of the player who just changed the room/segment
	//   newseg = New room/cell location
	//	 oldseg = Old room/cell location
	virtual void OnServerPlayerChangeSegment(int player_num,int newseg,int oldseg) = 0;

	// DMFCBase::OnServerObjectChangeSegment (Only called if server tells client to execute)
	//
	//   Event handler for when a player changes rooms or a "large" terrain cell 
	//   (8x8 Terrain block).
	//   obj = Object pointer of the object who just changed the room/segment
	//   newseg = New room/cell location
	//	 oldseg = Old room/cell location
	virtual void OnServerObjectChangeSegment(object *obj,int newseg,int oldseg) = 0;

	// DMFCBase::OnServerPlayerEntersGame
	//
	//   Event handler for when a player enters the game.  This will only get called once per
	//   player, it usually gets called right after they connect to the server to start playing.
	//   player_num = player number of the player entering the game
	virtual void OnServerPlayerEntersGame(int player_num) = 0;

	// DMFCBase::OnServerPlayerDisconnect
	//
	//   Event handler for when a player disconnects from the server.  
	//   player_num = player number of the player that just disconnected
	virtual void OnServerPlayerDisconnect(int player_num) = 0;

	// DMFCBase::OnServerGameCreated
	//
	//   Event handler when the server's game first gets started.  This will only be called once
	//   while the server is running, and that is when the server first starts up.
	virtual void OnServerGameCreated(void) = 0;

	// DMFCBase::OnServerLevelChange
	//
	//   Event handler when the server changes levels.  This will get called after a level ends and a server
	//   is starting a new level.
	virtual void OnServerLevelChange(void) = 0;

	// DMFCBase::OnServerLevelStart
	//
	//   Event handler for when a multiplayer level is starting up.  This will get called right before the level
	//   starts.
	virtual void OnServerLevelStart(void) = 0;

	// DMFCBase::OnServerLevelEnd
	//
	//   Event handler for when a multiplayer level is ending.
	virtual void OnServerLevelEnd(void) = 0;

	// DMFCBase::OnServerObjectShieldsChanged
	//
	//	Event handler for when an objects shield's change
	virtual void OnServerObjectShieldsChanged(object *obj,float amount) = 0;

	// DMFCBase::OnServerIsAddressBanned
	//
	//	Called by the game to determine if the given network address is banned from the game
	virtual bool OnServerIsAddressBanned(network_address *addr,char *tracker_id) = 0;

	// DMFCBase::OnServerWallCollide
	//
	//	Called by the game when their is a collision between an object and a wall
	virtual void OnServerWallCollide(object *obj,float hitspeed,int hitseg,int hitwall,vector * hitpt,vector *wall_normal,float hit_dot) = 0;

	//	DMFCBase::OnServerObjectKilled
	//
	//	Called when an object is being killed
	//	Not automatically sent to clients (OnClientObjectKilled)
	//	killer might not be valid (NULL)
	virtual void OnServerObjectKilled(object *obj,object *killer) = 0;

	//	DMFCBase::OnServerObjectDestroyed
	//
	//	Called when an object is about to be deleted
	//	Not automatically sent to clients
	virtual void OnServerObjectDestroyed(object *obj) = 0;

	//######################### Client allowed event handlers############################

	// DMFCBase::OnClientPlayerKilled (Only called if server tells client to execute)
	//
	//  Event handler for when a player gets killed, either by another player or some other way.
	//  killer_obj = object pointer to the object that killed the player
	//  victim_pnum = player number of the player that got killed
	virtual void OnClientPlayerKilled(object *killer_obj,int victim_pnum) = 0;

	// DMFCBase::OnClientPlayerExploded (Only called if server tells client to execute)
	//
	//  Event handler for when a player explodes.  Gets called after a EVT_GAMEPLAYERKILLED event.
	//  player_num = player number of the player exploding
	virtual void OnClientPlayerExploded(int player_num) = 0;

	// DMFCBase::OnClientCollide (Only called if server tells client to execute)
	//
	//   Event handler for when two objects collide.  At least one of the two objects is a player or a robot.
	//   Be aware that this event will be called twice per collision, the second time it is called the me and
	//   it objects will be flipped.
	//	 ALSO NOTE: In order for the OnClientCollide() that passes the point and normal to have valid values
	//	 the server must pass true to the parameter to send arguments in CallClientEvent().
	//   me_obj = object pointer to the me object
	//   it_obj = object pointer to the it object
	virtual void OnClientCollide(object *me_obj,object *it_obj) = 0;
	virtual void OnClientCollide(object *me_obj,object *it_obj,vector *point,vector *normal) = 0;

	// DMFCBase::OnClientPlayerChangeSegment (Only called if server tells client to execute)
	//
	//   Event handler for when a player changes rooms or a "large" terrain cell 
	//   (8x8 Terrain block).
	//   player_num = player number of the player who just changed the room/segment
	//   newseg = New room/cell location
	//	 oldseg = Old room/cell location
	virtual void OnClientPlayerChangeSegment(int player_num,int newseg,int oldseg) = 0;

	// DMFCBase::OnClientObjectChangeSegment (Only called if server tells client to execute)
	//
	//   Event handler for when a player changes rooms or a "large" terrain cell 
	//   (8x8 Terrain block).
	//   obj = Object pointer of the object who just changed the room/segment
	//   newseg = New room/cell location
	//	 oldseg = Old room/cell location
	virtual void OnClientObjectChangeSegment(object *obj,int newseg,int oldseg) = 0;

	// DMFCBase::OnClientPlayerEntersGame (Only called if server tells client to execute)
	//
	//   Event handler for when a player enters the game.  This will only get called once per
	//   player, it usually gets called right after they connect to the server to start playing.
	//   player_num = player number of the player entering the game
	virtual void OnClientPlayerEntersGame(int player_num) = 0;

	// DMFCBase::OnClientPlayerDisconnect (Only called if server tells client to execute)
	//
	//   Event handler for when a player disconnects from the server.  
	//   player_num = player number of the player that just disconnected
	virtual void OnClientPlayerDisconnect(int player_num) = 0;

	// DMFCBase::OnMeDisconnectFromServer
	//
	//	Event handler that gets called if we disconnect from the server for some reason (not purposesly quit)
	virtual void OnMeDisconnectFromServer(void) = 0;

	// DMFCBase::OnClientGameCreated (Only called if server tells client to execute)
	//
	//   Event handler when the server's game first gets started.  This will only be called once
	//   while the server is running, and that is when the server first starts up.
	virtual void OnClientGameCreated(void) = 0;

	// DMFCBase::OnClientObjectShieldsChanged
	//
	//	Event handler for when an objects shield's change
	virtual void OnClientObjectShieldsChanged(object *obj,float amount) = 0;

	// DMFCBase::OnWeaponFired
	//
	//	Event handler for when an object fires a weapon
	virtual void OnWeaponFired(object *weapon_obj,object *shooter) = 0;

	// DMFCBase::OnClientLevelChange (Only called if server tells client to execute)
	//
	//   Event handler when the server changes levels.  This will get called after a level ends and a server
	//   is starting a new level.
	virtual void OnClientLevelChange(void) = 0;

	// DMFCBase::OnClientLevelStart (Only called if server tells client to execute)
	//
	//   Event handler for when a multiplayer level is starting up.  This will get called right before the level
	//   starts.
	virtual void OnClientLevelStart(void) = 0;

	// DMFCBase::OnClientLevelEnd (Only called if server tells client to execute)
	//
	//   Event handler for when a multiplayer level is ending.
	virtual void OnClientLevelEnd(void) = 0;

	// DMFCBase::OnClientWallCollide
	//
	//	Called by the game when their is a collision between an object and a wall
	virtual void OnClientWallCollide(object *obj,float hitspeed,int hitseg,int hitwall,vector * hitpt,vector *wall_normal,float hit_dot) = 0;

	//	DMFCBase::OnClientObjectKilled
	//
	//	Called when an object is being killed
	//	Not automatically sent to clients (OnClientObjectKilled)
	//	killer might not be valid (NULL)
	virtual void OnClientObjectKilled(object *obj,object *killer) = 0;

	//	DMFCBase::OnClientObjectDestroyed
	//
	//	Called when an object is about to be deleted
	//	Not automatically sent to clients
	virtual void OnClientObjectDestroyed(object *obj) = 0;

	// DMFCBase::OnPlayerEntersObserver
	//
	//	Event handler when a player becomes an observer mode
	//	If they are piggybacking another player than piggy is the object pointer, else it's NULL
	virtual void OnPlayerEntersObserver(int pnum,object *piggy) = 0;

	// DMFCBase::OnPlayerExitsObserver
	//
	//	Event handler when a player is leaving observer mode
	virtual void OnPlayerExitsObserver(int pnum) = 0;

	// DMFCBase::OnCanChangeTeam
	//
	//    Called to determine if a player can change teams, you can override this so the server won't 
	//    let a player change teams if it is not desired (for that player)
	virtual bool OnCanChangeTeam(int pnum,int newteam) = 0;

	// DMFCBase::OnSpecialPacket
	//
	//   Event handler for when a special packet arrives and needs to be processed.
	//   Both the server and client can get this event, although it is more common for the
	//   client to recieve these.  
	virtual void OnSpecialPacket(void) = 0;

	// DMFCBase::OnInterval
	// 
	//   Event handler that gets called once a frame
	virtual void OnInterval(void) = 0;

	// DMFCBase::OnHUDInterval
	//
	//	Event handler that gets called once a frame when it's time to render the HUD images
	virtual void OnHUDInterval(void) = 0;

	// DMFCBase::OnPLRInterval
	//
	//  Event handler that gets called once a frame when the Post Level Results screen is being display
	virtual void OnPLRInterval(void) = 0;

	// DMFCBase::OnPLRInit
	//
	//  Event handler that gets called the first frame of the PLR screen for each level
	virtual void OnPLRInit(void) = 0;

	// DMFCBase::OnKeypress
	//
	//   Event handler for when a user presses a key while in the game
	//   key = Key code of the key being pressed
	virtual void OnKeypress(int key) = 0;

	// DMFCBase::OnInputString
	//
	//   Event handler for when the user types a message at the console (F8) that begins with a $
	//   input_string = string that was typed
	virtual void OnInputString(char *input_string) = 0;
	
	// DMFCBase::OnPlayerChangeTeam
	//
	//  Called when a player changes team
	//	if announce is true than it is a change in the middle of the game, else it's an initial assignment change
	virtual void OnPlayerChangeTeam(int player_num,int newteam,bool announce,bool spew_everything) = 0;

	//DMFCBase::OnGameStateRequest
	//
	//		Server only.  Override this to listen for Game state requests from the clients
	//    When this function is called a client (who's player number is passed in) is requesting
	//    game state information.  Do what you need to do to send game state information to that player
	virtual void OnGameStateRequest(int pnum) = 0;

	//DMFCBase::OnSaveStatsToFile
	//
	//
	//		The user is requesting the game stats to be saved to file, you must handle this completly on 
	//	the game's side, nothing is done in DMFC
	virtual void OnSaveStatsToFile(void) = 0;

	//DMFCBase::OnPlayerReconnect
	//
	//
	//	This player is reconnecting to the game
	virtual void OnPlayerReconnect(int player_num) = 0;

	//DMFCBase::OnPlayerConnect
	//
	//
	//	This player is connecting to the game for the first time
	virtual void OnPlayerConnect(int player_num) = 0;

	//DMFCBase::OnControlMessage
	//
	//
	//	There is a control message sent from someone
	virtual void OnControlMessage(ubyte msg,int from_pnum) = 0;

	//DMFCBase::OnAllowObserverChange
	//
	//
	//	returns true if the requested change for observer mode should go through
	virtual bool OnAllowObserverChange(bool turnonobserver) = 0;

	//DMFCBase::OnClientShowUI
	//
	//
	//	The game is saying it's ok to do any UI.  Only handle the id's that belong to you and pass the rest
	//	to DMFCBase::OnClientShowUI().
	virtual void OnClientShowUI(int id,void *user_data) = 0;

	//DMFCBase::OnPrintScores
	//
	//
	//	The user is requesting that the scores be printed out (For Dedicated Server use).  Use DPrintf
	//	level:	-1	Requesting all the available scores
	//			n	Print only the top n scores
	//	Override this to how you see fit, but it should conform to the above.
	virtual void OnPrintScores(int level) = 0;

	//DMFCBase::OnDisconnectSaveStatsToFile
	//
	//
	//	The game should save the stats because the player (only ourself as the client in the game),
	//	has disconnected.
	virtual void OnDisconnectSaveStatsToFile(void) = 0;

	//DMFCBase::OnLevelEndSaveStatsToFile
	//
	//
	//	The game should save the "End of Level" stats to file.  You must handle this completly on
	//	the game's side, nothing is done in DMFC
	virtual void OnLevelEndSaveStatsToFile(void) = 0;

	//DMFCBase::OnGetHudCallSignColor
	//
	//	This is an event sent in by the game requesting what color it should draw the HUD callsign
	//	of the passed in playernum.  Using GR_RGB return the color from the function.  This function
	//	gets called every frame that the player is on the clients HUD.
	virtual ddgr_color OnGetHudCallSignColor(int playernum) = 0;

	// DMFCBase::TranslateEvent
	//
	//   Translates the event passed in to handle, calls the appropriate handler function.  If a function isn't
	//   created to handle the event, then it calls the default handler.  Handler functions should call the default
	//   handler unless it absolutly shouldn't
	virtual void TranslateEvent(int eventnum,dllinfo *data) = 0;

	//DMFCBase::OnTeamChangeName
	//
	//	This event occurs on the client when a team's name has just changed
	virtual void OnTeamChangeName(int team,char *oldname,char *newname) = 0;

	//DMFCBase::OnDoControls
	//
	//	This event occurs when there is a new back of game controls data
	virtual void OnDoControls(game_controls *controls) = 0;

	//DMFCBase::OnPlayAudioTaunt
	//
	//	This event occurs when a player plays an audio taunt
	virtual void OnPlayAudioTaunt(int pnum) = 0;

	//	DMFCBase::OnGetTokenString
	//
	//	This event occurs when a text macro is being used that has tokens
	//	in it, if you have a specific token, override this event to handle it.
	virtual void OnGetTokenString(char *src,char *dest,int dest_size) = 0;

	virtual void DrawMenu(void) = 0;

	virtual void DisplayPlayerInfo(int background_bmp=-1,bool dedicated_server=false) = 0;

	// DMFCBase::SwitchPlayerInfoDisplay
	//
	//	Switches on/off displaying a Playerinfo
	virtual void SwitchPlayerInfoDisplay(int pnum) = 0;

	// DMFCBase::DisplayingPlayerInfo
	//
	//	Returns the pnum of who you are currently displaying, -1 if noone
	virtual int DisplayingPlayerInfo(void) = 0;
	
	// DMFCBase::GetTeamFromString
	//
	//		Returns the int value of a team based on a string, -1 if not a team
	virtual int GetTeamFromString(char *str) = 0;

	// DMFCBase::GetPlayerNum
	//
	//   Returns your playernum, useful when finding out if an event is referring to you (by looking at
	//   the playernum passed to the event handler, compared to your playernum
	virtual int GetPlayerNum(void) = 0;

	// DMFCBase::GetLocalRole
	//
	//   Returns your role in the multiplayer game.  The result returned will be either LR_SERVER or LR_CLIENT
	virtual int GetLocalRole(void) = 0;

	// DMFCBase::CheckPlayerNum
	//
	//    Returns true if the player number passed in is a valid player number (the player is connected)
	virtual bool CheckPlayerNum(int player_num) = 0;

	//	DMFCBase::PacketCheckPlayerNum
	//
	//		Returns true if it's ok to send a packet to this player
	virtual bool PacketCheckPlayerNum(int player_num) = 0;

	// DMFCBase::CallClientEvent
	//
	//    Server Only.  This will send an event to a client for it to execute.
	//    event = An EVT_CLIENT_*
	//    me_objnum,it_objnum = Object numbers of the objects to be me and it for the event
	//    destination = Player number of the client to send to, or -1 if to send to all
	virtual void CallClientEvent(int event,int me_objnum,int it_objnum,int destination,bool parms=false) = 0;

	// DMFCBase::GetTimeLeft
	//
	//    Server Only.  This will fill in the float pointer passed to it with how much time is
	//    left in the multiplayer game.  The value placed in the float is only valid if GetTimeLeft
	//    returns true.  If it returns false it was because either it is not a game with a time limit
	//    or it is being called on a client.
	virtual bool GetTimeLeft(float *time) = 0;

	// DMFCBase::EndLevel
	//
	//    Server Only.  This will end the current multiplayer level, and will go on to the next level.
	virtual void EndLevel(void) = 0;

	// DMFCBase::WarpToLevel
	//
	//	Warps to another level in the mission.  Pass a level number (1 to Current_mission.num_levels)
	//	It will end the level and go to that level.
	virtual void WarpToLevel(int lev) = 0;

	// DMFCBase::GetScoreLimit
	//
	//   Server Only.  This will fill in the int pointer passed to it with what the scoring limit
	//   is, set in in the multiplayer options.  The value point into the int is only valid if
	//   GetScoreLimit returns true.  If it returns false it is because you are either not the
	//   server or this option wasn't set in the multiplayer options
	virtual bool GetScoreLimit(int *limit) = 0;

	// DMFCBase::AutoTimeLimit
	//
	//    Server Only.  This turns off or on the automatic level ending by DMFC of a timed multiplayer game.
	//    If you turn it off, it is your responsibility to end a time multiplayer game when it's time
	//    is up.  If it is turned on, DMFC will automatically handle ending the game.  By default it is on.
	virtual void AutoTimeLimit(bool turnon) = 0;

	// DMFCBase::AutoDeathMessage
	//
	//    This turns on or off DMFC's automatic handling of death messages.  If it is turned on (Default)
	//    then when a player dies it will display a random death message from the list added by you using
	//    AddDeathMessage.  If it is turned off, then it is your responsibility to handle the messages, you
	//    can use DoRandomDeathMessage to display one when appropriate.
	virtual void AutoDeathMessage(bool turnon) = 0;

	// DMFCBase::AddDeathMessage
	//
	//    This will add a death message to DMFC.
	//    format = string in a "printf" type format (using %s for player callsigns) of the message
	//    victim_first = Set this to true if the victim is listed first in the format
	virtual void AddDeathMessage(char *string,bool victim_first=true) = 0;
			
	// DMFCBase::AddSuicideMessage
	//
	//    This will add a death message to DMFC.
	//    format = string in a "printf" type format (using %s for player callsigns) of the message
	virtual void AddSuicideMessage(char *string) = 0;

	// DMFCBase::DoRandomDeathMessage
	//
	//     DMFC will display a death message (or suicide message if killer and victim are the same) when
	//     this function is called, based on the strings added with AddDeathMessage or AddSuicideMessage.
	//     killernum = object number of the killer
	//     victimnum = object number of the victim
	//		    hash = hash index of the weapon killer, -1 if none
	virtual void DoRandomDeathMessage(int killernum,int victimnum,uint hash = -1) = 0;

	// DMFCBase::GetItObjNum
	//
	// Returns the it object number of the current event for use.
	virtual int GetItObjNum(void) = 0;

	// DMFCBase::GetMeObjNum
	//
	// Returns the me object number of the current event for use.
	virtual int GetMeObjNum(void) = 0;

	// DMFCBase::RegisterPacketReceiver
	//
	// Sets up a handler for a Special Packet ID.  When a special packet is recieved it's ID is compared
	// to the ID's given to this function.  If any match than it calls the handler given to process
	// the packet.
	// id = ID of the packet
	// func = Function handler to handle the packet.  Must be declared like void MyFunction(ubyte *data);
	virtual void RegisterPacketReceiver(ubyte id,void (*func)(ubyte *)) = 0;
	//@@@@virtual void RegisterPacketReceiver(ubyte id,void (DMFCBase::*func)(ubyte *)) = 0;

	// DMFCBase::StartPacket
	//
	//	 Initializes a packet so it is ready to be sent out.
	//	 data = pointer to buffer of data for packet.  This buffer should be MAX_GAME_DATA_SIZE in size, even
	//			if you don't plan on using all that data, there is header information that is added
	//			in this function.
	//   id = ID of the Special packet.  When the packet is recieved by whomever, this is the ID that determines which handler to
	//        call.
	//	 count = pointer to your packet index pointer
	virtual void StartPacket(ubyte *data,ubyte id,int *count) = 0;

	// DMFCBase::SendPacket
	//
	//   Ships a Special Packet away to the destination.  This call must be preceeded by a StartPacket call
	//	 data = buffer of data to be sent out (same buffer that was passed to StartPacket
	//   size = size (in bytes) of the packet
	//   destination = either a player number, SP_ALL for all the players or SP_SERVER to send to the server (if you are a client)
	virtual void SendPacket(ubyte *data,int size,int destination) = 0;

	// DMFCBase::GetTeamForNewPlayer
	//
	//    A Helper function (Server only), which will give you the optimum team assignment (whoever has the
	//    lowest amount of players on their team, for a new player.
	//    player_num = player number of the new player
	//    num_teams = The number of teams in your game
	virtual int GetTeamForNewPlayer(int player_num,int num_teams) = 0;

	// DMFCBase::SetNumberOfTeams
	//
	//    Sets the number of teams to be used in the game.  By default there is 1 team (everyone against everyone).
	//    You can set up to a maximum of 4 teams.  Call this function as soon as possible.
	//    teams = number of teams
	virtual void SetNumberOfTeams(int teams) = 0;

	// DMFCBase::AutoTeamSelect
	//
	//    Turns on or off DMFC's auto team assignment.  If it is on, then when a new player joins, they will
	//    be placed on the team with fewest players.  If it is off, then you must handle that.  Defualt on.
	virtual void AutoTeamSelect(bool turnon) = 0;

	// DMFCBase::SendTeamAssignment
	//
	//     A DMFC Special Packet function, this sends a team assignment packet to all the players. Server only.
	//     playernum = player to change teams
	//     team = new team
	virtual void SendTeamAssignment(int playernum,int team,bool spew_on_respawn) = 0;

	// DMFCBase::GetTeamAssignmentPacket
	//
	//     Reciever for the team assignment packet.
	virtual void GetTeamAssignmentPacket(ubyte *data) = 0;

	// DMFCBase::GetChangeTeamPacket
	//
	//     Reciever for the change team packet.(Server Only)
	virtual void GetChangeTeamPacket(ubyte *data) = 0;

	// DMFCBase::GetGameStateRequest
	//
	//		Receiver for the server from a client asking for the state of the game
	virtual void GetGameStateRequest(ubyte *data) = 0;

	// DMFCBase::SendChangeTeamRequest
	//
	//		Sends a request to the server to change teams (Client Only)
	//	spew_onchange should be true if you want the client the spew his inventory on the change
	virtual void SendChangeTeamRequest(int newteam,bool spew_onchange) = 0;

	// DMFCBase::GetDMFCGameInfo
	//
	//		Receives information about the DMFC game
	virtual void GetDMFCGameInfo(ubyte *data) = 0;

	// DMFCBase::SendDMFCGameInfo
	//
	//		Sends information about the DMFC game
	virtual void SendDMFCGameInfo(int player) = 0;

	// DMFCBase::ChangeTeams
	//
	//   If you are the server it will make the player change teams.  If you
	//   are a client then it sends a request to the server to change teams
	//	 spew_on_repsawn is true if the player should spew his inventory before he respawns
	virtual void RequestTeamChange(int team,int pnum,bool spew_on_respawn) = 0;

	// DMFCBase::RequestGameState
	//
	//		Sends a request to the server for the game state
	virtual void RequestGameState(void) = 0;

	// DMFCBase::AddHudItemCallback
	//
	//    Adds an item to the hud.  Everytime the hud needs to be updated, it will call the
	//    handler passed in.  The handler must be declared as:
	//	  void func(struct tHudItem *item);
	//    type = HI_BITMAP  for bitmap, HI_TEXT for a text item
	//    func = function callback
	virtual void AddHUDItemCallback(int type,void (*func)(struct tHUDItem *)) = 0;

	// DMFCBase::GetMyTeam
	//
	//    Returns the int value of the team you are on...only useful in a team game.
	virtual int GetMyTeam(void) = 0;

	// DMFCBase::GetTeamString
	//
	//     Returns a pointer to a string name of a team
	//   team = integer value of the team
	virtual const char *GetTeamString(int team) = 0;

	// DMFCBase::GetTeamColor
	//
	//    Returns the color components of a team
	//   team = integer value of the team
	virtual ddgr_color GetTeamColor(int team) = 0;

	// DMFCBase::GetNumTeams
	//
	//   Returns the number of teams in the game
	virtual int GetNumTeams(void) = 0;

	// DMFCBase::AllowTeamChange
	//
	//		Returns true if team changing is allowed
	virtual bool AllowTeamChange(void) = 0;

	// DMFCBase::SwitchAllowTeamChange
	//
	//		Turns on/off allowing of team changing
	virtual void SwitchAllowTeamChange(bool turnon) = 0;

	// DMFCBase::GetSortedPlayerSlots
	//
	//  Fills in the passed array (of size maxsize) with the playernums sorted by (kills-suicides)
	virtual void GetSortedPlayerSlots(int *sortedindex,int maxsize) = 0;

	//	DMFCBase::GetSortedPlayerSlotsByEfficiency
	//
	//	Fills in the passed array (of size maxsize) with the playernums sorted by kills/(kills+deaths+suicides)
	virtual void GetSortedPlayerSlotsByEfficiency(int *sortedindex,int maxsize) = 0;

	// DMFCBase::IsMenuUp
	//
	//	  Returns true if the on screen menu is being shown
	virtual bool IsMenuUp(void) = 0;

	// DMFCBase::ConvertHUDAlpha
	//
	//	  Returns a converted alpha based on what you give, it will be a more transparent if the onscreen menu is up
	virtual float ConvertHUDAlpha(float normal) = 0;
	virtual ubyte ConvertHUDAlpha(ubyte normal) = 0;

	// DMFCBase::ClipString
	//
	//	   Given a width, the string will be clipped to that width.  If you pass true for arrow, then an arrow will be
	//    appended if the string has been clipped.  Based off current font
	virtual void ClipString(int width,char *string,bool arrow) = 0;

	//DMFCBase::DisplayOutrageLogo(void)
	//
	//   Displays the Outrage Logo for 5 seconds (call repeatedly, after 5 seconds it will just short circuit)
	//	Does nothing for non-Outrage releases
	virtual void DisplayOutrageLogo(void) = 0;

	// DMFCBase::KillAllTimers
	//
	//	Kills all running timers
	virtual void KillAllTimers(void) = 0;

	// DMFCBase::SetTimerInterval
	//
	//	Starts a timer event running
	virtual int SetTimerInterval(void (*func)(void),float intval,float longevity,void (*onkill)(void)=NULL) = 0;

	//DMFCBase::KillTimer
	//
	//Kills a timer
	virtual void KillTimer(int handle) = 0;

	//DMFCBase::SwitchShowHudCallsignLevel
	//
	//
	//   Sets the level for displaying of Player's Callsigns on the HUD
	virtual void SwitchShowHudCallsignLevel(ubyte level,bool announce=true) = 0;

	//DMFCBase::SwitchServerHudCallsignLevel
	//
	//
	//	Sets the max level of HUD callsign displayage...determined by the server
	virtual void SwitchServerHudCallsignLevel(ubyte level) = 0;

	//DMFCBase::GetCounterMeasureOwner
	//
	//
	//	Given a counter measure it will determine the pnum of it's owner...if it can't find it, it returns -1
	virtual int GetCounterMeasureOwner(object *robot) = 0;

	//DMFCBase::CFGOpen
	//
	//	Opens the registry/cfg information for the DMFC game, see error codes for return values
	virtual int CFGOpen(char *filename) = 0;

	//DMFCBase::CFGClose
	//
	//	Closes the registry information for the game.
	virtual void CFGClose(void) = 0;

	//DMFCBase::CFGFlush
	//
	//	Flushes out the registry information to fall
	virtual int CFGFlush(void) = 0;

	//DMFCBase::CFGCreateKey
	//
	//  Creates a key in the registry
	virtual int CFGCreateKey(char *name) = 0;

	//DMFCBase::CFGLookupKey
	//
	//   Sets the active key in the registry
	virtual int CFGLookupKey(char *name) = 0;

	//DMFCBase::CFGLookupRecord
	//
	//	Looks up a record in the active key
	virtual int CFGLookupRecord(char *record,void *data) = 0;

	//DMFCBase::CFGCreateRecord
	//
	// Create/overwrites a record in the active key. Type is either REGT_DWORD or REGT_STRING
	virtual int CFGCreateRecord(char *name,char type,void *data) = 0;

	//DMFCBase::DecryptData
	//
	// Decrypts a buffer of data
	virtual void DecryptData(ubyte *data,int size) = 0;

	//DMFCBase::EncryptData
	//
	//	Encrypts (weak) a buffer of data
	virtual void EncryptData(ubyte *data,int size) = 0;

	//DMFCBase::VersionCheck
	//
	// Makes sure Client has same DMFC version
	virtual void VersionCheck(int pnum) = 0;

	// DMFCBase::UpdatePInfo
	//
	//
	//	Updates a victim's pinfo stat
	virtual void UpdatePInfo(int victim,int killer,int amount) = 0;

	// DMFCBase::ResetPInfo
	//
	//
	//	Resets all the PInfo stats (and frees memory)
	virtual void ResetPInfo(void) = 0;

	// DMFCBase::WriteDMFCStatsToFile
	//
	//
	//	This function will write out DMFC's stats to the file given (it must be opened for writing)
	virtual void WriteDMFCStatsToFile(CFILE *file) = 0;

	// DMFCBase::SetWeaponDeathMessage
	//
	//	Sets a death message for a weapon kill (you only need to pass in one version of the weapon if it 
	//  consists of multiple weapons...make sure AddWeaponHash has been called before this for this weapon)
	virtual bool SetWeaponDeathMessage(char *weapon_name,char *message,bool victim_first) = 0;

	// DMFCBase::GetWeaponDeathMessage
	//
	//  Returns the format string for a weapon death message, NULL if it doesn't exist
	virtual char *GetWeaponDeathMessage(int index,bool *victim_first) = 0;


	// DMFCBase::AddWeaponHash
	//
	//	Since one weapon may actually consist of many weapons, in order to save space you can create
	//  one weapon where all those other weapon id's will be mapped to it...use WeaponHash[id] to
	//  get the actual weapon.  End list of children with a NULL
	virtual void AddWeaponHash(char *parent, ... ) = 0;

	// DMFCBase::AddWeaponHashArray
	//
	//	Since one weapon may actually consist of many weapons, in order to save space you can create
	//  one weapon where all those other weapon id's will be mapped to it...use WeaponHash[id] to
	//  get the actual weapon.
	virtual void AddWeaponHashArray(char *parent,int count,char **array) = 0;

	// DMFCBase::SetupPlayerRecord
	//
	//	Sets up memory for multiplayer mod user defined data in the Player Records. Pass in the size (in bytes)
	//	of 1 struct (each player will get one).  Use GetPlayerRecordData to retrieve a pointer to the data.
	//
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
	virtual int SetupPlayerRecord(int sizeof_individual_data,int (*pack_callback)(void *user_info,ubyte *data),int (*unpack_callback)(void *user_info,ubyte *data)) = 0;

	// DMFCBase::GetPlayerRecordData
	//
	//	Returns a pointer to the memory set aside for a particular player (NULL on error)
	virtual void *GetPlayerRecordData(int pnum) = 0;

	// DMFCBase::GetPlayerRecord
	//
	//	Returns a pointer to the player record at the given slot (passed in slot must be in the range 0<=slot<MAX_PLAYER_RECORDS)
	virtual player_record *GetPlayerRecord(int slot) = 0;

	// DMFCBase::GetPlayerRecordByPnum
	//
	//	Same as above, but you can specify by a player's pnum, return NULL on error
	virtual player_record *GetPlayerRecordByPnum(int pnum) = 0;

	//DMFCBase::SendRequestForPlayerRecords
	//
	//
	//	Sends a request to the server to send the player records
	virtual void SendRequestForPlayerRecords(void) = 0;

	//DMFCBase::ReceiveRequestForPlayerRecords
	//
	//
	//	Recieves and processes a request for a player record
	virtual void ReceiveRequestForPlayerRecords(ubyte *data) = 0;

	// DMFCBase::IsPlayerBanned
	//
	//	returns true is the given pnum is banned from the game
	virtual bool IsPlayerBanned(int pnum) = 0;

	// DMFCBase::IsAddressBanned
	//
	//	returns true if the given address is banned from the game
	virtual bool IsAddressBanned(network_address *addr,char *tracker_id) = 0;

	// DMFCBase::RemoveAllBans
	//
	//	Removes all the temp bans
	virtual void RemoveAllBans(void) = 0;

	// DMFCBase::RemoveBan
	//
	//	Removes a temp ban on given the ban #...returns true on success
	virtual bool RemoveBan(int index) = 0;

	// DMCBase::GetBannedPlayerCallsign
	//
	//	Returns the callsign of the banned player at position index, NULL on error
	virtual char *GetBannedPlayerCallsign(int index) = 0;

	// DMFCBase::GetNumBannedPlayers
	//
	//	Returns the number of players banned from the server
	virtual int GetNumBannedPlayers(void) = 0;

	// DMFCBase::BanPlayerFromGame
	//
	//   Puts a temp ban on a player which will only last the duration of the game
	virtual void BanPlayerFromGame(int pnum) = 0;

	// DMFCBase::IsPlayerAlive
	//
	//	Returns true is the given pnum is a player, flying around the level (not dying, 
	//	dead or observing)
	virtual bool IsPlayerAlive(int pnum) = 0;

	// DMFCBase::SendControlMessageToPlayer
	//
	//
	//	Sends a [1 byte] control message to a player
	virtual void SendControlMessageToPlayer(int pnum,ubyte msg) = 0;

	// DMFCBase::ReceiveControlMessage
	//
	//
	//	Handles a control message
	virtual void ReceiveControlMessage(ubyte *data) = 0;

	// DMFCBase::FindPInfoStatFirst
	//
	//	Fills in a tPInfoStat that contains information about a given player record slot. Returns true on success, false
	//	if there is no information available for the given slot. Call this first, then follow with repeated calls to
	//	FindPInfoStatNext until you get a false value returned...call FindPInfoStatClose when done.
	virtual bool FindPInfoStatFirst(int slot,tPInfoStat *stat) = 0;

	// DMFCBase::FindPInfoStatNext
	//
	//	Call this repeatedly until you get a value of false, finish by calling FindPInfoStatClose
	virtual bool FindPInfoStatNext(tPInfoStat *stat) = 0;

	// DMFCBase::FindPInfoStatClose
	//
	//	Closes up a FindPInfo series of calls
	virtual void FindPInfoStatClose(void) = 0;

	// DMFCBase::DoDamageToPlayer
	//
	//	Server only...applies damage to player
	virtual void DoDamageToPlayer(int pnum,int type, float amount,bool playsound=true) = 0;

	// DMFCBase::StartUIWindow
	//
	//	Prepares the game so that you can display UI stuff, give a unique id in which will be passed
	//	to the OnClientShowUI().  In your OnClientShowUI() you must ONLY handle id's which belong to you
	//	and pass the others to DMFCBase::OnClientShowUI().
	virtual void StartUIWindow(int id,void *user_data) = 0;

	//	DMFCBase::DisconnectMe
	//
	//	Disconnects yourself from the game.  It should be called when you are kicked or banned to make the
	//	whole process a bit nicer/cleaner.  Even if it doesn't get called when kicked, you will still
	//	eventually disconnect.
	virtual void DisconnectMe(void) = 0;


	/*			Input Command Functions				*/
	// DMFCBase::AddInputCommand
	//
	//	When the client (or dedicated server) types a message that begins with a $, DMFC will look through
	//	all the values you passed to AddInputCommand() and see if any match.  If so it passes the entire
	//	command string to the given function handler. Returns 1 on success, -1 if out of memory, 0 if it already
	//	exists. These commands are not case sensitive.
	//	Ex. AddInputCommand("team");	//this handles all the '$team' passed in
	//  allow_remotely : if set true, this input command can be called remotely via remote administration
	virtual signed char AddInputCommand(char *command,char *description,void (*handler)(char *),bool allow_remotely=false) = 0;

	//	Does a check on on the pinfo info making sure it is valid
	virtual void CheckPInfo() = 0;

	//	DMFCBase::EnableStatisticalMessages
	//
	//	Turns on or off the statistical HUD messages that appear due to a player death
	virtual void EnableStatisticalMessages(bool on) = 0;

	//	DMFCBase::EnableOnScreenMenu
	//
	// Turns on/off the onscreen menu
	virtual void EnableOnScreenMenu(bool turn_on) = 0;

	//	DMFCBase::EnableAutoSaveLevelEnd
	//
	//	Disables/Enables the autosaving of game stats to file on level end
	virtual void EnableAutoSaveLevelEnd(bool enable) = 0;

	//	DMFCBase::EnableAutoSaveDisconnect
	//
	//	Disables/Enables the autosaving of game stats to file on disconnect from the game
	virtual void EnableAutoSaveDisconnect(bool enable) = 0;

	//	DMFCBase::GenerateStatFilename
	//
	//	Given the following information it will return a full path to what
	//	the recommended filename to save stats to should be.
	//	root = Multiplayer DLL Name (filename will start with this)
	//	end_of_level = pass true if this is the end of a level stats
	virtual void GenerateStatFilename(char *filename,char *root,bool end_of_level) = 0;

	//	DMFCBase::IsPlayerObserver
	//
	//	Returns true if the given pnum is currently an observer in the game, else returns false.
	//	If an invalid pnum is given, then it returns false
	virtual bool IsPlayerObserver(int pnum) = 0;

	//	DMFCBase::EnableOnScreenMenuBackground
	//
	//	Enables/disables the onscreen menu background 
	virtual void EnableOnScreenMenuBackground(bool enable) = 0;

	//	DMFCBase::GetTimeInGame
	//
	//	Returns the totaltime the player has been in the game at the moment you call
	//	this function.  Pass in the player record slot.
	virtual float GetTimeInGame(int slot) = 0;

	//	DMFCBase::GetTimeString
	//
	//	Returns a string that contains a formated time (i.e. 32 seconds, 1:15 minutes, 3:21:23 hours)
	virtual char *GetTimeString(float sec) = 0;

	//	DMFCBase::DisplayNetGameInfo
	//
	//	Displays information about the Netgame that is currently being played
	//	on to the screen.
	virtual void DisplayNetGameInfo(int background_bmp=-1,bool dedicated_server=false) = 0;

	// DMFCBase::SwitchNetGameInfoDisplay
	//
	//	Switches on/off displaying netgame info
	virtual void SwitchNetGameInfoDisplay(int on) = 0;

	//	DMFCBase::IsDisplayingNetGameInfo
	//
	//	returns true if we are currently displaying netgame info
	virtual bool IsDisplayingNetGameInfo(void) = 0;

	// DatabaseRegister
	//		Registers your multiplayer game with the database.  This must be
	//	called before any other database function, or they will fail until this
	//	is called.
	virtual void DatabaseRegister(char *name) = 0;
	// DatabaseRead
	//		Reads a string from the database
	virtual bool DatabaseRead(const char *label,char *entry,int *entrylen) = 0;
	// DatabaseRead
	//		Reads wordsize bytes from the database
	virtual bool DatabaseRead(const char *label, void *entry, int wordsize) = 0;
	// DatabaseRead
	//		Reads a bool from the database
	virtual bool DatabaseRead(const char *label, bool *entry) = 0;
	// DatabaseWrite
	//		Writes/Updates a string to the database
	virtual bool DatabaseWrite(const char *label, const char *entry, int entrylen) = 0;
	// DatabaseWrite
	//		Writes/Updates a value to the database
	virtual bool DatabaseWrite(const char *label, int entry) = 0;
	// DMFCBase::SendNetGameInfoSync
	//
	//	Sends out a NetGame info sync packet to all the players
	virtual void SendNetGameInfoSync(int to_who=SP_ALL) = 0;

	// CompareNetworkAddress
	//
	//	Compare's two network addresses, returns true if they are the same, false if not.  
	//	use_port: if this is true, than it will consider the port part of the network address
	virtual bool CompareNetworkAddress(network_address *one,network_address *two,bool use_port=true) = 0;

	// IsMasterTrackerGame
	//
	//	returns true if this game is being played on the master tracker
	virtual bool IsMasterTrackerGame(void) = 0;

	//	ConvertLocalToServerObjnum
	//
	//	Given an objnum, it will convert the number from your local objnum to the server's objnum
	//	It will return -1 on error
	virtual int ConvertLocalToServerObjnum(int objnum) = 0;

	//	ConvertServerToLocalObjnum
	//
	//	Given an objnum from the server, this function will convert the objnum to your local objnum
	//  It will return -1 on error
	virtual int ConvertServerToLocalObjnum(int objnum) = 0;

	//DMFCBase::ShouldIDisplayHUDName
	//
	//	Given a player num, it will determine if the callsign should be drawn on the HUD
	virtual bool ShouldIDisplayHUDName(int pnum) = 0;

	//DMFCBase::SetDeathMessageFilter
	//
	// Sets the death message filter
	virtual void SetDeathMessageFilter(int level) = 0;

	//DMFCBase::IAmDedicatedServer
	//
	//	Returns true if we are a dedicated server
	virtual bool IAmDedicatedServer(void) = 0;

	//DMFCBase::IsPlayerDedicatedServer
	//
	//	Returns true if the passed in pnum/playerrecord is a dedicated server
	virtual bool IsPlayerDedicatedServer(int pnum) = 0;
	virtual bool IsPlayerDedicatedServer(player_record *pr) = 0;

	//	Displays dedicated server help
	virtual void DisplayInputCommandHelp(char *s) = 0;

	//DMFCBase::GetPlayerTeam
	//
	//	Returns the team of the player...call this instead of accessing .team directly
	virtual int GetPlayerTeam(int pnum) = 0;

	//DMFCBase::SetTeamName
	//
	//	Sets the team name for a given team
	//	team:	integer value of the team to change
	//	name:	new name for the team
	//	announce:	if this is true, and we are the server, it will tell all the clients about the change
	virtual bool SetTeamName(int team,char *name,bool announce) = 0;

	//DMFCBase::ConvertHUDCoord
	//
	//	Given an x,y based on a virtual 640x480 screen, this will convert it to the x,y that should be
	//	used based on the current screen size
	virtual void ConvertHUDCoord(int x,int y,int *rx,int *ry) = 0;

	//DMFCBase::GetPlayerLogoBmp
	//
	//	Given a player_num, it will return a bitmap handle to that player's ship logo, or 
	//	-1 if they don't have a logo for their ship.
	//	if is_vclip comes back as true, than it is not a bitmap handle, but a handle
	//	to a vclip (animated bitmap).  It is an index into the DLLGameVClips[].
	virtual int GetPlayerLogoBmp(int player_num,bool *is_vclip) = 0;

	//DMFCBase::EnableShipLogos
	//
	//	Disables or enables logo displaying for the client
	virtual void EnableShipLogos(bool enable) = 0;

	//DMFCBase::AreLogosEnabled
	//
	//	returns true if ship logos are enabled
	virtual bool AreLogosEnabled(void) = 0;

	//DMFCBase::EnableAudioTaunts
	//
	//	Disables or enables audio taunts
	virtual void EnableAudioTaunts(bool enable) = 0;

	//DMFCBase::AreTauntsEnabled(void)
	//
	//	returns true if audio taunts are enabled
	virtual bool AreTauntsEnabled(void) = 0;

	//DMFCBase::EnableTimelimitCountdown
	//
	//	Enables/Disables the timelimit countdown (if there is a level time limit)
	//	Optionally you can specify what time to start the count down.
	virtual void EnableTimelimitCountdown(bool enable,int seconds = 10) = 0;

	//DMFCBase::RespawnPlayer
	//
	//	Takes the given player and respawns his at a random start point (in a team game, team respawn
	//	points are taken into consideration, so make sure the player is on the correct team before you
	//	respawn him.  spew_everything, if false, will override spew_energy_and_shields
	virtual void RespawnPlayer(int pnum,bool spew_energy_and_shield,bool spew_everything) = 0;

	//If you are going to create submenus you MUST use this function. along with:
	//void SetState(int state);
	//bool SetStateItemList(int count, ... ); for MIT_STATE items
	//or
	//bool SetStateItemListArray(int count, char **array);	for MIT_STATE items
	//virtual MenuItem *CreateMenuItem(char *title,char type,ubyte flags,void (*fp)(int), ... ) = 0;

	// DMFCBase::ReadInHostsAllowDeny
	//	
	//	Reads in the hosts.allow and hosts.deny files (if available)
	virtual void ReadInHostsAllowDeny(void) = 0;

	// DMFCBase::FreeHostsLists
	//	
	//	Frees all the memory allocated for the host allow/deny lists
	virtual void FreeHostsLists(void) = 0;

	// DMFCBase::RehashAllowDeny
	//
	//	Flushes and reloads the hosts.allow/.deny lists
	virtual void RehashAllowDeny(void) = 0;

	//	DMFCBase::AnnounceTeamChangeDeny
	//
	//	Tells a player that team change request was denied
	virtual void AnnounceTeamChangeDeny(int pnum) = 0;

	// DMFCBase::SetMaxPlayerHardLimit
	//
	//	Sets a hard limit to the max number of players allowed in the game
	//	changing the number of players can never go above this...defaults
	//	to DLLMAX_PLAYERS
	virtual void SetMaxPlayerHardLimit(int max) = 0;

	//	DMFCBase::WasPlayerInGameAtLevelEnd
	//
	//	Given a player record it returns the pnum of the player at that slot
	//	IF (and only if) the player was in the game at level end, else it 
	//	returns -1.
	virtual int WasPlayerInGameAtLevelEnd(int prec) = 0;

	//DMFCBase::SelectNextCameraView
	//
	//	This function, given which window (corresponds to left, middle, right), switches the 
	//	current view of the small camera windows on the screen
	virtual void SelectNextCameraView(int window) = 0;

	// =======================================================
	//	The following Set_ functions are to be used to set callback
	//	to event handlers.  If you set a callback for an event handler
	//	the callback will be called instead of the default handler.
	//	It is your responsibility to also call the default handler
	//	inside of your callback, to insure proper processesing.
	// ========================================================
	virtual void Set_OnServerPlayerKilled(void (*callback)(object *killer_obj,int victim_pnum)) = 0;
	virtual void Set_OnServerPlayerExploded(void (*callback)(int player_num)) = 0;
	virtual void Set_OnServerCollide(void (*callback)(object *me_obj,object *it_obj)) = 0;
	virtual void Set_OnServerCollide(void (*callback)(object *me_obj,object *it_obj,vector *point,vector *normal)) = 0;
	virtual void Set_OnServerPlayerChangeSegment(void (*callback)(int player_num,int newseg,int oldseg)) = 0;
	virtual void Set_OnServerObjectChangeSegment(void (*callback)(object *obj,int newseg,int oldseg)) = 0;
	virtual void Set_OnServerPlayerEntersGame(void (*callback)(int player_num)) = 0;
	virtual void Set_OnServerPlayerDisconnect(void (*callback)(int player_num)) = 0;
	virtual void Set_OnServerGameCreated(void (*callback)(void)) = 0;
	virtual void Set_OnServerLevelChange(void (*callback)(void)) = 0;
	virtual void Set_OnServerLevelStart(void (*callback)(void)) = 0;
	virtual void Set_OnServerLevelEnd(void (*callback)(void)) = 0;
	virtual void Set_OnServerObjectShieldsChanged(void (*callback)(object *obj,float amount)) = 0;
	virtual void Set_OnServerIsAddressBanned(bool (*callback)(network_address *addr,char *tracker_id)) = 0;
	virtual void Set_OnServerWallCollide(void (*callback)(object *obj,float hitspeed,int hitseg,int hitwall,vector * hitpt,vector *wall_normal,float hit_dot)) = 0;
	virtual void Set_OnServerObjectKilled(void (*callback)(object *obj,object *killer)) = 0;
	virtual void Set_OnServerObjectDestroyed(void (*callback)(object *obj)) = 0;
	virtual void Set_OnClientPlayerKilled(void (*callback)(object *killer_obj,int victim_pnum)) = 0;
	virtual void Set_OnClientPlayerExploded(void (*callback)(int player_num)) = 0;
	virtual void Set_OnClientCollide(void (*callback)(object *me_obj,object *it_obj)) = 0;
	virtual void Set_OnClientCollide(void (*callback)(object *me_obj,object *it_obj,vector *point,vector *normal)) = 0;
	virtual void Set_OnClientPlayerChangeSegment(void (*callback)(int player_num,int newseg,int oldseg)) = 0;
	virtual void Set_OnClientObjectChangeSegment(void (*callback)(object *obj,int newseg,int oldseg)) = 0;
	virtual void Set_OnClientPlayerEntersGame(void (*callback)(int player_num)) = 0;
	virtual void Set_OnClientPlayerDisconnect(void (*callback)(int player_num)) = 0;
	virtual void Set_OnMeDisconnectFromServer(void (*callback)(void)) = 0;
	virtual void Set_OnClientGameCreated(void (*callback)(void)) = 0;
	virtual void Set_OnClientObjectShieldsChanged(void (*callback)(object *obj,float amount)) = 0;
	virtual void Set_OnWeaponFired(void (*callback)(object *weapon_obj,object *shooter)) = 0;
	virtual void Set_OnClientLevelChange(void (*callback)(void)) = 0;
	virtual void Set_OnClientLevelStart(void (*callback)(void)) = 0;
	virtual void Set_OnClientLevelEnd(void (*callback)(void)) = 0;
	virtual void Set_OnClientWallCollide(void (*callback)(object *obj,float hitspeed,int hitseg,int hitwall,vector * hitpt,vector *wall_normal,float hit_dot)) = 0;
	virtual void Set_OnClientObjectKilled(void (*callback)(object *obj,object *killer)) = 0;
	virtual void Set_OnClientObjectDestroyed(void (*callback)(object *obj)) = 0;
	virtual void Set_OnPlayerEntersObserver(void (*callback)(int pnum,object *piggy)) = 0;
	virtual void Set_OnPlayerExitsObserver(void (*callback)(int pnum)) = 0;
	virtual void Set_OnCanChangeTeam(bool (*callback)(int pnum,int newteam)) = 0;
	virtual void Set_OnSpecialPacket(void (*callback)(void)) = 0;
	virtual void Set_OnInterval(void (*callback)(void)) = 0;
	virtual void Set_OnHUDInterval(void (*callback)(void)) = 0;
	virtual void Set_OnPLRInterval(void (*callback)(void)) = 0;
	virtual void Set_OnPLRInit(void (*callback)(void)) = 0;
	virtual void Set_OnKeypress(void (*callback)(int key)) = 0;
	virtual void Set_OnInputString(void (*callback)(char *input_string)) = 0;
	virtual void Set_OnPlayerChangeTeam(void (*callback)(int player_num,int newteam,bool announce,bool spew_everything)) = 0;
	virtual void Set_OnGameStateRequest(void (*callback)(int pnum)) = 0;
	virtual void Set_OnSaveStatsToFile(void (*callback)(void)) = 0;
	virtual void Set_OnPlayerReconnect(void (*callback)(int player_num)) = 0;
	virtual void Set_OnPlayerConnect(void (*callback)(int player_num)) = 0;
	virtual void Set_OnControlMessage(void (*callback)(ubyte msg,int from_pnum)) = 0;
	virtual void Set_OnAllowObserverChange(bool (*callback)(bool turnonobserver)) = 0;
	virtual void Set_OnClientShowUI(void (*callback)(int id,void *user_data)) = 0;
	virtual void Set_OnPrintScores(void (*callback)(int level)) = 0;
	virtual void Set_OnDisconnectSaveStatsToFile(void (*callback)(void)) = 0;
	virtual void Set_OnLevelEndSaveStatsToFile(void (*callback)(void)) = 0;
	virtual void Set_OnGetHudCallSignColor(ddgr_color (*callback)(int playernum)) = 0;
	virtual void Set_OnTeamChangeName(void (*callback)(int team,char *oldname,char *newname)) = 0;
	virtual void Set_OnDoControls(void (*callback)(game_controls *controls)) = 0;
	virtual void Set_OnPlayAudioTaunt(void (*callback)(int pnum)) = 0;
	virtual void Set_OnGetTokenString(void (*callback)(char *src,char *dest,int dest_size)) = 0;

	// ========================================================
	//	The following Call functions are to be used to simulate
	//	an event, you can call them wherever, if there is a call
	//	back set for what you call, it will executed, else the
	//	default handler will be called.  DO NOT CALL THE
	//	CORRESPONDING Call FUNCTION INSIDE OF AN EVENT HANDLER
	//	FOR THAT EVENT, AN INFINITE LOOP WILL RESULT.
	// ========================================================
	virtual void CallOnServerPlayerKilled(object *killer_obj,int victim_pnum) = 0;
	virtual void CallOnServerPlayerExploded(int player_num) = 0;
	virtual void CallOnServerCollide(object *me_obj,object *it_obj) = 0;
	virtual void CallOnServerCollide(object *me_obj,object *it_obj,vector *point,vector *normal) = 0;
	virtual void CallOnServerPlayerChangeSegment(int player_num,int newseg,int oldseg) = 0;
	virtual void CallOnServerObjectChangeSegment(object *obj,int newseg,int oldseg) = 0;
	virtual void CallOnServerPlayerEntersGame(int player_num) = 0;
	virtual void CallOnServerPlayerDisconnect(int player_num) = 0;
	virtual void CallOnServerGameCreated(void) = 0;
	virtual void CallOnServerLevelChange(void) = 0;
	virtual void CallOnServerLevelStart(void) = 0;
	virtual void CallOnServerLevelEnd(void) = 0;
	virtual void CallOnServerObjectShieldsChanged(object *obj,float amount) = 0;
	virtual bool CallOnServerIsAddressBanned(network_address *addr,char *tracker_id) = 0;
	virtual void CallOnServerWallCollide(object *obj,float hitspeed,int hitseg,int hitwall,vector * hitpt,vector *wall_normal,float hit_dot) = 0;
	virtual void CallOnServerObjectKilled(object *obj,object *killer) = 0;
	virtual void CallOnServerObjectDestroyed(object *obj) = 0;
	virtual void CallOnClientPlayerKilled(object *killer_obj,int victim_pnum) = 0;
	virtual void CallOnClientPlayerExploded(int player_num) = 0;
	virtual void CallOnClientCollide(object *me_obj,object *it_obj) = 0;
	virtual void CallOnClientCollide(object *me_obj,object *it_obj,vector *point,vector *normal) = 0;
	virtual void CallOnClientPlayerChangeSegment(int player_num,int newseg,int oldseg) = 0;
	virtual void CallOnClientObjectChangeSegment(object *obj,int newseg,int oldseg) = 0;
	virtual void CallOnClientPlayerEntersGame(int player_num) = 0;
	virtual void CallOnClientPlayerDisconnect(int player_num) = 0;
	virtual void CallOnMeDisconnectFromServer(void) = 0;
	virtual void CallOnClientGameCreated(void) = 0;
	virtual void CallOnClientObjectShieldsChanged(object *obj,float amount) = 0;
	virtual void CallOnWeaponFired(object *weapon_obj,object *shooter) = 0;
	virtual void CallOnClientLevelChange(void) = 0;
	virtual void CallOnClientLevelStart(void) = 0;
	virtual void CallOnClientLevelEnd(void) = 0;
	virtual void CallOnClientWallCollide(object *obj,float hitspeed,int hitseg,int hitwall,vector * hitpt,vector *wall_normal,float hit_dot) = 0;
	virtual void CallOnClientObjectKilled(object *obj,object *killer) = 0;
	virtual void CallOnClientObjectDestroyed(object *obj) = 0;
	virtual void CallOnPlayerEntersObserver(int pnum,object *piggy) = 0;
	virtual void CallOnPlayerExitsObserver(int pnum) = 0;
	virtual bool CallOnCanChangeTeam(int pnum,int newteam) = 0;
	virtual void CallOnSpecialPacket(void) = 0;
	virtual void CallOnInterval(void) = 0;
	virtual void CallOnHUDInterval(void) = 0;
	virtual void CallOnPLRInterval(void) = 0;
	virtual void CallOnPLRInit(void) = 0;
	virtual void CallOnKeypress(int key) = 0;
	virtual void CallOnInputString(char *input_string) = 0;
	virtual void CallOnPlayerChangeTeam(int player_num,int newteam,bool announce,bool spew_everything) = 0;
	virtual void CallOnGameStateRequest(int pnum) = 0;
	virtual void CallOnSaveStatsToFile(void) = 0;
	virtual void CallOnPlayerReconnect(int player_num) = 0;
	virtual void CallOnPlayerConnect(int player_num) = 0;
	virtual void CallOnControlMessage(ubyte msg,int from_pnum) = 0;
	virtual bool CallOnAllowObserverChange(bool turnonobserver) = 0;
	virtual void CallOnClientShowUI(int id,void *user_data) = 0;
	virtual void CallOnPrintScores(int level) = 0;
	virtual void CallOnDisconnectSaveStatsToFile(void) = 0;
	virtual void CallOnLevelEndSaveStatsToFile(void) = 0;
	virtual ddgr_color CallOnGetHudCallSignColor(int playernum) = 0;
	virtual void CallOnTeamChangeName(int team,char *oldname,char *newname) = 0;
	virtual void CallOnDoControls(game_controls *controls) = 0;
	virtual void CallOnPlayAudioTaunt(int pnum) = 0;
	virtual void CallOnGetTokenString(char *src,char *dest,int dest_size) = 0;

public:
	/*
	*************************************************************
	NEW INTERFACE STUFF
	*************************************************************
	*/
	//dllinfo *Data;					//Current event's data
	virtual dllinfo *GetDLLInfoCallData(void) = 0;
	//int *Highest_room_index;	//the highest room index (for room number validation)
	virtual int GetHighestRoomIndex(void) = 0;
	//int *Game_window_w;			//visible screen width
	virtual int GetGameWindowW(void) = 0;
	//int *Game_window_h;			//visible screen height
	virtual int GetGameWindowH(void) = 0;
	//int *Game_window_x;			//visible screen top-left x
	virtual int GetGameWindowX(void) = 0;
	//int *Game_window_y;			//visible screen top-left y
	virtual int GetGameWindowY(void) = 0;
	//int *Game_fonts;				//array of fonts
	virtual int *GetGameFontTranslateArray(void) = 0;
	//int hBitmapObserver;			//Bitmap handle for the 'in observer mode' icon
	virtual int GetObserverModeBitmap(void) = 0;
	//float *Frametime;				//how long it took the last frame to display
	virtual float GetFrametime(void) = 0;
	//float *Gametime;				//how long the game has been running
	//gametime doesn't pause in the wait for players dialogs
	virtual float GetGametime(void) = 0;
	//Gets the 'true' gametime...which maybe paused if the server is in the wait for players
	//dialog
	virtual float GetRealGametime(bool *ispaused=NULL) = 0;
	//float *ShieldDelta;			//array of player's...server update's their shields based on this array
	virtual float *GetShieldDeltaArray(void) = 0;
	//float *Hud_aspect_x,*Hud_aspect_y;
	virtual float GetHudAspectX(void) = 0;
	virtual float GetHudAspectY(void) = 0;
	//char *LocalD3Dir;			//Path to the root of the Descent3 installation directory
	virtual const char *GetLocalD3Dir(void) = 0;
	//tMission *Current_mission;	//the current mission
	virtual const tMission *GetCurrentMission(void) = 0;	
	//room *Rooms;					//rooms array
	virtual room *GetRooms(void) = 0;
	//object *Objects;				//objects array
	virtual object *GetObjects(void) = 0;
	//terrain_segment *Terrain_seg;	//Terrain segment array
	virtual terrain_segment *GetTerrainSegs(void) = 0;
	//netgame_info *Netgame;			//Netgame info
	virtual netgame_info *GetNetgameInfo(void) = 0;
	//player *Players;					//Player's array
	virtual player *GetPlayers(void) = 0;
	//netplayer *NetPlayers;			//NetPlayer's array
	virtual netplayer *GetNetPlayers(void) = 0;
	//ship *Ships;						//Ship's array
	virtual ship *GetShips(void) = 0;
	//weapon *Weapons;					//Weapon's array
	virtual weapon *GetWeapons(void) = 0;
	//texture *GameTextures;			//Texture's array
	virtual texture *GetGameTextures(void) = 0;
	//poly_model *Poly_models
	virtual poly_model *GetGamePolyModels(void) = 0;
	//vclip *GameVClips;				//VClip array
	virtual vclip *GetGameVClips(void) = 0;
	//ddgr_color *Player_colors;
	virtual ddgr_color *GetPlayerColors(void) = 0;
	//game_controls Last_game_controls;	// The last frame's set of control data
	virtual game_controls GetLastGameControls(void) = 0;
	//int PilotPicBmpHandles[DLLMAX_PLAYERS];
	virtual int *GetPilotPicBitmapHandles(void) = 0;
	//object *Viewer_object;				//object the player's view is from
	virtual void GetViewerObjectPtr(object **v_obj) = 0;
	virtual void SetViewerObjectPtr(object *v_obj) = 0;
	//float *Render_zoom;
	virtual float GetRenderZoom(void) = 0;

	//MenuItem Menu;???
	virtual IMenuItem *GetOnScreenMenu(void) = 0;
	//game_api API;??
	virtual tOSIRISModuleInit *GetOsirisModuleData(void) = 0;
	//get viseffect data
	virtual vis_effect *GetVisEffectArray(int **Highviseptr=NULL) = 0;
	//get current level information (watch for NULL return), although it should never be
	virtual level_info *GetLevelInfo() = 0;
	//gets the argument at the index passed	
	virtual char *GetGameArg(int arg) = 0;
	//returns the current state of the camera window
	virtual int GetCameraViewType(int window) = 0;
	// Given a generic object (OBJ_POWERUP,OBJ_ROBOT,OBJ_BUILDING or OBJ_CLUTTER) id
	// in the range of 0 to MAX_OBJECT_IDS, this returns a pointer to it's information (see objinfo.h)
	//	It returns NULL if an invalid id is given (or it's not used)
	virtual object_info *GetObjectInfo(int objinfo_id) = 0;
};

//===========================================================================================
/*
IMenuItem
	This is the interface class to Version 1.0 of MenuItem class (for OnScreenMenus)
*/
class IMenuItem : public IObject
{
public:
	virtual bool AddSubMenu(IMenuItem *p) = 0;
	virtual bool DetachSubMenu(IMenuItem *p) = 0;

	virtual bool Forward(void) = 0;
	virtual bool Back(void) = 0;
	virtual bool Up(void) = 0;
	virtual bool Down(void) = 0;

	virtual void Execute(void) = 0;
	virtual void Draw(int x,int y,int height,int backgroundbmp,float *ratio=NULL) = 0;

	virtual void SetInputFocus(void) = 0;
	virtual void LoseInputFocus(void) = 0;
	virtual bool GetFocus(void) = 0;

	virtual char *GetTitle(void) = 0;
	virtual int GetCustomSubMenuCount(void) = 0;

	virtual void CallFunc(int value) = 0;
	virtual bool HasSubMenus(void) = 0;
	virtual char GetType(void) = 0;

	virtual void SetState(int state) = 0;
	virtual bool SetStateItemList(int count, ... ) = 0;
	virtual bool SetStateItemListArray(int count, char **array ) = 0;
};

//===========================================================================================
/*
IDmfcStats
	This is the interface class to Version 1.0 of the CDmfcStats class
*/
class IDmfcStats : public IObject
{
public:
	//initialization member function, must be called first
	virtual bool Initialize(tDmfcStatsInit *init_info) = 0;

	//enables/disables the displaying of the stats screen
	virtual void Enable(bool enable) = 0;
	// returns the state of the stats manager
	virtual bool IsEnabled() = 0;

	//the do-it-all function, call once per frame
	virtual void DoFrame() = 0;
	// scrolls the player list down one
	virtual void ScrollDown() = 0;
	// scrolls the player list up one
	virtual void ScrollUp() = 0;
	// returns true if the player list can scroll down any more
	virtual bool CanScrollDown() = 0;
	// returns true if the player list can scroll up any more
	virtual bool CanScrollUp() = 0;
};

#endif //#ifdef USECLASSES

//===========================================================================================
#ifdef __cplusplus
extern "C" {
#endif

/*
Class creation interface functions
*/
EXTERN DLLEXPORT IDMFC		DLLFUNCCALLPTR CreateDMFC(void);
EXTERN DLLEXPORT IMenuItem	DLLFUNCCALLPTR CreateMenuItem(void);
EXTERN DLLEXPORT IMenuItem	DLLFUNCCALLPTR CreateMenuItemWArgs(char *title,char type,ubyte flags,void (*fp)(int),tCustomMenu *custom_menu=NULL);
EXTERN DLLEXPORT IDmfcStats	DLLFUNCCALLPTR CreateDmfcStats(void);

/***************************
IDMFC 1.0 Interface functions
****************************/
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_Delete(IDMFC *instance);
EXTERN DLLEXPORT void DLLFUNCCALLPTR IDMFC_Dynamic_Cast(IDMFC *instance,const char *pszType);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_DuplicatePointer(IDMFC *instance);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_DestroyPointer(IDMFC *instance);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_LoadFunctions(IDMFC *instance,int *api_func);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_GameInit(IDMFC *instance,int teams);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_GameClose(IDMFC *instance);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_OnServerPlayerKilled(IDMFC *instance,object *killer_obj,int victim_pnum);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_OnServerPlayerExploded(IDMFC *instance,int player_num);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_OnServerCollideA(IDMFC *instance,object *me_obj,object *it_obj);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_OnServerCollideB(IDMFC *instance,object *me_obj,object *it_obj,vector *point,vector *normal);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_OnServerPlayerChangeSegment(IDMFC *instance,int player_num,int newseg,int oldseg);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_OnServerObjectChangeSegment(IDMFC *instance,object *obj,int newseg,int oldseg);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_OnServerPlayerEntersGame(IDMFC *instance,int player_num);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_OnServerPlayerDisconnect(IDMFC *instance,int player_num);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_OnServerGameCreated(IDMFC *instance);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_OnServerLevelChange(IDMFC *instance);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_OnServerLevelStart(IDMFC *instance);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_OnServerLevelEnd(IDMFC *instance);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_OnServerObjectShieldsChanged(IDMFC *instance,object *obj,float amount);
EXTERN DLLEXPORT bool DLLFUNCCALL IDMFC_OnServerIsAddressBanned(IDMFC *instance,network_address *addr,char *tracker_id);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_OnServerWallCollide(IDMFC *instance,object *obj,float hitspeed,int hitseg,int hitwall,vector * hitpt,vector *wall_normal,float hit_dot);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_OnServerObjectKilled(IDMFC *instance,object *obj,object *killer);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_OnServerObjectDestroyed(IDMFC *instance,object *obj);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_OnClientPlayerKilled(IDMFC *instance,object *killer_obj,int victim_pnum);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_OnClientPlayerExploded(IDMFC *instance,int player_num);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_OnClientCollideA(IDMFC *instance,object *me_obj,object *it_obj);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_OnClientCollideB(IDMFC *instance,object *me_obj,object *it_obj,vector *point,vector *normal);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_OnClientPlayerChangeSegment(IDMFC *instance,int player_num,int newseg,int oldseg);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_OnClientObjectChangeSegment(IDMFC *instance,object *obj,int newseg,int oldseg);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_OnClientPlayerEntersGame(IDMFC *instance,int player_num);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_OnClientPlayerDisconnect(IDMFC *instance,int player_num);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_OnMeDisconnectFromServer(IDMFC *instance);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_OnClientGameCreated(IDMFC *instance);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_OnClientObjectShieldsChanged(IDMFC *instance,object *obj,float amount);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_OnWeaponFired(IDMFC *instance,object *weapon_obj,object *shooter);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_OnClientLevelChange(IDMFC *instance);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_OnClientLevelStart(IDMFC *instance);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_OnClientLevelEnd(IDMFC *instance);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_OnClientWallCollide(IDMFC *instance,object *obj,float hitspeed,int hitseg,int hitwall,vector * hitpt,vector *wall_normal,float hit_dot);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_OnClientObjectKilled(IDMFC *instance,object *obj,object *killer);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_OnClientObjectDestroyed(IDMFC *instance,object *obj);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_OnPlayerEntersObserver(IDMFC *instance,int pnum,object *piggy);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_OnPlayerExitsObserver(IDMFC *instance,int pnum);
EXTERN DLLEXPORT bool DLLFUNCCALL IDMFC_OnCanChangeTeam(IDMFC *instance,int pnum,int newteam);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_OnSpecialPacket(IDMFC *instance);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_OnInterval(IDMFC *instance);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_OnHUDInterval(IDMFC *instance);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_OnPLRInterval(IDMFC *instance);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_OnPLRInit(IDMFC *instance);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_OnKeypress(IDMFC *instance,int key);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_OnInputString(IDMFC *instance,char *input_string);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_OnPlayerChangeTeam(IDMFC *instance,int player_num,int newteam,bool announce,bool spew_everything);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_OnGameStateRequest(IDMFC *instance,int pnum);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_OnSaveStatsToFile(IDMFC *instance);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_OnPlayerReconnect(IDMFC *instance,int player_num);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_OnPlayerConnect(IDMFC *instance,int player_num);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_OnControlMessage(IDMFC *instance,ubyte msg,int from_pnum);
EXTERN DLLEXPORT bool DLLFUNCCALL IDMFC_OnAllowObserverChange(IDMFC *instance,bool turnonobserver);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_OnClientShowUI(IDMFC *instance,int id,void *user_data);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_OnPrintScores(IDMFC *instance,int level);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_OnDisconnectSaveStatsToFile(IDMFC *instance);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_OnLevelEndSaveStatsToFile(IDMFC *instance);
EXTERN DLLEXPORT ddgr_color DLLFUNCCALL IDMFC_OnGetHudCallSignColor(IDMFC *instance,int playernum);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_TranslateEvent(IDMFC *instance,int eventnum,dllinfo *data);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_OnTeamChangeName(IDMFC *instance,int team,char *oldname,char *newname);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_OnDoControls(IDMFC *instance,game_controls *controls);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_OnPlayAudioTaunt(IDMFC *instance,int pnum);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_DrawMenu(IDMFC *instance);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_DisplayPlayerInfo(IDMFC *instance,int background_bmp=-1,bool dedicated_server=false);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_SwitchPlayerInfoDisplay(IDMFC *instance,int pnum);
EXTERN DLLEXPORT int DLLFUNCCALL IDMFC_DisplayingPlayerInfo(IDMFC *instance);
EXTERN DLLEXPORT int DLLFUNCCALL IDMFC_GetTeamFromString(IDMFC *instance,char *str);
EXTERN DLLEXPORT int DLLFUNCCALL IDMFC_GetPlayerNum(IDMFC *instance);
EXTERN DLLEXPORT int DLLFUNCCALL IDMFC_GetLocalRole(IDMFC *instance);
EXTERN DLLEXPORT bool DLLFUNCCALL IDMFC_CheckPlayerNum(IDMFC *instance,int player_num);
EXTERN DLLEXPORT bool DLLFUNCCALL IDMFC_PacketCheckPlayerNum(IDMFC *instance,int player_num);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_CallClientEvent(IDMFC *instance,int event,int me_objnum,int it_objnum,int destination,bool parms=false);
EXTERN DLLEXPORT bool DLLFUNCCALL IDMFC_GetTimeLeft(IDMFC *instance,float *time);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_EndLevel(IDMFC *instance);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_WarpToLevel(IDMFC *instance,int lev);
EXTERN DLLEXPORT bool DLLFUNCCALL IDMFC_GetScoreLimit(IDMFC *instance,int *limit);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_AutoTimeLimit(IDMFC *instance,bool turnon);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_AutoDeathMessage(IDMFC *instance,bool turnon);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_AddDeathMessage(IDMFC *instance,char *string,bool victim_first=true);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_AddSuicideMessage(IDMFC *instance,char *string);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_DoRandomDeathMessage(IDMFC *instance,int killernum,int victimnum,uint hash = -1);
EXTERN DLLEXPORT int DLLFUNCCALL IDMFC_GetItObjNum(IDMFC *instance);
EXTERN DLLEXPORT int DLLFUNCCALL IDMFC_GetMeObjNum(IDMFC *instance);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_RegisterPacketReceiver(IDMFC *instance,ubyte id,void (*func)(ubyte *));
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_StartPacket(IDMFC *instance,ubyte *data,ubyte id,int *count);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_SendPacket(IDMFC *instance,ubyte *data,int size,int destination);
EXTERN DLLEXPORT int DLLFUNCCALL IDMFC_GetTeamForNewPlayer(IDMFC *instance,int player_num,int num_teams);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_SetNumberOfTeams(IDMFC *instance,int teams);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_AutoTeamSelect(IDMFC *instance,bool turnon);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_SendTeamAssignment(IDMFC *instance,int playernum,int team,bool spew_on_respawn);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_GetTeamAssignmentPacket(IDMFC *instance,ubyte *data);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_GetChangeTeamPacket(IDMFC *instance,ubyte *data);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_GetGameStateRequest(IDMFC *instance,ubyte *data);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_SendChangeTeamRequest(IDMFC *instance,int newteam,bool spew_onchange);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_GetDMFCGameInfo(IDMFC *instance,ubyte *data);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_SendDMFCGameInfo(IDMFC *instance,int player);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_RequestTeamChange(IDMFC *instance,int team,int pnum,bool spew_on_respawn);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_RequestGameState(IDMFC *instance);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_AddHUDItemCallback(IDMFC *instance,int type,void (*func)(struct tHUDItem *));
EXTERN DLLEXPORT int DLLFUNCCALL IDMFC_GetMyTeam(IDMFC *instance);
EXTERN DLLEXPORT char DLLFUNCCALLPTR IDMFC_GetTeamString(IDMFC *instance,int team);
EXTERN DLLEXPORT ddgr_color DLLFUNCCALL IDMFC_GetTeamColor(IDMFC *instance,int team);
EXTERN DLLEXPORT int DLLFUNCCALL IDMFC_GetNumTeams(IDMFC *instance);
EXTERN DLLEXPORT bool DLLFUNCCALL IDMFC_AllowTeamChange(IDMFC *instance);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_SwitchAllowTeamChange(IDMFC *instance,bool turnon);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_GetSortedPlayerSlots(IDMFC *instance,int *sortedindex,int maxsize);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_GetSortedPlayerSlotsByEfficiency(IDMFC *instance,int *sortedindex,int maxsize);
EXTERN DLLEXPORT bool DLLFUNCCALL IDMFC_IsMenuUp(IDMFC *instance);
EXTERN DLLEXPORT float DLLFUNCCALL IDMFC_ConvertHUDAlphaFloat(IDMFC *instance,float normal);
EXTERN DLLEXPORT ubyte DLLFUNCCALL IDMFC_ConvertHUDAlphaByte(IDMFC *instance,ubyte normal);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_ClipString(IDMFC *instance,int width,char *string,bool arrow);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_DisplayOutrageLogo(IDMFC *instance);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_KillAllTimers(IDMFC *instance);
EXTERN DLLEXPORT int DLLFUNCCALL IDMFC_SetTimerInterval(IDMFC *instance,void (*func)(void),float intval,float longevity,void (*onkill)(void)=NULL);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_KillTimer(IDMFC *instance,int handle);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_SwitchShowHudCallsignLevel(IDMFC *instance,ubyte level,bool announce=true);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_SwitchServerHudCallsignLevel(IDMFC *instance,ubyte level);
EXTERN DLLEXPORT int DLLFUNCCALL IDMFC_GetCounterMeasureOwner(IDMFC *instance,object *robot);
EXTERN DLLEXPORT int DLLFUNCCALL IDMFC_CFGOpen(IDMFC *instance,char *filename);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_CFGClose(IDMFC *instance);
EXTERN DLLEXPORT int DLLFUNCCALL IDMFC_CFGFlush(IDMFC *instance);
EXTERN DLLEXPORT int DLLFUNCCALL IDMFC_CFGCreateKey(IDMFC *instance,char *name);
EXTERN DLLEXPORT int DLLFUNCCALL IDMFC_CFGLookupKey(IDMFC *instance,char *name);
EXTERN DLLEXPORT int DLLFUNCCALL IDMFC_CFGLookupRecord(IDMFC *instance,char *record,void *data);
EXTERN DLLEXPORT int DLLFUNCCALL IDMFC_CFGCreateRecord(IDMFC *instance,char *name,char type,void *data);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_DecryptData(IDMFC *instance,ubyte *data,int size);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_EncryptData(IDMFC *instance,ubyte *data,int size);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_VersionCheck(IDMFC *instance,int pnum);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_UpdatePInfo(IDMFC *instance,int victim,int killer,int amount);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_ResetPInfo(IDMFC *instance);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_WriteDMFCStatsToFile(IDMFC *instance,CFILE *file);
EXTERN DLLEXPORT bool DLLFUNCCALL IDMFC_SetWeaponDeathMessage(IDMFC *instance,char *weapon_name,char *message,bool victim_first);
EXTERN DLLEXPORT char DLLFUNCCALLPTR IDMFC_GetWeaponDeathMessage(IDMFC *instance,int index,bool *victim_first);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_AddWeaponHash(IDMFC *instance,char *parent,int count, char **array);
EXTERN DLLEXPORT int DLLFUNCCALL IDMFC_SetupPlayerRecord(IDMFC *instance,int sizeof_individual_data,int (*pack_callback)(void *user_info,ubyte *data),int (*unpack_callback)(void *user_info,ubyte *data));
EXTERN DLLEXPORT void DLLFUNCCALLPTR IDMFC_GetPlayerRecordData(IDMFC *instance,int pnum);
EXTERN DLLEXPORT player_record DLLFUNCCALLPTR IDMFC_GetPlayerRecord(IDMFC *instance,int slot);
EXTERN DLLEXPORT player_record DLLFUNCCALLPTR IDMFC_GetPlayerRecordByPnum(IDMFC *instance,int pnum);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_SendRequestForPlayerRecords(IDMFC *instance);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_ReceiveRequestForPlayerRecords(IDMFC *instance,ubyte *data);
EXTERN DLLEXPORT bool DLLFUNCCALL IDMFC_IsPlayerBanned(IDMFC *instance,int pnum);
EXTERN DLLEXPORT bool DLLFUNCCALL IDMFC_IsAddressBanned(IDMFC *instance,network_address *addr,char *tracker_id);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_RemoveAllBans(IDMFC *instance);
EXTERN DLLEXPORT bool DLLFUNCCALL IDMFC_RemoveBan(IDMFC *instance,int index);
EXTERN DLLEXPORT char DLLFUNCCALLPTR IDMFC_GetBannedPlayerCallsign(IDMFC *instance,int index);
EXTERN DLLEXPORT int DLLFUNCCALL IDMFC_GetNumBannedPlayers(IDMFC *instance);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_BanPlayerFromGame(IDMFC *instance,int pnum);
EXTERN DLLEXPORT bool DLLFUNCCALL IDMFC_IsPlayerAlive(IDMFC *instance,int pnum);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_SendControlMessageToPlayer(IDMFC *instance,int pnum,ubyte msg);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_ReceiveControlMessage(IDMFC *instance,ubyte *data);
EXTERN DLLEXPORT bool DLLFUNCCALL IDMFC_FindPInfoStatFirst(IDMFC *instance,int slot,tPInfoStat *stat);
EXTERN DLLEXPORT bool DLLFUNCCALL IDMFC_FindPInfoStatNext(IDMFC *instance,tPInfoStat *stat);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_FindPInfoStatClose(IDMFC *instance);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_DoDamageToPlayer(IDMFC *instance,int pnum,int type, float amount,bool playsound=true);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_StartUIWindow(IDMFC *instance,int id,void *user_data);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_DisconnectMe(IDMFC *instance);
EXTERN DLLEXPORT signed char DLLFUNCCALL IDMFC_AddInputCommand(IDMFC *instance,char *command,char *description,void (*handler)(char *));
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_CheckPInfo(IDMFC *instance);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_EnableStatisticalMessages(IDMFC *instance,bool on);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_EnableOnScreenMenu(IDMFC *instance,bool turn_on);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_EnableAutoSaveLevelEnd(IDMFC *instance,bool enable);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_EnableAutoSaveDisconnect(IDMFC *instance,bool enable);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_GenerateStatFilename(IDMFC *instance,char *filename,char *root,bool end_of_level);
EXTERN DLLEXPORT bool DLLFUNCCALL IDMFC_IsPlayerObserver(IDMFC *instance,int pnum);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_EnableOnScreenMenuBackground(IDMFC *instance,bool enable);
EXTERN DLLEXPORT float DLLFUNCCALL IDMFC_GetTimeInGame(IDMFC *instance,int slot);
EXTERN DLLEXPORT char DLLFUNCCALLPTR IDMFC_GetTimeString(IDMFC *instance,float sec);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_DisplayNetGameInfo(IDMFC *instance,int background_bmp=-1,bool dedicated_server=false);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_SwitchNetGameInfoDisplay(IDMFC *instance,int on);
EXTERN DLLEXPORT bool DLLFUNCCALL IDMFC_IsDisplayingNetGameInfo(IDMFC *instance);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_DatabaseRegister(IDMFC *instance,char *name);
EXTERN DLLEXPORT bool DLLFUNCCALL IDMFC_DatabaseReadA(IDMFC *instance,const char *label,char *entry,int *entrylen);
EXTERN DLLEXPORT bool DLLFUNCCALL IDMFC_DatabaseReadB(IDMFC *instance,const char *label, void *entry, int wordsize);
EXTERN DLLEXPORT bool DLLFUNCCALL IDMFC_DatabaseReadC(IDMFC *instance,const char *label, bool *entry);
EXTERN DLLEXPORT bool DLLFUNCCALL IDMFC_DatabaseWriteA(IDMFC *instance,const char *label, const char *entry, int entrylen);
EXTERN DLLEXPORT bool DLLFUNCCALL IDMFC_DatabaseWriteB(IDMFC *instance,const char *label, int entry);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_SendNetGameInfoSync(IDMFC *instance,int to_who=SP_ALL);
EXTERN DLLEXPORT bool DLLFUNCCALL IDMFC_CompareNetworkAddress(IDMFC *instance,network_address *one,network_address *two,bool use_port=true);
EXTERN DLLEXPORT bool DLLFUNCCALL IDMFC_IsMasterTrackerGame(IDMFC *instance);
EXTERN DLLEXPORT int DLLFUNCCALL IDMFC_ConvertLocalToServerObjnum(IDMFC *instance,int objnum);
EXTERN DLLEXPORT int DLLFUNCCALL IDMFC_ConvertServerToLocalObjnum(IDMFC *instance,int objnum);
EXTERN DLLEXPORT bool DLLFUNCCALL IDMFC_ShouldIDisplayHUDName(IDMFC *instance,int pnum);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_SetDeathMessageFilter(IDMFC *instance,int level);
EXTERN DLLEXPORT bool DLLFUNCCALL IDMFC_IAmDedicatedServer(IDMFC *instance);
EXTERN DLLEXPORT bool DLLFUNCCALL IDMFC_IsPlayerDedicatedServerPnum(IDMFC *instance,int pnum);
EXTERN DLLEXPORT bool DLLFUNCCALL IDMFC_IsPlayerDedicatedServer(IDMFC *instance,player_record *pr);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_DisplayInputCommandHelp(IDMFC *instance,char *s);
EXTERN DLLEXPORT int DLLFUNCCALL IDMFC_GetPlayerTeam(IDMFC *instance,int pnum);
EXTERN DLLEXPORT bool DLLFUNCCALL IDMFC_SetTeamName(IDMFC *instance,int team,char *name,bool announce);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_ConvertHUDCoord(IDMFC *instance,int x,int y,int *rx,int *ry);
EXTERN DLLEXPORT int DLLFUNCCALL IDMFC_GetPlayerLogoBmp(IDMFC *instance,int player_num,bool *is_vclip);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_EnableShipLogos(IDMFC *instance,bool enable);
EXTERN DLLEXPORT bool DLLFUNCCALL IDMFC_AreLogosEnabled(IDMFC *instance);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_EnableAudioTaunts(IDMFC *instance,bool enable);
EXTERN DLLEXPORT bool DLLFUNCCALL IDMFC_AreTauntsEnabled(IDMFC *instance);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_RespawnPlayer(IDMFC *instance,int pnum,bool spew_energy_and_shield,bool spew_everything);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_ReadInHostsAllowDeny(IDMFC *instance);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_FreeHostsLists(IDMFC *instance);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_RehashAllowDeny(IDMFC *instance);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_AnnounceTeamChangeDeny(IDMFC *instance,int pnum);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_SetMaxPlayerHardLimit(IDMFC *instance,int max);
EXTERN DLLEXPORT int DLLFUNCCALL IDMFC_WasPlayerInGameAtLevelEnd(IDMFC *instance,int prec);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_Set_OnServerPlayerKilled(IDMFC *instance,void (*callback)(object *killer_obj,int victim_pnum));
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_Set_OnServerPlayerExploded(IDMFC *instance,void (*callback)(int player_num));
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_Set_OnServerCollideA(IDMFC *instance,void (*callback)(object *me_obj,object *it_obj));
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_Set_OnServerCollideB(IDMFC *instance,void (*callback)(object *me_obj,object *it_obj,vector *point,vector *normal));
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_Set_OnServerPlayerChangeSegment(IDMFC *instance,void (*callback)(int player_num,int newseg,int oldseg));
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_Set_OnServerObjectChangeSegment(IDMFC *instance,void (*callback)(object *obj,int newseg,int oldseg));
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_Set_OnServerPlayerEntersGame(IDMFC *instance,void (*callback)(int player_num));
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_Set_OnServerPlayerDisconnect(IDMFC *instance,void (*callback)(int player_num));
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_Set_OnServerGameCreated(IDMFC *instance,void (*callback)(void));
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_Set_OnServerLevelChange(IDMFC *instance,void (*callback)(void));
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_Set_OnServerLevelStart(IDMFC *instance,void (*callback)(void));
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_Set_OnServerLevelEnd(IDMFC *instance,void (*callback)(void));
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_Set_OnServerObjectShieldsChanged(IDMFC *instance,void (*callback)(object *obj,float amount));
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_Set_OnServerIsAddressBanned(IDMFC *instance,bool (*callback)(network_address *addr,char *tracker_id));
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_Set_OnServerWallCollide(IDMFC *instance,void (*callback)(object *obj,float hitspeed,int hitseg,int hitwall,vector * hitpt,vector *wall_normal,float hit_dot));
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_Set_OnServerObjectKilled(IDMFC *instance,void (*callback)(object *obj,object *killer));
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_Set_OnServerObjectDestroyed(IDMFC *instance,void (*callback)(object *obj));
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_Set_OnClientPlayerKilled(IDMFC *instance,void (*callback)(object *killer_obj,int victim_pnum));
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_Set_OnClientPlayerExploded(IDMFC *instance,void (*callback)(int player_num));
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_Set_OnClientCollideA(IDMFC *instance,void (*callback)(object *me_obj,object *it_obj));
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_Set_OnClientCollideB(IDMFC *instance,void (*callback)(object *me_obj,object *it_obj,vector *point,vector *normal));
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_Set_OnClientPlayerChangeSegment(IDMFC *instance,void (*callback)(int player_num,int newseg,int oldseg));
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_Set_OnClientObjectChangeSegment(IDMFC *instance,void (*callback)(object *obj,int newseg,int oldseg));
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_Set_OnClientPlayerEntersGame(IDMFC *instance,void (*callback)(int player_num));
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_Set_OnClientPlayerDisconnect(IDMFC *instance,void (*callback)(int player_num));
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_Set_OnMeDisconnectFromServer(IDMFC *instance,void (*callback)(void));
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_Set_OnClientGameCreated(IDMFC *instance,void (*callback)(void));
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_Set_OnClientObjectShieldsChanged(IDMFC *instance,void (*callback)(object *obj,float amount));
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_Set_OnWeaponFired(IDMFC *instance,void (*callback)(object *weapon_obj,object *shooter));
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_Set_OnClientLevelChange(IDMFC *instance,void (*callback)(void));
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_Set_OnClientLevelStart(IDMFC *instance,void (*callback)(void));
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_Set_OnClientLevelEnd(IDMFC *instance,void (*callback)(void));
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_Set_OnClientWallCollide(IDMFC *instance,void (*callback)(object *obj,float hitspeed,int hitseg,int hitwall,vector * hitpt,vector *wall_normal,float hit_dot));
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_Set_OnClientObjectKilled(IDMFC *instance,void (*callback)(object *obj,object *killer));
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_Set_OnClientObjectDestroyed(IDMFC *instance,void (*callback)(object *obj));
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_Set_OnPlayerEntersObserver(IDMFC *instance,void (*callback)(int pnum,object *piggy));
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_Set_OnPlayerExitsObserver(IDMFC *instance,void (*callback)(int pnum));
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_Set_OnCanChangeTeam(IDMFC *instance,bool (*callback)(int pnum,int newteam));
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_Set_OnSpecialPacket(IDMFC *instance,void (*callback)(void));
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_Set_OnInterval(IDMFC *instance,void (*callback)(void));
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_Set_OnHUDInterval(IDMFC *instance,void (*callback)(void));
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_Set_OnPLRInterval(IDMFC *instance,void (*callback)(void));
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_Set_OnPLRInit(IDMFC *instance,void (*callback)(void));
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_Set_OnKeypress(IDMFC *instance,void (*callback)(int key));
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_Set_OnInputString(IDMFC *instance,void (*callback)(char *input_string));
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_Set_OnPlayerChangeTeam(IDMFC *instance,void (*callback)(int player_num,int newteam,bool announce,bool spew_everything));
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_Set_OnGameStateRequest(IDMFC *instance,void (*callback)(int pnum));
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_Set_OnSaveStatsToFile(IDMFC *instance,void (*callback)(void));
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_Set_OnPlayerReconnect(IDMFC *instance,void (*callback)(int player_num));
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_Set_OnPlayerConnect(IDMFC *instance,void (*callback)(int player_num));
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_Set_OnControlMessage(IDMFC *instance,void (*callback)(ubyte msg,int from_pnum));
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_Set_OnAllowObserverChange(IDMFC *instance,bool (*callback)(bool turnonobserver));
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_Set_OnClientShowUI(IDMFC *instance,void (*callback)(int id,void *user_data));
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_Set_OnPrintScores(IDMFC *instance,void (*callback)(int level));
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_Set_OnDisconnectSaveStatsToFile(IDMFC *instance,void (*callback)(void));
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_Set_OnLevelEndSaveStatsToFile(IDMFC *instance,void (*callback)(void));
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_Set_OnGetHudCallSignColor(IDMFC *instance,ddgr_color (*callback)(int playernum));
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_Set_OnTeamChangeName(IDMFC *instance,void (*callback)(int team,char *oldname,char *newname));
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_Set_OnDoControls(IDMFC *instance,void (*callback)(game_controls *controls));
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_Set_OnPlayAudioTaunt(IDMFC *instance,void (*callback)(int pnum));
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_CallOnServerPlayerKilled(IDMFC *instance,object *killer_obj,int victim_pnum);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_CallOnServerPlayerExploded(IDMFC *instance,int player_num);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_CallOnServerCollideA(IDMFC *instance,object *me_obj,object *it_obj);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_CallOnServerCollideB(IDMFC *instance,object *me_obj,object *it_obj,vector *point,vector *normal);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_CallOnServerPlayerChangeSegment(IDMFC *instance,int player_num,int newseg,int oldseg);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_CallOnServerObjectChangeSegment(IDMFC *instance,object *obj,int newseg,int oldseg);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_CallOnServerPlayerEntersGame(IDMFC *instance,int player_num);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_CallOnServerPlayerDisconnect(IDMFC *instance,int player_num);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_CallOnServerGameCreated(IDMFC *instance);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_CallOnServerLevelChange(IDMFC *instance);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_CallOnServerLevelStart(IDMFC *instance);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_CallOnServerLevelEnd(IDMFC *instance);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_CallOnServerObjectShieldsChanged(IDMFC *instance,object *obj,float amount);
EXTERN DLLEXPORT bool DLLFUNCCALL IDMFC_CallOnServerIsAddressBanned(IDMFC *instance,network_address *addr,char *tracker_id);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_CallOnServerWallCollide(IDMFC *instance,object *obj,float hitspeed,int hitseg,int hitwall,vector * hitpt,vector *wall_normal,float hit_dot);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_CallOnServerObjectKilled(IDMFC *instance,object *obj,object *killer);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_CallOnServerObjectDestroyed(IDMFC *instance,object *obj);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_CallOnClientPlayerKilled(IDMFC *instance,object *killer_obj,int victim_pnum);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_CallOnClientPlayerExploded(IDMFC *instance,int player_num);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_CallOnClientCollideA(IDMFC *instance,object *me_obj,object *it_obj);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_CallOnClientCollideB(IDMFC *instance,object *me_obj,object *it_obj,vector *point,vector *normal);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_CallOnClientPlayerChangeSegment(IDMFC *instance,int player_num,int newseg,int oldseg);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_CallOnClientObjectChangeSegment(IDMFC *instance,object *obj,int newseg,int oldseg);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_CallOnClientPlayerEntersGame(IDMFC *instance,int player_num);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_CallOnClientPlayerDisconnect(IDMFC *instance,int player_num);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_CallOnMeDisconnectFromServer(IDMFC *instance);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_CallOnClientGameCreated(IDMFC *instance);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_CallOnClientObjectShieldsChanged(IDMFC *instance,object *obj,float amount);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_CallOnWeaponFired(IDMFC *instance,object *weapon_obj,object *shooter);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_CallOnClientLevelChange(IDMFC *instance);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_CallOnClientLevelStart(IDMFC *instance);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_CallOnClientLevelEnd(IDMFC *instance);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_CallOnClientWallCollide(IDMFC *instance,object *obj,float hitspeed,int hitseg,int hitwall,vector * hitpt,vector *wall_normal,float hit_dot);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_CallOnClientObjectKilled(IDMFC *instance,object *obj,object *killer);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_CallOnClientObjectDestroyed(IDMFC *instance,object *obj);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_CallOnPlayerEntersObserver(IDMFC *instance,int pnum,object *piggy);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_CallOnPlayerExitsObserver(IDMFC *instance,int pnum);
EXTERN DLLEXPORT bool DLLFUNCCALL IDMFC_CallOnCanChangeTeam(IDMFC *instance,int pnum,int newteam);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_CallOnSpecialPacket(IDMFC *instance);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_CallOnInterval(IDMFC *instance);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_CallOnHUDInterval(IDMFC *instance);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_CallOnPLRInterval(IDMFC *instance);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_CallOnPLRInit(IDMFC *instance);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_CallOnKeypress(IDMFC *instance,int key);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_CallOnInputString(IDMFC *instance,char *input_string);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_CallOnPlayerChangeTeam(IDMFC *instance,int player_num,int newteam,bool announce,bool spew_everything);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_CallOnGameStateRequest(IDMFC *instance,int pnum);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_CallOnSaveStatsToFile(IDMFC *instance);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_CallOnPlayerReconnect(IDMFC *instance,int player_num);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_CallOnPlayerConnect(IDMFC *instance,int player_num);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_CallOnControlMessage(IDMFC *instance,ubyte msg,int from_pnum);
EXTERN DLLEXPORT bool DLLFUNCCALL IDMFC_CallOnAllowObserverChange(IDMFC *instance,bool turnonobserver);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_CallOnClientShowUI(IDMFC *instance,int id,void *user_data);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_CallOnPrintScores(IDMFC *instance,int level);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_CallOnDisconnectSaveStatsToFile(IDMFC *instance);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_CallOnLevelEndSaveStatsToFile(IDMFC *instance);
EXTERN DLLEXPORT ddgr_color DLLFUNCCALL IDMFC_CallOnGetHudCallSignColor(IDMFC *instance,int playernum);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_CallOnTeamChangeName(IDMFC *instance,int team,char *oldname,char *newname);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_CallOnDoControls(IDMFC *instance,game_controls *controls);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_CallOnPlayAudioTaunt(IDMFC *instance,int pnum);
EXTERN DLLEXPORT dllinfo DLLFUNCCALLPTR IDMFC_GetDLLInfoCallData(IDMFC *instance);
EXTERN DLLEXPORT int DLLFUNCCALL IDMFC_GetHighestRoomIndex(IDMFC *instance);
EXTERN DLLEXPORT int DLLFUNCCALL IDMFC_GetGameWindowW(IDMFC *instance);
EXTERN DLLEXPORT int DLLFUNCCALL IDMFC_GetGameWindowH(IDMFC *instance);
EXTERN DLLEXPORT int DLLFUNCCALL IDMFC_GetGameWindowX(IDMFC *instance);
EXTERN DLLEXPORT int DLLFUNCCALL IDMFC_GetGameWindowY(IDMFC *instance);
EXTERN DLLEXPORT int DLLFUNCCALLPTR IDMFC_GetGameFontTranslateArray(IDMFC *instance);
EXTERN DLLEXPORT int DLLFUNCCALL IDMFC_GetObserverModeBitmap(IDMFC *instance);
EXTERN DLLEXPORT float DLLFUNCCALL IDMFC_GetFrametime(IDMFC *instance);
EXTERN DLLEXPORT float DLLFUNCCALL IDMFC_GetGametime(IDMFC *instance);
EXTERN DLLEXPORT float DLLFUNCCALL IDMFC_GetRealGametime(IDMFC *instance,bool *ispaused=NULL);
EXTERN DLLEXPORT float DLLFUNCCALLPTR IDMFC_GetShieldDeltaArray(IDMFC *instance);
EXTERN DLLEXPORT float DLLFUNCCALL IDMFC_GetHudAspectX(IDMFC *instance);
EXTERN DLLEXPORT float DLLFUNCCALL IDMFC_GetHudAspectY(IDMFC *instance);
EXTERN DLLEXPORT char DLLFUNCCALLPTR IDMFC_GetLocalD3Dir(IDMFC *instance);
EXTERN DLLEXPORT tMission DLLFUNCCALLPTR IDMFC_GetCurrentMission(IDMFC *instance);	
EXTERN DLLEXPORT room DLLFUNCCALLPTR IDMFC_GetRooms(IDMFC *instance);
EXTERN DLLEXPORT object DLLFUNCCALLPTR IDMFC_GetObjects(IDMFC *instance);
EXTERN DLLEXPORT terrain_segment DLLFUNCCALLPTR IDMFC_GetTerrainSegs(IDMFC *instance);
EXTERN DLLEXPORT netgame_info DLLFUNCCALLPTR IDMFC_GetNetgameInfo(IDMFC *instance);
EXTERN DLLEXPORT player DLLFUNCCALLPTR IDMFC_GetPlayers(IDMFC *instance);
EXTERN DLLEXPORT netplayer DLLFUNCCALLPTR IDMFC_GetNetPlayers(IDMFC *instance);
EXTERN DLLEXPORT ship DLLFUNCCALLPTR IDMFC_GetShips(IDMFC *instance);
EXTERN DLLEXPORT weapon DLLFUNCCALLPTR IDMFC_GetWeapons(IDMFC *instance);
EXTERN DLLEXPORT texture DLLFUNCCALLPTR IDMFC_GetGameTextures(IDMFC *instance);
EXTERN DLLEXPORT poly_model DLLFUNCCALLPTR IDMFC_GetGamePolyModels(IDMFC *instance);
EXTERN DLLEXPORT vclip DLLFUNCCALLPTR IDMFC_GetGameVClips(IDMFC *instance);
EXTERN DLLEXPORT ddgr_color DLLFUNCCALLPTR IDMFC_GetPlayerColors(IDMFC *instance);
EXTERN DLLEXPORT game_controls DLLFUNCCALL IDMFC_GetLastGameControls(IDMFC *instance);
EXTERN DLLEXPORT int DLLFUNCCALLPTR IDMFC_GetPilotPicBitmapHandles(IDMFC *instance);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_GetViewerObjectPtr(IDMFC *instance,object **v_obj);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_SetViewerObjectPtr(IDMFC *instance,object *v_obj);
EXTERN DLLEXPORT float DLLFUNCCALL IDMFC_GetRenderZoom(IDMFC *instance);
EXTERN DLLEXPORT IMenuItem DLLFUNCCALLPTR IDMFC_GetOnScreenMenu(IDMFC *instance);
EXTERN DLLEXPORT tOSIRISModuleInit DLLFUNCCALLPTR IDMFC_GetOsirisModuleData(IDMFC *instance);
EXTERN DLLEXPORT vis_effect DLLFUNCCALLPTR IDMFC_GetVisEffectArray(IDMFC *instance,int **Highviseptr=NULL);
EXTERN DLLEXPORT level_info DLLFUNCCALLPTR IDMFC_GetLevelInfo(IDMFC *instance);
EXTERN DLLEXPORT void DLLFUNCCALL IDMFC_SelectNextCameraView(IDMFC *instance,int window);
EXTERN DLLEXPORT int DLLFUNCCALL IDMFC_GetCameraViewType(IDMFC *instance,int window);
EXTERN DLLEXPORT object_info DLLFUNCCALLPTR IDMFC_GetObjectInfo(IDMFC *instance,int objinfo_id);

/********************************
IMenuItem 1.0 Interface functions
*********************************/
EXTERN DLLEXPORT void DLLFUNCCALL IMenuItem_Delete(IMenuItem *instance);
EXTERN DLLEXPORT void DLLFUNCCALLPTR IMenuItem_Dynamic_Cast(IMenuItem *instance,const char *pszType);
EXTERN DLLEXPORT void DLLFUNCCALL IMenuItem_DuplicatePointer(IMenuItem *instance);
EXTERN DLLEXPORT void DLLFUNCCALL IMenuItem_DestroyPointer(IMenuItem *instance);
EXTERN DLLEXPORT bool DLLFUNCCALL IMenuItem_AddSubMenu(IMenuItem *instance,IMenuItem *p);
EXTERN DLLEXPORT bool DLLFUNCCALL IMenuItem_DetachSubMenu(IMenuItem *instance,IMenuItem *p);
EXTERN DLLEXPORT bool DLLFUNCCALL IMenuItem_Forward(IMenuItem *instance);
EXTERN DLLEXPORT bool DLLFUNCCALL IMenuItem_Back(IMenuItem *instance);
EXTERN DLLEXPORT bool DLLFUNCCALL IMenuItem_Up(IMenuItem *instance);
EXTERN DLLEXPORT bool DLLFUNCCALL IMenuItem_Down(IMenuItem *instance);
EXTERN DLLEXPORT void DLLFUNCCALL IMenuItem_Execute(IMenuItem *instance);
EXTERN DLLEXPORT void DLLFUNCCALL IMenuItem_Draw(IMenuItem *instance,int x,int y,int height,int backgroundbmp,float *ratio=NULL);
EXTERN DLLEXPORT void DLLFUNCCALL IMenuItem_SetInputFocus(IMenuItem *instance);
EXTERN DLLEXPORT void DLLFUNCCALL IMenuItem_LoseInputFocus(IMenuItem *instance);
EXTERN DLLEXPORT bool DLLFUNCCALL IMenuItem_GetFocus(IMenuItem *instance);
EXTERN DLLEXPORT char DLLFUNCCALLPTR IMenuItem_GetTitle(IMenuItem *instance);
EXTERN DLLEXPORT int DLLFUNCCALL IMenuItem_GetCustomSubMenuCount(IMenuItem *instance);
EXTERN DLLEXPORT void DLLFUNCCALL IMenuItem_CallFunc(IMenuItem *instance,int value);
EXTERN DLLEXPORT bool DLLFUNCCALL IMenuItem_HasSubMenus(IMenuItem *instance);
EXTERN DLLEXPORT char DLLFUNCCALL IMenuItem_GetType(IMenuItem *instance);
EXTERN DLLEXPORT void DLLFUNCCALL IMenuItem_SetState(IMenuItem *instance,int state);
EXTERN DLLEXPORT bool DLLFUNCCALL IMenuItem_SetStateItemList(IMenuItem *instance,int count, char **array );

/********************************
IDmfcStats 1.0 Interface functions
*********************************/
EXTERN DLLEXPORT void DLLFUNCCALL IDmfcStats_Delete(IDmfcStats *instance);
EXTERN DLLEXPORT void DLLFUNCCALLPTR IDmfcStats_Dynamic_Cast(IDmfcStats *instance,const char *pszType);
EXTERN DLLEXPORT void DLLFUNCCALL IDmfcStats_DuplicatePointer(IDmfcStats *instance);
EXTERN DLLEXPORT void DLLFUNCCALL IDmfcStats_DestroyPointer(IDmfcStats *instance);
EXTERN DLLEXPORT bool DLLFUNCCALL IDmfcStats_Initialize(IDmfcStats *instance,tDmfcStatsInit *init_info);
EXTERN DLLEXPORT void DLLFUNCCALL IDmfcStats_Enable(IDmfcStats *instance,bool enable);
EXTERN DLLEXPORT bool DLLFUNCCALL IDmfcStats_IsEnabled(IDmfcStats *instance);
EXTERN DLLEXPORT void DLLFUNCCALL IDmfcStats_DoFrame(IDmfcStats *instance);
EXTERN DLLEXPORT void DLLFUNCCALL IDmfcStats_ScrollDown(IDmfcStats *instance);
EXTERN DLLEXPORT void DLLFUNCCALL IDmfcStats_ScrollUp(IDmfcStats *instance);
EXTERN DLLEXPORT bool DLLFUNCCALL IDmfcStats_CanScrollDown(IDmfcStats *instance);
EXTERN DLLEXPORT bool DLLFUNCCALL IDmfcStats_CanScrollUp(IDmfcStats *instance);
#ifdef __cplusplus
}
#endif

#endif