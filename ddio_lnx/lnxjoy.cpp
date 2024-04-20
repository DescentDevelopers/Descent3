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
 * $Logfile: /DescentIII/Main/ddio_lnx/lnxjoy.cpp $
 * $Revision: 1.4 $
 * $Date: 2000/06/29 06:41:23 $
 * $Author: icculus $
 *
 * Linux joystick routines
 *
 * $Log: lnxjoy.cpp,v $
 * Revision 1.4  2000/06/29 06:41:23  icculus
 * mad commits.
 *
 * Revision 1.3  2000/06/24 01:15:15  icculus
 * patched to compile.
 *
 * Revision 1.2  2000/05/29 05:21:09  icculus
 * Changed a fprintf(stderr, ...) to an mprintf()...
 *
 * Revision 1.1.1.1  2000/04/18 00:00:33  icculus
 * initial checkin
 *
 *
 * 9     8/22/99 5:55p Jeff
 * fixed assert
 *
 * 8     8/19/99 3:46p Jeff
 * removed mprintfs
 *
 * 7     8/19/99 3:22p Jeff
 * added support for joystick driver version pre 1.0
 *
 * 6     8/18/99 9:47p Jeff
 * joystick support! for kernel 2..2+ clients...need to handle before that
 * still.
 *
 * 5     8/17/99 2:32p Jeff
 * fixed joy_GetPos
 *
 * 4     7/14/99 9:06p Jeff
 * added comment header
 *
 * $NoKeywords: $
 */

#include <linux/joystick.h>
#include "joystick.h"
#include "pserror.h"
#include "pstypes.h"
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <memory.h>
#include <errno.h>

// rcg06182000 need this for specific joystick stuff.
#include "args.h"

#define JOY_DEV "/dev/js"

typedef struct tJoystickRecord {
  bool valid;          // is this a valid device.
  bool remote;         // is this device on another computer
  bool use_old_driver; // use pre joystick 1.0 driver interface
  tJoystick joy;       // joystick
  union {
    int joyid;
  } id;          // information on accessing the device
  tJoyInfo caps; // controller capabilities
  tJoyPos pos;   // current position
} tJoystickRecord;

//	---------------------------------------------------------------------------
//	globals

static int specificJoy = 0;
static tJoystickRecord Joysticks[MAX_JOYSTICKS];
static int Joy_initialized = 0; // is our joystick system initialized?
void joy_Close();
bool joy_InitRemoteStick(tJoystick joy, char *server_adr, tJoystickRecord *stick);
void joy_GetState(tJoystick stick, tJoyPos *pos);
void joy_GetRemoteState(tJoystick stick, tJoyPos *pos);
int joyGetNumDevs(void);
//	closes a stick
//		closes connection with controller.
void joy_CloseStick(tJoystick joy);
//	initializes a joystick
//		if server_adr is valid, a link is opened to another machine with a controller.
bool joy_InitStick(tJoystick joy, char *server_adr);

//	---------------------------------------------------------------------------
//	functions

//	joystick system initialization
bool joy_Init(bool remote) {
  int i;

  //	reinitialize joystick if already initialized.
  if (Joy_initialized) {
    joy_Close();
  } else {
    atexit(joy_Close);
  }

  //	check if this OS supports joysticks
  if (!joyGetNumDevs() && !remote) {
    Joy_initialized = 0;
    return false;
  }

  Joy_initialized = 1;

  // rcg06182000 specific joystick support.
  if (specificJoy) {
    Joysticks[0].valid = false;
    joy_InitStick((tJoystick)0, NULL);
  } // if
  else {
    //	initialize joystick list
    for (i = 0; i < MAX_JOYSTICKS; i++) {
      Joysticks[i].valid = false;
      joy_InitStick((tJoystick)i, NULL);
    }
  } // else

  return true;
}

void joy_Close() {
  int i;
  for (i = 0; i < MAX_JOYSTICKS; i++)
    if (Joysticks[i].valid)
      joy_CloseStick((i == 0) ? JOYSTICK_1 : (i == 1) ? JOYSTICK_2 : JOYSTICK_1);
}

