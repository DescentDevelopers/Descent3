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

/*
 * $Logfile: /DescentIII/Main/mac/maccore.cpp $
 * $Revision: 1.1.1.1 $
 * $Date: 2003/08/26 03:58:14 $
 * $Author: kevinb $
 *
 * Operating system management library MacOS
 *
 * $Log: maccore.cpp,v $
 * Revision 1.1.1.1  2003/08/26 03:58:14  kevinb
 * initial 1.5 import
 *
 *
 * 3     10/21/99 1:55p Kevin
 * Mac Merge!
 *
 * 3     5/15/97 1:49 AM Jeremy
 * added some macintosh memory initialization to macOsObject and removed
 * keyboard handler (to put it into the d3MacOsObject)
 *
 * 2     5/9/97 7:16 PM Jeremy
 * #defines of some constants and functions from the dos world
 *
 * 1     2/28/97 12:16 PM Jeremy
 * MacOS specific OS  object libraries
 *
 * $NoKeywords: $
 */

// =========================
// ANSI Headers
// =========================
#include <string.h>
#include <ctype.h>

// =========================
// Macintosh System Headers
// =========================
#include <QuickDraw.h>
#include <Fonts.h>
#include <Windows.h>
#include <Menus.h>
#include <TextEdit.h>
#include <Dialogs.h>
#include <Memory.h>

// =========================
//	D3 Headers
// =========================
#include "gameos.h"
#include "mono.h"
#include "pserror.h"

// =========================
//	File Level Globals
// =========================

// =========================
// Private Function Prototypes
// =========================

// =========================
// Function Definitions
// =========================

void osMacObject::os_init(void) { ; }

osMacObject::osMacObject(void) {
  ::InitGraf(&(qd.thePort));
  ::InitFonts();
  ::InitWindows();
  ::InitMenus();
  ::TEInit();
  ::InitDialogs(nil);
  ::InitCursor();

  ::MaxApplZone();
  ::MoreMasters();
  ::MoreMasters();
  ::MoreMasters();
  ::MoreMasters();
}

osMacObject::~osMacObject(void) { ; }

void osMacObject::init(osObject *parent_os, void *info) { ; }

bool osMacObject::create(osObject *parent_os, void *info) {
  bool success = true;

  return (success);
}

gameos_packet *osMacObject::defer() {
  gameos_packet *packet = NULL;

  return packet;
}

void osMacObject::get_info(void *info, int size_str) {
  ASSERT(info);

  if (size_str > 0) {
    memset(info, 0, size_str);
  }
}

/*******************************************
 Utility functions specific to the Mac OS
*******************************************/
int stricmp(const char *s1, const char *s2) {
  char c1, c2;
  while (1) {
    c1 = tolower(*s1++);
    c2 = tolower(*s2++);
    if (c1 < c2)
      return -1;
    if (c1 > c2)
      return 1;
    if (c1 == 0)
      return 0;
  }
}

int strnicmp(const char *s1, const char *s2, int n) {
  int i;
  char c1, c2;
  for (i = 0; i < n; i++) {
    c1 = tolower(*s1++);
    c2 = tolower(*s2++);
    if (c1 < c2)
      return -1;
    if (c1 > c2)
      return 1;
    if (!c1)
      return 0;
  }
  return 0;
}