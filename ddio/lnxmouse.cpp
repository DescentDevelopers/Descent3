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

 * $Logfile: /DescentIII/Main/ddio_lnx/lnxmouse.cpp $
 * $Revision: 1.10 $
 * $Date: 2000/07/07 03:47:19 $
 * $Author: hercules $
 *
 * Linux mouse routines
 *
 * $Log: lnxmouse.cpp,v $
 * Revision 1.10  2000/07/07 03:47:19  hercules
 * Scale the ball values to mouse range (64000 -> 640)
 *
 * Revision 1.9  2000/07/04 00:30:04  icculus
 * Again.
 *
 * Revision 1.8  2000/07/04 00:29:00  icculus
 * Cleanups. Removed chunks of commented-out, abandoned attempts, and some
 * no-longer used global variables.
 *
 * Revision 1.7  2000/06/29 10:38:45  hercules
 * Don't warp the mouse during mouse reset
 *
 * Revision 1.6  2000/06/29 09:51:53  hercules
 * Added support for wheel mouse
 *
 * Revision 1.5  2000/06/29 06:41:23  icculus
 * mad commits.
 *
 * Revision 1.4  2000/06/24 01:15:15  icculus
 * patched to compile.
 *
 * Revision 1.3  2000/04/28 20:13:42  icculus
 * Took out some mprintfs
 *
 * Revision 1.2  2000/04/27 11:36:29  icculus
 * Rewritten to use SDL.
 *
 * Revision 1.1.1.1  2000/04/18 00:00:33  icculus
 * initial checkin
 *
 *
 * 17    9/09/99 4:36p Jeff
 * use DGA extension for much better mouse control
 *
 * 16    9/07/99 4:35p Jeff
 * added a call to XSync() to attempt to better the mouse performance...
 *
 * 15    8/22/99 10:00p Jeff
 * attempt to better the mouse motion...but it's not working...XWindows
 * seems to buffer up motion motion events when the mouse moves very
 * small.  Added support for up to 5 mouse buttons
 *
 * 14    8/19/99 2:24p Jeff
 * mouse resets to center of the screen
 *
 * 13    7/14/99 9:06p Jeff
 * added comment header
 *
 * $NoKeywords: $
 */

// ----------------------------------------------------------------------------
//	Mouse Interface
// ----------------------------------------------------------------------------

#include <cstring>
#include <SDL3/SDL.h>

#include "pserror.h"
#include "psclass.h"
#include "ddio.h"
#include "ddio_lnx.h"

static bool ddio_mouseGrabbed = true;
static bool DDIO_mouse_init = false;

static struct mses_state {
  int fd;                   // file descriptor of mouse
  int t, l, b, r;           // limit rectangle of absolute mouse coords
  int x, y, cx, cy;         // current x,y,z in absolute mouse coords
  float dx, dy;             
  int btn_mask;
} DDIO_mouse_state;

struct t_mse_button_info {
  bool is_down[N_MSEBTNS];
  uint8_t down_count[N_MSEBTNS];
  uint8_t up_count[N_MSEBTNS];
  float time_down[N_MSEBTNS];
  float time_up[N_MSEBTNS];
};

struct t_mse_event {
  int16_t btn;
  int16_t state;
};

static t_mse_button_info DIM_buttons;
static tQueue<t_mse_event, 16> MB_queue;
static int Mouse_mode = MOUSE_STANDARD_MODE;

//	---------------------------------------------------------------------------
//	Initialization Functions

bool ddio_MouseInit(void) {
  DDIO_mouse_init = true;
  ddio_MouseReset();
  return true;
}

bool ddio_MouseGetGrab() {
  return ddio_mouseGrabbed;
}

void ddio_MouseSetGrab(bool grab) {
  ddio_mouseGrabbed = grab;
}

int ddio_MouseGetCaps(int *buttons, int *axes) {
  *buttons = 6;
  *axes = 2;
  return MOUSE_LB | MOUSE_CB | MOUSE_RB;
}

