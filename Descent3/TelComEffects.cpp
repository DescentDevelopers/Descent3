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

 * $Logfile: /DescentIII/Main/TelComEffects.cpp $
 * $Revision: 33 $
 * $Date: 5/13/99 3:42p $
 * $Author: Ardussi $
 *
 * TelCom Effect class defines
 *
 * $Log: /DescentIII/Main/TelComEffects.cpp $
 *
 * 33    5/13/99 3:42p Ardussi
 * changes for compiling on the Mac
 *
 * 32    4/14/99 3:57a Jeff
 * fixed case mismatch in #includes
 *
 * 31    2/17/99 6:55p Jeff
 * added jump button type.  Added no early render flag for bitmaps.  Fixed
 * color bug for type text
 *
 * 30    2/16/99 11:54a Jeff
 * fixed bug with 0 length strings
 *
 * 29    2/05/99 7:04p Jeff
 * table file parsing macros put in
 *
 * 28    2/03/99 1:23p Jeff
 * more updates to single player ship selection...all thats left is stats
 *
 * 27    2/03/99 12:14a Jeff
 * updated single player ship selection to full functional...needs effects
 * added in.  Had to add flag to buttons for gain focus to register as a
 * click
 *
 * 26    2/02/99 7:32p Jeff
 * begining of single player ship selection
 *
 * 25    11/01/98 1:57a Jeff
 * converted the vsprintf calls to use the Pvsprintf, which is a safe
 * vsprintf, no buffer overflows allowed
 *
 * 24    10/12/98 11:39p Jeff
 * finished up new focus system of telcom
 *
 * 23    10/12/98 8:32p Jeff
 * changed the way focus is handled
 *
 * 22    10/08/98 4:23p Kevin
 * Changed code to comply with memory library usage. Always use mem_malloc
 * , mem_free and mem_strdup
 *
 * 21    9/17/98 2:29p Jeff
 * added focus filenames to button effect
 *
 * 20    8/27/98 2:51p Jeff
 * New TelCom finally checked in
 *
 * 19    8/19/98 1:38p Jeff
 * removed update2dsound
 *
 * 18    6/15/98 4:00p Jason
 * replaced monochromatic polymodel lighting with rgb lighting
 *
 * 17    5/19/98 3:40p Jeff
 * poly models functional
 *
 * 16    5/15/98 5:16p Jeff
 * use regular draw chunk bitmap
 *
 * 15    5/11/98 6:21p Jeff
 * fixed setting glitch offsets
 *
 * 14    5/07/98 3:31p Jeff
 * various optimizations, removed Copy and similar functions
 *
 * 13    5/05/98 6:50p Jeff
 * Telcom doesn't use rend_DrawLFBitmap anymore...more speed!
 *
 * 12    5/04/98 6:22p Jeff
 * fixed possible sound bugs
 *
 * 11    5/04/98 5:29p Jeff
 * Added sounds to TelCom events
 *
 * 10    5/04/98 1:35p Jeff
 * Changes made for mouse handling
 *
 * 9     5/04/98 11:01a Jeff
 * added scroll parameter to text
 *
 * 8     5/03/98 7:57p Jeff
 * Got scrolling textboxes working
 *
 * 7     5/01/98 2:17p Jeff
 * Added sound effect support
 *
 * 6     4/30/98 7:18p Jeff
 * fixed poly model rotation
 *
 * 5     4/29/98 4:36p Jeff
 * fixed text going beyond bottom of text box
 *
 * 4     4/28/98 6:58p Jeff
 * Added new poly model effect driver
 *
 * 3     4/26/98 7:20p Jeff
 * Added the rest of the effect drivers except for scrolling text.  Added
 * invert bitmaps and stretch bitmaps
 *
 * 2     4/26/98 2:54a Jeff
 * Initial Creation
 *
 * 1     4/25/98 4:23p Jeff
 *
 * $NoKeywords: $
 */

#include <cstdarg>
#include <cstdio>

#include "TelComEffects.h"
#include "mem.h"
#include "vecmat.h"
#include <string.h>
#include "textaux.h"
#include "hlsoundlib.h"
#include "soundload.h"

tceffect TCEffects[MAX_TCEFFECTS];
int Screen_roots[MAX_TELCOM_SCREENS];
static bool Screen_has_effect_with_focus[MAX_TELCOM_SCREENS];

// Initializes the effects
void EfxInit(void) {
  int i;

  for (i = 0; i < MAX_TCEFFECTS; i++) {
    tceffect *tce = &TCEffects[i];
    tce->type = EFX_NONE;
    tce->monitor = -1;
    tce->screen = -1;
    tce->id = INVALID_EFFECT_ID;
    tce->prev = -1;
    tce->next = -1;
    tce->text_buffer = NULL;
    tce->tab_stop = false;
    tce->has_focus = false;
    for (int j = 0; j < MAX_EFFECT_EVENTS; j++)
      tce->event_queue[j].id = -1;
  }

  for (i = 0; i < MAX_TELCOM_SCREENS; i++) {
    Screen_has_effect_with_focus[i] = false;
    Screen_roots[i] = -1;
  }
}

// Shutdown of effects
void EfxClose(void) {
  for (int i = 0; i < MAX_TCEFFECTS; i++) {
    if (TCEffects[i].type != EFX_NONE) {
      tceffect *tce = &TCEffects[i];
      EfxFreeEffect(tce);
    }
  }
}

// Destroys all the screens
void EfxDestroyAllScreens(void) {
  for (int i = 0; i < MAX_TCEFFECTS; i++) {
    if (TCEffects[i].type != EFX_NONE) {
      tceffect *tce = &TCEffects[i];
      EfxFreeEffect(tce);
    }
  }
  EfxInit();
}

