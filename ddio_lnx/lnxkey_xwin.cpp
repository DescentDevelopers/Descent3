/*
 * $Logfile: /DescentIII/Main/ddio_lnx/lnxkey_xwin.cpp $
 * $Revision: 1.1.1.1 $
 * $Date: 2000/04/18 00:00:33 $
 * $Author: icculus $
 *
 * Linux Xwindows routines
 *
 * $Log: lnxkey_xwin.cpp,v $
 * Revision 1.1.1.1  2000/04/18 00:00:33  icculus
 * initial checkin
 *
 *
 * 6     8/19/99 3:45p Jeff
 * commented out mprintfs
 *
 * 5     8/19/99 1:32p Jeff
 * fixed o key. Added scroll lock, caps lock and print screen keys.
 *
 * 4     7/15/99 11:37a Jeff
 * moved autorepeat calls to lnxapp.cpp
 *
 * 3     7/14/99 9:06p Jeff
 * added comment header
 *
 * $NoKeywords: $
 */

#include "pserror.h"
#include "mono.h"
#include "ddio.h"
#include "ddio_lnx.h"
#include "application.h"
#include "../lib/linux/dyna_xwin.h"
#include "../lib/linux/dyna_xext.h"

//#include <X11/Xlib.h>
//#include <X11/Xutil.h>
#include <X11/keysym.h>

extern volatile struct tLnxKeys {
  union {
    int up_ticks;
    float up_time;
  };
  union {
    int down_ticks;
    float down_time;
  };
  bool status;
} LKeys[DDIO_MAX_KEYS];

// we need this for our XWindows calls.
static Display *X_display = NULL;

bool ddio_xwin_InternalKeyInit(ddio_init_info *init_info) {
  oeLnxApplication *app = (oeLnxApplication *)init_info->obj;

  X_display = app->m_Display;

  // reset key list
  for (int i = 0; i < DDIO_MAX_KEYS; i++) {
    LKeys[i].down_ticks = 0;
    LKeys[i].up_ticks = 0;
    LKeys[i].status = false;
  }

  return true;
}

void ddio_xwin_InternalKeyClose() {
  // Don't call any X functions here! At this point the XLib has been unloaded
  if (X_display) {
    X_display = NULL;
  }
}

bool ddio_xwin_InternalKeyState(ubyte key) { return LKeys[key].status; }

void ddio_xwin_InternalKeySuspend() {}

void ddio_xwin_InternalKeyResume() {}

float ddio_xwin_InternalKeyDownTime(ubyte key) {
  float down_time = 0.0f;
  if (LKeys[key].status) {
    float timer = timer_GetTime();
    down_time = timer - LKeys[key].down_time;
    LKeys[key].down_time = timer;
  } else {
    down_time = LKeys[key].up_time - LKeys[key].down_time;
    LKeys[key].down_time = LKeys[key].up_time = 0.0f;
  }

  return down_time;
}

void ddio_xwin_InternalResetKey(ubyte key) {
  LKeys[key].down_time = 0.0f;
  LKeys[key].up_time = 0.0f;
  LKeys[key].status = false;
}

int xkey_to_ddiocode[27] = {0,     KEY_A, KEY_B, KEY_C, KEY_D, KEY_E, KEY_F, KEY_G, KEY_H,
                            KEY_I, KEY_J, KEY_K, KEY_L, KEY_M, KEY_N, KEY_O, KEY_P, KEY_Q,
                            KEY_R, KEY_S, KEY_T, KEY_U, KEY_V, KEY_W, KEY_X, KEY_Y, KEY_Z};

