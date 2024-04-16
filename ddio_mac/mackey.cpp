/*
 * $Logfile: /DescentIII/Main/ddio_mac/mackey.cpp $
 * $Revision: 1.1.1.1 $
 * $Date: 2003/08/26 03:56:55 $
 * $Author: kevinb $
 *
 * Keyboard IO for macintosh
 *
 * $Log: mackey.cpp,v $
 * Revision 1.1.1.1  2003/08/26 03:56:55  kevinb
 * initial 1.5 import
 *
 *
 * 4     3/20/00 12:43p Matt
 * Merge of Duane's post-1.3 changes.
 *
 * 3     10/21/99 3:33p Jeff
 * Macintosh merges
 *
 * 2     7/28/99 3:31p Kevin
 * Mac Stuff!
 *
 * 8     5/21/97 3:40 PM Jeremy
 * fixed a bug where the mac/pc keycodes would get sign extended when
 * converted to an int because they were declared as char instead of
 * unsigned char
 *
 * 7     5/20/97 11:37 PM Jeremy
 * fixed the no-keyup reporting bug in the keyboard handler
 *
 * 6     5/20/97 4:59 PM Jeremy
 * added some calls to set and reset the event mask to handle key up
 * events
 *
 * 5     5/19/97 9:42 AM Jeremy
 * in keyboard handler changed call to getnextevent to getosevent which
 * does not yield time to other processses, should make this routine more
 * interrupt safe if it is called via interrupt instead of polled by
 * osObject::defer
 *
 * 4     5/15/97 2:31 PM Jeremy
 * made macKeyboardHandler return true if more keys are waiting.  This way
 * it can be repeatedly called to get all pending keys or just called once
 * to get one key.  Also set the application event mask to allow key up
 * events (for timing reasons)
 *
 * 3     5/11/97 7:58 PM Jeremy
 * added stubs for internal init/close functions
 *
 * 2     5/9/97 7:06 PM Jeremy
 * implemented mac keyboard handler
 *
 * 1     3/13/97 6:53 PM Jeremy
 * macintosh implementation of device dependent io functions
 *
 * $NoKeywords: $
 */
