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

 * $Logfile: /DescentIII/main/gamesequence.h $
 * $Revision: 24 $
 * $Date: 4/08/99 3:12p $
 * $Author: Matt $
 *
 * Game Sequencer
 *
 * $Log: /DescentIII/main/gamesequence.h $
 *
 * 24    4/08/99 3:12p Matt
 * Finished cleaning up level sequencing code.  Got rid of all the "level
 * minus one" stuff.
 *
 * 23    4/07/99 12:30p Matt
 * Added code for failed missions.
 *
 * 22    4/04/99 8:15p Jeff
 * added debug graph stuff
 *
 * 21    2/12/99 3:54a Jeff
 * added function to restart a level, and a cheat key to do so (del-alt-e)
 *
 * 20    1/31/99 3:44p Matt
 * Streamlined game sequencing
 *
 * 19    1/15/99 7:16p Kevin
 * Added GameGauge Configuration & code
 *
 * 18    12/15/98 4:28p Jeff
 * added mission data information to the pilot files to save what the
 * highest level they achieved on a mission is.  Added level select dialog
 * (not hooked up) and level warp cheat.
 *
 * 17    11/19/98 5:40p Kevin
 * Demo system
 *
 * 16    10/08/98 7:29p Samir
 * revamped sequencing.
 *
 * 15    10/08/98 12:00p Kevin
 * Demo system work
 *
 * 14    10/05/98 12:09p Kevin
 * Converted projects to VC6 and demo file stuff added
 *
 * 13    8/18/98 3:02p Samir
 * new sequencing for LoadGameState and PLEASE define Current_level some
 * other way (or at least keep the definition in StartNewLebvel)
 *
 * 12    8/18/98 1:05a Samir
 * added save and load interfaces.
 *
 * 11    8/16/98 5:56p Jeff
 * added game interface for multiplayer dll ui stuff
 *
 * 10    6/25/98 12:43p Jeff
 * Added exit game confirmation
 *
 * 9     5/18/98 4:36p Jeff
 * TelCom uses Game_interface_mode variable now
 *
 * 8     4/03/98 3:36p Chris
 * Added the start of the buddy menu
 *
 * 7     3/13/98 12:08p Samir
 * Added SimpleStartLevel.
 *
 * 6     3/05/98 2:51p Jeff
 * Added Help interface
 *
 * 5     3/03/98 12:10p Samir
 * Added simple pausing.
 *
 * 4     3/02/98 5:52p Samir
 * Added game interface system.
 *
 * 3     2/12/98 5:08p Matt
 * Reset cockpit mode when starting a level.  Unfortunately, this involved
 * some semi-major mucking with game sequencing.
 *
 * 2     2/08/98 5:01p Samir
 * New game sequencing changes.
 *
 * 1     2/08/98 3:54p Samir
 * Initial revision.
 *
 * $NoKeywords: $
 */

#ifndef GAMESEQUENCE_H
#define GAMESEQUENCE_H

//	gamesequencer states:
//		game states

enum tGameState {
  GAMESTATE_IDLE,         // no state
  GAMESTATE_NEW,          // starts a new game (with current mission.)
  GAMESTATE_LVLNEXT,      // go to the next level
  GAMESTATE_LVLSTART,     // happens when a level starts, game time starts here.
  GAMESTATE_LVLPLAYING,   // calls gameloop, polls for input.
  GAMESTATE_LOADGAME,     // a load game request has been issued.
  GAMESTATE_LVLEND,       // calls endlevel and any other endlevel stuff.
  GAMESTATE_LVLFAILED,    // a level was unsuccessfully ended
  GAMESTATE_LVLWARP,      // warp to a new level
  GAMESTATE_LOADDEMO,     // Load whatever demo was chosen in the UI
};

//	top level interfaces for game.
#define GAME_INTERFACE 0
#define GAME_OPTIONS_INTERFACE 1
#define GAME_PAUSE_INTERFACE 2
#define GAME_HELP_INTERFACE 3
#define GAME_BUDDY_INTERFACE 4
#define GAME_TELCOM_BRIEFINGS 5
#define GAME_TELCOM_AUTOMAP 6
#define GAME_TELCOM_CARGO 7
#define GAME_EXIT_CONFIRM 8
#define GAME_DLL_INTERFACE 9
#define GAME_SAVE_INTERFACE 10
#define GAME_LOAD_INTERFACE 11
#define GAME_TOGGLE_DEMO 12
#define GAME_POST_DEMO 13
#define GAME_DEMO_LOOP 14
#define GAME_LEVEL_WARP 15
#define GAME_DEBUGGRAPH_INTERFACE 16

//	variables
extern tGameState Game_state;
extern int Game_interface_mode; // current interface mode of game (menu, game?)
extern int paged_in_count;
extern int paged_in_num;

//	functions
//	main sequencing code for game.  run this to execute a game.
//		before calling this function make sure that a
//			mission has been loaded/initialized.
bool GameSequencer();

// Sets the current level for subsequent level loads, movies, or briefings
void SetCurrentLevel(int level);

//	loads and starts the specified leve. starts a new level (usually called internally)
bool LoadAndStartCurrentLevel();

//	creates a simple mission to play one level.
bool SimpleStartLevel(char *level_name);

//	sets and retrieves the current gamestate
inline void SetGameState(tGameState state) { Game_state = state; }

inline tGameState GetGameState() { return Game_state; }

#endif
