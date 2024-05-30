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

#ifndef __TELCOM_H_
#define __TELCOM_H_

#include "hotspotmap.h"
#include "gamefont.h"
#include "bitmap.h"

// TelCom Briefing System Header File

// TelCom Function Prototypes
// these are the functions that can/should be called outside of TelCom*.cpp

// inits the Telcom system...needs to be called before anything else, only needs to be called once
void TelComInit(void);

// handles input processing, etc
void TelComDefer(void);

//	Pages in all the necessary bitmaps, textures & sounds for the TelCom so that they are in memory
void TelcomPageAllIn(void);

// Cleanup crew, don't worry about calling this guy, it's an atexit() in the init
void TelComFree();

// actually displays the Telcom system, if you specify a system it will display showing that system,
// else it will display the last used system
// returns true on success

bool TelComShow(bool ingame = true, bool ShipSelect = false);
bool TelComShow(int system, bool ingame = true, bool ShipSelect = false);

// deactivates the TelCom system if it is running
void TelComDeactivate(void);

#define HOTSPOT_DISPLAY "TelComon.HSM" // string name for the HotSpot map

#define TELCOM_DISPLAY_TGA "TelComOff.TGA"   // string name for the TGA file that needs to be converted
#define TELCOM_DISPLAY_OGF "TelComOff.OGF"   // converted OGF filename
#define TELCOM_DISPLAY_OGF_ON "TelComOn.OGF" // TelCom in the on state file
#define BACKGROUND_COLOR GR_RGB(49, 29, 150)

/*
$$TABLE_GAMEFILE "TelComon.HSM"
$$TABLE_GAMEFILE "TelComOff.OGF"
$$TABLE_GAMEFILE "TelComOn.OGF"
*/

extern chunked_bitmap Telcom_bitmap;
extern int TC_cursor;
extern float last_frametime;

/*
//	Note for above: In TelCom.h there is a section of defines which give names to the above files
//		for use when calling a function like DisplayStillScreen()

*/
// NOTE: The following #defines MUST match up with Sean's alpha values
// TelCom Systems

#define TS_POWER 1
#define TS_OFF -3 // The TelCom is powered on...but nothing selected

#define POWER_BUTTON 1
#define NEON_LIGHT 2

#define NUMBER_OF_SYSTEMS 5 // Total number of systems listed below
#define TS_MAINMENU TS_OFF
#define TS_MISSION 1    // Mission Objective system
#define TS_CARGO 2      // Inventory system
#define TS_MAP 3        // Automap system
#define TS_SHIPSELECT 4 // SinglePlayer Ship Selection
#define TS_GOALS 5      // Goal status

// Windows (=alpha value-MAX_HOTSPOTS) used internally
#define MAX_TELCOM_SCREENS 20
#define MAX_TELCOM_EVENTS 10
#define DUMMY_SCREEN MAX_TELCOM_SCREENS - 1

// Monitors
#define MONITOR_MAIN 0
#define MONITOR_TOP 1
#define MAX_MONITOR 2

// Telcom keys
#define TCSYS_TAB 0
#define TCSYS_UP 1
#define TCSYS_DOWN 2
#define TCSYS_LEFT 3
#define TCSYS_RIGHT 4
#define TCSYS_ENTER 5
#define TCSYS_SPACE 6
#define TCSYS_Q 7
#define TCSYS_F1 8
#define TCSYS_F2 9
#define TCSYS_F3 10
#define TCSYS_PRINTSCRN 11
#define TCSYS_1 12
#define TCSYS_2 13
#define TCSYS_3 14
#define TCSYS_4 15
#define TCSYS_5 16
#define TCSYS_6 17
#define TCSYS_7 18
#define TCSYS_8 19
#define TCSYS_REVERSETAB 20

#define TCSYS_MAXKEYS 21
// Screen states
#define SS_EMPTY 0
#define SS_BEING_MADE 1
#define SS_READY 2

// Telcom system states
#define TCS_POWEROFF 0
#define TCS_POWERON 1

// A cleanup function, freeing up memory used for the monitor windows
void FreeViewports();

// Initializes the system buttons for use
void InitSystems();

// This function 'compresses' the on-state background, to save space
void CompressTelComOnImage(int bitmap, chunked_bitmap *array);
// frees up on-state bitmaps
void FreeTelComOnBitmaps(chunked_bitmap *array);

// returns the screen coordinates of a hotspot
int HotSpotW(int hotspot);
int HotSpotH(int hotspot);
int HotSpotL(int hotspot);
int HotSpotR(int hotspot);
int HotSpotT(int hotspot);
int HotSpotB(int hotspot);
/*
void TCStartPowerupSound(void);
void TCStopPowerupSound(void);
void TCStartPowerDownSound(void);
void TCStopPowerDownSound(void);
void TCStartRunningSound(void);
void TCStopRunningSound(void);
void TCStartStaticSound(float amp);
*/
void TCStartBulbSound(void);
void DoCursor(void);

extern hotspotmap_t hotspotmap;
extern windowmap_t windowmap;
extern bool TelCom_running; // Used within TelComMain(), when POWER button is pressed become false

// structure for a bounding box (used when getting a hotspot bounding box)
struct box {
  int top, bottom, left, right;
};

struct tTCEvent {
  int id;
  int parms[2];
};

