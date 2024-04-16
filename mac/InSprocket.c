#include "fix.h"
#include "Macros.h"
#include <InputSprocket.h>
#include "InSprocket.h"
#include "InSprocketResource.h"
#include "renderer.h"
#include "game.h"
#include "descent.h"
#include "streamaudio.h"
#include "pilot.h"
#include "macapp.h"

//#ifndef DAJ_DEBUG	//REMOVE
#define USE_MOUSE
//#endif
//#define MOUSE_MAX		32768.0f
#define MOUSE_MAX 48000.0f
//#define MOUSE_MAX		65336.0f
#undef USE_KEYBOARD // NOT on you life
Boolean iSprocket_inited = false;

ISpNeed ISp_needs[kNeed_COUNT] = {
    {"\pHeading", kIconSuiteID_Yaw, 0, 0, kISpElementKind_Axis, kISpElementLabel_Axis_Yaw, kISpNeedFlag_NoMultiConfig,
     0, 0, 0},
    {"\pPitch", kIconSuiteID_Pitch, 0, 0, kISpElementKind_Axis, kISpElementLabel_Axis_Pitch, kISpNeedFlag_NoMultiConfig,
     0, 0, 0},
    {"\pBank", kIconSuiteID_Roll, 0, 0, kISpElementKind_Axis, kISpElementLabel_Axis_Roll, kISpNeedFlag_NoMultiConfig, 0,
     0, 0},
    {"\pSideways", kIconSuiteID_SlideHorz, 0, 0, kISpElementKind_Axis, kISpElementLabel_Axis_XAxis,
     kISpNeedFlag_NoMultiConfig, 0, 0, 0},
    {"\pThrust", kIconSuiteID_Thrust, 0, 0, kISpElementKind_Axis, kISpElementLabel_Axis_ZAxis,
     kISpNeedFlag_NoMultiConfig, 0, 0, 0},
    {"\pVertical", kIconSuiteID_SlideVert, 0, 0, kISpElementKind_Axis, kISpElementLabel_Axis_YAxis,
     kISpNeedFlag_NoMultiConfig, 0, 0, 0},
    // Mouse Related Input
    {"\pMouse Heading", kIconSuiteID_Yaw, 0, 0, kISpElementKind_Delta, kISpElementLabel_Delta_Yaw,
     kISpNeedFlag_NoMultiConfig, 0, 0, 0},
    {"\pMouse Pitch", kIconSuiteID_Pitch, 0, 0, kISpElementKind_Delta, kISpElementLabel_Delta_Pitch,
     kISpNeedFlag_NoMultiConfig, 0, 0, 0},
    {"\pMouse Bank", kIconSuiteID_Roll, 0, 0, kISpElementKind_Delta, kISpElementLabel_Delta_Roll,
     kISpNeedFlag_NoMultiConfig, 0, 0, 0},
    {"\pMouse Sideways", kIconSuiteID_SlideHorz, 0, 0, kISpElementKind_Delta, kISpElementLabel_Delta_X,
     kISpNeedFlag_NoMultiConfig, 0, 0, 0},
    {"\pMouse Thrust", kIconSuiteID_Thrust, 0, 0, kISpElementKind_Delta, kISpElementLabel_Delta_Z,
     kISpNeedFlag_NoMultiConfig, 0, 0, 0},
    {"\pMouse Vertical", kIconSuiteID_SlideVert, 0, 0, kISpElementKind_Delta, kISpElementLabel_Delta_Y,
     kISpNeedFlag_NoMultiConfig, 0, 0, 0},
    // The buttons
    {"\pForward", kIconSuiteID_Foward, 0, 0, kISpElementKind_Button, kISpElementLabel_None, 0, 0, 0, 0},

    {"\pReverse", kIconSuiteID_Back, 0, 0, kISpElementKind_Button, kISpElementLabel_None, 0, 0, 0, 0},
    {"\pSlide Left", kIconSuiteID_SlideLeft, 0, 0, kISpElementKind_Button, kISpElementLabel_None, 0, 0, 0, 0},
    {"\pSlide Right", kIconSuiteID_SlideRight, 0, 0, kISpElementKind_Button, kISpElementLabel_None, 0, 0, 0, 0},
    {"\pSlide Up", kIconSuiteID_SlideUp, 0, 0, kISpElementKind_Button, kISpElementLabel_None, 0, 0, 0, 0},
    {"\pSlide Down", kIconSuiteID_SlideDown, 0, 0, kISpElementKind_Button, kISpElementLabel_None, 0, 0, 0, 0},
    {"\pTurn Left", kIconSuiteID_YawL, 0, 0, kISpElementKind_Button, kISpElementLabel_None, 0, 0, 0, 0},
    {"\pTurn Right", kIconSuiteID_YawR, 0, 0, kISpElementKind_Button, kISpElementLabel_None, 0, 0, 0, 0},
    {"\pPitch Up", kIconSuiteID_PitchU, 0, 0, kISpElementKind_Button, kISpElementLabel_None, 0, 0, 0, 0},
    {"\pPitch Down", kIconSuiteID_PitchD, 0, 0, kISpElementKind_Button, kISpElementLabel_None, 0, 0, 0, 0},
    {"\pBank Left", kIconSuiteID_RollL, 0, 0, kISpElementKind_Button, kISpElementLabel_None, 0, 0, 0, 0},
    {"\pBank Right", kIconSuiteID_RollR, 0, 0, kISpElementKind_Button, kISpElementLabel_None, 0, 0, 0, 0},

    {"\p-", 0, 0, 0, kISpElementKind_Button, kISpElementLabel_None, 0, 0, 0, 0},
    // seperator
    {"\pFire Primary", kIconSuiteID_FirePrim, 0, 0, kISpElementKind_Button, kISpElementLabel_Btn_Fire, 0, 0, 0, 0},
    {"\pFire Secondary", kIconSuiteID_FireSecond, 0, 0, kISpElementKind_Button, kISpElementLabel_Btn_SecondaryFire, 0,
     0, 0, 0},
    {"\pNext Primary", kIconSuiteID_NextPrim, 0, 0, kISpElementKind_Button, kISpElementLabel_None, 0, 0, 0, 0},
    {"\pNext Secondary", kIconSuiteID_NextSecond, 0, 0, kISpElementKind_Button, kISpElementLabel_None, 0, 0, 0, 0},
    {"\pAfterburner", kIconSuiteID_AfterBurn, 0, 0, kISpElementKind_Button, kISpElementLabel_None, 0, 0, 0, 0},
    {"\pHeadlight", kIconSuiteID_HeadLight, 0, 0, kISpElementKind_Button, kISpElementLabel_None, 0, 0, 0, 0},
    {"\pFlare", kIconSuiteID_Flare, 0, 0, kISpElementKind_Button, kISpElementLabel_None, 0, 0, 0, 0},
    {"\pRear View", kIconSuiteID_RearView, 0, 0, kISpElementKind_Button, kISpElementLabel_None, 0, 0, 0, 0},
    {"\pMap", kIconSuiteID_Map, 0, 0, kISpElementKind_Button, kISpElementLabel_None, 0, 0, 0, 0},
    {"\pUse Inventory", kIconSuiteID_UseInv, 0, 0, kISpElementKind_Button, kISpElementLabel_None, 0, 0, 0, 0},
    {"\pNext Inventory", kIconSuiteID_NextInv, 0, 0, kISpElementKind_Button, kISpElementLabel_None, 0, 0, 0, 0},
    {"\pPrevious Inventory", kIconSuiteID_PrevInv, 0, 0, kISpElementKind_Button, kISpElementLabel_None, 0, 0, 0, 0},
    {"\pUse Counter Measure", kIconSuiteID_UseCM, 0, 0, kISpElementKind_Button, kISpElementLabel_None, 0, 0, 0, 0},
    {"\pNext Counter Measure", kIconSuiteID_NextCM, 0, 0, kISpElementKind_Button, kISpElementLabel_None, 0, 0, 0, 0},
    {"\pPrevious Counter Measure", kIconSuiteID_PrevCM, 0, 0, kISpElementKind_Button, kISpElementLabel_None, 0, 0, 0,
     0},
    {"\pBank On", kIconSuiteID_BankOn, 0, 0, kISpElementKind_Button, kISpElementLabel_None, 0, 0, 0, 0},
    {"\pSlide On", kIconSuiteID_SlideOn, 0, 0, kISpElementKind_Button, kISpElementLabel_None, 0, 0, 0, 0}};
