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

/////////////////////////////////////////////////////////////////////
// D.A.L.L.A.S. Generated Level Script - DLL Source File
//
// Filename:	gamegauge.cpp
// Version:	3
/////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "osiris_import.h"
#include "osiris_common.h"
#include "DallasFuncs.cpp"

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
int STDCALL GetGOScriptID(char *name, ubyte is_door);
void STDCALLPTR CreateInstance(int id);
void STDCALL DestroyInstance(int id, void *ptr);
short STDCALL CallInstanceEvent(int id, void *ptr, int event, tOSIRISEventInfo *data);
int STDCALL GetTriggerScriptID(int trigger_room, int trigger_face);
int STDCALL GetCOScriptList(int **list, int **id_list);
int STDCALL SaveRestoreState(void *file_ptr, ubyte saving_state);
#ifdef __cplusplus
}
#endif

// =================
// Script ID Numbers
// =================
#define ID_LEVEL_0000 0x000

#define ID_CUSTOM_OBJECT_0877 0x001
#define ID_CUSTOM_OBJECT_086B 0x002
#define ID_CUSTOM_OBJECT_0871 0x003
#define ID_CUSTOM_OBJECT_1002 0x004
#define ID_CUSTOM_OBJECT_400A 0x005

#define ID_TRIGGER_0005 0x006
#define ID_TRIGGER_0000 0x007

// ========================
// Script Class Definitions
// ========================

class BaseScript {
public:
  BaseScript();
  ~BaseScript();
  virtual short CallEvent(int event, tOSIRISEventInfo *data);
};

class LevelScript_0000 : public BaseScript {
public:
  short CallEvent(int event, tOSIRISEventInfo *data);
};

class CustomObjectScript_0877 : public BaseScript {
public:
  short CallEvent(int event, tOSIRISEventInfo *data);
};

class CustomObjectScript_086B : public BaseScript {
public:
  short CallEvent(int event, tOSIRISEventInfo *data);
};

class CustomObjectScript_0871 : public BaseScript {
public:
  short CallEvent(int event, tOSIRISEventInfo *data);
};

class CustomObjectScript_1002 : public BaseScript {
public:
  short CallEvent(int event, tOSIRISEventInfo *data);
};

class CustomObjectScript_400A : public BaseScript {
public:
  short CallEvent(int event, tOSIRISEventInfo *data);
};

class TriggerScript_0005 : public BaseScript {
public:
  short CallEvent(int event, tOSIRISEventInfo *data);
};

class TriggerScript_0000 : public BaseScript {
public:
  short CallEvent(int event, tOSIRISEventInfo *data);
};

// ======================
// Global Action Counters
// ======================

#define MAX_ACTION_CTR_VALUE 100000

int ScriptActionCtr_000 = 0;
int ScriptActionCtr_002 = 0;
int ScriptActionCtr_005 = 0;
int ScriptActionCtr_008 = 0;
int ScriptActionCtr_009 = 0;
int ScriptActionCtr_015 = 0;
int ScriptActionCtr_016 = 0;
int ScriptActionCtr_003 = 0;
int ScriptActionCtr_004 = 0;
int ScriptActionCtr_001 = 0;

// ========================================
// Function to Clear Global Action Counters
// ========================================
void ClearGlobalActionCtrs(void) {
  ScriptActionCtr_000 = 0;
  ScriptActionCtr_002 = 0;
  ScriptActionCtr_005 = 0;
  ScriptActionCtr_008 = 0;
  ScriptActionCtr_009 = 0;
  ScriptActionCtr_015 = 0;
  ScriptActionCtr_016 = 0;
  ScriptActionCtr_003 = 0;
  ScriptActionCtr_004 = 0;
  ScriptActionCtr_001 = 0;
}

// ========================================
// Function to Save Global Action Counters
// ========================================
void SaveGlobalActionCtrs(void *file_ptr) {
  File_WriteInt(ScriptActionCtr_000, file_ptr);
  File_WriteInt(ScriptActionCtr_002, file_ptr);
  File_WriteInt(ScriptActionCtr_005, file_ptr);
  File_WriteInt(ScriptActionCtr_008, file_ptr);
  File_WriteInt(ScriptActionCtr_009, file_ptr);
  File_WriteInt(ScriptActionCtr_015, file_ptr);
  File_WriteInt(ScriptActionCtr_016, file_ptr);
  File_WriteInt(ScriptActionCtr_003, file_ptr);
  File_WriteInt(ScriptActionCtr_004, file_ptr);
  File_WriteInt(ScriptActionCtr_001, file_ptr);
}

// ===========================================
// Function to Restore Global Action Counters
// ===========================================
void RestoreGlobalActionCtrs(void *file_ptr) {
  ScriptActionCtr_000 = File_ReadInt(file_ptr);
  ScriptActionCtr_002 = File_ReadInt(file_ptr);
  ScriptActionCtr_005 = File_ReadInt(file_ptr);
  ScriptActionCtr_008 = File_ReadInt(file_ptr);
  ScriptActionCtr_009 = File_ReadInt(file_ptr);
  ScriptActionCtr_015 = File_ReadInt(file_ptr);
  ScriptActionCtr_016 = File_ReadInt(file_ptr);
  ScriptActionCtr_003 = File_ReadInt(file_ptr);
  ScriptActionCtr_004 = File_ReadInt(file_ptr);
  ScriptActionCtr_001 = File_ReadInt(file_ptr);
}

