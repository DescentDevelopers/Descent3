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

 * $Logfile: /DescentIII/main/D3ForceFeedback.cpp $
 * $Revision: 19 $
 * $Date: 5/10/99 9:25p $
 * $Author: Jeff $
 *
 * High level force-feedback implementation
 *
 * $Log: /DescentIII/main/D3ForceFeedback.cpp $
 *
 * 19    5/10/99 9:25p Jeff
 * first phase of Rock 'n' Ride support added
 *
 * 18    4/24/99 7:45p Jeff
 * fixed recoil
 *
 * 17    4/21/99 11:05a Kevin
 * new ps_rand and ps_srand to replace rand & srand
 *
 * 16    4/15/99 1:38a Jeff
 * changes for linux compile
 *
 * 15    4/12/99 1:06p Jeff
 * hooked force feedback into recoil of weapon struct
 *
 * 14    3/28/99 5:53p Jeff
 * fixed iforce crashes
 *
 * 13    2/09/99 3:32p Jeff
 * table file parser takes quotes strings for force keywords
 *
 * 12    2/05/99 7:04p Jeff
 * table file parsing macros put in
 *
 * 11    1/30/99 11:27p Jeff
 * added immersion support
 *
 * 10    1/28/99 12:09p Jeff
 * added force feedback to player shake...fixed spelling error in define
 * for forcefeedback
 *
 * 9     1/25/99 7:28p Jeff
 * fixed alt-tab bug (finally)...wasn't releasing effects on alt-tab out,
 * and re-creating them on restart
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
 * 3     9/21/98 11:09a Jeff
 * general update, new low level, small high level implementation
 *
 * 2     9/18/98 7:38p Jeff
 * creation of low-level forcefeedback and beginning of high-level
 * forcefeedback
 *
 * $NoKeywords: $
 */

#include <cstdlib>

#include "forcefeedback.h"
#include "D3ForceFeedback.h"
#include "pserror.h"
#include "log.h"
#include "weapon.h"
#include "ddio.h"
#include "psrand.h"
#include "descent.h"

extern float Gametime;

// D3Force_init
// true if ForceInit() has been called, meaning hi-level ForceFeedback has been initialized
// and any of the Hi-Level ForceFeedback commands can be called.
bool D3Force_init = false;

// D3Force_pause
// true if the ForceFeedback effects are currently being paused. ForceContinue() must be called
// before any other ForceFeedback functions are called.
bool D3Force_pause = false;

// D3Force_auto_center
// true if the user wants his joystick to be autocentered (if available)
bool D3Force_auto_center = false;

// D3Force_gain
// the gain setting of the ForceFeedback system
float D3Force_gain = 1.0f;

// D3Use_force_feedback
// true if the user wants force feedback during play (if available)
bool D3Use_force_feedback = true;

int Force_hi_to_low_map[DDIO_FF_MAXEFFECTS];

// -----------------------------------------------------------------
//	ForceInit
//	Purpose:
//		Initializes the Hi-Level Force Feedback system, creating all
//	of the effects so they are ready to be used.
// -----------------------------------------------------------------
void ForceInit(void) {
  static bool first_call = true;

  ASSERT(!D3Force_init);

  bool dd_found, dd_enabled;
  ddio_ff_GetInfo(&dd_found, &dd_enabled);

  if (dd_found) {
    D3Force_init = true;
    D3Force_pause = false;

    LOG_INFO << "Force: High Level Force Feedback system initialized";
  } else {
    D3Force_init = false;
    D3Force_pause = false;

    LOG_WARNING << "Force: Force Feedback System Not Found";
  }

  for (int i = 0; i < DDIO_FF_MAXEFFECTS; i++) {
    Force_hi_to_low_map[i] = -1;
  }

  ForceEffectsInit();

  if (D3Force_init) {
    // Enable/Disable the force feedback
    // ---------------------------------
    if (D3Use_force_feedback)
      ForceEnable();
    else
      ForceDisable();

    // Enable/Disable AutoCentering
    // ----------------------------
    if (D3Force_auto_center)
      ForceEnableAutoCenter();
    else
      ForceDisableAutoCenter();

    // Set ForceFeedback Gain
    // ----------------------
    ForceSetGain(D3Force_gain);
  }

  if (first_call) {
    first_call = false;
    atexit(ForceClose);
  }
}

