/*
 * $Logfile: /DescentIII/main/cockpit.h $
 * $Revision: 10 $
 * $Date: 10/20/98 12:42p $
 * $Author: Matt $
 *
 * cockpit code
 *
 * $Log: /DescentIII/main/cockpit.h $
 *
 * 10    10/20/98 12:42p Matt
 * Made the small views work on the cockpit.
 *
 * 9     8/18/98 1:11a Samir
 * fixed some stuff for multi cockpit configs.
 *
 * 8     5/26/98 5:05p Samir
 * cockpit and hud config file now 'unified'.   cockpit adds to hudconfig
 * load's funtionality, so cockpit info files can contain same syntax as
 * hud file.
 *
 * 7     5/14/98 7:40p Samir
 * shaking cockpit.
 *
 * 6     5/05/98 6:27p Samir
 * function that tells the cockpit system that it was resized
 *
 * 5     5/01/98 4:24p Samir
 * added function to quickly open the cockpit.
 *
 * 4     4/24/98 8:01a Samir
 * don't pass zoom argument to Render functions.
 *
 * 3     3/17/98 2:37p Samir
 * reorg of hud/gauge/cockpit dependencies.
 *
 * 2     3/16/98 3:30p Samir
 * incremental checkin.
 *
 * 1     3/13/98 11:22a Samir
 * initial revision.
 *
 * $NoKeywords: $
 */

#ifndef COCKPIT_H
#define COCKPIT_H

#include "pstypes.h"
#include "vecmat.h"

struct poly_model;
struct bsp_info;

//	renders the cockpit.
void RenderCockpit();

//	initialization of cockpit information.
void InitCockpit(int ship_index);

//	Forces freeing of cockpit information
void FreeCockpit();

//	check if cockpit exists
bool IsValidCockpit();

//	forces opening of cockpit on hud
void OpenCockpit();

//	forces closing of cockpit on hud.
void CloseCockpit();

//	resizes cockpit.
void ResizeCockpit();

//	forces quick open of cockpit
void QuickOpenCockpit();

//	forces quick closing of cockpit
void QuickCloseCockpit();

//	returns the submodel number of the monitor requested
bsp_info *CockpitGetMonitorSubmodel(ushort monitor_flag);

//	returns the polymodel for the hud
poly_model *CockpitGetPolyModel();

//	cockpit effects.
void StartCockpitShake(float mag, vector *vec);

#define COCKPIT_STATE_DORMANT 0
#define COCKPIT_STATE_QUASI 1 // state changing
#define COCKPIT_STATE_FUNCTIONAL 2

// Tell whether the cockpit is animating
int CockpitState();

#endif
