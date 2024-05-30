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

 * $Logfile: /DescentIII/Main/dmfc/dmfcinternal.h $
 * $Revision: 1.1.1.1 $
 * $Date: 2003/08/26 03:57:21 $
 * $Author: kevinb $
 *
 * Internal DMFC header
 *
 * $Log: dmfcinternal.h,v $
 * Revision 1.1.1.1  2003/08/26 03:57:21  kevinb
 * initial 1.5 import
 *
 *
 * 76    9/05/01 6:04p Matt
 * Added code to save the user's preferred HUD name level setting even if
 * the server bashes it down.
 *
 * 75    9/28/99 10:55a Jeff
 * changed size of pointer for mastertracker game from uint8_t* to int*
 *
 * 74    8/21/99 12:32a Jeff
 * Changed the name of the GetRealGametime function to
 * GetRealGametimePacket since it was confusing some compilers with the
 * other GetRealGametime function.
 *
 * 73    8/15/99 4:36p Jeff
 * finished exporting all inventory class functions.  export object_info
 * array. added check for -nooutragelogo to display Outrage logo display.
 *
 * 72    8/11/99 1:47p Jeff
 * interface functions
 *
 * 71    7/16/99 2:43p Jeff
 * changed CreateMenuItemWArgs function
 *
 * 70    7/14/99 11:47a Jeff
 * localized text for patch fixes
 *
 * 69    7/13/99 10:05a Jeff
 * text taunt token decoding
 *
 * 68    7/11/99 3:31p Jeff
 * exported game arguments, made command line option to specify
 * autoexec.dmfc
 *
 * 67    7/09/99 7:02p Jeff
 * put in countdown timer for when a level is about to end
 *
 * 66    7/09/99 6:18p Jeff
 * added $remoteadminlogout and $wait commands
 *
 * 65    7/09/99 2:54p Jeff
 * handle gametime better (pause it when needed) if the server is 'waiting
 * for players'
 *
 * 64    7/08/99 9:55p Jeff
 * added event handler for weapon fired event
 *
 * 63    7/08/99 6:25p Jeff
 * remote admin in and working
 *
 * 62    7/08/99 2:39a Jeff
 * rough implementation of remote administration checked in.  Still needs
 * some polishing, but should work basically.
 *
 * 61    7/07/99 6:10p Jeff
 * don't count the time that a player is in observer mode for death/kill
 * messages
 *
 * 60    7/07/99 5:00p Jeff
 * removed vararg functions from interface functions, just made different
 * versions of them
 *
 * 59    7/07/99 12:16p Jeff
 * all mangles symbol names fixed.  Finished making interface functions.
 *
 * 58    6/10/99 5:42p Jeff
 * fixed mprintf define
 *
 * 57    6/10/99 11:10a Jeff
 * don't display the Outrage logo for non-Outrage games
 *
 * 56    5/22/99 1:12a Jeff
 * correctly handle Viewer_object
 *
 * 55    5/19/99 11:51p Jeff
 * set version number for 1.0 for release
 *
 * 54    5/13/99 11:46a Jeff
 * increased version number
 *
 * 53    5/12/99 11:04p Jeff
 * dmfc and multiplayer games now have endian friendly packets (*whew*)
 *
 * 52    5/10/99 2:44a Jeff
 * handle new scheme of player's joining in a team game, where the team is
 * set before player enters game in the main code, but the team is
 * determined via event call to dmfc
 *
 * 51    5/09/99 6:20a Jeff
 * improved Entropy (added sounds, max virii per room).  Fixed rendering
 * bugs for other multiplayer dlls.
 *
 * 50    5/08/99 11:06a Jeff
 *
 * 49    5/07/99 12:52p Jeff
 * audio taunt icon is ppic if available.  coop has hard max team set of 4
 *
 * 48    5/04/99 8:46p Jeff
 * display icon when someone plays an audio taunt
 *
 * 47    4/30/99 10:53p Jeff
 * added $warp command
 *
 * 46    4/30/99 7:36p Jeff
 * exported vis_effects to dmfc
 *
 * 45    4/25/99 7:19p Jeff
 * added code to handle suicides in pinfo
 *
 * 44    4/23/99 6:15p Jeff
 * fixed double calls to GameClose
 *
 * 43    4/19/99 3:54a Jeff
 * got compiling for Linux
 *
 * 42    4/14/99 1:15a Jeff
 * fixed case mismatched #includes
 *
 * 41    4/12/99 11:36a Jeff
 * added permissable cs string
 *
 * 40    4/04/99 4:56p Jeff
 * added functionality to call osiris functions from multiplayer d3ms
 *
 * 39    4/03/99 4:06p Jeff
 * added loss/ping gauge
 *
 * 38    3/30/99 9:01p Jeff
 * exported polymodels
 *
 * 37    3/30/99 7:42p Jeff
 * fixed a misspelling on a function name
 *
 * 36    3/27/99 4:53p Jeff
 * player rankings in multiplayer games implemented.  Fixed join message
 * so it doesn't get cut off
 *
 * 35    3/22/99 6:20p Jeff
 * added 2 more audio taunts.  a mulitplayer event when someone plays an
 * audio taunt.  option to disable audio taunts.
 *
 * 34    3/22/99 1:55p Jeff
 * make sure initialization happens (possible crashing)
 *
 * 33    3/17/99 12:24p Jeff
 * converted DMFC to be COM interface
 *
 * 32    1/19/99 3:55a Jeff
 * all strings localized out
 *
 * 31    1/18/99 7:27p Jeff
 * localized strings in dmfcbase.cpp
 *
 * 30    1/10/99 7:41p Jeff
 * added initial version of waitforplayers dialog
 *
 * 29    1/04/99 12:21p Jeff
 * added support for hosts.allow/deny and updates stats manager a little
 *
 * 28    12/08/98 12:17p Jeff
 * various changes that include an improved Team Control dialog (doesn't
 * switch teams until exit..) and spew/respawn players that change teams
 *
 * 27    11/19/98 5:56p Jeff
 * added slider exported and improved Hoard
 *
 * 26    11/17/98 6:29p Jeff
 * mod can specify whether or not to display the team setup dialog on team
 * game start.  Added a menu item to display team setup dialog in mid-game
 *
 * 25    11/16/98 5:35p Jeff
 * removed log functions, added support for changing team names, fixed ctf
 * to work better with different team names
 *
 * 24    11/11/98 7:19p Jeff
 * changed version number
 *
 * 23    11/11/98 7:19p Jeff
 * changes made so that a dedicated server's team is always -1 (team game
 * or not)
 *
 * 22    10/29/98 7:01p Jeff
 * creation of team placement dialog.  Moved TranslateEvent into DMFC
 *
 * 21    10/20/98 4:35p Jeff
 * added a flag for menu to add a <None> to MIT_PLIST...
 *
 * 20    10/20/98 12:16p Jeff
 * added death message filter, hud callsign filter
 *
 * 19    10/18/98 7:59p Jeff
 * functions added to dmfc for client->server objnum matching.  Banning
 * now uses tracker id when available.
 *
 * 18    10/15/98 1:34p Jeff
 * added scrollable onscreen menu.  Remove ban in dmfc.  prejoin event
 *
 * 17    10/08/98 3:37p Jeff
 * general improvements (Netgame info things, save to registry).  Changes
 * so it would send packets on NETSEQ_OBJECTS
 *
 * 16    10/05/98 2:50p Jeff
 *
 * 15    9/30/98 3:50p Jeff
 * general improvements (many)
 *
 * 14    9/29/98 3:04p Jeff
 * added time in game and start_time support
 *
 * 13    9/28/98 5:05p Jeff
 * made the statisitical death messages an option in the menu
 *
 * 12    9/24/98 5:52p Jeff
 * starting adding statistical death messages, checked in for testing
 *
 * 11    9/21/98 7:11p Jeff
 * made InputCommand interface API and moved existing input commands to
 * the interface.  Changed mprintf/ASSERT so they are valid in DMFC
 *
 * $NoKeywords: $
 */

#ifndef __DMFC_INTERNAL_H
#define __DMFC_INTERNAL_H

#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include "idmfc.h"
#include "DMFC.h"
#include "gamedll_header.h"
#include "DMFCKeyCodes.h"

#define DMFC_VERSION_MAJOR 1
#define DMFC_VERSION_MINOR 0

// DEMO 1.0
// #define DMFC_VERSION_MAJOR	0
// #define DMFC_VERSION_MINOR	60

class DMFCBase;

#ifdef mprintf // undefine mprintf and redirect it to use DLLmprintf
#undef mprintf
#endif
#define mprintf(...) DLLmprintf(__VA_ARGS__)

#define MAX_DBNAME_SIZE 32
#define MAX_DBLABEL_SIZE 128

// DMFC Control Messages
#define CM_KICKED 0xFF           // tell the player he is kicked
#define CM_BANNED 0xFE           // tell the player he is banned
#define CM_TEAMCHANGEDENIED 0xFD // tell the player change team request denied
#define CM_SIGNALSTART 0xFC      // tell the player he can start moving
#define CM_SIGNALWAIT 0xFB       // tell the player to stop moving
#define CM_PAUSETIME 0xFA        // tell the player to pause gametime
#define CM_UNPAUSETIME 0xF9      // tell the player to unpause gametime

struct tDeathMsg {
  char *message;     // pointer to the actual message
  bool inuse;        // whether the node is being used
  bool victim_first; // whether the victim is listed first or second in the message
};

struct tSPHandler {
  void (*func)(uint8_t *); // Function handler to handle the packet
  void (DMFCBase::*DMFCfunc)(uint8_t *);
  tSPHandler *next; // Pointer to next handler
  int type;
  uint8_t ID; // ID of special packet
};

// Struct for Input Command nodes (commands that begin with $)
struct tInputCommandNode {
  char *data;
  char *desc;
  tInputCommandNode *next;
  void (*handler)(const char *);
  bool allow_remote;
};

struct tPKillerInfo {
  uint8_t slot;
  int kills;
  tPKillerInfo *next;
};

struct tPExtraInfo {
  float last_kill_time;   // Game time of the last kill for this player (previous kill)
  float last_death_time;  // Game time of the last time this player was killed (previous death)
  float kill_time;        // time of latest kill
  float death_time;       // time of latest death
  float observer_time;    // time the player entered observer mode (or 0 if not)
  uint16_t kills_in_a_row;  // Number of kills in a row for this player
  uint16_t deaths_in_a_row; // Number of deaths in a row for this player
  int8_t last_kill_num;    // Player record num of last person this player killed
  int8_t last_death_num;   // Player record num of the player that last killed this player
  char got_revenge;       // 1 if revenge has been made on last killer
};

class PInfo {
public:
  PInfo(int slot);
  ~PInfo();
  tPKillerInfo *GetKillerInfo(int slot);
  void Update(int slot, tPKillerInfo *amount);
  tPKillerInfo *GetFirstKiller(void);
  tPKillerInfo *GetNextKiller(void);
  void RemoveKiller(int slot);
  void ResetAll(void);
  int GetSizeOfData(void);
  int GetExtraInfoSize(void);
  void PackData(uint8_t *buffer);
  bool UnpackData(uint8_t *buffer, int buffsize);
  tPExtraInfo *GetExtraInfo(void);
  void HandleKill(int killer_num);
  void HandleDeath(int victim_num);
  void HandleSuicide(void);
  void EnterObserverMode(void);
  void ExitObserverMode(void);

private:
  tPKillerInfo *FindKiller(int slot, tPKillerInfo **prev = NULL);
  tPKillerInfo *CreateKiller(int slot);
  tPKillerInfo *killer_list;
  tPKillerInfo *curr;
  tPExtraInfo extra_info;
  int m_slot;
};

class MenuItem : public IMenuItem {
public:
  MenuItem();
  MenuItem(const char *title, char type, uint8_t flags, void (*fp)(int), ...); // the last parameters are the state items, which
                                                                       // is an int (the number of states), followed by
                                                                       // an int (the initial state), followed by a
                                                                       // a list of char * to the names of the items
                                                                       // this is required for MIT_STATE type only
  ~MenuItem();

  bool AddSubMenu(IMenuItem *p);
  bool DetachSubMenu(IMenuItem *p);

  bool Forward(void);
  bool Back(void);
  bool Up(void);
  bool Down(void);

  void Execute(void);
  void Draw(int x, int y, int height, int backgroundbmp, float *ratio = NULL);

  void SetInputFocus(void);
  void LoseInputFocus(void);
  bool GetFocus(void);

  const char *GetTitle(void);
  int GetCustomSubMenuCount(void);

  void CallFunc(int value);
  bool HasSubMenus(void);
  char GetType(void);

  void SetState(int state);
  bool SetStateItemList(int count, ...);
  bool SetStateItemListArray(int count, char **array);

public:
  void Delete(void);
  void *Dynamic_Cast(const char *pszType);
  void DuplicatePointer(void);
  void DestroyPointer(void);

private:
  int m_cPtrs;
  // How many submenus this has
  int SubMenuCount;
  // Points to the currently active submenu (-1 if none)
  int CurrSubMenu;
  // State of the menu (only for MIT_STATE)
  int m_iState;
  // List of player nums according to the submenu index
  int Pnums[DLLMAX_PLAYERS];
  // The array of Submenus
  IMenuItem **SubMenus;
  // The title for this Menu
  char m_sTitle[MAX_STRING_LEN];
  // Handler for execute
  void (*func)(int);
  // true is this menu currently has input focus
  bool HasInputFocus;
  // What type of submenus this has
  // only if this is set to MIT_NORMAL can it have submenus of its submenus
  // no submenus are allowed to be added to a MIT_PLIST, as this is automatically handled
  // no submenus are allowed for MIT_STATEITEM as they are specific ends for a MIT_STATE submenu
  char m_cType;
  uint8_t m_iFlags;
  uint8_t m_Alpha;
  bool m_AlphaDir;
  int m_iTopIndex;
  bool m_bMoreToScroll, m_bAtBottom;
  tCustomMenu m_cmInfo;
};

struct tDMFCTimerInfo {
  bool inuse;
  void (*handler)(void);
  void (*onkill)(void);
  float starttime;
  float longevity;
  float nexttime;
  float intval;
};

struct tWeaponKillMsg {
  bool inuse;
  bool victim_first;
  char *message;
};

struct tBanItem {
  network_address addr;
  char callsign[MAX_CALLSIGN_SIZE];
  char tracker_id[24];
  tBanItem *next;
};

// This class handles displaying and processing a customizable and detailed stats screen (in-game)
class CDmfcStats : public IDmfcStats {
public:
  // constuctor/destructor
  CDmfcStats();
  ~CDmfcStats();

  // initialization member function, must be called first
  bool Initialize(tDmfcStatsInit *init_info);

  // enables/disables the displaying of the stats screen
  void Enable(bool enable);
  // returns the state of the stats manager
  bool IsEnabled();

  // the do-it-all function, call once per frame
  void DoFrame();
  // scrolls the player list down one
  void ScrollDown();
  // scrolls the player list up one
  void ScrollUp();
  // returns true if the player list can scroll down any more
  bool CanScrollDown();
  // returns true if the player list can scroll up any more
  bool CanScrollUp();

public:
  void Delete(void);
  void *Dynamic_Cast(const char *pszType);
  void DuplicatePointer(void);
  void DestroyPointer(void);

protected:
  int m_cPtrs;

  // true if initialized
  bool m_init;
  int m_flags;
  // array of sorted prec numbers
  int *m_SortedPRecs;
  // pointer to the max number displayed (if the flag is set)
  int *m_pMaxNumberDisplayed;
  // number of players to be displayed for this frame
  int m_MaxCount;
  // number of columns for the player lists
  int m_iPLColCount;
  // number of columns for the detailed list
  int m_iDColCount;
  // information for the player list columns
  tDmfcStatsColumnInfo *m_PLColInfo;
  // information for the detailed columns
  tDmfcStatsColumnInfo *m_DColInfo;
  // callback for DSCOL_CUSTOM column types for Player list
  void (*m_PLColCallback)(int precord_num, int column_num, char *buffer, int buffer_size);
  // callback for DSCOL_CUSTOM column types for detailed
  void (*m_DColCallback)(int precord_num, int column_num, char *buffer, int buffer_size);
  // callback for DSCOL_BMP column types for Player list (bitmaps)
  void (*m_PLColCallbackBMP)(int precord_num, int column_num, int x, int y, int w, int h, uint8_t alpha_to_use);
  // callback for DSCOL_BMP column types for detailed (bitmaps)
  void (*m_DColCallbackBMP)(int precord_num, int column_num, int x, int y, int w, int h, uint8_t alpha_to_use);
  // callback for team data
  void (*m_TeamCallback)(int team, char *buffer, int buffer_size);
  // name of the multiplayer game
  char *m_GameName;

