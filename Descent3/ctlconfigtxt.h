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

 * $Logfile: /DescentIII/Main/ctlconfigtxt.h $
 * $Revision: 26 $
 * $Date: 3/23/99 4:26p $
 * $Author: Jeff $
 *
 * Control Config text items
 *
 * $Log: /DescentIII/Main/ctlconfigtxt.h $
 *
 * 26    3/23/99 4:26p Jeff
 * fixed text for audio taunt 3 & 4
 *
 * 25    3/22/99 6:20p Jeff
 * added 2 more audio taunts.  a mulitplayer event when someone plays an
 * audio taunt.  option to disable audio taunts.
 *
 * 24    2/28/99 3:22a Samir
 * added rearview text.
 *
 * 23    2/26/99 2:13a Samir
 * massive reorg of weapon select prec
 *
 * 22    2/16/99 12:07p Samir
 * redid controller config with new ui.
 *
 * 21    11/30/98 5:22p Samir
 * you can no longer bind keys to number keys.
 *
 * 20    10/21/98 12:00p Samir
 * oops.
 *
 * 19    10/21/98 12:00p Samir
 * don't allow player to configure ESC and function keys.
 *
 * 18    10/17/98 7:32p Samir
 * made controller config text lowercase to save space.
 *
 * 17    10/08/98 10:52a Samir
 * added countermeasure and weapon cycling.
 *
 * 16    9/17/98 3:24p Samir
 * added headlight configuration.
 *
 * 15    8/31/98 6:50p Jeff
 * made inventory and countermeasure keys customizable
 *
 * 14    6/18/98 5:49p Samir
 * finished controller string localization minus the binding stuff.
 *
 * 13    6/18/98 4:48p Samir
 * added changes for multiple configs for joystick controls.
 *
 * 12    6/17/98 3:27p Jeff
 * Changes made for localization
 *
 * 11    6/05/98 5:34p Samir
 * new config system.
 *
 * 10    5/19/98 3:36p Samir
 * added automap key.
 *
 * 9     5/15/98 3:13p Samir
 * formatted text better.
 *
 * 8     5/11/98 4:53p Samir
 * added axis sliding, config.
 *
 * 7     4/08/98 3:44p Samir
 * added toggle bank and configuration for afterburner
 *
 * 6     2/16/98 9:27p Samir
 * Simple joystick configuration working.
 *
 * 5     2/16/98 3:02p Samir
 * Joystick config sort of working.
 *
 * 4     2/15/98 7:06p Samir
 * Keyboard configuration system done.  Not joystick.
 *
 * 3     2/13/98 6:37p Samir
 * Simple keyboard configuration.
 *
 * 2     2/10/98 4:55p Samir
 * Added rudimentary items for controller config.
 *
 * 1     2/10/98 3:47p Samir
 *
 * $NoKeywords: $
 */
#include "stringtable.h"

