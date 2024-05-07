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


#include "ddio_win.h"
#include "ddio.h"

#include <stdlib.h>

#include "pserror.h"
#include "mono.h"

#include <windows.h>
#include <mmsystem.h>

#define MAX_TIMER_HOOKS 4
#define TIMER_CLOCK_RATE 100 // ms

//	---------------------------------------------------------------------------
//	Variables
//	---------------------------------------------------------------------------

static bool Timer_initialized = 0;
static bool Timer_use_highres_timer = 0;
static bool Timer_firstinit = false;
static bool Timer_preemptive = 0;
static int Timer_resolution = 0;
static DWORD Timer_sys_start_time = 0;

bool timerhi_Init(void);
void timerhi_Close(void);
float ddio_TickToSeconds(LARGE_INTEGER ticks);
float timerhi_GetTime();
longlong timerhi_GetMSTime();
void timerhi_Normalize();
LARGE_INTEGER Timer_hi_sys_start_time;
LARGE_INTEGER Timer_hi_resolution;

void timer_Normalize();

//@@static void (*Timer_function_hook[MAX_TIMER_HOOKS])();
//@@static int Timer_function_period[MAX_TIMER_HOOKS];

//@@void CALLBACK timer_Proc(UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2);

//	---------------------------------------------------------------------------

bool timer_Init(int preemptive, bool force_lores) {
  TIMECAPS tc;
  MMRESULT mmresult;

  ASSERT(!Timer_initialized);

  if (force_lores) {
    Timer_use_highres_timer = false;
    mprintf((0, "Timer: Forcing to Lo-Res Timer\n"));
  } else {
    if (timerhi_Init())
      mprintf((0, "Timer: Hi-Resolution Timer available\n"));
    else
      mprintf((0, "Timer: Hi-Resolution Timer NOT available\n"));
  }

  if (!Timer_firstinit) {
    Timer_sys_start_time = GetTickCount();
    Timer_firstinit = true;
    atexit(timer_Close);
  }

  if (preemptive) {
    //	Initialize Win32 Timer system
    //@@	timeGetDevCaps(&tc, sizeof(tc));
    //@@	if (tc.wPeriodMin < 50) tc.wPeriodMin = 50;
    //@@
    //@@	Timer_resolution = tc.wPeriodMin;
    //@@
    //@@	mmresult = timeBeginPeriod(tc.wPeriodMin);
    //@@	if (mmresult != TIMERR_NOERROR) {
    //@@		Error("Unable to initialize Win32 timer (err:%x).\n", mmresult);
    //@@	}
    //@@
    //@@	Timer_preemptive = 1;
    //@@
    //@@ //	clear out timer function hook list
    //@@	for (int i = 0; i < MAX_TIMER_HOOKS;i++)
    //@@		Timer_function_hook[i] = NULL;
    //@@
    //@@	Timer_event_id = timeSetEvent(TIMER_CLOCK_RATE, Timer_resolution, timer_Proc, 0, TIME_PERIODIC);
    //@@	if (Timer_event_id == 0) Error("Unable to instantiate the timer procedure.\n");
    //@@
    //@@	mprintf((0, "Preemptive timer system initialized.\n"));
    mprintf((0, "No preemptive timer, default to non-preemptive!"));
    Int3();
    Timer_preemptive = 0;
  } else {
    mprintf((0, "Timer system initialized.\n"));
    Timer_preemptive = 0;
  }

  //	Initialize Win32 Timer system
  timeGetDevCaps(&tc, sizeof(tc));
  if (tc.wPeriodMin < 50)
    tc.wPeriodMin = 50;

  Timer_resolution = tc.wPeriodMin;

  mmresult = timeBeginPeriod(tc.wPeriodMin);
  if (mmresult != TIMERR_NOERROR) {
    // Error("Unable to initialize Win32 timer (err:%x).\n", mmresult);
  }

  Timer_initialized = 1;

  return 1;
}

void timer_Close() {
  ASSERT(Timer_initialized);

  //	mprintf((0, "Timer system closed.\n"));

  if (Timer_use_highres_timer) {
    timerhi_Close();
  } else {
    if (Timer_resolution) {
      timeEndPeriod(Timer_resolution);
      Timer_resolution = 0;
    }
  }

  Timer_initialized = 0;
}

