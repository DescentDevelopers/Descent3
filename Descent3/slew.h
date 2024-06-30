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

 * Header for slew.c
 *
 */

#ifndef _SLEW_H
#define _SLEW_H

#if (defined(_DEBUG) || defined(EDITOR))

#include "object.h"

#define SLEW_MOVE 1
#define SLEW_KEY 2
#define SLEW_ROTATE 4

void SlewResetOrient(object *obj); // Resets the object's orientation
int SlewFrame(object *obj,
              int movement_limitations = 0); // Moves the object for one frame  (returns flags.  1=MOVED 2=KEY_ACTION
int SlewStop(object *obj);                   // Stop the object
void SlewControlInit();                      // initializes controller system for slewer

extern float Slew_key_speed;
// If there's an active joystick, this is the stick number.  Else, this is -1
extern int Joystick_active;

#else

#define SlewResetOrient(obj)
#define SlewFrame(obj)
#define SlewStop(obj)
#define SlewControlInit()

#endif

#endif