#define CtlText_FirePrimary TXI_KB_FIREPRIMARY
#define CtlText_FireSecondary TXI_KB_FIRESECONDARY
#define CtlText_Accelerate TXI_KB_ACCELERATE
#define CtlText_Reverse TXI_KB_REVERSE
#define CtlText_SlideHoriz TXI_KB_SLIDEHORIZ
#define CtlText_SlideLeft TXI_KB_SLIDELEFT
#define CtlText_SlideRight TXI_KB_SLIDERIGHT
#define CtlText_SlideVert TXI_KB_SLIDEVERT
#define CtlText_SlideUp TXI_KB_SLIDEUP
#define CtlText_SlideDown TXI_KB_SLIDEDOWN
#define CtlText_BankLeft TXI_KB_BANKLEFT
#define CtlText_BankRight TXI_KB_BANKRIGHT
#define CtlText_PitchUp TXI_KB_PITCHUP
#define CtlText_PitchDown TXI_KB_PITCHDOWN
#define CtlText_TurnLeft TXI_KB_TURNLEFT
#define CtlText_TurnRight TXI_KB_TURNRIGHT
#define CtlText_FireFlare TXI_KB_FIREFLARE
#define CtlText_ToggleSlide TXI_KB_TOGGLESLIDE
#define CtlText_ToggleBank TXI_KB_TOGGLEBANK
#define CtlText_Heading TXI_KB_HEADING
#define CtlText_Pitch TXI_KB_PITCH
#define CtlText_Throttle TXI_KB_THROTTLE
#define CtlText_Forward TXI_KB_FORWARD
#define CtlText_Bank TXI_KB_BANK
#define CtlText_Afterburn TXI_KB_AFTERBURN
#define CtlText_Automap TXI_KB_AUTOMAP
#define CtlText_PrevInv TXI_KB_PREVINV
#define CtlText_NextInv TXI_KB_NEXTINV
#define CtlText_InvUse TXI_KB_INVUSE
#define CtlText_PrevCntMs TXI_KB_PREVCNTMS
#define CtlText_NextCntMs TXI_KB_NEXTCNTMS
#define CtlText_CntMsUse TXI_KB_CNTMSUSE
#define CtlText_Headlight TXI_KB_HEADLIGHT
#define CtlText_WpnCycP TXI_KB_WPNPCYCLE
#define CtlText_WpnCycS TXI_KB_WPNSCYCLE
#define CtlText_AudioTaunt1 TXI_KB_AUDIOTAUNT1
#define CtlText_AudioTaunt2 TXI_KB_AUDIOTAUNT2
#define CtlText_Rearview TXI_KB_REARVIEW
#define CtlText_AudioTaunt3 TXI_KB_AUDIOTAUNT3
#define CtlText_AudioTaunt4 TXI_KB_AUDIOTAUNT4

#define CtlText_WeaponGroup TXI_KB_WPNGRP
#define CtlText_MiscGroup TXI_KB_MISCGRP
#define CtlText_ThrustGroup TXI_KB_THRUSTGRP
#define CtlText_TurningGroup TXI_KB_TURNINGGRP

/*
static char *CtlText_FirePrimary		="Fire Primary\t\t";
static char *CtlText_FireSecondary	="Fire Secondary\t\t";
static char *CtlText_Accelerate		="Accelerate\t\t\t";
static char *CtlText_Reverse			="Reverse\t\t\t";
static char *CtlText_SlideHoriz		="Slide Horizontal\t\t";
static char *CtlText_SlideLeft		="Slide Left\t\t\t";
static char *CtlText_SlideRight		="Slide Right\t\t\t";
static char *CtlText_SlideVert		="Slide Vertical\t\t";
static char *CtlText_SlideUp			="Slide Up\t\t\t";
static char *CtlText_SlideDown		="Slide Down\t\t\t";
static char *CtlText_BankLeft			="Bank Left\t\t\t";
static char *CtlText_BankRight		="Bank Right\t\t\t";
static char *CtlText_PitchUp			="Pitch Up\t\t\t";
static char *CtlText_PitchDown		="Pitch Down\t\t\t";
static char *CtlText_TurnLeft			="Turn Left\t\t\t";
static char *CtlText_TurnRight		="Turn Right\t\t\t";
static char *CtlText_FireFlare		="Fire Flare\t\t\t";
static char *CtlText_ToggleSlide		="Toggle Slide\t\t";
static char *CtlText_ToggleBank		="Toggle Bank\t\t\t";
static char *CtlText_Heading			="Heading\t\t\t";
static char *CtlText_Pitch				="Pitch\t\t\t\t";
static char *CtlText_Throttle			="Throttle\t\t\t";
static char *CtlText_Forward			="Forward\t\t\t";
static char *CtlText_Bank				="Bank\t\t\t\t";
static char *CtlText_Afterburn		="Afterburner\t\t\t";
static char *CtlText_Automap			="Automap\t\t\t";
*/