// -----------------------------------------------------------------
//	ForceClose
//	Purpose:
//		Shutsdown the Hi-Level Force Feedback system
// -----------------------------------------------------------------
void ForceClose(void) {
  if (!D3Force_init)
    return;

  D3Force_pause = false;
  D3Force_init = false;

  LOG_INFO << "Force: Shutting down high level force feedback";
  ForceEffectsClose();

  for (int i = 0; i < DDIO_FF_MAXEFFECTS; i++) {
    Force_hi_to_low_map[i] = -1;
  }
}

// -----------------------------------------------------------------
//	ForceShutdown
//	Purpose:
//		Puts the Force Feedback system on pause while the application
//	is doing other things (like it lost focus)
// -----------------------------------------------------------------
void ForceShutdown(void) {
  D3Force_pause = true;

  ForceClose();
  if (D3Force_init) {
    LOG_INFO << "Force: Shutting down Force Feedback System";
    ddio_ffb_Pause(kJoy1);
  }
}

// ------------------------------------------------------------------
//	ForceRestart
//	Purpose:
//		Restores the Force Feedback system that has been put on pause
//	from ForceShutdown.
// ------------------------------------------------------------------
void ForceRestart(void) {
  D3Force_pause = false;

  if (D3Force_init) {
    LOG_INFO << "Force: Restarting Force Feedback System";
    ddio_ff_Acquire(kJoy1);
    ddio_ffb_Continue(kJoy1);

    // Enable/Disable the force feedback
    // ---------------------------------
    if (D3Use_force_feedback)
      ForceEnable();
    else
      ForceDisable();

    // Enable/Disable AutoCentering
    // ----------------------------
    if (D3Force_auto_center)
      ForceEnableAutoCenter();
    else
      ForceDisableAutoCenter();

    // Set ForceFeedback Gain
    // ----------------------
    ForceSetGain(D3Force_gain);
  }

  ForceInit();
}

// ------------------------------------------------------------------
//	ForceDisable
//	Purpose:
//		Disables Force Feedback on a Force Feedback system
// ------------------------------------------------------------------
void ForceDisable(void) {
  if (D3Force_init) {
    LOG_INFO << "Force: Disabling Force Feedback System";
    //@@ddio_ffb_Disable(kJoy1);
    D3Use_force_feedback = false;
  }
}

// ------------------------------------------------------------------
//	ForceEnable
//	Purpose:
//		Enables Force Feedback that has been previously disabled
// ------------------------------------------------------------------
void ForceEnable(void) {
  if (D3Force_init) {
    LOG_INFO << "Force: Enabling Force Feedback System";
    //@@ddio_ffb_Enable(kJoy1);
    D3Use_force_feedback = true;
  }
}

// ------------------------------------------------------------------
//	ForceDisableAutoCenter
//	Purpose:
//		Disables autocentering on the joystick
// ------------------------------------------------------------------
void ForceDisableAutoCenter(void) {
  if (D3Force_init) {
    D3Force_auto_center = false;

    ddio_ffjoy_EnableAutoCenter(kJoy1, false);
  }
}

// ------------------------------------------------------------------
//	ForceEnableAutoCenter
//	Purpose:
//		Enables autocentering on the joystick
// ------------------------------------------------------------------
void ForceEnableAutoCenter(void) {
  if (D3Force_init) {
    D3Force_auto_center = true;

    ddio_ffjoy_EnableAutoCenter(kJoy1, true);
  }
}

// ------------------------------------------------------------------
//	ForceIsEnabled
//	Purpose:
//		Returns true if Force Feedback is enabled on the system
// ------------------------------------------------------------------
bool ForceIsEnabled(void) { return D3Use_force_feedback; }

// ------------------------------------------------------------------
//	ForceIsAutoCenter
//	Purpose:
//		Returns true if Force Feedback joystick is autocentering
// ------------------------------------------------------------------
bool ForceIsAutoCenter(void) { return D3Force_auto_center; }

// ------------------------------------------------------------------
//	ForceSetGain
//	Purpose:
//		Sets the gain of the ForceFeedback system (0 -> 1)
// ------------------------------------------------------------------
void ForceSetGain(float val) {
  if (val < 0.0f)
    val = 0.0f;
  if (val > 1.0f)
    val = 1.0f;

  D3Force_gain = val;
  ddio_ffjoy_SetGain(kJoy1, D3Force_gain);
}

// ------------------------------------------------------------------
// ForceGetGain
//	Purpose:
//		Returns the current gain setting of the ForceFeedback system (0-1)
// ------------------------------------------------------------------
float ForceGetGain(void) { return D3Force_gain; }

