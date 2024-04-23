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
 * $Logfile: /DescentIII/Main/ddio_mac/macController.cpp $
 * $Revision: 1.1.1.1 $
 * $Date: 2003/08/26 03:56:55 $
 * $Author: kevinb $
 *
 * Keyboard IO for macintosh
 *
 * $Log: macController.cpp,v $
 * Revision 1.1.1.1  2003/08/26 03:56:55  kevinb
 * initial 1.5 import
 *
 *
 * 5     4/12/00 6:52p Matt
 * From Duane for 1.4: deadzone stuff
 *
 * 4     3/20/00 12:43p Matt
 * Merge of Duane's post-1.3 changes.
 *
 * 3     10/21/99 3:33p Jeff
 * Macintosh merges
 *
 * 2     7/28/99 3:31p Kevin
 * Mac Stuff!
 *
 * 4     5/21/97 5:17 PM Jeremy
 * switch keyboard controls to default values of PC (still need to
 * implement needs/controls structure)
 *
 * 3     5/15/97 1:43 AM Jeremy
 * changed mprintf's to be standard (with newline at end)
 *
 * 2     5/13/97 11:45 AM Jeremy
 * implementation  for gameMacController object as well as create/destroy
 * controller object (still stubbed out)
 *
 * 1     5/11/97 8:09 PM Jeremy
 * Initial check in
 *
 * $NoKeywords: $
 */
// ----------------------------------------------------------------------------
//	Keyboard Interface
// ----------------------------------------------------------------------------
// ANSI Headers
#include <stdlib.h>
#include <math.h>

// Macintosh Headers
#include <Events.h>
#include <InputSprocket.h>
#include "InSprocket.h"
// Descent 3 Headers
#include "pserror.h"
#include "mono.h"
#include "macController.h"
#include "Controller.h"
#include "controls.h"
#include "ddio_common.h"
#include "ddio.h"
#include "hud.h"

#define SIGN(a) ((a < 0) ? -1 : 1)

typedef struct {
  ubyte pcAction;
  ubyte isEvent;
} PC_IS_Map;
PC_IS_Map m_pc_is_map[] = {{ctfFORWARD_THRUSTAXIS, kInputEvent_None},
                           {ctfFORWARD_THRUSTKEY, kInputEvent_None},
                           {ctfREVERSE_THRUSTKEY, kInputEvent_None},
                           {ctfUP_THRUSTAXIS, kInputEvent_None},
                           {ctfUP_THRUSTKEY, kInputEvent_None},
                           {ctfDOWN_THRUSTKEY, kInputEvent_None},
                           {ctfRIGHT_THRUSTAXIS, kInputEvent_None},
                           {ctfRIGHT_THRUSTKEY, kInputEvent_None},
                           {ctfLEFT_THRUSTKEY, kInputEvent_None},
                           {ctfPITCH_DOWNAXIS, kInputEvent_None},
                           {ctfPITCH_DOWNKEY, kInputEvent_None},
                           {ctfPITCH_UPKEY, kInputEvent_None},
                           {ctfBANK_RIGHTAXIS, kInputEvent_None},
                           {ctfBANK_RIGHTKEY, kInputEvent_None},
                           {ctfBANK_LEFTKEY, kInputEvent_None},
                           {ctfHEADING_RIGHTAXIS, kInputEvent_None},
                           {ctfHEADING_RIGHTKEY, kInputEvent_None},
                           {ctfHEADING_LEFTKEY, kInputEvent_None},
                           {ctfFIREPRIMARY_BUTTON, kInputEvent_FirePrim_On},
                           {ctfFIREPRIMARY_KEY, kInputEvent_None},
                           {ctfFIREPRIMARY_KEY2, kInputEvent_None},
                           {ctfFIRESECONDARY_BUTTON, kInputEvent_FireSecond_On},
                           {ctfFIRESECONDARY_KEY, kInputEvent_None},
                           {ctfTOGGLE_SLIDEKEY, kInputEvent_None},
                           {ctfTOGGLE_SLIDEBUTTON, kInputEvent_Slide_On},
                           {ctfFIREFLARE_KEY, kInputEvent_None},
                           {ctfFIREFLARE_BUTTON, kInputEvent_Flare},
                           {ctfUP_BUTTON, kInputEvent_SlideUp_On},
                           {ctfDOWN_BUTTON, kInputEvent_SlideDown_On},
                           {ctfLEFT_BUTTON, kInputEvent_SlideLeft_On},
                           {ctfRIGHT_BUTTON, kInputEvent_SlideRight_On},
                           {ctfAFTERBURN_KEY, kInputEvent_None},
                           {ctfAFTERBURN_BUTTON, kInputEvent_AfterBurn_On},
                           {ctfFORWARD_BUTTON, kInputEvent_Accelerate_On},
                           {ctfREVERSE_BUTTON, kInputEvent_Reverse_On},
                           {ctfTOGGLE_BANKKEY, kInputEvent_None},
                           {ctfTOGGLE_BANKBUTTON, kInputEvent_Bank_On},
                           {ctfHEADING_LEFTBUTTON, kInputEvent_TurnLeft_On},
                           {ctfHEADING_RIGHTBUTTON, kInputEvent_TurnRight_On},
                           {ctfPITCH_UPBUTTON, kInputEvent_PitchUp_On},
                           {ctfPITCH_DOWNBUTTON, kInputEvent_PitchDown_On},
                           {ctfBANK_LEFTBUTTON, kInputEvent_BankLeft_On},
                           {ctfBANK_RIGHTBUTTON, kInputEvent_BankRight_On},
                           {ctfAUTOMAP_KEY, kInputEvent_None},
                           {ctfPREV_INVKEY, kInputEvent_None},
                           {ctfNEXT_INVKEY, kInputEvent_None},
                           {ctfINV_USEKEY, kInputEvent_None},
                           {ctfPREV_CNTMSKEY, kInputEvent_None},
                           {ctfNEXT_CNTMSKEY, kInputEvent_None},
                           {ctfCNTMS_USEKEY, kInputEvent_None},
                           {ctfHEADLIGHT_KEY, kInputEvent_None},
                           {ctfHEADLIGHT_BUTTON, kInputEvent_HeadLight},
                           {ctfAUTOMAP_BUTTON, kInputEvent_Map},
                           {ctfPREV_INVBTN, kInputEvent_InventoryPrev},
                           {ctfNEXT_INVBTN, kInputEvent_InventoryNext},
                           {ctfINV_USEBTN, kInputEvent_InventoryUse},
                           {ctfPREV_CNTMSBTN, kInputEvent_CounterPrev},
                           {ctfNEXT_CNTMSBTN, kInputEvent_CounterNext},
                           {ctfCNTMS_USEBTN, kInputEvent_CounterUse},
                           {ctfWPNSEL_PCYCLEKEY, kInputEvent_None},
                           {ctfWPNSEL_PCYCLEBTN, kInputEvent_NextPrim},
                           {ctfWPNSEL_SCYCLEKEY, kInputEvent_None},
                           {ctfWPNSEL_SCYCLEBTN, kInputEvent_NextSecond},
                           {ctfREARVIEW_KEY, kInputEvent_None},
                           {ctfREARVIEW_BTN, kInputEvent_RearView_On},
                           {ctfAUDIOTAUNT1_KEY, kInputEvent_None},
                           {ctfAUDIOTAUNT1_BTN, kInputEvent_ATaunt1},
                           {ctfAUDIOTAUNT2_KEY, kInputEvent_None},
                           {ctfAUDIOTAUNT2_BTN, kInputEvent_ATaunt2},
                           {ctfAUDIOTAUNT3_KEY, kInputEvent_None},
                           {ctfAUDIOTAUNT3_BTN, kInputEvent_ATaunt3},
                           {ctfAUDIOTAUNT4_KEY, kInputEvent_None},
                           {ctfAUDIOTAUNT4_BTN, kInputEvent_ATaunt4}};