  // starting scroll index
  int m_top_index;
  // selected index
  int m_selected_index;
  // bottom index value
  int m_bottom_index;
  // true if the stats is enabled
  bool m_enabled;
  // height of the font
  int m_font_height;
  // starting y value of player list data
  int m_y_start_plist;
  // starting y value of detailed list data
  int m_y_start_detail;
  // starting x value of player list data
  int m_x_start_plist;
  // starting x value of detailed list data
  int m_x_start_detail;
  // selected prec num
  int m_selected_prec;
  // translatation index->prec
  int m_translate[MAX_PLAYER_RECORDS];

  // bmp handle to highlight bar
  int m_highlight_bmp;
  // bmp handle to window background
  int m_background_bmp;
  // what page of who-killed-who stats are we on
  int m_page_index;
  // how many items can we fit on the who-killed-who stats
  int m_can_fit_count;
  // how long has the current page been displayed
  float m_page_display_time;
  // does this guy have multiple pages?
  bool m_has_multiple_pages;
  // alpha of the items
  uint8_t m_alpha;
  float alpha_in_time;

  // validates all the variables
  void ValidateInfo();

  // renders the stats screen
  void Render();

  // displays in a window the view of the viewer object
  void DisplayPlayerView(int x, int y, int w, int h, object *viewer);
};

struct tHostsNode {
  uint32_t ip;
  uint32_t mask;
  tHostsNode *next;
}; // Allow/Deny IP address nodes

// DMFCBase class
//     This is the base class that the DMFCApp class is based off of.  This just handles general
//     functions, and handling default events.
extern DMFCBase *basethis;

class DMFCBase : public IDMFC {
public:
  DMFCBase(void);
  ~DMFCBase(void);

  // ##################Setup and processing functions#######################

  // DMGFBase::LoadFunctions
  //
  //	Initializes all the functions for a multiplayer game...must be first thing called
  void LoadFunctions(int *api_func);

  // DMFCBase::GameInit
  //
  //   Sets up all the DLL functions and pointers and preps the class for use.  This ABSOLUTLY must be
  //   called, so if you override DMFCApp::GameInit, make sure that you put a call to this somewhere in
  //   the override.
  void GameInit(int teams);

  // DMFCBase::GameClose
  //
  //   Closes up any needed DLL and DMFC tasks.  This ABSOLUTLY must be called, so if you override
  //   DMFCApp::GameClose, make sure that you put a call to this somewhere in the override
  void GameClose(void);

  // ###################Server Only functions################################

  // DMFCBase::OnServerPlayerKilled
  //
  //  Event handler for when a player gets killed, either by another player or some other way
  //  killer_obj = object pointer to the object that killed the player
  //  victim_pnum = player number of the player that got killed
  virtual void OnServerPlayerKilled(object *killer_obj, int victim_pnum);

  // DMFCBase::OnServerPlayerExploded
  //
  //  Event handler for when a player explodes.  Gets called after a EVT_GAMEPLAYERKILLED event.
  //  player_num = player number of the player exploding
  virtual void OnServerPlayerExploded(int player_num);

  // DMFCBase::OnServerCollide
  //
  //   Event handler for when two objects collide.  At least one of the two objects is a player or a robot.
  //   Be aware that this event will be called twice per collision, the second time it is called the me and
  //   it objects will be flipped.
  //   me_obj = object pointer to the me object
  //   it_obj = object pointer to the it object
  virtual void OnServerCollide(object *me_obj, object *it_obj);
  virtual void OnServerCollide(object *me_obj, object *it_obj, vector *point, vector *normal);

  // DMFCBase::OnServerPlayerChangeSegment
  //
  //   Event handler for when a player changes rooms or a "large" terrain cell
  //   (8x8 Terrain block).
  //   player_num = player number of the player who just changed the room/segment
  //   newseg = New room/cell location
  //	 oldseg = Old room/cell location
  virtual void OnServerPlayerChangeSegment(int player_num, int newseg, int oldseg);

  // DMFCBase::OnServerObjectChangeSegment (Only called if server tells client to execute)
  //
  //   Event handler for when a player changes rooms or a "large" terrain cell
  //   (8x8 Terrain block).
  //   obj = Object pointer of the object who just changed the room/segment
  //   newseg = New room/cell location
  //	 oldseg = Old room/cell location
  virtual void OnServerObjectChangeSegment(object *obj, int newseg, int oldseg);

  // DMFCBase::OnServerPlayerEntersGame
  //
  //   Event handler for when a player enters the game.  This will only get called once per
  //   player, it usually gets called right after they connect to the server to start playing.
  //   player_num = player number of the player entering the game
  virtual void OnServerPlayerEntersGame(int player_num);

  // DMFCBase::OnServerPlayerDisconnect
  //
  //   Event handler for when a player disconnects from the server.
  //   player_num = player number of the player that just disconnected
  virtual void OnServerPlayerDisconnect(int player_num);

  // DMFCBase::OnServerGameCreated
  //
  //   Event handler when the server's game first gets started.  This will only be called once
  //   while the server is running, and that is when the server first starts up.
  virtual void OnServerGameCreated(void);

  // DMFCBase::OnServerLevelChange
  //
  //   Event handler when the server changes levels.  This will get called after a level ends and a server
  //   is starting a new level.
  virtual void OnServerLevelChange(void);

  // DMFCBase::OnServerLevelStart
  //
  //   Event handler for when a multiplayer level is starting up.  This will get called right before the level
  //   starts.
  virtual void OnServerLevelStart(void);

  // DMFCBase::OnServerLevelEnd
  //
  //   Event handler for when a multiplayer level is ending.
  virtual void OnServerLevelEnd(void);

  // DMFCBase::OnServerObjectShieldsChanged
  //
  //	Event handler for when an objects shield's change
  virtual void OnServerObjectShieldsChanged(object *obj, float amount);

  // DMFCBase::OnServerIsAddressBanned
  //
  //	Called by the game to determine if the given network address is banned from the game
  virtual bool OnServerIsAddressBanned(network_address *addr, const char *tracker_id);

  // DMFCBase::OnServerWallCollide
  //
  //	Called by the game when their is a collision between an object and a wall
  virtual void OnServerWallCollide(object *obj, float hitspeed, int hitseg, int hitwall, vector *hitpt,
                                   vector *wall_normal, float hit_dot);

  //	DMFCBase::OnServerObjectKilled
  //
  //	Called when an object is being killed
  //	Not automatically sent to clients (OnClientObjectKilled)
  //	killer might not be valid (NULL)
  virtual void OnServerObjectKilled(object *obj, object *killer);

  //	DMFCBase::OnServerObjectDestroyed
  //
  //	Called when an object is about to be deleted
  //	Not automatically sent to clients
  virtual void OnServerObjectDestroyed(object *obj);

  // ######################### Client allowed event handlers############################

  // DMFCBase::OnClientPlayerKilled (Only called if server tells client to execute)
  //
  //  Event handler for when a player gets killed, either by another player or some other way.
  //  killer_obj = object pointer to the object that killed the player
  //  victim_pnum = player number of the player that got killed
  virtual void OnClientPlayerKilled(object *killer_obj, int victim_pnum);

  // DMFCBase::OnClientPlayerExploded (Only called if server tells client to execute)
  //
  //  Event handler for when a player explodes.  Gets called after a EVT_GAMEPLAYERKILLED event.
  //  player_num = player number of the player exploding
  virtual void OnClientPlayerExploded(int player_num);

  // DMFCBase::OnClientCollide (Only called if server tells client to execute)
  //
  //   Event handler for when two objects collide.  At least one of the two objects is a player or a robot.
  //   Be aware that this event will be called twice per collision, the second time it is called the me and
  //   it objects will be flipped.
  //	 ALSO NOTE: In order for the OnClientCollide() that passes the point and normal to have valid values
  //	 the server must pass true to the parameter to send arguments in CallClientEvent().
  //   me_obj = object pointer to the me object
  //   it_obj = object pointer to the it object
  virtual void OnClientCollide(object *me_obj, object *it_obj);
  virtual void OnClientCollide(object *me_obj, object *it_obj, vector *point, vector *normal);

  // DMFCBase::OnClientPlayerChangeSegment (Only called if server tells client to execute)
  //
  //   Event handler for when a player changes rooms or a "large" terrain cell
  //   (8x8 Terrain block).
  //   player_num = player number of the player who just changed the room/segment
  //   newseg = New room/cell location
  //	 oldseg = Old room/cell location
  virtual void OnClientPlayerChangeSegment(int player_num, int newseg, int oldseg);

  // DMFCBase::OnClientObjectChangeSegment (Only called if server tells client to execute)
  //
  //   Event handler for when a player changes rooms or a "large" terrain cell
  //   (8x8 Terrain block).
  //   obj = Object pointer of the object who just changed the room/segment
  //   newseg = New room/cell location
  //	 oldseg = Old room/cell location
  virtual void OnClientObjectChangeSegment(object *obj, int newseg, int oldseg);

  // DMFCBase::OnClientPlayerEntersGame (Only called if server tells client to execute)
  //
  //   Event handler for when a player enters the game.  This will only get called once per
  //   player, it usually gets called right after they connect to the server to start playing.
  //   player_num = player number of the player entering the game
  virtual void OnClientPlayerEntersGame(int player_num);

  // DMFCBase::OnClientPlayerDisconnect (Only called if server tells client to execute)
  //
  //   Event handler for when a player disconnects from the server.
  //   player_num = player number of the player that just disconnected
  virtual void OnClientPlayerDisconnect(int player_num);

  // DMFCBase::OnMeDisconnectFromServer
  //
  //	Event handler that gets called if we disconnect from the server for some reason (not purposesly quit)
  virtual void OnMeDisconnectFromServer(void);

  // DMFCBase::OnClientGameCreated (Only called if server tells client to execute)
  //
  //   Event handler when the server's game first gets started.  This will only be called once
  //   while the server is running, and that is when the server first starts up.
  virtual void OnClientGameCreated(void);

  // DMFCBase::OnClientObjectShieldsChanged
  //
  //	Event handler for when an objects shield's change
  virtual void OnClientObjectShieldsChanged(object *obj, float amount);

  // DMFCBase::OnWeaponFired
  //
  //	Event handler for when an object fires a weapon
  virtual void OnWeaponFired(object *weapon_obj, object *shooter);

  // DMFCBase::OnClientLevelChange (Only called if server tells client to execute)
  //
  //   Event handler when the server changes levels.  This will get called after a level ends and a server
  //   is starting a new level.
  virtual void OnClientLevelChange(void);

  // DMFCBase::OnClientLevelStart (Only called if server tells client to execute)
  //
  //   Event handler for when a multiplayer level is starting up.  This will get called right before the level
  //   starts.
  virtual void OnClientLevelStart(void);

  // DMFCBase::OnClientLevelEnd (Only called if server tells client to execute)
  //
  //   Event handler for when a multiplayer level is ending.
  virtual void OnClientLevelEnd(void);

  // DMFCBase::OnClientWallCollide
  //
  //	Called by the game when their is a collision between an object and a wall
  virtual void OnClientWallCollide(object *obj, float hitspeed, int hitseg, int hitwall, vector *hitpt,
                                   vector *wall_normal, float hit_dot);

  //	DMFCBase::OnClientObjectKilled
  //
  //	Called when an object is being killed
  //	Not automatically sent to clients (OnClientObjectKilled)
  //	killer might not be valid (NULL)
  virtual void OnClientObjectKilled(object *obj, object *killer);

  //	DMFCBase::OnClientObjectDestroyed
  //
  //	Called when an object is about to be deleted
  //	Not automatically sent to clients
  virtual void OnClientObjectDestroyed(object *obj);

  // DMFCBase::OnPlayerEntersObserver
  //
  //	Event handler when a player becomes an observer mode
  //	If they are piggybacking another player than piggy is the object pointer, else it's NULL
  virtual void OnPlayerEntersObserver(int pnum, object *piggy);

  // DMFCBase::OnPlayerExitsObserver
  //
  //	Event handler when a player is leaving observer mode
  virtual void OnPlayerExitsObserver(int pnum);

  // DMFCBase::OnCanChangeTeam
  //
  //    Called to determine if a player can change teams, you can override this so the server won't
  //    let a player change teams if it is not desired (for that player)
  virtual bool OnCanChangeTeam(int pnum, int newteam);

  // DMFCBase::OnSpecialPacket
  //
  //   Event handler for when a special packet arrives and needs to be processed.
  //   Both the server and client can get this event, although it is more common for the
  //   client to recieve these.
  virtual void OnSpecialPacket(void);

  // DMFCBase::OnInterval
  //
  //   Event handler that gets called once a frame
  virtual void OnInterval(void);

  // DMFCBase::OnHUDInterval
  //
  //	Event handler that gets called once a frame when it's time to render the HUD images
  virtual void OnHUDInterval(void);

  // DMFCBase::OnPLRInterval
  //
  //  Event handler that gets called once a frame when the Post Level Results screen is being display
  virtual void OnPLRInterval(void);

  // DMFCBase::OnPLRInit
  //
  //  Event handler that gets called the first frame of the PLR screen for each level
  virtual void OnPLRInit(void);

  // DMFCBase::OnKeypress
  //
  //   Event handler for when a user presses a key while in the game
  //   key = Key code of the key being pressed
  virtual void OnKeypress(int key);

  // DMFCBase::OnInputString
  //
  //   Event handler for when the user types a message at the console (F8) that begins with a $
  //   input_string = string that was typed
  virtual void OnInputString(char *input_string);

  // DMFCBase::OnPlayerChangeTeam
  //
  //  Called when a player changes team
  //	if announce is true than it is a change in the middle of the game, else it's an initial assignment change
  virtual void OnPlayerChangeTeam(int player_num, int newteam, bool announce, bool spew_everything);

  // DMFCBase::OnGameStateRequest
  //
  //		Server only.  Override this to listen for Game state requests from the clients
  //    When this function is called a client (who's player number is passed in) is requesting
  //    game state information.  Do what you need to do to send game state information to that player
  virtual void OnGameStateRequest(int pnum);

  // DMFCBase::OnSaveStatsToFile
  //
  //
  //		The user is requesting the game stats to be saved to file, you must handle this completly on
  //	the game's side, nothing is done in DMFC
  virtual void OnSaveStatsToFile(void);

  // DMFCBase::OnPlayerReconnect
  //
  //
  //	This player is reconnecting to the game
  virtual void OnPlayerReconnect(int player_num);

  // DMFCBase::OnPlayerConnect
  //
  //
  //	This player is connecting to the game for the first time
  virtual void OnPlayerConnect(int player_num);

  // DMFCBase::OnControlMessage
  //
  //
  //	There is a control message sent from someone
  virtual void OnControlMessage(uint8_t msg, int from_pnum);

  // DMFCBase::OnAllowObserverChange
  //
  //
  //	returns true if the requested change for observer mode should go through
  virtual bool OnAllowObserverChange(bool turnonobserver);

  // DMFCBase::OnClientShowUI
  //
  //
  //	The game is saying it's ok to do any UI.  Only handle the id's that belong to you and pass the rest
  //	to DMFCBase::OnClientShowUI().
  virtual void OnClientShowUI(int id, void *user_data);

  // DMFCBase::OnPrintScores
  //
  //
  //	The user is requesting that the scores be printed out (For Dedicated Server use).  Use DPrintf
  //	level:	-1	Requesting all the available scores
  //			n	Print only the top n scores
  //	Override this to how you see fit, but it should conform to the above.
  virtual void OnPrintScores(int level);

  // DMFCBase::OnDisconnectSaveStatsToFile
  //
  //
  //	The game should save the stats because the player (only ourself as the client in the game),
  //	has disconnected.
  virtual void OnDisconnectSaveStatsToFile(void);

  // DMFCBase::OnLevelEndSaveStatsToFile
  //
  //
  //	The game should save the "End of Level" stats to file.  You must handle this completly on
  //	the game's side, nothing is done in DMFC
  virtual void OnLevelEndSaveStatsToFile(void);

  // DMFCBase::OnGetHudCallSignColor
  //
  //	This is an event sent in by the game requesting what color it should draw the HUD callsign
  //	of the passed in playernum.  Using GR_RGB return the color from the function.  This function
  //	gets called every frame that the player is on the clients HUD.
  virtual ddgr_color OnGetHudCallSignColor(int playernum);