// ===============================================================
// Start of Custom Script Block - DO NOT EDIT ANYTHING BEFORE THIS
// ===============================================================
/**{CUSTOM_SCRIPT_BLOCK_START}** DO NOT EDIT! **/

// Enter your custom script code here

/**{CUSTOM_SCRIPT_BLOCK_END}**** DO NOT EDIT! **/
// ============================================================
// End of Custom Script Block - DO NOT EDIT ANYTHING AFTER THIS
// ============================================================

// =================
// Message File Data
// =================

#define MAX_SCRIPT_MESSAGES 256
#define MAX_MSG_FILEBUF_LEN 1024
#define NO_MESSAGE_STRING "*Message Not Found*"
#define INV_MSGNAME_STRING "*Message Name Invalid*"
#define WHITESPACE_CHARS " \t\r\n"

// Structure for storing a script message
typedef struct {
  char *name;    // the name of the message
  char *message; // the actual message text
} tScriptMessage;

// Global storage for level script messages
tScriptMessage *message_list[MAX_SCRIPT_MESSAGES];
int num_messages;

// ======================
// Message File Functions
// ======================

// Initializes the Message List
void InitMessageList(void) {
  for (int j = 0; j < MAX_SCRIPT_MESSAGES; j++)
    message_list[j] = NULL;
  num_messages = 0;
}

// Clear the Message List
void ClearMessageList(void) {
  for (int j = 0; j < num_messages; j++) {
    free(message_list[j]->name);
    free(message_list[j]->message);
    free(message_list[j]);
    message_list[j] = NULL;
  }
  num_messages = 0;
}

// Adds a message to the list
int AddMessageToList(char *name, char *msg) {
  int pos;

  // Make sure there is room in the list
  if (num_messages >= MAX_SCRIPT_MESSAGES)
    return false;

  // Allocate memory for this message entry
  pos = num_messages;
  message_list[pos] = (tScriptMessage *)malloc(sizeof(tScriptMessage));
  if (message_list[pos] == NULL)
    return false;

  // Allocate memory for the message name
  message_list[pos]->name = (char *)malloc(strlen(name) + 1);
  if (message_list[pos]->name == NULL) {
    free(message_list[pos]);
    return false;
  }
  strcpy(message_list[pos]->name, name);

  // Allocate memory for the message name
  message_list[pos]->message = (char *)malloc(strlen(msg) + 1);
  if (message_list[pos]->message == NULL) {
    free(message_list[pos]->name);
    free(message_list[pos]);
    return false;
  }
  strcpy(message_list[pos]->message, msg);
  num_messages++;

  return true;
}

// Removes any whitespace padding from the end of a string
void RemoveTrailingWhitespace(char *s) {
  int last_char_pos;

  last_char_pos = strlen(s) - 1;
  while (last_char_pos >= 0 && isspace(s[last_char_pos])) {
    s[last_char_pos] = '\0';
    last_char_pos--;
  }
}

// Returns a pointer to the first non-whitespace char in given string
char *SkipInitialWhitespace(char *s) {
  while ((*s) != '\0' && isspace(*s))
    s++;

  return (s);
}

// Read in the Messages
int ReadMessageFile(char *filename) {
  void *infile;
  char filebuffer[MAX_MSG_FILEBUF_LEN + 1];
  char *line, *msg_start;
  int line_num;
  bool next_msgid_found;

  // Try to open the file for loading
  infile = File_Open(filename, "rt");
  if (!infile)
    return false;

  line_num = 0;
  next_msgid_found = true;

  // Clear the message list
  ClearMessageList();

  // Read in and parse each line of the file
  while (!File_eof(infile)) {

    // Clear the buffer
    strcpy(filebuffer, "");

    // Read in a line from the file
    File_ReadString(filebuffer, MAX_MSG_FILEBUF_LEN, infile);
    line_num++;

    // Remove whitespace padding at start and end of line
    RemoveTrailingWhitespace(filebuffer);
    line = SkipInitialWhitespace(filebuffer);

    // If line is a comment, or empty, discard it
    if (strlen(line) == 0 || strncmp(line, "//", 2) == 0)
      continue;

    if (!next_msgid_found) { // Parse out the last message ID number

      // Grab the first keyword, make sure it's valid
      line = strtok(line, WHITESPACE_CHARS);
      if (line == NULL)
        continue;

      // Grab the second keyword, and assign it as the next message ID
      line = strtok(NULL, WHITESPACE_CHARS);
      if (line == NULL)
        continue;

      next_msgid_found = true;
    } else { // Parse line as a message line

      // Find the start of message, and mark it
      msg_start = strchr(line, '=');
      if (msg_start == NULL)
        continue;
      msg_start[0] = '\0';
      msg_start++;

      // Add the message to the list
      AddMessageToList(line, msg_start);
    }
  }
  File_Close(infile);

  return true;
}