inline ubyte ConvertPC_IS(ubyte action) {
  if (m_pc_is_map[action].pcAction == action) {
    return m_pc_is_map[action].isEvent;
  } else {
    for (int i = 0; i < NUM_CONTROLLER_FUNCTIONS; i++) {
      if (m_pc_is_map[i].pcAction == action) {
        return m_pc_is_map[i].isEvent;
      }
    }
  }
  return 0;
}

struct {
  float DownTime;
  float UpTime;
  bool state;
} iSbutton[kNumEvents];

gameMacController::gameMacController(int num_funcs, ct_function *gameFunctions, char *remote_adr)
    : gameController(num_funcs, gameFunctions) {
  for (int i = 0; i < kNumEvents; i++) {
    iSbutton[i].DownTime = 0.0;
    iSbutton[i].UpTime = 0.0;
    iSbutton[i].state = false;
  }
  //	inSprocket_Init();

  enum_controllers(remote_adr);
  for (int i = 0; i < num_funcs; i++)
    assign_function(&gameFunctions[i]);
  m_Suspended = 0;
  //	m_frame_timer_ms = -1;
  //	m_frame_time = 1.0f;
  //	m_ControlList[MC_JOY].deadzone = 8;

  gameMacController::flush();
}
gameMacController::~gameMacController(void) {
  //	inSprocket_Exit();
}
//	these functions suspend or resume any controller reading.  this is really only useful for
//	preemptive controller polling, but they should be used to activate and deactivate controller
//	reading.
void gameMacController::suspend() {
  inSprocket_Activate(false);

  for (int i = 0; i < kNumEvents; i++) {
    iSbutton[i].DownTime = 0.0;
    iSbutton[i].UpTime = 0.0;
    iSbutton[i].state = false;
  }
}
void gameMacController::flush() {
  InSprocket_Flush();
  for (int i = 0; i < kNumEvents; i++) {
    iSbutton[i].DownTime = 0;
    iSbutton[i].UpTime = 0;
    iSbutton[i].state = false;
  }
}

void gameMacController::resume() { inSprocket_Activate(true); }
//	this functions polls the controllers if needed.  some systems may not need to implement
//	this function.
#define IS_MOUSE 0
#define IS_JOY 1

