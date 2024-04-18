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

// scripttest1.cpp
// 0.1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "osiris_import.h"
#include "osiris_common.h"

#include "module.h"

#ifdef __cplusplus
extern "C" {
#endif
char STDCALL InitializeDLL(tOSIRISModuleInit *func_list);
void STDCALL ShutdownDLL(void);
int STDCALL GetGOScriptID(char *name, ubyte isdoor);
void STDCALLPTR CreateInstance(int id);
void STDCALL DestroyInstance(int id, void *ptr);
short STDCALL CallInstanceEvent(int id, void *ptr, int event, tOSIRISEventInfo *data);
int STDCALL GetTriggerScriptID(int trigger_room, int trigger_face);
int STDCALL GetCOScriptList(int **list, int **id_list);
int STDCALL SaveRestoreState(void *file_ptr, ubyte saving_state);
#ifdef __cplusplus
}
#endif

// ==========================
// Script class definitions//
// ==========================
#define ID_SHIELD_ORB 0x00 // shield id
#define MAX_IDS 0x01       // maximum number of GOS IDs

#define ID_CUST_SHIELD_ORB 0x01
#define ID_MY_TRIGGER 0x02
#define ID_MY_DOOR 0x03

typedef struct {
  int id;
  char *name;
} tScriptInfo;

tScriptInfo ScriptInfo[MAX_IDS] = {{ID_SHIELD_ORB, "Shield"}};

class BaseObjScript {
public:
  BaseObjScript();
  ~BaseObjScript();
  virtual short CallEvent(int event, tOSIRISEventInfo *data);

protected:
  bool called;
};

class MyTrigger {
public:
  short CallEvent(int event, tOSIRISEventInfo *data);
};

class MyDoor {
public:
  short CallEvent(int event, tOSIRISEventInfo *data);
};

//------------------
// Shield orb script
//------------------
class ShieldOrb : public BaseObjScript {
public:
  short CallEvent(int event, tOSIRISEventInfo *data);
};

class CustomShieldOrb : public BaseObjScript {
public:
  short CallEvent(int event, tOSIRISEventInfo *data);
};

char STDCALL InitializeDLL(tOSIRISModuleInit *func_list) {
  osicommon_Initialize((tOSIRISModuleInit *)func_list);
  return 1;
}

void STDCALL ShutdownDLL(void) {}

int STDCALL GetGOScriptID(char *name, ubyte isdoor) {
  if (!isdoor) {
    for (int i = 0; i < MAX_IDS; i++) {
      if (!stricmp(name, ScriptInfo[i].name)) {
        return ScriptInfo[i].id;
      }
    }
  } else {
    if (!stricmp("MARK'S OLD DOOR", name))
      return ID_MY_DOOR;
  }
  return -1;
}

void STDCALLPTR CreateInstance(int id) {
  switch (id) {
  case ID_SHIELD_ORB:
    return new ShieldOrb;
    break;
  case ID_CUST_SHIELD_ORB:
    return new CustomShieldOrb;
    break;
  case ID_MY_TRIGGER:
    return new MyTrigger;
    break;
  case ID_MY_DOOR:
    return new MyDoor;
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
  case ID_CUST_SHIELD_ORB:
    delete ((CustomShieldOrb *)ptr);
    break;
  case ID_MY_TRIGGER:
    delete ((MyTrigger *)ptr);
    break;
  case ID_MY_DOOR:
    delete ((MyDoor *)ptr);
    break;
  default:
    mprintf(0, "SCRIPT: Illegal ID (%d)\n", id);
    break;
  };
}

short STDCALL CallInstanceEvent(int id, void *ptr, int event, tOSIRISEventInfo *data) {
  switch (id) {
  case ID_SHIELD_ORB:
  case ID_CUST_SHIELD_ORB:
    return ((BaseObjScript *)ptr)->CallEvent(event, data);
    break;
  case ID_MY_TRIGGER:
    return ((MyTrigger *)ptr)->CallEvent(event, data);
  case ID_MY_DOOR:
    return ((MyDoor *)ptr)->CallEvent(event, data);
  default:
    mprintf(0, "SCRIPT: Illegal ID (%d)\n", id);
    break;
  };

  return CONTINUE_CHAIN | CONTINUE_DEFAULT;
}

int STDCALL GetTriggerScriptID(int trigger_room, int trigger_face) {
  if (trigger_room == 2 && trigger_face == 1) {
    return ID_MY_TRIGGER;
  }
  return -1;
}

int STDCALL GetCOScriptList(int **list, int **id_list) {
  static int cust_handle_list[1] = {0x403};
  static int cust_id_list[1] = {ID_CUST_SHIELD_ORB};

  *list = cust_handle_list;
  *id_list = cust_id_list;
  return 1;
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
  mprintf(0, "BaseObjScript::CallEvent()\n");
  return CONTINUE_CHAIN | CONTINUE_DEFAULT;
}

// --------------
// Shield orb
// --------------
short ShieldOrb::CallEvent(int event, tOSIRISEventInfo *data) {
  switch (event) {
  case EVT_COLLIDE:
    if (!called) {
      mprintf(0, "Collide with LEVEL shield\n");
      called = true;
    }
    break;
  }
  return CONTINUE_CHAIN | CONTINUE_DEFAULT;
}

short CustomShieldOrb::CallEvent(int event, tOSIRISEventInfo *data) {
  switch (event) {
  case EVT_COLLIDE:
    if (!called) {
      mprintf(0, "Collide with CUSTOM shield\n");
      called = true;
    }
    break;
  }
  return CONTINUE_CHAIN | CONTINUE_DEFAULT;
}

short MyTrigger::CallEvent(int event, tOSIRISEventInfo *data) {
  mprintf(0, "TRIGGER: %d\n", event);
  return CONTINUE_CHAIN | CONTINUE_DEFAULT;
}

short MyDoor::CallEvent(int event, tOSIRISEventInfo *data) {
  switch (event) {
  case EVT_COLLIDE: {
    msafe_struct mo;
    mo.objhandle = data->me_handle;
    MSafe_GetValue(MSAFE_OBJECT_ID, &mo);

    mo.doorway_index = mo.id;
    mprintf(0, "Activating doorway %d\n", mo.id);
    MSafe_CallFunction(MSAFE_DOOR_ACTIVATE, &mo);

  } break;
  };
  return CONTINUE_CHAIN | CONTINUE_DEFAULT;
}