/*
=============================================================================
*/

// -----------------------------------------------------------------
//	ForceEffectsClose
//	Purpose:
//		Destroys all the effects created
// -----------------------------------------------------------------
void ForceEffectsClose(void) { ddio_ffb_DestroyAll(); }

// -----------------------------------------------------------------
//	ForceEffectsPlay
//	Purpose:
//		Plays an effect
// -----------------------------------------------------------------
void ForceEffectsPlay(int id, float *scale, int *direction) {
  if (!D3Force_init || !D3Use_force_feedback)
    return;

  if (id < 0 || id >= DDIO_FF_MAXEFFECTS)
    return;

  int low_id = Force_hi_to_low_map[id];
  if (low_id == -1)
    return;

  if (scale || direction) {
    uint32_t *ns = NULL;
    uint32_t new_gain;

    if (scale) {
      new_gain = (uint32_t)(10000.0f * (*scale));
      ns = &new_gain;
    }

    ddio_ffb_effectModify(low_id, direction, NULL, ns, NULL, NULL, NULL);
  }

  ddio_ffb_effectPlay(low_id);
}
void ForceEffectsPlay(int id, float *scale, vector *direction) {
  if (!D3Force_init || !D3Use_force_feedback)
    return;

  if (id < 0 || id >= DDIO_FF_MAXEFFECTS)
    return;

  int low_id = Force_hi_to_low_map[id];
  if (low_id == -1)
    return;

  int new_dir = -1;

  if (direction) {
    matrix mat = Identity_matrix;
    angvec ag;

    vm_VectorToMatrix(&mat, direction);
    vm_ExtractAnglesFromMatrix(&ag, &mat);

    new_dir = ((((scalar)ag.h()) / 65535.0f) * 360.0f);

    new_dir = (new_dir)*FF_DEGREES;
  }

  if (scale || direction) {
    int *d = NULL;
    uint32_t *ns = NULL;
    uint32_t new_gain;

    if (direction)
      d = &new_dir;

    if (scale) {
      new_gain = (uint32_t)(10000.0f * (*scale));
      ns = &new_gain;
    }

    ddio_ffb_effectModify(low_id, d, NULL, ns, NULL, NULL, NULL);
  }

  ddio_ffb_effectPlay(low_id);
}

void DoForceForWeapon(object *me_obj, object *it_obj, vector *force_vec) {
  if (it_obj->id < 0 || it_obj->id >= MAX_WEAPONS)
    return;

  weapon *weap = &Weapons[it_obj->id];

  if (weap->flags & WF_MICROWAVE) {
    vector local_norm, v;
    float scale = 1.00f;
    v.x() = ps_rand() % 10;
    v.y() = 0;
    v.z() = ps_rand() % 10;
    vm_NormalizeVector(&v);
    vm_MatrixMulVector(&local_norm, &v, &me_obj->orient);

    local_norm *= -1.0f;

    ForceEffectsPlay(FORCE_MICROWAVE, &scale, &local_norm);
    return;
  }
}

void DoForceForWall(object *playerobj, float hitspeed, int hitseg, int hitwall, vector *wall_normal) {
  vector local_norm;
  float scale = 1.00f;

  if (hitspeed < 20)
    return;

  scale = hitspeed / 80.0f;
  if (scale < 0.0f)
    scale = 0.0f;
  if (scale > 1.0f)
    scale = 1.0f;

  vm_MatrixMulVector(&local_norm, wall_normal, &playerobj->orient);
  local_norm *= -1.0f;

  ForceEffectsPlay(FORCE_WALLHIT, &scale, &local_norm);
}

void DoForceForRecoil(object *playerobj, object *weap) {
  weapon *w_ptr = &Weapons[weap->id];

  vector local_norm;
  float scale;
#define MIN_RECOIL 1000.0f
#define MAX_RECOIL 5000.0f
#define RECOIL_THRESHOLD 1100.0f

  if (w_ptr->recoil_force < RECOIL_THRESHOLD)
    return;

  vector offset = weap->pos - playerobj->pos;
  vm_MatrixMulVector(&local_norm, &offset, &playerobj->orient);

  if (weap->movement_type != MT_PHYSICS)
    return;

  ASSERT(w_ptr->recoil_force >= MIN_RECOIL && w_ptr->recoil_force <= MAX_RECOIL);
  scale = (w_ptr->recoil_force - MIN_RECOIL) / (MAX_RECOIL - MIN_RECOIL);

  ForceEffectsPlay(FORCE_WEAPON_RECOIL, &scale, &local_norm);
}