// ----------------------------------------------------------------------------
//	Keyboard Interface
// ----------------------------------------------------------------------------
// ANSI Headers
#include <stdlib.h>
// Macintosh Headers
#include <Events.h>
#include <LowMem.h>
#include <Timer.h>
#include <Retrace.h>
// Descent 3 Headers
#include "pserror.h"
#include "mono.h"
#include "ddio.h"
#include "ddio_common.h"
#include "ddio_mac.h"
// ----------------------------------------------------------------------------
//	File-Level global data
// ----------------------------------------------------------------------------
// Translates from macintosh to pc scancodes, value at table[macCode] = pcCode;
//	mac keycodes are in commments: /* 0xHH */
//	0xFF means this keycode is unused on the macintosh and does not translate
static unsigned char mac_to_pc_scancode_table[128] = {
    0x1E /* 0x00 */, 0x1F /* 0x01 */, 0x20 /* 0x02 */, 0x21 /* 0x03 */,
    0x23 /* 0x04 */, 0x22 /* 0x05 */, 0x2C /* 0x06 */, 0x2D /* 0x07 */,
    0x2E /* 0x08 */, 0x2F /* 0x09 */, 0xFF /* 0x0A */, 0x30 /* 0x0B */,
    0x10 /* 0x0C */, 0x11 /* 0x0D */, 0x12 /* 0x0E */, 0x13 /* 0x0F */,
    0x15 /* 0x10 */, 0x14 /* 0x11 */, 0x02 /* 0x12 */, 0x03 /* 0x13 */,
    0x04 /* 0x14 */, 0x05 /* 0x15 */, 0x07 /* 0x16 */, 0x06 /* 0x17 */,
    0x0D /* 0x18 */, 0x0A /* 0x19 */, 0x08 /* 0x1A */, 0x0C /* 0x1B */,
    0x09 /* 0x1C */, 0x0B /* 0x1D */, 0x1B /* 0x1E */, 0x18 /* 0x1F */,
    0x16 /* 0x20 */, 0x1A /* 0x21 */, 0x17 /* 0x22 */, 0x19 /* 0x23 */,
    0x1C /* 0x24 */, 0x26 /* 0x25 */, 0x24 /* 0x26 */, 0x28 /* 0x27 */,
    0x25 /* 0x28 */, 0x27 /* 0x29 */, 0x2B /* 0x2A */, 0x33 /* 0x2B */,
    0x35 /* 0x2C */, 0x31 /* 0x2D */, 0x32 /* 0x2E */, 0x34 /* 0x2F */,
    0x0F /* 0x30 */, 0x39 /* 0x31 */, 0x29 /* 0x32 */, 0x0E /* 0x33 */,
    0xFF /* 0x34 */, 0x01 /* 0x35 */, 0xFF /* 0x36 */, 0xE0 /* 0x37 */,
    0x2A /* 0x38 */, 0x3A /* 0x39 */, 0x38 /* 0x3A */, 0x1D /* 0x3B */,
    0x36 /* 0x3C */, 0xB8 /* 0x3D */, 0x9D /* 0x3E */, 0xFF /* 0x3F */,
    0xFF /* 0x40 */, 0x53 /* 0x41 */, 0xFF /* 0x42 */, 0x37 /* 0x43 */,
    0xFF /* 0x44 */, 0x4E /* 0x45 */, 0xFF /* 0x46 */, 0x45 /* 0x47 */,
    0xFF /* 0x48 */, 0xFF /* 0x49 */, 0xFF /* 0x4A */, 0xB5 /* 0x4B */,
    0x9C /* 0x4C */, 0xFF /* 0x4D */, 0x4A /* 0x4E */, 0xFF /* 0x4F */,
    0xFF /* 0x50 */, 0xFF /* 0x51 */, 0x52 /* 0x52 */, 0x4F /* 0x53 */,
    0x50 /* 0x54 */, 0x51 /* 0x55 */, 0x4B /* 0x56 */, 0x4C /* 0x57 */,
    0x4D /* 0x58 */, 0x47 /* 0x59 */, 0xFF /* 0x5A */, 0x48 /* 0x5B */,
    0x49 /* 0x5C */, 0xFF /* 0x5D */, 0xFF /* 0x5E */, 0xFF /* 0x5F */,
    0x3F /* 0x60 */, 0x40 /* 0x61 */, 0x41 /* 0x62 */, 0x3D /* 0x63 */,
    0x42 /* 0x64 */, 0x43 /* 0x65 */, 0xFF /* 0x66 */, 0x57 /* 0x67 */,
    0xFF /* 0x68 */, 0xB7 /* 0x69 */, 0xFF /* 0x6A */, 0x46 /* 0x6B */,
    0xFF /* 0x6C */, 0x44 /* 0x6D */, 0xFF /* 0x6E */, 0x58 /* 0x6F */,
    0xFF /* 0x70 */, 0x61 /* 0x71 */, 0xD2 /* 0x72 */, 0xC7 /* 0x73 */,
    0xC9 /* 0x74 */, 0xD3 /* 0x75 */, 0x3E /* 0x76 */, 0xCF /* 0x77 */,
    0x3C /* 0x78 */, 0xD1 /* 0x79 */, 0x3B /* 0x7A */, 0xCB /* 0x7B */,
    0xCD /* 0x7C */, 0xD0 /* 0x7D */, 0xC8 /* 0x7E */, 0xFF /* 0x7F */
};
const unsigned char kUnusedScanCode = 0xFF;
struct MacKeys {
  union {
    int up_ticks;
    float up_time;
  };
  union {
    int down_ticks;
    float down_time;
  };
  //	bool status;
} MKeys[DDIO_MAX_KEYS];
// ----------------------------------------------------------------------------
//	Local Prototypes
// ----------------------------------------------------------------------------
// translates scan code to foreign equivs.
ubyte xlate_scancode(ubyte scan_code);

