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
 * $Logfile: /DescentIII/Main/ddio_lnx/lnxforcefeedback.cpp $
 * $Revision: 1.1.1.1 $
 * $Date: 2000/04/18 00:00:33 $
 * $Author: icculus $
 *
 * Linux force feedback stub
 *
 * $Log: lnxforcefeedback.cpp,v $
 * Revision 1.1.1.1  2000/04/18 00:00:33  icculus
 * initial checkin
 *
 *
 * 3     4/16/99 8:28p Jeff
 * created more stub functions
 *
 * 2     4/14/99 1:57a Jeff
 * fixed case mismatched #includes
 *
 * 1     1/18/99 12:13a Jeff
 *
 * $NoKeywords: $
 */

#include "DDAccess.h"
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>

#include "pserror.h"
#include "pstring.h"
#include "mono.h"
#include "ddio.h"
#include "application.h"
#include "forcefeedback.h"
#include "mem.h"

bool ddForce_found;   // a Force Feedback device was found
bool ddForce_enabled; // Force Feedback is ready and can be used

// -------------------------------------------------------------------
//	ddio_ff_AttachForce
//	Purpose:
//		Attaches variables initialized in the general ddio system to
//	the force feedback system.
// -------------------------------------------------------------------
void ddio_ff_AttachForce(void) {}

// -------------------------------------------------------------------
//	ddio_ff_DetachForce
//	Purpose:
//		Detaches variables used by the force-feedback system from the
//	ddio system
// -------------------------------------------------------------------
void ddio_ff_DetachForce(void) {}

// -------------------------------------------------------------------
// ddio_ff_Init
// Purpose:
//    Initialize force feedback if available.
// -------------------------------------------------------------------
int ddio_ff_Init(void) {
  ddForce_found = ddForce_enabled = false;
  return 0;
}

// -------------------------------------------------------------------
// ddio_ffjoy_Init
// Purpose:
//    Creates and acquires all joysticks
//
// Input:
//    None
//
// Return:
//    # of sticks acquired
//
// Description:
//
// -------------------------------------------------------------------
int ddio_ffjoy_Init(void) {
  ddForce_found = ddForce_enabled = false;
  return 0;
}

// -------------------------------------------------------------------
// ddio_ff_Acquire
// Purpose:
//    Acquires a direct input device for use.
//
// Input:
//    The device to acquire (use kDI_MaxJoy to acquire all available
//    joysticks).
//
// Return:
//    # of devices acquired.
//
// Description:
//    Call this to gain access to a device after the device has been
//    created & after regaining the focus after losing it.
//
// -------------------------------------------------------------------
int ddio_ff_Acquire(tDevice) { return 0; }

// -------------------------------------------------------------------
// ddio_ff_Unacquire
// Purpose:
//    Unacquires a direct input device
//
// Input:
//    The device to unacquire (use kDI_MaxJoy to unacquire all available
//    joysticks).
//
// Return:
//    # of devices unacquired.
//
// Description:
//    Call this to lose access to a device after the device has been
//    aquired
//
// -------------------------------------------------------------------
int ddio_ff_Unacquire(tDevice) { return 0; }

// -------------------------------------------------------------------
// ddio_ff_SetCoopLevel
// -------------------------------------------------------------------
int ddio_ff_SetCoopLevel(tDevice, int) { return 0; }

// -------------------------------------------------------------------
// ddio_ffjoy_Query
// Purpose:
//    Besides checking what buttons/axis are available, this function
//    also checks for force feedback support.
// -------------------------------------------------------------------
int ddio_ffjoy_Query(int, int *, int *) { return 0; }

/*
========================================================================
                                Force Feedback Effect Functions
========================================================================


*/
// -------------------------------------------------------------------
//	ddio_ff_GetInfo
//	Purpose:
//		Returns information about the current state of the low-level
//	Force Feedback system.
// -------------------------------------------------------------------
void ddio_ff_GetInfo(bool *ff_found, bool *ff_enabled) {
  if (ff_found)
    *ff_found = false;
  if (ff_enabled)
    *ff_enabled = false;
}

// -------------------------------------------------------------------
// ddio_ffb_Pause
// Purpose:
//    Pause the FFB output on the given device.  Use ddio_ffb_Continue to
//    continue where you left off.
// -------------------------------------------------------------------
void ddio_ffb_Pause(tDevice) {}

// -------------------------------------------------------------------
// ddio_ffb_Continue
// Purpose:
//    Unpause the FFB output on the given device.  Complimentary to
//    ddio_ffb_Pause.
// -------------------------------------------------------------------
void ddio_ffb_Continue(tDevice) {}

