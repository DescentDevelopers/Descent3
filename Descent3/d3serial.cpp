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
 * $Logfile: /DescentIII/main/d3serial.cpp $
 * $Revision: 15 $
 * $Date: 5/13/99 11:18a $
 * $Author: Matt $
 *
 * Functions to check the copy of the executable for expiration, registered name
 *
 * $Log: /DescentIII/main/d3serial.cpp $
 *
 * 15    5/13/99 11:18a Matt
 * Added some text.
 *
 * 14    4/16/99 11:59a Matt
 * Took out include of io.h, because it wasn't needed.
 *
 * 13    4/16/99 12:39a Matt
 * Took out Linux ifdef around include of io.h, since it's a system header
 * file and there's no harm in including it in the Windows version.
 *
 * 12    4/15/99 1:38a Jeff
 * changes for linux compile
 *
 * 11    4/14/99 2:50a Jeff
 * fixed some case mismatched #includes
 *
 * 10    2/17/99 3:06p Matt
 * Updated copyrights.
 *
 * 9     10/18/98 9:12p Matt
 * Use new symbolic constant for the program name.
 *
 * 8     10/13/98 2:30p Jeff
 * changed serialization msg for demo
 *
 * 7     10/11/98 2:58a Jeff
 * fixed serialization code
 *
 * 6     7/13/98 12:41p Jeff
 * added serial number support
 *
 * 5     6/08/98 3:16p Matt
 * Mucked with formatting & content of serialization message.
 *
 * 4     6/04/98 11:32a Jeff
 * Better 'warning' box for the serialization
 *
 * 3     2/10/98 10:43a Jeff
 * fixed it so a nonregistered version will run (to make development
 * easier)
 *
 * 2     2/07/98 6:33p Jeff
 * Initial Creation (Based on D2 serialization)
 *
 * $NoKeywords: $
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "d3serial.h"
#include "game.h"
#include "debug.h"
#include "descent.h"
#include <time.h>
#include "mono.h"
#include "cfile/cfile.h"
#include "program.h"

#include <fcntl.h>

#define SERIAL_NO_ERR 0
#define SERIAL_EXPIRED 1
#define SERIAL_BAD_CHECKSUM 2
#define SERIAL_UNREGISTERED 3

static char name_copy[DESC_ID_LEN];

unsigned long d3_serialnum = 100000;

// checks the exectuable (serialization)
int SerialCheck(void) {
  char name2[] = DESC_ID_TAG "0000000000000000000000000000000000000000";
  char time_str[] = DESC_DEAD_TIME_TAG "00000000";
  int i, found;
  unsigned long *checksum, test_checksum;
  unsigned long *serialnum;
  time_t current_time, saved_time;

#ifdef DEMO
  char regstr[] = "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!    NOTICE    "
                  "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
                  "\nThis Descent 3 Demo %s has been specially prepared for\n"
                  "\n"
                  "\t\t%s (Serial Num: %d)\n"
                  "\n"
                  "        and is the Confidential Property of Outrage Entertainment, Inc.\n"
                  "\n"
                  "\n"
                  "\t\tDISTRIBUTION IS PROHIBITED\n"
                  "\n"
                  "\n"
                  "               Descent 3 is Copyright (c) 1998,1999 Outrage Entertainment, Inc.\n"
                  "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
                  "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
#else
  char regstr[] = "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!    NOTICE    "
                  "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
                  "\nThis pre-release version of Descent 3 has been specially prepared for\n"
                  "\n"
                  "\t\t%s (Serial Num: %d)\n"
                  "\n"
                  "        and is the Confidential Property of Outrage Entertainment, Inc.\n"
                  "\n"
                  "\n"
                  "\t\tDISTRIBUTION IS PROHIBITED\n"
                  "\n"
                  "This version of Descent 3 should be used for evaluation and testing only.\n"
                  "Because the game is not yet complete, items may be missing the bugs may be\n"
                  "encountered.\n"
                  "\n"
                  "               Descent 3 is Copyright (c) 1998,1999 Outrage Entertainment, Inc.\n"
                  "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
                  "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
#endif

  // get the expiration time of the executable
  char *start_time = &time_str[DESC_DEAD_TIME_TAG_LEN];
  saved_time = (time_t)strtol(start_time, NULL, 16);
  if (saved_time == (time_t)0)
    // this guy has never been registered with the serialization program
    return SERIAL_NO_ERR;

  // adjust pointer, so it points to the registered name
  char *name = name2 + DESC_ID_TAG_LEN;

  // save for later use
  strcpy(name_copy, name);

  // get current date from the system
  current_time = time(NULL);

  // check to see if executable has expired
  if (current_time >= saved_time) {
    return SERIAL_EXPIRED;
  }

  // check the checksum created by the registered name
  test_checksum = 0;
  for (i = 0; i < (signed)strlen(name); i++) {
    found = 0;
    test_checksum += name[i];
    if (((test_checksum / 2) * 2) != test_checksum)
      found = 1;
    test_checksum = test_checksum >> 1;
    if (found)
      test_checksum |= 0x80000000;
  }
  static char desc_id_checksum_str[] = DESC_CHKSUM_TAG "0000"; // 4-byte checksum
  char *checksum_ptr = desc_id_checksum_str + DESC_CHKSUM_TAG_LEN;

  // compare generated checksum with that in the executable
  checksum = (unsigned long *)&(checksum_ptr[0]);
  if (test_checksum != *checksum) {
    return SERIAL_BAD_CHECKSUM;
  }

  static char desc_id_serialnum_str[] = DESC_SERIALNUM_TAG "0000"; // 4-byte serialnum
  char *serialnum_ptr = desc_id_serialnum_str + DESC_SERIALNUM_TAG_LEN;

  serialnum = (unsigned long *)&(serialnum_ptr[0]);
  d3_serialnum = *serialnum;

  // this guy is ok, we can exit clean now
  char buffer[400];

#ifdef DEMO
  char ver[10];
  snprintf(ver, sizeof(ver), "Beta %d.%d.%d", D3_MAJORVER, D3_MINORVER, D3_BUILD);
  snprintf(buffer, sizeof(buffer), regstr, ver, name, d3_serialnum);
#else
  snprintf(buffer, sizeof(buffer), regstr, name, d3_serialnum);
#endif

  Debug_MessageBox(OSMBOX_OK, PRODUCT_NAME, buffer);

  return SERIAL_NO_ERR;
}

// displays an error box displaying what went wrong with serialization checking
void SerialError(int error) {
  switch (error) {
  case SERIAL_EXPIRED: {
    Debug_MessageBox(OSMBOX_OK, PRODUCT_NAME, "Executable has expired");
    break;
  }
  case SERIAL_BAD_CHECKSUM: {
    Debug_MessageBox(OSMBOX_OK, PRODUCT_NAME, "Bad Checksum");
    break;
  }
  case SERIAL_UNREGISTERED: {
    Debug_MessageBox(OSMBOX_OK, PRODUCT_NAME, "This is unregistered, please serialize");
    break;
  }
  }
}

unsigned long SerialGetSerialNum(void) { return d3_serialnum; }