// Destroys the screens within a monitor
void EfxDestroyScreen(int screen) {
  ASSERT(screen >= 0 && screen < MAX_TELCOM_SCREENS);
  int node = Screen_roots[screen];
  tceffect *tce;
  while (node != -1) {
    tce = &TCEffects[node];
    node = TCEffects[node].next;

    EfxFreeEffect(tce);
    tce->type = EFX_NONE;
    tce->monitor = -1;
    tce->screen = -1;
    tce->prev = tce->next = -1;
    tce->id = INVALID_EFFECT_ID;
    tce->text_buffer = NULL;
    for (int j = 0; j < MAX_EFFECT_EVENTS; j++)
      tce->event_queue[j].id = -1;
  }
  Screen_roots[screen] = -1;
  Screen_has_effect_with_focus[screen] = false;
}

// Creates an instance of an Effect
int EfxCreate(int type, int monitor, int screen, int id, bool is_tab_stop) {
  ASSERT(type != EFX_NONE);
  ASSERT(monitor >= 0 && monitor < MAX_MONITOR);
  ASSERT(screen >= 0 && screen < MAX_TELCOM_SCREENS);

  bool make_focus = false;

  // find an available index
  for (int i = 0; i < MAX_TCEFFECTS; i++) {
    if (TCEffects[i].type == EFX_NONE) {
      // we found a slot
      TCEffects[i].type = type;
      TCEffects[i].monitor = monitor;
      TCEffects[i].screen = screen;
      TCEffects[i].id = id;

      if (is_tab_stop && !Screen_has_effect_with_focus[screen]) {
        // this effect should get focus
        make_focus = true;
      }

      EfxInit(&TCEffects[i], is_tab_stop, make_focus);
      return i;
    }
  }
  return INVALID_EFFECT_HANDLE;
}

// Frees up any memory allocated for effect, sets type to EFX_NONE
void EfxFreeEffect(tceffect *tce) {
  ASSERT(tce);

  // unlink it out of the list
  int prev, next;
  prev = tce->prev;
  next = tce->next;

  if (prev == -1) {
    // removing a root
    if (next != -1) {
      Screen_roots[tce->screen] = next;
    } else {
      Screen_roots[tce->screen] = -1;
    }
  } else {
    // removing a middle node
    TCEffects[prev].next = next;
    if (next != -1)
      TCEffects[next].prev = prev;
  }
  tce->prev = tce->next = -1;

  if (tce->text_buffer) {
    mem_free(tce->text_buffer);
    tce->text_buffer = NULL;
  }

  if (tce->has_focus) {
    // disable the tab stop
    tce->tab_stop = false;

    // uh oh, we're removing the item on the screen that has focus
    Screen_has_effect_with_focus[tce->screen] = false;

    // see if we can find another item and give it focus
    int node = Screen_roots[tce->screen];
    while (node != -1) {
      if (TCEffects[node].tab_stop) {
        // ok we found some one with a tab stop, give em focus
        TCEffects[node].has_focus = true;
        Screen_has_effect_with_focus[tce->screen] = true;
        break;
      }
      node = TCEffects[node].next;
    }
  }

  int i;

  switch (tce->type) {
  case EFX_TEXT_TYPE:
    TelcomStopSound(TCSND_TYPING);
    break;
  case EFX_TEXT_STATIC:
  case EFX_TEXT_FADE:
    break;
  case EFX_BMP_STATIC:
  case EFX_BMP_BLUR:
  case EFX_BMP_SCANLINE:
  case EFX_BMP_INVERT:
  case EFX_BMP_STRETCH:
    if (tce->bmpinfo.bm_handle > BAD_BITMAP_HANDLE)
      bm_FreeBitmap(tce->bmpinfo.bm_handle);
    tce->bmpinfo.bm_handle = -1;

    if (tce->bmpinfo.temp_bmhandle > BAD_BITMAP_HANDLE)
      bm_FreeBitmap(tce->bmpinfo.temp_bmhandle);
    tce->bmpinfo.temp_bmhandle = -1;

    if (tce->bmpinfo.bitmaps) {
      for (i = 0; i < tce->bmpinfo.bm_count; i++) {
        if (tce->bmpinfo.bitmaps[i] > BAD_BITMAP_HANDLE)
          bm_FreeBitmap(tce->bmpinfo.bitmaps[i]);
      }
      mem_free(tce->bmpinfo.bitmaps);
      tce->bmpinfo.bitmaps = NULL;
    }

    if (tce->bmpinfo.chunk_bmp.pw != 0 || tce->bmpinfo.chunk_bmp.ph != 0) {
      bm_DestroyChunkedBitmap(&tce->bmpinfo.chunk_bmp);
      tce->bmpinfo.chunk_bmp.pw = tce->bmpinfo.chunk_bmp.ph = 0;
    }
    break;
  case EFX_BACKGROUND:
    break;
  case EFX_MOVIE:
    if (tce->movieinfo.handle) {
      EndMovie(tce->movieinfo.handle);
      tce->movieinfo.handle = NULL;
    }

    if (tce->movieinfo.filename) {
      mem_free(tce->movieinfo.filename);
      tce->movieinfo.filename = NULL;
    }
    break;
  case EFX_POLYMODEL:
    break;
  case EFX_SOUND:
    break;
  case EFX_BUTTON:
    if (tce->buttoninfo.bm_handle > BAD_BITMAP_HANDLE)
      bm_FreeBitmap(tce->buttoninfo.bm_handle);
    tce->buttoninfo.bm_handle = -1;

    if (tce->buttoninfo.bmfocus_handle > BAD_BITMAP_HANDLE)
      bm_FreeBitmap(tce->buttoninfo.bmfocus_handle);
    tce->buttoninfo.bmfocus_handle = -1;

    if (tce->buttoninfo.chunk_bmp.pw != 0 || tce->buttoninfo.chunk_bmp.ph != 0) {
      bm_DestroyChunkedBitmap(&tce->buttoninfo.chunk_bmp);
      tce->buttoninfo.chunk_bmp.pw = tce->buttoninfo.chunk_bmp.ph = 0;
    }

    if (tce->buttoninfo.chunkfocus_bmp.pw != 0 || tce->buttoninfo.chunkfocus_bmp.ph != 0) {
      bm_DestroyChunkedBitmap(&tce->buttoninfo.chunkfocus_bmp);
      tce->buttoninfo.chunkfocus_bmp.pw = tce->buttoninfo.chunkfocus_bmp.ph = 0;
    }

    if (tce->buttoninfo.flash_handle > BAD_BITMAP_HANDLE)
      bm_FreeBitmap(tce->buttoninfo.flash_handle);
    tce->buttoninfo.flash_handle = -1;

    if (tce->buttoninfo.flashfocus_handle > BAD_BITMAP_HANDLE)
      bm_FreeBitmap(tce->buttoninfo.flashfocus_handle);
    tce->buttoninfo.flashfocus_handle = -1;

    if (tce->buttoninfo.flash_chunk.pw != 0 || tce->buttoninfo.flash_chunk.ph != 0) {
      bm_DestroyChunkedBitmap(&tce->buttoninfo.flash_chunk);
      tce->buttoninfo.flash_chunk.pw = tce->buttoninfo.flash_chunk.ph = 0;
    }

    if (tce->buttoninfo.flashfocus_chunk.pw != 0 || tce->buttoninfo.flashfocus_chunk.ph != 0) {
      bm_DestroyChunkedBitmap(&tce->buttoninfo.flashfocus_chunk);
      tce->buttoninfo.flashfocus_chunk.pw = tce->buttoninfo.flashfocus_chunk.ph = 0;
    }

    break;
  default:
    Int3(); // invalid type
    break;
  }
  tce->type = EFX_NONE;
  tce->monitor = -1;
}

