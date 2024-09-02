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

 * $Logfile: /DescentIII/Main/ddio_lnx/lnxkey_sdl.cpp $
 * $Revision: 1.6 $
 * $Date: 2001/02/07 09:18:02 $
 * $Author: icculus $
 *
 * Linux Xwindows routines
 *
 * $Log: lnxkey_sdl.cpp,v $
 * Revision 1.6  2001/02/07 09:18:02  icculus
 * Fullscreen/windowed toggle keybinding attempts to toggle regardless of
 * renderer, now. Otherwise, 2D surfaces (movies) wouldn't toggle. Glide uses
 * can just risk it.
 *
 * Revision 1.5  2001/01/11 11:40:53  icculus
 * Added CTRL-G and ALT-Enter keybindings.
 *
 * Revision 1.4  2000/09/22 19:01:49  icculus
 * SDLK_WORLD* support.
 *
 * Revision 1.3  2000/06/29 09:52:45  hercules
 * Don't poll for events twice per frame (already done in mouse driver)
 *
 * Revision 1.2  2000/06/24 01:15:15  icculus
 * patched to compile.
 *
 * Revision 1.1  2000/04/27 11:16:01  icculus
 * First (and hopefully last) commit.
 *
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

#include <SDL.h>

#include "ddio.h"
#include "pserror.h"


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

int sdlkey_to_ddiocode[27] = {0,     KEY_A, KEY_B, KEY_C, KEY_D, KEY_E, KEY_F, KEY_G, KEY_H,
                              KEY_I, KEY_J, KEY_K, KEY_L, KEY_M, KEY_N, KEY_O, KEY_P, KEY_Q,
                              KEY_R, KEY_S, KEY_T, KEY_U, KEY_V, KEY_W, KEY_X, KEY_Y, KEY_Z};