void ddio_MouseClose() { DDIO_mouse_init = false; }

void ddio_MouseSetLimits(int left, int top, int right, int bottom, int zmin, int zmax) {
  DDIO_mouse_state.l = left;
  DDIO_mouse_state.t = top;
  DDIO_mouse_state.r = right;
  DDIO_mouse_state.b = bottom;
}

void ddio_MouseGetLimits(int *left, int *top, int *right, int *bottom, int *zmin, int *zmax) {
  *left = DDIO_mouse_state.l;
  *top = DDIO_mouse_state.t;
  *right = DDIO_mouse_state.r;
  *bottom = DDIO_mouse_state.b;

  if (zmin)
    *zmin = 0;
  if (zmax)
    *zmax = 10;
}

void ddio_MouseReset() {
  ddio_MouseSetLimits(0, 0, Lnx_app_obj->m_W, Lnx_app_obj->m_H);
  DDIO_mouse_state.btn_mask = 0;

  // reset button states
  ddio_MouseQueueFlush();

  DDIO_mouse_state.x = DDIO_mouse_state.cx = Lnx_app_obj->m_W / 2;
  DDIO_mouse_state.y = DDIO_mouse_state.cy = Lnx_app_obj->m_H / 2;
  DDIO_mouse_state.dy = DDIO_mouse_state.dx = 0.0f;
}

//	displays the mouse pointer.  Each Hide call = Show call.

static int Mouse_counter = 0;

void ddio_MouseShow() { Mouse_counter++; }

void ddio_MouseHide() { Mouse_counter--; }

void ddio_InternalMouseSuspend(void) {}

void ddio_InternalMouseResume(void) {}

void ddio_MouseMode(int mode) { Mouse_mode = mode; }

// virtual coordinate system for mouse (match to video resolution set for optimal mouse usage.
void ddio_MouseSetVCoords(int width, int height) { ddio_MouseSetLimits(0, 0, width, height); }

bool sdlMouseButtonDownFilter(SDL_Event const *event) {
  ASSERT(event->type == SDL_EVENT_MOUSE_BUTTON_DOWN);

  const SDL_MouseButtonEvent *ev = &event->button;
  t_mse_event mevt;

  if (ev->button == 1) {
    DDIO_mouse_state.btn_mask |= MOUSE_LB;
    DIM_buttons.down_count[0]++;
    DIM_buttons.time_down[0] = timer_GetTime();
    DIM_buttons.is_down[0] = true;
    mevt.btn = 0;
    mevt.state = true;
    MB_queue.send(mevt);
    //		mprintf(0, "MOUSE Button 0: Down\n");
  } else if (ev->button == 2) {
    DDIO_mouse_state.btn_mask |= MOUSE_RB;
    DIM_buttons.down_count[1]++;
    DIM_buttons.time_down[1] = timer_GetTime();
    DIM_buttons.is_down[1] = true;
    mevt.btn = 1;
    mevt.state = true;
    MB_queue.send(mevt);
    //     	mprintf(0, "MOUSE Button 1: Down\n");
  }
  else if (ev->button == 3) {
    DDIO_mouse_state.btn_mask |= MOUSE_CB;
    DIM_buttons.down_count[2]++;
    DIM_buttons.time_down[2] = timer_GetTime();
    DIM_buttons.is_down[2] = true;
    mevt.btn = 2;
    mevt.state = true;
    MB_queue.send(mevt);
    //		mprintf(0, "MOUSE Button 2: Down\n");
  }

  // buttons 4 and 5 are reserved for the mouse wheel...that's how the engine works...adjust in here.

  else if (ev->button == 4) {
    DDIO_mouse_state.btn_mask |= MOUSE_B6;
    DIM_buttons.down_count[5]++;
    DIM_buttons.time_down[5] = timer_GetTime();
    DIM_buttons.is_down[5] = true;
    mevt.btn = 5;
    mevt.state = true;
    MB_queue.send(mevt);
    //		mprintf(0, "MOUSE Button 5: Down\n");
  }
  else if (ev->button == 5) {
    DDIO_mouse_state.btn_mask |= MOUSE_B7;
    DIM_buttons.down_count[6]++;
    DIM_buttons.time_down[6] = timer_GetTime();
    DIM_buttons.is_down[6] = true;
    mevt.btn = 6;
    mevt.state = true;
    MB_queue.send(mevt);
    //		mprintf(0, "MOUSE Button 6: Down\n");
  }
  else if (ev->button == 6) {
    DDIO_mouse_state.btn_mask |= MOUSE_B8;
    DIM_buttons.down_count[7]++;
    DIM_buttons.time_down[7] = timer_GetTime();
    DIM_buttons.is_down[7] = true;
    mevt.btn = 7;
    mevt.state = true;
    MB_queue.send(mevt);
    //		mprintf(0, "MOUSE Button 7: Down\n");
  }

  return false;
}

