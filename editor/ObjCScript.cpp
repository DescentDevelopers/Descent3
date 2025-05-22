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

 * $Logfile: /DescentIII/Main/editor/ObjCScript.cpp $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:57:38 $
 * $Author: kevinb $
 *
 * Script loading and saving from editor.
 *
 * $Log: not supported by cvs2svn $
 *
 * 29    10/08/98 4:24p Kevin
 * Changed code to comply with memory library usage. Always use mem_malloc
 * , mem_free and mem_strdup
 *
 * 28    6/23/98 3:04p Samir
 * sped up script list generation.
 *
 * 27    5/04/98 3:22p Samir
 * ability to select which scripts you want to generate scriptwiz info
 * for.
 *
 * 26    4/21/98 12:16a Samir
 * Hopefully if you change script definition, it's reflected as you edit
 * it.
 *
 * 25    4/19/98 9:56p Chris
 * AI system is integrated with OSIRIS.  Path system is integrated with
 * the AI system.  Bugs will ensue.
 *
 * 24    4/15/98 5:56p Chris
 * Made the AI system script friendly
 *
 * 23    3/16/98 11:19a Jason
 * got scripts working with multiplayer
 *
 * 22    2/25/98 4:06p Samir
 * Added function to generate a table of all scripts on disk.
 *
 * 21    1/22/98 6:22p Samir
 * Parse script parameters now.
 *
 * 20    11/16/97 4:35p Samir
 * Took out annoying mprintfs.
 *
 * 19    9/25/97 5:28p Samir
 * Even newer script code due to more changes in ObjCScript.cpp
 *
 * 18    9/24/97 2:56p Samir
 * New ScriptWizard functionality, without extra commenting in files.
 *
 * 17    9/22/97 5:59p Samir
 * Changed ObjCScript system, so everything is broken, but it shouldn't
 * crash the game.
 *
 * 16    9/12/97 4:14p Samir
 * Fixed up list of events and strings.
 *
 * 15    9/10/97 1:57p Samir
 * Redid FindScriptFile functions, adding scripts to script files involves
 * specifying a type also.
 *
 * 14    9/09/97 1:04p Samir
 * Added code to create new script and save level script name.
 *
 * 13    9/08/97 4:29p Samir
 * Added EVT_DESTROYED support to editing code.
 *
 * 12    9/04/97 4:39p Matt
 * Added includes needed as a result of removing includes from d3edit.h
 *
 * 11    8/21/97 6:00p Samir
 * A whole lot of script editing stuff added for ScriptWizard.
 *
 * 10    8/18/97 12:14p Samir
 * Added function to get the index of a script name in the script list.
 *
 * 9     8/15/97 6:32p Samir
 * Use a more generic editor script interface instead of specifying
 * default scripts, etc.
 *
 * 8     8/13/97 5:39p Samir
 * Use CompileScript to compile a script now, and we compile default
 * script at startup of editor.
 *
 * 7     8/12/97 3:23p Samir
 * D3XReallocProgram has more arguments.
 *
 * 6     8/06/97 10:38a Samir
 * Don't load in script obejct file.  Don't need to.
 *
 * 5     8/04/97 6:45p Samir
 * Fixed crash when freeing a null script.
 *
 * 4     8/04/97 4:13p Samir
 * Added code to load and save the default script code.
 *
 * $NoKeywords: $
 */

#include <filesystem>
#include <regex>

#include "stdafx.h"
#include "ObjCScript.h"
#include "editor.h"
#include "cfile.h"
#include "ddio.h"
#include "d3x.h"
#include "manage.h"
#include "mission.h"
#include "gamefile.h"
#include "pserror.h"
#include "mem.h"
#include "mono.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_SCRIPT_LINE_SIZE 800

static void GenerateScriptListFromFile(const char *fname);

//	script name for current level
char Level_script_name[MAX_SCRFILENAME];

// Currently loaded script file
char *Default_script_source = NULL;
char *Level_script_source = NULL;

// Table of scripts within script file
tScriptName Script_names[MAX_SCRIPTS];

#define MAX_SCREVTS 15
//	INCREMENT MAX_SCREVTS in ObjCScript when you add a new event!
char *Script_evt_names[MAX_SCREVTS] = {
    "EVT_AI_FRAME",       "EVT_AI_NOTIFY",       "EVT_AI_INIT",
    "EVT_CREATED",        "EVT_COLLIDE",         "EVT_DAMAGED",
    "EVT_INTERVAL",       "EVT_DESTROY",         "EVT_USE",
    "EVT_CLIENT_AI",      "EVT_CLIENT_CREATED",  "EVT_CLIENT_COLLIDE",
    "EVT_CLIENT_DAMAGED", "EVT_CLIENT_INTERVAL", "EVT_CLIENT_DESTROY",
};

