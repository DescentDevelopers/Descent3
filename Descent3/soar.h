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
 * $Logfile: /DescentIII/Main/soar.h $
 * $Revision: 4 $
 * $Date: 4/16/99 12:33a $
 * $Author: Matt $
 *
 * Header for Soar DLL functions
 *
 * $Log: /DescentIII/Main/soar.h $
 *
 * 4     4/16/99 12:33a Matt
 * Disable Soar on non-Windows systems.
 *
 */

#ifndef SOAR_H_
#define SOAR_H_

// #ifdef _WIN32
// #define SOAR_ENABLED
// #endif

#ifdef SOAR_ENABLED

extern bool Soar_active;

extern __declspec(dllexport) int SoarInit(void *TerrainList, int terrain_size, void *RoomList, int room_size,
                                          void *ObjectList, int object_size, void *FunctionList);
extern __declspec(dllexport) int SoarTick(float FrameTime);
extern __declspec(dllexport) int SoarCreateAgent(int Objnum, int Flags);
extern __declspec(dllexport) int SoarClearAgent(int Objnum, int Flags);
extern __declspec(dllexport) int SoarDestroyAgent(int Objnum, int Flags);
extern __declspec(dllexport) int SoarEnd(void);

#else

#define Soar_active false

// I've defined all of these to not return values, even though the functions they
// replay return ints, because no one actually looks at the return values

#define SoarInit(TerrainList, terrain_size, RoomList, room_size, ObjectList, object_size, FunctionList)                \
  do {                                                                                                                 \
  } while (0)
#define SoarTick(FrameTime)                                                                                            \
  do {                                                                                                                 \
  } while (0)
#define SoarCreateAgent(Objnum, Flags)                                                                                 \
  do {                                                                                                                 \
  } while (0)
#define SoarClearAgent(Objnum, Flags)                                                                                  \
  do {                                                                                                                 \
  } while (0)
#define SoarDestroyAgent(Objnum, Flags)                                                                                \
  do {                                                                                                                 \
  } while (0)
#define SoarEnd()                                                                                                      \
  do {                                                                                                                 \
  } while (0)

#endif

#endif