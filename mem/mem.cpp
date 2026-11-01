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

 * $Logfile: /DescentIII/Main/mem/mem.cpp $
 * $Revision: 56 $
 * $Date: 1/03/02 5:27p $
 * $Author: Matt $
 *
 * Memory library
 *
 * $Log: /DescentIII/Main/mem/mem.cpp $
 *
 * 56    1/03/02 5:27p Matt
 * Add a longlong cast to prevent math overflow.
 *
 * 55    4/19/00 5:33p Matt
 * From Duane for 1.4
 * Close renderer on malloc fail error exit
 * Mac changes
 *
 * 54    10/21/99 2:25p Kevin
 * Mac merge
 *
 * 53    8/10/99 5:12p Jeff
 * added a debug function to dump the current state of the mem library to
 * file
 *
 * 52    7/28/99 2:22p Kevin
 * Macintosh Stuff
 *
 * 51    5/13/99 5:06p Ardussi
 * changes for compiling on the Mac
 *
 * 50    5/02/99 12:24a Jason
 * mem was incorrectly choosing lowmem mode on 64 meg machines
 *
 * 49    4/30/99 5:07p Kevin
 * misc dedicated server, networking and low memory enhancements
 *
 * 48    4/16/99 10:42p Jeff
 * moved global variables out of window scope code to all builds for linux
 *
 * 47    4/15/99 11:09p Jeff
 * fixed mem_strdup for linux
 *
 * 46    4/15/99 9:22p Jeff
 * Changes for Linux version
 *
 * 45    4/12/99 2:23p Kevin
 * fixed missing symbol
 *
 * 44    4/12/99 1:13p Kevin
 * Moved call to GetLastError and played with some debugging
 *
 * 43    3/17/99 11:19a 3dsmax
 * Took out debugging
 *
 * 42    3/16/99 4:49p Kevin
 * Fixed realloc
 *
 * 41    3/15/99 4:32p Jeff
 * fixed code so mem library compiles correctly
 *
 * 40    3/15/99 11:24a Kevin
 * Improved memory leak detection for mem_strdup and turned on debugging
 *
 * 39    3/14/99 2:31p Kevin
 * Added mem_heapcheck() function for debugging
 *
 * 38    3/02/99 5:50p Kevin
 * Ouch. Duplicate structures existed and were conflicting.
 *
 * 37    2/28/99 6:30p Kevin
 * Added a call to Error() when unable to alloc memory
 *
 * 36    1/09/99 4:40p Jeff
 * added some ifdefs and fixes to get files to compile under Linux
 *
 * 35    1/05/99 3:46p Matt
 * For Kevin, added call to GetLastError()
 *
 * 34    11/05/98 2:00p Sean
 *
 * 33    11/05/98 11:23a Kevin
 * fixed bug that caused it not to work under NT
 *
 * 32    10/30/98 11:23a Samir
 *
 * 31    10/30/98 11:10a Kevin
 * doh! fixed ifdef
 *
 * 30    10/30/98 11:01a Kevin
 * took out mem debug stuff for nt users (temp)
 *
 * 29    10/28/98 5:01p Kevin
 * Improved debugging aids in mem lib
 *
 * 28    10/21/98 7:18p Matt
 * Added quick-exit system to not free individual mem chunks on exit,
 * since the whole heap gets freed at the end.
 *
 * 27    10/18/98 9:11p Matt
 * Took the program name out of some error messages.
 *
 * 26    10/18/98 6:21p Matt
 * Changed a Debug_ConsolePrintf() to mprintf() since the former doesn't
 * exist in a Release build and the latter compiles out.
 *
 * 25    10/17/98 12:46p Kevin
 * Beta 4 fixes
 *
 * 24    10/15/98 12:15p Kevin
 * put in heap checking code
 *
 * 23    10/15/98 11:59a Kevin
 * removed useage of timer_GetTime and FindArg
 *
 * 22    10/14/98 11:25p Jeff
 * commented out call to HeapDestroy() to keep NT users from crashing on
 * exit because it destroys the heap before Sound thread closes
 *
 * 21    10/14/98 7:20p Kevin
 * More dsp changes...
 *
 * 20    10/14/98 4:39p Matt
 * Changed OutrageMessageBox() calls to use either Error() or
 * EditorMessageBox()
 *
 * 19    10/13/98 3:42p Kevin
 * bug fixes
 *
 * 18    10/12/98 8:39p Kevin
 * removed mprintf's and fixed some smallish bugs
 *
 * 17    10/12/98 11:32a Kevin
 * doh! heap size was 160 megs
 *
 * 16    10/12/98 11:25a Kevin
 * doh!
 *
 * 15    10/12/98 10:55a Kevin
 * Don't use memory lib for editor currently
 *
 * 14    10/12/98 10:22a Samir
 * made mem_strdup take a const char pointer.
 *
 * 13    10/09/98 7:40p Kevin
 *
 * 12    10/09/98 6:55p Kevin
 * fized bug with zero byte mallocs and mem_size
 *
 * 11    10/09/98 3:32p Kevin
 * New memory library
 *
 * 10    10/08/98 4:24p Kevin
 * Changed code to comply with memory library usage. Always use mem_malloc
 * , mem_free and mem_strdup
 *
 * 9     10/08/98 2:40p Kevin
 *
 * 8     10/08/98 12:00p Kevin
 * Demo system work
 *
 * 7     9/22/98 3:55p Samir
 * ifdef out stuff for non-debug version.
 *
 * 6     8/31/98 12:42p Samir
 * added some debug code.
 *
 * 5     7/31/98 5:44p Samir
 * improved memory debugging.
 *
 * $NoKeywords: $
 */

#if defined(MACOSX)
#include <malloc/malloc.h>
#else
#include <malloc.h>
#endif

#include "mem.h"

bool Mem_low_memory_mode = false;
bool Mem_superlow_memory_mode = false;

int mem_size(void *memblock) {
#if defined(WIN32)
  return static_cast<int>(_msize(memblock));
#elif defined(MACOSX)
  return static_cast<int>(malloc_size(memblock));
#else
  return static_cast<int>(malloc_usable_size(memblock));
#endif
}
