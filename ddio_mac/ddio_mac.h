/*
 * $Logfile: /DescentIII/Main/ddio_mac/ddio_mac.h $
 * $Revision: 1.1.1.1 $
 * $Date: 2003/08/26 03:56:55 $
 * $Author: kevinb $
 *
 * macintosh implementation of device dependent io functions
 *
 * $Log: ddio_mac.h,v $
 * Revision 1.1.1.1  2003/08/26 03:56:55  kevinb
 * initial 1.5 import
 *
 * 
 * 4     3/20/00 12:43p Matt
 * Merge of Duane's post-1.3 changes.
 * 
 * 3     10/21/99 3:33p Jeff
 * Macintosh merges
 * 
 * 2     7/28/99 3:31p Kevin
 * Mac Stuff!
 * 
 * 4     5/15/97 2:33 PM Jeremy
 * made macKeyboardHandler return true if more keys are waiting.  This way
 * it can be repeatedly called to get all pending keys or just called once
 * to get one key.  
 * 
 * 3     5/15/97 1:45 AM Jeremy
 * added keyboard handler prototype
 * 
 * 2     5/9/97 7:14 PM Jeremy
 * initial checkin
 * 
 * 1     3/13/97 6:53 PM Jeremy
 * macintosh implementation of device dependent io functions
 *
 * $NoKeywords: $
 */
#ifndef DDIO_MAC_H
#define DDIO_MAC_H
#include "pstypes.h"
// Keyboard handler which checks the OS for keyboard events
//  returns true if there are more keys waiting to be read, false if done
bool	MacKeyboardHandler(void);

#define INTERRUPT_MS	90	

// Interrupt timer data structures
typedef struct timer_info
{
	TMTask timer_task;
} timer_info;

extern timer_info ktimer_info;

extern UniversalProcPtr timer_proc;

// Interrupt macro functions

#define _INTERRUPT_DISABLE()	RmvTime((QElemPtr)(&ktimer_info))

#define _INTERRUPT_ENABLE() \
	ktimer_info.timer_task.tmAddr = timer_proc; \
	ktimer_info.timer_task.tmWakeUp = 0; \
	ktimer_info.timer_task.tmReserved = 0; \
	InsXTime((QElemPtr)(&ktimer_info)); \
	PrimeTime((QElemPtr)(&ktimer_info), INTERRUPT_MS);

// Interrupt timer routines

void interrupt_Init();
void interrupt_Handler(timer_info *tminfo);
void interrupt_Close();
void interrupt_Flush();

FILE * mac_fopen(char *filename, const char * open_mode);
FILE * FSp_fopen(ConstFSSpecPtr spec, const char * open_mode, OSType type);

void setup_sioux(void);	//DAJ debug console
#endif