// -------------------------------------------------------------------
// ddio_ffb_Enable
// Purpose:
//    Must be called after initialization in order to activate the
//    device.
//    Use ddio_ffb_Pause & ddio_ffb_Continue if you want disable forces
//    temporarily and resume later.
// -------------------------------------------------------------------
void ddio_ffb_Enable(tDevice) {}

// -------------------------------------------------------------------
// ddio_ffb_Disable
// Purpose:
//    Turns off FFB, but effects still play on processor.
// -------------------------------------------------------------------
void ddio_ffb_Disable(tDevice) {}

// -------------------------------------------------------------------
// ddio_ffb_effectCreate
// Purpose:
//    Create a single effect for future playback.
//    Effect is given a logical ID
// -------------------------------------------------------------------
int ddio_ffb_effectCreate(tDevice, tFFB_Effect *) { return 0; }

// -------------------------------------------------------------------
// ddio_ffb_DestroyAll
//	Purpose:
//		Destroys all created effects
// -------------------------------------------------------------------
void ddio_ffb_DestroyAll(void) {}

// -------------------------------------------------------------------
// ddio_ffb_effectPlay
// Purpose:
//    Play an effect that was previously created.
// -------------------------------------------------------------------
void ddio_ffb_effectPlay(short) {}

// -------------------------------------------------------------------
// ddio_ffb_effectStop
// Purpose:
//    Stop a single effect.
// -------------------------------------------------------------------
void ddio_ffb_effectStop(short) {}

// -------------------------------------------------------------------
// ddio_ffb_effectStopAll
// Purpose:
//    Stops all forces on the given device.
// -------------------------------------------------------------------
void ddio_ffb_effectStopAll(tDevice) {}

// -------------------------------------------------------------------
// ddio_ffb_effectUnload
// Purpose:
//    Unload a single effect...  Necessary to make room for other
//    effects.
// -------------------------------------------------------------------
void ddio_ffb_effectUnload(short) {}

// -------------------------------------------------------------------
// ddio_ffb_effectModify
// Purpose:
//    Modifies a single effect, only if the given parameters are
//    different from what's currently loaded.
// -------------------------------------------------------------------
void ddio_ffb_effectModify(short, int *, unsigned int *, unsigned int *, unsigned int *, tEffInfo *, tEffEnvelope *) {}

// -------------------------------------------------------------------
// ddio_ffb_GetEffectData
// Purpose:
//    Retrieves affect data for the given parameters, pass NULL for those you don't want
// -------------------------------------------------------------------
void ddio_ffb_GetEffectData(short, int *, unsigned int *, unsigned int *, unsigned int *, tEffInfo *, tEffEnvelope *) {}

// -------------------------------------------------------------------
// ddio_ffjoy_EnableAutoCenter
// Purpose:
//	Disables/Enables the autocentering of the joystick
// -------------------------------------------------------------------
void ddio_ffjoy_EnableAutoCenter(tDevice, bool) {}

// -------------------------------------------------------------------
// ddio_ffjoy_SetGain
// Purpose:
//	Sets the gain for joystick, pass a value of 0-1
// -------------------------------------------------------------------
void ddio_ffjoy_SetGain(tDevice, float) {}

// -------------------------------------------------------------------
// ddio_ffjoy_IsAutoCentered
// Purpose:
//	Returns true if the joystick is set for autocentering
// -------------------------------------------------------------------
bool ddio_ffjoy_IsAutoCentered(tDevice) { return true; }

// -------------------------------------------------------------------
// ddio_ffjoy_SupportAutoCenter
// Purpose:
//	Returns true if the FF joystick supports auto centering
// -------------------------------------------------------------------
bool ddio_ffjoy_SupportAutoCenter(tDevice) { return false; }

//	Given a filename resource, this loads the file and creates a resource
//	for it.  It returns a handle to that resource.
//	If it returns NULL, then it couldn't load the project.
//	Make sure device is aquired before calling.
FORCEPROJECT ddio_ForceLoadProject(char *filename, tDevice dev) { return NULL; }

//	Unloads a FORCEPROJECT file
void ddio_ForceUnloadProject(FORCEPROJECT prj) {}

//	Given a handle to a resource, and the name of the effect to load
//	it will load that effect.  Returns the effect ID, or -1 if it couldn't
//	be created
int ddio_CreateForceFromProject(FORCEPROJECT project, char *forcename) { return -1; }
