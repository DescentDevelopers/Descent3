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

 * $Source: f:/miner/source/bios/rcs/mono.h $
 * $Revision: 9 $
 * $Author: Kevin $
 * $Date: 7/28/99 3:16p $
 *
 * Header for monochrome/mprintf functions
 *
 * $Log: /DescentIII/Main/lib/mono.h $
 *
 * 9     7/28/99 3:16p Kevin
 * Mac Stuff
 *
 * 8     1/26/99 9:44p Jeff
 * moved tcplog functions to mono library
 *
 * 7     1/09/99 4:39p Jeff
 * added some ifdefs and fixes to get files to compile under Linux
 *
 * 6     10/18/98 8:54p Matt
 * Fixed macro which used an if statement not inside of a do..while block
 *
 * 5     10/13/98 12:03p Kevin
 * Changed use of preprocessors for debug, etc.
 *
 * 4     10/09/98 12:52p Samir
 * mono define also defines mprintf.
 *
 * 3     4/22/98 3:50p Chris
 * Added DebugBlockPrint
 *
 * 2     9/16/97 5:06p Matt
 * Disable mprintf() & mprintf_at() when _DEBUG not set
 *
 * 1     6/23/97 9:25p Samir
 * added because source safe sucks
 *
 * 3     6/11/97 1:11p Samir
 * Implemented new Debug system
 *
 * 2     3/10/97 12:29p Samir
 * Moved mono code and altered lowlevel console interface to machine
 * library.
 *
 */
#ifndef _MONO_H
#define _MONO_H
#include "debug.h"
void nw_InitTCPLogging(char *ip, uint16_t port);
void nw_TCPPrintf(int n, char *format, ...);
#if (!defined(RELEASE)) && defined(LOGGER)
extern bool Debug_print_block;
// Prints a formatted string to the debug window
#define mprintf(args) Debug_ConsolePrintf args
// Prints a formatted string on window n at row, col.
#define mprintf_at(args) Debug_ConsolePrintfAt args
#define DebugBlockPrint(args)                                                                                          \
  do {                                                                                                                 \
    if (Debug_print_block)                                                                                             \
      mprintf_at((1, 5, 51, args));                                                                                    \
  } while (0)
#else // ifdef _DEBUG
#define mprintf(args) // DAJ defined in target headers
#define mprintf_at(args)
#define DebugBlockPrint(args)
#endif // ifdef _DEBUG
#endif