// Find a message
char *GetMessage(char *name) {
  // Make sure given name is valid
  if (name == NULL)
    return INV_MSGNAME_STRING;

  // Search message list for name
  for (int j = 0; j < num_messages; j++)
    if (strcmp(message_list[j]->name, name) == 0)
      return (message_list[j]->message);

  // Couldn't find it
  return NO_MESSAGE_STRING;
}

//======================
// Name List Arrays
//======================

#define NUM_DOOR_NAMES 0
char **Door_names = NULL;
int *Door_handles = NULL;

#define NUM_OBJECT_NAMES 21
char *Object_names[NUM_OBJECT_NAMES] = {
    "MatcenSwitch", "Blow-4",        "Blow-3",  "Blow-2",  "Blow-1",  "Blow-5",     "Blow-10",
    "Blow-9",       "Blow-6",        "Blow-8",  "Blow-7",  "Blow-13", "Blow-15",    "Blow-12",
    "Blow-11",      "GiantAssRadar", "Spewer2", "Spewer3", "Spewer1", "Clamp4Star", "PlayerShip"};
int Object_handles[NUM_OBJECT_NAMES];

#define NUM_ROOM_NAMES 1
char *Room_names[NUM_ROOM_NAMES] = {"CoolingShaft"};
int Room_indexes[NUM_ROOM_NAMES];

#define NUM_TRIGGER_NAMES 2
char *Trigger_names[NUM_TRIGGER_NAMES] = {"StartMatcen", "DestructionTrigger"};
int Trigger_indexes[NUM_TRIGGER_NAMES];
int Trigger_faces[NUM_TRIGGER_NAMES];
int Trigger_rooms[NUM_TRIGGER_NAMES];

#define NUM_SOUND_NAMES 1
char *Sound_names[NUM_SOUND_NAMES] = {"AmbSwitch31"};
int Sound_indexes[NUM_SOUND_NAMES];

#define NUM_TEXTURE_NAMES 0
char **Texture_names = NULL;
int *Texture_indexes = NULL;

#define NUM_PATH_NAMES 2
char *Path_names[NUM_PATH_NAMES] = {"IntroCam", "IntroShip"};
int Path_indexes[NUM_PATH_NAMES];

#define NUM_MATCEN_NAMES 1
char *Matcen_names[NUM_MATCEN_NAMES] = {"Matcen 1"};
int Matcen_indexes[NUM_MATCEN_NAMES];

#define NUM_GOAL_NAMES 0
char **Goal_names = NULL;
int *Goal_indexes = NULL;

#define NUM_MESSAGE_NAMES 4
char *Message_names[NUM_MESSAGE_NAMES] = {"DisableMatcen", "WeatherControl", "WindThing", "Intro"};
char *Message_strings[NUM_MESSAGE_NAMES];

// ===============
// InitializeDLL()
// ===============
char STDCALL InitializeDLL(tOSIRISModuleInit *func_list) {
  osicommon_Initialize((tOSIRISModuleInit *)func_list);
  if (func_list->game_checksum != CHECKSUM) {
    mprintf(0, "Game-Checksum FAIL!!! (%ul!=%ul)\n", func_list->game_checksum, CHECKSUM);
    mprintf(0, "RECOMPILE YOUR SCRIPTS!!!\n");
    return 0;
  }

  ClearGlobalActionCtrs();
  dfInit();
  InitMessageList();
  ReadMessageFile("gamegauge.msg");

  int j;

  // Do Door Index lookups
  for (j = 0; j < NUM_DOOR_NAMES; j++)
    Door_handles[j] = Scrpt_FindDoorName(Door_names[j]);

  // Do Object Index lookups
  for (j = 0; j < NUM_OBJECT_NAMES; j++)
    Object_handles[j] = Scrpt_FindObjectName(Object_names[j]);

  // Do Room Index lookups
  for (j = 0; j < NUM_ROOM_NAMES; j++)
    Room_indexes[j] = Scrpt_FindRoomName(Room_names[j]);

  // Do Trigger Index lookups
  for (j = 0; j < NUM_TRIGGER_NAMES; j++) {
    Trigger_indexes[j] = Scrpt_FindTriggerName(Trigger_names[j]);
    Trigger_faces[j] = Scrpt_GetTriggerFace(Trigger_indexes[j]);
    Trigger_rooms[j] = Scrpt_GetTriggerRoom(Trigger_indexes[j]);
  }

  // Do Sound Index lookups
  for (j = 0; j < NUM_SOUND_NAMES; j++)
    Sound_indexes[j] = Scrpt_FindSoundName(Sound_names[j]);

  // Do Texture Index lookups
  for (j = 0; j < NUM_TEXTURE_NAMES; j++)
    Texture_indexes[j] = Scrpt_FindTextureName(Texture_names[j]);

  // Do Path Index lookups
  for (j = 0; j < NUM_PATH_NAMES; j++)
    Path_indexes[j] = Scrpt_FindPathName(Path_names[j]);

  // Do Matcen Index lookups
  for (j = 0; j < NUM_MATCEN_NAMES; j++)
    Matcen_indexes[j] = Scrpt_FindMatcenName(Matcen_names[j]);

  // Do Goal Index lookups
  for (j = 0; j < NUM_GOAL_NAMES; j++)
    Goal_indexes[j] = Scrpt_FindLevelGoalName(Goal_names[j]);

  // Do Message Name lookups
  for (j = 0; j < NUM_MESSAGE_NAMES; j++)
    Message_strings[j] = GetMessage(Message_names[j]);

  return 1;
}

