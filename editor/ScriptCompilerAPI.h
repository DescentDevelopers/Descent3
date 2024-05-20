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

* $Logfile: /DescentIII/Main/editor/ScriptCompilerAPI.h $
* $Revision: 1.1.1.1 $
* $Date: 2003-08-26 03:57:38 $
* $Author: kevinb $
*
* Header file for Script Compiler API functions
*
* $Log: not supported by cvs2svn $
 * 
 * 3     12/13/98 7:50p Jeff
 * automatically add new DLLs that aren't in the manage
 * system...implemented create new script function, even though it's still
 * in infancy.
 * 
 * 2     12/13/98 3:09a Jeff
 * Initial creation, added ScriptCompile() function
*
* $NoKeywords: $
*/


#ifndef __SCRIPTCOMPILERAPI_H_
#define __SCRIPTCOMPILERAPI_H_

#include "pstypes.h"

#define ST_LEVEL	0
#define ST_GAME		1
struct{
	char source_filename[_MAX_PATH];
	uint8_t script_type;
	void (*callback)(char *str);
}tCompilerInfo;

#define CERR_NOERR				0
#define CERR_NOCOMPILERDEFINED	1
#define CERR_COMPILERMISSING	2
#define CERR_SOURCENOEXIST		3


int ScriptCompile(tCompilerInfo *ci);
bool ScriptCreateEmptyScript(char *filename,uint8_t script_type);
#endif
