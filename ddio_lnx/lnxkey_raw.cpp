/*
 * $Logfile: /DescentIII/Main/ddio_lnx/lnxkey_raw.cpp $
 * $Revision: 1.2 $
 * $Date: 2004/02/25 00:04:06 $
 * $Author: ryan $
 *
 * Linux stdin/stdout routines
 *
 * $Log: lnxkey_raw.cpp,v $
 * Revision 1.2  2004/02/25 00:04:06  ryan
 * Removed loki_utils dependency and ported to MacOS X (runs, but incomplete).
 *
 * Revision 1.1.1.1  2000/04/18 00:00:33  icculus
 * initial checkin
 *
 *
 * 7     7/19/99 12:53p Jeff
 * put in hacks for enter and backspace keys
 *
 * 6     7/14/99 9:06p Jeff
 * added comment header
 *
 * $NoKeywords: $
 */

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
//#include <linux/keyboard.h>
#include <sys/ioctl.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
//#include <linux/unistd.h>
#include <termios.h>
#include <term.h>

#include "ddio_common.h"
#include "ddio.h"
#include "mono.h"

//##########################################################
// raw Interface Functions
//##########################################################
void ddio_raw_EmergencyQuit(int id);
void ddio_raw_InternalKeyInit(void);
void ddio_raw_KeyBoardEventHandler(int scancode, int press);
static int ddio_raw_TransKey(int scancode);
void ddio_raw_DoKeyFrame(void);
void ddio_raw_InternalClose(void);
void init_keyboard();
void close_keyboard();
int kbhit();
int readch();
//##########################################################

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
bool DDIO_key_suspend = false;

bool ddio_normal_InternalKeyInit(ddio_init_info *init_info) {
  static bool first_call = true;
  // reset key list
  for (int i = 0; i < DDIO_MAX_KEYS; i++) {
    LKeys[i].down_ticks = 0;
    LKeys[i].up_ticks = 0;
    LKeys[i].status = false;
  }
  DDIO_key_suspend = false;

  ddio_raw_InternalKeyInit();

  if (first_call) {
    atexit(ddio_InternalKeyClose);
    first_call = false;
  }

  return true;
}

void ddio_normal_InternalKeyClose() { ddio_raw_InternalClose(); }

//	ddio_KeyFrame
//		handle input of keyboard per frame.
bool ddio_normal_KeyFrame(void) {
  ddio_raw_DoKeyFrame();
  return true;
}

void ddio_normal_InternalKeyFrame(void) {
  ddio_KeyFrame(); // which gets called?
}

bool ddio_normal_InternalKeyState(ubyte key) { return LKeys[key].status; }

void ddio_normal_InternalKeySuspend() { DDIO_key_suspend = true; }

void ddio_normal_InternalKeyResume() { DDIO_key_suspend = false; }