float is_mouse[6];
int is_joy[6];
void MacInSprocketHandler(void) {
  TInputEvent iSevent;

  float cur_time = timer_GetTime();

  is_mouse[kNeed_Yaw] = inSprocket_GetMouse(kNeed_Yaw);
  is_mouse[kNeed_Pitch] = inSprocket_GetMouse(kNeed_Pitch);
  is_mouse[kNeed_Roll] = inSprocket_GetMouse(kNeed_Roll);
  is_mouse[kNeed_MoveX] = inSprocket_GetMouse(kNeed_MoveX);
  is_mouse[kNeed_MoveZ] = inSprocket_GetMouse(kNeed_MoveZ);
  is_mouse[kNeed_MoveY] = inSprocket_GetMouse(kNeed_MoveY);
  is_joy[kNeed_Yaw] = inSprocket_GetAxisInt(kNeed_Yaw);
  is_joy[kNeed_Pitch] = inSprocket_GetAxisInt(kNeed_Pitch);
  is_joy[kNeed_Roll] = inSprocket_GetAxisInt(kNeed_Roll);
  is_joy[kNeed_MoveX] = inSprocket_GetAxisInt(kNeed_MoveX);
  is_joy[kNeed_MoveZ] = inSprocket_GetAxisInt(kNeed_MoveZ);
  is_joy[kNeed_MoveY] = inSprocket_GetAxisInt(kNeed_MoveY);

  while ((iSevent = inSprocket_GetButtonEvent()) != kInputEvent_None) {
    switch (iSevent) {
    case kInputEvent_Accelerate_On:
    case kInputEvent_Reverse_On:
    case kInputEvent_SlideLeft_On:
    case kInputEvent_SlideRight_On:
    case kInputEvent_SlideUp_On:
    case kInputEvent_SlideDown_On:
    case kInputEvent_BankLeft_On:
    case kInputEvent_BankRight_On:
    case kInputEvent_TurnLeft_On:
    case kInputEvent_TurnRight_On:
    case kInputEvent_PitchUp_On:
    case kInputEvent_PitchDown_On:
    case kInputEvent_FirePrim_On:
    case kInputEvent_FireSecond_On:
      iSbutton[iSevent].state = true;
      if (iSbutton[iSevent].DownTime == 0.0)
        iSbutton[iSevent].DownTime = cur_time;
      break;

    case kInputEvent_Accelerate_Off:
    case kInputEvent_Reverse_Off:
    case kInputEvent_SlideLeft_Off:
    case kInputEvent_SlideRight_Off:
    case kInputEvent_SlideUp_Off:
    case kInputEvent_SlideDown_Off:
    case kInputEvent_BankLeft_Off:
    case kInputEvent_BankRight_Off:
    case kInputEvent_TurnLeft_Off:
    case kInputEvent_TurnRight_Off:
    case kInputEvent_PitchUp_Off:
    case kInputEvent_PitchDown_Off:
    case kInputEvent_FirePrim_Off:
    case kInputEvent_FireSecond_Off:
      iSbutton[iSevent - 1].state = false;
      iSbutton[iSevent - 1].UpTime = cur_time;
      break;

    case kInputEvent_CounterUse:
    case kInputEvent_CounterNext:
    case kInputEvent_CounterPrev:
    case kInputEvent_InventoryUse:
    case kInputEvent_InventoryNext:
    case kInputEvent_InventoryPrev:
    case kInputEvent_NextPrim:
    case kInputEvent_NextSecond:
    case kInputEvent_Map:
    case kInputEvent_Flare:
    case kInputEvent_HeadLight:
    case kInputEvent_Bank_On:
    case kInputEvent_Slide_On:
    case kInputEvent_AfterBurn_On:
    case kInputEvent_RearView_On:
      iSbutton[iSevent].state = true;
      break;
    case kInputEvent_Bank_Off:
    case kInputEvent_Slide_Off:
    case kInputEvent_AfterBurn_Off:
    case kInputEvent_RearView_Off:
      iSbutton[iSevent - 1].state = false;
      break;
    }
  }
}
void gameMacController::poll() {
  //	TInputEvent	iSevent;
  //	extern float Frametime;
  /*
  float cur_time = timer_GetTime();

//	while((iSevent = inSprocket_GetEvent()) != kInputEvent_None) {
  for(int i = 0; i < kNumEvents; i++) {
          switch(i) {
                  case kInputEvent_SlideLeft_On:
                  mprintf((1, "ln %f %f\n", iSbutton[i].UpTime, iSbutton[i].DownTime));
                  break;
          }
          switch(i) {
                  case kInputEvent_EnergyShield_Off:
                  case kInputEvent_Accelerate_Off:
                  case kInputEvent_Reverse_Off:
                  case kInputEvent_SlideLeft_Off:
                  case kInputEvent_SlideRight_Off:
                  case kInputEvent_SlideUp_Off:
                  case kInputEvent_SlideDown_Off:
                  case kInputEvent_BankLeft_Off:
                  case kInputEvent_BankRight_Off:
                  case kInputEvent_TurnLeft_Off:
                  case kInputEvent_TurnRight_Off:
                  case kInputEvent_PitchUp_Off:
                  case kInputEvent_PitchDown_Off:
                          iSbutton[i-1].UpTime = cur_time;
                  break;
                  case kInputEvent_Flare:
                  case kInputEvent_Bomb:
                  case kInputEvent_Map:
                  case kInputEvent_NextPrim:
                  case kInputEvent_PrevPrim:
                  case kInputEvent_NextSecond:
                  case kInputEvent_PrevSecond:
                  case kInputEvent_HeadLight:
                  case kInputEvent_Bank_On:
                  case kInputEvent_Slide_On:
                  case kInputEvent_FirePrim_On:
                  case kInputEvent_FireSecond_On:
                  case kInputEvent_AfterBurn_On:
                  case kInputEvent_RearView_On:
                  case kInputEvent_CounterPrev:
                          if(iSbutton[i].DownTime != 0.0)
                                  iSbutton[i].DeltaTime = 1.0;

                          if(iSbutton[i].UpTime) {
                                  iSbutton[i].DownTime = iSbutton[i].UpTime = 0.0;
                          }
                  break;

                  case kInputEvent_Accelerate_On:
                  case kInputEvent_Reverse_On:
                  case kInputEvent_SlideLeft_On:
                  case kInputEvent_BankRight_On:
                  case kInputEvent_TurnRight_On:
                  case kInputEvent_SlideDown_On:
                  case kInputEvent_PitchUp_On:
                  case kInputEvent_SlideRight_On:
                  case kInputEvent_BankLeft_On:
                  case kInputEvent_TurnLeft_On:
                  case kInputEvent_SlideUp_On:
                  case kInputEvent_PitchDown_On:
                          if(iSbutton[i].DownTime != 0.0) {
                                  iSbutton[i].DeltaTime = cur_time - iSbutton[i].DownTime;
                          }
                  break;

                  case kInputEvent_Bank_Off:
                  case kInputEvent_Slide_Off:

                  case kInputEvent_FirePrim_Off:
                  case kInputEvent_FireSecond_Off:
                  case kInputEvent_AfterBurn_Off:
                  case kInputEvent_RearView_Off:
                  case kInputEvent_InventoryPrev:
                  case kInputEvent_CounterNext:
                  case kInputEvent_EnergyShield_Off:
                  case kInputEvent_Accelerate_Off:
                  case kInputEvent_Reverse_Off:
                  case kInputEvent_SlideLeft_Off:
                  case kInputEvent_SlideRight_Off:
                  case kInputEvent_SlideUp_Off:
                  case kInputEvent_SlideDown_Off:
                  case kInputEvent_BankLeft_Off:
                  case kInputEvent_BankRight_Off:
                  case kInputEvent_TurnLeft_Off:
                  case kInputEvent_TurnRight_Off:
                  case kInputEvent_PitchUp_Off:
                  case kInputEvent_PitchDown_Off:
                          if(iSbutton[i-1].DownTime != 0.0) {
                                  if(iSbutton[i-1].UpTime != 0)
                                          iSbutton[i-1].DeltaTime = iSbutton[i-1].UpTime - iSbutton[i-1].DownTime;
                                  iSbutton[i-1].UpTime = iSbutton[i-1].DownTime = 0.0;
                          } else {
                                  iSbutton[i-1].DeltaTime = 0.0;
                          }
                  break;
          }
  }
*/
}
//	all systems need to implement this function.  this returns information about the controller
bool gameMacController::get_packet(int id, ct_packet *packet, ct_format alt_format) {
  bool success = false;
  float val = (float)0.0;
  packet->flags = 0;
  int i;
  packet->format = (alt_format != ctNoFormat) ? alt_format : m_ElementList[id].format;
  alt_format = packet->format;
  if (!m_ElementList[id].enabled) {
    return success;
  }
  if (m_ElementList[id].ctype[0] == ctKey)
    val += get_key_value(m_ElementList[id].value[0], alt_format);
  if (m_ElementList[id].ctype[1] == ctKey)
    val += get_key_value(m_ElementList[id].value[1], alt_format);

  if (m_ElementList[id].ctype[0] == ctButton)
    val += get_button_value(id);
  if (m_ElementList[id].ctype[1] == ctButton)
    val += get_button_value(id);

  if (m_ElementList[id].ctype[0] == ctAxis)
    val += get_axis_value(0, m_ElementList[id].value[0], alt_format, false);

  if (val)
    packet->flags |= CTPK_ELEMENTACTIVE;
  packet->value = val;

  return success;
}
gameController *CreateController(int num_funcs, ct_function *gameFunctions, char *remote_ip) {
  mprintf((0, "Creating mac game controller.\n"));

  return new gameMacController(num_funcs, gameFunctions);
}
void DestroyController(gameController *ctl) {
  mprintf((0, "Destroying mac game controller.\n"));

  if (ctl)
    delete ctl;
}
//	temporarily enables or disables a function
void gameMacController::enable_function(int id, bool enable) { m_ElementList[id].enabled = enable; }
//	returns information about a requested function
void gameMacController::get_controller_function(int id, ct_type *type, ct_config_data *value, ubyte *flags) {
  type[0] = m_ElementList[id].ctype[0];
  type[1] = m_ElementList[id].ctype[1];
  *value = makeword(CONTROLLER_CTL_INFO(m_ElementList[id].ctl[0], m_ElementList[id].ctl[1]),
                    CONTROLLER_CTL_VALUE(m_ElementList[id].value[0], m_ElementList[id].value[1]));
  flags[0] = m_ElementList[id].flags[0];
  flags[1] = m_ElementList[id].flags[1];
}
//	sets the configuration of a function
void gameMacController::set_controller_function(int id, const ct_type *type, ct_config_data value, const ubyte *flags) {
  ct_element elem;
  if (id >= CT_MAX_ELEMENTS)
    return;
  // auto assign keyboard controller if type is key.
  if (type[0] == ctKey)
    elem.ctl[0] = CONTROLLER_CTL1_INFO(0);
  else
    elem.ctl[0] = CONTROLLER_CTL1_INFO(CONTROLLER_INFO(value));
  if (type[1] == ctKey)
    elem.ctl[1] = CONTROLLER_CTL2_INFO(0);
  else
    elem.ctl[1] = CONTROLLER_CTL2_INFO(CONTROLLER_INFO(value));
  elem.ctype[0] = type[0];
  elem.ctype[1] = type[1];
  elem.format = m_ElementList[id].format;
  elem.value[0] = CONTROLLER_CTL1_VALUE(CONTROLLER_VALUE(value));
  elem.value[1] = CONTROLLER_CTL2_VALUE(CONTROLLER_VALUE(value));
  elem.flags[0] = flags[0];
  elem.flags[1] = flags[1];
  elem.enabled = m_ElementList[id].enabled;
  //	if controller doesn't exist, set it to invalid.
  if (elem.ctl[0] > CT_MAX_CONTROLLERS)
    elem.ctl[0] = NULL_WINCONTROLLER;
  if (elem.ctl[1] >= CT_MAX_CONTROLLERS)
    elem.ctl[1] = NULL_WINCONTROLLER;
  assign_element(id, &elem);
}
#define TABLE_LOOKUP
//	note controller is index into ControlList.
float gameMacController::get_axis_value(sbyte controller, ubyte axis, ct_format format, bool invert) {
  float axisval = 0.0;

  switch (axis) {
    //	note we take care of mouse controls and external controls here
  case CT_X_AXIS:
    axisval += is_mouse[kNeed_Yaw];
    if (ABS(is_joy[kNeed_Yaw]) > m_ControlList[MC_JOY].deadzone && m_ControlList[MC_JOY].sens[AXIS_YAW] != 0.0) {
#if defined TABLE_LOOKUP
      axisval += SIGN(is_joy[kNeed_Yaw]) * m_ControlList[MC_JOY].sens_curve[AXIS_YAW][ABS(is_joy[kNeed_Yaw])];
#elif defined REAL_POW
      if (m_ControlList[MC_JOY].sens[AXIS_YAW] == 1.0)
        axisval += is_joy[kNeed_Yaw];
      else
        axisval += powf(ABS(is_joy[kNeed_Yaw]), m_ControlList[MC_JOY].inv_sens[AXIS_YAW]) * SIGN(is_joy[kNeed_Yaw]);
#else
      axisval += is_joy[kNeed_Yaw] * m_ControlList[MC_JOY].sens[AXIS_YAW];
#endif
    }
    break;

  case CT_Y_AXIS:
    axisval -= is_mouse[kNeed_Pitch];
    if (ABS(is_joy[kNeed_Pitch]) > m_ControlList[MC_JOY].deadzone && m_ControlList[MC_JOY].sens[AXIS_PITCH] != 0.0) {
#if defined TABLE_LOOKUP
      axisval -=
          SIGN(is_joy[kNeed_Pitch]) * m_ControlList[MC_JOY].sens_curve[AXIS_PITCH][ABS((int)(is_joy[kNeed_Pitch]))];
#elif defined REAL_POW
      if (m_ControlList[MC_JOY].sens[AXIS_PITCH] == 1.0)
        axisval -= is_joy[kNeed_Pitch];
      else
        axisval -=
            powf(ABS(is_joy[kNeed_Pitch]), m_ControlList[MC_JOY].inv_sens[AXIS_PITCH]) * SIGN(is_joy[kNeed_Pitch]);
#else
      axisval -= is_joy[kNeed_Pitch] * m_ControlList[MC_JOY].sens[AXIS_PITCH];
#endif
    }
    break;

  case CT_R_AXIS:
    axisval += is_mouse[kNeed_Roll];
    if (ABS(is_joy[kNeed_Roll]) > m_ControlList[MC_JOY].deadzone && m_ControlList[MC_JOY].sens[AXIS_ROLL] != 0.0) {
#if defined TABLE_LOOKUP
      axisval += SIGN(is_joy[kNeed_Roll]) * m_ControlList[MC_JOY].sens_curve[AXIS_ROLL][ABS((int)(is_joy[kNeed_Roll]))];
#elif defined REAL_POW
      if (m_ControlList[MC_JOY].sens[AXIS_ROLL] == 1.0)
        axisval += is_joy[kNeed_Roll];
      else
        axisval += powf(ABS(is_joy[kNeed_Roll]), m_ControlList[MC_JOY].inv_sens[AXIS_ROLL]) * SIGN(is_joy[kNeed_Roll]);
#else
      axisval += is_joy[kNeed_Roll] * m_ControlList[MC_JOY].sens[AXIS_ROLL];
#endif
    }
    break;

  case CT_Z_AXIS:
    axisval -= is_mouse[kNeed_MoveZ];
    if (ABS(is_joy[kNeed_MoveZ]) > m_ControlList[MC_JOY].deadzone && m_ControlList[MC_JOY].sens[AXIS_FORWARD] != 0.0) {
#if defined TABLE_LOOKUP
      axisval -=
          SIGN(is_joy[kNeed_MoveZ]) * m_ControlList[MC_JOY].sens_curve[AXIS_FORWARD][ABS((int)(is_joy[kNeed_MoveZ]))];
#elif defined REAL_POW
      if (m_ControlList[MC_JOY].sens[AXIS_FORWARD] == 1.0)
        axisval -= is_joy[kNeed_MoveZ];
      else
        axisval -=
            powf(ABS(is_joy[kNeed_MoveZ]), m_ControlList[MC_JOY].inv_sens[AXIS_FORWARD]) * SIGN(is_joy[kNeed_MoveZ]);
#else
      axisval -= is_joy[kNeed_MoveZ] * m_ControlList[MC_JOY].sens[AXIS_FORWARD];
#endif
    }
    break;

  case CT_U_AXIS:
    axisval += is_mouse[kNeed_MoveX];
    if (ABS(is_joy[kNeed_MoveX]) > m_ControlList[MC_JOY].deadzone && m_ControlList[MC_JOY].sens[AXIS_SIDE] != 0.0) {
#if defined TABLE_LOOKUP
      axisval +=
          SIGN(is_joy[kNeed_MoveX]) * m_ControlList[MC_JOY].sens_curve[AXIS_SIDE][ABS((int)(is_joy[kNeed_MoveX]))];
#elif defined REAL_POW
      if (m_ControlList[MC_JOY].sens[AXIS_SIDE] == 1.0)
        axisval += is_joy[kNeed_MoveX];
      else
        axisval +=
            powf(ABS(is_joy[kNeed_MoveX]), m_ControlList[MC_JOY].inv_sens[AXIS_SIDE]) * SIGN(is_joy[kNeed_MoveX]);
#else
      axisval += is_joy[kNeed_MoveX] * m_ControlList[MC_JOY].sens[AXIS_SIDE];
#endif
    }
    break;

  case CT_V_AXIS:
    axisval -= is_mouse[kNeed_MoveY];
    if (ABS(is_joy[kNeed_MoveY]) > m_ControlList[MC_JOY].deadzone && m_ControlList[MC_JOY].sens[AXIS_VERT] != 0.0) {
#if defined TABLE_LOOKUP
      axisval -=
          SIGN(is_joy[kNeed_MoveY]) * m_ControlList[MC_JOY].sens_curve[AXIS_VERT][ABS((int)(is_joy[kNeed_MoveY]))];
#elif defined REAL_POW
      if (m_ControlList[MC_JOY].sens[AXIS_VERT] == 1.0)
        axisval -= is_joy[kNeed_MoveY];
      else
        axisval -=
            powf(ABS(is_joy[kNeed_MoveY]), m_ControlList[MC_JOY].inv_sens[AXIS_VERT]) * SIGN(is_joy[kNeed_MoveY]);
#else
      axisval -= is_joy[kNeed_MoveY] * m_ControlList[MC_JOY].sens[AXIS_VERT];
#endif
    }
    break;
  default:
    mprintf((2, "invalid axis %d\n", axis));
    Int3(); // NOT A VALID AXIS
    break;
  }
  //	mprintf((2, "x %d y %d z %d p %d h %d r %d\n",
  // is_joy[kNeed_MoveX],is_joy[kNeed_MoveY],is_joy[kNeed_MoveZ],is_joy[kNeed_Pitch],is_joy[kNeed_Yaw],is_joy[kNeed_Roll]));

  return axisval;
}
//	get keyboard info
float gameMacController::get_key_value(int key, ct_format format) {
  float val = (float)0.0;
  ASSERT(key < DDIO_MAX_KEYS);
  switch (format) {
    //	note we take care of mouse controls and external controls here
  case ctDigital:
    if (KEY_STATE(key))
      val = 1.0f;
    break;
  case ctDownCount:
    val = (float)ddio_KeyDownCount(key);
    break;
  case ctTime:
    val = (float)ddio_KeyDownTime(key);
    break;
  default:
    mprintf((1, "gameController::key unsupported format for function\n"));
  }
  return val;
}
//	get keyboard info
float gameMacController::get_button_value(int button) {
  float down_time = (float)0.0;
  int sprockButton = ConvertPC_IS(button);
  float cur_time = timer_GetTime();

  switch (button) {
  case ctfAFTERBURN_BUTTON:
  case ctfTOGGLE_SLIDEBUTTON:
  case ctfTOGGLE_BANKBUTTON:
    if (iSbutton[sprockButton].state)
      down_time = 1;
    else
      down_time = 0;
    break;

  case ctfWPNSEL_PCYCLEBTN:
  case ctfWPNSEL_SCYCLEBTN:
  case ctfREARVIEW_BTN:
  case ctfPREV_CNTMSBTN:
  case ctfNEXT_CNTMSBTN:
  case ctfCNTMS_USEBTN:
  case ctfPREV_INVBTN:
  case ctfNEXT_INVBTN:
  case ctfINV_USEBTN:
  case ctfHEADLIGHT_BUTTON:
  case ctfFIREFLARE_BUTTON:
  case ctfAUTOMAP_BUTTON:
    if (iSbutton[sprockButton].state) {
      down_time = 1;
      iSbutton[sprockButton].state = false;
    } else {
      down_time = 0;
    }
    break;

  default:
    if (iSbutton[sprockButton].state) {
      if (iSbutton[sprockButton].DownTime != 0.0) {
        down_time = cur_time - iSbutton[sprockButton].DownTime;
      }
    } else {
      if (iSbutton[sprockButton].UpTime != 0.0 && iSbutton[sprockButton].DownTime != 0.0) {
        down_time = iSbutton[sprockButton].UpTime - iSbutton[sprockButton].DownTime;
        iSbutton[sprockButton].UpTime = iSbutton[sprockButton].DownTime = 0.0;
      }
    }
  }
  if (!(button == ctfFIREPRIMARY_BUTTON || button == ctfFIRESECONDARY_BUTTON))
    if (down_time > 1.0)
      down_time = 1.0;
  return down_time;
}
int gameMacController::assign_function(ct_function *func) {
  //	for now this is a straight forward translation (that is, no mapping of needs to controller
  //	list to create elements.
  ct_element elem;
  int i;
  for (i = 0; i < CTLBINDS_PER_FUNC; i++) {
    elem.ctl[i] = NULL_WINCONTROLLER;
    switch (func->ctype[i]) {
    case ctNone:
      break;
    case ctKey:
      elem.ctl[i] = 0;
      break;
    case ctAxis:
      elem.ctl[i] = get_axis_controller(func->value[i]);
      break;
    case ctButton:
      elem.ctl[i] = get_button_controller(func->value[i]);
      break;
    case ctMouseAxis:
      elem.ctl[i] = 1;
      break;
    case ctMouseButton:
      //	find a free mouse button.
      if ((m_ControlList[1].btnmask & (1 << (func->value[i] - 1))) &&
          ((func->value[i] - 1) < m_ControlList[1].buttons)) {
        elem.ctl[i] = 1;
      }
      break;
    case ctPOV:
      elem.ctl[i] = get_pov_controller();
      break;
    }
    elem.ctype[i] = func->ctype[i];
    elem.value[i] = func->value[i];
  }
  elem.format = func->format;
  elem.flags[0] = func->flags[0];
  elem.flags[1] = func->flags[1];
  elem.enabled = true;
  assign_element(func->id, &elem);
  return func->id;
}
//	sets up an elements information structure
void gameMacController::assign_element(int id, ct_element *elem) {
  //	assign element, check to see if valid.
  int i;
  m_ElementList[id].format = elem->format;
  m_ElementList[id].flags[0] = elem->flags[0];
  m_ElementList[id].flags[1] = elem->flags[1];
  m_ElementList[id].enabled = elem->enabled;
  //	look through each controller and validate each element
  for (i = 0; i < CTLBINDS_PER_FUNC; i++) {
    m_ElementList[id].ctl[i] = elem->ctl[i];
    m_ElementList[id].value[i] = elem->value[i];
    m_ElementList[id].ctype[i] = elem->ctype[i];
    if (m_ElementList[id].ctl[i] != NULL_WINCONTROLLER) {
      // this function shouldn't do any error checking!!!!  keep same controller values and bindings unless
      // bindings are truly bogus.
      switch (m_ElementList[id].ctype[i]) {
      case ctMouseButton:
      case ctButton:
        if (elem->value[i] > CT_MAX_BUTTONS) {
          m_ElementList[id].ctl[i] = NULL_WINCONTROLLER;
          m_ElementList[id].value[i] = NULL_BINDING;
        }
        break;
      case ctMouseAxis:
      case ctAxis:
        //					if (!(m_ControlList[elem->ctl[i]].flags & (1<<(elem->value[i]-1))))
        //						m_ElementList[id].ctl[i] = NULL_WINCONTROLLER;
        break;
      case ctPOV:
        //					if (!(m_ControlList[elem->ctl[i]].flags & CTF_POV))
        //						m_ElementList[id].ctl[i] = NULL_WINCONTROLLER;
        break;
      case ctKey:
        break;
      default:
        m_ElementList[id].value[i] = NULL_BINDING;
        m_ElementList[id].ctl[i] = NULL_WINCONTROLLER;
      }
    } else {
      m_ElementList[id].value[i] = NULL_BINDING;
    }
  }
}
sbyte gameMacController::get_axis_controller(ubyte axis) {
  return 2;
  //	return NULL_CONTROLLER;
}
sbyte gameMacController::get_button_controller(ubyte btn) { return NULL_CONTROLLER; }
sbyte gameMacController::get_pov_controller() { return NULL_CONTROLLER; }
//	enumerate all controllers on system
bool gameMacController::enum_controllers(char *remote_adr) {
  int num_devs = 0, dev;
  int i;
  for (i = 0; i < CT_MAX_CONTROLLERS; i++)
    m_ControlList[i].id = CTID_INVALID;
  //	Add keyboard controller
  m_ControlList[num_devs].id = CTID_KEYBOARD;
  m_ControlList[num_devs].buttons = 0;
  m_ControlList[num_devs].flags = 0;
  num_devs++;
  return true;
}
// gets sensitivity of axis item
float gameMacController::get_axis_sensitivity(ct_type axis_type, ubyte axis) {
  axis--;
  ASSERT(axis < CT_NUM_AXES);
  switch (axis_type) {
  case ctMouseAxis:
    return m_ControlList[1].sens[axis];
  case ctAxis:
    return m_ControlList[MC_JOY].sens[axis];
  default:
    Int3();
  }
  return 0.0f;
}
// sets sensitivity of axis item
void gameMacController::set_axis_sensitivity(ct_type axis_type, ubyte axis, float val) {
  int i;
  axis--;
  ASSERT(axis < CT_NUM_AXES);
  switch (axis_type) {
  case ctMouseAxis:
    m_ControlList[1].sens[axis] = val;
    break;
  case ctAxis:
    for (i = 2; i < CT_MAX_CONTROLLERS; i++) {
      m_ControlList[i].sens[axis] = val;
      //			m_ControlList[i].inv_sens[axis] = 1.0/val;
      m_ControlList[i].sens_curve[axis][0] = 0;
      for (int j = 1; j <= JOY_SAMPLES; j++) {
        m_ControlList[i].sens_curve[axis][j] = powf((float)j / (float)(JOY_SAMPLES), 1.0 / val);
      }
    }
    break;
  default:
    Int3();
  }
}
// #ifndef MACINTOSH
//	toggles use of deadzone for controllers. ctl can be 0 to ???
//  dead zone is from 0.0 to 0.5
void gameMacController::set_controller_deadzone(int ctl, float deadzone) {
  if (deadzone < 0.0)
    deadzone = 0.0;
  if (deadzone > 25)
    deadzone = 25;
  m_ControlList[MC_JOY].deadzone = deadzone * 2;
}
float gameMacController::get_controller_deadzone(int ctl) {
  float val = m_ControlList[MC_JOY].deadzone / 2;
  return val;
}