//	initializes a joystick
//		if server_adr is valid, a link is opened to another machine with a controller.
bool joy_InitStick(tJoystick joy, char *server_adr) {
  ASSERT(Joy_initialized);

  //	close down already open joystick.
  if (Joysticks[(int)joy].valid)
    joy_CloseStick(joy);

  //	okay, now if this is a remote joystick, open it
  if (server_adr) {
    return joy_InitRemoteStick(joy, server_adr, &Joysticks[(int)joy]);
  } else {
    int fd;
    char joy_dev[32];
    int num_of_axis = 0, num_of_buttons = 0;
    char name_of_joystick[80];

    // rcg06182000 specific joy support.
    if (specificJoy)
      strcpy(joy_dev, GameArgs[FindArgChar("-joystick", 'j') + 1]);
    else
      sprintf(joy_dev, "%s%d", JOY_DEV, (int)joy);

    if ((fd = open(joy_dev, O_RDONLY)) == -1) {
      if (errno == EBUSY) {
        mprintf((0, "JOYSTICK: Couldn't open joystick %d, in use.", joy));
      }
      return false;
    }

    // determine what version of the joystick driver we are using
    unsigned int version = 0;
    ioctl(fd, JSIOCGVERSION, &version);
    if (version < 0x10000) {
      Joysticks[joy].use_old_driver = true;
    } else {
      Joysticks[joy].use_old_driver = false;
    }

    fcntl(fd, F_SETFL, O_NONBLOCK);

    if (Joysticks[joy].use_old_driver) {
      num_of_axis = 2;
      num_of_buttons = 2;
      strcpy(name_of_joystick, "Generic");

      mprintf((0, "Joystick %d opened: Using OLD JOYSTICK driver, %d buttons and %d axis\n", (int)joy, num_of_buttons,
               num_of_axis));

      Joysticks[joy].caps.minx = 0;
      Joysticks[joy].caps.maxx = 255;
      Joysticks[joy].caps.miny = 0;
      Joysticks[joy].caps.maxy = 255;
      Joysticks[joy].caps.minz = 0;
      Joysticks[joy].caps.maxz = 255;
      Joysticks[joy].caps.minr = 0;
      Joysticks[joy].caps.maxr = 255;
      Joysticks[joy].caps.minu = -32767;
      Joysticks[joy].caps.maxu = 32767;
      Joysticks[joy].caps.minv = -32767;
      Joysticks[joy].caps.maxv = 32767;
    } else {
      ioctl(fd, JSIOCGAXES, &num_of_axis);
      ioctl(fd, JSIOCGBUTTONS, &num_of_buttons);
      ioctl(fd, JSIOCGNAME(80), &name_of_joystick);

      mprintf((0, "Joystick %d opened: %s with %d buttons and %d axis\n", (int)joy, name_of_joystick, num_of_buttons,
               num_of_axis));

      Joysticks[joy].caps.minx = -32767;
      Joysticks[joy].caps.maxx = 32767;
      Joysticks[joy].caps.miny = -32767;
      Joysticks[joy].caps.maxy = 32767;
      Joysticks[joy].caps.minz = -32767;
      Joysticks[joy].caps.maxz = 32767;
      Joysticks[joy].caps.minr = -32767;
      Joysticks[joy].caps.maxr = 32767;
      Joysticks[joy].caps.minu = -32767;
      Joysticks[joy].caps.maxu = 32767;
      Joysticks[joy].caps.minv = -32767;
      Joysticks[joy].caps.maxv = 32767;
    }

    Joysticks[joy].valid = true;
    Joysticks[joy].remote = false;
    Joysticks[joy].joy = joy;
    Joysticks[joy].id.joyid = fd;

    strcpy(Joysticks[joy].caps.name, name_of_joystick);
    Joysticks[joy].caps.num_btns = num_of_buttons;
    Joysticks[joy].caps.axes_mask = JOYFLAG_XVALID | JOYFLAG_YVALID;

    if (num_of_axis > 2)
      Joysticks[joy].caps.axes_mask |= JOYFLAG_ZVALID;
    if (num_of_axis > 3)
      Joysticks[joy].caps.axes_mask |= JOYFLAG_RVALID;
    if (num_of_axis > 4) {
      // rcg06182000 God, I hope this works.
      // rcg06182000 Attempt to hack 2 more axes into this driver...
      // rcg06182000 Before, it was just JOYFLAG_POVVALID...
      Joysticks[joy].caps.axes_mask |= JOYFLAG_UVALID;
      Joysticks[joy].caps.axes_mask |= JOYFLAG_VVALID;

      Joysticks[joy].caps.axes_mask |= JOYFLAG_POVVALID;
    } // if
    memset(&Joysticks[joy].pos, 0, sizeof(tJoyPos));

    return true;
  }

  return false;
}

