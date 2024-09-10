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

 * $Logfile: /DescentIII/Main/ddio_lnx/lnxjoy.cpp $
 * $Revision: 1.3 $
 * $Date: 2001/02/07 09:16:45 $
 * $Author: icculus $
 *
 * Linux joystick routines
 *
 * $Log: sdljoy.cpp,v $
 * Revision 1.3  2001/02/07 09:16:45  icculus
 * More robust debugging information.
 *
 * Revision 1.2  2000/06/29 22:15:25  hercules
 * Fixed hat motion
 *
 * Revision 1.1  2000/06/29 09:53:00  hercules
 * Use SDL joystick support (hats off to you! :)
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

#include <cstdlib>
#include <cstring>
#include <SDL.h>

// rcg06182000 need this for specific joystick stuff.
#include "args.h"
#include "joystick.h"
#include "log.h"

//	---------------------------------------------------------------------------
//	globals

static int specificJoy = -1;
static struct {
  SDL_Joystick *handle;
  tJoyInfo caps;
} Joysticks[MAX_JOYSTICKS];
void joy_Close();
static int joyGetNumDevs(void);
//	closes a stick
//		closes connection with controller.
static void joy_CloseStick(tJoystick joy);
//	initializes a joystick
//		if server_adr is valid, a link is opened to another machine with a controller.
static bool joy_InitStick(tJoystick joy, char *server_adr);

//	---------------------------------------------------------------------------
//	functions

//	joystick system initialization
bool joy_Init(bool remote) {
  //	reinitialize joystick if already initialized.
  joy_Close();
  if (SDL_InitSubSystem(SDL_INIT_JOYSTICK) < 0) {
    // FIXME: report an error?
    return false;
  }

  //	check if this OS supports joysticks
  if (!joyGetNumDevs() && !remote) {
    return false;
  }

  // rcg06182000 specific joystick support.
  if (specificJoy >= 0) {
    joy_InitStick((tJoystick)specificJoy, NULL);
  } // if
  else {
    //	initialize joystick list
    for (int i = 0; i < MAX_JOYSTICKS; i++) {
      joy_InitStick((tJoystick)i, NULL);
    }
  } // else
  return true;
}

void joy_Close() {
  //	initialize joystick list
  for (int i = 0; i < MAX_JOYSTICKS; i++) {
    joy_CloseStick((tJoystick)i);
  }
  SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
}

//	initializes a joystick
//		if server_adr is valid, a link is opened to another machine with a controller.
static bool joy_InitStick(tJoystick joy, char *server_adr) {
  SDL_Joystick *stick;

  //	close down already open joystick.
  joy_CloseStick(joy);

  //	okay, now if this is a remote joystick, open it
  if (server_adr) {
    return false;
  }
  stick = SDL_JoystickOpen(joy);
  Joysticks[joy].handle = stick;
  if (stick) {
    tJoyInfo caps;

    memset(&caps, 0, (sizeof caps));
    strncpy(caps.name, SDL_JoystickNameForIndex(joy), sizeof(caps.name) - 1);
    caps.num_btns = SDL_JoystickNumButtons(stick);
    int axes = SDL_JoystickNumAxes(stick);
    switch (axes) {
    default:
      // Fall through to 6 axes
    case 6:
      caps.axes_mask |= JOYFLAG_VVALID;
      caps.minv = -32767;
      caps.maxv = 32768;
    case 5:
      caps.axes_mask |= JOYFLAG_UVALID;
      caps.minu = -32767;
      caps.maxu = 32768;
    case 4:
      caps.axes_mask |= JOYFLAG_RVALID;
      caps.minr = -32767;
      caps.maxr = 32768;
    case 3:
      caps.axes_mask |= JOYFLAG_ZVALID;
      caps.minz = -32767;
      caps.maxz = 32768;
    case 2:
      caps.axes_mask |= JOYFLAG_YVALID;
      caps.miny = -32767;
      caps.maxy = 32768;
    case 1:
      caps.axes_mask |= JOYFLAG_XVALID;
      caps.minx = -32767;
      caps.maxx = 32768;
    case 0:
      break;
    }
    int hats = SDL_JoystickNumHats(stick);
    switch (hats) {
    default:
      // Fall through to 4 hats
    case 4:
      caps.axes_mask |= JOYFLAG_POV4VALID;
    case 3:
      caps.axes_mask |= JOYFLAG_POV3VALID;
    case 2:
      caps.axes_mask |= JOYFLAG_POV2VALID;
    case 1:
      caps.axes_mask |= JOYFLAG_POVVALID;
    case 0:
      break;
    }
    Joysticks[joy].caps = caps;

    LOG_DEBUG.printf("JOYSTICK: Initialized stick named [%s].", caps.name);
    LOG_DEBUG.printf("JOYSTICK: (%d) axes, (%d) hats, and (%d) buttons.", axes, hats, caps.num_btns);
  }

  return (Joysticks[joy].handle != NULL);
}