// Initializes an effect based on it's type
void EfxInit(tceffect *tce, bool tab_stop, bool gets_focus) {

  ASSERT(tce);
  ASSERT(tce->type != EFX_NONE);
  ASSERT(tce->monitor >= 0 && tce->monitor < MAX_MONITOR);
  ASSERT(tce->screen >= 0 && tce->screen < MAX_TELCOM_SCREENS);
  ASSERT(tce >= TCEffects);

  // Link effect in
  int root = Screen_roots[tce->screen];
  int effect_num = (tce - TCEffects);
  tce->tab_stop = tab_stop;
  tce->has_focus = (tab_stop) ? gets_focus : false;
  if (tce->has_focus) {
    // the screen has a definite focus, so set the global
    Screen_has_effect_with_focus[tce->screen] = true;
  }

  if (root == -1) {
    // no roots for this monitor, add it
    Screen_roots[tce->screen] = effect_num;
    tce->prev = -1;
    tce->next = -1;
  } else {
    // traverse until we get to the end, then add it
    int curr, next = -1;
    bool done = false;

    curr = root;

    if (tce->type == EFX_BUTTON) {
      // go to the end of the list
      while (!done) {
        if (TCEffects[curr].next == -1) {
          done = true;
        } else {
          curr = TCEffects[curr].next;
        }
      }
      TCEffects[curr].next = effect_num;
      TCEffects[effect_num].prev = curr;
      TCEffects[effect_num].next = -1;
    } else {
      // go to where the buttons start
      if (TCEffects[root].type == EFX_BUTTON) {
        // replace the root
        Screen_roots[tce->screen] = effect_num;
        TCEffects[effect_num].prev = -1;
        TCEffects[effect_num].next = root;
        TCEffects[root].prev = effect_num;
      } else {
        while (!done) {
          if (TCEffects[curr].next == -1 || TCEffects[TCEffects[curr].next].type == EFX_BUTTON) {
            next = TCEffects[curr].next;
            done = true;
          } else {
            curr = TCEffects[curr].next;
          }
        }
        TCEffects[curr].next = effect_num;
        TCEffects[effect_num].prev = curr;
        TCEffects[effect_num].next = next;
      }
    } // end else
  }   // end else

  tce->pos_x = tce->pos_y = 0;
  tce->text_buffer = NULL;
  tce->age = 0;
  tce->flags = 0;
  tce->color = GR_GREEN;
  tce->w = tce->h = 0;
  tce->speed = 1.0f;
  tce->start_time = 0;

  // clear the event queue
  for (int j = 0; j < MAX_EFFECT_EVENTS; j++)
    tce->event_queue[j].id = -1;

  switch (tce->type) {
  case EFX_TEXT_STATIC:
  case EFX_TEXT_TYPE:
  case EFX_TEXT_FADE:
    tce->textinfo.last_letter = 0;
    tce->textinfo.sound_index = -1;
    tce->textinfo.font_index = BRIEF_FONT_INDEX;
    ;
    tce->textinfo.line_index = 0;
    tce->textinfo.scroll_u = tce->textinfo.scroll_d = false;
    tce->textinfo.scroll = false;
    break;
  case EFX_BMP_STATIC:
  case EFX_BMP_BLUR:
  case EFX_BMP_SCANLINE:
  case EFX_BMP_INVERT:
  case EFX_BMP_STRETCH:
    tce->bmpinfo.temp_bmhandle = -1;
    tce->bmpinfo.stage = 0;
    tce->bmpinfo.bitmaps = NULL;
    tce->bmpinfo.bm_count = 0;
    tce->bmpinfo.bm_handle = -1;
    tce->bmpinfo.chunk_bmp.pw = tce->bmpinfo.chunk_bmp.ph = 0;
    break;
  case EFX_BACKGROUND:
    break;
  case EFX_MOVIE:
    tce->movieinfo.handle = NULL;
    tce->movieinfo.filename = NULL;
    tce->movieinfo.fps = 20.0f;
    break;
  case EFX_POLYMODEL:
    vm_MakeZero(&tce->polyinfo.m_Rot);
    vm_MakeZero(&tce->polyinfo.m_Pos);
    vm_MakeZero(&tce->polyinfo.m_Ori);
    vm_MakeIdentity(&tce->polyinfo.m_mOrient);
    tce->polyinfo.handle = -1;
    break;
  case EFX_SOUND:
    tce->soundinfo.started = false;
    tce->soundinfo.handle = -1;
    break;
  case EFX_BUTTON:
    tce->buttoninfo.click_type = CLICKTYPE_CLICKUP;
    tce->buttoninfo.button_type = BUTT_NEXTPAGE;
    tce->buttoninfo.sibling = -1;
    tce->buttoninfo.parent = -1;
    tce->buttoninfo.internal = NULL;
    tce->buttoninfo.jump_page = 0;
    break;
  default:
    Int3(); // Invalid Effect
  }
}