// this function takes a server address and tries to initialize a stick that's on a machine
// running the remote control server.
bool joy_InitRemoteStick(tJoystick joy, char *server_adr, tJoystickRecord *stick) { return false; }

//	closes a stick
//		closes connection with controller.
void joy_CloseStick(tJoystick joy) {
  ASSERT(Joysticks[(int)joy].valid);

  if (Joysticks[(int)joy].remote) {
  }

  // CLOSE joystick here
  close(Joysticks[(int)joy].id.joyid);
  Joysticks[(int)joy].valid = false;
}

//	returns true if joystick valid
bool joy_IsValid(tJoystick joy) {
  if (Joysticks[(int)joy].valid)
    return true;
  else
    return false;
}

//	retreive information about joystick.
void joy_GetJoyInfo(tJoystick joy, tJoyInfo *info) {
  ASSERT(Joy_initialized);

  if (Joysticks[(int)joy].valid) {
    memcpy(info, &Joysticks[(int)joy].caps, sizeof(tJoyInfo));
  } else
    Int3(); // This should never happen.
}

//	retreive uncalibrated position of joystick
#define LNX_JOYAXIS_RANGE 65535
void joy_GetRawPos(tJoystick joy, tJoyPos *pos) {
  joy_GetPos(joy, pos);

  if (!Joysticks[joy].use_old_driver) {
    pos->x = (pos->x + 32767);
    pos->y = (pos->y + 32767);
    pos->z = (pos->z + 32767);
    pos->r = (pos->r + 32767);
    pos->u = (pos->u + 32767);
    pos->v = (pos->v + 32767);
  }
}

//	returns the state of a stick, remote or otherwise
void joy_GetPos(tJoystick stick, tJoyPos *pos) {
  int i;

  for (i = 0; i < JOYPOV_NUM; i++) {
    pos->pov[i] = 0;
  }
  pos->x = 0;
  pos->y = 0;
  pos->z = 0;
  pos->r = 0;
  pos->u = 0;
  pos->v = 0;
  pos->buttons = 0;
  pos->btn = 0;

  if (!Joy_initialized)
    return;

  if (!Joysticks[(int)stick].valid)
    return;

  //	retrieve joystick info from the net, or locally.
  if (Joysticks[(int)stick].remote) {
    joy_GetRemoteState(stick, pos);
  } else {
    memcpy(pos, &Joysticks[(int)stick].pos, sizeof(tJoyPos));
  }
}

//	returns the position of a remote stick.
void joy_GetRemoteState(tJoystick stick, tJoyPos *pos) {}

int joyGetNumDevs(void) {
  int found = 0;
  int count = 0, fd;
  char joy_dev[80];

  // rcg06182000 add support for specific joydev.
  int rc = FindArgChar("-joystick", 'j');
  if ((rc > 0) && (GameArgs[rc + 1] != NULL)) {
    fd = open(GameArgs[rc + 1], O_RDONLY);
    if (fd != -1) {
      specificJoy = 1;
      close(fd);
      return (1);
    } // if
  }   // if
      // rcg end addition

  for (count = 0; count < MAX_JOYSTICKS; count++) {
    sprintf(joy_dev, "%s%d", JOY_DEV, count);
    fd = open(joy_dev, O_RDONLY);

    if (fd != -1) {
      found++;
      close(fd);
    }
  }

  mprintf((0, "Joystick: Found %d joysticks.", found));
  return found;
}