// #endif
char Ctltext_AxisBindings[][16] = {"", "X-axis", "Y-axis", "Z-axis", "R-axis", "U-axis", "V-axis"};
char Ctltext_BtnBindings[][16] = {"",      "btn1",  "btn2",  "btn3",  "btn4",  "btn5",  "btn6",  "btn7",  "btn8",
                                  "btn9",  "btn10", "btn11", "btn12", "btn13", "btn14", "btn15", "btn16", "btn17",
                                  "btn18", "btn19", "btn20", "btn21", "btn22", "btn23", "btn24", "btn25", "btn26",
                                  "btn27", "btn28", "btn29", "btn30", "btn31", "btn32"};
char Ctltext_PovBindings[][16] = {"", "pov-U", "pov-R", "pov-D", "pov-L"};
#define NUM_AXISBINDSTRINGS (sizeof(Ctltext_AxisBindings) / sizeof(Ctltext_AxisBindings[0]))
#define NUM_BTNBINDSTRINGS (sizeof(Ctltext_BtnBindings) / sizeof(Ctltext_AxisBindings[0]))
#ifdef MACINTOSH
// retrieves binding text for desired function, binding, etc.
const char *gameMacController::get_binding_text(ct_type type, ubyte ctrl, ubyte bind) {
  static char binding_text[16];
  const char *str;
  if (ctrl == NULL_CONTROLLER) {
    return NULL;
  }
  switch (type) {
  case ctAxis: {
    ASSERT(bind < NUM_AXISBINDSTRINGS);
    str = Ctltext_AxisBindings[bind];
    if ((ctrl - 2) > 0) {
      sprintf(binding_text, "J%d:%s", (ctrl - 2) + 1, str);
    } else {
      return str;
    }
    break;
  }
  case ctMouseAxis: {
    str = ddio_MouseGetAxisText(((sbyte)bind) - 1);
    return str;
  }
  case ctButton: {
    ASSERT(bind < NUM_BTNBINDSTRINGS);
    str = Ctltext_BtnBindings[bind];
    if ((ctrl - 2) > 0) {
      sprintf(binding_text, "J%d:%s", (ctrl - 2) + 1, str);
    } else {
      return str;
    }
    break;
  }
  case ctMouseButton: {
    str = ddio_MouseGetBtnText(((sbyte)bind) - 1);
    return str;
  }
  case ctPOV:
  case ctPOV2:
  case ctPOV3:
  case ctPOV4: {
    ushort povpos = bind;
    if (povpos == JOYPOV_UP)
      str = Ctltext_PovBindings[1];
    else if (povpos == JOYPOV_DOWN)
      str = Ctltext_PovBindings[3];
    else if (povpos == JOYPOV_LEFT)
      str = Ctltext_PovBindings[4];
    else if (povpos == JOYPOV_RIGHT)
      str = Ctltext_PovBindings[2];
    else
      str = Ctltext_PovBindings[0];
    if ((ctrl - 2) > 0) {
      if (type - ctPOV) {
        sprintf(binding_text, "J%d:%s%d", (ctrl - 2) + 1, str, type - ctPOV);
      } else {
        sprintf(binding_text, "J%d:%s", (ctrl - 2) + 1, str);
      }
    } else {
      if (type - ctPOV) {
        sprintf(binding_text, "%s%d", str, type - ctPOV);
      } else {
        return str;
      }
    }
    break;
  }
  case ctKey:
    break;
  default:
    if (type == ctNone) {
      Int3();
    }
    binding_text[0] = 0;
  }
  return binding_text;
}
#endif