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
 * $Logfile: /DescentIII/Main/ddio_common/key.cpp $
 * $Revision: 27 $
 * $Date: 10/21/99 9:28p $
 * $Author: Jeff $
 *
 * Keyboard common interface
 *
 * $Log: /DescentIII/Main/ddio_common/key.cpp $
 *
 * 27    10/21/99 9:28p Jeff
 * B.A. Macintosh code merge
 *
 * 26    8/31/99 5:30p Jason
 * network statistics code
 *
 * 25    4/24/99 8:40p Samir
 * debug key for mono screens moved into key handler.
 *
 * 24    4/24/99 5:41p Samir
 * moved key to ascii, ascii to key to the ddio_common library.
 *
 * 23    4/22/99 2:00a Jeff
 * pass in ddio_init_info
 *
 * 22    2/21/99 6:38p Samir
 * mouse and key input better. buffered mouse.
 *
 * 21    2/05/99 1:16p Samir
 * reset low level keys when flushing keyboard in the high level.  added a
 * function to the low level to reset the status of a key, called from
 * high level key flush.
 *
 * 20    1/25/99 7:27p Samir
 * fixed timing issues with emulated keyboard key down times. (Win32 sends
 * multiple keydown messages, so  ignore them.)
 *
 * 19    1/25/99 6:47p Samir
 * allow slow keyboard
 *
 * 18    1/25/99 11:02a Samir
 * revamped mouse and key controls.
 *
 * 17    10/21/98 6:51p Samir
 * took out dynamic key updating.
 *
 * 16    10/21/98 12:02p Samir
 * properly update odd keys when they are released throug
 * ddio_KeyGetDownTime.
 *
 * 15    10/16/98 12:16p Samir
 * more accurate key timing broke ddio_KeyDownTime, so key down time is
 * rounded off 5 decimal places.
 *
 * 14    9/17/98 12:50p Samir
 * added ddio_KeyFlushKey.
 *
 * 13    7/01/98 4:56p Samir
 * changed some deinit stuff.
 *
 * 12    3/31/98 12:46p Samir
 * keyboard IO system better.  uses getmsgproc windows hook.
 *
 * 11    3/24/98 4:26p Samir
 * added function to return the state of an adjusted key.
 *
 * 10    2/25/98 6:11p Samir
 * Added functions to better deal with key flushing.
 *
 * 9     2/03/98 5:16p Matt
 * Changed key buffer to be ushorts so shift,ctrl,etc. bits don't get
 * lost.
 * Added support for KEY_DEBUGGED.
 *
 * 8     1/21/98 6:47p Samir
 * Added debugging info.
 *
 * 7     12/10/97 1:12p Samir
 * Pass time to ddio_UpdateKeyState
 *
 * 6     10/29/97 4:14p Samir
 * Keep record of key down count per key.
 *
 * 5     10/16/97 2:28p Samir
 * move keyboard init into ddio_init and added preemptive select for
 * keyboard.
 *
 * 4     8/29/97 11:54a Samir
 * Fixed some problem with keycodes that were greater than 0x80, being
 * sign extended upon return from KeyInKey.
 *
 * 3     8/13/97 5:41p Samir
 * Took out annoying mprintf of keycode.
 *
 * 2     7/25/97 6:31p Samir
 * Fixed some really stupid key queue buffer check bugs that could cause
 * instability.
 *
 * 3     5/09/97 6:45p Samir
 * Common keystate update function
 *
 * 2     5/08/97 1:54p Samir
 * Took out prototype to ddio_Close
 *
 * 1     5/08/97 12:22p Samir
 * Moved common keyboard code from ddio_win to ddio_common.
 *
 * $NoKeywords: $
 */

//	Handles keyboard queue management

#include "ddio.h"
#include "pserror.h"
#include "Macros.h"

#include <stdlib.h>

#define KEY_QUEUE_SIZE 16

// This is the key used as a debugging modifier
#ifdef MACINTOSH
#define KEY_DEBUG KEY_BACKSP
#else
#define KEY_DEBUG KEY_DELETE
#endif

// ----------------------------------------------------------------------------
//	Data for ddio_common keyboard code.
// ----------------------------------------------------------------------------

static bool DDIO_key_init = 0;
volatile ubyte DDIO_key_state[DDIO_MAX_KEYS];
volatile short DDIO_key_down_count[DDIO_MAX_KEYS];

static struct t_key_queue {
  ushort buffer[KEY_QUEUE_SIZE]; // Keyboard buffer queue
  int tail;
  int head;
} DDIO_key_queue;

// ----------------------------------------------------------------------------
//	Common initialization
// ----------------------------------------------------------------------------

bool ddio_KeyInit(ddio_init_info *init_info) {
  if (ddio_InternalKeyInit(init_info)) {
    DDIO_key_init = 1;
    ddio_KeyFlush();
    return 1;
  }

  return 0;
}

