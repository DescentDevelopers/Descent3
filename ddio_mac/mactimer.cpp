/*
 * $Logfile: /DescentIII/Main/ddio_mac/mactimer.cpp $
 * $Revision: 1.1.1.1 $
 * $Date: 2003/08/26 03:56:55 $
 * $Author: kevinb $
 *
 * Macintosh timer
 *
 * $Log: mactimer.cpp,v $
 * Revision 1.1.1.1  2003/08/26 03:56:55  kevinb
 * initial 1.5 import
 *
 *
 * 5     3/20/00 12:43p Matt
 * Merge of Duane's post-1.3 changes.
 *
 * 4     10/21/99 3:33p Jeff
 * Macintosh merges
 *
 * 3     7/28/99 3:31p Kevin
 * Mac Stuff!
 *
 * 2     5/10/99 10:28p Ardussi
 * changes to compile on Mac
 *
 * 3     5/15/97 1:43 AM Jeremy
 * changed mprintf's to be standard (with newline at end)
 *
 * 2     5/7/97 11:26 AM Jeremy
 * completed implementation of timer based on microseconds (still need to
 * implement hook functions)
 *
 * $NoKeywords: $
 */
// ANSI Headers
#include <stdlib.h>
// Macintosh Headers
#include <Math64.h>
#include <Timer.h>
#include <MacTypes.h>
#include <DriverServices.h>
#include <InputSprocket.h>
// Descent 3 Headers
#include "ddio_mac.h"
#include "ddio.h"
#include "pserror.h"
#include "mono.h"
#include "maccontroller.h"
//===========================================
//	Local Data Types
//===========================================
class CTimerInfo {
public:
  CTimerInfo(void);

  bool mInitted;
  float mElapsedTime;
  float mStartTime;
};
timer_info ktimer_info;
UniversalProcPtr timer_proc = NULL;
//===========================================
//	File Level Globals
//===========================================
static CTimerInfo gTimerInfo;
static unsigned char Installed = 0;
//===========================================
//	Helpful Inline Functions
//===========================================
//===========================================
//	Functions
//===========================================
CTimerInfo::CTimerInfo(void) {
  mInitted = false;
  mElapsedTime = 0.0;
  mStartTime = 0.0;
}
bool timer_Init(int preemptive, bool /*force_lores*/) {
  mprintf((0, "Initializing Timer.\n"));
  //	long double		currentTimeLD	= 0.0;
  //	UInt64			currentTimeUI	= 0;

  // Get the current time in microseconds
  //	Microseconds((UnsignedWide*) &currentTimeUI);
  // Convert to a long double
  //	currentTimeLD = UInt64ToLongDouble(currentTimeUI);

  float currentTime;
  UnsignedWide startmicro;
  AbsoluteTime startAbs = UpTime();
  ISpTimeToMicroseconds(&startAbs, &startmicro);
  //	currentTimeLD = UInt64ToLongDouble(* (UInt64 *) &startmicro);
  currentTime = (float)(*(UInt64 *)&startmicro);

  // Convert from microseconds to seconds
  gTimerInfo.mStartTime = currentTime / 1000000;
  gTimerInfo.mElapsedTime = 0;

  // Timer Initted
  gTimerInfo.mInitted = true;

  return (gTimerInfo.mInitted);
}
void timer_Close() {
  mprintf((0, "Closing Timer.\n"));
  gTimerInfo.mInitted = false;
  gTimerInfo.mElapsedTime = 0.0;
}
//	returns time in seconds
float timer_GetTime() {
  if (gTimerInfo.mInitted) {
    //		long double		currentTimeLD	= 0.0;
    //		UInt64			currentTimeUI	= 0;

    // Get the current time in microseconds
    //		Microseconds((UnsignedWide*)(&currentTimeUI));
    // Convert to a long double
    //		currentTimeLD = UInt64ToLongDouble(currentTimeUI);

    float currentTime;
    UnsignedWide startmicro;
    AbsoluteTime startAbs = UpTime();
    ISpTimeToMicroseconds(&startAbs, &startmicro);
    //		currentTimeLD = UInt64ToLongDouble(* (UInt64 *) &startmicro);
    currentTime = (float)(*(UInt64 *)&startmicro);

    // Convert from microseconds to seconds
    gTimerInfo.mElapsedTime = (currentTime / 1000000);
    gTimerInfo.mElapsedTime -= gTimerInfo.mStartTime;
  }

  return gTimerInfo.mElapsedTime;
}
longlong timer_GetMSTime() {
  UInt64 currentTimeUI = 0;

  // Get the current time in microseconds
  //	Microseconds((UnsignedWide*)(&currentTimeUI));
  //	return(currentTimeUI>>10);

  UnsignedWide startmicro;
  AbsoluteTime startAbs = UpTime();
  ISpTimeToMicroseconds(&startAbs, &startmicro);
  return ((*(UInt64 *)&startmicro) >> 10);
}
/*
//	hook in timer function at certain period.  returns a handle to this function
int timer_HookFunction(void (*fncptr)(), int period)
{
}
//	clears function from hook list specified by a handle returned from HookFunction
void timer_ReleaseFunction(int func)
{
}
*/
// Interrupt time routines
void interrupt_Handler(timer_info *tminfo) {
  MacKeyboardHandler();
  MacInSprocketHandler();

  PrimeTime((QElemPtr)(&ktimer_info), INTERRUPT_MS);
}
void interrupt_Close() {
  if (!Installed)
    return;
  _INTERRUPT_DISABLE();
  Installed = 0;
}
void interrupt_Flush() {
  if (!Installed)
    interrupt_Init();

  _INTERRUPT_DISABLE();
  FlushEvents(everyEvent, 0);
  _INTERRUPT_ENABLE();
}
void interrupt_Init() {
#ifdef DAJ_DEBUG
  return;
#endif
  if (Installed)
    return;
  Installed = 1;

  SetEventMask(0xffff);
  LMSetSysEvtMask(0xffff);

  //  now set up the structure for the keyboard timer
  if (timer_proc == NULL) {
    timer_proc = NewTimerProc(interrupt_Handler);
  }

  _INTERRUPT_ENABLE();

  // Clear the keyboard array
  interrupt_Flush();
  atexit(interrupt_Close);
}