#if 0 // LGT - EVT_X undefined
uint16_t Script_evt_ids[MAX_SCREVTS] = {
	EVT_AI_FRAME,
	EVT_AI_NOTIFY,
	EVT_AI_INIT,
	EVT_CREATED,
	EVT_COLLIDE,
	EVT_DAMAGED,
	EVT_INTERVAL,
	EVT_DESTROY,
	EVT_USE,
	EVT_CLIENT_AI,
	EVT_CLIENT_CREATED,
	EVT_CLIENT_COLLIDE,
	EVT_CLIENT_DAMAGED,
	EVT_CLIENT_INTERVAL,
	EVT_CLIENT_DESTROY
};
#endif

//	function to find first free slot in script list.
int FindFreeScriptSlot();
char *GotoScriptInText(char *text, const char *script);
int GenerateScriptParamInfo(int id, const char *script_text);

bool NewScript(const char *filename) {
  char path[_MAX_PATH];
  CFILE *file;

  ddio_MakePath(path, LocalLevelsDir, filename, NULL);

  //	create the file
  file = cfopen(path, "wt");
  if (!file)
    return false;

  cfclose(file);

  return true;
}

// Loads the script into into memory  (Don't forget to call FreeScript() to free the memory when done)
// Returns true on success
char *LoadScript(const char *filename) {
  char *source;
  char file_path[256];
  CFILE *file;
  CString temp;
  int size;
  char buffer[MAX_SCRIPT_LINE_SIZE];

  memset(buffer, 0, MAX_SCRIPT_LINE_SIZE);

  ddio_MakePath(file_path, LocalLevelsDir, filename, NULL);
  //	mprintf(0,"Loading script from %s\n",file_path);
  if (!cfexist(file_path)) {
    return false;
  }

  file = cfopen(file_path, "rt");
  do {
    size = cf_ReadString(buffer, MAX_SCRIPT_LINE_SIZE - 2, file);
    buffer[size] = '\xd';
    buffer[size + 1] = '\xa';
    buffer[size + 2] = '\0';
    temp += buffer;
    memset(buffer, 0, size);
  } while (!cfeof(file));

  char *tmp_script;
  size = strlen(temp.GetBuffer(1));
  tmp_script = (char *)mem_malloc(size + 1);
  ASSERT(tmp_script);
  strcpy(tmp_script, temp.GetBuffer(1));
  if (size > 2)
    tmp_script[size - 2] = '\0';
  temp = tmp_script;
  if (tmp_script)
    mem_free(tmp_script);

  cfclose(file);

  source = mem_rmalloc<char>(strlen(temp.GetBuffer(1)) + 1);
  if (!source)
    return false;
  strcpy(source, temp.GetBuffer(1));

  return source;
}

void SaveScript(const char *filename, char *script) {
  CFILE *file;
  char file_path[256];

  ddio_MakePath(file_path, LocalLevelsDir, filename, NULL);

  file = cfopen(file_path, "wt");
  cf_WriteString(file, script);
  //		mprintf(0,"Saving script to %s\n",file_path);
  cfclose(file);
}

void FreeScript(char *script) { mem_free(script); }

//	Generates a program from a script
bool CompileScript(tD3XProgram *program, char *script) {
#if 0 // LGT: D3XReallocProgram undefined. Legacy scripting system?
	int nscr, nstr, d3xlen;
	char *strbuf;								// string buffer grabbed from compile
	tD3XInstruction *ins;					// temporary holder for code
	tD3XPMap *map;								// temporary holder for map

	if (!script) {
		mprintf(1, "Unable to compile null script!\n");
		return false;
	}

	if (program == NULL) {
		mprintf(1, "You can't compile an uninitialized script!\n");
		return false;
	}

	if(!osi_Compile(script, &d3xlen, &ins, &nscr, &map, &nstr, &strbuf)) {
		mprintf(1, "Script failed to compile.\n");
		return false;
	}

	D3XReallocProgram(program, d3xlen, nscr, nstr);
	D3XLoadProgramFromComponents(program, ins, map, strbuf);

	return true;
#else
  return false;
#endif
}

