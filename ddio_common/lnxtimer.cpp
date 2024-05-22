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

 * $Logfile: /DescentIII/Main/ddio_lnx/lnxtimer.cpp $
 * $Revision: 1.2 $
 * $Date: 2000/06/29 08:50:17 $
 * $Author: icculus $
 *
 * Linux timer routines
 *
 * $Log: lnxtimer.cpp,v $
 * Revision 1.2  2000/06/29 08:50:17  icculus
 * Fixed to be more sane. Probably fixes lockup bug, too.
 *
 * Revision 1.1.1.1  2000/04/18 00:00:33  icculus
 * initial checkin
 *
 *
 * 7     8/20/99 7:19p Jeff
 * rewrote timer code...trying to get rid of mysterious bug
 *
 * 6     8/19/99 3:40p Jeff
 * possibly fix any holes with signed/unsigned issues
 *
 * 5     7/14/99 9:06p Jeff
 * added comment header
 *
 * $NoKeywords: $
 */

#include "ddio_lnx.h"
#include "ddio.h"

#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#include "pserror.h"
#include "mono.h"

#include "SDL.h"

//	---------------------------------------------------------------------------
//	Variables
//	---------------------------------------------------------------------------

static bool Timer_initialized = 0;
// rcg06292000 not used with SDL.
// static uint64_t Timer_sys_start_time = 0;
// static uint64_t Timer_accum = 0,Timer_high_mark = 0;
// void timer_Normalize();
// uint64_t timer_GetTickCount();

//	---------------------------------------------------------------------------

bool timer_Init(int preemptive, bool force_lores) {
  //	struct timeval t;
  //	gettimeofday(&t,NULL);
  //	Timer_sys_start_time = t.tv_sec*1000000.0 + t.tv_usec;
  //  Timer_sys_start_time = 0;

  Timer_initialized = 1;

  //	Timer_accum = 0;
  //	Timer_high_mark = 0;
  return true;
}

void timer_Close() {
  ASSERT(Timer_initialized);
  Timer_initialized = 0;
}

float ddio_TickToSeconds(uint32_t ticks) {
  // rcg06292000 not used with SDL.
  //	uint32_t time_ms;
  //	uint64_t new_ticks = ticks;

  //	timer_Normalize();
  //	time_ms = new_ticks;// - Timer_sys_start_time;

  //	return (float)((double)time_ms/((double)1000000.0));

  return (ticks / 1000.0);
}

float timer_GetTime() {
  // rcg06292000 ain't working.
  //	uint32_t time_ms;
  //	timer_Normalize();
  //	time_ms = timer_GetTickCount() - Timer_sys_start_time;
  //	return (float)((double)time_ms/((double)1000000.0));

  // rcg06292000 'dis sucka's MINE!
  return ((float)SDL_GetTicks() / 1000.0);
}

int64_t timer_GetMSTime() {
  // rcg06292000 not used with SDL.
  //	uint32_t time_ms;
  //	timer_Normalize();
  //	time_ms = timer_GetTickCount() - Timer_sys_start_time;
  //	return (int64_t)((double)time_ms/((double)1000.0));

  return (SDL_GetTicks());
}

//	---------------------------------------------------------------------------
//	Internal functions
//	---------------------------------------------------------------------------

// rcg06292000 not used with SDL.
/*
void timer_Normalize()
{
        uint64_t new_time;
        new_time = timer_GetTickCount();

        if (new_time < Timer_sys_start_time) {
                Timer_sys_start_time = new_time;
                return;
        }
}

uint64_t timer_GetTickCount(void)
{
        uint64_t ret;
        struct timeval t;
        gettimeofday(&t,NULL);

        ret = t.tv_sec*1000000.0 + t.tv_usec;

        if(ret > Timer_high_mark)
        {
                Timer_high_mark = ret;
        }else
        {
                // timer roll over
                mprintf((0,"TIMER: ROLL OVER\n"));
                if(Timer_high_mark>0)
                        Timer_accum += (Timer_high_mark - Timer_sys_start_time);
                Timer_high_mark = ret;
                Timer_sys_start_time = ret;
                ret = 100;//give some time
        }

        return Timer_accum + ret;
}
*/