// =============
// ShutdownDLL()
// =============
void STDCALL ShutdownDLL(void) { ClearMessageList(); }

// ===============
// GetGOScriptID()
// ===============
int STDCALL GetGOScriptID(char *name, ubyte isdoor) { return -1; }

// ================
// CreateInstance()
// ================
void STDCALLPTR CreateInstance(int id) {
  switch (id) {
  case ID_LEVEL_0000:
    return new LevelScript_0000;
    break;
  case ID_CUSTOM_OBJECT_0877:
    return new CustomObjectScript_0877;
    break;
  case ID_CUSTOM_OBJECT_086B:
    return new CustomObjectScript_086B;
    break;
  case ID_CUSTOM_OBJECT_0871:
    return new CustomObjectScript_0871;
    break;
  case ID_CUSTOM_OBJECT_1002:
    return new CustomObjectScript_1002;
    break;
  case ID_CUSTOM_OBJECT_400A:
    return new CustomObjectScript_400A;
    break;
  case ID_TRIGGER_0005:
    return new TriggerScript_0005;
    break;
  case ID_TRIGGER_0000:
    return new TriggerScript_0000;
    break;
  default:
    mprintf(0, "SCRIPT: Illegal ID (%d)\n", id);
    break;
  }
  return NULL;
}

// =================
// DestroyInstance()
// =================
void STDCALL DestroyInstance(int id, void *ptr) {
  switch (id) {
  case ID_LEVEL_0000:
    delete ((LevelScript_0000 *)ptr);
    break;
  case ID_CUSTOM_OBJECT_0877:
    delete ((CustomObjectScript_0877 *)ptr);
    break;
  case ID_CUSTOM_OBJECT_086B:
    delete ((CustomObjectScript_086B *)ptr);
    break;
  case ID_CUSTOM_OBJECT_0871:
    delete ((CustomObjectScript_0871 *)ptr);
    break;
  case ID_CUSTOM_OBJECT_1002:
    delete ((CustomObjectScript_1002 *)ptr);
    break;
  case ID_CUSTOM_OBJECT_400A:
    delete ((CustomObjectScript_400A *)ptr);
    break;
  case ID_TRIGGER_0005:
    delete ((TriggerScript_0005 *)ptr);
    break;
  case ID_TRIGGER_0000:
    delete ((TriggerScript_0000 *)ptr);
    break;
  default:
    mprintf(0, "SCRIPT: Illegal ID (%d)\n", id);
    break;
  }
}

// ===================
// CallInstanceEvent()
// ===================
short STDCALL CallInstanceEvent(int id, void *ptr, int event, tOSIRISEventInfo *data) {
  switch (id) {
  case ID_LEVEL_0000:
  case ID_CUSTOM_OBJECT_0877:
  case ID_CUSTOM_OBJECT_086B:
  case ID_CUSTOM_OBJECT_0871:
  case ID_CUSTOM_OBJECT_1002:
  case ID_CUSTOM_OBJECT_400A:
  case ID_TRIGGER_0005:
  case ID_TRIGGER_0000:
    return ((BaseScript *)ptr)->CallEvent(event, data);
    break;
  default:
    mprintf(0, "SCRIPT: Illegal ID (%d)\n", id);
    break;
  }
  return CONTINUE_CHAIN | CONTINUE_DEFAULT;
}

// ==================
// SaveRestoreState()
// ==================
int STDCALL SaveRestoreState(void *file_ptr, ubyte saving_state) { return 0; }

// ====================
// GetTriggerScriptID()
// ====================
int STDCALL GetTriggerScriptID(int trigger_room, int trigger_face) {
  if (trigger_room == Trigger_rooms[0] && trigger_face == Trigger_faces[0])
    return ID_TRIGGER_0005;

  if (trigger_room == Trigger_rooms[1] && trigger_face == Trigger_faces[1])
    return ID_TRIGGER_0000;

  return -1;
}

// =================
// GetCOScriptList()
// =================
int STDCALL GetCOScriptList(int **list, int **id_list) {
  static int cust_handle_list[5];
  static int cust_id_list[5] = {ID_CUSTOM_OBJECT_0877, ID_CUSTOM_OBJECT_086B, ID_CUSTOM_OBJECT_0871,
                                ID_CUSTOM_OBJECT_1002, ID_CUSTOM_OBJECT_400A};

  // Fill in the custom handle list
  cust_handle_list[0] = Object_handles[0];
  cust_handle_list[1] = Object_handles[5];
  cust_handle_list[2] = Object_handles[6];
  cust_handle_list[3] = Object_handles[15];
  cust_handle_list[4] = Object_handles[19];

  *list = cust_handle_list;
  *id_list = cust_id_list;

  return 5;
}

//=======================
// Script Implementation
//=======================

BaseScript::BaseScript() {}

BaseScript::~BaseScript() {}

short BaseScript::CallEvent(int event, tOSIRISEventInfo *data) {
  mprintf(0, "BaseScript::CallEvent()\n");
  return CONTINUE_CHAIN | CONTINUE_DEFAULT;
}

