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

#if _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <MMSystem.h>

unsigned int platform_timeGetTime(void) { return timeGetTime(); }

#else
#include <unistd.h>
#include <sys/time.h>

unsigned int platform_timeGetTime(void) {
  struct timeval t;
  gettimeofday(&t, NULL);

  return (t.tv_sec * 1000 + t.tv_usec / 1000);
}

#endif