void ddio_KeyClose() {
  if (DDIO_key_init) {
    ddio_InternalKeyClose();
    DDIO_key_init = 0;
  }
}

// ----------------------------------------------------------------------------
//	Keyboard accessor functions
// ----------------------------------------------------------------------------

float ddio_KeyDownTime(int key) {
  ASSERT(DDIO_key_init);

  //	snap off shift states, etc.
  return ddio_InternalKeyDownTime((ubyte)(key & 0xff));
}

//	return number of times a key's been down since last call.
int ddio_KeyDownCount(int key) {
  int count = DDIO_key_down_count[key];
  DDIO_key_down_count[key] = 0;

  return count;
}

//	returns the state of an ADJUSTED KEY VALUE
bool ddio_GetAdjKeyState(int adj_key) {
  ubyte key_raw = adj_key & 0xff;
  int key_test = 0;

  if (adj_key & KEY_SHIFTED) {
    if (KEY_STATE(KEY_LSHIFT) || KEY_STATE(KEY_RSHIFT))
      key_test |= KEY_SHIFTED;
  }
  if (adj_key & KEY_ALTED) {
    if (KEY_STATE(KEY_LALT) || KEY_STATE(KEY_RALT))
      key_test |= KEY_ALTED;
  }
  if (adj_key & KEY_CTRLED) {
    if (KEY_STATE(KEY_LCTRL) || KEY_STATE(KEY_RCTRL))
      key_test |= KEY_CTRLED;
  }
  if (adj_key & KEY_DEBUGGED) {
    if (KEY_STATE(KEY_DEBUG))
      key_test |= KEY_DEBUGGED;
  }

  if (KEY_STATE(key_raw))
    key_test |= key_raw;

  if (key_test == adj_key)
    return true;
  else
    return false;
}

// ----------------------------------------------------------------------------
//	Keyboard Queue functions
// ----------------------------------------------------------------------------

void ddio_KeyFlush() {
  int i;

  if (!DDIO_key_init)
    return;

  //	flush out internal key input system by gathering all data in buffer.
  DDIO_key_queue.head = DDIO_key_queue.tail = 0;

  for (i = 0; i < KEY_QUEUE_SIZE; i++) {
    DDIO_key_queue.buffer[i] = 0;
  }

  //	flush keyboard array too
  for (i = 0; i < DDIO_MAX_KEYS; i++) {
    DDIO_key_down_count[i] = 0;
    DDIO_key_state[i] = 0;
    ddio_InternalResetKey(i);
  }
}

void ddio_KeyFlushKey(int key) {
  DDIO_key_down_count[key] = 0;
  DDIO_key_state[key] = 0;
}

int ddio_KeyInKey() {
  unsigned key = 0;

  if (DDIO_key_queue.head != DDIO_key_queue.tail) {
    key = (unsigned)DDIO_key_queue.buffer[DDIO_key_queue.head];
    //	mprintf((1, "%x ", key));
    DDIO_key_queue.head++;
    if (DDIO_key_queue.head >= KEY_QUEUE_SIZE)
      DDIO_key_queue.head = 0;
  }

  return (int)key;
}

int ddio_KeyPeek() {
  if (DDIO_key_queue.head != DDIO_key_queue.tail) {
    return (int)DDIO_key_queue.buffer[DDIO_key_queue.head];
  }
  return 0;
}

void ddio_AddKeyToQueue(int key) {
  int keycode = key;
  int temp;

  if (DDIO_key_state[KEY_LSHIFT] || DDIO_key_state[KEY_RSHIFT])
    keycode |= KEY_SHIFTED;
  if (DDIO_key_state[KEY_LCTRL] || DDIO_key_state[KEY_RCTRL])
    keycode |= KEY_CTRLED;
  if (DDIO_key_state[KEY_LALT] || DDIO_key_state[KEY_RALT])
    keycode |= KEY_ALTED;
  if (DDIO_key_state[KEY_CMD])
    keycode |= KEY_CMD; // DAJ

#ifdef _DEBUG
  if (DDIO_key_state[KEY_DEBUG])
    keycode |= KEY_DEBUGGED;
  if (keycode == (KEY_DEBUGGED + KEY_SHIFTED + KEY_M)) {
    static int current_virtual_window = 1;

    current_virtual_window++;
    if (current_virtual_window == 2) { // skip stats window
      current_virtual_window++;
    }
    if (current_virtual_window == 5) {
      current_virtual_window = 1;
    }
    Debug_ConsoleRedirectMessages(current_virtual_window, 1);
  }
#endif

  temp = DDIO_key_queue.tail + 1;
  if (temp >= KEY_QUEUE_SIZE)
    temp = 0;

  if (temp != DDIO_key_queue.head) {
    DDIO_key_queue.buffer[DDIO_key_queue.tail] = (ushort)keycode;
    DDIO_key_queue.tail = temp;
    //	mprintf((1, "%d ", keycode));
  }
}

