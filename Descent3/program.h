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

 * $Logfile: /DescentIII/main/program.h $
 * $Revision: 40 $
 * $Date: 8/30/01 4:31p $
 * $Author: Matt $
 *
 * program state header
 *
 * $Log: /DescentIII/main/program.h $
 *
 * 40    8/30/01 4:31p Matt
 * Updated version number to 1.5
 *
 * 39    4/19/00 5:37p Matt
 * Changed version number to 1.4 (accidentally left it as 1.4.1 last time)
 *
 * 38    3/21/00 10:20a Matt
 * Changed to version 1.4
 *
 * 37    10/18/99 4:03p Chris
 * Merc is 1.3
 *
 * 35    10/18/99 1:27p Kevin
 * Added cf_IsFileInHog
 *
 * 34    9/29/99 8:04a Kevin
 *
 * 33    7/06/99 5:55p Kevin
 * 1.1 patch for the US version
 *
 * 32    6/16/99 12:03p Kevin
 * version
 *
 * 31    6/10/99 4:12p Kevin
 * Fixed SetLevel in dedicated server for HEAT.NET, and added
 * level_names.str for level name localization.
 *
 * 30    5/03/99 5:24p Kevin
 * Version number
 *
 * 29    5/01/99 10:41a Kevin
 * beta 6
 *
 * 28    4/28/99 4:14p Kevin
 *
 * 27    4/25/99 12:19p Kevin
 * version change
 *
 * 26    4/21/99 9:47a Kevin
 * beta 3
 *
 * 25    4/18/99 1:18p Kevin
 * Demo2 build changes
 *
 * 24    4/15/99 9:25a Kevin
 * CD check fixes and updates
 *
 * 23    4/07/99 9:19a Kevin
 * Changed version number for full version beta 1(?)
 *
 * 22    3/22/99 10:14a Kevin
 * Added a build number to the release builds of main. Also put in code to
 * increment the build number each time a release build is done.
 *
 * 21    3/19/99 4:36p Kevin
 *
 * 20    3/16/99 11:26a Kevin
 * Changed version number for OEM build
 *
 * 19    3/03/99 7:21p Jeff
 * updated program version for oem final
 *
 * 18    3/03/99 1:03a Kevin
 * Version Change
 *
 * 17    3/02/99 8:50p Jeff
 *
 * 16    3/02/99 11:52a Kevin
 * Fixes for OEM Beta 4.1
 *
 * 15    3/01/99 9:03p Kevin
 * OEM Beta 4
 *
 * 14    2/26/99 6:48p Kevin
 * Fixes to load/save game, and change minor version for OEM beta 3
 *
 * 13    2/24/99 3:35p Kevin
 * Changed OEM minor version to 2
 *
 * 12    2/20/99 12:33p Kevin
 * Added some OEM stuff
 *
 * 11    10/23/98 1:09p Samir
 * Demo Release 1.0
 *
 * 10    10/22/98 11:07a Samir
 * Demo Beta 7!
 *
 * 9     10/20/98 3:52p Samir
 * Demo Beta 6
 *
 * 8     10/19/98 7:48p Samir
 * beta 5 demo
 *
 * 7     10/16/98 1:11p Samir
 * Beta 4 Demo
 *
 * 6     10/13/98 5:51p Samir
 * beta 3
 *
 * 5     10/12/98 1:45p Samir
 * added version key to database.
 *
 * 4     10/08/98 10:48p Samir
 * version 2.0
 *
 * 3     10/08/98 7:30p Samir
 * changed version numbers.
 *
 * 2     10/02/98 12:30p Samir
 * added version information for beta/demo.
 *
 * 4     2/11/97 3:17p Samir
 * Tookout editor defines.
 *
 * 3     2/06/97 4:04p Samir
 * Added editor and beta defines
 *
 * 2     2/04/97 5:07p Samir
 * Added windowed state to determine if we run in a window
 *
 * 1     2/03/97 5:07p Samir
 * Program version info
 *
 * $NoKeywords: $
 */

#ifndef PROGRAM_H
#define PROGRAM_H

#include "d3_version.h"
#include "pstypes.h"

#define DEVELOPMENT_VERSION 0x1 // without editor: with debug, no beta
#define RELEASE_VERSION 0x2     // final release candidate: no debug, beta, editor

#define BETA_VERSION 0x1000 // beta modifier.
#define DEMO_VERSION 0x2000 // same as release, but its the demo.

struct program_version {
  int version_type;
  uint8_t major, minor, build;
  bool debug : 1;    // are we in debug mode
  bool beta : 1;     // are we in beta testing mode
  bool release : 1;  // are we a final release candidate
  bool editor : 1;   // editor code?
  bool windowed : 1; // runs in a window?
  bool demo : 1;     // demo?
};

//	Program state available to everyone
extern program_version Program_version;

//	Useful macros
#define PROGRAM(_c) Program_version._c

//	functions
void ProgramVersion(int version_type, uint8_t major, uint8_t minor, uint8_t build);

#endif