  // DMFCBase::TranslateEvent
  //
  //   Translates the event passed in to handle, calls the appropriate handler function.  If a function isn't
  //   created to handle the event, then it calls the default handler.  Handler functions should call the default
  //   handler unless it absolutly shouldn't
  virtual void TranslateEvent(int eventnum, dllinfo *data);

  // DMFCBase::OnTeamChangeName
  //
  //	This event occurs on the client when a team's name has just changed
  virtual void OnTeamChangeName(int team, char *oldname, char *newname);

  // DMFCBase::OnDoControls
  //
  //	This event occurs when there is a new back of game controls data
  virtual void OnDoControls(game_controls *controls);

  // DMFCBase::OnPlayAudioTaunt
  //
  //	This event occurs when a player plays an audio taunt
  virtual void OnPlayAudioTaunt(int pnum);

  //	DMFCBase::OnGetTokenString
  //
  //	This event occurs when a text macro is being used that has tokens
  //	in it, if you have a specific token, override this event to handle it.
  virtual void OnGetTokenString(char *src, char *dest, int dest_size);

  void DrawMenu(void);

  void DisplayPlayerInfo(int background_bmp = -1, bool dedicated_server = false);

  // DMFCBase::SwitchPlayerInfoDisplay
  //
  //	Switches on/off displaying a Playerinfo
  void SwitchPlayerInfoDisplay(int pnum);

  // DMFCBase::DisplayingPlayerInfo
  //
  //	Returns the pnum of who you are currently displaying, -1 if noone
  int DisplayingPlayerInfo(void);

  // DMFCBase::GetTeamFromString
  //
  //		Returns the int value of a team based on a string, -1 if not a team
  int GetTeamFromString(char *str);

  // DMFCBase::GetPlayerNum
  //
  //   Returns your playernum, useful when finding out if an event is referring to you (by looking at
  //   the playernum passed to the event handler, compared to your playernum
  int GetPlayerNum(void);

  // DMFCBase::GetLocalRole
  //
  //   Returns your role in the multiplayer game.  The result returned will be either LR_SERVER or LR_CLIENT
  int GetLocalRole(void);

  // DMFCBase::CheckPlayerNum
  //
  //    Returns true if the player number passed in is a valid player number (the player is connected)
  bool CheckPlayerNum(int player_num);

  //	DMFCBase::PacketCheckPlayerNum
  //
  //		Returns true if it's ok to send a packet to this player
  bool PacketCheckPlayerNum(int player_num);

  // DMFCBase::CallClientEvent
  //
  //    Server Only.  This will send an event to a client for it to execute.
  //    event = An EVT_CLIENT_*
  //    me_objnum,it_objnum = Object numbers of the objects to be me and it for the event
  //    destination = Player number of the client to send to, or -1 if to send to all
  void CallClientEvent(int event, int me_objnum, int it_objnum, int destination, bool parms = false);

  // DMFCBase::GetTimeLeft
  //
  //    Server Only.  This will fill in the float pointer passed to it with how much time is
  //    left in the multiplayer game.  The value placed in the float is only valid if GetTimeLeft
  //    returns true.  If it returns false it was because either it is not a game with a time limit
  //    or it is being called on a client.
  bool GetTimeLeft(float *time);

  // DMFCBase::EndLevel
  //
  //    Server Only.  This will end the current multiplayer level, and will go on to the next level.
  void EndLevel(void);

  // DMFCBase::WarpToLevel
  //
  //	Warps to another level in the mission.  Pass a level number (1 to Current_mission.num_levels)
  //	It will end the level and go to that level.
  void WarpToLevel(int lev);

  // DMFCBase::GetScoreLimit
  //
  //   Server Only.  This will fill in the int pointer passed to it with what the scoring limit
  //   is, set in in the multiplayer options.  The value point into the int is only valid if
  //   GetScoreLimit returns true.  If it returns false it is because you are either not the
  //   server or this option wasn't set in the multiplayer options
  bool GetScoreLimit(int *limit);

  // DMFCBase::AutoTimeLimit
  //
  //    Server Only.  This turns off or on the automatic level ending by DMFC of a timed multiplayer game.
  //    If you turn it off, it is your responsibility to end a time multiplayer game when it's time
  //    is up.  If it is turned on, DMFC will automatically handle ending the game.  By default it is on.
  void AutoTimeLimit(bool turnon);

  // DMFCBase::AutoDeathMessage
  //
  //    This turns on or off DMFC's automatic handling of death messages.  If it is turned on (Default)
  //    then when a player dies it will display a random death message from the list added by you using
  //    AddDeathMessage.  If it is turned off, then it is your responsibility to handle the messages, you
  //    can use DoRandomDeathMessage to display one when appropriate.
  void AutoDeathMessage(bool turnon);

  // DMFCBase::AddDeathMessage
  //
  //    This will add a death message to DMFC.
  //    format = string in a "printf" type format (using %s for player callsigns) of the message
  //    victim_first = Set this to true if the victim is listed first in the format
  void AddDeathMessage(const char *string, bool victim_first = true);

  // DMFCBase::AddSuicideMessage
  //
  //    This will add a death message to DMFC.
  //    format = string in a "printf" type format (using %s for player callsigns) of the message
  void AddSuicideMessage(const char *string);

  // DMFCBase::DoRandomDeathMessage
  //
  //     DMFC will display a death message (or suicide message if killer and victim are the same) when
  //     this function is called, based on the strings added with AddDeathMessage or AddSuicideMessage.
  //     killernum = object number of the killer
  //     victimnum = object number of the victim
  //		    hash = hash index of the weapon killer, -1 if none
  void DoRandomDeathMessage(int killernum, int victimnum, uint32_t hash = -1);

  // DMFCBase::GetItObjNum
  //
  // Returns the it object number of the current event for use.
  int GetItObjNum(void);

  // DMFCBase::GetMeObjNum
  //
  // Returns the me object number of the current event for use.
  int GetMeObjNum(void);

  // DMFCBase::RegisterPacketReceiver
  //
  // Sets up a handler for a Special Packet ID.  When a special packet is recieved it's ID is compared
  // to the ID's given to this function.  If any match than it calls the handler given to process
  // the packet.
  // id = ID of the packet
  // func = Function handler to handle the packet.  Must be declared like void MyFunction(uint8_t *data);
  void RegisterPacketReceiver(uint8_t id, void (*func)(uint8_t *));
  void RegisterPacketReceiver(uint8_t id, void (DMFCBase::*func)(uint8_t *));

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
  void StartPacket(uint8_t *data, uint8_t id, int *count);

  // DMFCBase::SendPacket
  //
  //   Ships a Special Packet away to the destination.  This call must be preceeded by a StartPacket call
  //	 data = buffer of data to be sent out (same buffer that was passed to StartPacket
  //   size = size (in bytes) of the packet
  //   destination = either a player number, SP_ALL for all the players or SP_SERVER to send to the server (if you are a
  //   client)
  void SendPacket(uint8_t *data, int size, int destination);

  // DMFCBase::GetTeamForNewPlayer
  //
  //    A Helper function (Server only), which will give you the optimum team assignment (whoever has the
  //    lowest amount of players on their team, for a new player.
  //    player_num = player number of the new player
  //    num_teams = The number of teams in your game
  int GetTeamForNewPlayer(int player_num, int num_teams);

  // DMFCBase::SetNumberOfTeams
  //
  //    Sets the number of teams to be used in the game.  By default there is 1 team (everyone against everyone).
  //    You can set up to a maximum of 4 teams.  Call this function as soon as possible.
  //    teams = number of teams
  void SetNumberOfTeams(int teams);

  // DMFCBase::AutoTeamSelect
  //
  //    Turns on or off DMFC's auto team assignment.  If it is on, then when a new player joins, they will
  //    be placed on the team with fewest players.  If it is off, then you must handle that.  Defualt on.
  void AutoTeamSelect(bool turnon);

  // DMFCBase::SendTeamAssignment
  //
  //     A DMFC Special Packet function, this sends a team assignment packet to all the players. Server only.
  //     playernum = player to change teams
  //     team = new team
  void SendTeamAssignment(int playernum, int team, bool spew_on_respawn);

  // DMFCBase::GetTeamAssignmentPacket
  //
  //     Reciever for the team assignment packet.
  void GetTeamAssignmentPacket(uint8_t *data);

  // DMFCBase::GetChangeTeamPacket
  //
  //     Reciever for the change team packet.(Server Only)
  void GetChangeTeamPacket(uint8_t *data);

  // DMFCBase::GetGameStateRequest
  //
  //		Receiver for the server from a client asking for the state of the game
  void GetGameStateRequest(uint8_t *data);

  // DMFCBase::SendChangeTeamRequest
  //
  //		Sends a request to the server to change teams (Client Only)
  //	spew_onchange should be true if you want the client the spew his inventory on the change
  void SendChangeTeamRequest(int newteam, bool spew_onchange);

  // DMFCBase::GetDMFCGameInfo
  //
  //		Receives information about the DMFC game
  void GetDMFCGameInfo(uint8_t *data);

  // DMFCBase::SendDMFCGameInfo
  //
  //		Sends information about the DMFC game
  void SendDMFCGameInfo(int player);

  // DMFCBase::GetRemoteKey
  //
  // Handles a new remote key from the server
  void GetRemoteKey(uint8_t *data);

  // DMFCBase::SendRemoteKey
  //
  // Sends out the players remote admin key
  void SendRemoteKey(int pnum);

  // DMFCBase::ChangeTeams
  //
  //   If you are the server it will make the player change teams.  If you
  //   are a client then it sends a request to the server to change teams
  //	 spew_on_repsawn is true if the player should spew his inventory before he respawns
  void RequestTeamChange(int team, int pnum, bool spew_on_respawn);

  // DMFCBase::RequestGameState
  //
  //		Sends a request to the server for the game state
  void RequestGameState(void);

  // DMFCBase::AddHudItemCallback
  //
  //    Adds an item to the hud.  Everytime the hud needs to be updated, it will call the
  //    handler passed in.  The handler must be declared as:
  //	  void func(struct tHudItem *item);
  //    type = HI_BITMAP  for bitmap, HI_TEXT for a text item
  //    func = function callback
  void AddHUDItemCallback(int type, void (*func)(struct tHUDItem *));

  // DMFCBase::GetMyTeam
  //
  //    Returns the int value of the team you are on...only useful in a team game.
  int GetMyTeam(void);

  // DMFCBase::GetTeamString
  //
  //     Returns a pointer to a string name of a team
  //   team = integer value of the team
  const char *GetTeamString(int team);

  // DMFCBase::GetTeamColor
  //
  //    Returns the color components of a team
  //   team = integer value of the team
  ddgr_color GetTeamColor(int team);

  // DMFCBase::GetNumTeams
  //
  //   Returns the number of teams in the game
  int GetNumTeams(void);

  // DMFCBase::AllowTeamChange
  //
  //		Returns true if team changing is allowed
  bool AllowTeamChange(void);

  // DMFCBase::SwitchAllowTeamChange
  //
  //		Turns on/off allowing of team changing
  void SwitchAllowTeamChange(bool turnon);

  // DMFCBase::GetSortedPlayerSlots
  //
  //  Fills in the passed array (of size maxsize) with the playernums sorted by (kills-suicides)
  void GetSortedPlayerSlots(int *sortedindex, int maxsize);

  //	DMFCBase::GetSortedPlayerSlotsByEfficiency
  //
  //	Fills in the passed array (of size maxsize) with the playernums sorted by kills/(kills+deaths+suicides)
  void GetSortedPlayerSlotsByEfficiency(int *sortedindex, int maxsize);

  // DMFCBase::IsMenuUp
  //
  //	  Returns true if the on screen menu is being shown
  bool IsMenuUp(void);

  // DMFCBase::ConvertHUDAlpha
  //
  //	  Returns a converted alpha based on what you give, it will be a more transparent if the onscreen menu is up
  float ConvertHUDAlpha(float normal);
  uint8_t ConvertHUDAlpha(uint8_t normal);

  // DMFCBase::ClipString
  //
  //	   Given a width, the string will be clipped to that width.  If you pass true for arrow, then an arrow will be
  //    appended if the string has been clipped.  Based off current font
  void ClipString(int width, char *string, bool arrow);

  // DMFCBase::DisplayOutrageLogo(void)
  //
  //   Displays the Outrage Logo for 5 seconds (call repeatedly, after 5 seconds it will just short circuit)
  //	Does nothing for non-Outrage releases
  void DisplayOutrageLogo(void);

  // DMFCBase::InitTimers
  //
  //	Inializes the timers
  void InitTimers(void);

  // DMFCBase::KillAllTimers
  //
  //	Kills all running timers
  void KillAllTimers(void);

  // DMFCBase::SetTimerInterval
  //
  //	Starts a timer event running
  int SetTimerInterval(void (*func)(void), float intval, float longevity, void (*onkill)(void) = NULL);

  // DMFCBase::KillTimer
  //
  // Kills a timer
  void KillTimer(int handle);

  // DMFCBase::ProcessTimers
  //
  //	Goes through all the timers and updates them if they need to be updated
  void ProcessTimers(void);

  // DMFCBase::SwitchShowHudCallsignLevel
  //
  //
  //   Sets the level for displaying of Player's Callsigns on the HUD
  void SwitchShowHudCallsignLevel(uint8_t level, bool announce = true);

  // DMFCBase::SwitchServerHudCallsignLevel
  //
  //
  //	Sets the max level of HUD callsign displayage...determined by the server
  void SwitchServerHudCallsignLevel(uint8_t level);

  // DMFCBase::GetCounterMeasureOwner
  //
  //
  //	Given a counter measure it will determine the pnum of it's owner...if it can't find it, it returns -1
  int GetCounterMeasureOwner(object *robot);

  // DMFCBase::CFGOpen
  //
  //	Opens the registry/cfg information for the DMFC game, see error codes for return values
  int CFGOpen(char *filename);

  // DMFCBase::CFGClose
  //
  //	Closes the registry information for the game.
  void CFGClose(void);

  // DMFCBase::CFGFlush
  //
  //	Flushes out the registry information to fall
  int CFGFlush(void);

  // DMFCBase::CFGCreateKey
  //
  //  Creates a key in the registry
  int CFGCreateKey(char *name);

  // DMFCBase::CFGLookupKey
  //
  //   Sets the active key in the registry
  int CFGLookupKey(char *name);

  // DMFCBase::CFGLookupRecord
  //
  //	Looks up a record in the active key
  int CFGLookupRecord(char *record, void *data);

  // DMFCBase::CFGCreateRecord
  //
  // Create/overwrites a record in the active key. Type is either REGT_DWORD or REGT_STRING
  int CFGCreateRecord(char *name, char type, void *data);

  // DMFCBase::DecryptData
  //
  // Decrypts a buffer of data
  void DecryptData(uint8_t *data, int size);

  // DMFCBase::EncryptData
  //
  //	Encrypts (weak) a buffer of data
  void EncryptData(uint8_t *data, int size);

  // DMFCBase::VersionCheck
  //
  // Makes sure Client has same DMFC version
  void VersionCheck(int pnum);

  // DMFCBase::FindPInfo
  //
  //	Given a player number it will search the list and find that player, returning the pointer, or NULL if it
  //	can't be found or created
  PInfo *FindPInfo(int pnum);

  // DMFCBase::UpdatePInfo
  //
  //
  //	Updates a victim's pinfo stat
  void UpdatePInfo(int victim, int killer, int amount);

  // DMFCBase::ResetPInfo
  //
  //
  //	Resets all the PInfo stats (and frees memory)
  void ResetPInfo(void);

  // DMFCBase::WriteDMFCStatsToFile
  //
  //
  //	This function will write out DMFC's stats to the file given (it must be opened for writing)
  void WriteDMFCStatsToFile(CFILE *file);

  // DMFCBase::SetWeaponDeathMessage
  //
  //	Sets a death message for a weapon kill (you only need to pass in one version of the weapon if it
  //  consists of multiple weapons...make sure AddWeaponHash has been called before this for this weapon)
  bool SetWeaponDeathMessage(const char *weapon_name, const char *message, bool victim_first);

  // DMFCBase::GetWeaponDeathMessage
  //
  //  Returns the format string for a weapon death message, NULL if it doesn't exist
  char *GetWeaponDeathMessage(int index, bool *victim_first);

  // DMFCBase::AddWeaponHash
  //
  //	Since one weapon may actually consist of many weapons, in order to save space you can create
  //  one weapon where all those other weapon id's will be mapped to it...use WeaponHash[id] to
  //  get the actual weapon.  End list of children with a NULL
  void AddWeaponHash(const char *parent, ...);