void inSprocket_Init(void) {
  int i;

  if (iSprocket_inited)
    return;

  mprintf((1, "inSprocket_init: need_count %d\n", kNeed_COUNT));

#ifdef USE_MOUSE
  ISpDevices_ActivateClass(kISpDeviceClass_Mouse);
#endif
#ifdef USE_KEYBOARD
  ISpDevices_ActivateClass(kISpDeviceClass_Keyboard);
#endif
  ISpElement_NewVirtualFromNeeds(kNeed_COUNT, ISp_needs, gInputElement, 0);

  char pilot_name[40];
  Current_pilot.get_name(pilot_name);
  if (pilot_name[0])
    ISpInit(kNeed_COUNT, ISp_needs, gInputElement, 'DNT3', (ulong)pilot_name, 0, kSetListID, 0);
  else
    ISpInit(kNeed_COUNT, ISp_needs, gInputElement, 'DNT3', kNeedsVersion, 0, kSetListID, 0);

  ISpElementList_New(0, NULL, &gInputEventList, 0);
  ISpElementList_New(0, NULL, &gInputHoldDownEventList, 0);
  ISpElementList_AddElements(gInputHoldDownEventList, kInputEvent_FirePrim_On, 1, &gInputElement[kNeed_FirePrim]);
  ISpElementList_AddElements(gInputHoldDownEventList, kInputEvent_FireSecond_On, 1, &gInputElement[kNeed_FireSecond]);
  ISpElementList_AddElements(gInputHoldDownEventList, kInputEvent_AfterBurn_On, 1, &gInputElement[kNeed_AfterBurn]);
  ISpElementList_AddElements(gInputHoldDownEventList, kInputEvent_RearView_On, 1, &gInputElement[kNeed_RearView]);
  ISpElementList_AddElements(gInputHoldDownEventList, kInputEvent_Bank_On, 1, &gInputElement[kNeed_BankOn]);
  ISpElementList_AddElements(gInputHoldDownEventList, kInputEvent_Slide_On, 1, &gInputElement[kNeed_SlideOn]);

  ISpElementList_AddElements(gInputHoldDownEventList, kInputEvent_Accelerate_On, 1, &gInputElement[kNeed_Accelerate]);
  ISpElementList_AddElements(gInputHoldDownEventList, kInputEvent_Reverse_On, 1, &gInputElement[kNeed_Reverse]);
  ISpElementList_AddElements(gInputHoldDownEventList, kInputEvent_SlideLeft_On, 1, &gInputElement[kNeed_SlideLeft]);
  ISpElementList_AddElements(gInputHoldDownEventList, kInputEvent_SlideRight_On, 1, &gInputElement[kNeed_SlideRight]);
  ISpElementList_AddElements(gInputHoldDownEventList, kInputEvent_SlideUp_On, 1, &gInputElement[kNeed_SlideUp]);
  ISpElementList_AddElements(gInputHoldDownEventList, kInputEvent_SlideDown_On, 1, &gInputElement[kNeed_SlideDown]);

  ISpElementList_AddElements(gInputHoldDownEventList, kInputEvent_TurnLeft_On, 1, &gInputElement[kNeed_TurnLeft]);
  ISpElementList_AddElements(gInputHoldDownEventList, kInputEvent_TurnRight_On, 1, &gInputElement[kNeed_TurnRight]);
  ISpElementList_AddElements(gInputHoldDownEventList, kInputEvent_PitchUp_On, 1, &gInputElement[kNeed_PitchUp]);
  ISpElementList_AddElements(gInputHoldDownEventList, kInputEvent_PitchDown_On, 1, &gInputElement[kNeed_PitchDown]);
  ISpElementList_AddElements(gInputHoldDownEventList, kInputEvent_BankLeft_On, 1, &gInputElement[kNeed_BankLeft]);
  ISpElementList_AddElements(gInputHoldDownEventList, kInputEvent_BankRight_On, 1, &gInputElement[kNeed_BankRight]);
  ISpElementList_AddElements(gInputEventList, kInputEvent_NextPrim, 1, &gInputElement[kNeed_NextPrim]);
  ISpElementList_AddElements(gInputEventList, kInputEvent_NextSecond, 1, &gInputElement[kNeed_NextSecond]);
  ISpElementList_AddElements(gInputEventList, kInputEvent_HeadLight, 1, &gInputElement[kNeed_HeadLight]);
  ISpElementList_AddElements(gInputEventList, kInputEvent_Flare, 1, &gInputElement[kNeed_Flare]);
  ISpElementList_AddElements(gInputEventList, kInputEvent_Map, 1, &gInputElement[kNeed_Map]);
  ISpElementList_AddElements(gInputEventList, kInputEvent_InventoryUse, 1, &gInputElement[kNeed_InventoryUse]);
  ISpElementList_AddElements(gInputEventList, kInputEvent_InventoryNext, 1, &gInputElement[kNeed_InventoryNext]);
  ISpElementList_AddElements(gInputEventList, kInputEvent_InventoryPrev, 1, &gInputElement[kNeed_InventoryPrev]);
  ISpElementList_AddElements(gInputEventList, kInputEvent_CounterUse, 1, &gInputElement[kNeed_CounterUse]);
  ISpElementList_AddElements(gInputEventList, kInputEvent_CounterNext, 1, &gInputElement[kNeed_CounterNext]);
  ISpElementList_AddElements(gInputEventList, kInputEvent_CounterPrev, 1, &gInputElement[kNeed_CounterPrev]);
  /*
          ISpElementList_AddElements(gInputEventList, kInputEvent_ATaunt1, 	1, &gInputElement[kNeed_ATaunt1]);
          ISpElementList_AddElements(gInputEventList, kInputEvent_ATaunt2, 	1, &gInputElement[kNeed_ATaunt2]);
          ISpElementList_AddElements(gInputEventList, kInputEvent_ATaunt3, 	1, &gInputElement[kNeed_ATaunt3]);
          ISpElementList_AddElements(gInputEventList, kInputEvent_ATaunt4, 	1, &gInputElement[kNeed_ATaunt4]);
  */
  ISpElementList_Flush(gInputEventList);
  ISpElementList_Flush(gInputHoldDownEventList);
  for (i = 0; i < 12; i++)
    ISpElement_Flush(gInputElement[i]);

  ISpSuspend();

  iSprocket_inited = true;
}
void inSprocket_Configure(void) {
#if defined USE_GLIDE
  rend_Close();
#elif defined USE_OPENGL
  PauseDSpContext();
#endif

  AudioStream::PauseAll();

  inSprocket_Init(); // Just incase the config gets called before noral init

  ISpConfigure(NULL);

  AudioStream::ResumeAll();
#ifdef USE_MOUSE
  HideCursor();
#endif
#if defined USE_GLIDE
  rend_Init(PreferredRenderer, Descent, &Render_preferred_state);
#elif defined USE_OPENGL
  ResumeDSpContext();
#endif
}
void inSprocket_Activate(Boolean inActivate) {
  Boolean doActivate;
  int i;

  doActivate = inActivate;
  if (gInputActive != doActivate) {
    gInputActive = doActivate;

    if (gInputActive) {
#ifdef USE_MOUSE
      HideCursor();
#endif

      ISpResume();
    } else {
      ISpSuspend();

#ifdef USE_MOUSE
      //		 		ShowCursor();
#endif
    }
  }
}

