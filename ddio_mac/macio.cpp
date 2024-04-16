/*
 * $Logfile: /DescentIII/Main/ddio_mac/macio.cpp $
 * $Revision: 1.1.1.1 $
 * $Date: 2003/08/26 03:56:55 $
 * $Author: kevinb $
 *
 * macintosh implementation of device dependent io functions *
 *
 * $Log: macio.cpp,v $
 * Revision 1.1.1.1  2003/08/26 03:56:55  kevinb
 * initial 1.5 import
 *
 * 
 * 3     10/21/99 3:33p Jeff
 * Macintosh merges
 * 
 * 2     7/28/99 3:31p Kevin
 * Mac Stuff!
 * 
 * 3     5/11/97 7:58 PM Jeremy
 * added stubs for internal init/close functions
 * 
 * 2     5/9/97 7:14 PM Jeremy
 * initial checkin
 * 
 * 1     3/13/97 6:53 PM Jeremy
 * macintosh implementation of device dependent io functions
 *
 * $NoKeywords: $
 */
// ----------------------------------------------------------------------------
// Mac IO System Main Library Interface
// ----------------------------------------------------------------------------
#include <stdlib.h>
#include "pserror.h"
#include "mono.h"
#include "gameos.h"
#include "ddio_mac.h"
#include "ddio.h"
// ----------------------------------------------------------------------------
//	Initialization and destruction functions
// ----------------------------------------------------------------------------
bool
ddio_InternalInit(ddio_init_info *init_info)
{
	bool	result = true;
	
	return result;
}
void
ddio_InternalClose()
{
	;
}