// Renders a Monitor
void RenderScreen(int screen, tTelComInfo *tsys, float Frametime) {
  ASSERT(screen >= -1 && screen < MAX_TELCOM_SCREENS);

  int node = Screen_roots[screen];
  tceffect *tce;

  int count = 0;
  int x, y;

  while (node != -1) {
    tce = &TCEffects[node];
    x = tsys->Monitor_coords[tce->monitor].left;
    y = tsys->Monitor_coords[tce->monitor].top;

    if (tce->start_time <= 0) {
      RenderEffect(tce, Frametime, x, y, true);
      count++;
    } else {
      RenderEffect(tce, Frametime, x, y, false);
      tce->start_time -= Frametime;
    }

    node = TCEffects[node].next;
  }
  // mprintf(0,"Effect Count: %d effects\n",count);
}

// Renders an effect
void RenderEffect(tceffect *tce, float frametime, int xoff, int yoff, bool ok_to_render) {
  ASSERT(tce->type != EFX_NONE);

  switch (tce->type) {
  case EFX_TEXT_STATIC:
    RenderTextStatic(tce, frametime, xoff, yoff, ok_to_render);
    break;
  case EFX_TEXT_TYPE:
    RenderTextType(tce, frametime, xoff, yoff, ok_to_render);
    break;
  case EFX_TEXT_FADE:
    RenderTextFade(tce, frametime, xoff, yoff, ok_to_render);
    break;
  case EFX_BMP_STATIC:
    RenderBmpStatic(tce, frametime, xoff, yoff, ok_to_render);
    break;
  case EFX_BMP_BLUR:
    RenderBmpBlur(tce, frametime, xoff, yoff, ok_to_render);
    break;
  case EFX_BMP_SCANLINE:
    RenderBmpScanline(tce, frametime, xoff, yoff, ok_to_render);
    break;
  case EFX_BMP_INVERT:
    RenderBmpInvert(tce, frametime, xoff, yoff, ok_to_render);
    break;
  case EFX_BMP_STRETCH:
    RenderBmpStretch(tce, frametime, xoff, yoff, ok_to_render);
    break;
  case EFX_BACKGROUND:
    RenderBackground(tce, frametime, xoff, yoff, ok_to_render);
    break;
  case EFX_MOVIE:
    RenderMovie(tce, frametime, xoff, yoff, ok_to_render);
    break;
  case EFX_POLYMODEL:
    RenderPolyModel(tce, frametime, xoff, yoff, ok_to_render);
    break;
  case EFX_SOUND:
    RenderSound(tce, frametime, xoff, yoff, ok_to_render);
    break;
  case EFX_BUTTON:
    RenderButton(tce, frametime, xoff, yoff, ok_to_render);
    break;
  default:
    Int3();
  };
}

// Sends an event to an effect
void SendEventToEffect(int effect, int event, int parm1, int parm2) {
  ASSERT(effect >= 0 && effect < MAX_TCEFFECTS);
  ASSERT(event >= 0);

  tceffect *tce = &TCEffects[effect];

  for (int j = 0; j < MAX_EFFECT_EVENTS; j++) {
    if (tce->event_queue[j].id == -1) {
      tce->event_queue[j].id = event;
      tce->event_queue[j].parms[0] = parm1;
      tce->event_queue[j].parms[1] = parm2;
      return;
    }
  }

  // if we got here than the event queue is filled up
  mprintf(0, "Warning: Event queue filled up for effect %d\n", effect);
}

//	Pops off an event for processesing, returns -1 if no event available
bool PopEvent(tceffect *tce, tTCEvent *evt) {
  ASSERT(tce);
  int ret = tce->event_queue[0].id;

  if (ret == -1)
    return false;

  memcpy(evt, &tce->event_queue[0], sizeof(tTCEvent));

  // slide everything up (if it isn't -1)
  for (int j = 1; j < MAX_EFFECT_EVENTS; j++) {
    memcpy(&tce->event_queue[j - 1], &tce->event_queue[j], sizeof(tTCEvent));
  }
  tce->event_queue[MAX_EFFECT_EVENTS - 1].id = -1;
  return true;
}

//	Returns the efxnum given an id & screen, -1 if not found
int GetEfxNumFromID(int id, int screen) {
  ASSERT(screen >= 0 && screen < MAX_TELCOM_SCREENS);

  int node = Screen_roots[screen];

  while (node != -1) {
    if (TCEffects[node].id == id) {
      return node;
    }

    node = TCEffects[node].next;
  }
  return -1;
}

//	returns the efxnum of the button that contains the X,Y on the given monitor/screen, -1 if none
int FindButtonEffectByXY(int x, int y, int screen) {
  ASSERT(screen >= 0 && screen < MAX_TELCOM_SCREENS);
  tceffect *tce;
  int node = Screen_roots[screen];

  while (node != -1) {
    if (TCEffects[node].type == EFX_BUTTON) {
      tce = &TCEffects[node];
      if ((tce->pos_x + glitch_dx <= x) && (x <= tce->pos_x + tce->w + glitch_dx) && (tce->pos_y + glitch_dy <= y) &&
          (y <= tce->pos_y + tce->h + glitch_dy)) {
        // we got a winner
        return node;
      }
    }
    node = TCEffects[node].next;
  }
  return -1;
}

