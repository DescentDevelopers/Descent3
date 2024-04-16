/*
* $Logfile: /DescentIII/Main/linux/lnxtask.cpp $
* $Revision: 1.1.1.1 $
* $Date: 2000/04/18 00:00:39 $
* $Author: icculus $
*
* Linux multitasking routines
*
* $Log: lnxtask.cpp,v $
* Revision 1.1.1.1  2000/04/18 00:00:39  icculus
* initial checkin
*
 * 
 * 3     7/14/99 9:09p Jeff
 * added comment header
*
* $NoKeywords: $
*/

#include "DDAccess.h"
#include "TaskSystem.h"
#include "pserror.h"

osMutex::osMutex()
{
}

osMutex::~osMutex()
{
	Destroy();
}

bool osMutex::Create()
{
	return false;
}

void osMutex::Destroy()
{
}

bool osMutex::Acquire(int timeout)
{
}

void osMutex::Release()
{
}