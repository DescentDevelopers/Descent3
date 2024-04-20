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
#include "ddebug/mono.h"
#include "gameos.h"
#include "ddio_mac.h"
#include "ddio.h"
// ----------------------------------------------------------------------------
//	Initialization and destruction functions
// ----------------------------------------------------------------------------
bool ddio_InternalInit(ddio_init_info *init_info) {
  bool result = true;

  return result;
}
void ddio_InternalClose() { ; }