/////////////////////////////////////////////////////////////////////////
char *format(const char *fmt, ...) {
  static char tempbuffer[8192];

  std::va_list ap;
  va_start(ap, fmt);
  std::vsnprintf(tempbuffer, sizeof(tempbuffer), fmt, ap);
  va_end(ap);
  return tempbuffer;
}

bool CreateTextEffect(TCTEXTDESC* desc, const char *text, int monitor, int screen, int id) {
  ASSERT(desc);
  ASSERT(text);

  int efxtype = -1;
  switch (desc->type) {
  case TC_TEXT_STATIC:
    efxtype = EFX_TEXT_STATIC;
    break;
  case TC_TEXT_FADE:
    efxtype = EFX_TEXT_FADE;
    break;
  case TC_TEXT_FLASH:
    efxtype = EFX_TEXT_TYPE;
    break;
  default:
    Int3(); // not supported
  }
  if (efxtype == -1)
    return false;

  // Creates an instance of the Effect
  int efxnum = EfxCreate(efxtype, monitor, screen, id, (desc->caps & TCTD_TABSTOP) ? true : false);
  if (efxnum == -1)
    return false;
  tceffect *tce = &TCEffects[efxnum];

  if (desc->caps & TCTD_FONT)
    tce->textinfo.font_index = desc->font;
  if (desc->caps & TCTD_COLOR)
    tce->color = desc->color;
  if (desc->caps & TCTD_SPEED)
    tce->speed = desc->speed;
  else
    tce->speed = 1.0f;
  if (desc->caps & TCTD_TEXTBOX) {
    tce->pos_x = desc->textbox.left;
    tce->pos_y = desc->textbox.top;
    tce->w = desc->textbox.right - desc->textbox.left;
    tce->h = desc->textbox.bottom - desc->textbox.top;
  } else
    Int3(); // need a text box

  if (desc->caps & TCTD_SCROLL)
    tce->textinfo.scroll = true;

  tce->flags = desc->flags;
  if (desc->caps & TCTD_WAITTIME)
    tce->start_time = desc->waittime;
  else
    tce->start_time = 0;
  if (desc->caps & TCTD_TABSTOP)
    tce->tab_stop = true;
  else
    tce->tab_stop = false;

  tce->id = id;

  switch (tce->type) {
  case EFX_TEXT_STATIC:
    return CreateTextStatic(tce, text);
  case EFX_TEXT_FADE:
    return CreateTextFade(tce, text);
  case EFX_TEXT_TYPE:
    return CreateTextType(tce, text);
  }
  return false;
}

bool CreateBitmapEffect(TCBMPDESC* desc, int monitor, int screen, int id) {
  ASSERT(desc);
  int efxtype = -1;
  switch (desc->type) {
  case TC_BMP_STATIC:
    efxtype = EFX_BMP_STATIC;
    break;
  case TC_BMP_BLUR:
    efxtype = EFX_BMP_BLUR;
    break;
  case TC_BMP_SCANLINE:
    efxtype = EFX_BMP_SCANLINE;
    break;
  case TC_BMP_INVERT:
    efxtype = EFX_BMP_INVERT;
    break;
  case TC_BMP_STRETCH:
    efxtype = EFX_BMP_STRETCH;
    break;
  default:
    Int3();
  }
  if (efxtype == -1)
    return false;

  // Creates an instance of the Effect
  int efxnum = EfxCreate(efxtype, monitor, screen, id);
  if (efxnum == -1)
    return false;
  tceffect *tce = &TCEffects[efxnum];

  if (desc->caps & TCBD_XY) {
    tce->pos_x = desc->x;
    tce->pos_y = desc->y;
  } else {
    tce->pos_x = tce->pos_y = 0;
  }
  if (desc->caps & TCBD_WAITTIME)
    tce->start_time = desc->waittime;
  else
    tce->start_time = 0;
  if (desc->caps & TCBD_SPEED)
    tce->speed = desc->speed;
  else
    tce->speed = 1;
  tce->flags = desc->flags;

  switch (tce->type) {
  case EFX_BMP_STATIC:
    return CreateBmpStatic(tce, desc->filename);
  case EFX_BMP_BLUR:
    return CreateBmpBlur(tce, desc->filename);
  case EFX_BMP_SCANLINE:
    return CreateBmpScanline(tce, desc->filename);
  case EFX_BMP_INVERT:
    return CreateBmpInvert(tce, desc->filename);
  case EFX_BMP_STRETCH:
    return CreateBmpStretch(tce, desc->filename);
  }
  return false;
}

bool CreateMovieEffect(TCMOVIEDESC* desc, int monitor, int screen, int id) {
  ASSERT(desc);
  int efxtype = -1;
  switch (desc->type) {
  case TC_MOVIE_STATIC:
    efxtype = EFX_MOVIE;
    break;
  default:
    Int3();
  }
  if (efxtype == -1)
    return false;
  // Creates an instance of the Effect
  int efxnum = EfxCreate(efxtype, monitor, screen, id);
  if (efxnum == -1)
    return false;
  tceffect *tce = &TCEffects[efxnum];

  if (desc->caps & TCMD_XY)
    tce->movieinfo.fps = desc->fps;
  else
    tce->movieinfo.fps = 20.0f;
  if (desc->caps & TCMD_WAITTIME)
    tce->start_time = desc->waittime;
  else
    tce->start_time = 0;
  if (desc->caps & TCMD_XY) {
    tce->pos_x = desc->x;
    tce->pos_y = desc->y;
  } else {
    tce->pos_x = tce->pos_y = 0;
  }

  switch (tce->type) {
  case EFX_MOVIE:
    return CreateMovie(tce, desc->filename);
  }

  return false;
}

