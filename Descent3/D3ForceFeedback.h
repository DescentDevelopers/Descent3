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
 * $Logfile: /DescentIII/main/D3ForceFeedback.h $
 * $Revision: 9 $
 * $Date: 1/28/99 12:09p $
 * $Author: Jeff $
 *
 * High Level Force Feedback implementation
 *
 * $Log: /DescentIII/main/D3ForceFeedback.h $
 *
 * 9     1/28/99 12:09p Jeff
 * added force feedback to player shake...fixed spelling error in define
 * for forcefeedback
 *
 * 8     11/18/98 5:50p Jeff
 * added some cheap recoil effects for ForceFeedback...not fully
 * implemented
 *
 * 7     11/10/98 5:16p Jeff
 * updated forcefeedback system...pretty complete now
 *
 * 6     11/06/98 7:00p Jeff
 * first round of new force feedback installed
 *
 * 5     11/03/98 6:43p Jeff
 * new low-level & high level Force Feedback system implemented, handles
 * window losing focus, etc.
 *
 * 4     10/12/98 3:49p Jeff
 * struct changes
 *
 * 3     9/21/98 11:10a Jeff
 * general update, new low level, small high level implementation
 *
 * 2     9/18/98 7:38p Jeff
 * creation of low-level forcefeedback and beginning of high-level
 * forcefeedback
 *
 * $NoKeywords: $
 */

#ifndef __D3FORCEFEEDBACK_H_
#define __D3FORCEFEEDBACK_H_

#include "forcefeedback.h"
#include "vecmat.h"
#include "object.h"

// D3Use_force_feedback
// true if the user wants force feedback during play (if available)
extern bool D3Use_force_feedback;
// D3Force_auto_center
// true if the user wants his joystick to be autocentered (if available)
extern bool D3Force_auto_center;
// D3Force_gain
// the gain setting of the ForceFeedback system
extern float D3Force_gain;

#define FORCE_TEST_FORCE 0
#define FORCE_MICROWAVE 1
#define FORCE_WALLHIT 2
#define FORCE_WEAPON_RECOIL 3
#define FORCE_VAUSS_RECOIL 4
#define FORCE_AFTERBURNER 5
#define FORCE_SHIPSHAKE 6

// -----------------------------------------------------------------
//	ForceInit
//	Purpose:
//		Initializes the Hi-Level Force Feedback system, creating all
//	of the effects so they are ready to be used.
// -----------------------------------------------------------------
void ForceInit(void);

// -----------------------------------------------------------------
//	ForceClose
//	Purpose:
//		Shutsdown the Hi-Level Force Feedback system
// -----------------------------------------------------------------
void ForceClose(void);

// -----------------------------------------------------------------
//	ForceShutdown
//	Purpose:
//		Puts the Force Feedback system on pause while the application
//	is doing other things (like it lost focus)
// -----------------------------------------------------------------
void ForceShutdown(void);

// ------------------------------------------------------------------
//	ForceRestart
//	Purpose:
//		Restores the Force Feedback system that has been put on pause
//	from ForceShutdown.
// ------------------------------------------------------------------
void ForceRestart(void);

// ------------------------------------------------------------------
//	ForceDisable
//	Purpose:
//		Disables Force Feedback on a Force Feedback system
// ------------------------------------------------------------------
void ForceDisable(void);

// ------------------------------------------------------------------
//	ForceEnable
//	Purpose:
//		Enables Force Feedback that has been previously disabled
// ------------------------------------------------------------------
void ForceEnable(void);

// ------------------------------------------------------------------
//	ForceIsEnabled
//	Purpose:
//		Returns true if Force Feedback is enabled on the system
// ------------------------------------------------------------------
bool ForceIsEnabled(void);

// ------------------------------------------------------------------
//	ForceEnableAutoCenter
//	Purpose:
//		Enables autocentering on the joystick
// ------------------------------------------------------------------
void ForceEnableAutoCenter(void);

// ------------------------------------------------------------------
//	ForceDisableAutoCenter
//	Purpose:
//		Disables autocentering on the joystick
// ------------------------------------------------------------------
void ForceDisableAutoCenter(void);

// ------------------------------------------------------------------
//	ForceIsAutoCenter
//	Purpose:
//		Returns true if Force Feedback joystick is autocentering
// ------------------------------------------------------------------
bool ForceIsAutoCenter(void);

// ------------------------------------------------------------------
//	ForceSetGain
//	Purpose:
//		Sets the gain of the ForceFeedback system (0 -> 1)
// ------------------------------------------------------------------
void ForceSetGain(float val);

// ------------------------------------------------------------------
// ForceGetGain
//	Purpose:
//		Returns the current gain setting of the ForceFeedback system (0-1)
// ------------------------------------------------------------------
float ForceGetGain(void);

// ------------------------------------------------------------------
//	ForceEffectsInit
//	Purpose:
//		Initializes the force feedback effects for Descent 3
// ------------------------------------------------------------------
void ForceEffectsInit(void);

// -----------------------------------------------------------------
//	ForceEffectsClose
//	Purpose:
//		Destroys all the effects created
// -----------------------------------------------------------------
void ForceEffectsClose(void);

// -----------------------------------------------------------------
//	ForceEffectsPlay
//	Purpose:
//		Plays an effect
// -----------------------------------------------------------------
void ForceEffectsPlay(int id, float *scale, int *direction);
void ForceEffectsPlay(int id, float *scale, vector *direction);

void DoForceForWeapon(object *me_obj, object *it_obj, vector *force);
void DoForceForWall(object *playerobj, float hitspeed, int hitseg, int hitwall, vector *wall_normal);
void DoForceForRecoil(object *playerobj, object *weap);
void DoForceForShake(float magnitude);

extern bool D3Use_force_feedback;

#endif
