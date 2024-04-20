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
 *	File:		InSprocket.h
 *
 */

#ifndef __InSprocket__
#define __InSprocket__

#include <Types.h>
#include <InputSprocket.h>

typedef enum TInputEvent {
  kInputEvent_None,
  // it's IMPORTANT that end be begin+1
  kInputEvent_FirePrim_On,
  kInputEvent_FirePrim_Off,
  kInputEvent_FireSecond_On,
  kInputEvent_FireSecond_Off,
  kInputEvent_AfterBurn_On,
  kInputEvent_AfterBurn_Off,
  kInputEvent_RearView_On,
  kInputEvent_RearView_Off,

  kInputEvent_NextPrim,
  kInputEvent_NextSecond,
  kInputEvent_Bank_On,
  kInputEvent_Bank_Off,
  kInputEvent_Slide_On,
  kInputEvent_Slide_Off,

  kInputEvent_InventoryUse,
  kInputEvent_InventoryNext,
  kInputEvent_InventoryPrev,
  kInputEvent_CounterUse,
  kInputEvent_CounterNext,
  kInputEvent_CounterPrev,

  kInputEvent_Accelerate_On,
  kInputEvent_Accelerate_Off,
  kInputEvent_Reverse_On,
  kInputEvent_Reverse_Off,
  kInputEvent_SlideLeft_On,
  kInputEvent_SlideLeft_Off,
  kInputEvent_SlideRight_On,
  kInputEvent_SlideRight_Off,
  kInputEvent_SlideUp_On,
  kInputEvent_SlideUp_Off,
  kInputEvent_SlideDown_On,
  kInputEvent_SlideDown_Off,

  kInputEvent_BankLeft_On,
  kInputEvent_BankLeft_Off,
  kInputEvent_BankRight_On,
  kInputEvent_BankRight_Off,
  kInputEvent_TurnLeft_On,
  kInputEvent_TurnLeft_Off,
  kInputEvent_TurnRight_On,
  kInputEvent_TurnRight_Off,
  kInputEvent_PitchUp_On,
  kInputEvent_PitchUp_Off,
  kInputEvent_PitchDown_On,
  kInputEvent_PitchDown_Off,

  kInputEvent_HeadLight,
  kInputEvent_Flare,
  kInputEvent_Map,

  kInputEvent_ATaunt1,
  kInputEvent_ATaunt2,
  kInputEvent_ATaunt3,
  kInputEvent_ATaunt4,

  kNumEvents

} TInputEvent;

// input sprocket
enum {
  kNeed_Yaw,
  kNeed_Pitch,
  kNeed_Roll,
  kNeed_MoveX,
  kNeed_MoveZ,
  kNeed_MoveY,

  kNeed_MouseYaw,
  kNeed_MousePitch,
  kNeed_MouseRoll,
  kNeed_MouseMoveX,
  kNeed_MouseMoveZ,
  kNeed_MouseMoveY,

  kNeed_Accelerate,
  kNeed_Reverse,
  kNeed_SlideLeft,
  kNeed_SlideRight,

  kNeed_SlideUp,
  kNeed_SlideDown,
  kNeed_TurnLeft,
  kNeed_TurnRight,
  kNeed_PitchUp,
  kNeed_PitchDown,
  kNeed_BankLeft,
  kNeed_BankRight,

  kNeed_Seperator,

  kNeed_FirePrim,
  kNeed_FireSecond,
  kNeed_NextPrim,
  kNeed_NextSecond,
  //	kNeed_PrevPrim,
  //	kNeed_PrevSecond,
  kNeed_AfterBurn,
  kNeed_HeadLight,
  kNeed_Flare,
  kNeed_RearView,
  kNeed_Map,

  kNeed_InventoryUse,
  kNeed_InventoryNext,
  kNeed_InventoryPrev,
  kNeed_CounterUse,
  kNeed_CounterNext,
  kNeed_CounterPrev,

  kNeed_BankOn,
  kNeed_SlideOn,
  /*
          kNeed_ATaunt1,
          kNeed_ATaunt2,
          kNeed_ATaunt3,
          kNeed_ATaunt4,
  */
  kNeed_COUNT
};

#define kNeedsVersion '0003'

static Boolean gInputActive = false;
static ISpElementListReference gInputEventList = NULL;
static ISpElementListReference gInputHoldDownEventList = NULL;
static ISpElementReference gInputElement[kNeed_COUNT] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};

#define ISpSymmetricAxisToFloat(axis) ((((float)axis) - kISpAxisMiddle) / (kISpAxisMaximum - kISpAxisMiddle))
#define ISpAsymmetricAxisToFloat(axis) (((float)axis) / (kISpAxisMaximum))

void inSprocket_Init(void);

void inSprocket_Exit(void);

void inSprocket_Configure(void);
void InSprocket_Flush(void);

float inSprocket_GetAllAxis(int axis);
float inSprocket_GetAxis(int axis);
float inSprocket_GetMouse(int axis);
int inSprocket_GetAxisInt(int axis);

TInputEvent inSprocket_GetButtonEvent(void);

void inSprocket_Activate(Boolean inActivate);

#endif /* __InSprocket__ */