bool sdlMouseButtonUpFilter(SDL_Event const *event) {
  ASSERT(event->type == SDL_EVENT_MOUSE_BUTTON_UP);

  const SDL_MouseButtonEvent *ev = &event->button;
  t_mse_event mevt;

  if (ev->button == 1) {
    DDIO_mouse_state.btn_mask &= (~MOUSE_LB);
    DIM_buttons.up_count[0]++;
    DIM_buttons.is_down[0] = false;
    DIM_buttons.time_up[0] = timer_GetTime();
    mevt.btn = 0;
    mevt.state = false;
    MB_queue.send(mevt);
    //		mprintf(0, "MOUSE Button 0: Up\n");
  }
  else if (ev->button == 2) {
    DDIO_mouse_state.btn_mask &= (~MOUSE_RB);
    DIM_buttons.up_count[1]++;
    DIM_buttons.is_down[1] = false;
    DIM_buttons.time_up[1] = timer_GetTime();
    mevt.btn = 1;
    mevt.state = false;
    MB_queue.send(mevt);
    //		mprintf(0, "MOUSE Button 1: Up\n");
  }
  else if (ev->button == 3) {
    DDIO_mouse_state.btn_mask &= (~MOUSE_CB);
    DIM_buttons.up_count[2]++;
    DIM_buttons.is_down[2] = false;
    DIM_buttons.time_up[2] = timer_GetTime();
    mevt.btn = 2;
    mevt.state = false;
    MB_queue.send(mevt);
    //		mprintf(0, "MOUSE Button 2: Up\n");

  }

  // buttons 4 and 5 are reserved for the mouse wheel...that's how the engine works...adjust in here.

  else if (ev->button == 4) {
    DDIO_mouse_state.btn_mask &= (~MOUSE_B6);
    DIM_buttons.up_count[5]++;
    DIM_buttons.is_down[5] = false;
    DIM_buttons.time_up[5] = timer_GetTime();
    mevt.btn = 5;
    mevt.state = false;
    MB_queue.send(mevt);
    //		mprintf(0, "MOUSE Button 5: Up\n");
  }
  else if (ev->button == 5) {
    DDIO_mouse_state.btn_mask &= (~MOUSE_B7);
    DIM_buttons.up_count[6]++;
    DIM_buttons.is_down[6] = false;
    DIM_buttons.time_up[6] = timer_GetTime();
    mevt.btn = 6;
    mevt.state = false;
    MB_queue.send(mevt);
    //		mprintf(0, "MOUSE Button 6: Up\n");
  }
  else if (ev->button == 6) {
    DDIO_mouse_state.btn_mask &= (~MOUSE_B8);
    DIM_buttons.up_count[7]++;
    DIM_buttons.is_down[7] = false;
    DIM_buttons.time_up[7] = timer_GetTime();
    mevt.btn = 7;
    mevt.state = false;
    MB_queue.send(mevt);
    //		mprintf(0, "MOUSE Button 7: Up\n");
  }

  return false;
}