  // DMFCBase::AddWeaponHashArray
  //
  //	Since one weapon may actually consist of many weapons, in order to save space you can create
  //  one weapon where all those other weapon id's will be mapped to it...use WeaponHash[id] to
  //  get the actual weapon.
  void AddWeaponHashArray(const char *parent, int count, char **array);

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
  int SetupPlayerRecord(int sizeof_individual_data, int (*pack_callback)(void *user_info, uint8_t *data),
                        int (*unpack_callback)(void *user_info, uint8_t *data));

  // DMFCBase::GetPlayerRecordData
  //
  //	Returns a pointer to the memory set aside for a particular player (NULL on error)
  void *GetPlayerRecordData(int pnum);

  // DMFCBase::GetPlayerRecord
  //
  //	Returns a pointer to the player record at the given slot (passed in slot must be in the range
  // 0<=slot<MAX_PLAYER_RECORDS)
  player_record *GetPlayerRecord(int slot);

  // DMFCBase::GetPlayerRecordByPnum
  //
  //	Same as above, but you can specify by a player's pnum, return NULL on error
  player_record *GetPlayerRecordByPnum(int pnum);

  // DMFCBase::SendRequestForPlayerRecords
  //
  //
  //	Sends a request to the server to send the player records
  void SendRequestForPlayerRecords(void);

  // DMFCBase::ReceiveRequestForPlayerRecords
  //
  //
  //	Recieves and processes a request for a player record
  void ReceiveRequestForPlayerRecords(uint8_t *data);

  // DMFCBase::IsPlayerBanned
  //
  //	returns true is the given pnum is banned from the game
  bool IsPlayerBanned(int pnum);

  // DMFCBase::IsAddressBanned
  //
  //	returns true if the given address is banned from the game
  bool IsAddressBanned(network_address *addr, const char *tracker_id);

  // DMFCBase::RemoveAllBans
  //
  //	Removes all the temp bans
  void RemoveAllBans(void);

  // DMFCBase::RemoveBan
  //
  //	Removes a temp ban on given the ban #...returns true on success
  bool RemoveBan(int index);

  // DMCBase::GetBannedPlayerCallsign
  //
  //	Returns the callsign of the banned player at position index, NULL on error
  char *GetBannedPlayerCallsign(int index);

  // DMFCBase::GetNumBannedPlayers
  //
  //	Returns the number of players banned from the server
  int GetNumBannedPlayers(void);

  // DMFCBase::BanPlayerFromGame
  //
  //   Puts a temp ban on a player which will only last the duration of the game
  void BanPlayerFromGame(int pnum);

  // DMFCBase::IsPlayerAlive
  //
  //	Returns true is the given pnum is a player, flying around the level (not dying,
  //	dead or observing)
  bool IsPlayerAlive(int pnum);

  // DMFCBase::SendControlMessageToPlayer
  //
  //
  //	Sends a [1 byte] control message to a player
  void SendControlMessageToPlayer(int pnum, uint8_t msg);

  // DMFCBase::ReceiveControlMessage
  //
  //
  //	Handles a control message
  void ReceiveControlMessage(uint8_t *data);

  // DMFCBase::SendRealGametime
  //
  //	Updates a player about the 'real' game time
  void SendRealGametime(int pnum);

  // DMFCBase::GetRealGametime
  //
  //	handles a packet about updating the real game time
  void GetRealGametimePacket(uint8_t *data);

  // DMFCBase::FindPInfoStatFirst
  //
  //	Fills in a tPInfoStat that contains information about a given player record slot. Returns true on success, false
  //	if there is no information available for the given slot. Call this first, then follow with repeated calls to
  //	FindPInfoStatNext until you get a false value returned...call FindPInfoStatClose when done.
  bool FindPInfoStatFirst(int slot, tPInfoStat *stat);

  // DMFCBase::FindPInfoStatNext
  //
  //	Call this repeatedly until you get a value of false, finish by calling FindPInfoStatClose
  bool FindPInfoStatNext(tPInfoStat *stat);

  // DMFCBase::FindPInfoStatClose
  //
  //	Closes up a FindPInfo series of calls
  void FindPInfoStatClose(void);

  // DMFCBase::DoDamageToPlayer
  //
  //	Server only...applies damage to player
  void DoDamageToPlayer(int pnum, int type, float amount, bool playsound = true);

  // DMFCBase::StartUIWindow
  //
  //	Prepares the game so that you can display UI stuff, give a unique id in which will be passed
  //	to the OnClientShowUI().  In your OnClientShowUI() you must ONLY handle id's which belong to you
  //	and pass the others to DMFCBase::OnClientShowUI().
  void StartUIWindow(int id, void *user_data);

  //	DMFCBase::DisconnectMe
  //
  //	Disconnects yourself from the game.  It should be called when you are kicked or banned to make the
  //	whole process a bit nicer/cleaner.  Even if it doesn't get called when kicked, you will still
  //	eventually disconnect.
  void DisconnectMe(void);

  /*			Input Command Functions				*/
  // DMFCBase::AddInputCommand
  //
  //	When the client (or dedicated server) types a message that begins with a $, DMFC will look through
  //	all the values you passed to AddInputCommand() and see if any match.  If so it passes the entire
  //	command string to the given function handler. Returns 1 on success, -1 if out of memory, 0 if it already
  //	exists. These commands are not case sensitive.
  //	Ex. AddInputCommand("team");	//this handles all the '$team' passed in
  //  allow_remotely : if set true, this input command can be called remotely via remote administration
  int8_t AddInputCommand(const char *command, const char *description, void (*handler)(const char *), bool allow_remotely = false);

  // DMFCBase::CanInputCommandBeUsedRemotely
  //
  // Returns 1 if the input command can be called remotely
  // Returns -1 if the input command is not found
  // Returns 0 if the input command can be called remotely
  int CanInputCommandBeUsedRemotely(char *command);

  //	Does a check on on the pinfo info making sure it is valid
  void CheckPInfo();

  //	DMFCBase::EnableStatisticalMessages
  //
  //	Turns on or off the statistical HUD messages that appear due to a player death
  void EnableStatisticalMessages(bool on);

  //	DMFCBase::EnableOnScreenMenu
  //
  // Turns on/off the onscreen menu
  void EnableOnScreenMenu(bool turn_on);

  //	DMFCBase::EnableAutoSaveLevelEnd
  //
  //	Disables/Enables the autosaving of game stats to file on level end
  void EnableAutoSaveLevelEnd(bool enable);

  //	DMFCBase::EnableAutoSaveDisconnect
  //
  //	Disables/Enables the autosaving of game stats to file on disconnect from the game
  void EnableAutoSaveDisconnect(bool enable);

  //	DMFCBase::GenerateStatFilename
  //
  //	Given the following information it will return a full path to what
  //	the recommended filename to save stats to should be.
  //	root = Multiplayer DLL Name (filename will start with this)
  //	end_of_level = pass true if this is the end of a level stats
  void GenerateStatFilename(char *filename, const char *root, bool end_of_level);

  //	DMFCBase::IsPlayerObserver
  //
  //	Returns true if the given pnum is currently an observer in the game, else returns false.
  //	If an invalid pnum is given, then it returns false
  bool IsPlayerObserver(int pnum);

  //	DMFCBase::EnableOnScreenMenuBackground
  //
  //	Enables/disables the onscreen menu background
  void EnableOnScreenMenuBackground(bool enable);

  //	DMFCBase::GetTimeInGame
  //
  //	Returns the totaltime the player has been in the game at the moment you call
  //	this function.  Pass in the player record slot.
  float GetTimeInGame(int slot);

  //	DMFCBase::GetTimeString
  //
  //	Returns a string that contains a formated time (i.e. 32 seconds, 1:15 minutes, 3:21:23 hours)
  char *GetTimeString(float sec);

  //	DMFCBase::DisplayNetGameInfo
  //
  //	Displays information about the Netgame that is currently being played
  //	on to the screen.
  void DisplayNetGameInfo(int background_bmp = -1, bool dedicated_server = false);

  // DMFCBase::SwitchNetGameInfoDisplay
  //
  //	Switches on/off displaying netgame info
  void SwitchNetGameInfoDisplay(int on);

  //	DMFCBase::IsDisplayingNetGameInfo
  //
  //	returns true if we are currently displaying netgame info
  bool IsDisplayingNetGameInfo(void);

  // DatabaseRegister
  //		Registers your multiplayer game with the database.  This must be
  //	called before any other database function, or they will fail until this
  //	is called.
  void DatabaseRegister(char *name);
  // DatabaseRead
  //		Reads a string from the database
  bool DatabaseRead(const char *label, char *entry, int *entrylen);
  // DatabaseRead
  //		Reads wordsize bytes from the database
  bool DatabaseRead(const char *label, void *entry, int wordsize);
  // DatabaseRead
  //		Reads a bool from the database
  bool DatabaseRead(const char *label, bool *entry);
  // DatabaseWrite
  //		Writes/Updates a string to the database
  bool DatabaseWrite(const char *label, const char *entry, int entrylen);
  // DatabaseWrite
  //		Writes/Updates a value to the database
  bool DatabaseWrite(const char *label, int entry);
  // DMFCBase::SendNetGameInfoSync
  //
  //	Sends out a NetGame info sync packet to all the players
  void SendNetGameInfoSync(int to_who = SP_ALL);
  // DMFCBase::SetMaxPlayerHardLimit
  //
  //	Sets a hard limit to the max number of players allowed in the game
  //	changing the number of players can never go above this...defaults
  //	to DLLMAX_PLAYERS
  void SetMaxPlayerHardLimit(int max);

  // CompareNetworkAddress
  //
  //	Compare's two network addresses, returns true if they are the same, false if not.
  //	use_port: if this is true, than it will consider the port part of the network address
  bool CompareNetworkAddress(network_address *one, network_address *two, bool use_port = true);

  // IsMasterTrackerGame
  //
  //	returns true if this game is being played on the master tracker
  bool IsMasterTrackerGame(void);

  //	ConvertLocalToServerObjnum
  //
  //	Given an objnum, it will convert the number from your local objnum to the server's objnum
  //	It will return -1 on error
  int ConvertLocalToServerObjnum(int objnum);

  //	ConvertServerToLocalObjnum
  //
  //	Given an objnum from the server, this function will convert the objnum to your local objnum
  //  It will return -1 on error
  int ConvertServerToLocalObjnum(int objnum);

  // DMFCBase::ShouldIDisplayHUDName
  //
  //	Given a player num, it will determine if the callsign should be drawn on the HUD
  bool ShouldIDisplayHUDName(int pnum);

  // DMFCBase::SetDeathMessageFilter
  //
  // Sets the death message filter
  void SetDeathMessageFilter(int level);

  // DMFCBase::IAmDedicatedServer
  //
  //	Returns true if we are a dedicated server
  bool IAmDedicatedServer(void);

  // DMFCBase::IsPlayerDedicatedServer
  //
  //	Returns true if the passed in pnum/playerrecord is a dedicated server
  bool IsPlayerDedicatedServer(int pnum);
  bool IsPlayerDedicatedServer(player_record *pr);

  //	Displays dedicated server help
  void DisplayInputCommandHelp(const char *s);

  // DMFCBase::GetPlayerTeam
  //
  //	Returns the team of the player...call this instead of accessing .team directly
  int GetPlayerTeam(int pnum);

  // DMFCBase::GetConnectingPlayerTeam
  //
  //	Returns the team of a player just connecting, this is to be set by the
  //	game and is DMFC assumes that all players have the correct team on join
  int GetConnectingPlayerTeam(int slot);

  // DMFCBase::SetTeamName
  //
  //	Sets the team name for a given team
  //	team:	integer value of the team to change
  //	name:	new name for the team
  //	announce:	if this is true, and we are the server, it will tell all the clients about the change
  bool SetTeamName(int team, const char *name, bool announce);

  // DMFCBase::ConvertHUDCoord
  //
  //	Given an x,y based on a virtual 640x480 screen, this will convert it to the x,y that should be
  //	used based on the current screen size
  void ConvertHUDCoord(int x, int y, int *rx, int *ry);

  // DMFCBase::GetPlayerLogoBmp
  //
  //	Given a player_num, it will return a bitmap handle to that player's ship logo, or
  //	-1 if they don't have a logo for their ship.
  //	if is_vclip comes back as true, than it is not a bitmap handle, but a handle
  //	to a vclip (animated bitmap).  It is an index into the DLLGameVClips[].
  int GetPlayerLogoBmp(int player_num, bool *is_vclip);

  // DMFCBase::EnableShipLogos
  //
  //	Disables or enables logo displaying for the client
  void EnableShipLogos(bool enable);

  // DMFCBase::AreLogosEnabled
  //
  //	returns true if ship logos are enabled
  bool AreLogosEnabled(void);

  // DMFCBase::EnableAudioTaunts
  //
  //	Disables or enables audio taunts
  void EnableAudioTaunts(bool enable);

  // DMFCBase::AreTauntsEnabled(void)
  //
  //	returns true if audio taunts are enabled
  bool AreTauntsEnabled(void);

  // DMFCBase::EnableTimelimitCountdown
  //
  //	Enables/Disables the timelimit countdown (if there is a level time limit)
  //	Optionally you can specify what time to start the count down.
  void EnableTimelimitCountdown(bool enable, int seconds = 10);

  // DMFCBase::RespawnPlayer
  //
  //	Takes the given player and respawns his at a random start point (in a team game, team respawn
  //	points are taken into consideration, so make sure the player is on the correct team before you
  //	respawn him.  spew_everything, if false, will override spew_energy_and_shields
  void RespawnPlayer(int pnum, bool spew_energy_and_shield, bool spew_everything);

  // If you are going to create submenus you MUST use this function. along with:
  // void SetState(int state);
  // bool SetStateItemList(int count, ... ); for MIT_STATE items
  MenuItem *CreateMenuItem(const char *title, char type, uint8_t flags, void (*fp)(int), ...);

  // DMFCBase::ReadInHostsAllowDeny
  //
  //	Reads in the hosts.allow and hosts.deny files (if available)
  void ReadInHostsAllowDeny(void);

  // DMFCBase::FreeHostsLists
  //
  //	Frees all the memory allocated for the host allow/deny lists
  void FreeHostsLists(void);

  // DMFCBase::RehashAllowDeny
  //
  //	Flushes and reloads the hosts.allow/.deny lists
  void RehashAllowDeny(void);

  //	DMFCBase::AnnounceTeamChangeDeny
  //
  //	Tells a player that team change request was denied
  void AnnounceTeamChangeDeny(int pnum);

  //	DMFCBase::EnableLossGuage
  //
  //	Turns on/off the Loss guage on the screen
  void EnableLossGuage(bool enable);

  //	DMFCBase::WasPlayerInGameAtLevelEnd
  //
  //	Given a player record it returns the pnum of the player at that slot
  //	IF (and only if) the player was in the game at level end, else it
  //	returns -1.
  int WasPlayerInGameAtLevelEnd(int prec);

  // DMFCBase::SelectNextCameraView
  //
  //	This function, given which window (corresponds to left, middle, right), switches the
  //	current view of the small camera windows on the screen
  void SelectNextCameraView(int window);

  // DMFCBase::ResetPlayersInGame
  //
  //	Resets the Players-in-game list
  void ResetPlayersInGame(void);

  // DMFCBase::MarkPlayersInGame
  //
  //	Goes through all the player records and marks those that are in the game
  //	This is needed because at level end, all clients disconnect, so we lose
  //	that information.
  void MarkPlayersInGame(void);

  // DMFCBase::InputCommandHandle
  //
  //	Goes through the list of input commands and calls the associated handle (if it exists).  Returns
  //	true if it called a handler, else false.
  bool InputCommandHandle(char *command_string);

  // DMFCBase::PauseRealGameTime
  //
  //	Pauses/unpauses the realgame time
  void PauseRealGameTime(bool enable);

  //	DMFCBase::MakeClientsWait
  //
  //	If passed true than all joining clients are forced to wait until they are given the signal to
  //	join, either through SignalClientStart() or by passing false to MakeClientsWait
  void MakeClientsWait(bool wait);

  //	DMFCBase::SetDedicatedWait
  //
  //	Sets the level wait time for the dedicated server.  A dedicated server will make all
  //	clients wait until this time (in seconds) is up each level.
  void SetDedicatedWait(float time_secs);

  // DMFCBase::TranslateTextMacro
  //
  //	Given a macro with tokens, this function will replace the tokens
  //	and create a new string for display.
  //	All tokens begin with $$ (i.e.  $$TEAMBASE)
  void TranslateTextMacro(const char *src, char *destination, int dest_size);

