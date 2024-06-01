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

 * $Logfile: /DescentIII/main/ctlconfig.h $
 * $Revision: 7 $
 * $Date: 4/30/99 12:12p $
 * $Author: Samir $
 *
 * <insert description of file here>
 *
 * $Log: /DescentIII/main/ctlconfig.h $
 *
 * 7     4/30/99 12:12p Samir
 * added strings for key ramping, other buttons in general config menu.
 *
 * 6     4/22/99 3:43p Kevin
 * Training missions show controls on screen
 *
 * 5     2/16/99 12:07p Samir
 * redid controller config with new ui.
 *
 * 4     10/18/98 7:24p Samir
 * added a reset to defaults options for key and joystick config.
 *
 * 3     2/15/98 7:06p Samir
 * Keyboard configuration system done.  Not joystick.
 *
 * 2     2/10/98 3:45p Samir
 * Added list of configuration items.
 *
 * 1     2/10/98 11:49a Samir
 * Initial revision.
 *
 * $NoKeywords: $
 */

#ifndef CTLCONFIG_H
#define CTLCONFIG_H

#include <cstdint>

#define CTLCONFIG_KEYBOARD 0
#define CTLCONFIG_CONTROLLER 1
#define CTLCONFIG_WPNSEL 2

struct t_cfg_element {
  int16_t fn_id; // -1 = group start
  int16_t text;  // text string id.
  int16_t x;
  int16_t y; // location (for groups only)
};

extern t_cfg_element Cfg_key_elements[];
extern t_cfg_element Cfg_joy_elements[];

int CtlFindBinding(int controlid, bool keyboard);
//	configures controllers.
void CtlConfig(int mode);

// opens the settings dialogs.
void joystick_settings_dialog();
void key_settings_dialog();

#endif
