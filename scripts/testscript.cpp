// testscript.cpp
// 0.1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "osiris_import.h"
#include "osiris_common.h"

#ifdef _MSC_VER // Visual C++ Build
#define STDCALL __stdcall
#define STDCALLPTR *STDCALL
#else // Non-Visual C++ Build
#define STDCALL __attribute__((stdcall))
#define STDCALLPTR STDCALL *
#endif

#ifdef __cplusplus
extern "C" {
#endif
char STDCALL InitializeDLL(tOSIRISModuleInit *func_list);
void STDCALL ShutdownDLL(void);
int STDCALL GetGOScriptID(char *name, ubyte isdoor);
void STDCALLPTR CreateInstance(int id);
void STDCALL DestroyInstance(int id, void *ptr);
short STDCALL CallInstanceEvent(int id, void *ptr, int event, tOSIRISEventInfo *data);
int STDCALL SaveRestoreState(void *file_ptr, ubyte saving_state);
#ifdef __cplusplus
}
#endif

// ==========================
// Script class definitions//
// ==========================
#define ID_SHIELD_ORB 0x00 // shield id
#define ID_ENERGY_ORB 0x01 // energy id
#define MAX_IDS 0x02       // maximum number of IDs
typedef struct {
  int id;
  char *name;
} tScriptInfo;

tScriptInfo ScriptInfo[MAX_IDS] = {{ID_SHIELD_ORB, "Shield"}, {ID_ENERGY_ORB, "Energy"}};

class BaseObjScript {
public:
  BaseObjScript();
  ~BaseObjScript();
  virtual short CallEvent(int event, tOSIRISEventInfo *data);

protected:
  bool called;
};

typedef struct {
  int hitcount;
} tShieldOrbInfo;
//------------------
// Shield orb script
//------------------
class ShieldOrb : public BaseObjScript {
public:
  ShieldOrb() { info = NULL; }
  short CallEvent(int event, tOSIRISEventInfo *data);

private:
  tShieldOrbInfo *info;
};

//------------------
// Energy orb script
//------------------
class EnergyOrb : public BaseObjScript {
public:
  short CallEvent(int event, tOSIRISEventInfo *data);
};

char STDCALL InitializeDLL(tOSIRISModuleInit *func_list) {
  osicommon_Initialize((tOSIRISModuleInit *)func_list);

  return 1;
}

void STDCALL ShutdownDLL(void) {}

int STDCALL GetGOScriptID(char *name, ubyte isdoor) {
  for (int i = 0; i < MAX_IDS; i++) {
    if (!stricmp(name, ScriptInfo[i].name)) {
      return ScriptInfo[i].id;
    }
  }
  return -1;
}

void STDCALLPTR CreateInstance(int id) {
  switch (id) {
  case ID_SHIELD_ORB:
    return new ShieldOrb;
    break;
  case ID_ENERGY_ORB:
    return new EnergyOrb;
    break;
  default:
    mprintf(0, "SCRIPT: Illegal ID (%d)\n", id);
    break;
  };
  return NULL;
}

void STDCALL DestroyInstance(int id, void *ptr) {
  switch (id) {
  case ID_SHIELD_ORB:
    delete ((ShieldOrb *)ptr);
    break;
  case ID_ENERGY_ORB:
    delete ((EnergyOrb *)ptr);
    break;
  default:
    mprintf(0, "SCRIPT: Illegal ID (%d)\n", id);
    break;
  };
}

short STDCALL CallInstanceEvent(int id, void *ptr, int event, tOSIRISEventInfo *data) {
  switch (id) {
  case ID_SHIELD_ORB:
  case ID_ENERGY_ORB:
    return ((BaseObjScript *)ptr)->CallEvent(event, data);
    break;
  default:
    mprintf(0, "SCRIPT: Illegal ID (%d)\n", id);
    break;
  };

  return CONTINUE_CHAIN | CONTINUE_DEFAULT;
}

//	SaveRestoreState
//	Purpose:
//		This function is called when Descent 3 is saving or restoring the game state.  In this function
//	you should save/restore any global data that you want preserved through load/save (which includes
//	demos).  You must be very careful with this function, corrupting the file (reading or writing too
//	much or too little) may be hazardous to the game (possibly making it impossible to restore the
//	state).  It would be best to use version information to keep older versions of saved states still
//	able to be used.  IT IS VERY IMPORTANT WHEN SAVING THE STATE TO RETURN THE NUMBER OF _BYTES_ WROTE
//	TO THE FILE.  When restoring the data, the return value is ignored.  saving_state is 1 when you should
//	write data to the file_ptr, 0 when you should read in the data.
int STDCALL SaveRestoreState(void *file_ptr, ubyte saving_state) { return 0; }

//============================================
// Script Implementation
//============================================
BaseObjScript::BaseObjScript() { called = false; }

BaseObjScript::~BaseObjScript() {}