void InSprocket_Flush(void) {
  ISpElementList_Flush(gInputEventList);
  ISpElementList_Flush(gInputHoldDownEventList);
  for (int i = 0; i < 12; i++)
    ISpElement_Flush(gInputElement[i]);
}

float inSprocket_GetAllAxis(int axis) {
  ISpAxisData axisValue = kISpAxisMiddle;
  OSStatus err;
  ISpElementEvent event;
  Boolean gotEvent;
  float val = 0, fval;
  signed int maxval = 0;

  while (!(err = ISpElement_GetNextEvent(gInputElement[axis + 6], sizeof(event), &event, &gotEvent)) && gotEvent) {
    maxval += (signed int)event.data;
  }
  if (maxval)
    return maxval / MOUSE_MAX;
  err = ISpElement_GetNextEvent(gInputElement[axis], sizeof(event), &event, &gotEvent);
  if (!err && gotEvent) {
    return ISpSymmetricAxisToFloat(event.data);
  }
  err = ISpElement_GetSimpleState(gInputElement[axis], &axisValue);
  if (!err) {
    return ISpSymmetricAxisToFloat(axisValue);
  }
  return val;
}
float inSprocket_GetMouse(int axis) {
  OSStatus err;
  ISpElementEvent event;
  Boolean gotEvent = false;
  float val = 0;
  signed int maxval = 0;

  err = ISpElement_GetNextEvent(gInputElement[axis + 6], sizeof(event), &event, &gotEvent);
  while (!err && gotEvent) {
    maxval += (signed int)event.data;
    err = ISpElement_GetNextEvent(gInputElement[axis + 6], sizeof(event), &event, &gotEvent);
  }
  if (maxval)
    return val = maxval / MOUSE_MAX;
  return val;
}
float inSprocket_GetAxis(int axis) {
  ISpAxisData axisValue = kISpAxisMiddle;
  OSStatus err;
  float val = 0;
  ISpElementEvent event;
  Boolean gotEvent;

  if (!(err = ISpElement_GetNextEvent(gInputElement[axis], sizeof(event), &event, &gotEvent)) && gotEvent) {
    val = ISpSymmetricAxisToFloat(event.data);
  } else if (!(err = ISpElement_GetSimpleState(gInputElement[axis], &axisValue))) {
    val = ISpSymmetricAxisToFloat(axisValue);
  }
  return val;
}
int inSprocket_GetAxisInt(int axis) {
  ISpAxisData axisValue = kISpAxisMiddle;
  OSStatus err;
  int val = 0;
  ISpElementEvent event;
  Boolean gotEvent;

  if (!(err = ISpElement_GetSimpleState(gInputElement[axis], &axisValue))) {
    val = (int)((axisValue >> 24) - 0x7F);
  }
  return val;
}
TInputEvent inSprocket_GetButtonEvent(void) {
  OSErr err;
  //	TInputEvent			result = kInputEvent_None;
  int result = kInputEvent_None;
  ISpElementEvent event;
  Boolean gotEvent;

  if (gInputActive) {
    err = ISpElementList_GetNextEvent(gInputHoldDownEventList, sizeof(event), &event, &gotEvent);
    if (err == noErr && gotEvent) {
      result = (TInputEvent)event.refCon;
      if (event.data == kISpButtonUp)
        result += 1; // Note: we rely on off being on+1 (ie kInputEvent_InertialDampers_Off ==
                     // kInputEvent_InertialDampers_On + 1)
    } else {
      err = ISpElementList_GetNextEvent(gInputEventList, sizeof(event), &event, &gotEvent);

      if (err == noErr && gotEvent && event.data == kISpButtonDown)
        result = (TInputEvent)event.refCon;
    }
  }

  return (TInputEvent)result;
}
void inSprocket_Exit(void) {
  if (gInputActive) {
    inSprocket_Activate(false);
  }
  ISpElementList_Dispose(gInputEventList);
  ISpStop();
  //??	ISpElement_DisposeVirtual(kNeed_COUNT, gInputElement);
}
