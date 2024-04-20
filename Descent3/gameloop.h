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

/*
 * $Logfile: /DescentIII/main/gameloop.h $
 * $Revision: 16 $
 * $Date: 4/30/99 12:02p $
 * $Author: Matt $
 *
 * Header for gameloop.c
 *
 * $Log: /DescentIII/main/gameloop.h $
 *
 * 16    4/30/99 12:02p Matt
 * Keep the rear view up between levels, but close the other camera views.
 *
 * 15    4/17/99 6:15p Samir
 * replaced gr.h with grdefs.h and fixed resulting compile bugs.
 *
 * 14    10/21/98 11:14a Samir
 * added generic code to skip rendering while in game controller config or
 * telcom.
 *
 * 13    8/24/98 10:40a Jason
 * fixed some rendering problems
 *
 * 12    3/03/98 12:10p Samir
 * Added simple pausing.
 *
 * 11    2/25/98 2:05p Jason
 * did FOV and object visibility changes
 *
 * 10    2/24/98 2:31p Jason
 * added FOV warp effect for when coming back to life
 *
 * 9     2/18/98 3:25p Samir
 * Added test systems code.
 *
 * 8     2/12/98 6:51p Matt
 * Renamed GameLoop() to be GameFrame(), cleaned up the StartTime/StopTime
 * system, and moved the interval script call to GameFrame().
 *
 * 7     2/09/98 3:54p Matt
 * Added prototype
 *
 * 6     2/04/98 6:09p Matt
 * Changed object room number to indicate a terrain cell via a flag.  Got
 * rid of the object flag which used to indicate terrain.
 *
 * 5     2/04/98 12:50a Matt
 * Added ability to view from a gun point in small views.
 * Made pop-up view separate from and bigger than other small views.
 * Added real keys to set view mode in the three small views.
 * Changed rendering functions a bit to accommodate smallview changes.
 *
 * 4     1/30/98 2:56p Matt
 * Added support for small views.  Made R toggle rear view, and Delete-F8
 * show a view from object 2.  Also changed main window render flow so
 * could share code with small views.
 *
 * 3     1/06/98 1:28p Matt
 * Cleaned up interfaces to rendering routines, deleted unused code, etc.
 *
 * 2     12/29/97 5:44p Samir
 * Took out references to grViewport and old 2d library.
 *
 * 3     5/05/97 4:47p Jason
 * made terrain and mine rendering work without explicit calls to
 * g3_Startframe in their main function calls
 * This allows us to integrate mine/terrain engines nicely
 *
 * 2     2/03/97 8:14p Matt
 * Added gameloop.cpp & gameloop.h
 *
 * 1     2/03/97 6:30p Matt
 *
 * $NoKeywords: $
 */

#include "grdefs.h"
#include "object.h"

// Do all game functions for one frame: render, move objects, etc.
void GameFrame(void);

// Render the world into a game window
// Parameters:	viewer - if not null, this object disabled from rendering.  Not used otherwise.
//					viewer_eye - where we're rendering from
//					viewer_roomnum - the roomnum viewer_eye is in
//					viewer_orient - the oriention for this view
//					zoom - the zoom for this view
//					rear_view - if true, we're looking out the rear of this object
void GameRenderWorld(object *viewer, vector *viewer_eye, int viewer_roomnum, matrix *viewer_orient, float zoom,
                     bool rear_view);

extern float Render_zoom;
extern float Render_FOV;

extern bool Game_paused; //	determines if game is paused.

extern bool Rendering_main_view;    // determines if we're rendering the main view
extern bool Skip_render_game_frame; // skips rendering the game frame if set.

// Turn off all camera views
// If total reset is true, set all views to none, otherwise kill object view but keep rear views.
void InitCameraViews(bool total_reset);

// Pauses game
void PauseGame();

//	resumes game operation.
void ResumeGame();

#ifdef _DEBUG
//	initializes test systems.
void InitTestSystems();
#endif