void SaveScriptCode(const char *filename, tD3XProgram *program) {
  CFILE *file;
  char file_path[256];

  //	write out default script program
  ddio_MakePath(file_path, LocalLevelsDir, filename, NULL);
  file = cfopen(file_path, "wb");
  // D3XSaveProgram(file, program); // LGT: function undefined
  mprintf(0, "Saving %s.\n", filename);
  cfclose(file);
}

//////////////////////////////////////////////////////////////////////////////
//	LIST FUNCTIONS

inline int AddScriptToList(char *name, uint16_t type, bool custom) {
  int slot = FindFreeScriptSlot();
  strcpy(Script_names[slot].name, name);
  Script_names[slot].used = true;
  Script_names[slot].type = type;
  Script_names[slot].iscustom = custom;
  Script_names[slot].free_parameters();
  return slot;
}

//	Resets all entries in script list to null.
void ResetScriptList() {
  int i;
  for (i = 0; i < MAX_SCRIPTS; i++)
    Script_names[i].used = false;
}

//	This function generates ScriptWizard comment header information for script files
//	if it doesn't already have it.  This should be called whenever a script file is loaded and compiled
//	via script wizard.
//	returns true

bool GenerateScriptWizardInfo(tD3XProgram *prog, bool custom) {
#if 0 // LGT: Legacy scripting system?
	int i;

	if (!prog) {
		Int3();
		return false;
	}

	for (i = 0; i < prog->nscripts; i++)
	{
		int slot;
		slot = AddScriptToList(prog->map[i].name, prog->map[i].type, custom);
		Script_names[slot].line = 0;
	}

	return true;
#else
  return false;
#endif
}

//	This function actually tries to go throught the script source to find script names.
//	to find a script name we use the following algorithmn
//
//		find one line containing:
//			trigger || object || level && script in succession
//			next word will be the script name
//			following script name will be '(' then look for endscript
//			repeat.

bool GenerateScriptWizardInfo(char *script, bool custom) {
  char word[256];
  bool inword;
  int i, index, line;
  int state; // 0 = nothing, 1 = typeword, 2 = scriptword
  int slot, strsize;
  uint16_t type = 0xffff;

  inword = false;
  index = 0;
  state = 0;
  line = 1;
  slot = 0;

  if (script == NULL)
    return false;

  strsize = strlen(script);

  for (i = 0; i < strsize; i++) {
    char ch;

    ch = script[i];

    if (ch != ' ' && ch != '\t' && ch != '\r' && ch != '\n' && ch != '(' && ch != ')' && ch != ',') {
      //	no whitespace
      if (!inword)
        inword = true;
      if (inword)
        word[index++] = ch;
    } else {
      //	whitespace
      //	 word is complete, check if a keyword
      if (ch == '\n')
        line++;
      word[index] = 0;

      if (state == 0) {
        if (!strcmp(word, "object"))
          type = REF_OBJTYPE;
        else if (!strcmp(word, "trigger"))
          type = REF_TRIGTYPE;
        else if (!strcmp(word, "level"))
          type = REF_LEVELTYPE;
        else
          type = 0xffff;
        if (type != 0xffff) {
          state = 1;
        }
      } else if (state == 1) {
        if (!strcmp(word, "script"))
          state = 2;
        else
          state = 0;
      } else if (state == 2) {
        //	if we are in state 2, then we have PROBABLY FOUND a script name, saved_ptr defined.
        if (inword) {
          slot = AddScriptToList(word, type, custom);
          Script_names[slot].line = line;
          i += GenerateScriptParamInfo(slot, &script[i]);
          state = 3;
        }
      } else if (state == 3) {
        //	check for endscript, then we go to state 0
        if (!strcmp(word, "endscript"))
          state = 0;
      }
      index = 0;
      inword = false;
    }
  }

  return true;
}