static inline uint8_t sdlkeycode_to_keycode(uint32_t sdlkeycode) {
  // unceremoniously taken from Heretic source code with a few modifications.
  //  (by Outrage. Not Loki. We know better.  :)  --ryan.)
  int rc = sdlkeycode;

  switch (rc) {
  case SDLK_DELETE:
    rc = KEY_DELETE;
    break;
  case SDLK_INSERT:
    rc = KEY_INSERT;
    break;
  case SDLK_PAGEUP:
    rc = KEY_PAGEUP;
    break;
  case SDLK_PAGEDOWN:
    rc = KEY_PAGEDOWN;
    break;
  case SDLK_HOME:
    rc = KEY_HOME;
    break;
  case SDLK_END:
    rc = KEY_END;
    break;
  case SDLK_LEFT:
    rc = KEY_LEFT;
    break;
  case SDLK_RIGHT:
    rc = KEY_RIGHT;
    break;
  case SDLK_DOWN:
    rc = KEY_DOWN;
    break;
  case SDLK_UP:
    rc = KEY_UP;
    break;
  case SDLK_ESCAPE:
    rc = KEY_ESC;
    break;
  case SDLK_RETURN:
    rc = KEY_ENTER;
    break;
  case SDLK_TAB:
    rc = KEY_TAB;
    break;
  case SDLK_F1:
    rc = KEY_F1;
    break;
  case SDLK_F2:
    rc = KEY_F2;
    break;
  case SDLK_F3:
    rc = KEY_F3;
    break;
  case SDLK_F4:
    rc = KEY_F4;
    break;
  case SDLK_F5:
    rc = KEY_F5;
    break;
  case SDLK_F6:
    rc = KEY_F6;
    break;
  case SDLK_F7:
    rc = KEY_F7;
    break;
  case SDLK_F8:
    rc = KEY_F8;
    break;
  case SDLK_F9:
    rc = KEY_F9;
    break;
  case SDLK_F10:
    rc = KEY_F10;
    break;
  case SDLK_F11:
    rc = KEY_F11;
    break;
  case SDLK_F12:
    rc = KEY_F12;
    break;
  case SDLK_BACKSPACE:
    rc = KEY_BACKSP;
    break;
  case SDLK_PAUSE:
    rc = KEY_PAUSE;
    break;
  case SDLK_KP_PLUS:
    rc = KEY_PADPLUS;
    break;
  case SDLK_KP_MINUS:
    rc = KEY_PADMINUS;
    break;
  case SDLK_KP_ENTER:
    rc = KEY_PADENTER;
    break;
  case SDLK_KP_PERIOD:
    rc = KEY_PADPERIOD;
    break;
  case SDLK_KP_DIVIDE:
    rc = KEY_PADDIVIDE;
    break;
  case SDLK_KP_MULTIPLY:
    rc = KEY_PADMULTIPLY;
    break;
  case SDLK_KP_0:
    rc = KEY_PAD0;
    break;
  case SDLK_KP_1:
    rc = KEY_PAD1;
    break;
  case SDLK_KP_2:
    rc = KEY_PAD2;
    break;
  case SDLK_KP_3:
    rc = KEY_PAD3;
    break;
  case SDLK_KP_4:
    rc = KEY_PAD4;
    break;
  case SDLK_KP_5:
    rc = KEY_PAD5;
    break;
  case SDLK_KP_6:
    rc = KEY_PAD6;
    break;
  case SDLK_KP_7:
    rc = KEY_PAD7;
    break;
  case SDLK_KP_8:
    rc = KEY_PAD8;
    break;
  case SDLK_KP_9:
    rc = KEY_PAD9;
    break;
  case SDLK_SPACE:
    rc = KEY_SPACEBAR;
    break;
  case SDLK_1:
    rc = KEY_1;
    break;
  case SDLK_2:
    rc = KEY_2;
    break;
  case SDLK_3:
    rc = KEY_3;
    break;
  case SDLK_4:
    rc = KEY_4;
    break;
  case SDLK_5:
    rc = KEY_5;
    break;
  case SDLK_6:
    rc = KEY_6;
    break;
  case SDLK_7:
    rc = KEY_7;
    break;
  case SDLK_8:
    rc = KEY_8;
    break;
  case SDLK_9:
    rc = KEY_9;
    break;
  case SDLK_0:
    rc = KEY_0;
    break;

    // modifier keys
  case SDLK_LSHIFT:
    rc = KEY_LSHIFT;
    break;
  case SDLK_RSHIFT:
    rc = KEY_RSHIFT;
    break;
  case SDLK_LCTRL:
    rc = KEY_LCTRL;
    break;
  case SDLK_RCTRL:
    rc = KEY_RCTRL;
    break;
  case SDLK_LALT:
  case SDLK_LGUI:
    rc = KEY_LALT;
    break;
  case SDLK_RALT:
  case SDLK_RGUI:
    rc = KEY_RALT;
    break;

    // since we look at unmodified keycodes, we don't have to worry about shifted states, I think.
  case SDLK_EQUALS:
    rc = KEY_EQUAL;
    break;
  case SDLK_MINUS:
    rc = KEY_MINUS;
    break;
  case SDLK_LEFTBRACKET:
    rc = KEY_LBRACKET;
    break;
  case SDLK_RIGHTBRACKET:
    rc = KEY_RBRACKET;
    break;
  case SDLK_BACKSLASH:
    rc = KEY_BACKSLASH;
    break;
  case SDLK_BACKQUOTE:
    rc = KEY_LAPOSTRO;
    break;
  case SDLK_QUOTE:
    rc = KEY_RAPOSTRO;
    break;
  case SDLK_SEMICOLON:
    rc = KEY_SEMICOL;
    break;
  case SDLK_PERIOD:
    rc = KEY_PERIOD;
    break;
  case SDLK_SLASH:
    rc = KEY_SLASH;
    break;
  case SDLK_COMMA:
    rc = KEY_COMMA;
    break;
  case SDLK_NUMLOCKCLEAR:
    rc = KEY_NUMLOCK;
    break;
  case SDLK_PRINTSCREEN:
    rc = KEY_PRINT_SCREEN;
    break;
  case SDLK_SCROLLLOCK:
    rc = KEY_SCROLLOCK;
    break;
  case SDLK_CAPSLOCK:
    rc = KEY_CAPSLOCK;
    break;

    // convert 'a' - 'z' to 0-27, and then convert to ddio format.
  default:
    if (rc >= SDLK_a && rc <= SDLK_z) {
      rc = (rc - SDLK_a) + 1;
    } else {
      rc = 0;
    }
    ASSERT(rc >= 0 && rc <= 26);
    rc = sdlkey_to_ddiocode[rc];
    break;
  }

  return (uint8_t)rc;
}