  dllinfo *GetDLLInfoCallData(void);
  int GetHighestRoomIndex(void);
  int GetGameWindowW(void);
  int GetGameWindowH(void);
  int GetGameWindowX(void);
  int GetGameWindowY(void);
  int *GetGameFontTranslateArray(void);
  int GetObserverModeBitmap(void);
  float GetFrametime(void);
  float GetGametime(void);
  float GetRealGametime(bool *ispaused = NULL);
  float *GetShieldDeltaArray(void);
  float GetHudAspectX(void);
  float GetHudAspectY(void);
  const char *GetLocalD3Dir(void);
  const tMission *GetCurrentMission(void);
  room *GetRooms(void);
  object *GetObjects(void);
  terrain_segment *GetTerrainSegs(void);
  netgame_info *GetNetgameInfo(void);
  player *GetPlayers(void);
  netplayer *GetNetPlayers(void);
  ship *GetShips(void);
  weapon *GetWeapons(void);
  texture *GetGameTextures(void);
  vclip *GetGameVClips(void);
  ddgr_color *GetPlayerColors(void);
  poly_model *GetGamePolyModels(void);
  game_controls GetLastGameControls(void);
  int *GetPilotPicBitmapHandles(void);
  void GetViewerObjectPtr(object **v_obj);
  void SetViewerObjectPtr(object *v_obj);
  float GetRenderZoom(void);
  IMenuItem *GetOnScreenMenu(void);
  tOSIRISModuleInit *GetOsirisModuleData(void);
  vis_effect *GetVisEffectArray(int **Highest_viseffect_index_ptr = NULL);
  level_info *GetLevelInfo(void);
  char *GetGameArg(int index);
  int GetCameraViewType(int window);
  object_info *GetObjectInfo(int objinfoid);

public:
  void Delete(void);
  void *Dynamic_Cast(const char *pszType);
  void DuplicatePointer(void);
  void DestroyPointer(void);

public:
  dllinfo *Data;           // Current event's data
  int *Player_num;         // Your player num
  int *Highest_room_index; // the highest room index (for room number validation)
  int *Game_window_w;      // visible screen width
  int *Game_window_h;      // visible screen height
  int *Game_window_x;      // visible screen top-left x
  int *Game_window_y;      // visible screen top-left y
  int *Game_fonts;         // array of fonts
  int hBitmapObserver;     // Bitmap handle for the 'in observer mode' icon
  float *Frametime;        // how long it took the last frame to display
  float *Gametime;         // how long the game has been running
  float *ShieldDelta;      // array of player's...server update's their shields based on this array
  float *Hud_aspect_x, *Hud_aspect_y;
  char *LocalD3Dir;          // Path to the root of the Descent3 installation directory
  tMission *Current_mission; // the current mission
  game_api API;
  room *Rooms;                  // rooms array
  object *Objects;              // objects array
  terrain_segment *Terrain_seg; // Terrain segment array
  netgame_info *Netgame;        // Netgame info
  player *Players;              // Player's array
  netplayer *NetPlayers;        // NetPlayer's array
  ship *Ships;                  // Ship's array
  weapon *Weapons;              // Weapon's array
  texture *GameTextures;        // Texture's array
  vclip *GameVClips;            // VClip array
  ddgr_color *Player_colors;
  poly_model *Poly_models;          // polymodel array
  game_controls Last_game_controls; // The last frame's set of control data
  int PilotPicBmpHandles[DLLMAX_PLAYERS];
  MenuItem Menu;
  object **Viewer_object; // object the player's view is from
  float *Render_zoom;
  vis_effect *VisEffects;
  int *Highest_vis_effect_index;
  int *Multi_next_level;
  level_info *Level_info;
  void *GameArgs;
  object_info *Object_info;

  int Hard_max_players;

protected:
  void SetPlayerInfo(int pnum);
  void SendVersionToClient(int pnum);
  void GetDMFCVersionCheck(uint8_t *data);
  bool DMFC_compare_slots(int a, int b);
  bool DMFC_compare_slots_efficiency(int a, int b);

  // DMFCBase::InputCommandFree
  //
  //	Frees up all memory allocated for the input commands ($ messages)
  void InputCommandFree(void);
  // DMFCBase::InputCommandInit
  //
  //	Initializes the variables and data for the input commands.  Default DMFC commands are to
  //	be placed in here.
  void InputCommandInit(void);
  //	DMFCBase::GetStatisticalDeathMessage
  //
  //	Generates a statistical death message about the deaths
  void GetStatisticalDeathMessage(int knum, int vnum);
  //	DMFCBase::GetStatisticalSuicideMessage
  //
  //	Generates a statistical suicide message about the suicide
  char *GetStatisticalSuicideMessage(int pnum);

  //	DMFCBase::DoDMFCUITeamPlacement
  //
  //	Displays the dialog for doing team placement in DMFC
  void DoDMFCUITeamPlacement(bool make_clients_wait, bool called_by_level_start = true);

  //	DMFCBase::DoDMFCUIWaitForPlayers
  //
  //	Displays the dialog for in the beginning of the level, waiting for all the players
  void DoDMFCUIWaitForPlayers(bool clients_wait);

  //	DMFCBase::SignalClientStart
  //
  //	Given a player num, it tells the client that they can start moving and playing
  void SignalClientStart(int pnum);

  //	DMFCBase::WaitForServer
  //
  //	Call this to start waiting, until the server tells us to stop (by passing false as a parm)
  void WaitForServer(bool wait);

  //	DMFCBase::WaitingForServerLoop
  //
  //	Called per frame while waiting for the server to signal go
  void WaitingForServerLoop(void);

  // DMFCBase::DisplayTimelimitCountdown
  //
  //	Displays (if needed) the time limit countdown
  void DisplayTimelimitCountdown(void);

  int m_iProtectedFlags;
  int m_iDeathMsgCount;
  int m_iSuicideMsgCount;
  int m_iNumTeams;        // how many teams there are
  int m_iPlayerDisplayed; // the pnum of the player's info being displayed (if above is true)
  int MenuBackgroundBMP;

  int *Game_interface_mode;
  tSPHandler *SPRoot;                        // Root node of the Special packet handler list
  tBanItem *m_BanList;                       // root node of ban addresses
  tInputCommandNode *m_InputCommandRootNode; // root node for input commands ($ messages)

  MenuItem *TeamMenuItem;
  MenuItem *AutoBalanceItem;
  MenuItem *AllowTeamChangeItem;
  MenuItem *ObserverItem;
  MenuItem *HUDCallsignsItem;
  MenuItem *ServerHUDCallsignsItem;
  MenuItem *StatisticMessagesItem;
  MenuItem *SaveStatsLevelEndItem;
  MenuItem *SaveStatsDisconnectItem;
  MenuItem *MenuBackgroundItem;
  MenuItem *NetGameInfoItem;
  MenuItem *DeathMessageFilterItem;
  MenuItem *ShipLogosItem;
  MenuItem *AudioTauntsItem;
  MenuItem *HUDIndicatorItem;

  tDMFCTimerInfo Timers[MAX_DMFC_TIMERS];
  tWeaponKillMsg WeaponMessages[MAX_WEAPONS];
  int WeaponHash[MAX_WEAPONS];
  tDeathMsg DeathMsgs[MAX_DEATH_MSGS];
  tDeathMsg SuicideMsgs[MAX_DEATH_MSGS];
  char DMFC_team_names[DLLMAX_TEAMS][MAX_TEAMNAME_LEN]; // strings that are the team names
private:
  int m_cPtrs;
  bool m_bMakeClientsWait;
  bool m_bHasInitialized;
  bool m_bDisplayOutrageLogo;
  int *m_bTrackerGame;
  bool *Dedicated_server;
  uint8_t m_iServerHUDCallsignLevel;
  uint8_t m_iMyPreferredHUDCallsignLevel; // what the user selected for callsign level
  uint8_t m_iMyCurrentHUDCallsignLevel;   // might be lower than preferred level if limited by server
  uint8_t m_iDeathMessageFilter;
  int m_iNumBanPlayers;
  int m_iUIWindowID;
  void *m_UIUserData;
  uint16_t *Local_object_list;
  uint16_t *Server_object_list;
  tHostsNode *m_DenyList;
  tHostsNode *m_AllowList;
  bool LossGuageEnabled;
  bool DMFCInit;
  bool DisplayTauntIndicator;
  bool m_bDisplayTimelimitCountdown;
  int TauntIndicatorPlayerNum;
  int TauntIndicatorBMP;
  float TauntIndicatorStartTime;
  float RealGametime;
  float DedicatedLevelWait;
  int m_iTimeLimitCountdown;
  int *Camera_view_mode;

  char DatabaseRegisteredName[MAX_DBNAME_SIZE];
  int16_t players_in_game[MAX_PLAYER_RECORDS];

  // DMFCBase::InitializeForLevel
  //
  //
  void InitializeForLevel(void);

  // DMFCBase::LoadSettings
  //
  //	Loads the settings of DMFC from the registry and initializes those variables
  void LoadSettings(void);
  // DMFCBase::SaveSettings
  //
  //	Saves out the settings of DMFC to the registry
  void SaveSettings(void);
  // DMFCBase::ReceiveNetGameInfoSync
  //
  //  Receives a NetGame info sync packet from the server
  void ReceiveNetGameInfoSync(uint8_t *data);
  // DMFCBase::SendNewTeamName
  //
  //	Tells the clients about a team's new name
  void SendNewTeamName(int team);
  // DMFCBase::ReceiveNewTeamName
  //
  //	The server is telling us about a new team name
  void ReceiveNewTeamName(uint8_t *data);

  //	DMFCBase::ParseStartupScript
  //
  //
  //	Loads up the startup script and sets variables accordingly
  void ParseStartupScript(void);

  //	DMFCBase::LossGuageFrame
  //
  //	Processes the Loss guage for the frame
  void LossGuageFrame(void);

  //	DMFCBase::SetPlayerTauntIndicator
  //
  //	Sets up a taunt indicator for display
  void SetPlayerTauntIndicator(int pnum);

  //	DMFCBase::DisplayTauntIndicatorFrame
  //
  //	Performs any needed actions to display taunt indicator
  void DisplayTauntIndicatorFrame(void);

  //---------------------------------------------------------------------------------
  //---------------------------------------------------------------------------------
  //---------------------------------------------------------------------------------
  //------------------  VIRTUAL EVENT HANDLER FUNCTIONS  ----------------------------
  //---------------------------------------------------------------------------------
  //---------------------------------------------------------------------------------
  //---------------------------------------------------------------------------------
public:
  void Set_OnServerPlayerKilled(void (*callback)(object *killer_obj, int victim_pnum));
  void Set_OnServerPlayerExploded(void (*callback)(int player_num));
  void Set_OnServerCollide(void (*callback)(object *me_obj, object *it_obj));
  void Set_OnServerCollide(void (*callback)(object *me_obj, object *it_obj, vector *point, vector *normal));
  void Set_OnServerPlayerChangeSegment(void (*callback)(int player_num, int newseg, int oldseg));
  void Set_OnServerObjectChangeSegment(void (*callback)(object *obj, int newseg, int oldseg));
  void Set_OnServerPlayerEntersGame(void (*callback)(int player_num));
  void Set_OnServerPlayerDisconnect(void (*callback)(int player_num));
  void Set_OnServerGameCreated(void (*callback)(void));
  void Set_OnServerLevelChange(void (*callback)(void));
  void Set_OnServerLevelStart(void (*callback)(void));
  void Set_OnServerLevelEnd(void (*callback)(void));
  void Set_OnServerObjectShieldsChanged(void (*callback)(object *obj, float amount));
  void Set_OnServerIsAddressBanned(bool (*callback)(network_address *addr, const char *tracker_id));
  void Set_OnServerWallCollide(void (*callback)(object *obj, float hitspeed, int hitseg, int hitwall, vector *hitpt,
                                                vector *wall_normal, float hit_dot));
  void Set_OnServerObjectKilled(void (*callback)(object *obj, object *killer));
  void Set_OnServerObjectDestroyed(void (*callback)(object *obj));
  void Set_OnClientPlayerKilled(void (*callback)(object *killer_obj, int victim_pnum));
  void Set_OnClientPlayerExploded(void (*callback)(int player_num));
  void Set_OnClientCollide(void (*callback)(object *me_obj, object *it_obj));
  void Set_OnClientCollide(void (*callback)(object *me_obj, object *it_obj, vector *point, vector *normal));
  void Set_OnClientPlayerChangeSegment(void (*callback)(int player_num, int newseg, int oldseg));
  void Set_OnClientObjectChangeSegment(void (*callback)(object *obj, int newseg, int oldseg));
  void Set_OnClientPlayerEntersGame(void (*callback)(int player_num));
  void Set_OnClientPlayerDisconnect(void (*callback)(int player_num));
  void Set_OnMeDisconnectFromServer(void (*callback)(void));
  void Set_OnClientGameCreated(void (*callback)(void));
  void Set_OnClientObjectShieldsChanged(void (*callback)(object *obj, float amount));
  void Set_OnWeaponFired(void (*callback)(object *weapon_obj, object *shooter));
  void Set_OnClientLevelChange(void (*callback)(void));
  void Set_OnClientLevelStart(void (*callback)(void));
  void Set_OnClientLevelEnd(void (*callback)(void));
  void Set_OnClientWallCollide(void (*callback)(object *obj, float hitspeed, int hitseg, int hitwall, vector *hitpt,
                                                vector *wall_normal, float hit_dot));
  void Set_OnClientObjectKilled(void (*callback)(object *obj, object *killer));
  void Set_OnClientObjectDestroyed(void (*callback)(object *obj));
  void Set_OnPlayerEntersObserver(void (*callback)(int pnum, object *piggy));
  void Set_OnPlayerExitsObserver(void (*callback)(int pnum));
  void Set_OnCanChangeTeam(bool (*callback)(int pnum, int newteam));
  void Set_OnSpecialPacket(void (*callback)(void));
  void Set_OnInterval(void (*callback)(void));
  void Set_OnHUDInterval(void (*callback)(void));
  void Set_OnPLRInterval(void (*callback)(void));
  void Set_OnPLRInit(void (*callback)(void));
  void Set_OnKeypress(void (*callback)(int key));
  void Set_OnInputString(void (*callback)(char *input_string));
  void Set_OnPlayerChangeTeam(void (*callback)(int player_num, int newteam, bool announce, bool spew_everything));
  void Set_OnGameStateRequest(void (*callback)(int pnum));
  void Set_OnSaveStatsToFile(void (*callback)(void));
  void Set_OnPlayerReconnect(void (*callback)(int player_num));
  void Set_OnPlayerConnect(void (*callback)(int player_num));
  void Set_OnControlMessage(void (*callback)(uint8_t msg, int from_pnum));
  void Set_OnAllowObserverChange(bool (*callback)(bool turnonobserver));
  void Set_OnClientShowUI(void (*callback)(int id, void *user_data));
  void Set_OnPrintScores(void (*callback)(int level));
  void Set_OnDisconnectSaveStatsToFile(void (*callback)(void));
  void Set_OnLevelEndSaveStatsToFile(void (*callback)(void));
  void Set_OnGetHudCallSignColor(ddgr_color (*callback)(int playernum));
  void Set_OnTeamChangeName(void (*callback)(int team, char *oldname, char *newname));
  void Set_OnDoControls(void (*callback)(game_controls *controls));
  void Set_OnPlayAudioTaunt(void (*callback)(int pnum));
  void Set_OnGetTokenString(void (*callback)(char *src, char *dest, int dest_size));