short LevelScript_0000::CallEvent(int event, tOSIRISEventInfo *data) {
  switch (event) {
  case EVT_SAVESTATE: {
    tOSIRISEVTSAVESTATE *event_data = &data->evt_savestate;

    SaveGlobalActionCtrs(event_data->fileptr);
    dfSave(event_data->fileptr);
  } break;
  case EVT_RESTORESTATE: {
    tOSIRISEVTRESTORESTATE *event_data = &data->evt_restorestate;

    RestoreGlobalActionCtrs(event_data->fileptr);
    dfRestore(event_data->fileptr);
  } break;
  case EVT_LEVELSTART: {
    tOSIRISEVTLEVELSTART *event_data = &data->evt_levelstart;

    ClearGlobalActionCtrs();
    dfInit();

    // Script 003: Level INIT
    if (1) {
      aTurnOnSpew(Object_handles[16], -1, 2, 0.000000f, 0.000000f, 65536, 0, 1.500000f, 0.150000f, -1.000000f,
                  5.000000f, 15.000000f, 0, -1);
      aTurnOnSpew(Object_handles[17], -1, 2, 0.000000f, 0.000000f, 65536, 0, 1.500000f, 0.150000f, -1.000000f,
                  5.000000f, 15.000000f, 0, -1);
      aTurnOnSpew(Object_handles[18], -1, 2, 0.000000f, 0.000000f, 65536, 0, 1.500000f, 0.150000f, -1.000000f,
                  5.000000f, 15.000000f, 0, -1);

      // Increment the script action counter
      if (ScriptActionCtr_003 < MAX_ACTION_CTR_VALUE)
        ScriptActionCtr_003++;
    }

    // Script 001: LevelStart
    if (1) {
      aCinematicIntro(Path_indexes[0], Message_strings[3], Object_handles[20], Path_indexes[1], 4.000000f);

      // Increment the script action counter
      if (ScriptActionCtr_001 < MAX_ACTION_CTR_VALUE)
        ScriptActionCtr_001++;
    }
  } break;
  }
  return CONTINUE_CHAIN | CONTINUE_DEFAULT;
}

short CustomObjectScript_0877::CallEvent(int event, tOSIRISEventInfo *data) {
  switch (event) {
  case EVT_COLLIDE: {
    tOSIRISEVTCOLLIDE *event_data = &data->evt_collide;

    // Script 002: SwitchMatcen
    if ((ScriptActionCtr_002 < 1) && (qObjIsPlayerWeapon(event_data->it_handle) == true)) {
      aShowHUDMessage(Message_strings[0]);
      aMatcenSetState(0, Matcen_indexes[0]);
      aSoundPlayObject(Sound_indexes[0], data->me_handle, 1.000000f);
      aObjPlayAnim(data->me_handle, 0, 4, 2.000000f, 0);

      // Increment the script action counter
      if (ScriptActionCtr_002 < MAX_ACTION_CTR_VALUE)
        ScriptActionCtr_002++;
    }
  } break;
  }
  return CONTINUE_CHAIN | CONTINUE_DEFAULT;
}

short CustomObjectScript_086B::CallEvent(int event, tOSIRISEventInfo *data) {
  switch (event) {
  case EVT_TIMER: {
    tOSIRISEVTTIMER *event_data = &data->evt_timer;

    // Script 008: Blow Part 3
    if (1) {
      aObjDestroy(Object_handles[7]);
      aObjDestroy(Object_handles[8]);
      aObjDestroy(Object_handles[9]);
      aObjDestroy(Object_handles[10]);
      aObjDestroy(Object_handles[5]);

      // Increment the script action counter
      if (ScriptActionCtr_008 < MAX_ACTION_CTR_VALUE)
        ScriptActionCtr_008++;
    }
  } break;
  }
  return CONTINUE_CHAIN | CONTINUE_DEFAULT;
}

short CustomObjectScript_0871::CallEvent(int event, tOSIRISEventInfo *data) {
  switch (event) {
  case EVT_TIMER: {
    tOSIRISEVTTIMER *event_data = &data->evt_timer;

    // Script 009: Blow Part 5
    if (1) {
      aObjDestroy(Object_handles[11]);
      aObjDestroy(Object_handles[12]);
      aObjDestroy(Object_handles[13]);
      aObjDestroy(Object_handles[14]);
      aObjDestroy(Object_handles[6]);

      // Increment the script action counter
      if (ScriptActionCtr_009 < MAX_ACTION_CTR_VALUE)
        ScriptActionCtr_009++;
    }
  } break;
  }
  return CONTINUE_CHAIN | CONTINUE_DEFAULT;
}

short CustomObjectScript_1002::CallEvent(int event, tOSIRISEventInfo *data) {
  switch (event) {
  case EVT_COLLIDE: {
    tOSIRISEVTCOLLIDE *event_data = &data->evt_collide;

    // Script 016: Weather Control
    if ((ScriptActionCtr_016 < 1) && (qObjIsPlayerWeapon(event_data->it_handle) == true)) {
      aShowHUDMessage(Message_strings[1]);
      aLightningTurnOff();

      // Increment the script action counter
      if (ScriptActionCtr_016 < MAX_ACTION_CTR_VALUE)
        ScriptActionCtr_016++;
    }
  } break;
  case EVT_CREATED: {
    tOSIRISEVTCREATED *event_data = &data->evt_created;

    // Script 015: Start Weather
    if (1) {
      aLightningTurnOn(0.250000f, 0.200000f);

      // Increment the script action counter
      if (ScriptActionCtr_015 < MAX_ACTION_CTR_VALUE)
        ScriptActionCtr_015++;
    }
  } break;
  }
  return CONTINUE_CHAIN | CONTINUE_DEFAULT;
}