float ddio_normal_InternalKeyDownTime(ubyte key) {
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

void ddio_normal_InternalResetKey(ubyte key) {
  LKeys[key].down_time = 0.0f;
  LKeys[key].up_time = 0.0f;
  LKeys[key].status = false;
}

//#################################################
// raw Interface Functions
//#################################################
void (*kb_event_handler)(int scancode, int press);
void ddio_raw_InternalKeyInit(void) {
  static bool first_time = true;

  if (!first_time) {
    return;
  }
  first_time = false;

  struct sigaction sact;
  sact.sa_handler = ddio_raw_EmergencyQuit;
  sigfillset(&sact.sa_mask);
  sact.sa_flags = 0;
  sigaction(SIGINT, &sact, NULL);

  init_keyboard();
  kb_event_handler = ddio_raw_KeyBoardEventHandler;
}

void ddio_raw_InternalClose(void) {
  static bool first_time = true;

  if (first_time) {
    close_keyboard();
    first_time = false;
  }
}

void ddio_raw_EmergencyQuit(int id) {
  ddio_raw_InternalClose();
  // somehow we need to signal quit here
  exit(-1);
}

void ddio_raw_KeyBoardEventHandler(int scancode, int press) {
  int transkey;

  switch (scancode) {
  case 10: // enter key hack
    transkey = KEY_ENTER;
    break;
  case 127: // backspace key hack
    transkey = KEY_BACKSP;
    break;
  default:
    transkey = ddio_AsciiToKey(scancode);
    break;
  }

  float key_time = timer_GetTime();

  if (transkey & KEY_SHIFTED) {
    transkey &= ~KEY_SHIFTED;
    if (press == 1) {
      LKeys[KEY_LSHIFT].status = 1;
      LKeys[KEY_LSHIFT].down_time = key_time;
      ddio_UpdateKeyState(KEY_LSHIFT, true);
    }
    if (press == 0) {
      LKeys[KEY_LSHIFT].status = 0;
      LKeys[KEY_LSHIFT].up_time = key_time;
      ddio_UpdateKeyState(KEY_LSHIFT, false);
    }
  }
  if (transkey & KEY_ALTED) {
    transkey &= ~KEY_ALTED;
    if (press == 1) {
      LKeys[KEY_LALT].status = 1;
      LKeys[KEY_LALT].down_time = key_time;
      ddio_UpdateKeyState(KEY_LALT, true);
    }
    if (press == 0) {
      LKeys[KEY_LALT].status = 0;
      LKeys[KEY_LALT].up_time = key_time;
      ddio_UpdateKeyState(KEY_LALT, false);
    }
  }
  if (transkey & KEY_CTRLED) {
    transkey &= ~KEY_CTRLED;
    if (press == 1) {
      LKeys[KEY_LCTRL].status = 1;
      LKeys[KEY_LCTRL].down_time = key_time;
      ddio_UpdateKeyState(KEY_LCTRL, true);
    }
    if (press == 0) {
      LKeys[KEY_LCTRL].status = 0;
      LKeys[KEY_LCTRL].up_time = key_time;
      ddio_UpdateKeyState(KEY_LCTRL, false);
    }
  }

  if (press == 1) {
    LKeys[transkey].status = 1;
    LKeys[transkey].down_time = key_time;
    ddio_UpdateKeyState(transkey, true);
  }
  if (press == 0) {
    LKeys[transkey].status = 0;
    LKeys[transkey].up_time = key_time;
    ddio_UpdateKeyState(transkey, false);
  }
}

void ddio_raw_DoKeyFrame(void) {
  // make all down keys up
  for (int i = 0; i < DDIO_MAX_KEYS; i++) {
    if (LKeys[i].status) {
      LKeys[i].status = 0;
      LKeys[i].up_time = timer_GetTime();
      ddio_UpdateKeyState(i, false);
    }
  }
  char keyboard_buf;

  if (kbhit()) {
    keyboard_buf = readch();
  } else {
    return;
  }
  /*
          fd_set rdfd;
          int fd = fileno(stdin);

          FD_ZERO(&rdfd);
          FD_SET(fd,&rdfd);
          if(select(fd+1,&rdfd,NULL,NULL,NULL)<=0){
                  return;
          }

          mprintf((0,"Reading keys...\n"));
          char keyboard_buf[16];
          int n = read(fd,keyboard_buf,sizeof(keyboard_buf));
          if(n==0)
                  return;

          for(int i=0;i<n;i++)
          {
  */
  mprintf((0, "Got Key %d\n", keyboard_buf));
  (*kb_event_handler)(keyboard_buf, true);
  /*
          }
  */
}

static struct termios initial_settings, new_settings;
static int peek_character = -1;

void init_keyboard() {
  tcgetattr(0, &initial_settings);
  new_settings = initial_settings;
  new_settings.c_lflag &= ~ICANON;
  new_settings.c_lflag &= ~ECHO;
  new_settings.c_lflag &= ~ISIG;
  new_settings.c_cc[VMIN] = 1;
  new_settings.c_cc[VTIME] = 0;
  tcsetattr(0, TCSANOW, &new_settings);
}

void close_keyboard() {
  mprintf((0, "Shutting down RAW keyboard\n"));
  tcsetattr(0, TCSANOW, &initial_settings);
}

int kbhit() {
  char ch;
  int nread;

  if (peek_character != -1)
    return 1;
  new_settings.c_cc[VMIN] = 0;
  tcsetattr(0, TCSANOW, &new_settings);
  nread = read(0, &ch, 1);
  new_settings.c_cc[VMIN] = 1;
  tcsetattr(0, TCSANOW, &new_settings);

  if (nread == 1) {
    peek_character = ch;
    return 1;
  }

  return 0;
}

int readch() {
  char ch;
  if (peek_character != -1) {
    ch = peek_character;
    peek_character = -1;
    return ch;
  }
  read(0, &ch, 1);
  return ch;
}