float ddio_TickToSeconds(unsigned long ticks) {
  if (Timer_use_highres_timer) {
    LARGE_INTEGER t;
    t.QuadPart = ticks;
    return ddio_TickToSeconds(t);
  } else {
    timer_Normalize();

    DWORD time_ms;

    time_ms = ticks - Timer_sys_start_time;

    return ((float)time_ms / ((float)1000.0));
  }

  Int3(); // ack!!
  return 0;
}

float timer_GetTime() {
  if (Timer_use_highres_timer) {
    return timerhi_GetTime();
  } else {
    DWORD time_ms;

    timer_Normalize();
    time_ms = GetTickCount() - Timer_sys_start_time;

    return ((float)time_ms / ((float)1000.0));
  }

  Int3(); // ack!!
  return 0;
}

longlong timer_GetMSTime() {
  if (Timer_use_highres_timer) {
    return timerhi_GetMSTime();
  } else {
    DWORD time_ms;

    timer_Normalize();
    time_ms = GetTickCount() - Timer_sys_start_time;

    return time_ms;
  }

  Int3(); // ack!!
  return 0;
}

//	---------------------------------------------------------------------------
//	Internal functions
//	---------------------------------------------------------------------------
//	hook in timer function at certain period.  returns a handle to this function
DWORD timer_HookFunction(void(CALLBACK *fncptr)(UINT, UINT, DWORD, DWORD, DWORD), UINT delay) {
  DWORD time_event_id;

  time_event_id = timeSetEvent(delay, Timer_resolution, fncptr, 0, TIME_PERIODIC);

  return time_event_id;
}

//	clears function from hook list specified by a handle returned from HookFunction
void timer_ReleaseFunction(DWORD func) { timeKillEvent(func); }

void timer_Normalize() {
  DWORD new_time;

  new_time = GetTickCount();

  if (new_time < Timer_sys_start_time) {
    Timer_sys_start_time = new_time;
    return;
  }
}

//	---------------------------------------------------------------------------
//	Support for Win32 timer functions
//	We support a timer callback function which we can hook functions to.
//	Call function hooks at specified period
//	---------------------------------------------------------------------------

//@@void CALLBACK timer_Proc(UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2)
//@@{
//@@	static int ticks = 0;
//@@	static BOOL in_func = FALSE;
//@@	int i;
//@@
//@@/*	Hopefully this will prevent some reentrance, but the timer functions should be very minimal
//@@	and follow Win32 conventions
//@@*/
//@@	if (!in_func) {
//@@		in_func = TRUE;
//@@
//@@		for (i = 0; i < MAX_TIMER_HOOKS; i++)
//@@		{
//@@			if (Timer_function_hook[i] && !(ticks % Timer_function_period[i]))
//@@				(*Timer_function_hook[i])();
//@@		}
//@@
//@@		in_func = FALSE;
//@@	}
//@@
//@@	ticks++;
//@@	if (ticks == 256) ticks = 0;
//@@}

bool timerhi_Init(void) {
  // See if we can get the Windows hi res time
  Timer_use_highres_timer = (bool)(QueryPerformanceFrequency(&Timer_hi_resolution) != FALSE);

  if (!Timer_use_highres_timer)
    return false;

  // determine the start tick
  QueryPerformanceCounter(&Timer_hi_sys_start_time);
  return true;
}

void timerhi_Close(void) {
  // do any shutdown here!?
}

float ddio_TickToSeconds(LARGE_INTEGER ticks) {
  timerhi_Normalize();

  LARGE_INTEGER time_ms;

  time_ms.QuadPart = ticks.QuadPart - Timer_hi_sys_start_time.QuadPart;

  return (float)((double)time_ms.QuadPart / ((double)Timer_hi_resolution.QuadPart));
}

float timerhi_GetTime() {
  LARGE_INTEGER time_tick;

  timerhi_Normalize();

  QueryPerformanceCounter(&time_tick);

  return (float)(((double)time_tick.QuadPart - Timer_hi_sys_start_time.QuadPart) /
                 ((double)Timer_hi_resolution.QuadPart));
}

// This should return a timer in milliseconds
longlong timerhi_GetMSTime() {
  LARGE_INTEGER time_tick;

  timerhi_Normalize();

  QueryPerformanceCounter(&time_tick);

  return ((time_tick.QuadPart - Timer_hi_sys_start_time.QuadPart) / (Timer_hi_resolution.QuadPart / 1000));
}

void timerhi_Normalize() {
  LARGE_INTEGER new_time;

  QueryPerformanceCounter(&new_time);

  if (new_time.QuadPart < Timer_hi_sys_start_time.QuadPart) {
    Timer_hi_sys_start_time = new_time;
  }
}
