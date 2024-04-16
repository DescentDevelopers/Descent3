/*
 * $Logfile: /DescentIII/Main/lib/DDAccess.h $
 * $Revision: 3 $
 * $Date: 8/13/97 3:02p $
 * $Author: Samir $
 *
 * Device Dependant Access Manager.  (Allow access of libraries to OS information)
 *
 * $Log: /DescentIII/Main/lib/DDAccess.h $
 *
 * 3     8/13/97 3:02p Samir
 * A 'cleaner' way to do DirectInput mouse stuff.
 *
 * 2     8/01/97 7:31p Samir
 * Slightly better interface.
 *
 * 1     5/23/97 4:07p Samir
 * Device Dependant Data Access Manager.
 *
 * $NoKeywords: $
 */

#ifndef DDACCESS_H
#define DDACCESS_H

//	allows access to DD system information, such as OS specific info.

#define DD_ACCESS_RING

//	Win32 region of DD_ACCESS
//		Include the standard windows header since API calls are available in DD_ACCESS_RING
//		Define any additional messages too.

#if defined(WIN32)

//	Headers
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

//	Messages
#define WM_SYNCMOUSEACQUIRE (WM_USER + 0)

#endif // WIN32

#endif