  void CallOnServerPlayerKilled(object *killer_obj, int victim_pnum);
  void CallOnServerPlayerExploded(int player_num);
  void CallOnServerCollide(object *me_obj, object *it_obj);
  void CallOnServerCollide(object *me_obj, object *it_obj, vector *point, vector *normal);
  void CallOnServerPlayerChangeSegment(int player_num, int newseg, int oldseg);
  void CallOnServerObjectChangeSegment(object *obj, int newseg, int oldseg);
  void CallOnServerPlayerEntersGame(int player_num);
  void CallOnServerPlayerDisconnect(int player_num);
  void CallOnServerGameCreated(void);
  void CallOnServerLevelChange(void);
  void CallOnServerLevelStart(void);
  void CallOnServerLevelEnd(void);
  void CallOnServerObjectShieldsChanged(object *obj, float amount);
  bool CallOnServerIsAddressBanned(network_address *addr, const char *tracker_id);
  void CallOnServerWallCollide(object *obj, float hitspeed, int hitseg, int hitwall, vector *hitpt, vector *wall_normal,
                               float hit_dot);
  void CallOnServerObjectKilled(object *obj, object *killer);
  void CallOnServerObjectDestroyed(object *obj);
  void CallOnClientPlayerKilled(object *killer_obj, int victim_pnum);
  void CallOnClientPlayerExploded(int player_num);
  void CallOnClientCollide(object *me_obj, object *it_obj);
  void CallOnClientCollide(object *me_obj, object *it_obj, vector *point, vector *normal);
  void CallOnClientPlayerChangeSegment(int player_num, int newseg, int oldseg);
  void CallOnClientObjectChangeSegment(object *obj, int newseg, int oldseg);
  void CallOnClientPlayerEntersGame(int player_num);
  void CallOnClientPlayerDisconnect(int player_num);
  void CallOnMeDisconnectFromServer(void);
  void CallOnClientGameCreated(void);
  void CallOnClientObjectShieldsChanged(object *obj, float amount);
  void CallOnWeaponFired(object *weapon_obj, object *shooter);
  void CallOnClientLevelChange(void);
  void CallOnClientLevelStart(void);
  void CallOnClientLevelEnd(void);
  void CallOnClientWallCollide(object *obj, float hitspeed, int hitseg, int hitwall, vector *hitpt, vector *wall_normal,
                               float hit_dot);
  void CallOnClientObjectKilled(object *obj, object *killer);
  void CallOnClientObjectDestroyed(object *obj);
  void CallOnPlayerEntersObserver(int pnum, object *piggy);
  void CallOnPlayerExitsObserver(int pnum);
  bool CallOnCanChangeTeam(int pnum, int newteam);
  void CallOnSpecialPacket(void);
  void CallOnInterval(void);
  void CallOnHUDInterval(void);
  void CallOnPLRInterval(void);
  void CallOnPLRInit(void);
  void CallOnKeypress(int key);
  void CallOnInputString(char *input_string);
  void CallOnPlayerChangeTeam(int player_num, int newteam, bool announce, bool spew_everything);
  void CallOnGameStateRequest(int pnum);
  void CallOnSaveStatsToFile(void);
  void CallOnPlayerReconnect(int player_num);
  void CallOnPlayerConnect(int player_num);
  void CallOnControlMessage(uint8_t msg, int from_pnum);
  bool CallOnAllowObserverChange(bool turnonobserver);
  void CallOnClientShowUI(int id, void *user_data);
  void CallOnPrintScores(int level);
  void CallOnDisconnectSaveStatsToFile(void);
  void CallOnLevelEndSaveStatsToFile(void);
  ddgr_color CallOnGetHudCallSignColor(int playernum);
  void CallOnTeamChangeName(int team, char *oldname, char *newname);
  void CallOnDoControls(game_controls *controls);
  void CallOnPlayAudioTaunt(int pnum);
  void CallOnGetTokenString(char *src, char *dest, int dest_size);

private:
  //	DMFCBase::InitializeVirtualHandles
  //
  //		Prepares the virtual handles
  void InitializeVirtualHandles(void);

  void (*UserOnServerPlayerKilled)(object *killer_obj, int victim_pnum);
  void (*UserOnServerPlayerExploded)(int player_num);
  void (*UserOnServerCollide)(object *me_obj, object *it_obj);
  void (*UserOnServerCollide2)(object *me_obj, object *it_obj, vector *point, vector *normal);
  void (*UserOnServerPlayerChangeSegment)(int player_num, int newseg, int oldseg);
  void (*UserOnServerObjectChangeSegment)(object *obj, int newseg, int oldseg);
  void (*UserOnServerPlayerEntersGame)(int player_num);
  void (*UserOnServerPlayerDisconnect)(int player_num);
  void (*UserOnServerGameCreated)(void);
  void (*UserOnServerLevelChange)(void);
  void (*UserOnServerLevelStart)(void);
  void (*UserOnServerLevelEnd)(void);
  void (*UserOnServerObjectShieldsChanged)(object *obj, float amount);
  bool (*UserOnServerIsAddressBanned)(network_address *addr, const char *tracker_id);
  void (*UserOnServerWallCollide)(object *obj, float hitspeed, int hitseg, int hitwall, vector *hitpt,
                                  vector *wall_normal, float hit_dot);
  void (*UserOnServerObjectKilled)(object *obj, object *killer);
  void (*UserOnServerObjectDestroyed)(object *obj);
  void (*UserOnClientPlayerKilled)(object *killer_obj, int victim_pnum);
  void (*UserOnClientPlayerExploded)(int player_num);
  void (*UserOnClientCollide)(object *me_obj, object *it_obj);
  void (*UserOnClientCollide2)(object *me_obj, object *it_obj, vector *point, vector *normal);
  void (*UserOnClientPlayerChangeSegment)(int player_num, int newseg, int oldseg);
  void (*UserOnClientObjectChangeSegment)(object *obj, int newseg, int oldseg);
  void (*UserOnClientPlayerEntersGame)(int player_num);
  void (*UserOnClientPlayerDisconnect)(int player_num);
  void (*UserOnMeDisconnectFromServer)(void);
  void (*UserOnClientGameCreated)(void);
  void (*UserOnClientObjectShieldsChanged)(object *obj, float amount);
  void (*UserOnWeaponFired)(object *weapon_obj, object *shooter);
  void (*UserOnClientLevelChange)(void);
  void (*UserOnClientLevelStart)(void);
  void (*UserOnClientLevelEnd)(void);
  void (*UserOnClientWallCollide)(object *obj, float hitspeed, int hitseg, int hitwall, vector *hitpt,
                                  vector *wall_normal, float hit_dot);
  void (*UserOnClientObjectKilled)(object *obj, object *killer);
  void (*UserOnClientObjectDestroyed)(object *obj);
  void (*UserOnPlayerEntersObserver)(int pnum, object *piggy);
  void (*UserOnPlayerExitsObserver)(int pnum);
  bool (*UserOnCanChangeTeam)(int pnum, int newteam);
  void (*UserOnSpecialPacket)(void);
  void (*UserOnInterval)(void);
  void (*UserOnHUDInterval)(void);
  void (*UserOnPLRInterval)(void);
  void (*UserOnPLRInit)(void);
  void (*UserOnKeypress)(int key);
  void (*UserOnInputString)(char *input_string);
  void (*UserOnPlayerChangeTeam)(int player_num, int newteam, bool announce, bool spew_everything);
  void (*UserOnGameStateRequest)(int pnum);
  void (*UserOnSaveStatsToFile)(void);
  void (*UserOnPlayerReconnect)(int player_num);
  void (*UserOnPlayerConnect)(int player_num);
  void (*UserOnControlMessage)(uint8_t msg, int from_pnum);
  bool (*UserOnAllowObserverChange)(bool turnonobserver);
  void (*UserOnClientShowUI)(int id, void *user_data);
  void (*UserOnPrintScores)(int level);
  void (*UserOnDisconnectSaveStatsToFile)(void);
  void (*UserOnLevelEndSaveStatsToFile)(void);
  ddgr_color (*UserOnGetHudCallSignColor)(int playernum);
  void (*UserOnTeamChangeName)(int team, char *oldname, char *newname);
  void (*UserOnDoControls)(game_controls *controls);
  void (*UserOnPlayAudioTaunt)(int pnum);
  void (*UserOnGetTokenString)(char *src, char *dest, int dest_size);
};

#define ENABLE_FLAGS(f, i) (f |= (i))
#define DISABLE_FLAGS(f, i) (f &= ~(i))

#define DMFC_PRF_PLRFIRSTTIME 0x00000001  // This flag will be enabled if the Post Level Results has had one frame shown
#define DMFC_PRF_AUTOTIMELIMIT 0x00000002 // Enabled: DMFC should handle ending a timed multiplayer game
#define DMFC_PRF_AUTODEATHMSG 0x00000004  // Enabled: DMFC should handle displaying death messages
#define DMFC_PRF_AUTOTEAMSELECT 0x00000008   // Enabled: DMFC should handle initial team assignments
#define DMFC_PRF_DISPPLAYERINFO 0x00000010   // Enabled: DMFC is currently displaying a player's info on the screen
#define DMFC_PRF_DISPNETGAMEINFO 0x00000020  // Enabled: DMFC is currently displaying net game info
#define DMFC_PRF_DISPSTATHUDMSGS 0x00000040  // Enabled: DMFC should display random statistical messages
#define DMFC_PRF_AUTOSAVELEVELEND 0x00000080 // Enabled: DMFC should signal an auto-save when the level ends
#define DMFC_PRF_AUTOSAVEDISC 0x00000100     // Enabled: DMFC should signal an auto-save if the player gets disconnected
#define DMFC_PRF_DISPMENUBACKGR 0x00000200   // Enabled: DMFC should put a background on the On-Screen menu
#define DMFC_PRF_INOPTIONS 0x00000400        // Enabled: DMFC is currently displayed the On-Screen menu
#define DMFC_PRF_CANTEAMCHANGE 0x00000800    // Enabled: The server is allowing client specified team changing
#define DMFC_PRF_NOTEAMSELECTONSTART                                                                                   \
  0x00001000 // Enabled: DMFC should NOT display the Team Select dialog on startup of a team game level
#define DMFC_PRF_SHIPLOGOSENABLED 0x00002000 // Enabled: Display ship logos
#define DMFC_PRF_NOWAITFORPLAYERS                                                                                      \
  0x00004000 // Enabled: DMFC should NOT display the wait for players dialog on startup of a single player game
#define DMFC_PRF_PAUSETIME 0x00008000 // Enabled: Real game time (used for endlevel) is not updated

///////////////////////////////////////////////
// localization info
#define DTXT(d) DMFCGetString(d)
#define DTXT_KILLED1 DTXT(0)             //"%s was killed by %s"
#define DTXT_SUICIDE1 DTXT(1)            //"%s blasts themself"
#define DTXT_MNUSRVRCOMMAND DTXT(2)      //"Server Commands"
#define DTXT_MNUKICK DTXT(3)             //"Kick"
#define DTXT_MNUBAN DTXT(4)              //"Ban"
#define DTXT_MNUENDLEVEL DTXT(5)         //"End Level"
#define DTXT_MNUTEAMCONTROL DTXT(6)      //"Team Control"
#define DTXT_MNUBALANCE DTXT(7)          //"Balance Teams"
#define DTXT_MNUAUTOBALANCE DTXT(8)      //"AutoTeam Balance"
#define DTXT_MNUALLOWTEAMCNG DTXT(9)     //"Allow Team Changing"
#define DTXT_MNUON DTXT(10)              //"On"
#define DTXT_MNUOFF DTXT(11)             //"Off"
#define DTXT_MNUGETPLYRINFO DTXT(12)     //"Get Player Info"
#define DTXT_MNUOBSERVER DTXT(13)        //"Observer Mode"
#define DTXT_MNUCHANGETEAMS DTXT(14)     //"Change Teams"
#define DTXT_RED DTXT(15)                //"Red"
#define DTXT_BLUE DTXT(16)               //"Blue"
#define DTXT_GREEN DTXT(17)              //"Green"
#define DTXT_YELLOW DTXT(18)             //"Yellow"
#define DTXT_SERVER DTXT(19)             //"Server"
#define DTXT_CLIENT DTXT(20)             //"Client"
#define DTXT_TEAMFORMAT DTXT(21)         //"Team: %s"
#define DTXT_PLRNUMFORMAT DTXT(22)       //"PlayerNum: %d"
#define DTXT_TCPIPADDRFORM DTXT(23)      //"TCP/IP Address: %s"
#define DTXT_SHIPFORM DTXT(24)           //"Ship: %s"
#define DTXT_STRAYFIREFORM DTXT(25)      //"%s was killed by a U.F.O."
#define DTXT_LONER DTXT(26)              //"Loner"
#define DTXT_SAVESTATS DTXT(27)          //"Save Stats To File"
#define DTXT_ROBOTKILL DTXT(28)          //"%s was killed by a robot"
#define DTXT_MNUSHOWCALLSIGNS DTXT(29)   //"HUD Callsign Level"
#define DTXT_KICKEDMSG DTXT(30)          //"You've Been Kicked From The Game"
#define DTXT_BANNEDMSG DTXT(31)          //"You've Been Banned From This Game"
#define DTXT_MNUSSHOWCALLSIGNS DTXT(32)  //"Max HUD Callsign Level"
#define DTXT_NONE DTXT(33)               //"<None>"
#define DTXT_DEATHLASER DTXT(34)         //%s's Laser blasts %s to smithereens"
#define DTXT_DEATHVAUSS DTXT(35)         //"%s punctures %s's ship with the Vauss"
#define DTXT_DEATHMICROWAVE DTXT(36)     //"%s is vaporized by %s's Microwave beam"
#define DTXT_DEATHPLASMA DTXT(37)        //"%s pulverizes %s with Plasma power"
#define DTXT_DEATHFUSION DTXT(38)        //"%s disintigrates %s's hull with the fusion"
#define DTXT_DEATHSUPERLASER DTXT(39)    //"%s's Super Laser blasts %s to smithereens"
#define DTXT_DEATHMASS DTXT(40)          //"%s targets %s for Mass destruction"
#define DTXT_DEATHNAPALM DTXT(41)        //"%s's Napalm burns %s beyond recognition"
#define DTXT_DEATHEMD DTXT(42)           //"%s's EMD electrocutes %s!"
#define DTXT_DEATHOMEGA DTXT(43)         //"%s's Omega peels off %s's shields"
#define DTXT_DEATHCONC DTXT(44)          //%s takes a pounding from %s's Concussion"
#define DTXT_DEATHHOMING DTXT(45)        //"%s's Homer swoops down on %s for the kill"
#define DTXT_DEATHSMART DTXT(46)         //"%s can't shake %s's Smart missile"
#define DTXT_DEATHMEGA DTXT(47)          //"%s's Mega reduces %s to smoldering scrap"
#define DTXT_DEATHFRAG DTXT(48)          //"%s catches shrapnel from %s's Frag"
#define DTXT_DEATHGUIDED DTXT(49)        //"%s's Guided hunts down %s"
#define DTXT_DEATHNAPALMROCKET DTXT(50)  //"%s is incinerated by %s's Napalm Rocket"
#define DTXT_DEATHCYCLONE DTXT(51)       //%s's Cyclone overwhelms %s's defenses"
#define DTXT_DEATHFLARE DTXT(52)         //"%s's Flare ignites %s's fuel leak"
#define DTXT_OSM_REMOVEBAN DTXT(53)      //"Remove Ban"
#define DTXT_OSM_REHASHLIST DTXT(54)     //"Rehash Allow/Deny List"
#define DTXT_OSM_MAXHUDNAME DTXT(55)     //"Max HUD Name Level"
#define DTXT_PLAIN_NONE DTXT(56)         //"None"
#define DTXT_OSM_TEAMONLY DTXT(57)       //"Team Only"
#define DTXT_OSM_FULL DTXT(58)           //"Full"
#define DTXT_OSM_TEAMCONFIG DTXT(59)     //"Show Team Config Dialog"
#define DTXT_OSM_PIGGYBACK DTXT(60)      //"PiggyBack Observer"
#define DTXT_OSM_BACKGROUND DTXT(61)     //"OnScreen Menu Background"
#define DTXT_OSM_HUDNAMELEVEL DTXT(62)   //"HUD Name Level"
#define DTXT_OSM_SHIPLOGOS DTXT(63)      //"Ship Logos"
#define DTXT_OSM_STATSTOFILE DTXT(64)    //"Game Stats To File"
#define DTXT_OSM_STATLEVELEND DTXT(65)   //"AutoSave On Level End"
#define DTXT_OSM_STATDISCONNECT DTXT(66) //"AutoSave On Disconnect"
#define DTXT_OSM_NETGAMEINFO DTXT(67)    //"NetGame Info"
#define DTXT_OSM_HUDFILTER DTXT(68)      //"HUD Message Filters"
#define DTXT_OSM_KILLMESSAGES DTXT(69)   //"Kill Messages"
#define DTXT_OSM_SIMPLE DTXT(70)         //"Simple"
#define DTXT_OSM_STATMESSAGES DTXT(71)   //"Statistical Messages"
#define DTXT_OSM_EXIT DTXT(72)           //"Exit Menu"
#define DTXT_PI_TIMEINGAME DTXT(73)      //"Total Time In Game: %s\n"
#define DTXT_SETTEAMNAME DTXT(74)        //"%s changed team name to %s"
#define DTXT_HUDLEVEL_FULL DTXT(75)      //"Personal HUD Name Level: Full"
#define DTXT_HUDLEVEL_TEAM DTXT(76)      //"Personal HUD Name Level: Team Only"
#define DTXT_HUDLEVEL_NONE DTXT(77)      //"Personal HUD Name Level: None"
#define DTXT_SHUDLEVEL_FULL DTXT(78)     //"Server Max HUD Name Level: Full"
#define DTXT_SHUDLEVEL_TEAM DTXT(79)     //"Server Max HUD Name Level: Team Only"
#define DTXT_SHUDLEVEL_NONE DTXT(80)     //"Server Max HUD Name Level: None"
#define DTXT_DMFC_STAT_HEADER DTXT(81)   //"Callsign:                     Kills:    Deaths:"
#define DTXT_DEDS_BAN DTXT(82)           //"Banning %s\n"
#define DTXT_DEDS_BAN_ERROR DTXT(83)     //"Unable to ban player...pnum not valid\n"
#define DTXT_TIME_HOUR DTXT(84)          //"01:00:00 hour"
#define DTXT_TIME_HOURS DTXT(85)         //"%d:%.2d:%.2d hours"
#define DTXT_TIME_MINUTE DTXT(86)        //"1:00 minute"
#define DTXT_TIME_MINUTES DTXT(87)       //"%d:%.2d minutes"
#define DTXT_TIME_SECOND DTXT(88)        //"1 second"
#define DTXT_TIME_SECONDS DTXT(89)       //"%d seconds"
#define DTXT_IC_ALLOWTEAMCHANGE DTXT(90) //"allowteamchange"
#define DTXT_IC_ALLOWTEAMCHANGED                                                                                       \
  DTXT(91) //"[Server Only]\nTurns off/on allowing clients to change their team.\nUsage: \"$allowteamchange <off/on>\""