//	closes a stick
//		closes connection with controller.
static void joy_CloseStick(tJoystick joy) {
  // CLOSE joystick here
  SDL_JoystickClose(Joysticks[joy].handle);
  Joysticks[joy].handle = 0;
}

//	returns true if joystick valid
bool joy_IsValid(tJoystick joy) {
  if (specificJoy >= 0) {
    if (joy != specificJoy) {
      return false;
    }
  }
  return (Joysticks[joy].handle != NULL);
}

//	retreive information about joystick.
void joy_GetJoyInfo(tJoystick joy, tJoyInfo *info) { memcpy(info, &Joysticks[(int)joy].caps, sizeof(tJoyInfo)); }

//	retreive uncalibrated position of joystick
#define LNX_JOYAXIS_RANGE 65535
void joy_GetRawPos(tJoystick joy, tJoyPos *pos) {
  joy_GetPos(joy, pos);

  pos->x = (pos->x + 32767);
  pos->y = (pos->y + 32767);
  pos->z = (pos->z + 32767);
  pos->r = (pos->r + 32767);
  pos->u = (pos->u + 32767);
  pos->v = (pos->v + 32767);
}

static inline uint32_t map_hat(Uint8 value) {
  uint32_t mapped = 0;

  switch (value) {
  case SDL_HAT_CENTERED:
    mapped = JOYPOV_CENTER;
    break;
  case SDL_HAT_UP:
    mapped = 0x00;
    break;
  case SDL_HAT_UP | SDL_HAT_RIGHT:
    mapped = 0x20;
    break;
  case SDL_HAT_RIGHT:
    mapped = 0x40;
    break;
  case SDL_HAT_RIGHT | SDL_HAT_DOWN:
    mapped = 0x60;
    break;
  case SDL_HAT_DOWN:
    mapped = 0x80;
    break;
  case SDL_HAT_DOWN | SDL_HAT_LEFT:
    mapped = 0xA0;
    break;
  case SDL_HAT_LEFT:
    mapped = 0xC0;
    break;
  case SDL_HAT_LEFT | SDL_HAT_UP:
    mapped = 0xE0;
    break;
  }
  return mapped;
}

//	returns the state of a stick, remote or otherwise
void joy_GetPos(tJoystick joy, tJoyPos *pos) {
  SDL_Joystick *stick;
  int i;

  memset(pos, 0, (sizeof *pos));

  //	retrieve joystick info from the net, or locally.
  stick = Joysticks[joy].handle;
  if (stick) {
    uint32_t mask;

    mask = Joysticks[joy].caps.axes_mask;
    pos->x = SDL_JoystickGetAxis(stick, 0);
    pos->y = SDL_JoystickGetAxis(stick, 1);
    if (mask & JOYFLAG_ZVALID) {
      pos->z = SDL_JoystickGetAxis(stick, 2);
    }
    if (mask & JOYFLAG_RVALID) {
      pos->r = SDL_JoystickGetAxis(stick, 3);
    }
    if (mask & JOYFLAG_UVALID) {
      pos->u = SDL_JoystickGetAxis(stick, 4);
    }
    if (mask & JOYFLAG_VVALID) {
      pos->v = SDL_JoystickGetAxis(stick, 5);
    }
    for (i = 0; i < JOYPOV_NUM; ++i) {
      if (mask & (JOYFLAG_POVVALID << i)) {
        pos->pov[i] = map_hat(SDL_JoystickGetHat(stick, i));
      }
    }
    for (i = Joysticks[joy].caps.num_btns; i >= 0; --i) {
      if (SDL_JoystickGetButton(stick, i)) {
        pos->buttons |= (1 << i);
      }
    }
  }
}

static int joyGetNumDevs(void) {
  int found = 0;

  // rcg06182000 add support for specific joydev.
  int rc = FindArgChar("-joystick", 'j');
  specificJoy = -1;
  if ((rc > 0) && (GameArgs[rc + 1] != NULL)) {
    specificJoy = atoi(GameArgs[rc + 1]);
    if ((specificJoy >= 0) && (specificJoy < SDL_NumJoysticks())) {
      found = 1;
    } else {
      specificJoy = -1;
    }
  }
  if (specificJoy < 0) {
    found = SDL_NumJoysticks();
  }

  LOG_INFO.printf("Joystick: Found %d joysticks.", found);
  return found;
}

void ddio_InternalJoyFrame(void) {
  // All the work is done already in SDL_PumpEvents()
}
