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

 * $Logfile: /DescentIII/main/d3serial.h $
 * $Revision: 5 $
 * $Date: 10/11/98 2:58a $
 * $Author: Jeff $
 *
 * file header for serialization
 *
 * $Log: /DescentIII/main/d3serial.h $
 *
 * 5     10/11/98 2:58a Jeff
 * fixed serialization code
 *
 * 4     9/29/98 3:04p Jeff
 *
 * 3     7/13/98 12:41p Jeff
 * added serial number support
 *
 * 2     2/07/98 6:35p Jeff
 *
 * $NoKeywords: $
 */

#ifndef __SERIALIZE_H_
#define __SERIALIZE_H_

#include <time.h>

#define DESC_ID_LEN 40       // how long the id string can be
#define DESC_CHECKSUM_LEN 4  // checksum is 4 bytes
#define DESC_SERIALNUM_LEN 4 // serialnum is 4 bytes
#define DESC_DEAD_TIME_LEN 8 // dead time is 8 bytes

#define DESC_ID_TAG "Midway in our life's journey, I went astray"
#define DESC_ID_TAG_LEN 43

#define DESC_CHKSUM_TAG "alone in a dark wood."
#define DESC_CHKSUM_TAG_LEN 21

#define DESC_SERIALNUM_TAG "You've entered Lord Spam's Pleasure Dome"
#define DESC_SERIALNUM_TAG_LEN 40

#define DESC_DEAD_TIME_TAG "from the straight road and woke to find myself"
#define DESC_DEAD_TIME_TAG_LEN 46

// checks exectuable (returns !0 on error)
int SerialCheck(void);

// given return value from SerialCheck() it reports the error
void SerialError(int error);

// returns the serialnumber of the user
unsigned int SerialGetSerialNum(void);

/////////////////////////////////////////////////////////////////////////////
//	These are the functions used for serialization
//	if this function returns:
//	1 : than it's a serialized exe, num will be given the serial num
//	0 : than it isn't a serialized exe
//	-1 : detected a hacked exe
char GetInternalSerializationNumber(int *num);

#endif