#define DTXT_IC_AUTOBALANCE DTXT(92) //"autobalance"
#define DTXT_IC_AUTOBALANCED                                                                                           \
  DTXT(93) //"[Server Only]\nTurns off/on allowing the automatic team placement of new players by the server.\nUsage:
           //\"$autobalance <off/on>\""
#define DTXT_IC_AUTOSAVEDISC DTXT(94) //"autosavedisconnect"
#define DTXT_IC_AUTOSAVEDISCD                                                                                          \
  DTXT(95) //"Enables/Disables the automatic saving of the game stats if you disconnect from the server.\nUsage:
           //\"$autosavedisconnect <on/off>\""
#define DTXT_IC_AUTOSAVELEVEL DTXT(96) //"autosavelevel"
#define DTXT_IC_AUTOSAVELEVELD                                                                                         \
  DTXT(97) //"Enables/Disables the automatic saving of the game stats when the level ends.\nUsage: \"$autosavelevel
           //<on/off>\""
#define DTXT_IC_BALANCE DTXT(98) //"balance"
#define DTXT_IC_BALANCED                                                                                               \
  DTXT(99) //"[Server Only]\nAutomatically balances the teams, based on senority.\nUsage: \"$balance\""
#define DTXT_IC_BAN DTXT(100)     //"ban"
#define DTXT_IC_BAND DTXT(101)    //"[Server Only]\nBans a player from the game.\nUsage: \"$ban <pnum>\""
#define DTXT_IC_BANLIST DTXT(102) //"banlist"
#define DTXT_IC_BANLISTD                                                                                               \
  DTXT(103) //"[Dedicated Server Only]\nLists the players banned from the game along with their ban number, which can be
            // used to remove the ban.\nUsage: \"$banlist\""
#define DTXT_IC_CHANGETEAM DTXT(104) //"changeteam"
#define DTXT_IC_CHANGETEAMD                                                                                            \
  DTXT(105) //"[Server Only]\nForces a player to a team.\nUsage: \"$changeteam <pnum> <team_name>\""
#define DTXT_IC_ENDLEVEL DTXT(106)  //"endlevel"
#define DTXT_IC_ENDLEVELD DTXT(107) //"[Server Only]\nEnds the level.\nUsage: \"$endlevel\""
#define DTXT_IC_HELP DTXT(108)      //"help"
#define DTXT_IC_HELPD DTXT(109)     //"Displays help information for the input commands.\nUsage: \"$help [command]\""
#define DTXT_IC_HUDNAME DTXT(110)   //"hudnames"
#define DTXT_IC_HUDNAMED                                                                                               \
  DTXT(111) //"Sets your personal level for the HUD name filter.\nUsage: \"$hudnames <full/team/none>\"\nNOTE: You can
            // only set your HUD Callsign level up to the level that the server is.  So if the server is only allowing
            // up to teammates, you won't be able to set to full"
#define DTXT_IC_KICK DTXT(112)          //"kick"
#define DTXT_IC_KICKD DTXT(113)         //"[Server Only]\nKicks a player from the game.\nUsage: \"$kick <pnum>\""
#define DTXT_IC_KILLMSGFILTER DTXT(114) //"killmsgfilter"
#define DTXT_IC_KILLMSGFILTERD                                                                                         \
  DTXT(115) //"Sets the kill message filter, for what style of messages you want.\nUsage: \"$killmsgfilter
            //<full/simple/none>\""
#define DTXT_IC_NETGAMEINFO DTXT(116)  //"netgameinfo"
#define DTXT_IC_NETGAMEINFOD DTXT(117) //"Displays information about the net game.\nUsage: \"$netgameinfo\""
#define DTXT_IC_OBSERVER DTXT(118)     //"observer"
#define DTXT_IC_OBSERVERD                                                                                              \
  DTXT(119) //"If you pass specify 'on', it puts you into observer mode, else it will return you back to normal
            // mode.\nUsage: \"$observer <on/off>\""
#define DTXT_IC_PIGGYBACK DTXT(120)   //"piggyback"
#define DTXT_IC_PIGGYBACKD DTXT(121)  //"Puts you into Piggyback Observer mode.\"$piggyback <pnum>\""
#define DTXT_IC_PLAYERINFO DTXT(122)  //"playerinfo"
#define DTXT_IC_PLAYERINFOD DTXT(123) //"Displays information about a player.\nUsage: \"$playerinfo <pnum>\""
#define DTXT_IC_PLAYERS DTXT(124)     //"players"
#define DTXT_IC_PLAYERSD                                                                                               \
  DTXT(125) //"[Dedicated Server Only]\nDisplays a list of the players in the game, with their player numbers.\nUsage:
            //\"$players\""
#define DTXT_IC_REHASH DTXT(126) //"rehash"
#define DTXT_IC_REHASHD                                                                                                \
  DTXT(127) //"[Server Only]\nRehashes the hosts.allow and hosts.deny files.  First it flushes the old, and reloads
            // them.\nUsages: \"$rehash\""
#define DTXT_IC_REMOVEBAN DTXT(128) //"removeban"
#define DTXT_IC_REMOVEBAND                                                                                             \
  DTXT(129) //"[Dedicated Server Only]\nRemoves a ban from a player, given the number associated with them from
            //$banlist.\nUsage: \"$removeban <player>\""
#define DTXT_IC_SAVESTATS DTXT(130)      //"savestats"
#define DTXT_IC_SAVESTATSD DTXT(131)     //"Saves the game stats to file immediatly.\nUsage: \"$savestats\""
#define DTXT_IC_SCORES DTXT(132)         //"scores"
#define DTXT_IC_SCORESD DTXT(133)        //"Displays the scores or stats of the game.\nUsage: \"$scores\""
#define DTXT_IC_SERVERHUDNAMES DTXT(134) //"serverhudnames"
#define DTXT_IC_SERVERHUDNAMESD                                                                                        \
  DTXT(135) //"[Server Only]\nSets the highest HUD name filter permitted for the clients.\nUsage: \"$serverhudnames
            //<full/team/none>\""
#define DTXT_IC_SETGOALLIMIT DTXT(136) //"setgoallimit"
#define DTXT_IC_SETGOALLIMITD                                                                                          \
  DTXT(137) //"[Server Only]\nChanges the goal limit for the level.\nUsage: \"$setgoallimit <points>\""
#define DTXT_IC_SETMAXPLAYERS DTXT(138) //"setmaxplayers"
#define DTXT_IC_SETMAXPLAYERSD                                                                                         \
  DTXT(139) //"[Server Only]\nSets the maximum number of players allowed in the game.\nUsage: \"$setmaxplayers
            //<count>\""
#define DTXT_IC_SETPPS DTXT(140) //"setpps"
#define DTXT_IC_SETPPSD                                                                                                \
  DTXT(141) //"[Server Only]\nChanges the Packets Per Second (PPS) threshold of the game\nUsage: \"$setpps <pps>\""
#define DTXT_IC_SETRESPAWNTIME DTXT(142) //"setrespawntime"
#define DTXT_IC_SETRESPAWNTIMED                                                                                        \
  DTXT(143) //"[Server Only]\nChanges the respawn time of the powerups in the level.\nUsage: \"$setrespawntime
            //<seconds>\""
#define DTXT_IC_SETTEAMNAME DTXT(144) //"setteamname"
#define DTXT_IC_SETTEAMNAMED                                                                                           \
  DTXT(145) //"[Server Only]\nChanges the name of a team.\nUsage: \"$setteamname <team_num> <new_team_name>\""
#define DTXT_IC_SETTIMELIMIT DTXT(146) //"settimelimit"
#define DTXT_IC_SETTIMELIMITD                                                                                          \
  DTXT(147) //"[Server Only]\nChanges the time limit for the level.\nUsage: \"$settimelimit <minutes>\""
#define DTXT_IC_STATMSGS DTXT(148)    //"statmsgs"
#define DTXT_IC_STATMSGSD DTXT(149)   //"Enables/Disables random statistical messages.\nUsage: \"$statmsgs <on/off>\""
#define DTXT_IC_TEAM DTXT(150)        //"team"
#define DTXT_IC_TEAMD DTXT(151)       //"Change teams for yourself.\nUsage: \"$team <team_name>\""
#define DTXT_IC_HEADEERLIST DTXT(152) //"Input Command List:\n"
#define DTXT_IC_HEADERINSTRUCT                                                                                         \
  DTXT(153) //"Prefix a '$' before the commands listed below.  To get more detailed help\nabout a command, type '$help
            //<command>'\n"
#define DTXT_IC_NOADDITIONALINFO DTXT(154)   //"No additional information is available\n"
#define DTXT_IC_COMMANDNOTFOUND DTXT(155)    //"Command not found\n"
#define DTXT_SUNDAY DTXT(156)                //"Sun"
#define DTXT_MONDAY DTXT(157)                //"Mon"
#define DTXT_TUESDAY DTXT(158)               //"Tue"
#define DTXT_WEDNESDAY DTXT(159)             //"Wed"
#define DTXT_THURSDAY DTXT(160)              //"Thur"
#define DTXT_FRIDAY DTXT(161)                //"Fri"
#define DTXT_SATURDAY DTXT(162)              //"Sat"
#define DTXT_JANUARY DTXT(163)               //"Jan"
#define DTXT_FEBRUARY DTXT(164)              //"Feb"
#define DTXT_MARCH DTXT(165)                 //"March"
#define DTXT_APRIL DTXT(166)                 //"April"
#define DTXT_MAY DTXT(167)                   //"May"
#define DTXT_JUNE DTXT(168)                  //"June"
#define DTXT_JULY DTXT(169)                  //"July"
#define DTXT_AUGUST DTXT(170)                //"Aug"
#define DTXT_SEPTEMBER DTXT(171)             //"Sept"
#define DTXT_OCTOBER DTXT(172)               //"Oct"
#define DTXT_NOVEMBER DTXT(173)              //"Nov"
#define DTXT_DECEMBER DTXT(174)              //"Dec"
#define DTXT_ENDOFLEVELCONCAT DTXT(175)      //"End_Of_Level_"
#define DTXT_NGI_HEADING DTXT(176)           //"NetGame Information\n"
#define DTXT_NGI_GAMENAME DTXT(177)          //"Game Name: %s\n"
#define DTXT_NGI_MISSIONNAME DTXT(178)       //"Mission Name: %s\n"
#define DTXT_NGI_SCRIPTNAME DTXT(179)        //"Script Name: %s\n"
#define DTXT_NGI_PPS DTXT(180)               //"PPS: %d\n"
#define DTXT_NGI_PPSSERVER DTXT(181)         //"PPS (Server/You): %d/%d\n"
#define DTXT_NGI_MAXPLAYERS DTXT(182)        //"Max Players: %d\n"
#define DTXT_NGI_ACCURATECOLL DTXT(183)      //"Accurate Weapon Collisions: %s\n"
#define DTXT_NGI_SENDROTATIONAL DTXT(184)    //"Send Rotational Velocity: %s\n"
#define DTXT_NGI_TIMELIMIT DTXT(185)         //"Time Limit: %d %s\n"
#define DTXT_MINUTE DTXT(186)                //"minute"
#define DTXT_MINUTES DTXT(187)               //"minutes"
#define DTXT_NGI_TIMELEFT DTXT(188)          //"Time Left: %s\n"
#define DTXT_NGI_NOTIMELIMIT DTXT(189)       //"Time Limit: None"
#define DTXT_NGI_GOALLIMIT DTXT(190)         //"Goal: %d points\n"
#define DTXT_NGI_NOGOAL DTXT(191)            //"Goal: None\n"
#define DTXT_NGI_RESPAWNTIME DTXT(192)       //"Respawn Time: %s\n"
#define DTXT_NGI_PACKETLOSSNA DTXT(193)      //"Packet Loss: N/A\n"
#define DTXT_NGI_PACKETLOSS DTXT(194)        //"Packet Loss: %.1f%%\n"
#define DTXT_NGI_NETWORKMODEL DTXT(195)      //"Network Model: %s\n"
#define DTXT_PEERTOPEER DTXT(196)            //"Peer-To-Peer"
#define DTXT_CLIENTSERVER DTXT(197)          //"Client/Server"
#define DTXT_NGI_MOVEMENTSMOOTHING DTXT(198) //"Movement Smoothing: %s\n"
#define DTXT_CHANGETEAMDENIED DTXT(199)      //"Team Change Was Denied"
#define DTXT_KILLMSGFULL DTXT(200)           //"Kill Message Filter: Full"
#define DTXT_KILLMSGSIMPLE DTXT(201)         //"Kill Message Filter: Simple"
#define DTXT_KILLMSGNONE DTXT(202)           //"Kill Message Filter: None"
#define DTXT_WAITFORSERVER DTXT(203)         //"Waiting For Server..."
#define DTXT_PRESSPRNTSCRN DTXT(204)         //"Press PrintScreen For A ScreenShot"
#define DTXT_CHANGETEAM DTXT(205)            //"%s changes teams to the %s team\n"
#define DTXT_NOKILLERDEATHMSG DTXT(206)      //"%s was killed"
#define DTXT_SIMPLESUICIDEMSG DTXT(207)      //"%s went suicidal"
#define DTXT_HM_KILLSINAROW DTXT(208)        //"That's %d kills in a row for %s!"
#define DTXT_HM_DEATHSINAROW DTXT(209)       //"That's %d deaths in a row for %s!"
#define DTXT_HM_TIMESINCELASTKILL DTXT(210)  //"It's %s's first kill in %s"
#define DTXT_HM_TIMESINCELASTDEATH DTXT(211) //"%s lasted %s without being killed!"
#define DTXT_HM_HIGHEFFICENCY DTXT(212)      //"%s has an awesome efficiency of %.2f!!"
#define DTXT_HM_GOODEFFICENCY DTXT(213)      //"%s has an efficiency of %.2f"
#define DTXT_HM_TIMESKILLED DTXT(214)        //"%s has killed %s %d times!"
#define DTXT_HM_REVENGE DTXT(215)            //"%s got revenge on %s!"
#define DTXT_TEAMCHANGEATTEMPT DTXT(216)     //"Attempting to change teams to %s team"
#define DTXT_STEAMCHANGEATTEMPT DTXT(217)    //"Attempting to change %s to %s team"
#define DTXT_GETPLAYERINFO DTXT(218)         //"Getting Playerinfo for %s"
#define DTXT_BALANCETEAMMSG DTXT(219)        //"Balancing Teams"
#define DTXT_AUTOTEAMBALANCEMSG DTXT(220)    //"Auto Team Balance: %s"
#define DTXT_ALLOWTEAMCHANGEMSG DTXT(221)    //"Allow Team Changing: %s"
#define DTXT_KICKMSG DTXT(222)               //"Kicking %s from game"
#define DTXT_BANMSG DTXT(223)                //"Banning %s from game"
#define DTXT_ENDLEVELMSG DTXT(224)           //"Ending the level"
#define DTXT_STATSMSGSET DTXT(225)           //"Statistical Messages: %s"
#define DTXT_AUTOSAVELEVELMSG DTXT(226)      //"AutoSave Stats on Level End: %s"
#define DTXT_AUTOSAVEDISCMSG DTXT(227)       //"AutoSave Stats on Disconnect: %s"
#define DTXT_TIMELIMITSETMSG DTXT(228)       //"Time Limit: %d"
#define DTXT_TIMELIMITOFFMSG DTXT(229)       //"Time Limit: Off"
#define DTXT_GOALLIMITSETMSG DTXT(230)       //"Goal Limit: %d"
#define DTXT_GOALLIMITOFFMSG DTXT(231)       //"Goal Limit: None"
#define DTXT_MAXPLAYERSSETMSG DTXT(232)      //"Max Players: %d"
#define DTXT_RESPAWNTIMEMSG DTXT(233)        //"Respawn Time: %d"
#define DTXT_PPSSETMSG DTXT(234)             //"Max PPS: %d"
#define DTXT_BANREMOVEDMSG DTXT(235)         //"Ban Removed\n"
#define DTXT_BANNOTREMOVEDMSG DTXT(236)      //"Couldn't remove ban\n"
#define DTXT_NOTINDEDICATED DTXT(237)        //"Command Not Available To Dedicated Server\n"
#define DTXT_OBSERVERDENIED DTXT(238)        //"Observer mode request denied"
#define DTXT_INVALIDPNUM DTXT(239)           //"Invalid Player Number"
#define DTXT_PLAYERLISTHEADER DTXT(240)      //"PNum Name\n"
#define DTXT_INVALIDTEAM DTXT(241)           //"Invalid Team, Must be between 0 and %d"
#define DTXT_SERVERCANTKICK DTXT(242)        //"Server can't kick themself\n"
#define DTXT_SERVERCANTBAN DTXT(243)         //"Server can't ban themself\n"
#define DTXT_BALANCINGTEAMS DTXT(244)        //"Balancing Teams\n"
#define DTXT_NODEDICATEDTEAM DTXT(245)       //"Dedicated Server Can't Change Teams\n"
#define DTXT_SAMETEAMCHANGE DTXT(246)        //"You're already on the %s team\n"
#define DTXT_SHIPLOGOSMSG DTXT(247)          //"Ship Logos: %s"
#define DTXT_ENABLED DTXT(248)               //"Enabled"
#define DTXT_DISABLED DTXT(249)              //"Disabled"
#define DTXT_REHASHINGMSG DTXT(250)          //"Rehashing Hosts.allow and Hosts.deny"
#define DTXT_STAT_STATUS DTXT(251)           //"Status:"
#define DTXT_STAT_TEAM DTXT(252)             //"Team:"
#define DTXT_STAT_PLAYTIME DTXT(253)         //"Play Time:"
#define DTXT_STAT_STREAK DTXT(254)           //"Streak:"
#define DTXT_STAT_LASTVICTIM DTXT(255)       //"Last Victim:"
#define DTXT_STAT_LASTKILLER DTXT(256)       //"Last Killer:"
#define DTXT_STAT_SERVER DTXT(257)           //"Server"
#define DTXT_STAT_OBSERVER DTXT(258)         //"Observing"
#define DTXT_STAT_PLAYING DTXT(259)          //"Playing"
#define DTXT_STAT_DISCONNECTED DTXT(260)     //"Disconnected"
#define DTXT_STAT_KILL DTXT(261)             //"Kill"
#define DTXT_STAT_KILLS DTXT(262)            //"Kills"
#define DTXT_STAT_DEATH DTXT(263)            //"Death"
#define DTXT_STAT_DEATHS DTXT(264)           //"Deaths"
#define DTXT_STAT_PILOTNAMELABEL DTXT(265)   //"Pilot Name"
#define DTXT_UI_STARTGAME DTXT(266)          //"Start Game"
#define DTXT_UI_RETURNTOGAME DTXT(267)       //"Return to Game"
#define DTXT_UI_CLIENTSWAIT DTXT(268)        //"Make Clients Wait"
#define DTXT_UI_TEAMSETUP DTXT(269)          //"Team Setup"
#define DTXT_UI_WAITFORPLAYERS DTXT(270)     //"Waiting For Players"
#define DTXT_UI_KICKPLAYER DTXT(271)         //"Kick Player"
#define DTXT_UI_BANPLAYER DTXT(272)          //"Ban Player"
#define DTXT_OSM_AUDIOTAUNTS DTXT(273)       //"Audio Taunts"
#define DTXT_AUDIOTAUNTSMSG DTXT(274)        //"Audio Taunts: %s"
#define DTXT_RANK DTXT(275)                  //"Rank:"
#define DTXT_NETWORK_LOSS DTXT(276)          //"Network Loss"
#define DTXT_NETWORK_PING DTXT(277)          //"Network Ping"
#define DTXT_LOSSPINGIND DTXT(278)           //"Loss/Ping HUD Indicator"
#define DTXT_LOSSINDICATOR DTXT(279)         //"Loss/Ping Indicator: %s"
#define DTXT_PERMISSABLE DTXT(280)           //"Permissable C/S"
#define DTXT_IC_WARP DTXT(281)               //"warp"
#define DTXT_IC_WARPD                                                                                                  \
  DTXT(282) //"[Server Only]\nChanges the current level to another level in the mission.\nUsage: \"$warp <level>\""