float Force_time_since_last_shake;
#define SHAKE_TIME 0.2f
void DoForceForShake(float magnitude) {
  if (Force_time_since_last_shake + SHAKE_TIME > Gametime) {
    if (Force_time_since_last_shake < Gametime) {
      return;
    }
  }

  Force_time_since_last_shake = Gametime;

  vector local_norm;

  if (magnitude < 0.0f)
    magnitude = 0.0f;
  if (magnitude > 1.0f)
    magnitude = 1.0f;

  local_norm.x() = (ps_rand() % 5);
  local_norm.y() = (ps_rand() % 5);
  local_norm.z() = (ps_rand() % 5);
  vm_NormalizeVector(&local_norm);

  ForceEffectsPlay(FORCE_SHIPSHAKE, &magnitude, &local_norm);
}

//************************************************************************

void FFECreate_Test(tFFB_Effect *ef) {
  memset(ef, 0, sizeof(tFFB_Effect));

  ef->Type = kConstant;
  ef->TypeInfo.Constant.Mag = FF_NOMINALMAX;
  ef->Duration = 0.1 * FF_SECONDS;
  ef->Gain = FF_NOMINALMAX;
  ef->Period = 0;
  ef->Axis = kBothAxes;
  ef->Trigger = kNoButton;
  ef->Direction = 0;
}

void FFECreate_Microwave(tFFB_Effect *ef) {
  memset(ef, 0, sizeof(tFFB_Effect));

  ef->Flags = FF_USEENVELOPE;
  ef->Type = kWave_Sine;
  ef->TypeInfo.Wave.Mag = FF_NOMINALMAX;
  ef->TypeInfo.Wave.Offset = 0;
  ef->TypeInfo.Wave.Phase = 0;
  ef->TypeInfo.Wave.Period = 0.2 * FF_SECONDS;

  ef->Duration = 0.6 * FF_SECONDS;
  ef->Gain = FF_NOMINALMAX;
  ef->Period = 0;
  ef->Axis = kBothAxes;
  ef->Trigger = kNoButton;
  ef->Direction = 0;

  ef->Envelope.AttackLevel = FF_NOMINALMAX * 0.5f;
  ef->Envelope.AttackTime = 0;
  ef->Envelope.FadeLevel = FF_NOMINALMAX * 0.1f;
  ef->Envelope.FadeTime = 0.6 * FF_SECONDS;
}

void FFECreate_Wallhit(tFFB_Effect *ef) {
  memset(ef, 0, sizeof(tFFB_Effect));

  ef->Type = kConstant;
  ef->TypeInfo.Constant.Mag = FF_NOMINALMAX;
  ef->Duration = 0.1 * FF_SECONDS;
  ef->Gain = FF_NOMINALMAX;
  ef->Period = 0;
  ef->Axis = kBothAxes;
  ef->Trigger = kNoButton;
  ef->Direction = 0;
}

void FFECreate_WeaponRecoil(tFFB_Effect *ef) {
  memset(ef, 0, sizeof(tFFB_Effect));

  ef->Type = kConstant;
  ef->TypeInfo.Constant.Mag = FF_NOMINALMAX;
  ef->Duration = 0.1 * FF_SECONDS;
  ef->Gain = FF_NOMINALMAX;
  ef->Period = 0;
  ef->Axis = kBothAxes;
  ef->Trigger = kNoButton;
  ef->Direction = 0;
}

void FFECreate_VaussRecoil(tFFB_Effect *ef) {
  memset(ef, 0, sizeof(tFFB_Effect));

  ef->Type = kWave_Square;
  ef->TypeInfo.Wave.Mag = FF_NOMINALMAX;
  ef->TypeInfo.Wave.Offset = 0;
  ef->TypeInfo.Wave.Phase = 0;
  ef->TypeInfo.Wave.Period = 0.05 * FF_SECONDS;

  ef->Duration = 0.2 * FF_SECONDS;
  ef->Gain = FF_NOMINALMAX;
  ef->Period = 0;
  ef->Axis = kBothAxes;
  ef->Trigger = kNoButton;
  ef->Direction = 0;
}