//	Updates timing info for keys
void ddio_UpdateKeyState(int key, bool isdown) {
  if (isdown) {
    if (!DDIO_key_state[key]) {
      ddio_AddKeyToQueue(key);
      DDIO_key_down_count[key]++;
    }
    DDIO_key_state[key] = 1;
  } else {
    DDIO_key_state[key] = 0;
  }
}

////////////////////////////////////////////////////////////////////////////////

static unsigned char ascii_table[128] = {
    255, 255, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 255, 255, 'q', 'w', 'e', 'r', 't', 'y',
    'u', 'i', 'o', 'p', '[', ']', 255, 255, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', 39,  '`', 255, '\\',
    'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 255, '*', 255, ' ', 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, '7', '8', '9', '-', '4', '5', '6', '+', '1', '2', '3', '0', '.', 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255};

static unsigned char shifted_ascii_table[128] = {
    255, 255, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', 255, 255, 'Q', 'W', 'E', 'R', 'T', 'Y',
    'U', 'I', 'O', 'P', '{', '}', 255, 255, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~', 255, '|',
    'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 255, 255, 255, ' ', 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, '7', '8', '9', '-', '4', '5', '6', '+', '1', '2', '3', '0', '.', 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255};

#define SHK KEY_SHIFTED

// keycode value for each ascii.
static int keycode_table[128] = {
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    KEY_TAB,
    0,
    0,
    0,
    KEY_ENTER,
    0,
    0, // 0-15
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    KEY_ESC,
    0,
    0,
    0,
    0, // 16-31
    KEY_SPACEBAR,
    SHK + KEY_1,
    SHK + KEY_RAPOSTRO,
    SHK + KEY_3,
    SHK + KEY_4,
    SHK + KEY_5, // 32-37
    SHK + KEY_7,
    KEY_RAPOSTRO,
    SHK + KEY_9,
    SHK + KEY_0,
    SHK + KEY_8,
    SHK + KEY_EQUAL, // 38-43
    KEY_COMMA,
    KEY_MINUS,
    KEY_PERIOD,
    KEY_SLASH,
    KEY_0,
    KEY_1,
    KEY_2,
    KEY_3, // 44-51
    KEY_4,
    KEY_5,
    KEY_6,
    KEY_7,
    KEY_8,
    KEY_9,
    SHK + KEY_SEMICOL,
    KEY_SEMICOL, // 52-59
    SHK + KEY_COMMA,
    KEY_EQUAL,
    SHK + KEY_PERIOD,
    SHK + KEY_SLASH,
    SHK + KEY_2, // 60-64
    KEY_A,
    KEY_B,
    KEY_C,
    KEY_D,
    KEY_E,
    KEY_F,
    KEY_G,
    KEY_H,
    KEY_I,
    KEY_J,
    KEY_K, // 65-75
    KEY_L,
    KEY_M,
    KEY_N,
    KEY_O,
    KEY_P,
    KEY_Q,
    KEY_R,
    KEY_S,
    KEY_T,
    KEY_U,
    KEY_V, // 76-86
    KEY_W,
    KEY_X,
    KEY_Y,
    KEY_Z,
    KEY_LBRACKET,
    KEY_BACKSLASH,
    KEY_RBRACKET, // 87-93
    SHK + KEY_6,
    SHK + KEY_MINUS,
    KEY_LAPOSTRO, // 94-96
    KEY_A,
    KEY_B,
    KEY_C,
    KEY_D,
    KEY_E,
    KEY_F,
    KEY_G,
    KEY_H,
    KEY_I,
    KEY_J,
    KEY_K, // 97-107
    KEY_L,
    KEY_M,
    KEY_N,
    KEY_O,
    KEY_P,
    KEY_Q,
    KEY_R,
    KEY_S,
    KEY_T,
    KEY_U,
    KEY_V, // 108-118
    KEY_W,
    KEY_X,
    KEY_Y,
    KEY_Z,
    SHK + KEY_LBRACKET,
    SHK + KEY_BACKSLASH, // 119-124
    SHK + KEY_RBRACKET,
    SHK + KEY_LAPOSTRO,
    0, // 125-127
};

//	converts keycode to ASCII
int ddio_KeyToAscii(int code) {
  int shifted;

  shifted = code & KEY_SHIFTED;
  code &= 0xFF;

  if (code >= 127)
    return 255;

  if (shifted)
    return (int)shifted_ascii_table[code];
  else
    return (int)ascii_table[code];
}

//	converts ascii code to key
int ddio_AsciiToKey(int ascii) {
  if (ascii < 0 || ascii >= 128)
    return 0;

  if (ascii >= 'A' && ascii <= 'Z') {
    // I'm lazy
    return (SHK + keycode_table[ascii]);
  } else {
    return keycode_table[ascii];
  }
}
