/*
 * $Logfile: /DescentIII/main/init.h $
 * $Revision: 8 $
 * $Date: 5/12/99 1:57p $
 * $Author: Jason $
 *
 * Initialization code
 *
 * $Log: /DescentIII/main/init.h $
 *
 * 8     5/12/99 1:57p Jason
 * fixed yet more buggy/ugly code
 *
 * 7     4/02/99 5:08p Matt
 * Added intro movie.
 *
 * 6     10/14/98 4:37p Matt
 * Made InitD3System() exit with error if there's a problem instead of
 * returning a status value.  Also moved some editor-specific code from
 * init.cpp to mainfrm.cpp, and cleaned up some other initialization and
 * error-handling code.
 *
 * 5     9/25/98 2:53p Jason
 * added progress bar
 *
 * 4     7/01/98 4:56p Samir
 * redid init code.
 *
 * 3     3/23/98 8:03p Samir
 * A bunch of changes to allow for ALT-TAB to work.
 *
 * 2     7/24/97 3:04p Matt
 * Added functions to load/save game variables from/to the registry
 *
 * 1     6/23/97 9:25p Samir
 * added because source safe sucks
 *
 * 8     6/11/97 1:07p Samir
 * The removal of gameos and replaced with oeApplication, oeDatabase
 *
 * 7     2/04/97 2:32p Samir
 * Added separate editor initialization
 *
 * $NoKeywords: $
 */

#ifndef INIT_H
#define INIT_H

//	Universal globals for game
//	---------------------------------------------------------------------------

//	name of ddvideo subsystem to use for game.
extern char App_ddvid_subsystem[];

extern int ServerTimeout;
extern float LastPacketReceived;

//	---------------------------------------------------------------------------

// Initializes all the subsystems that need to be initialized BEFORE application creation.
void PreInitD3Systems();

// Initializes all the subsystems that D3/Editor needs to run.
// Returns 1 if all is good, 0 if something is wrong
void InitD3Systems1(bool editor = false);
void InitD3Systems2(bool editor = false);

// Show the intro screen, or something
void IntroScreen();

// Save game variables to the registry
void SaveGameSettings();

//	This function shutdowns all game systems
void ShutdownD3();

//	This function restarts all game systems
void RestartD3();

void InitMessage(char *c, float progress = -1);

#endif