extern tTelComInfo Telcom_system;
bool CreateBackgroundEffect(TCBKGDESC* desc, int monitor, int screen, int id) {
  ASSERT(desc);
  int efxtype = -1;
  switch (desc->type) {
  case TC_BACK_STATIC:
    efxtype = EFX_BACKGROUND;
    break;
  default:
    Int3();
  }
  if (efxtype == -1)
    return false;
  // Creates an instance of the Effect
  int efxnum = EfxCreate(efxtype, monitor, screen, id);
  if (efxnum == -1)
    return false;
  tceffect *tce = &TCEffects[efxnum];

  if (desc->caps & TCBGD_COLOR)
    tce->color = desc->color;
  else
    tce->color = GR_BLUE;
  if (desc->caps & TCBGD_WAITTIME)
    tce->start_time = desc->waittime;
  else
    tce->start_time = 0;

  tce->pos_x = 0;
  tce->pos_y = 0;
  tce->w = Telcom_system.Monitor_coords[monitor].right - Telcom_system.Monitor_coords[monitor].left;
  tce->h = Telcom_system.Monitor_coords[monitor].bottom - Telcom_system.Monitor_coords[monitor].top;

  switch (tce->type) {
  case EFX_BACKGROUND:
    return CreateBackground(tce);
  }
  return false;
}

bool CreatePolyModelEffect(TCPOLYDESC* desc, int monitor, int screen, int id) {
  ASSERT(desc);
  int efxtype = -1;
  switch (desc->type) {
  case TC_POLY_STATIC:
    efxtype = EFX_POLYMODEL;
    break;
  default:
    Int3();
  }
  Int3(); // polymodel not supported yet
  return false;
}

bool CreateSoundEffect(TCSNDDESC* desc, int monitor, int screen, int id) {
  ASSERT(desc);
  int efxtype = -1;
  switch (desc->type) {
  case TC_SND_STATIC:
    efxtype = EFX_SOUND;
    break;
  default:
    Int3();
  }
  if (efxtype == -1)
    return false;
  // Creates an instance of the Effect
  int efxnum = EfxCreate(efxtype, monitor, screen, id);
  if (efxnum == -1)
    return false;
  tceffect *tce = &TCEffects[efxnum];
  if (desc->caps & TCSD_WAITTIME)
    tce->start_time = desc->waittime;
  else
    tce->start_time = 0;

  switch (tce->type) {
  case EFX_SOUND:
    return CreateSound(tce, desc->filename);
  }
  return false;
}

int CreateButtonEffect(TCBUTTONDESC* desc, int monitor, int screen, int id) {
  ASSERT(desc);
  ASSERT(monitor == MONITOR_MAIN);

  // Creates an instance of the Effect
  int efxnum = EfxCreate(EFX_BUTTON, monitor, screen, id, desc->tab_stop);
  if (efxnum == -1)
    return -1;
  tceffect *tce = &TCEffects[efxnum];

  // Load in regular bitmap
  tce->buttoninfo.chunk_bmp.pw = tce->buttoninfo.chunk_bmp.ph = 0;
  tce->buttoninfo.bm_handle = bm_AllocLoadFileBitmap(IGNORE_TABLE(desc->filename), 0);
  if (tce->buttoninfo.bm_handle == -1) {
    desc->w = desc->h = 0;
    tce->buttoninfo.bm_handle = BAD_BITMAP_HANDLE;
    mprintf(0, "Unable to load bmp '%s'\n", desc->filename);
  } else {
    desc->w = bm_w(tce->buttoninfo.bm_handle, 0);
    desc->h = bm_h(tce->buttoninfo.bm_handle, 0);
    bm_CreateChunkedBitmap(tce->buttoninfo.bm_handle, &tce->buttoninfo.chunk_bmp);
  }

  tce->buttoninfo.chunkfocus_bmp.pw = tce->buttoninfo.chunkfocus_bmp.ph = 0;
  tce->buttoninfo.bmfocus_handle = bm_AllocLoadFileBitmap(IGNORE_TABLE(desc->filename_focus), 0);
  if (tce->buttoninfo.bmfocus_handle == -1) {
    tce->buttoninfo.bmfocus_handle = BAD_BITMAP_HANDLE;
    mprintf(0, "Unable to load bmp '%s'\n", desc->filename_focus);
  } else {
    bm_CreateChunkedBitmap(tce->buttoninfo.bmfocus_handle, &tce->buttoninfo.chunkfocus_bmp);
  }

  // Load in flash/glow bitmap
  tce->buttoninfo.flash_chunk.pw = tce->buttoninfo.flash_chunk.ph = 0;
  if (desc->flasher) {
    tce->buttoninfo.flash_handle = bm_AllocLoadFileBitmap(IGNORE_TABLE(desc->flash_filename), 0);
    if (tce->buttoninfo.flash_handle == -1) {
      mprintf(0, "Unable to load bmp '%s'\n", desc->flash_filename);
    } else {
      bm_CreateChunkedBitmap(tce->buttoninfo.flash_handle, &tce->buttoninfo.flash_chunk);
    }
  } else {
    tce->buttoninfo.flash_handle = -1;
  }

  tce->buttoninfo.flashfocus_chunk.pw = tce->buttoninfo.flashfocus_chunk.ph = 0;
  if (desc->flasher) {
    tce->buttoninfo.flashfocus_handle = bm_AllocLoadFileBitmap(IGNORE_TABLE(desc->flash_filename_focus), 0);
    if (tce->buttoninfo.flashfocus_handle == -1) {
      mprintf(0, "Unable to load bmp '%s'\n", desc->flash_filename_focus);
    } else {
      bm_CreateChunkedBitmap(tce->buttoninfo.flashfocus_handle, &tce->buttoninfo.flashfocus_chunk);
    }
  } else {
    tce->buttoninfo.flashfocus_handle = -1;
  }

  if (desc->button_type == BUTT_INTERNAL) {
    tce->buttoninfo.internal = desc->internal;
  } else {
    tce->buttoninfo.internal = NULL;
  }

  tce->buttoninfo.sibling = desc->sibling_id;
  tce->buttoninfo.parent = desc->parent_id;
  tce->buttoninfo.button_type = desc->button_type;
  tce->buttoninfo.click_type = desc->click_type;
  tce->buttoninfo.flash_time = desc->flash_time;
  tce->w = desc->w;
  tce->h = desc->h;
  tce->pos_x = desc->x;
  tce->pos_y = desc->y;
  tce->flags = desc->osflags;
  tce->buttoninfo.jump_page = desc->jump_page;

  return efxnum;
}