bool sdlMouseWheelFilter(SDL_Event const *event) {
  ASSERT(event->type == SDL_EVENT_MOUSE_WHEEL);

  const SDL_MouseWheelEvent *ev = &event->wheel;
  t_mse_event mevt;

  // !!! FIXME: this ignores horizontal wheels for now, since Descent3 doesn't currently have a concept of them
  // !!! FIXME: (vertical mouse wheels are represented as mouse buttons 4 and 5, incorrectly, on all platforms).
  // !!! FIXME: this will require improvements to the engine before this changes here, though.

  if (ev->y > 0) { /* Mouse scroll up */
    DDIO_mouse_state.btn_mask |= MOUSE_B5;
    DIM_buttons.down_count[4]++;
    DIM_buttons.time_down[4] = timer_GetTime();
    DIM_buttons.is_down[4] = true;
    mevt.btn = 4;
    mevt.state = true;
    MB_queue.send(mevt);

    // send an immediate release event, as if the "button" was clicked. !!! FIXME: this also needs improvements in the engine.
    // don't remove from btn_mask
    DIM_buttons.up_count[4]++;
    DIM_buttons.is_down[4] = false;
    DIM_buttons.time_up[4] = timer_GetTime();
    mevt.btn = 4;
    mevt.state = false;
    MB_queue.send(mevt);
    //		mprintf(0, "MOUSE Scrollwheel: Rolled Up\n");
  } else if (ev->y < 0) { /* Mouse scroll down */
    DDIO_mouse_state.btn_mask |= MOUSE_B6;
    DIM_buttons.down_count[5]++;
    DIM_buttons.time_down[5] = timer_GetTime();
    DIM_buttons.is_down[5] = true;
    mevt.btn = 5;
    mevt.state = true;
    MB_queue.send(mevt);

    // send an immediate release event, as if the "button" was clicked. !!! FIXME: this also needs improvements in the engine.
    // don't remove from btn_mask
    DIM_buttons.up_count[5]++;
    DIM_buttons.is_down[5] = false;
    DIM_buttons.time_up[5] = timer_GetTime();
    mevt.btn = 5;
    mevt.state = false;
    MB_queue.send(mevt);
    //		mprintf(0, "MOUSE Scrollwheel: Rolled Down\n");
  }

  return false;
}

bool sdlMouseMotionFilter(SDL_Event const *event) {
  if (event->type == SDL_EVENT_JOYSTICK_BALL_MOTION) {
    DDIO_mouse_state.dx = event->jball.xrel / 100.0f;
    DDIO_mouse_state.dy = event->jball.yrel / 100.0f;
    DDIO_mouse_state.x += DDIO_mouse_state.dx;
    DDIO_mouse_state.y += DDIO_mouse_state.dy;
  } else {
    DDIO_mouse_state.dx += event->motion.xrel;
    DDIO_mouse_state.dy += event->motion.yrel;
    DDIO_mouse_state.x += DDIO_mouse_state.dx;
    DDIO_mouse_state.y += DDIO_mouse_state.dy;
  }

  if (DDIO_mouse_state.x < DDIO_mouse_state.l)
    DDIO_mouse_state.x = DDIO_mouse_state.l;
  if (DDIO_mouse_state.x >= DDIO_mouse_state.r)
    DDIO_mouse_state.x = DDIO_mouse_state.r - 1;
  if (DDIO_mouse_state.y < DDIO_mouse_state.t)
    DDIO_mouse_state.y = DDIO_mouse_state.t;
  if (DDIO_mouse_state.y >= DDIO_mouse_state.b)
    DDIO_mouse_state.y = DDIO_mouse_state.b - 1;

  return false;
}

//	This function will handle all mouse events.
void ddio_InternalMouseFrame(void) {
  static unsigned frame_count = 0;
  SDL_PumpEvents();
  frame_count++;
}

