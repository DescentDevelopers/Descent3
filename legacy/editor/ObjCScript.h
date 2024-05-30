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

 * $Logfile: /DescentIII/Main/editor/ObjCScript.h $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:57:38 $
 * $Author: kevinb $
 *
 * Osiris Script loader for editor
 *
 * $Log: not supported by cvs2svn $
 * 
 * 19    6/23/98 3:04p Samir
 * sped up script list generation.
 * 
 * 18    5/04/98 3:22p Samir
 * ability to select which scripts you want to generate scriptwiz info
 * for.
 * 
 * 17    2/25/98 4:06p Samir
 * Added function to generate a table of all scripts on disk.
 * 
 * 16    1/22/98 6:22p Samir
 * Parse script parameters now.
 * 
 * 15    9/25/97 5:28p Samir
 * Even newer script code due to more changes in ObjCScript.cpp
 * 
 * 14    9/24/97 2:57p Samir
 * New ScriptWizard functionality, without extra commenting in files.
 * 
 * 13    9/22/97 5:59p Samir
 * Changed ObjCScript system, so everything is broken, but it shouldn't
 * crash the game.
 * 
 * 12    9/12/97 4:15p Samir
 * Upped number of mappable events.
 * 
 * 11    9/10/97 2:00p Samir
 * Changed prototype of AddScriptBlock
 * 
 * 10    9/09/97 1:04p Samir
 * Added code to create new script and save level script name.
 * 
 * 9     9/08/97 4:29p Samir
 * Added EVT_DESTROYED support to editing code.
 * 
 * 8     8/21/97 6:00p Samir
 * A whole lot of script editing stuff added for ScriptWizard.
 * 
 * 7     8/18/97 12:14p Samir
 * Added function to get the index of a script name in the script list.
 * 
 * 6     8/15/97 6:32p Samir
 * Use a more generic editor script interface instead of specifying
 * default scripts, etc.
 * 
 * 5     8/13/97 5:39p Samir
 * Use CompileScript to compile a script now, and we compile default
 * script at startup of editor.
 * 
 * 4     8/06/97 10:40a Samir
 * Added FreeLevelScript.
 * 
 * 3     8/04/97 4:17p Samir
 * Added externals for scripts in editor.
 * 
 * $NoKeywords: $
 */

#ifndef OBJCSCRIPT_H
#define OBJCSCRIPT_H

#include "pstypes.h"
#include "objscript.h"

#define MAX_SCRIPTS					256
#define MAX_SCRNAME					32
#define MAX_EVTNAME					32
#define MAX_SCRFILENAME				64
#define DEFAULT_SCRIPT_NAME		"default.scr"
#define LEVEL_SCRIPT_NAME			"level.scr"

#define DEFAULT_SCRIPT_MASK		0x01
#define CUSTOM_SCRIPT_MASK			0x02
#define ALL_SCRIPT_MASK				0xff

struct tD3XProgram;

//	Data structure for a default script name.  
// There exists a list of these names too.
struct tScriptName
{
// script info.
	char name[MAX_SCRNAME];
	uint16_t type;
	uint16_t line;
	bool iscustom;
	bool used;

//	parameters
	int nparms;
	char *paramtypes[MAX_SCRPARAMS];
	char *parameters[MAX_SCRPARAMS];

	
	tScriptName()
	{
		for (int i = 0; i < MAX_SCRPARAMS; i++)
		{
			parameters[i] = NULL;
			paramtypes[i] = NULL;
		}
		used = false;
		nparms = 0;
	}
	~tScriptName()
	{
		free_parameters();
		used = false;
	};

	void add_parameter(const char *type, const char *parm);
	void free_parameters();
}
tScriptName;

//	per level script.
extern char Level_script_name[];

//Currently loaded script file
extern char *Default_script_source;
extern char *Level_script_source;

//Table of scripts within script file
extern tScriptName Script_names[];

//Script event names
extern char *Script_evt_names[];
extern uint16_t Script_evt_ids[];


bool NewScript(const char *filename);
char *LoadScript(const char *filename);
void SaveScript(const char *filename, char *script);
void FreeScript(char *script);

// resets script list.
void ResetScriptList();

//	enumerates through script files.
char *StartScriptFileList();
char *GetNextScriptFile();
void EndScriptFileList();

// identify indices into script names array from the name.
int FindScriptIDFromName(const char *name);

//	find event id from it's name or vice-versa
char *FindEventNameFromID(int id);
int FindEventIDFromName(const char *name);

//	these functions add ScriptWizard formatting
char *AddScriptBlockToScript(char *script, const char *newname, const char *type_str);
char *AddEventBlockToScript(char *script, const char *evtname, const char *scriptname);

//	generates script list from all script files.
void GenerateScriptListFromAllFiles(int mask=ALL_SCRIPT_MASK);

//	generates script list from one file
void GenerateScriptListFromFile(const char *fname);

//	This function generates ScriptWizard comment header information for script files
//	if it doesn't already have it.  This should be called whenever a script file is loaded and compiled
//	via script wizard.
bool GenerateScriptWizardInfo(tD3XProgram *prog, bool custom);

//	This function actually tries to go throught the script source to find script names.
//	to find a script name we use the following algorithmn
bool GenerateScriptWizardInfo(char *script, bool custom);

//	Generates a program from a script
bool CompileScript(tD3XProgram *program, char *script);
void SaveScriptCode(const char *filename, tD3XProgram *program);


#endif
