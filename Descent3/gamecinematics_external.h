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

 * $Logfile: /DescentIII/Main/gamecinematics_external.h $
 * $Revision: 13 $
 * $Date: 5/10/99 10:22p $
 * $Author: Ardussi $
 *
 * Defines and structs external for DLLs
 *
 * $Log: /DescentIII/Main/gamecinematics_external.h $
 *
 * 13    5/10/99 10:22p Ardussi
 * changes to compile on Mac
 *
 * 12    5/04/99 6:53p Jeff
 * added new canned cinematic to fade screen to white and endlevel.  Fixed
 * crash bug with invalid player path for canned cine with player paths
 *
 * 11    4/04/99 8:21p Matt
 * Fixed include problems with osiris vector stuff.  Osiris headers now
 * include vecmat_external.h, instead of defining the typedefs, structs,
 * and inlines on the Osiris side.
 *
 * 10    3/27/99 7:22p Jeff
 * fixed cinematics when going from one cut to another immediatly.  Added
 * start transition
 *
 * 9     2/28/99 8:32p Jeff
 * added fade and move player dallas action.  Fixed the end-level sequence
 * changing view back to player for split second.
 *
 * 8     2/22/99 1:20a Jeff
 * added support for inventory (simple) in dallas.  Moved end-level
 * sequence to use IGC.  Add position clipboard stuff for dallas.  Fixed
 * some inventory bug with storing object handles
 *
 * 7     2/21/99 8:36p Jeff
 * misc changes to handle new matcen and path types of dallas
 *
 * 6     2/16/99 9:36p Jeff
 * added low text layout for cinematics
 *
 * 5     2/14/99 1:16a Jeff
 * added canned cinematic function/structures.  Added a flag to push
 * target to end of path if on a path (quick exit).  Added canned intro
 * cine.  Determine correct velocity on player ship for intro cine.
 *
 * 4     2/12/99 2:45a Jeff
 * added end-cinematic transitions
 *
 * 3     2/02/99 3:58p Jeff
 * started to implement level intro cinematic (need ai functions first).
 * No longer need camera object to do cinematic (auto-created)...path
 * cameras use speed based on distance needed to travel.
 *
 * 2     2/01/99 12:55p Jeff
 * restore correct hud mode, added flag to stop cinematics if target dies
 *
 * 1     1/31/99 10:28p Jeff
 *
 * $NoKeywords: $
 */

#ifndef __GAMECINEMATICS_EXTERNAL_H_
#define __GAMECINEMATICS_EXTERNAL_H_

#include <cstdint>

#include "vecmat_external.h"
#include "manage_external.h"

/*
==========================================================
                Canned Cinematics
*/
#define CANNED_LEVEL_INTRO 1
#define CANNED_LEVEL_END_PATH 2       // end level with camera on path
#define CANNED_LEVEL_END_POINT 3      // end level with camera on point
#define CANNED_MOVE_PLAYER_FADE 4     // fade the screen out and move the player to a new position
#define CANNED_LEVEL_END_FADE_WHITE 5 // fade the screen to white and end level

struct tCannedCinematicInfo {
  int32_t type;
  /*
  CANNED_LEVEL_INTRO:
          camera_pathid
          target_pathid
          text_to_display
          time
  */
  /*
  CANNED_LEVEL_END_PATH
          camera_pathid
          target_pathid
          text_to_display
          time
  */
  /*
  CANNED_LEVEL_END_POINT
          object_to_use_for_point
          target_pathid
          text_to_display
          time
  */
  /*
  CANNED_MOVE_PLAYER_FADE
          room	//destination room
          pos		//destination pos
          orient	//destination orient
          target_objhandle	//the player
  */
  /*
  CANNED_LEVEL_END_FADE_WHITE
          time
          text_to_display
  */

  int32_t camera_pathid;
  int32_t target_pathid;
  const char *text_to_display;

  int32_t target_objhandle;

  int32_t room;

  float time;

  int32_t object_to_use_for_point;

  vector pos;
  matrix orient;

};

//========================================================

// flags for game cinematic struct
#define GCF_LETTERBOX 0x00000000
#define GCF_FULLSCREEN 0x00000001
#define GCF_SCREENFORMAT 0x00000001 // mask for screen format to use

#define GCF_USEPATH 0x00000000
#define GCF_USEPOINT 0x00000002
#define GCF_CAMERAPLACEMENT 0x00000002 // mask for camera placement (use path or point)

#define GCF_TEXT_WIPEIN 0x00000004
#define GCF_TEXT_FADEINOUT 0x00000008
#define GCF_TEXT_NOEFFECT 0x00000000
#define GCF_TEXT_MASK 0x0000000C // mask for text modes

#define GCF_LAYOUT_BOTTOM 0x00000000
#define GCF_LAYOUT_TOP 0x00000010
#define GCF_LAYOUT_MIDDLE 0x00000020
#define GCF_LAYOUT_LOW 0x00000040
#define GCF_LAYOUT_MASK 0x00000070 // mask for text layout to use

#define GCF_STOPIFTAGETDEAD 0x00000100 // stop the cinematic if the target dies
#define GCF_FORCETARGETTOEND                                                                                           \
  0x00000200 // if there is a quick exit (by keypress) force target position to the end of the path
#define GCF_DONTRESTOREVIEWER                                                                                          \
  0x00000400 // don't restore the view back to the player (this is really an internal flag and shouldn't be used)
#define GCF_IGNOREPLAYERDEAD 0x00000800  // don't stop cinematic if player dies
#define GCF_DOTRANSBEFORETEXT 0x00001000 // process end transitions before drawing text

// callback types
#define GCCT_START 0x00000001 // first frame of the movie
#define GCCT_STOP 0x00000002  // last frame of the movie
#define GCCT_FRAME 0x00000003 // frame of the movie

// Transition types
#define GCTT_NONE 0
#define GCTT_WACKY 1
#define GCTT_FADE 2
#define GCTT_FADEINOUT 3
#define GCTT_FADEWHITE 4

struct PercentageRange {
  float min, max; // 0.0f->1.0f
};

struct tGameCinematic {
  uint32_t flags;

  int32_t target_objhandle;

  int32_t end_transition;
  int32_t start_transition;

  // the name of the path or the position that the camera should be at
  int32_t pathid;
  vector position;
  matrix *orient;
  int32_t room;

  // the total time that the cinematic should play
  float max_time_play;

  // called every frame while the cinematic is processing
  void (*callback)(int type);

  // ranges (based on max_time_play)
  PercentageRange text_display;    // range that the text should be displayed
  PercentageRange track_target;    // range that the camera should track the target
  PercentageRange player_disabled; // range that the player can't move/fire/etc.
  PercentageRange in_camera_view;  // range that the view is from the camera
  PercentageRange quick_exit;      // range where a key-press will quick exit cinematic

};

#endif
