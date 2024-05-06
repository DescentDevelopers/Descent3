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

 * $Logfile: /DescentIII/main/gauges.h $
 * $Revision: 16 $
 * $Date: 10/20/98 12:42p $
 * $Author: Matt $
 *
 *	Gauge interface
 *
 * $Log: /DescentIII/main/gauges.h $
 *
 * 16    10/20/98 12:42p Matt
 * Made the small views work on the cockpit.
 *
 * 15    5/26/98 5:05p Samir
 * shield frames now contained in hud structure and share common graphics
 * between gauges and hud.
 *
 * 14    5/18/98 4:52p Samir
 * fixed hud image for weapons.
 *
 * 13    3/20/98 8:23p Samir
 * Started implementing images.
 *
 * 12    3/18/98 6:24p Samir
 * Implemented monitor system.
 *
 * 11    3/17/98 2:37p Samir
 * reorg of hud/gauge/cockpit dependencies.
 *
 * 10    1/23/98 12:07p Jason
 * made framerate display toggleable
 *
 * 9     12/29/97 5:47p Samir
 * Uses new text system and took out references to Game_viewport.
 *
 * 8     11/16/97 6:55p Samir
 * Added init flag for gauge structure.
 *
 * 7     11/14/97 5:29p Samir
 * Now we pass four world coordinates of the 4 vertex monitor to a gauge
 * rendering function.
 *
 * 6     11/11/97 1:25p Samir
 * Added modified flag to tGauge and cleaned up gauge renderering a
 * little.
 *
 * 5     11/04/97 6:23p Samir
 * New Gauge interface.
 *
 * 4     10/29/97 12:33p Samir
 * Framework for RenderHudGauges
 *
 * 3     10/28/97 6:36p Samir
 * Gauge system for text controls functioning.
 *
 * 2     10/28/97 12:47p Samir
 * Gauges are drawn once per frame from GameLoop to a call to gauges.cpp
 *
 * 1     10/28/97 12:27p Samir
 * Initial revision
 *
 * $NoKeywords: $
 */

#ifndef GAUGES_H
#define GAUGES_H

#include "pstypes.h"
#include "vecmat.h"
#include "hud.h"

// Hack vars for turning off the monitors
extern bool Disable_primary_monitor, Disable_secondary_monitor;

//	number of gauges
#define NUM_GAUGES 16

//	initializes cockpit gauges
void InitGauges(tStatMask gauge_mask);

//	deinitializes cockpit gauges
void CloseGauges();

//	renders gauges
void RenderGauges(vector *cockpit_pos, matrix *cockpit_mat, float *normalized_times, bool moving, bool reset = false);

//	flags certain gauges to be modified next frame.
void FlagGaugesModified(tStatMask mask_modified);

//	sets gauges as functional
void FlagGaugesFunctional(tStatMask mask);

//	sets gauges as gauges nonfunctional
void FlagGaugesNonfunctional(tStatMask mask);

// Returns the coordinates of the specified cockpit monitor
// Parameter:	window - 0 means primary monitor, 1 means secondary
//					x0,y0,x1,y1 - these are filled in with the coordinates of the montiro
// Returns: true if got coords, false if the monitor was animating
bool GetCockpitWindowCoords(int window, int *left, int *top, int *right, int *bot);

#endif