//---------------------
#define DTXT_REMOTELOGGEDOUT DTXT(283)   //"Remote Admin Logged Out"
#define DTXT_HUDMSGLOGGEDOUT DTXT(284)   //"==%s has logged out=="
#define DTXT_HUDMSGLOGGEDIN DTXT(285)    //"==%s is remote administrating=="
#define DTXT_REMOTELOGGEDIN DTXT(286)    //"Remote Admin Logged In"
#define DTXT_REMOTEEXECUTE DTXT(287)     //"==%s executed "%s""
#define DTXT_REMOTESUCCESS DTXT(288)     //"Remote Command Successful"
#define DTXT_REMOTENOTSUCCESS DTXT(289)  //"Remote Command Not Successful"
#define DTXT_REMOTENOTALLOWED DTXT(290)  //"Remote Command Not Allowed"
#define DTXT_REMOTENOTFOUND DTXT(291)    //"Remote Command Not Found"
#define DTXT_UNABLETOREMOTE DTXT(292)    //"Unable to enable remote administration (no password?)"
#define DTXT_REMOTEADMINENABLE DTXT(293) //"Remote Administration: %s"
#define DTXT_REMOTEADMINPASS DTXT(294)   //"Remote Administration Password Set"
#define DTXT_WAITSECS DTXT(295)          //"Making Clients Wait %.2f seconds\n"
#define DTXT_WAITOFF DTXT(296)           //"Turning Off Client Wait Time\n"
#define DTXT_WAITON DTXT(297)            //"Making Clients Wait\n"
#define DTXT_WAITNOLONGER DTXT(298)      //"No Longer Making Clients Wait\n"
#define DTXT_TEAMLABEL DTXT(299)         //"%s Team:"
#define DTXT_IC_REMOTE DTXT(300)         //"remote"
#define DTXT_IC_REMOTED                                                                                                \
  DTXT(301) //"[Client Only]\nhandles a remote admin command\nUsage: \"$remote <command> <option parms> <...>\""
#define DTXT_IC_REMOTEADMIN DTXT(302) //"remoteadmin"
#define DTXT_IC_REMOTEADMIND                                                                                           \
  DTXT(303) //"[Server Only]\nhandles enable/disable remote administration\nUsage: \"$remoteadmin <on/off>\""
#define DTXT_IC_REMOTEADMINLOGOUT DTXT(304) //"remoteadminlogout"
#define DTXT_IC_REMOTEADMINLOGOUTD                                                                                     \
  DTXT(305) //"[Server Only]\nhandles seeing who is logged in, and allows the server to log them out\nIf no parameter is
            // given it lists all the players logged in.\nTo log out a player give the login-id as a parameter\nUsage:
            //\"$remoteadminlogout [login-id]\""
#define DTXT_IC_REMOTEADMINPASS DTXT(306) //"remoteadminpass"
#define DTXT_IC_REMOTEADMINPASSD                                                                                       \
  DTXT(307) //"[Server Only]\nhandles setting/changing the remote administration password\nUsage: \"$remoteadminpass
            //<password>\""
#define DTXT_IC_WAIT DTXT(308) //"wait"
#define DTXT_IC_WAITD                                                                                                  \
  DTXT(309) //"[Dedicated Server Only]\nhandles a request to make all clients wait/or stop waiting.  If a time is
            // giving, the server will wait that long each level until it lets clients to play.\nUsage: \"$wait <on/off
            // or time-in-seconds>\""

#define GAME_DLL_INTERFACE 9

// DMFC Special Packet definer
#define SPID_TEAMASSIGNMENT 0xE0
#define SPID_CHANGETEAM 0xE1
#define SPID_REQGAMESTATE 0xE2
#define SPID_DMFCGAMEINFO 0xE3
#define SPID_VERSIONID 0xE4
#define SPID_PRECORD 0xE5
#define SPID_PRECORDREQ 0xE6
#define SPID_CONTROLMSG 0xE7
#define SPID_NETGAMESYNC 0xE8
#define SPID_NEWTEAMNAME 0xE9
#define SPID_REMOTETOSERVER 0xEA
#define SPID_REMOTETOCLIENT 0xEB
#define SPID_SERVERREMOTEMSG 0xEC
#define SPID_REMOTEKEY 0xED
#define SPID_REALGAMETIME 0xEE

#define HUD_CALLSIGN_LEVEL_FULL 2
#define HUD_CALLSIGN_LEVEL_TEAM 1
#define HUD_CALLSIGN_LEVEL_NONE 0

#define DM_FILTER_FULL 2
#define DM_FILTER_SIMPLE 1
#define DM_FILTER_NONE 0

#define UIID_TEAMPLACEMENT 0xEEE0
#define UIID_WAITFORPLAYERS 0xEEE1

// Uncomment the following define to put a black background on the in-game menus
// #define DMFC_MENUBACKGROUND

#define PINFO_CLEAR -2
#define PINFO_INUSE -1

#define MAX_STATE_SUBMENUS 64 // maximum number of state menu items a MIT_STATE menu can have

int GetBanPlayerList(void);
char *GetBannedPlayerString(int index);

// Menu Function handlers
void KickPlayer(int pnum);
void EndMultiLevel(int i);
void BanPlayer(int pnum);
// Removes a ban for a player, give the ban index #
void RemoveBanByIndex(int index);
void BalanceTeams(int i);
void SwitchAutoTeamSelect(int i);
void SwitchPlayerInfo(int pnum);
void ChangeTeams(int team);
void SwitchTeamChange(int i);
void SwitchHudPlayerNum(int i);
void SaveStatsToFile(int i);
void SwitchStatHUDMessages(int i);
// Turns on/off Observer Mode
void SwitchObserverMode(int i);
// Turns on/off PiggyBack mode
void SwitchPiggyBack(int pnum);
// Exits the on-screen menu
void MenuExitMenu(int i);
//	Switches on/off auto saving of the stats to file on level end
void SwitchSaveStatsLevelEnd(int i);
//	Switches on/off auto saving of the stats to file on disconnect from the game
void SwitchSaveStatsDisconnect(int i);
//	Switches on/off displaying the background
void SwitchMenuBackground(int i);
//	Switches on/off displaying the netgame info
void SwitchNetGameInfo(int i);
// Switch the server's Max HUD Callsign level
void SwitchServerHudPlayerName(int i);
// Switches the death message filter
void SwitchDeathMessageFilter(int i);
// brings up the team config dialog for the server
void OnScreenDisplayTeamConfig(int i);
// Switches turning on/off displaying ship logos
void SwitchShipLogoEnable(int i);
// Switches turning on/off audio taunts
void SwitchAudioTauntsEnable(int i);
// Change the pps for a client
void DecreasePacketsPerSecond(int i);
void IncreasePacketsPerSecond(int i);
// Rehashs the hosts.allow/deny lists
void RehashAllowDenyLists(int i);
void SwitchLossPingIndicator(int i);

/*
****************************************
  Remote administration
****************************************
*/
// Initializes the remote administration system
void Remote_Initialize(void);
// Processes remote administration for the frame
void Remote_ProcessFrame(void);
// Gets the key for the given player record
uint8_t *Remote_GetKey(int prec);
// Enables/Disables remote administration
void Remote_Enable(bool enable);
// sets the administration password for the server
// pass in an unencrypted password
void Remote_SetPassword(char *password);
// Sets a clients key
void Remote_SetMyKey(uint8_t key[8]);
// handles a remote command (client side)
void Remote_ClientProcess(const char *command);
// Lists the players logged in
void Remote_ListLogins(void);
// Logs a player out from being an administrator
void Remote_Logout(int precnum);

#define MAX_RECORD_NAME 256

struct tRecord {
  char name[MAX_RECORD_NAME];
  char type;
  void *data;
  tRecord *next;
};

struct tKey {
  char name[MAX_RECORD_NAME];
  tKey *next;
  tRecord *records;
};

class CRegistry {
public:
  CRegistry(const char *name);
  ~CRegistry();
  void Export();
  bool Import();
  void CreateKey(const char *name);
  bool LookupKey(const char *name);
  bool CreateRecord(const char *name, char type, void *data);
  tRecord *LookupRecord(const char *record, void *data);
  int GetDataSize(const char *record);

private:
  void Destroy(void);
  void DestroyKey(tKey *key);
  void DestroyRecord(tRecord *record);
  void ExportKey(tKey *key, CFILE *file);
  void ExportRecord(tRecord *record, CFILE *file);
  char *SkipWhite(char *p);
  char *ParseString(char *p, char *buf, int bufsize, char sdelim, char edelim);
  char *ParseToken(char *p, char *buf, int bufsize);
  char name[MAX_RECORD_NAME];
  tKey *root;
  tKey *currentkey;
};

//	Initializes the Player records
void PRec_Init(void);
//	Frees up and closes the Player records
void PRec_Close(void);
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
int PRec_SetupUserPRec(int sizeof_user_stats, int (*pack_callback)(void *user_info, uint8_t *data),
                       int (*unpack_callback)(void *user_info, uint8_t *data));
//	Given a pnum it will return the PRec slot of that player, or -1 if it's not a valid player
int PRec_GetPlayerSlot(int pnum);
//	Given a PRec slot, it returns a pointer to the user stats struct, NULL if error, or PRec_SetupUserPRec
//	hasn't been called yet
void *PRec_GetUserStats(int slot);
//	Returns a pointer to a player_record for the requested slot, NULL if error
player_record *PRec_GetPRecord(int slot);

//	Sets a disconnect time for a player
void PRec_SetDisconnectTime(int slot, float time);

//	Returns the PRec slot of a player who matches the given info, -1 if none
//	addr = network address			(can be NULL)
//	tracker_id = Mastertracker ID  (can be NULL)
int PRec_FindPlayer(const char *callsign, network_address *addr, const char *tracker_id);

//	Gets the first available free slot (STATE_EMPTY), if none are available it clears and returns
//	the oldest disconnected player (STATE_DISCONNECTED)
int PRec_GetFreeSlot(void);
//	Assigns a player to a specified slot (fills in the info, and prepares the slot for action)
//	Do Not use this for a reconnecting player
bool PRec_AssignPlayerToSlot(int pnum, int slot, player *players_array, netplayer *netplayers_array);
//	Reconnects a player to a STATE_DISCONNECTED slot
bool PRec_ReconnectPlayerToSlot(int pnum, int slot, player *players_array, netplayer *netplayers_array);
//	Disconnects a player from the Player records
bool PRec_DisconnectPlayer(int pnum);
//	Clear all the level volatile (things that reset with the change of levels) things
void PRec_LevelReset(void);
//	Sends the Player records info to the given client
void PRec_SendPRecToPlayer(int pnum);
//	Receives the Player records info from the server
void PRec_ReceivePRecFromServer(uint8_t *data);
//	Sets the team of a player given the pnum
void PRec_SetPlayerTeam(int pnum, int team);
//	Gets the team of a player given the pnum
int PRec_GetPlayerTeam(int pnum);
//	Returns a pointer to a player_record of a player, NULL if error
player_record *PRec_GetPRecordByPnum(int pnum);
int translate_precptr_to_index(player_record *pr);

#endif
