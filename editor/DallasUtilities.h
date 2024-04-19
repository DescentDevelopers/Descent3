/*
* $Logfile: /DescentIII/Main/editor/DallasUtilities.h $
* $Revision: 1.1.1.1 $
* $Date: 2003-08-26 03:57:37 $
* $Author: kevinb $
*
* Header for DALLAS related utility functions
*
* $Log: not supported by cvs2svn $
 * 
 * 5     2/19/99 5:35p Nate
 * Added new types and events
 * 
 * 4     12/16/98 8:45p Nate
 * Added loading of Actions
 * 
 * 3     12/15/98 7:48p Nate
 * Fixed minor buffering problem
 * 
 * 2     12/14/98 12:45p Nate
 * Initial Version
*
* $NoKeywords: $
*/

#ifndef _DALLAS_UTILITIES_H_
#define _DALLAS_UTILITIES_H_

#define WHITESPACE_CHARS	" \t\r\n"

// Removes any whitespace padding from the end of a string
void RemoveTrailingWhitespace(char *s);

// Returns a pointer to the first non-whitespace char in given string
char *SkipInitialWhitespace(char *s);


bool FindManageFirst(char *buffer,char *wildcard);
bool FindManageNext(char *buffer);
void FindManageClose(void);
bool GamefileExists(char *name);

#endif /*_DALLAS_UTILITIES_H_*/