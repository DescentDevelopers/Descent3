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

 * $Logfile: /DescentIII/Main/ui/UIlib.h $
 * $Revision: 14 $
 * $Date: 2/17/99 8:32p $
 * $Author: Samir $
 *
 *	UI Library internal header.
 *
 * $Log: /DescentIII/Main/ui/UIlib.h $
 *
 * 14    2/17/99 8:32p Samir
 * fixed bugs in remove gadget, added callbacks for attaching and
 * detaching gadgets.
 *
 * 13    10/13/98 8:23p Samir
 * added more keyboard input options.
 *
 * 12    6/05/98 5:35p Samir
 * massive improvement in UI keyboard interface.
 *
 * 11    5/06/98 4:34p Samir
 * added function to set ui screen resolution.
 *
 * 10    3/13/98 5:18p Jeff
 * Added arrow character defines
 *
 * 9     3/05/98 6:40p Samir
 * Implemented radio buttons.
 *
 * 8     2/13/98 6:35p Samir
 * Added double click delay constant.
 *
 * 7     1/30/98 7:06p Samir
 * Moved a lot of functions out to uixxx.h library files.
 *
 * 6     1/18/98 4:23p Samir
 * Took out old bitmap management system for UI.
 *
 * 5     1/13/98 6:59p Jeff
 * (Samir) moved defines from UIlib.h to ui.h.
 *
 * 4     1/13/98 4:32p Samir
 * Made tUIInput a shared structure to be used by ui_DoFrame to return UI
 * input values.
 *
 * 3     1/13/98 4:29p Samir
 * Added some ID numbers and macros.
 *
 * 2     1/02/98 12:49p Samir
 * Added ui_GetTextWidth.
 *
 * 1     12/30/97 4:37p Samir
 * Initial revision
 *
 * $NoKeywords: $
 */

#ifndef UILIB_H
#define UILIB_H

#include "ui.h"
#include "ddio.h"

struct tUIOutput {
  int id;           // return value
  UIGadget *gadget; // object that's linked to return value.
};

//	ids internal to UI system
#define UID_UP 0
#define UID_RIGHT 1
#define UID_DOWN 2
#define UID_LEFT 3

//	constants
#define UI_SCROLL_REGION_EXTENT 40
#define UI_LEFT_ARROW_CHAR 24
#define UI_UP_ARROW_CHAR 25
#define UI_RIGHT_ARROW_CHAR 26
#define UI_DOWN_ARROW_CHAR 27
#define UI_CHECKBOX_OFF_CHAR 28
#define UI_CHECKBOX_ON_CHAR 29
#define UI_RADIO_OFF_CHAR 30
#define UI_RADIO_ON_CHAR 31

//	user ouput structure
extern tUIOutput UI_output;

extern float UI_aspect_x;
extern float UI_aspect_y;

#define UI_X(_x) ((int)(UI_aspect_x * (_x)))
#define UI_Y(_y) ((int)(UI_aspect_y * (_y)))

//	retrieves input for user interface
bool ui_Poll();

//	flushes out key input info currently in ui.
void ui_KeyFlush();

#endif
