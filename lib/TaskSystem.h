/*
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

typedef enum tTaskPriority { TASKPRIORITY_HIGHEST, TASKPRIORITY_NORMAL, TASKPRIORITY_LOWEST } tTaskPriority;

//	Since this system handles concepts such as multitasking, and the such,
//	we will define the event structures differently, since different systems
//	handle multitasking differently.

class osEvent {
#if defined(DD_ACCESS_RING)
public:
#else
private:
#endif // DD_ACCESS_RING_0
#if defined(WIN32)
  unsigned event_os_handle; // this is the Win32 Event Handle
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
#if defined(DD_ACCESS_RING)
public:
#else
private:
#endif // DD_ACCESS_RING_0
#if defined(WIN32)
  unsigned task_os_handle; // This is the Win32 EventHandle
  unsigned task_os_id;     // Win32 Thread ID
#endif                     // WIN32

public:
  osTask(unsigned (*func)(void *), tTaskPriority priority, void *parm = NULL);
  ~osTask();

  bool error() const; // reports error

  void suspend(); // suspends task
  void resume();  // resumes task
};

#endif
