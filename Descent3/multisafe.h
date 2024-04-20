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

#ifndef MULTISAFE_H
#define MULTISAFE_H

#include "vecmat.h"
#include "osiris_share.h"

// LOCAL MULTISAFE FUNCTIONS
void msafe_CallFunction(ubyte type, msafe_struct *mstruct);
void msafe_DoPowerup(msafe_struct *mstruct);

// Gets a value for the calling party
void msafe_GetValue(int type, msafe_struct *mstruct);
//----------------------------------------------------------------------

// MULTPLAYER SPECIFIC STUFF
// MULTISAFE FUNCTIONS
void MultiSendMSafeFunction(ubyte type, msafe_struct *mstruct);
void MultiSendMSafePowerup(msafe_struct *mstruct);

#endif