short CustomObjectScript_400A::CallEvent(int event, tOSIRISEventInfo *data) {
  switch (event) {
  case EVT_DESTROY: {
    tOSIRISEVTDESTROY *event_data = &data->evt_destroy;

    // If destroy event is due to level ending, don't run scripts
    if (!event_data->is_dying)
      break;

    // Script 004: ClampDestroyed
    if (1) {
      aShowHUDMessage(Message_strings[2]);
      aRoomSetLightingFlicker(0, Room_indexes[0]);

      // Increment the script action counter
      if (ScriptActionCtr_004 < MAX_ACTION_CTR_VALUE)
        ScriptActionCtr_004++;
    }
  } break;
  }
  return CONTINUE_CHAIN | CONTINUE_DEFAULT;
}

short TriggerScript_0005::CallEvent(int event, tOSIRISEventInfo *data) {
  switch (event) {
  case EVT_COLLIDE: {
    tOSIRISEVTCOLLIDE *event_data = &data->evt_collide;

    // Script 000: ActivateMatcen
    if ((ScriptActionCtr_000 < 1) && (1)) {
      aMatcenSetState(1, Matcen_indexes[0]);

      // Increment the script action counter
      if (ScriptActionCtr_000 < MAX_ACTION_CTR_VALUE)
        ScriptActionCtr_000++;
    }
  } break;
  }
  return CONTINUE_CHAIN | CONTINUE_DEFAULT;
}

short TriggerScript_0000::CallEvent(int event, tOSIRISEventInfo *data) {
  switch (event) {
  case EVT_COLLIDE: {
    tOSIRISEVTCOLLIDE *event_data = &data->evt_collide;

    // Script 005: BlowingTrigger
    if ((ScriptActionCtr_005 < 1) && (1)) {
      aObjDestroy(Object_handles[1]);
      aObjDestroy(Object_handles[2]);
      aObjDestroy(Object_handles[3]);
      aObjDestroy(Object_handles[4]);
      aSetObjectTimer(Object_handles[5], 1.000000f, -1);
      aSetObjectTimer(Object_handles[6], 2.000000f, -1);

      // Increment the script action counter
      if (ScriptActionCtr_005 < MAX_ACTION_CTR_VALUE)
        ScriptActionCtr_005++;
    }
  } break;
  }
  return CONTINUE_CHAIN | CONTINUE_DEFAULT;
}