void ddio_InternalJoyFrame(void) {
  int joy;

  for (joy = 0; joy < MAX_JOYSTICKS; joy++) {
    if (!Joysticks[joy].valid)
      continue;

    int count = 0;
    struct js_event js;
    struct JS_DATA_TYPE old_js;
    fd_set watchset;
    tJoyPos *pos;
    struct timeval tv;

    tv.tv_sec = 0;
    tv.tv_usec = 0;

    int new_pov = 0x0000;
#define PV_UP 0x01
#define PV_DOWN 0x02
#define PV_LEFT 0x04
#define PV_RIGHT 0x08

    pos = &Joysticks[joy].pos;

    switch (pos->pov[0]) {
    case 0x00: // up
      new_pov = PV_UP;
      break;
    case 0x20: // up-right;
      new_pov = PV_UP | PV_RIGHT;
      break;
    case 0x40: // right
      new_pov = PV_RIGHT;
      break;
    case 0x60: // down-right
      new_pov = PV_DOWN | PV_RIGHT;
      break;
    case 0x80: // down
      new_pov = PV_DOWN;
      break;
    case 0xA0: // down-left
      new_pov = PV_DOWN | PV_LEFT;
      break;
    case 0xC0: // left
      new_pov = PV_LEFT;
      break;
    case 0xE0: // up-left
      new_pov = PV_UP | PV_LEFT;
      break;
    }

    FD_ZERO(&watchset);
    FD_SET(Joysticks[joy].id.joyid, &watchset);

    while (1 && count < 50) {
      if (select(Joysticks[joy].id.joyid + 1, &watchset, NULL, NULL, &tv) < 0) {
        perror("Error reading joystick file descriptor \n");
        break;
      }

      if (!FD_ISSET(Joysticks[joy].id.joyid, &watchset))
        break; // done reading, nothing waiting

      if (Joysticks[joy].use_old_driver) {
        if (read(Joysticks[joy].id.joyid, &old_js, JS_RETURN) != JS_RETURN) {
          break;
        }

        pos->x = old_js.x;
        pos->y = old_js.y;
        pos->buttons = 0;

        pos->buttons |= (old_js.buttons & 1) ? 0x01 : 0;
        pos->buttons |= (old_js.buttons & 2) ? 0x02 : 0;

        break;
      } else {
        // read the joystick state
        read(Joysticks[joy].id.joyid, &js, sizeof(struct js_event));

        // see what to do with the event
        switch (js.type & ~JS_EVENT_INIT) {
        case JS_EVENT_AXIS:
          switch (js.number) {
          case 0: // X Axis
            pos->x = js.value;
            break;
          case 1: // Y Axis
            pos->y = js.value;
            break;
          case 2: // Z Axis
            pos->z = js.value;
            break;
          case 3: // R Axis
            pos->r = js.value;
            break;
          case 4: // POV U Axis
            // rcg 06152000 set pos->u. Wasn't here.  !?
            pos->u = js.value;

            new_pov &= ~(PV_LEFT | PV_RIGHT);

            if (js.value > 0) {
              // mprintf((0,"POV RIGHT\n"));
              new_pov |= PV_RIGHT;
            } else if (js.value < 0) {
              // mprintf((0,"POV LEFT\n"));
              new_pov |= PV_LEFT;
            }
            break;
          case 5: // POV V Axis
            // rcg 06152000 set pos->v. Wasn't here.  !?
            pos->v = js.value;

            new_pov &= ~(PV_UP | PV_DOWN);

            if (js.value > 0) {
              // mprintf((0,"POV DOWN\n"));
              new_pov |= PV_DOWN;
            } else if (js.value < 0) {
              // mprintf((0,"POV UP\n"));
              new_pov |= PV_UP;
            }
            break;
          }

          break;
        case JS_EVENT_BUTTON:
          int bit;
          bit = 0x01;
          bit = bit << js.number;

          if (js.value) {
            // button push
            pos->buttons |= bit;
          } else {
            // button release
            pos->buttons &= ~bit;
          }
          break;
        }
      }

      count++;
    }

    if (count == 50) {
      // mprintf((0,"JOYSTICK BREAK!\n"));
    }

    // recalc pov
    switch (new_pov) {
    case 0: // center
      pos->pov[0] = JOYPOV_CENTER;
      break;
    case PV_UP:
      pos->pov[0] = 0x00;
      break;
    case PV_UP | PV_RIGHT:
      pos->pov[0] = 0x20;
      break;
    case PV_RIGHT:
      pos->pov[0] = 0x40;
      break;
    case PV_DOWN | PV_RIGHT:
      pos->pov[0] = 0x60;
      break;
    case PV_DOWN:
      pos->pov[0] = 0x80;
      break;
    case PV_DOWN | PV_LEFT:
      pos->pov[0] = 0xA0;
      break;
    case PV_LEFT:
      pos->pov[0] = 0xC0;
      break;
    case PV_UP | PV_LEFT:
      pos->pov[0] = 0xE0;
      break;
    }
  }
}
