/*
 * $Logfile: /DescentIII/Main/ddio_lnx/lnxkey_null.cpp $
 * $Revision: 1.2 $
 * $Date: 2004/02/25 00:04:06 $
 * $Author: ryan $
 *
 * Linux NULL keyboard routines
 *
 * $Log: lnxkey_null.cpp,v $
 * Revision 1.2  2004/02/25 00:04:06  ryan
 * Removed loki_utils dependency and ported to MacOS X (runs, but incomplete).
 *
 * Revision 1.1.1.1  2000/04/18 00:00:33  icculus
 * initial checkin
 *
 *
 * 2     7/14/99 9:06p Jeff
 * added comment header
 *
 * $NoKeywords: $
 */

#include "pserror.h"
#include "mono.h"
#include "ddio.h"
#include "ddio_lnx.h"
#include "application.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <signal.h>
#include <fcntl.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
//#include <linux/unistd.h>
//#include <linux/keyboard.h>
//#include <linux/kd.h>
//#include <linux/vt.h>

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

bool ddio_null_InternalKeyInit(ddio_init_info *init_info) {
  // reset key list
  for (int i = 0; i < DDIO_MAX_KEYS; i++) {
    LKeys[i].down_ticks = 0;
    LKeys[i].up_ticks = 0;
    LKeys[i].status = false;
  }

  return true;
}

void ddio_null_InternalKeyClose() {}

bool ddio_null_InternalKeyState(ubyte key) { return LKeys[key].status; }

void ddio_null_InternalKeySuspend() {}

void ddio_null_InternalKeyResume() {}

float ddio_null_InternalKeyDownTime(ubyte key) {
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

void ddio_null_InternalResetKey(ubyte key) {
  LKeys[key].down_time = 0.0f;
  LKeys[key].up_time = 0.0f;
  LKeys[key].status = false;
}

bool ddio_null_KeyFrame() { return true; }

void ddio_null_InternalKeyFrame(void) {}
