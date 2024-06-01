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

 * $Logfile: /DescentIII/Main/game.h $
 * $Revision: 55 $
 * $Date: 9/14/01 4:47p $
 * $Author: Matt $
 *
 *	Game management
 *
 * $Log: /DescentIII/Main/game.h $
 *
 * 55    9/14/01 4:47p Matt
 * Cleaned up problems when screen bit depth set to 32.
 *
 * 54    10/14/99 1:08p Chris
 * Added another custom bounce force field texture slot
 *
 * 53    10/13/99 2:19p Chris
 * Added the ability to disable wind for powerups on a level
 *
 * 52    10/13/99 10:41a Chris
 * Added 'special forcefields' that have custom bounce factors
 *
 * 51    10/08/99 4:29p Chris
 * Added the forcefield and glass breaking override options
 *
 * 50    4/15/99 1:38a Jeff
 * changes for linux compile
 *
 * 49    4/14/99 10:33p Jeff
 * fixed missing extern
 *
 * 48    3/19/99 12:54p Jeff
 * base support for requesting the number of teams for a multiplayer game
 *
 * 47    2/21/99 12:28p Matt
 * Added terrain sound system
 *
 * 46    2/16/99 12:00p Samir
 * added new video resolution swtich test.
 *
 * 45    2/10/99 2:41p Chris
 * Added debug info
 *
 * 44    2/09/99 9:59a Chris
 * Massive BOA update  :)  Terrain happy now.  Vis happy now.  Sound happy
 * now.
 *
 * 43    2/08/99 7:50p Jeff
 * made StartFrame/EndFrame stack based...no longer have to worry about
 * preserving current frame settings to restore
 *
 * 42    2/08/99 12:04a Jeff
 * added a function to get the current StartFrame parameters (from last
 * call to StartFrame)
 *
 * 39    1/10/99 10:23p Jeff
 * added initial start/end boss intro function
 *
 * 38    10/22/98 2:58p Chris
 * Difficulty levels are in beta
 *
 * 37    10/15/98 8:55a Matt
 * Took out Endlevel() declaration, since no one should be calling this
 * function except the gamesequence code.
 *
 * 36    10/14/98 7:30p Matt
 * Changed StartEndlevelSeqence to take a time parameter for how long to
 * delay before ending the level.
 *
 * 35    10/14/98 12:44a Matt
 * Cleaned up endlevel sequence system (though it still doesn't work --
 * ChrisP is going to look at the path-following).
 *
 * 34    10/05/98 11:06a Chris
 * Level goals version .9
 *
 * 33    9/01/98 4:11p Samir
 * moved screenshot code from gameloop to game.cpp
 *
 * 32    6/22/98 12:00p Chris
 * Working on sound system to make it in a nice shape.
 *
 * 31    4/14/98 9:18p Samir
 * redid post level results.
 *
 * 30    3/19/98 9:19p Samir
 * removed dependency of hud.h on game.h.
 *
 * 29    3/17/98 2:37p Samir
 * reorg of hud/gauge/cockpit dependencies.
 *
 * 28    3/13/98 6:55p Craig
 *
 * 27    3/11/98 4:58p Chris
 * Changed Show_game_sphere to 3 types
 *
 * 26    3/10/98 5:15p Chris
 * DEL+B shows the bounding-sphere of an object
 *
 * 25    2/14/98 10:48p Jason
 * got preferred rendering working
 *
 * 24    2/13/98 10:57a Samir
 * Changed some gamescript initialization.
 *
 * 23    2/12/98 5:08p Matt
 * Reset cockpit mode when starting a level.  Unfortunately, this involved
 * some semi-major mucking with game sequencing.
 *
 * 22    2/12/98 4:46p Matt
 * Added needed include
 *
 * 21    2/12/98 4:28p Matt
 * Added function to start endlevel in-game "cutscene"
 *
 * 20    2/11/98 4:35p Samir
 * SetGameMode fully implented takes a script.
 *
 * 19    2/10/98 11:28a Samir
 * Added gamemode info support
 *
 * 18    2/09/98 3:20p Matt
 * Added missile camera system
 *
 * 17    2/08/98 5:01p Samir
 * New game sequencing changes.
 *
 * 16    2/02/98 7:35p Samir
 * Call QuickPlayGame to play game using quick mission system.
 *
 * 15    1/21/98 1:11p Jason
 * incremental checkin for multiplayer
 *
 * 14    1/13/98 4:27p Samir
 * Fixed clear screen strangeness.
 *
 * 13    1/05/98 10:51a Samir
 * Changed number of parameters in StartFrame
 *
 * 12    12/29/97 5:46p Samir
 * Added new frame start and end functions.
 *
 * 11    11/10/97 12:35p Samir
 * Added SM_CINEMATIC screen mode, and make sure we don't reinit renderer
 * for hardware rendering between two screen modes that use hardware.
 *
 * 10    10/28/97 6:36p Samir
 * Moved SetCockpitMode to gauges.cpp
 *
 * 9     10/28/97 12:46p Samir
 * Moved SetCockpitMode to new gauges.cpp
 *
 * 8     10/16/97 2:32p Samir
 * Fixed up some timing problems.
 *
 * 7     10/10/97 3:56p Samir
 * Added a cockpit mode selection function.
 *
 * 6     10/02/97 12:36p Samir
 * Redid game sequencing flow.
 *
 * $NoKeywords: $
 */