inline ubyte xkeycode_to_keycode(unsigned int xkeycode) {
  int rc = XKeycodeToKeysym(X_display, xkeycode, 0);

  // unceremoniously taken from Heretic source code with a few modifications.
  switch (rc) {
  case XK_Delete:
    rc = KEY_DELETE;
    break;
  case XK_Insert:
    rc = KEY_INSERT;
    break;
  case XK_Page_Up:
    rc = KEY_PAGEUP;
    break;
  case XK_Page_Down:
    rc = KEY_PAGEDOWN;
    break;
  case XK_Home:
    rc = KEY_HOME;
    break;
  case XK_End:
    rc = KEY_END;
    break;
  case XK_Left:
    rc = KEY_LEFT;
    break;
  case XK_Right:
    rc = KEY_RIGHT;
    break;
  case XK_Down:
    rc = KEY_DOWN;
    break;
  case XK_Up:
    rc = KEY_UP;
    break;
  case XK_Escape:
    rc = KEY_ESC;
    break;
  case XK_Return:
    rc = KEY_ENTER;
    break;
  case XK_Tab:
    rc = KEY_TAB;
    break;
  case XK_F1:
    rc = KEY_F1;
    break;
  case XK_F2:
    rc = KEY_F2;
    break;
  case XK_F3:
    rc = KEY_F3;
    break;
  case XK_F4:
    rc = KEY_F4;
    break;
  case XK_F5:
    rc = KEY_F5;
    break;
  case XK_F6:
    rc = KEY_F6;
    break;
  case XK_F7:
    rc = KEY_F7;
    break;
  case XK_F8:
    rc = KEY_F8;
    break;
  case XK_F9:
    rc = KEY_F9;
    break;
  case XK_F10:
    rc = KEY_F10;
    break;
  case XK_F11:
    rc = KEY_F11;
    break;
  case XK_F12:
    rc = KEY_F12;
    break;
  case XK_BackSpace:
    rc = KEY_BACKSP;
    break;
  case XK_Pause:
    rc = KEY_PAUSE;
    break;
  case XK_KP_Add:
    rc = KEY_PADPLUS;
    break;
  case XK_KP_Subtract:
    rc = KEY_PADMINUS;
    break;
  case XK_KP_Enter:
    rc = KEY_PADENTER;
    break;
  case XK_KP_Decimal:
    rc = KEY_PADPERIOD;
    break;
  case XK_KP_Divide:
    rc = KEY_PADDIVIDE;
    break;
  case XK_KP_Multiply:
    rc = KEY_PADMULTIPLY;
    break;
  case XK_KP_0:
    rc = KEY_PAD0;
    break;
  case XK_KP_1:
    rc = KEY_PAD1;
    break;
  case XK_KP_2:
    rc = KEY_PAD2;
    break;
  case XK_KP_3:
    rc = KEY_PAD3;
    break;
  case XK_KP_4:
    rc = KEY_PAD4;
    break;
  case XK_KP_5:
    rc = KEY_PAD5;
    break;
  case XK_KP_6:
    rc = KEY_PAD6;
    break;
  case XK_KP_7:
    rc = KEY_PAD7;
    break;
  case XK_KP_8:
    rc = KEY_PAD8;
    break;
  case XK_KP_9:
    rc = KEY_PAD9;
    break;
  case XK_space:
    rc = KEY_SPACEBAR;
    break;
  case XK_1:
    rc = KEY_1;
    break;
  case XK_2:
    rc = KEY_2;
    break;
  case XK_3:
    rc = KEY_3;
    break;
  case XK_4:
    rc = KEY_4;
    break;
  case XK_5:
    rc = KEY_5;
    break;
  case XK_6:
    rc = KEY_6;
    break;
  case XK_7:
    rc = KEY_7;
    break;
  case XK_8:
    rc = KEY_8;
    break;
  case XK_9:
    rc = KEY_9;
    break;
  case XK_0:
    rc = KEY_0;
    break;

    // since we look at unmodified keycodes, we don't have to worry about shifted states, I think.
  case XK_equal:
    rc = KEY_EQUAL;
    break;
  case XK_minus:
    rc = KEY_MINUS;
    break;
  case XK_bracketleft:
    rc = KEY_LBRACKET;
    break;
  case XK_bracketright:
    rc = KEY_RBRACKET;
    break;
  case XK_backslash:
    rc = KEY_BACKSLASH;
    break;
  case XK_quoteleft:
    rc = KEY_LAPOSTRO;
    break;
  case XK_quoteright:
    rc = KEY_RAPOSTRO;
    break;
  case XK_semicolon:
    rc = KEY_SEMICOL;
    break;
  case XK_period:
    rc = KEY_PERIOD;
    break;
  case XK_slash:
    rc = KEY_SLASH;
    break;
  case XK_comma:
    rc = KEY_COMMA;
    break;
  case XK_Num_Lock:
    rc = KEY_NUMLOCK;
    break;
  case XK_Print:
    rc = KEY_PRINT_SCREEN;
    break;
  case XK_Scroll_Lock:
    rc = KEY_SCROLLOCK;
    break;
  case XK_Caps_Lock:
  case XK_Shift_Lock:
    rc = KEY_CAPSLOCK;
    break;

    // modifier keys
  case XK_Shift_L:
    rc = KEY_LSHIFT;
    break;
  case XK_Shift_R:
    rc = KEY_RSHIFT;
    break;
  case XK_Control_L:
    rc = KEY_LCTRL;
    break;
  case XK_Control_R:
    rc = KEY_RCTRL;
    break;
  case XK_Alt_L:
  case XK_Meta_L:
    rc = KEY_LALT;
    break;
  case XK_Alt_R:
  case XK_Meta_R:
    rc = KEY_RALT;
    break;

    // convert 'a' - 'z' to 0-27, and then convert to ddio format.
  default:
    if (rc >= XK_a && rc <= XK_z) {
      rc = (rc - XK_a) + 1;
    } else if (rc >= XK_A && rc <= XK_Z) {
      rc = (rc - XK_A) + 1;
    } else {
      rc = 0;
    }
    ASSERT(rc >= 0 && rc <= 26);
    rc = xkey_to_ddiocode[rc];
    break;
  }

  return (ubyte)rc;
}

void ddio_xwin_InternalKeyFrame(void) {
  XEvent evt;

  //	this is the big function.
  //	we'll grab all the keyboard events from the event queue currently
  //	and process them accordingly.

  while (XCheckMaskEvent(X_display, KeyPressMask | KeyReleaseMask, &evt)) {
    ubyte kc;

    switch (evt.type) {
    case KeyPress:
      kc = xkeycode_to_keycode(evt.xkey.keycode);
      LKeys[kc].down_time = timer_GetTime();
      LKeys[kc].status = true;
      ddio_UpdateKeyState(kc, true);
      // mprintf((0, "key %x down (state=%d).\n", (int)kc, (int)evt.xkey.state));
      break;

    case KeyRelease:
      kc = xkeycode_to_keycode(evt.xkey.keycode);
      if (LKeys[kc].status) {
        LKeys[kc].up_time = timer_GetTime();
        LKeys[kc].status = false;
        ddio_UpdateKeyState(kc, false);
        // mprintf((0, "key %x up (state=%d).\n", (int)kc, (int)evt.xkey.state));
      }
      break;
    }
  }
}