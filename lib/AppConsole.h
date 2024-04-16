/*
 * $Logfile: /DescentIII/Main/lib/AppConsole.h $
 * $Revision: 3 $
 * $Date: 10/30/98 5:52p $
 * $Author: Jeff $
 *
 * 
 *
 * $Log: /DescentIII/Main/lib/AppConsole.h $
 * 
 * 3     10/30/98 5:52p Jeff
 * implemented up arrow for last command, and esc to clear the line
 * 
 * 2     9/16/98 5:02p Samir
 * reimplemented X style console.
 * 
 * 1     9/14/98 4:02p Samir
 * new console library.
 *
 * $NoKeywords: $
 */

#ifndef APPCONSOLE_H
#define APPCONSOLE_H

#define CON_MAX_STRINGLEN	768
void con_Printf(const char *fmt, ...);
bool con_Input(char *buf, int buflen);

#endif