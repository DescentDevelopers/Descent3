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

 * $Logfile: /DescentIII/Main/lib/TaskSystem.h $
 * $Revision: 3 $
 * $Date: 9/14/98 4:02p $
 * $Author: Samir $
 *
 * Task manager
 *
 * $Log: /DescentIII/Main/lib/TaskSystem.h $
 *
 * 3     9/14/98 4:02p Samir
 * allow variable lengths of blocking for tasks.
 *
 * 2     8/10/98 5:54p Samir
 * added mutexes.
 *
 * 1     5/23/97 4:07p Samir
 * Initial revision.
 *
 * $NoKeywords: $
 */

#ifndef TASKSYSTEM_H
#define TASKSYSTEM_H

#include "pstypes.h"

enum tTaskPriority { TASKPRIORITY_HIGHEST, TASKPRIORITY_NORMAL, TASKPRIORITY_LOWEST };

//	Since this system handles concepts such as multitasking, and the such,
//	we will define the event structures differently, since different systems
//	handle multitasking differently.

class osEvent {
public:
#if defined(WIN32)
  void *event_os_handle;    // this is the Win32 Event Handle
#endif                      // WIN32

public:
  osEvent(char *name);
  ~osEvent();

  bool error() const; // reports error

  void signal();                // signal the event so blocking can stop
  void clear();                 // clear the event so blocking can continue
  bool block(int timeout = -1); // block until signaled (-1 = full blocking, else time in ms to block)
};

class osTask {
public:
#if defined(WIN32)
  void *task_os_handle;     // This is the Win32 Thread Handle
#endif                      // WIN32

public:
  osTask(unsigned (*func)(void *), tTaskPriority priority, void *parm = NULL);
  ~osTask();

  bool error() const; // reports error

  void suspend(); // suspends task
  void resume();  // resumes task
};

#endif