//	create paramameter table for script
int GenerateScriptParamInfo(int id, const char *script_text) {
  char word[256];
  char saved_word[256];
  bool inword = false;
  int index = 0;
  int state = 0;
  int i, strsize = strlen(script_text);

  for (i = 0; i < strsize; i++) {
    char ch;

    ch = script_text[i];

    if (ch != ' ' && ch != '\t' && ch != '\r' && ch != '\n' && ch != '(' && ch != ')' && ch != ',') {
      //	no whitespace
      if (!inword)
        inword = true;
      if (inword)
        word[index++] = ch;
    } else {
      //	 word is complete, check if a keyword
      word[index] = 0;
      //	if we are in state 1, then we have PROBABLY FOUND a script name, make sure name matches
      if (state == 0) {
        //	in parameter definition	if we've reached a ')', reset state to start
        if (Script_names[id].nparms == MAX_SCRPARAMS) {
          OutrageMessageBox("Parameter parsing list size maxed.");
          return i;
        } else if (ch == ')')
          break;
        else if (inword) {
          strcpy(saved_word, word);
          state = 1;
        }
      } else if (state == 1) {
        // in parameter name
        if (inword) {
          Script_names[id].add_parameter(saved_word, word);
          state = 0; // read next parameter
        }
        if (ch == ')')
          break;
      }
      index = 0;
      inword = false;
    }
  }

  return i;
}

char *GotoScriptInText(char *text, const char *script) {
  char word[256];
  bool inword;
  int i, index, line;
  int state; // 0 = nothing, 1 = typeword, 2 = scriptword
  int strsize = strlen(text);

  inword = false;
  index = 0;
  state = 0;
  line = 1;

  for (i = 0; i < strsize; i++) {
    char ch;

    ch = text[i];

    if (ch != ' ' && ch != '\t' && ch != '\r' && ch != '\n' && ch != '(') {
      //	no whitespace
      if (!inword)
        inword = true;
      if (inword)
        word[index++] = ch;
    } else {
      //	whitespace
      if (ch == '\n')
        line++;
      if (inword) {
        //	 word is complete, check if a keyword
        word[index] = 0;
        if (!strcmp(word, "trigger") || !strcmp(word, "object") || !strcmp(word, "level")) {
          if (state == 0)
            state = 1;
          else
            state = 0; // type word out of sequence
        } else if (!strcmp(word, "script")) {
          if (state == 1)
            state = 2;
          else
            state = 0;
        } else {
          //	if we are in state 2, then we have PROBABLY FOUND a script name
          if (state == 2) {
            if (!strcmp(word, script))
              return &text[i];
            else
              state = 0;
          } else
            state = 0;
        }
        index = 0;
        inword = false;
      }
    }
  }

  return NULL;
}

//////////////////////////////////////////////////////////////////////////////
//	Script source management

int FindScriptIDFromName(const char *name) {
  int i;

  for (i = 0; i < MAX_SCRIPTS; i++)
    if (Script_names[i].used && strcmp(name, Script_names[i].name) == 0)
      return i;

  return -1;
}

char *FindEventNameFromID(int id) {
#if 0 // LGT: MAX_SCREVTS undefined
	for (int i = 0; i < MAX_SCREVTS; i++)
		if (id == Script_evt_ids[i]) return Script_evt_names[i];
#endif
  return NULL;
}

int FindEventIDFromName(const char *name) {
#if 0 // LGT: MAX_SCREVTS undefined
	for (int i = 0; i < MAX_SCREVTS; i++)
		if (strcmp(name, Script_evt_names[i]) == 0) return Script_evt_ids[i];
#endif
  return 0;
}

//	these functions add ScriptWizard formatting

#define SCRHDR_FORMAT "%s script %s()"
#define SCRIPT_FORMAT "// %s\r\n//\t<comments here>\r\n%s\r\n\r\n\r\nendscript\r\n\r\n\r\n"

char *AddScriptBlockToScript(char *script, const char *newname, const char *type_str) {
  char *newsrc, *strline;
  char newhdr[128];
  char newblk[384];
  int slot, line = 0, type = 0;

  strline = script;
  while (strline) {
    line++;
    strline = strchr(strline, '\n');
    if (strline)
      strline++;
  }
  if (!strcmp(type_str, "object"))
    type = REF_OBJTYPE;
  else if (!strcmp(type_str, "trigger"))
    type = REF_TRIGTYPE;
  else if (!strcmp(type_str, "level"))
    type = REF_LEVELTYPE;

  sprintf(newhdr, SCRHDR_FORMAT, type_str, newname);
  sprintf(newblk, SCRIPT_FORMAT, newname, newhdr);

  newsrc = mem_rmalloc<char>(strlen(script) + strlen(newblk) + 1);
  if (!newsrc) {
    mprintf(1, "Allocation failure in creating new script buffer.\n");
    return NULL;
  }

  sprintf(newsrc, "%s%s", script, newblk);
  slot = AddScriptToList((char *)newname, type, false);
  Script_names[slot].line = line;

  return newsrc;
}