#ifndef _GAME_H
#define _GAME_H

#include "pserror.h"
#include "renderer.h"
#include "object.h"

//	return 0 if we wan't to return to the menu, or return 1 if everything
//	is okay. starts a new game based off the current mission.
void PlayGame(void);

//	meant for 'instant' action	(usually run from editor, but...)
void QuickPlayGame();

float GetFPS();

// Stop the Frame_time clock
void StopTime(void);

// Restart the Frame_time clock
void StartTime(void);

// Compute how long last frame took
void CalcFrameTime(void);

//	Initialize frame timer
void InitFrameTime(void);

// Sets screen mode
const int SM_NULL = 0, SM_GAME = 1, SM_MENU = 2, SM_CINEMATIC = 3;

void SetScreenMode(int sm, bool force_res_change = false);
int GetScreenMode();

//	ALWAYS CALL THESE TO START AND END RENDERING
void StartFrame(bool clear = true);
void StartFrame(int x, int y, int x2, int y2, bool clear = true, bool push_on_stack = true);
void EndFrame();

// retrives the settings of the last call to StartFrame
// returns false if it's not currently in between a StartFrame/EndFrame block
bool GetFrameParameters(int *x1, int *y1, int *x2, int *y2);

//	call this to set the game mode
void SetGameMode(int mode);

// How long (in seconds) the last frame took
extern float Frametime;

// How long (in seconds) since the game started
extern float Gametime;

//	set this to clear the screen X number of times.
extern int Clear_screen;

// How many frames have been renered.
// NOTE: this is a count of 3d frames, not game frames
extern int FrameCount;

// Vars for game 3D window
extern int Game_window_x, Game_window_y, Game_window_w, Game_window_h;
extern int Max_window_w, Max_window_h;

extern int Game_mode;
extern int Difficulty_level;

extern int sound_override_force_field;
extern int sound_override_glass_breaking;

#define MAX_FORCE_FIELD_BOUNCE_TEXTURES 3
extern int force_field_bounce_texture[MAX_FORCE_FIELD_BOUNCE_TEXTURES];
extern float force_field_bounce_multiplier[MAX_FORCE_FIELD_BOUNCE_TEXTURES];

extern bool Level_powerups_ignore_wind;

extern bool IsCheater;

#ifdef _DEBUG
extern int Game_show_sphere;
extern int Game_show_portal_vis_pnts;
extern int Game_update_attach;
extern int Game_do_walking_sim;
extern int Game_do_vis_sim;
extern int Game_do_flying_sim;
extern int Game_do_ai_movement;
extern int Game_do_ai_vis;
#endif

extern bool UseHardware;
extern renderer_type PreferredRenderer;
// State variables for our renderer
extern rendering_state Render_state;
extern renderer_preferred_state Render_preferred_state;
extern int Render_preferred_bitdepth;

#define GM_SINGLE 1 // Single player game.
// #define GM_SERIAL	2								// You are in serial mode (Jason
// doesn't like this.)
#define GM_NETWORK 4 // You are in network mode
#define GM_MODEM 32  // You are in a modem (serial) game

#define GM_GAME_OVER 128 // Game has been finished

#define GM_NONE 0                        // You are not in any mode, kind of dangerous...
#define GM_NORMAL 1                      // You are in normal play mode, no multiplayer stuff
#define GM_MULTI (GM_NETWORK + GM_MODEM) // You are in some type of multiplayer game

/*
struct trigger;
struct tD3XThread;
struct tD3XProgram;
*/

struct gamemode {
  char scriptname[64];
  int requested_num_teams;
  /*
          tD3XProgram *d3xmod;
          tD3XThread *d3xthread;
          int objmehandle;
          int objithandle;
          trigger *trigme;
          trigger *trigit;
  */
};

// Structure for a terrain sound "band"
struct terrain_sound_band {
  int sound_index;               // the sound to play
  uint8_t low_alt, high_alt;       // top & bottom of range of sound
  float low_volume, high_volume; // volume at top & bottom of range
};

// How many terrain sound bands we have
#define NUM_TERRAIN_SOUND_BANDS 5

// The terrain sound bands for the current level
extern terrain_sound_band Terrain_sound_bands[];

// Clear out all the terrain sound bands
void ClearTerrainSound();

// Starts the sound on the terrain
void StartTerrainSound();

// Missile camera.  If disabled, this is -1.  Otherwise, it's a window number (see SmallViews.h).
extern int Missile_camera_window;

//	contains all relevent information for gamemode pertaining to d3x system.
extern gamemode Gamemode_info;

// Starts the game-engine "cutscene"
// Puts the player in AI mode, sets the view to an external camera, switches to letterbox, & puts the player on a path
// Parameters:	camera - the camera for the view
//					pathnum - the path the player should follow
//					time - if > 0.0, how long the sequence plays before the level ends
void StartEndlevelSequence(object *camera, int pathnum, float time);

inline void ResetGamemode() {
  Gamemode_info.scriptname[0] = 0;
  Gamemode_info.requested_num_teams = 1;
  /*
  Gamemode_info.objmehandle = 0;
  Gamemode_info.objithandle = 0;
  Gamemode_info.trigme = 0;
  Gamemode_info.trigit = 0;
  */
}

void SetGamemodeScript(const char *scrfilename, int num_requested_teams = -1);

// Does a screenshot and tells the bitmap lib to save out the picture as a tga
void DoScreenshot();

#endif