int sdlKeyFilter(const SDL_Event *event) {
  uint8_t kc = 0;

  if ((event->type != SDL_KEYUP) && (event->type != SDL_KEYDOWN))
    return (1);

  switch (event->key.state) {
  case SDL_PRESSED:
    if (event->key.repeat) break;  // ignore these, we only want to know if it's a first time pressed, not a key-repeat.
    kc = sdlkeycode_to_keycode(event->key.keysym.sym);
    if (event->key.keysym.mod & KMOD_CTRL) {
      switch (kc) {
      case KEY_G: // toggle grabbed input.
        bool grab = !ddio_MouseGetGrab();
        ddio_MouseSetGrab(grab);
        SDL_SetRelativeMouseMode((SDL_bool)grab);
        return 0;
      } // switch
    }   // if

    else if (event->key.keysym.mod & KMOD_ALT) {
      if ((kc == KEY_ENTER) || (kc == KEY_PADENTER)) {
        extern SDL_Window *GSDLWindow;
        Uint32 flags = SDL_GetWindowFlags(GSDLWindow);
        // (In SDL2, SDL_WINDOW_FULLSCREEN_DESKTOP is SDL_WINDOW_FULLSCREEN plus an extra bit set, so just check for _any_ fullscreen in this bitwise AND.)
        if (flags & SDL_WINDOW_FULLSCREEN) {
          flags &= ~SDL_WINDOW_FULLSCREEN_DESKTOP;
        } else {
          flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
        }
        SDL_SetWindowFullscreen(GSDLWindow, flags);
        return(0);
      } // if
    }   // else if

    LKeys[kc].down_time = timer_GetTime();
    LKeys[kc].status = true;
    ddio_UpdateKeyState(kc, true);
    break;

  case SDL_RELEASED:
    kc = sdlkeycode_to_keycode(event->key.keysym.sym);
    if (LKeys[kc].status) {
      LKeys[kc].up_time = timer_GetTime();
      LKeys[kc].status = false;
      ddio_UpdateKeyState(kc, false);
    } // if
    break;
  } // switch

  return (0);
} // sdlKeyFilter

bool ddio_sdl_InternalKeyInit(ddio_init_info *init_info) {
  // reset key list
  for (int i = 0; i < DDIO_MAX_KEYS; i++) {
    LKeys[i].down_ticks = 0;
    LKeys[i].up_ticks = 0;
    LKeys[i].status = false;
  }
  return true;
}

void ddio_sdl_InternalKeyClose() {}

bool ddio_sdl_InternalKeyState(uint8_t key) { return LKeys[key].status; }

void ddio_sdl_InternalKeySuspend() {}

void ddio_sdl_InternalKeyResume() {}

float ddio_sdl_InternalKeyDownTime(uint8_t key) {
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

void ddio_sdl_InternalResetKey(uint8_t key) {
  LKeys[key].down_time = 0.0f;
  LKeys[key].up_time = 0.0f;
  LKeys[key].status = false;
}

void ddio_sdl_InternalKeyFrame(void) {
  // Sam 6/28 - This is already done in the mouse handling code
  // SDL_PumpEvents();
}
