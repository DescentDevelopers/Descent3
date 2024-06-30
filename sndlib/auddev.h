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

 * $Source: $
 * $Revision: 16 $
 * $Author: Samir $
 * $Date: 8/23/99 5:29p $
 *
 *
 *
 * $Log: /DescentIII/Main/dd_sndlib/auddev.h $
 *
 * 16    8/23/99 5:29p Samir
 * incremental EAX 2.0 checkin
 *
 * 15    8/13/99 2:00p Samir
 * more aureal and geometry fixes.
 *
 * 14    8/11/99 3:12p Samir
 * fixes for aureal support.
 *
 * 13    5/03/99 3:12a Samir
 * added event management routines.
 *
 * 12    4/29/99 3:01p Samir
 * added some more functions.
 *
 * 11    4/23/99 7:51p Samir
 * added flush function.
 *
 * 10    4/13/99 4:09p Samir
 * more priority stuff.
 *
 * 9     4/06/99 8:30p Samir
 * added reflection support.
 *
 * 8     3/29/99 11:02a Samir
 * added geometry supoprt.
 *
 * 7     1/14/99 6:10p Samir
 * added DirectSound buffer duplication code.
 *
 * 6     1/11/99 5:52p Samir
 * updated EAX support.
 *
 * 5     1/08/99 6:31p Samir
 * added reverb
 *
 * 4     1/08/99 11:36a Samir
 * implemented basic Aureal 2.0 support.
 *
 * 3     12/23/98 11:50a Samir
 *
 * 2     12/23/98 11:48a Samir
 * basic functionality.
 *
 * 1     12/22/98 11:56a Samir
 *
 */

#ifndef AUDDEV_H
#define AUDDEV_H

#include <windows.h>
#include <mmsystem.h>
#include "dsound.h"
#include "vmanpset.h"
#include "pstypes.h"
#include "vecmat.h"

#define SOUND_DECAY_MAXTIME 100.0f
#define SOUND_GAIN_MAXSCALAR 1.0f

// Creative labs Environmental Audio Extension Interface

//	starts up standard audio device (DirectSound)
bool EAX_Create(GUID *pguid, LPDIRECTSOUND *lpds);

//	shutdown standard audio device (DirectSound)
void EAX_Destroy();

// returns EAX caps
#define EAXF_SOURCE_OBSTRUCTION 16
int EAX_Caps();

// initializes EAX specific interfaces.
bool EAX_SetPrimaryBuffer();

// sets up current global environment reverb
bool EAX_SetEnvironmentalReverb(float volume, float damping, float decay);

// intializes a sound source for EAX
bool EAX_InitSource(LPDIRECTSOUND3DBUFFER lpBuffer3D, LPKSPROPERTYSET *plpksp);

//	frees an eax sound source
void EAX_FreeSource(LPKSPROPERTYSET lpksp);

//	sets source properties
void EAX_SetSourceProperties(LPKSPROPERTYSET lpksp, float obstruction);

#endif