// ----------------------------------------------------------------------------
//	Macintosh Keyboard Handler
// ----------------------------------------------------------------------------
bool MacKeyboardHandler(void) {
  unsigned char macKeyCode = 255;
  unsigned char pcScanCode = 255;
  bool b_shift = false;
  bool b_option = false;
  bool b_control = false;
  bool b_command = false;

  EventRecord event;

  // Allow us to get key up events
  SetEventMask(everyEvent);
  LMSetSysEvtMask(everyEvent);

  float now_time = timer_GetTime();
  // Get the event
  while (::GetOSEvent(keyDownMask + keyUpMask + autoKeyMask, &event)) {
    //	if(::GetOSEvent(keyDownMask + keyUpMask + autoKeyMask, &event)) {
    macKeyCode = (event.message & keyCodeMask) >> 8;
    ASSERT(macKeyCode < 128);
    pcScanCode = xlate_scancode(mac_to_pc_scancode_table[macKeyCode]);
    if (pcScanCode != kUnusedScanCode) {
      if (event.what == keyUp) {
        MKeys[pcScanCode].up_time = now_time;
      } else {
        if (MKeys[pcScanCode].down_time == 0.0) {
          MKeys[pcScanCode].down_time = now_time;
        }
      }
      ddio_UpdateKeyState(pcScanCode, (event.what == keyDown) || (event.what == autoKey));
    }
  }

  b_shift = (event.modifiers & shiftKey) >> 9;
  ddio_UpdateKeyState(KEY_LSHIFT, b_shift);
  ddio_UpdateKeyState(KEY_RSHIFT, b_shift);
  if (b_shift && MKeys[KEY_LSHIFT].down_time == 0.0)
    MKeys[KEY_LSHIFT].down_time = now_time;
  else if (MKeys[KEY_LSHIFT].down_time != 0.0)
    MKeys[KEY_LSHIFT].up_time = now_time;

  b_option = (event.modifiers & optionKey) >> 11;
  ddio_UpdateKeyState(KEY_RALT, b_option);
  ddio_UpdateKeyState(KEY_LALT, b_option);
  if (b_option && MKeys[KEY_RALT].down_time == 0.0)
    MKeys[KEY_RALT].down_time = now_time;
  else if (MKeys[KEY_RALT].down_time != 0.0)
    MKeys[KEY_RALT].up_time = now_time;

  b_control = (event.modifiers & controlKey) >> 12;
  ddio_UpdateKeyState(KEY_LCTRL, b_control);
  if (b_control && MKeys[KEY_LCTRL].down_time == 0.0)
    MKeys[KEY_LCTRL].down_time = now_time;
  else if (MKeys[KEY_LCTRL].down_time != 0.0)
    MKeys[KEY_LCTRL].up_time = now_time;

  b_command = (event.modifiers & cmdKey) >> 8;
  ddio_UpdateKeyState(KEY_CMD, b_command);
  if (b_command && MKeys[KEY_CMD].down_time == 0.0)
    MKeys[KEY_CMD].down_time = now_time;
  else if (MKeys[KEY_CMD].down_time != 0.0)
    MKeys[KEY_CMD].up_time = now_time;

  return false;
}
// ----------------------------------------------------------------------------
//	Keyboard Interface
// ----------------------------------------------------------------------------
bool ddio_InternalKeyInit(ddio_init_info *init_info) {
  bool result = true;
  // ¥ Make is so we can get key ups
  SetEventMask(everyEvent);
  LMSetSysEvtMask(everyEvent);

  SetEventMask(everyEvent);
  LMSetSysEvtMask(everyEvent);
  FlushEvents(everyEvent, 0);
  return result;
}
void ddio_InternalKeyClose() {
  // ¥ Restore the normal event mask
  SetEventMask(everyEvent - keyUpMask);
  LMSetSysEvtMask(everyEvent - keyUpMask);
}
bool ddio_InternalKeyState(ubyte key) { return false; }
void ddio_InternalKeySuspend() {
  _INTERRUPT_DISABLE();
  FlushEvents(everyEvent, 0);
}
void ddio_InternalKeyResume() {
  FlushEvents(everyEvent, 0);
  _INTERRUPT_ENABLE();
}
float ddio_InternalKeyDownTime(ubyte key) {
  float down_time = 0.0f;
  if (DDIO_key_state[key]) {
    float cur_time = timer_GetTime();
    if (MKeys[key].down_time != 0.0) {
      down_time = cur_time - MKeys[key].down_time;
    }
  } else {
    if (MKeys[key].up_time != 0.0 && MKeys[key].down_time != 0.0) {
      down_time = MKeys[key].up_time - MKeys[key].down_time;
      MKeys[key].down_time = MKeys[key].up_time = 0.0f;
    }
  }
  return down_time;
}
void ddio_InternalResetKey(ubyte key) { MKeys[key].down_time = MKeys[key].up_time = 0.0f; }
bool ddio_KeyFrame() { return true; }
void ddio_InternalKeyFrame(void) {}

static int DDIO_key_language = KBLANG_AMERICAN;

void ddio_SetKeyboardLanguage(int language) { DDIO_key_language = language; }

// translates scan code to foreign equivs.
ubyte xlate_scancode(ubyte scan_code) {
  ubyte code = scan_code;

  if (DDIO_key_language == KBLANG_FRENCH) {
    switch (scan_code) {
    case KEY_A:
      code = KEY_Q;
      break;
    case KEY_M:
      code = KEY_COMMA;
      break;
    case KEY_Q:
      code = KEY_A;
      break;
    case KEY_W:
      code = KEY_Z;
      break;
    case KEY_Z:
      code = KEY_W;
      break;
    case KEY_SEMICOL:
      code = KEY_M;
      break;
    case KEY_COMMA:
      code = KEY_SEMICOL;
      break;
    }
  } else if (DDIO_key_language == KBLANG_GERMAN) {
    switch (scan_code) {
    case KEY_Y:
      code = KEY_Z;
      break;
    case KEY_Z:
      code = KEY_Y;
      break;
    }
  } else if (DDIO_key_language == KBLANG_BRITISH) {
    if (scan_code == KEY_BSLASH_UK) { // KEY_SLASH_UK == 0x56
      code = KEY_SLASH;               // KEY_SLASH is really the backslash, 0x2B
    }
  }

  return code;
}