/*********************************************************
Script Save Block: DO NOT TOUCH ANYTHING IN THIS BLOCK!!!
**********************************************************
$$SCRIPT_BLOCK_START

VERSION		3
NEXT_ID		17

// UserType value blocks
$$UTYPE_VALS_START UserVar
$$UTYPE_VALS_END

$$UTYPE_VALS_START UserFlag
$$UTYPE_VALS_END

$$UTYPE_VALS_START SpewHandle
$$UTYPE_VALS_END

$$UTYPE_VALS_START TimerID
$$UTYPE_VALS_END

$$UTYPE_VALS_START SavedObjectSlot
$$UTYPE_VALS_END

$$UTYPE_VALS_START GoalID
$$UTYPE_VALS_END


// Name Lists
$$DOOR_LIST_START
$$DOOR_LIST_END

$$OBJECT_LIST_START
MatcenSwitch
Blow-4
Blow-3
Blow-2
Blow-1
Blow-5
Blow-10
Blow-9
Blow-6
Blow-8
Blow-7
Blow-13
Blow-15
Blow-12
Blow-11
GiantAssRadar
Spewer2
Spewer3
Spewer1
Clamp4Star
PlayerShip
$$OBJECT_LIST_END

$$ROOM_LIST_START
CoolingShaft
$$ROOM_LIST_END

$$TRIGGER_LIST_START
StartMatcen
DestructionTrigger
$$TRIGGER_LIST_END

$$SOUND_LIST_START
AmbSwitch31
$$SOUND_LIST_END

$$TEXTURE_LIST_START
$$TEXTURE_LIST_END

$$SPECNAME_LIST_START
$$SPECNAME_LIST_END

$$PATH_LIST_START
IntroCam
IntroShip
$$PATH_LIST_END

$$MATCEN_LIST_START
Matcen 1
$$MATCEN_LIST_END

$$GOAL_LIST_START
$$GOAL_LIST_END

$$STRM_AUDIO_LIST_START
$$STRM_AUDIO_LIST_END


// Script Tree Dump
00:0:ActivateMatcen
        $$CHILD_BLOCK_START
        01:0:0
        02:1
        03:0
                $$CHILD_BLOCK_START
                05:0
                $$CHILD_BLOCK_END
        04:0:0:1
                $$CHILD_BLOCK_START
                08:aMatcenSetState
                        $$CHILD_BLOCK_START
                        10:10:0:1:Activate/Deactivate
                        10:17:0:0:MatcenName
                        $$CHILD_BLOCK_END
                $$CHILD_BLOCK_END
        $$CHILD_BLOCK_END
00:2:SwitchMatcen
        $$CHILD_BLOCK_START
        01:1:0
        02:0
        03:0
                $$CHILD_BLOCK_START
                05:1
                        $$CHILD_BLOCK_START
                        06:qObjIsPlayerWeapon:Bool
                                $$CHILD_BLOCK_START
                                10:1:1:-1:Object
                                $$CHILD_BLOCK_END
                        07:0:6
                        $$CHILD_BLOCK_END
                $$CHILD_BLOCK_END
        04:0:4864752:1
                $$CHILD_BLOCK_START
                08:aShowHUDMessage
                        $$CHILD_BLOCK_START
                        10:8:DisableMatcen:Message
                        $$CHILD_BLOCK_END
                08:aMatcenSetState
                        $$CHILD_BLOCK_START
                        10:10:0:0:Activate/Deactivate
                        10:17:0:0:MatcenName
                        $$CHILD_BLOCK_END
                08:aSoundPlayObject
                        $$CHILD_BLOCK_START
                        10:12:0:Sound
                        10:1:2:-1:Object
                        10:9:1.000000:Volume
                        $$CHILD_BLOCK_END
                08:aObjPlayAnim
                        $$CHILD_BLOCK_START
                        10:1:2:-1:Object
                        10:4:0:StartFrame
                        10:4:4:EndFrame
                        10:6:2.000000:CycleTime
                        10:5:0:Looping
                        $$CHILD_BLOCK_END
                $$CHILD_BLOCK_END
        $$CHILD_BLOCK_END
00:5:BlowingTrigger
        $$CHILD_BLOCK_START
        01:0:1
        02:1
        03:0
                $$CHILD_BLOCK_START
                05:0
                $$CHILD_BLOCK_END
        04:0:4864752:1
                $$CHILD_BLOCK_START
                08:aObjDestroy
                        $$CHILD_BLOCK_START
                        10:1:0:1:Object
                        $$CHILD_BLOCK_END
                08:aObjDestroy
                        $$CHILD_BLOCK_START
                        10:1:0:2:Object
                        $$CHILD_BLOCK_END
                08:aObjDestroy
                        $$CHILD_BLOCK_START
                        10:1:0:3:Object
                        $$CHILD_BLOCK_END
                08:aObjDestroy
                        $$CHILD_BLOCK_START
                        10:1:0:4:Object
                        $$CHILD_BLOCK_END
                08:aSetObjectTimer
                        $$CHILD_BLOCK_START
                        10:1:0:5:Object
                        10:6:1.000000:Time
                        10:10:0:-1:TimerID
                        $$CHILD_BLOCK_END
                08:aSetObjectTimer
                        $$CHILD_BLOCK_START
                        10:1:0:6:Object
                        10:6:2.000000:Time
                        10:10:0:-1:TimerID
                        $$CHILD_BLOCK_END
                $$CHILD_BLOCK_END
        $$CHILD_BLOCK_END
00:8:Blow Part 3
        $$CHILD_BLOCK_START
        01:1:5
        02:9
        03:0
                $$CHILD_BLOCK_START
                05:0
                $$CHILD_BLOCK_END
        04:0:4864752:0
                $$CHILD_BLOCK_START
                08:aObjDestroy
                        $$CHILD_BLOCK_START
                        10:1:0:7:Object
                        $$CHILD_BLOCK_END
                08:aObjDestroy
                        $$CHILD_BLOCK_START
                        10:1:0:8:Object
                        $$CHILD_BLOCK_END
                08:aObjDestroy
                        $$CHILD_BLOCK_START
                        10:1:0:9:Object
                        $$CHILD_BLOCK_END
                08:aObjDestroy
                        $$CHILD_BLOCK_START
                        10:1:0:10:Object
                        $$CHILD_BLOCK_END
                08:aObjDestroy
                        $$CHILD_BLOCK_START
                        10:1:0:5:Object
                        $$CHILD_BLOCK_END
                $$CHILD_BLOCK_END
        $$CHILD_BLOCK_END
00:9:Blow Part 5
        $$CHILD_BLOCK_START
        01:1:6
        02:9
        03:0
                $$CHILD_BLOCK_START
                05:0
                $$CHILD_BLOCK_END
        04:0:4864752:0
                $$CHILD_BLOCK_START
                08:aObjDestroy
                        $$CHILD_BLOCK_START
                        10:1:0:11:Object
                        $$CHILD_BLOCK_END
                08:aObjDestroy
                        $$CHILD_BLOCK_START
                        10:1:0:12:Object
                        $$CHILD_BLOCK_END
                08:aObjDestroy
                        $$CHILD_BLOCK_START
                        10:1:0:13:Object
                        $$CHILD_BLOCK_END
                08:aObjDestroy
                        $$CHILD_BLOCK_START
                        10:1:0:14:Object
                        $$CHILD_BLOCK_END
                08:aObjDestroy
                        $$CHILD_BLOCK_START
                        10:1:0:6:Object
                        $$CHILD_BLOCK_END
                $$CHILD_BLOCK_END
        $$CHILD_BLOCK_END
00:15:Start Weather
        $$CHILD_BLOCK_START
        01:1:15
        02:2
        03:0
                $$CHILD_BLOCK_START
                05:0
                $$CHILD_BLOCK_END
        04:0:4864752:0
                $$CHILD_BLOCK_START
                08:aLightningTurnOn
                        $$CHILD_BLOCK_START
                        10:6:0.250000:Seconds
                        10:9:0.200000:Probability
                        $$CHILD_BLOCK_END
                $$CHILD_BLOCK_END
        $$CHILD_BLOCK_END
00:16:Weather Control
        $$CHILD_BLOCK_START
        01:1:15
        02:0
        03:0
                $$CHILD_BLOCK_START
                05:1
                        $$CHILD_BLOCK_START
                        06:qObjIsPlayerWeapon:Bool
                                $$CHILD_BLOCK_START
                                10:1:1:-1:Object
                                $$CHILD_BLOCK_END
                        07:0:6
                        $$CHILD_BLOCK_END
                $$CHILD_BLOCK_END
        04:0:4864752:1
                $$CHILD_BLOCK_START
                08:aShowHUDMessage
                        $$CHILD_BLOCK_START
                        10:8:WeatherControl:Message
                        $$CHILD_BLOCK_END
                08:aLightningTurnOff
                $$CHILD_BLOCK_END
        $$CHILD_BLOCK_END
00:3:Level INIT
        $$CHILD_BLOCK_START
        01:2:0
        02:6
        03:0
                $$CHILD_BLOCK_START
                05:0
                $$CHILD_BLOCK_END
        04:0:0:0
                $$CHILD_BLOCK_START
                08:aTurnOnSpew
                        $$CHILD_BLOCK_START
                        10:1:0:16:Object
                        10:4:-1:GunNum
                        10:10:0:2:SpewType
                        10:6:0.000000:Mass
                        10:6:0.000000:Drag
                        10:15:65536:PhysicsFlags
                        10:5:0:IsRealObject
                        10:6:1.500000:BlobLifetime
                        10:6:0.150000:BlobInterval
                        10:6:-1.000000:SpewLife
                        10:6:5.000000:BlobSize
                        10:6:15.000000:BlobSpeed
                        10:5:0:Randomize
                        10:10:0:-1:SpewHandle
                        $$CHILD_BLOCK_END
                08:aTurnOnSpew
                        $$CHILD_BLOCK_START
                        10:1:0:17:Object
                        10:4:-1:GunNum
                        10:10:0:2:SpewType
                        10:6:0.000000:Mass
                        10:6:0.000000:Drag
                        10:15:65536:PhysicsFlags
                        10:5:0:IsRealObject
                        10:6:1.500000:BlobLifetime
                        10:6:0.150000:BlobInterval
                        10:6:-1.000000:SpewLife
                        10:6:5.000000:BlobSize
                        10:6:15.000000:BlobSpeed
                        10:5:0:Randomize
                        10:10:0:-1:SpewHandle
                        $$CHILD_BLOCK_END
                08:aTurnOnSpew
                        $$CHILD_BLOCK_START
                        10:1:0:18:Object
                        10:4:-1:GunNum
                        10:10:0:2:SpewType
                        10:6:0.000000:Mass
                        10:6:0.000000:Drag
                        10:15:65536:PhysicsFlags
                        10:5:0:IsRealObject
                        10:6:1.500000:BlobLifetime
                        10:6:0.150000:BlobInterval
                        10:6:-1.000000:SpewLife
                        10:6:5.000000:BlobSize
                        10:6:15.000000:BlobSpeed
                        10:5:0:Randomize
                        10:10:0:-1:SpewHandle
                        $$CHILD_BLOCK_END
                $$CHILD_BLOCK_END
        $$CHILD_BLOCK_END
00:4:ClampDestroyed
        $$CHILD_BLOCK_START
        01:1:19
        02:4
        03:0
                $$CHILD_BLOCK_START
                05:0
                $$CHILD_BLOCK_END
        04:0:0:0
                $$CHILD_BLOCK_START
                08:aShowHUDMessage
                        $$CHILD_BLOCK_START
                        10:8:WindThing:Message
                        $$CHILD_BLOCK_END
                08:aRoomSetLightingFlicker
                        $$CHILD_BLOCK_START
                        10:10:0:0:On/Off
                        10:2:0:Room
                        $$CHILD_BLOCK_END
                $$CHILD_BLOCK_END
        $$CHILD_BLOCK_END
00:1:LevelStart
        $$CHILD_BLOCK_START
        01:2:0
        02:6
        03:0
                $$CHILD_BLOCK_START
                05:0
                $$CHILD_BLOCK_END
        04:0:0:0
                $$CHILD_BLOCK_START
                08:aCinematicIntro
                        $$CHILD_BLOCK_START
                        10:16:0:PathName
                        10:8:Intro:Text
                        10:1:0:20:Target
                        10:16:1:TargetPath
                        10:6:4.000000:Seconds
                        $$CHILD_BLOCK_END
                $$CHILD_BLOCK_END
        $$CHILD_BLOCK_END

$$SCRIPT_BLOCK_END
*********************************************************/