#define EVT_STATEMENT "\r\n\r\nevent '%s'\r\n//insert code here\r\ncomplete\r\n"

char *AddEventBlockToScript(char *script, const char *evtname, const char *scriptname) {
  //	we must first find the requested script, then add the event BEFORE the next 'endscript'
  char *txtstart, *newtxt;
  char *preword;
  char word[256];
  bool inword;
  int i, index;
  int strsize;

  inword = false;
  index = 0;

  txtstart = GotoScriptInText(script, scriptname);
  if (!txtstart)
    return NULL;

  preword = txtstart;
  strsize = strlen(script);

  for (i = 0; i < strsize; i++) {
    char ch;

    ch = txtstart[i];

    if (ch != ' ' && ch != '\t' && ch != '\r' && ch != '\n') {
      //	no whitespace
      if (!inword)
        inword = true;
      if (inword)
        word[index++] = ch;
    } else {
      //	whitespace
      if (inword) {
        //	 word is complete, check if a keyword, save location of word if not a keyword
        word[index] = 0;
        if (!strcmp(word, "endscript")) {
          // okay, we can now add the event block at preword.
          int prelen = (int)(preword - script);
          int postlen = (int)strlen(script) - prelen;
          char *preblk = new char[prelen + 1];
          char *postblk = new char[postlen + 1];
          char evtblk[256];

          strncpy(preblk, script, prelen);
          preblk[prelen] = 0;
          strncpy(postblk, preword, postlen);
          postblk[postlen] = 0;
          sprintf(evtblk, EVT_STATEMENT, evtname);

          newtxt = mem_rmalloc<char>(strlen(preblk) + strlen(postblk) + strlen(evtblk) + 1);
          sprintf(newtxt, "%s%s%s", preblk, evtblk, postblk);

          delete[] postblk;
          delete[] preblk;

          return newtxt;
        } else
          preword = &txtstart[i];

        index = 0;
        inword = false;
      }
    }
  }

  return NULL;
}

//	generates script list from all script files.
void GenerateScriptListFromAllFiles(int mask) {
  //	initialize script list.
  ResetScriptList();

  std::filesystem::path dir = std::filesystem::path(LocalLevelsDir);
  //	compile all script files and place into script list.
  ddio_DoForeachFile(dir, std::regex(".+\\.scr"), [&mask](const std::filesystem::path& path){
    std::filesystem::path file = path.filename();
    if (!stricmp(PATH_TO_CSTR(file), DEFAULT_SCRIPT_NAME) && (mask & DEFAULT_SCRIPT_MASK))
      GenerateScriptListFromFile(PATH_TO_CSTR(file));
    else if ((mask & CUSTOM_SCRIPT_MASK) && stricmp(PATH_TO_CSTR(file), DEFAULT_SCRIPT_NAME))
      GenerateScriptListFromFile(PATH_TO_CSTR(file));
  });
}

//	generates script list from one file
void GenerateScriptListFromFile(const char *fname) {
  char *script;
  script = LoadScript(fname);
  if (script) {
    if (stricmp(fname, DEFAULT_SCRIPT_NAME) == 0)
      GenerateScriptWizardInfo(script, false);
    else
      GenerateScriptWizardInfo(script, true);

    FreeScript(script);
  }
}

//@@
//@@//	****************************************************************************
//@@//	INTERNAL FUNCTIONS
//@@//	****************************************************************************
//@@
//@@//	function to find first free slot in default script list.
int FindFreeScriptSlot() {
  int i;
  for (i = 0; i < MAX_SCRIPTS; i++)
    if (!Script_names[i].used)
      return i;

  return -1;
}

//	other stuff
void tScriptName::add_parameter(const char *type, const char *parm) {
  ASSERT(nparms < MAX_SCRPARAMS);

  paramtypes[nparms] = new char[strlen(type) + 1];
  parameters[nparms] = new char[strlen(parm) + 1];
  strcpy(parameters[nparms], parm);
  strcpy(paramtypes[nparms], type);
  nparms++;
}

void tScriptName::free_parameters() {
  for (int i = 0; i < nparms; i++) {
    if (parameters[i]) {
      delete[] (parameters[i]);
      parameters[i] = NULL;
    }
    if (paramtypes[i]) {
      delete[] (paramtypes[i]);
      paramtypes[i] = NULL;
    }
  }
  nparms = 0;
}
