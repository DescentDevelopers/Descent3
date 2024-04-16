// Keyboard handler for SVGAlib

#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdarg.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <signal.h>
#include <sys/stat.h>
#include <linux/keyboard.h>
#include <sys/ioctl.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <ggi/gii.h>
#include <ctype.h>

#include "ddio_common.h"
#include "ddio.h"
#include "mono.h"

//##########################################################
// GGI Interface Functions
//##########################################################
void ddio_ggi_EmergencyQuit(int id);
void ddio_ggi_InternalKeyInit(void);
static int ddio_ggi_TranslateKey(int c, int code);
void ddio_ggi_DoKeyFrame(void);
void ddio_ggi_InternalClose(void);
void ddio_ggi_KeyHandler(void);
gii_input_t GGI_input_handle;
void ddio_ggi_KeyBoardEventHandler(int transkey, int press);
//##########################################################

volatile struct tLnxKeys {
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
bool DDIO_key_suspend = false;

bool ddio_InternalKeyInit(bool preemptive) {
  static bool first_call = true;
  // reset key list
  for (int i = 0; i < DDIO_MAX_KEYS; i++) {
    LKeys[i].down_ticks = 0;
    LKeys[i].up_ticks = 0;
    LKeys[i].status = false;
  }
  DDIO_key_suspend = false;

  ddio_ggi_InternalKeyInit();

  if (first_call) {
    atexit(ddio_InternalKeyClose);
    first_call = false;
  }

  return true;
}

void ddio_InternalKeyClose() { ddio_ggi_InternalClose(); }

//	ddio_KeyFrame
//		handle input of keyboard per frame.
bool ddio_KeyFrame(void) {
  ddio_ggi_DoKeyFrame();
  return true;
}

void ddio_InternalKeyFrame(void) {
  ddio_KeyFrame(); // which gets called?
}

bool ddio_InternalKeyState(ubyte key) { return LKeys[key].status; }

void ddio_InternalKeySuspend() { DDIO_key_suspend = true; }

void ddio_InternalKeyResume() { DDIO_key_suspend = false; }

float ddio_InternalKeyDownTime(ubyte key) {
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

void ddio_InternalResetKey(ubyte key) {
  LKeys[key].down_time = 0.0f;
  LKeys[key].up_time = 0.0f;
  LKeys[key].status = false;
}

static unsigned char ascii_table[128] = {
    255, 255, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 255, 255, 'q', 'w', 'e', 'r', 't', 'y',
    'u', 'i', 'o', 'p', '[', ']', 255, 255, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', 39,  '`', 255, '\\',
    'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 255, '*', 255, ' ', 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255};

static unsigned char shifted_ascii_table[128] = {
    255, 255, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', 255, 255, 'Q', 'W', 'E', 'R', 'T', 'Y',
    'U', 'I', 'O', 'P', '{', '}', 255, 255, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~', 255, '|',
    'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 255, 255, 255, ' ', 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255};

//	converts keycode to ASCII
int ddio_KeyToAscii(int code) {
  int shifted;

  shifted = code & KEY_SHIFTED;
  code &= 0xFF;

  if (code >= 127) {
    return 255;
  }

  int out;
  if (shifted) {
    out = (int)shifted_ascii_table[code];
  } else {
    out = (int)ascii_table[code];
  }
  return out;
}

//#################################################
// ggi Interface Functions
//#################################################
void ddio_ggi_InternalKeyInit(void) {
  static bool first_time = true;

  if (!first_time) {
    return;
  }
  first_time = false;

  if (giiInit()) {
    mprintf((0, "Unable to initialize GII!\n"));
    exit(1);
  }

  gii_input_t inp2;
  // Open the nulldevice for testing ...
  if ((GGI_input_handle = giiOpen("input-null", NULL)) == NULL) {
    giiPanic("Unable to open input-null\n");
    exit(1);
  }

  // Open stdin for testing ...
  if ((inp2 = giiOpen("input-stdin", NULL)) == NULL) {
    giiPanic("Unable to open input-stdin\n");
    exit(1);
  }

  // Now join them. Note the usage of _i_n_p_=_giiJoin(inp,inp2);
  // This is the recommended way to do this.
  GGI_input_handle = giiJoinInputs(GGI_input_handle, inp2);

  giiSetEventMask(GGI_input_handle, emKey);

  struct sigaction sact;
  sact.sa_handler = ddio_ggi_EmergencyQuit;
  sigfillset(&sact.sa_mask);
  sact.sa_flags = 0;
  sigaction(SIGINT, &sact, NULL);
}

void ddio_ggi_InternalClose(void) {
  static bool first_time = true;

  if (first_time) {
    giiExit();
    first_time = false;
  }
}

void ddio_ggi_EmergencyQuit(int id) {
  giiPanic("GII System shutdown...\n");
  // somehow we need to signal quit here
  exit(-1);
}

void ddio_ggi_KeyBoardEventHandler(int transkey, int press) {
  if (press == 1) {
    LKeys[transkey].status = 1;
    LKeys[transkey].down_time = timer_GetTime();
    ddio_UpdateKeyState(transkey, true);
  }
  if (press == 0) {
    LKeys[transkey].status = 0;
    LKeys[transkey].up_time = timer_GetTime();
    ddio_UpdateKeyState(transkey, false);
  }
}

void ddio_ggi_DoKeyFrame(void) { ddio_ggi_KeyHandler(); }

void ddio_ggi_KeyHandler(void) {
  ubyte state;
  gii_event event;
  gii_event_mask event_mask;
  int key_sym;
  int i, keycode, event_key, key_state;
  unsigned char temp;
  int ggi_key_repeat = 0; // should disable when playing, enable otherwise?
  struct timeval tv;
  int c;

  event_key = -1;

  event_mask = (gii_event_mask)(emKeyPress | emKeyRelease);
  if (ggi_key_repeat)
    event_mask = (gii_event_mask)event_mask | ((gii_event_mask)emKeyRepeat);

  tv.tv_sec = 0;
  tv.tv_usec = 0;

  while (giiEventPoll(GGI_input_handle, event_mask, &tv)) {
    giiEventRead(GGI_input_handle, &event, event_mask);
    switch (event.any.type) {
    case evKeyRepeat:
    case evKeyPress:
      keycode = event.key.button;
      c = event.key.sym;
      mprintf((0, "ggi key press %d (sym=%d)\n", keycode, c));
      break;
    case evKeyRelease:
      keycode = event.key.button;
      c = event.key.sym;
      mprintf((0, "ggi key release %d (sym=%d)\n", keycode, c));
      break;
    }

    if ((event.any.type == evKeyPress) || (event.any.type == evKeyRelease)) {
      event_key = event.key.button;
      key_sym = event.key.sym;
      event_key = ddio_ggi_TranslateKey(c, keycode);

      if (event.any.type == evKeyPress)
        key_state = 1;
      else
        key_state = 0;

      ddio_ggi_KeyBoardEventHandler(event_key, key_state);
    }

    /*
                    for (i = 127; i >= 0; i--) {
                            keycode = i;

                            key = &(key_data.keys[keycode]);

                            if (i == event_key)
                                    state = key_state;
                            else
                                    state = key->last_state;

                            if ( key->last_state == state )	{
                                    if (state) {
                                            key->counter++;
                                            keyd_last_pressed = keycode;
                                            keyd_time_when_last_pressed = timer_get_fixed_seconds();
                                    }
                            } else {
                                    if (state)	{
                                            keyd_last_pressed = keycode;
                                            keyd_pressed[keycode] = 1;
                                            key->downcount += state;
                                            key->state = 1;
                                            key->timewentdown = keyd_time_when_last_pressed = timer_get_fixed_seconds();
                                            key->counter++;
                                    } else {
                                            keyd_pressed[keycode] = 0;
                                            keyd_last_released = keycode;
                                            key->upcount += key->state;
                                            key->state = 0;
                                            key->counter = 0;
                                            key->timehelddown += timer_get_fixed_seconds() - key->timewentdown;
                                    }
                            }
                            if ( (state && !key->last_state) || (state && key->last_state && (key->counter > 30) &&
       (key->counter & 0x01)) ) { if ( keyd_pressed[KEY_LSHIFT] || keyd_pressed[KEY_RSHIFT]) keycode |= KEY_SHIFTED; if
       ( keyd_pressed[KEY_LALT] || keyd_pressed[KEY_RALT]) keycode |= KEY_ALTED; if ( keyd_pressed[KEY_LCTRL] ||
       keyd_pressed[KEY_RCTRL]) keycode |= KEY_CTRLED; if ( keyd_pressed[KEY_CMD] ) keycode |= KEY_COMMAND; temp =
       key_data.keytail+1; if ( temp >= KEY_BUFFER_SIZE ) temp=0; if (temp!=key_data.keyhead)	{
                                            key_data.keybuffer[key_data.keytail] = keycode;
                                            key_data.time_pressed[key_data.keytail] = keyd_time_when_last_pressed;
                                            key_data.keytail = temp;
                                    }
                            }
                            key->last_state = state;
                    }*/
  } // ggi event reading loop
}

int TranslateGGIToKey[96] = {
    // !"#$%&'()*+,-./
    KEY_SPACEBAR, KEY_1, KEY_RAPOSTRO, KEY_3, KEY_4, KEY_5, KEY_7, KEY_RAPOSTRO, KEY_9, KEY_0, KEY_8, KEY_EQUAL,
    KEY_COMMA, KEY_MINUS, KEY_PERIOD, KEY_SLASH,

    // 0123456789:;<=>?
    KEY_0, KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9, KEY_SEMICOL, KEY_SEMICOL, KEY_COMMA,
    KEY_EQUAL, KEY_PERIOD, KEY_SLASH,

    //@ABCDEFGHIJKLMNO
    KEY_2, KEY_A, KEY_B, KEY_C, KEY_D, KEY_E, KEY_F, KEY_G, KEY_H, KEY_I, KEY_J, KEY_K, KEY_L, KEY_M, KEY_N, KEY_O,

    // PQRSTUVWXYZ[\]^_
    KEY_P, KEY_Q, KEY_R, KEY_S, KEY_T, KEY_U, KEY_V, KEY_W, KEY_X, KEY_Y, KEY_Z, KEY_LBRACKET, KEY_SLASH, KEY_RBRACKET,
    KEY_6, KEY_MINUS,

    //`abcdefghijklmno
    KEY_LAPOSTRO, KEY_A, KEY_B, KEY_C, KEY_D, KEY_E, KEY_F, KEY_G, KEY_H, KEY_I, KEY_J, KEY_K, KEY_L, KEY_M, KEY_N,
    KEY_O,

    // pqrstuvwxyz{|}~
    KEY_P, KEY_Q, KEY_R, KEY_S, KEY_T, KEY_U, KEY_V, KEY_W, KEY_X, KEY_Y, KEY_Z, KEY_LBRACKET, KEY_SLASH, KEY_RBRACKET,
    KEY_LAPOSTRO, KEY_MINUS /* KEY_MINUS just for completeness */
};

static int ddio_ggi_TranslateKey(int c, int code) {
  int result;

  switch (KTYP(GII_KVAL(c))) {
  case KT_LATIN:
  case KT_LETTER:
  case KT_META:
    result = KVAL(GII_KVAL(c));
    if (result < 32) {
      if (result == 9)
        result = KEY_TAB;
      else if (result == 8)
        result = KEY_BACKSP;
      else if (result == 27)
        result = KEY_ESC;
      else if (result == 28)
        result = KEY_PRINT_SCREEN;
      else {
        result += 'A' - 1;
        mprintf((0, "DDIO: Unhandled Latin Symbol <32 (%d)\n", result));
      }
    } else if (result == 127)
      result = KEY_DELETE;
    else {
      result = tolower(result);
      result = TranslateGGIToKey[result - 32];
    }
    mprintf((0, "DDIO: Decoded Key = %d\n", result));
    return result;
  default:
    switch (GII_KVAL(c)) {
    case K_NUM:
      return KEY_NUMLOCK;

    case K_P0:
      return KEY_PAD0;
    case K_P1:
      return KEY_PAD1;
    case K_P2:
      return KEY_PAD2;
    case K_P3:
      return KEY_PAD3;
    case K_P4:
      return KEY_PAD4;
    case K_P5:
      return KEY_PAD5;
    case K_P6:
      return KEY_PAD6;
    case K_P7:
      return KEY_PAD7;
    case K_P8:
      return KEY_PAD8;
    case K_P9:
      return KEY_PAD9;

    case K_PPLUS:
      return KEY_PADPLUS;
    case K_PMINUS:
      return KEY_PADMINUS;
    case K_PSTAR:
      return KEY_PADMULTIPLY;
    case K_PSLASH:
      return KEY_PADDIVIDE;
    case K_PENTER:
      return KEY_PADENTER;
      //	case K_PCOMMA: return KEY_PAD;
    case K_PDOT:
      return KEY_PADPERIOD;
      //	case K_PPLUSMINUS: return KEY_PAD;
      //	case K_PPARENL: return KEY_PAD;
      //	case K_PPARENR: return KEY_PAD;
      //	case K_P: return KEY_PAD;
      //	case K_HOME: return KEY_HOME;
    case K_FIND:
      return KEY_HOME;
      //	case K_END: return KEY_END;
    case K_SELECT:
      return KEY_END;

    case K_UP:
      return KEY_UP;
    case K_DOWN:
      return KEY_DOWN;
    case K_LEFT:
      return KEY_LEFT;
    case K_RIGHT:
      return KEY_RIGHT;

      // Should choose the next two depending on keycode & keysym:
      //@@	case K_NORMAL_SHIFT:
      //@@	    if (code==54) return KEY_RSHIFT;
      //@@	    return KEY_LSHIFT;
      //@@	case K_NORMAL_CTRL:
      //@@	    if (code==97) return KEY_RCTRL;
      //@@	    return KEY_LCTRL;
      //@@	case K_NORMAL_ALT: return KEY_LALT;
      //@@	case K_NORMAL_ALTGR: return KEY_RALT;
      //@@	case K_NORMAL_SHIFTL: return KEY_LSHIFT;
      //@@	case K_NORMAL_SHIFTR: return KEY_RSHIFT;
      //@@	case K_NORMAL_CTRLL: return KEY_LCTRL;
      //@@	case K_NORMAL_CTRLR: return KEY_LCTRL;

    case K_CAPS:
      return KEY_CAPSLOCK;

    case K_ENTER:
      return KEY_ENTER;

    case K_PGUP:
      return KEY_PAGEUP;
    case K_PGDN:
      return KEY_PAGEDOWN;

    case K_INSERT:
      return KEY_INSERT;
    case K_REMOVE:
      return KEY_DELETE;

    case K_PAUSE:
      return KEY_PAUSE;
    case K_HOLD:
      return KEY_SCROLLOCK;

      // K_TAB is not defined, but = latin 9.
      //	case K_TAB: return XK_Tab;
    case K_F1:
      return KEY_F1;
    case K_F2:
      return KEY_F2;
    case K_F3:
      return KEY_F3;
    case K_F4:
      return KEY_F4;
    case K_F5:
      return KEY_F5;
    case K_F6:
      return KEY_F6;
    case K_F7:
      return KEY_F7;
    case K_F8:
      return KEY_F8;
    case K_F9:
      return KEY_F9;
    case K_F10:
      return KEY_F10;
    case K_F11:
      return KEY_F11;
    case K_F12:
      return KEY_F12;

    default:
      mprintf((0, "GGI undhandled key event, keycode=%d (keysym=%d)\n", code, c));
      return 0;
    }
  }
  return 0;
}
