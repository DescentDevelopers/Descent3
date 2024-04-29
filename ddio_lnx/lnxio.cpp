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
 * $Logfile: /DescentIII/Main/ddio_lnx/lnxio.cpp $
 * $Revision: 1.6 $
 * $Date: 2004/02/25 00:04:06 $
 * $Author: ryan $
 *
 * Linux IO routines
 *
 * $Log: lnxio.cpp,v $
 * Revision 1.6  2004/02/25 00:04:06  ryan
 * Removed loki_utils dependency and ported to MacOS X (runs, but incomplete).
 *
 * Revision 1.5  2000/05/29 05:20:13  icculus
 * Removed a real repetitive mprintf().
 *
 * Revision 1.4  2000/05/29 05:19:17  icculus
 * Serial behaviour is now correct (and the Rock'n'Ride works!). A
 * little more debugging code was added.
 *
 * Revision 1.3  2000/04/25 06:10:27  icculus
 * Added buttloads of debug information, and fixed some more glitches.
 *
 * Revision 1.2  2000/04/24 03:18:53  icculus
 * Serial port i/o code written, so I can ROCK'n'RIDE it!
 *
 * Revision 1.1.1.1  2000/04/18 00:00:33  icculus
 * initial checkin
 *
 *
 * 5     7/14/99 9:06p Jeff
 * added comment header
 *
 * $NoKeywords: $
 */

// ----------------------------------------------------------------------------
// Linux IO System Main Library Interface
// ----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <termios.h>

#include "pserror.h"
#include "application.h"
#include "ddio_lnx.h"
#include "ddio.h"

bool DDIO_init = false;
oeLnxApplication *Lnx_app_obj = NULL;
static struct termios oldtio[4];
static int portfd[4] = {-1, -1, -1, -1};

// ----------------------------------------------------------------------------
//	Initialization and destruction functions
// ----------------------------------------------------------------------------

bool ddio_InternalInit(ddio_init_info *init_info) {
  mprintf((0, "DDIO: ddio_InternalInit() called."));
  Lnx_app_obj = (oeLnxApplication *)init_info->obj;
  DDIO_init = true;
  return true;
}

void ddio_InternalClose() {
  mprintf((0, "DDIO: ddio_InternalClose() called."));

  if (DDIO_init) {
    for (int i = 0; i < (sizeof(portfd) / sizeof(portfd[0])); i++) {
      if (portfd[i] != -1)
        ddio_SerialClosePort((tSerialPort)&portfd[i]);
    } // for

    DDIO_init = false;
    Lnx_app_obj = NULL;
  } // if

  mprintf((0, "DDIO: ddio_InternalClose() returning."));
}

void ddio_DebugMessage(unsigned err, char *fmt, ...) {
  char buf[128];
  va_list arglist;

  va_start(arglist, fmt);
  vsnprintf(buf, sizeof(buf), fmt, arglist);
  va_end(arglist);

  mprintf((0, "%s\n", buf));
}

// takes port number 1-4, returns a port object
tSerialPort ddio_SerialOpenPort(int port_number, int baud) {
#if MACOSX
  return NULL;
#else
  char devName[50];
  struct termios newtio;
  unsigned int _baud;

  mprintf((0, "DDIO: ddio_SerialOpenPort(%d) called.", port_number));

  if ((port_number < 0) || (port_number > 3))
    return (NULL);

  if (baud == 1200)
    _baud = B1200;
  else if (baud == 2400)
    _baud = B2400;
  else if (baud == 4800)
    _baud = B4800;
  else if (baud == 9600)
    _baud = B9600;
  else if (baud == 19200)
    _baud = B19200;
  else if (baud == 38400)
    _baud = B38400;
  else if (baud == 57600)
    _baud = B57600;
  else if (baud == 115200)
    _baud = B115200;
  else
    return (NULL);

  snprintf(devName, sizeof(devName), "/dev/ttyS%d", port_number);
  int fd = open(devName, O_RDWR | O_NOCTTY | O_SYNC | O_NONBLOCK);
  if (fd == -1) {
    mprintf((0, "DDIO: ddio_SerialOpenPort(%d) FAILED.", port_number));
    return (NULL);
  } // if

  mprintf((0, "DDIO: opened file descriptor is (%d).", fd));

  portfd[port_number] = fd;

  tcgetattr(fd, &oldtio[port_number]);
  memset(&newtio, '\0', sizeof(newtio));

  newtio.c_cflag = _baud | CLOCAL | CS8; //_baud | CRTSCTS | CLOCAL | CS8;
  newtio.c_iflag = 0;                    // IGNPAR | IGNBRK;
  newtio.c_oflag = 0;
  newtio.c_lflag = 0; // ICANON;

  newtio.c_cc[VINTR] = 0;    /* Ctrl-c */
  newtio.c_cc[VQUIT] = 0;    /* Ctrl-\ */
  newtio.c_cc[VERASE] = 0;   /* del */
  newtio.c_cc[VKILL] = 0;    /* @ */
  newtio.c_cc[VEOF] = 4;     /* Ctrl-d */
  newtio.c_cc[VTIME] = 0;    /* inter-character timer unused */
  newtio.c_cc[VMIN] = 1;     /* blocking read until 1 character arrives */
  newtio.c_cc[VSWTC] = 0;    /* '\0' */
  newtio.c_cc[VSTART] = 0;   /* Ctrl-q */
  newtio.c_cc[VSTOP] = 0;    /* Ctrl-s */
  newtio.c_cc[VSUSP] = 0;    /* Ctrl-z */
  newtio.c_cc[VEOL] = 0;     /* '\0' */
  newtio.c_cc[VREPRINT] = 0; /* Ctrl-r */
  newtio.c_cc[VDISCARD] = 0; /* Ctrl-u */
  newtio.c_cc[VWERASE] = 0;  /* Ctrl-w */
  newtio.c_cc[VLNEXT] = 0;   /* Ctrl-v */
  newtio.c_cc[VEOL2] = 0;    /* '\0' */

  tcflush(fd, TCIOFLUSH);
  tcsetattr(fd, TCSANOW, &newtio);

  mprintf((0, "DDIO: ddio_SerialOpenPort(%d) succeeded.", port_number));
  return (&portfd[port_number]);
#endif
}

// takes port structure and frees it.
void ddio_SerialClosePort(tSerialPort port) {
#if !MACOSX
  int fd = *((int *)port);
  int index = (int)((((char *)port) - ((char *)&portfd)) / sizeof(int));

  mprintf((0, "DDIO: ddio_SerialClosePort(comport == %d) called.", index));
  mprintf((0, "DDIO: Serial port file descriptor to close is (%d).", fd));

  if (fd != -1) {
    tcsetattr(fd, TCSANOW, &oldtio[index]);
    close(fd);
    *((int *)port) = -1;
  } // if
#endif
}

// writes one byte.  true return value means it worked.
bool ddio_SerialWriteByte(tSerialPort port, ubyte b) {
#if MACOSX
  return false;
#else
  int fd = *((int *)port);
  bool retVal = ((write(fd, &b, sizeof(b)) == sizeof(b)) ? true : false);
  if (retVal == false) {
    mprintf((0, "DDIO: Writing byte (%u) to descriptor (%d) failed!", (unsigned int)b, fd));
  } // if

  return (retVal);
#endif
}