////////////////////////////////////////////////////////////////////
bool CreateTextStatic(tceffect *tce, const char *text) {
  ASSERT(text);
  if (!text)
    return false;

  tce->text_buffer = (char *)mem_malloc(strlen(text) + 10);
  if (!tce->text_buffer)
    return false;

  tce->alpha = 255;

  char *tempbuffer = (char *)mem_malloc(strlen(text) + 10);
  if (!tempbuffer)
    return false;
  strcpy(tempbuffer, text);

  // apply the word wrap
  textaux_WordWrap(tempbuffer, tce->text_buffer, tce->w, Game_fonts[tce->textinfo.font_index]);

  mem_free(tempbuffer);

  return true;
}

bool CreateTextFade(tceffect *tce, const char *text) {
  ASSERT(text);
  if (!text || text[0] == '\0')
    return false;
  tce->text_buffer = (char *)mem_malloc(strlen(text) + 10);
  if (!tce->text_buffer)
    return false;

  char *tempbuffer = (char *)mem_malloc(strlen(text) + 10);
  if (!tempbuffer)
    return false;
  strcpy(tempbuffer, text);

  // apply the word wrap
  textaux_WordWrap(tempbuffer, tce->text_buffer, tce->w, Game_fonts[tce->textinfo.font_index]);

  mem_free(tempbuffer);

  switch (tce->flags) {
  case TC_TEXTF_OUT:
    tce->alpha = 255.0f;
    break;
  case TC_TEXTF_IN:
  case TC_TEXTF_PINGPONG:
  default:
    tce->alpha = 0.01f;
    break;
  }
  return true;
}

bool CreateTextType(tceffect *tce, const char *text) {
  ASSERT(text);
  if (!text || text[0] == '\0')
    return false;
  tce->text_buffer = (char *)mem_malloc(strlen(text) + 10);
  if (!tce->text_buffer)
    return false;

  char *tempbuffer = (char *)mem_malloc(strlen(text) + 10);
  if (!tempbuffer)
    return false;
  strcpy(tempbuffer, text);

  // apply the word wrap
  textaux_WordWrap(tempbuffer, tce->text_buffer, tce->w, Game_fonts[tce->textinfo.font_index]);

  mem_free(tempbuffer);

  tce->textinfo.last_letter = 0;
  return true;
}

bool CreateBmpStatic(tceffect *tce, const char *filename) {
  ASSERT(filename);

  tce->bmpinfo.chunk_bmp.pw = tce->bmpinfo.chunk_bmp.ph = 0;
  tce->bmpinfo.bm_handle = bm_AllocLoadFileBitmap(IGNORE_TABLE(filename), 0);
  if (tce->bmpinfo.bm_handle == -1) {
    tce->bmpinfo.bm_handle = BAD_BITMAP_HANDLE;
    mprintf(0, "Unable to load bmp '%s'\n", filename);
  } else
    bm_CreateChunkedBitmap(tce->bmpinfo.bm_handle, &tce->bmpinfo.chunk_bmp);
  return true;
}

bool CreateBmpBlur(tceffect *tce, const char *filename) {
  ASSERT(filename);

  tce->bmpinfo.chunk_bmp.pw = tce->bmpinfo.chunk_bmp.ph = 0;
  tce->bmpinfo.bm_handle = bm_AllocLoadFileBitmap(IGNORE_TABLE(filename), 0);
  if (tce->bmpinfo.bm_handle == -1) {
    tce->bmpinfo.bm_handle = BAD_BITMAP_HANDLE;
    mprintf(0, "Unable to load bmp '%s'\n", filename);
  } else {
    tce->bmpinfo.temp_bmhandle = bm_AllocBitmap(bm_w(tce->bmpinfo.bm_handle, 0), bm_h(tce->bmpinfo.bm_handle, 0), 0);
    if (tce->bmpinfo.temp_bmhandle == -1)
      tce->bmpinfo.temp_bmhandle = BAD_BITMAP_HANDLE;

    if (tce->flags == TC_BMPF_OUT)
      tce->bmpinfo.stage = BLUR_STAGES;
    else
      tce->bmpinfo.stage = 0;

    // create the chunk bitmap for use
    bm_CreateChunkedBitmap(tce->bmpinfo.bm_handle, &tce->bmpinfo.chunk_bmp);
  }
  return true;
}

bool CreateBmpScanline(tceffect *tce, const char *filename) {
  ASSERT(filename);

  tce->bmpinfo.chunk_bmp.pw = tce->bmpinfo.chunk_bmp.ph = 0;
  tce->bmpinfo.bm_handle = bm_AllocLoadFileBitmap(IGNORE_TABLE(filename), 0);
  tce->bmpinfo.temp_bmhandle = BAD_BITMAP_HANDLE;
  if (tce->bmpinfo.bm_handle == -1) {
    tce->bmpinfo.bm_handle = BAD_BITMAP_HANDLE;
    mprintf(0, "Unable to load bmp '%s'\n", filename);
  } else {
    tce->bmpinfo.temp_bmhandle = bm_AllocBitmap(bm_w(tce->bmpinfo.bm_handle, 0), bm_h(tce->bmpinfo.bm_handle, 0), 0);
    if (tce->bmpinfo.temp_bmhandle == -1)
      tce->bmpinfo.temp_bmhandle = BAD_BITMAP_HANDLE;

    // create the chunk bitmap for use
    bm_CreateChunkedBitmap(tce->bmpinfo.bm_handle, &tce->bmpinfo.chunk_bmp);
  }
  return true;
}