/*	x, y = absolute mouse position
        dx, dy = mouse deltas since last call
        return value is mouse button mask
*/
int ddio_MouseGetState(int *x, int *y, int *dx, int *dy, int *z, int *dz) {
  //	update mouse timer.
  int btn_mask = DDIO_mouse_state.btn_mask;

  //	get return values.
  if (x)
    *x = DDIO_mouse_state.x;
  if (y)
    *y = DDIO_mouse_state.y;
  if (z)
    *z = 0;
  if (dx)
    *dx = DDIO_mouse_state.dx;
  if (dy)
    *dy = DDIO_mouse_state.dy;
  if (dz)
    *dz = 0;

  DDIO_mouse_state.dx = 0.0f;
  DDIO_mouse_state.dy = 0.0f;

  // unset the mouse wheel "button" once it's been retrieved.
  DDIO_mouse_state.btn_mask &= ~(MOUSE_B5|MOUSE_B6);

  return btn_mask;
}

// resets mouse queue and button info only.
void ddio_MouseQueueFlush() {
  memset(&DIM_buttons, 0, sizeof(DIM_buttons));
  MB_queue.flush();
}

// gets a mouse button event, returns false if none.
bool ddio_MouseGetEvent(int *btn, bool *state) {
  t_mse_event evt;

  if (MB_queue.recv(&evt)) {
    *btn = (int)evt.btn;
    *state = evt.state ? true : false;
    return true;
  }

  return false;
}

// return mouse button down time.
float ddio_MouseBtnDownTime(int btn) {
  float dtime, curtime = timer_GetTime();

  ASSERT(btn >= 0 && btn < N_MSEBTNS);

  if (DIM_buttons.is_down[btn]) {
    dtime = curtime - DIM_buttons.time_down[btn];
    DIM_buttons.time_down[btn] = curtime;
  } else {
    dtime = DIM_buttons.time_up[btn] - DIM_buttons.time_down[btn];
    DIM_buttons.time_down[btn] = 0;
    DIM_buttons.time_up[btn] = 0;
  }

  return dtime;
}

// return mouse button down time
int ddio_MouseBtnDownCount(int btn) {
  if (btn < 0 || btn >= N_MSEBTNS)
    return 0;
  int n_downs = DIM_buttons.down_count[btn];

  if (n_downs) {
    DIM_buttons.down_count[btn] = 0;
  }

  return n_downs;
}

// return mouse button up count
int ddio_MouseBtnUpCount(int btn) {
  if (btn < 0 || btn >= N_MSEBTNS)
    return 0;
  int n_ups = DIM_buttons.up_count[btn];
  DIM_buttons.up_count[btn] = 0;
  return n_ups;
}

char Ctltext_MseBtnBindings[N_MSEBTNS][32] = {"mse-1\0\0\0\0\0\0\0\0\0\0\0\0",
                                              "mse-2\0\0\0\0\0\0\0\0\0\0\0\0",
                                              "mse-3\0\0\0\0\0\0\0\0\0\0\0\0",
                                              "mse-4\0\0\0\0\0\0\0\0\0\0\0",
                                              "msew-u\0\0\0\0\0\0\0\0\0\0\0",
                                              "msew-d\0\0\0\0\0\0\0\0\0\0\0",
                                              "",
                                              ""};

char Ctltext_MseAxisBindings[][32] = {"mse-X\0\0\0\0\0\0\0\0\0\0\0\0", "mse-Y\0\0\0\0\0\0\0\0\0\0\0\0",
                                      "msewheel\0\0\0\0\0\0\0\0\0\0"};

// returns string to binding.
const char *ddio_MouseGetBtnText(int btn) {
  if (btn >= N_MSEBTNS || btn < 0)
    return ("");
  return Ctltext_MseBtnBindings[btn];
}

const char *ddio_MouseGetAxisText(int axis) {
  if (axis < 0 || static_cast<size_t>(axis) >= std::size(Ctltext_MseAxisBindings))
    return ("");
  return Ctltext_MseAxisBindings[axis];
}
