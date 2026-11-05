/*
 * Descent 3
 * Copyright (C) 2026 Parallax Software
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
 * A level script for osiris_sandbox_tests that passes structures to the engine
 * with fields left unset, the way original scripts do. Each EVT_INTERVAL makes
 * one call, chosen by the event's me_handle (see ProbeCase in the test).
 *
 * Unset fields are filled with kGarbage, an address outside guest memory, so
 * the bridge rejects the call if it reads them.
 */

#include <string.h>

#include "osiris_import.h"
#include "osiris_common.h"

#define STDCALL __stdcall
#define STDCALLPTR *STDCALL

static void *const kGarbage = (void *)0xDEADBEEF;

extern "C" {
char STDCALL InitializeDLL(tOSIRISModuleInit *func_list);
void STDCALL ShutdownDLL(void);
int STDCALL GetGOScriptID(char *name, ubyte is_door);
void STDCALLPTR CreateInstance(int id);
void STDCALL DestroyInstance(int id, void *ptr);
short STDCALL CallInstanceEvent(int id, void *ptr, int event, tOSIRISEventInfo *data);
int STDCALL GetTriggerScriptID(int trigger_room, int trigger_face);
int STDCALL GetCOScriptList(int **list, int **id_list);
int STDCALL SaveRestoreState(void *file_ptr, ubyte saving_state);
}

static int g_instance;

char STDCALL InitializeDLL(tOSIRISModuleInit *func_list) {
  osicommon_Initialize(func_list);
  return 1;
}
void STDCALL ShutdownDLL(void) {}
int STDCALL GetGOScriptID(char *, ubyte) { return -1; }
void STDCALLPTR CreateInstance(int) { return &g_instance; }
void STDCALL DestroyInstance(int, void *) {}
int STDCALL GetTriggerScriptID(int, int) { return -1; }
int STDCALL GetCOScriptList(int **list, int **id_list) {
  *list = 0;
  *id_list = 0;
  return 0;
}
int STDCALL SaveRestoreState(void *, ubyte) { return 0; }

short STDCALL CallInstanceEvent(int, void *, int event, tOSIRISEventInfo *data) {
  if (event != EVT_INTERVAL)
    return CONTINUE_CHAIN | CONTINUE_DEFAULT;
  switch (data->me_handle) {
  case 1: { // a call that does not read the list: list and count are unset
    msafe_struct m;
    memset(&m, 0, sizeof(m));
    m.objhandle = 1234;
    m.list = kGarbage;
    m.count = -7;
    MSafe_CallFunction(MSAFE_OBJECT_NO_RENDER, &m);
  } break;
  case 2: { // the call that reads the list, with no list: must be refused
    msafe_struct m;
    memset(&m, 0, sizeof(m));
    m.list = 0;
    m.count = 3;
    MSafe_CallFunction(MSAFE_OBJECT_DESTROY_ROBOTS_EXCEPT, &m);
  } break;
  case 3:   // camera on a path: orient is unset
  case 4: { // camera on a point: orient is read, so it must be refused
    tGameCinematic c;
    memset(&c, 0, sizeof(c));
    c.flags = data->me_handle == 3 ? GCF_USEPATH : GCF_USEPOINT;
    c.orient = (matrix *)kGarbage;
    c.callback = 0;
    Cine_Start(&c, (char *)"probe");
  } break;
  case 5:   // CANNED_MOVE_PLAYER_FADE has no text: text_to_display is unset
  case 6: { // CANNED_LEVEL_INTRO reads the text, so it must be refused
    tCannedCinematicInfo c;
    memset(&c, 0, sizeof(c));
    c.type = data->me_handle == 5 ? CANNED_MOVE_PLAYER_FADE : CANNED_LEVEL_INTRO;
    c.text_to_display = (char *)kGarbage;
    c.target_objhandle = 55;
    Cine_StartCanned(&c);
  } break;
  }
  return CONTINUE_CHAIN | CONTINUE_DEFAULT;
}
