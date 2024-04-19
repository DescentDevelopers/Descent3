/*
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
typedef struct{
	char source_filename[_MAX_PATH];
	ubyte script_type;
	void (*callback)(char *str);
}tCompilerInfo;

#define CERR_NOERR				0
#define CERR_NOCOMPILERDEFINED	1
#define CERR_COMPILERMISSING	2
#define CERR_SOURCENOEXIST		3


int ScriptCompile(tCompilerInfo *ci);
bool ScriptCreateEmptyScript(char *filename,ubyte script_type);
#endif