struct tTelComInfo {
  box Monitor_coords[MAX_MONITOR];
  uint8_t Screen_state[MAX_TELCOM_SCREENS];
  tTCEvent Telcom_event_queue[MAX_TELCOM_EVENTS]; // Event queue for TelCom System
  int TranslateSysKey[TCSYS_MAXKEYS];
  int current_status;
  bool Telcom_systemkey_states[TCSYS_MAXKEYS];
  float Telcom_mouse_downtime;
  uint8_t state;
  int TelComSysKeyEnable;
};
extern tTelComInfo Telcom_system;

// This is the function called by TelCom to choose a ship
void TelComSingleShipSelect(tTelComInfo *tcs);

//**********************************************
//			TelCom Rendering Functions
//**********************************************
//	Initializes the Telcom rendering engine
void TelcomRenderInit(void);
//	Closes down the Telcom rendering engine
void TelcomRenderClose(void);
//	Renders the current screen for 1 frame
void TelcomRenderScreen(bool poweron = true, bool powerup = false, uint8_t power_effect = 1);
//	Sets what screen should be drawn by the Telcom
void TelcomRenderSetScreen(int screen);
//	Sets the callback of the Render.  This will get called after Effects are drawn, before the screen overlays
void TelcomRenderSetCallback(void (*callback)());
//	Creates a placeholder screen that can be used if no screens are available
void TelcomCreateDummyScreen(void);
//	Creates a placeholder screen that can be used for startup
void TelcomCreateStartupScreen(void);
//	Creates a placeholder screen that can be used for shutdown
void TelcomCreateShutdownScreen(void);
//	Enables Static on the Telcom main screen, setting is the probability of static happening
void TelcomEnableStatic(float setting);
//	Disables static on the Telcom main screen
void TelcomDisableStatic(void);
//	Enables glitching on the Telcom main screen
void TelcomEnableGlitch(float setting);
//	Disables glitching on the TelCom
void TelcomDisableGlitch(void);

/***************************************************
 *			TelCom Screen Control
 ***************************************************
 *
 */
//	Initializes the TelCom Screens so they're ready for input
void InitAllScreens(void);
//	Destroys and clears all screens of the TelCom
void DestroyAllScreens(bool remove_dummy = false);
//	Starts a TelCom screen so it can be created
void TelcomStartScreen(int screen);
//	Ends a TelCom screen.  Calling this enables the screen to be displayed
void TelcomEndScreen(void);
//	Advances to the next screen
//	returns true is there is no screen (it's not being made, nor ready)
bool TelcomAdvanceScreen(void);
//	Backs up a screen
//	returns true is there is no screen (it's not being made, nor ready)
bool TelcomBackupScreen(void);
// Jumps to a screen
//	returns true if there is no screen (not being made, nor ready)
bool TelComJumpScreen(int screen);

/***************************************************
 *			TelCom Event Handling
 ***************************************************
 */

// Prototypes
/*
 *	Initializes the TelCom Event manager system
 *		Call this as soon as possible
 *	You can call it whenever the events need to be flushed
 */
void TelComInitEventManager(void);
/*
 *	Calling this sends an event message to the TelCom, adding it to it's event queue
 *
 */
void TelComSendEvent(int event_num, int parm1 = 0, int parm2 = 0);
/*
 *	Call this when handling the TelCom system events, returns true if there is an event waiting, it will then fill
 *	in the passed struct.  You should call this repeatedly until a value of false is returned, signaling no more
 *	events in the queue
 */
bool TelComPopEvent(tTCEvent *evt);

// Similar to the TelCom's main system event queue, these are for use by individual systems
void TelCom_ClearSystemQueue(void);
void TelCom_SendSystemEvent(int event_num, int parm1 = -1, int parm2 = -1);
bool TelCom_PopSystemEvent(tTCEvent *evt);

/*
        Handles a possible custom key event
*/
void TelCom_ProcessCustomKeyEvent(int key_id);
/*
        Removes any custom key event translation masks
*/
void TelCom_ClearCustomKeyEvents(void);
/*
        Adds a custom key event translation mask
*/
void TelCom_AddCustomKeyEvent(int key_id, int event_id);

/*
 *	This should be called once a frame, it will handle all events in the TelCom system and send off any system
 *	events to the effects/buttons
 */
void TelComHandleAllEvents(tTelComInfo *tcs);
/*
 *	Given the key that has been pressed, or being held down, it processes the event depending on what key
 *	it is.
 *
 */
void TelComHandleKeyPress(int key, bool click, int screen_id);
/*
 *	Enables/Disables a TelCom system key, pass in TCSYS_MAXKEYS to enable/disable all keys
 *
 */
void TelComEnableSystemKey(int key, bool enable);

/*
 *	Forces an effect to have focus (if is allowed to have focus)
 *
 */
void TelComSetFocusOnEffect(int efxnum);

/*
 ****************************************************************
 *				TelCom Sound Functions							*
 ****************************************************************
 *
 */

#define TCSND_STARTUP 0
#define TCSND_STATIC 1
#define TCSND_SHUTDOWN 2
#define TCSND_RUNNING 3
#define TCSND_LIGHTBULB 4
#define TCSND_TYPING 5
#define TCSND_CLICK 6
#define TCSND_SOUNDCOUNT 7

//	Initalizes the Telcom sound sytem
void TelcomInitSounds(void);
//	Closes down the Telcom sound system
void TelcomCloseSounds(void);
//	Starts a sound playing (if it isn't already)
void TelcomStartSound(int sid);
//	Stops a sound playing
void TelcomStopSound(int sid);

extern bool Telcom_called_from_game;

#endif