bool CreateBmpInvert(tceffect *tce, const char *filename) {
  ASSERT(filename);

  tce->bmpinfo.chunk_bmp.pw = tce->bmpinfo.chunk_bmp.ph = 0;
  tce->bmpinfo.bm_handle = bm_AllocLoadFileBitmap(IGNORE_TABLE(filename), 0);
  tce->bmpinfo.temp_bmhandle = BAD_BITMAP_HANDLE;
  if (tce->bmpinfo.bm_handle == -1) {
    tce->bmpinfo.bm_handle = BAD_BITMAP_HANDLE;
    mprintf(0, "Unable to load bmp '%s'\n", filename);
  } else {
    tce->bmpinfo.temp_bmhandle = bm_AllocBitmap(bm_w(tce->bmpinfo.bm_handle, 0), bm_h(tce->bmpinfo.bm_handle, 0), 0);
    if (tce->bmpinfo.temp_bmhandle == -1)
      tce->bmpinfo.temp_bmhandle = BAD_BITMAP_HANDLE;

    // create the chunk bitmap for use
    bm_CreateChunkedBitmap(tce->bmpinfo.bm_handle, &tce->bmpinfo.chunk_bmp);
  }
  return true;
}

bool CreateBmpStretch(tceffect *tce, const char *filename) {
  ASSERT(filename);

  tce->bmpinfo.chunk_bmp.pw = tce->bmpinfo.chunk_bmp.ph = 0;
  tce->bmpinfo.bm_handle = bm_AllocLoadFileBitmap(IGNORE_TABLE(filename), 0);
  tce->bmpinfo.bitmaps = NULL;
  tce->bmpinfo.bm_count = 0;
  if (tce->bmpinfo.bm_handle == -1) {
    tce->bmpinfo.bm_handle = BAD_BITMAP_HANDLE;
    mprintf(0, "Unable to load bmp '%s'\n", filename);
  } else {
    // determine how many wide we need
    int src_w = bm_w(tce->bmpinfo.bm_handle, 0);
    int w_count = 0;
    w_count = ((src_w + 63) / 64);
    // determine how many tall we need
    int src_h = bm_h(tce->bmpinfo.bm_handle, 0);
    int h_count = 0;
    h_count = ((src_h + 63) / 64);

    int total = w_count * h_count;
    int index;
    tce->bmpinfo.bm_count = total;
    tce->bmpinfo.bitmaps = (int *)mem_malloc(sizeof(int) * total);
    if (!tce->bmpinfo.bitmaps)
      return false;

    for (index = 0; index < total; index++) {
      tce->bmpinfo.bitmaps[index] = bm_AllocBitmap(64, 64, 0);
      if (tce->bmpinfo.bitmaps[index] == -1)
        tce->bmpinfo.bitmaps[index] = BAD_BITMAP_HANDLE;
    }

    // now fill in the bitmaps
    int realx, realy;
    realx = 0;
    realy = 0;

    int wi, hi;
    uint16_t *srcdata = bm_data(tce->bmpinfo.bm_handle, 0);
    uint16_t *destdata;

    realx = 0;
    realy = 0;
    index = 0;
    for (hi = 0, realy = 0; hi < h_count; hi++, realy += 64)
      for (wi = 0, realx = 0; wi < w_count; wi++, realx += 64) {
        int x, y;
        int ix, iy;

        destdata = bm_data(tce->bmpinfo.bitmaps[index], 0);

        for (y = realy, iy = 0; y < realy + 64; iy++, y++)
          for (x = realx, ix = 0; x < realx + 64; ix++, x++) {
            if ((x < src_w) && (y < src_h)) {
              destdata[(iy * 64) + ix] = srcdata[(y * src_w) + x];
            } else {
              destdata[(iy * 64) + ix] = NEW_TRANSPARENT_COLOR;
            }
          }
        index++;
      }
  }
  return true;
}

bool CreateMovie(tceffect *tce, const char *filename) {
  ASSERT(filename);

  tce->movieinfo.handle = NULL;
  tce->movieinfo.filename = NULL;
  tce->w = tce->h = 100;

  tce->movieinfo.filename = (char *)mem_malloc(strlen(filename) + 1);
  if (!tce->movieinfo.filename)
    return false;
  strcpy(tce->movieinfo.filename, filename);
  tce->movieinfo.handle = StartMovie(filename);
  if (!tce->movieinfo.handle)
    return false;
  return true;
}

bool CreateBackground(tceffect *tce) { return true; }

bool CreateSound(tceffect *tce, const char *filename) {
  tce->soundinfo.handle = FindSoundName(IGNORE_TABLE(filename));
  if (tce->soundinfo.handle == -1) {
    mprintf(0, "Unable to find sound '%s'\n", filename);
  }
  return true;
}

void BltBmpToScreen(int dx, int dy, chunked_bitmap *src_bmp) {
  BltBmpToScreen(dx, dy, src_bmp, 0, 0, src_bmp->pw, src_bmp->ph);
}
void BltBmpToScreen(int dx, int dy, chunked_bitmap *src_bmp, int sx, int sy, int sw, int sh) {
  rend_DrawChunkedBitmap(src_bmp, dx, dy, 255);
}
// forces a chunk bitmap to be uploaded
void UploadChunk(chunked_bitmap *bmp) {
  ASSERT(bmp);

  int count = bmp->w * bmp->h;

  for (int index = 0; index < count; index++) {
    int bm = bmp->bm_array[index];
    GameBitmaps[bm].flags |= BF_CHANGED;
  }
}