short BaseObjScript::CallEvent(int event, tOSIRISEventInfo *data) {
  switch (event) {
  case EVT_INTERVAL:
    mprintf(0, ".");
    break;
  case EVT_AI_FRAME:
    mprintf(0, "*");
    break;
  case EVT_DAMAGED:
    mprintf(0, "EVT_DAMAGED ");
    break;
  case EVT_COLLIDE:
    mprintf(0, "EVT_COLLIDE ");
    break;
  case EVT_CREATED:
    mprintf(0, "EVT_CREATED ");
    break;
  case EVT_DESTROY:
    mprintf(0, "EVT_DESTROY ");
    break;
  case EVT_TIMER:
    mprintf(0, "EVT_TIMER ");
    break;
  case EVT_USE:
    mprintf(0, "EVT_USE ");
    break;
  case EVT_AI_NOTIFY:
    mprintf(0, "EVT_AI_NOTIFY ");
    break;
  case EVT_AI_INIT:
    mprintf(0, "EVT_AI_INIT ");
    break;
  case EVT_CHANGESEG:
    mprintf(0, "EVT_CHANGESEG ");
    break;
  };

  return CONTINUE_CHAIN | CONTINUE_DEFAULT;
}

// --------------
// Shield orb
// --------------
short ShieldOrb::CallEvent(int event, tOSIRISEventInfo *data) {
  switch (event) {
  case EVT_COLLIDE:
    if (!called) {
      mprintf(0, "Collide with shield\n");
      msafe_struct mo;

      mo.objhandle = data->evt_collide.it_handle;
      MSafe_GetValue(MSAFE_OBJECT_TYPE, &mo);
      MSafe_GetValue(MSAFE_OBJECT_ID, &mo);

      if (mo.type == OBJ_PLAYER) {

        MSafe_GetValue(MSAFE_OBJECT_SHIELDS, &mo);
        if (mo.shields < 200) {
          mo.shields += 15;
          if (mo.shields > 200)
            mo.shields = 200;
          MSafe_CallFunction(MSAFE_OBJECT_SHIELDS, &mo);

          mo.slot = mo.id;
          mo.color = GR_RGB(0, 255, 0);
          sprintf(mo.message, "Shields boosted to %d", (int)mo.shields);
          MSafe_CallFunction(MSAFE_MISC_HUD_MESSAGE, &mo);
          called = true;
        }

        tOSIRISTIMER ti;
        ti.flags = 0;
        ti.id = 0;
        ti.repeat_count = -1;
        ti.object_handle = data->me_handle;
        ti.timer_interval = 10.0f;
        Scrpt_CreateTimer(&ti);
      }

      tOSIRISMEMCHUNK ch;
      ch.id = 4;
      ch.size = sizeof(tShieldOrbInfo);
      ch.my_id.type = OBJECT_SCRIPT;
      ch.my_id.objhandle = data->me_handle;
      info = (tShieldOrbInfo *)Scrpt_MemAlloc(&ch);
      if (info) {
        info->hitcount = 1;
      }
    } else {
      if (info) {
        info->hitcount++;
      }
    }

    if (info) {
      mprintf(0, "HIT(%d)\n", info->hitcount);
    }
    break;

  case EVT_MEMRESTORE: {
    info = (tShieldOrbInfo *)data->evt_memrestore.memory_ptr;
    mprintf(0, "MEM RESTORE!!! -- %d(%d)\n", data->evt_memrestore.id, info->hitcount);
  } break;

  case EVT_TIMER: {
    msafe_struct mo;
    mo.objhandle = data->me_handle;
    MSafe_GetValue(MSAFE_OBJECT_ID, &mo);
    mo.slot = mo.id;
    mo.color = GR_RGB(0, 255, 0);
    sprintf(mo.message, "TIMER: HIT COUNT %d", (info) ? info->hitcount : -1);
    MSafe_CallFunction(MSAFE_MISC_HUD_MESSAGE, &mo);
  } break;
  }
  return CONTINUE_CHAIN | CONTINUE_DEFAULT; // BaseObjScript::CallEvent(event,data);
}

// --------------
// Energy orb
// --------------
short EnergyOrb::CallEvent(int event, tOSIRISEventInfo *data) {
  switch (event) {
  case EVT_COLLIDE:
    if (!called) {
      mprintf(0, "Collide with energy\n");
      msafe_struct mo;

      mo.objhandle = data->evt_collide.it_handle;
      MSafe_GetValue(MSAFE_OBJECT_TYPE, &mo);
      MSafe_GetValue(MSAFE_OBJECT_ID, &mo);

      if (mo.type == OBJ_PLAYER) {

        MSafe_GetValue(MSAFE_OBJECT_ENERGY, &mo);
        if (mo.energy < 200) {
          mo.energy += 15;
          if (mo.energy > 200)
            mo.energy = 200;
          MSafe_CallFunction(MSAFE_OBJECT_ENERGY, &mo);

          mo.slot = mo.id;
          mo.color = GR_RGB(0, 255, 0);
          sprintf(mo.message, "Energy boosted to %d", (int)mo.energy);
          MSafe_CallFunction(MSAFE_MISC_HUD_MESSAGE, &mo);
          called = true;
        }
      }
    }
    break;
  }
  return CONTINUE_CHAIN | CONTINUE_DEFAULT;
}