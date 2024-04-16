/*
 * $Logfile: /DescentIII/main/gamecinematics.h $
 * $Revision: 8 $
 * $Date: 4/28/99 3:32a $
 * $Author: Jeff $
 *
 * In-Game Cinematics system
 *
 * $Log: /DescentIII/main/gamecinematics.h $
 *
 * 8     4/28/99 3:32a Jeff
 * created cinematic level reset function
 *
 * 7     4/27/99 8:31p Jason
 * fixed cinematic bug
 *
 * 6     3/10/99 6:20p Jeff
 * many fixes to demo system.  Fixed IGC so cameras move...fixed osiris to
 * be restored correctly, and it handles errors on restore
 *
 * 5     2/23/99 12:44a Jeff
 * added support for in-game-cinematics in demo system
 *
 * 4     2/14/99 1:16a Jeff
 * added canned cinematic function/structures.  Added a flag to push
 * target to end of path if on a path (quick exit).  Added canned intro
 * cine.  Determine correct velocity on player ship for intro cine.
 *
 * 3     2/01/99 12:55p Jeff
 * restore correct hud mode, added flag to stop cinematics if target dies
 *
 * 2     1/31/99 8:48p Jeff
 * new in game cinematics system finished
 *
 * $NoKeywords: $
 */

#ifndef __INGAME_CINEMATICS_H_
#define __INGAME_CINEMATICS_H_

#include "vecmat_external.h"
#include "gamecinematics_external.h"
#include "hud.h"

//	Cinematic_Init
//
//	Initializes the in-game cinematics
void Cinematic_Init(void);

//	Cinematic_Close
//
//	Closes the in-game cinematics
void Cinematic_Close(void);

//	Cinematic_Start
//
//	Starts an in-game cinematic sequence.  text_string is the text to be displayed
//	use pipes (|) to seperate lines.
bool Cinematic_Start(tGameCinematic *info, char *text_string);

//	Cinematic_Stop
//
//	Stops and clears up a in-game cinematic.
void Cinematic_Stop(void);

//	Cinematic_Frame
//
//	Processes a frame for the Cinematics
void Cinematic_Frame(void);

// Renders anything that needs to be rendered for the cinematic frame
void Cinematic_RenderFrame(void);

void Cinematic_LevelInit(void);

extern bool Cinematic_inuse;
extern bool Cinematic_fake_queued;

//	Returns the hud mode before cinematics
tHUDMode Cinematic_GetOldHudMode(void);

// Starts a canned cinematic sequence
// Only the demo system passing in a camera_handle, so it should never be explicitly passed by you
void Cinematic_StartCanned(tCannedCinematicInfo *info, int camera_handle = -1);

void Cinematic_DoDemoFileData(ubyte *data);

#endif