void FFECreate_Afterburner(tFFB_Effect *ef) {
  memset(ef, 0, sizeof(tFFB_Effect));

  ef->Type = kWave_Square;
  ef->TypeInfo.Wave.Mag = FF_NOMINALMAX;
  ef->TypeInfo.Wave.Offset = 0;
  ef->TypeInfo.Wave.Phase = 0;
  ef->TypeInfo.Wave.Period = 0.05 * FF_SECONDS;

  ef->Duration = 0.2 * FF_SECONDS;
  ef->Gain = FF_NOMINALMAX;
  ef->Period = 0;
  ef->Axis = kBothAxes;
  ef->Trigger = kNoButton;
  ef->Direction = 0;
}

void FFECreate_ShipShake(tFFB_Effect *ef) {
  memset(ef, 0, sizeof(tFFB_Effect));

  ef->Type = kWave_Sine;
  ef->TypeInfo.Wave.Mag = (FF_NOMINALMAX / 3);
  ef->TypeInfo.Wave.Offset = 0;
  ef->TypeInfo.Wave.Phase = 0;
  ef->TypeInfo.Wave.Period = (SHAKE_TIME / 2) * FF_SECONDS;

  ef->Duration = SHAKE_TIME * FF_SECONDS;
  ef->Gain = FF_NOMINALMAX;
  ef->Period = 0;
  ef->Axis = kBothAxes;
  ef->Trigger = kNoButton;
  ef->Direction = 0;
}

//*************************************************************
// ------------------------------------------------------------------
//	ForceEffectsInit
//	Purpose:
//		Initializes the force feedback effects for Descent 3
// ------------------------------------------------------------------
void ForceEffectsInit(void) {
  tFFB_Effect eff;
  int lowid;
  Force_time_since_last_shake = 0;
  FORCEPROJECT prj;
  std::filesystem::path path;

  if (cfexist("D3Force.ifr")) {
    path = Descent3_temp_directory / "D3Force.ifr";
    cf_CopyFile(path, "D3Force.ifr", 0);
    prj = ddio_ForceLoadProject(IGNORE_TABLE(path), kJoy1);
  } else {
    prj = NULL;
  }
  /*
  $$TABLE_GAMEFILE "D3Force.ifr"
  */
  lowid = ddio_CreateForceFromProject(prj, "Test");
  if (lowid == -1) {
    FFECreate_Test(&eff);
    lowid = ddio_ffb_effectCreate(kJoy1, &eff);
  }
  Force_hi_to_low_map[FORCE_TEST_FORCE] = lowid;

  lowid = ddio_CreateForceFromProject(prj, "Microwave");
  if (lowid == -1) {
    FFECreate_Microwave(&eff);
    lowid = ddio_ffb_effectCreate(kJoy1, &eff);
  }
  Force_hi_to_low_map[FORCE_MICROWAVE] = lowid;

  lowid = ddio_CreateForceFromProject(prj, "WallHit");
  if (lowid == -1) {
    FFECreate_Wallhit(&eff);
    lowid = ddio_ffb_effectCreate(kJoy1, &eff);
  }
  Force_hi_to_low_map[FORCE_WALLHIT] = lowid;

  lowid = ddio_CreateForceFromProject(prj, "Recoil");
  if (lowid == -1) {
    FFECreate_WeaponRecoil(&eff);
    lowid = ddio_ffb_effectCreate(kJoy1, &eff);
  }
  Force_hi_to_low_map[FORCE_WEAPON_RECOIL] = lowid;

  lowid = ddio_CreateForceFromProject(prj, "VaussRecoil");
  if (lowid == -1) {
    FFECreate_VaussRecoil(&eff);
    lowid = ddio_ffb_effectCreate(kJoy1, &eff);
  }
  Force_hi_to_low_map[FORCE_VAUSS_RECOIL] = lowid;

  lowid = ddio_CreateForceFromProject(prj, "Afterburner");
  if (lowid == -1) {
    FFECreate_Afterburner(&eff);
    lowid = ddio_ffb_effectCreate(kJoy1, &eff);
  }
  Force_hi_to_low_map[FORCE_AFTERBURNER] = lowid;

  lowid = ddio_CreateForceFromProject(prj, "ShipShake");
  if (lowid == -1) {
    FFECreate_ShipShake(&eff);
    lowid = ddio_ffb_effectCreate(kJoy1, &eff);
  }
  Force_hi_to_low_map[FORCE_SHIPSHAKE] = lowid;

  ddio_ForceUnloadProject(prj);
  if (cfexist(path)) {
    std::filesystem::remove(path);
  }
}
