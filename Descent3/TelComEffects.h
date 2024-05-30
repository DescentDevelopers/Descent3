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

 * $Logfile: /DescentIII/main/TelComEffects.h $
 * $Revision: 27 $
 * $Date: 4/20/99 12:46p $
 * $Author: Jeff $
 *
 * TelCom Effect class defines
 *
 * $Log: /DescentIII/main/TelComEffects.h $
 *
 * 27    4/20/99 12:46p Jeff
 * telcom main menu, mouse over button sets focus.  if you go into telcom
 * main menu, when you leave a system it will return you to main menu.
 *
 * 26    4/17/99 6:15p Samir
 * replaced gr.h with grdefs.h and fixed resulting compile bugs.
 *
 * 25    4/14/99 3:57a Jeff
 * fixed case mismatch in #includes
 *
 * 24    4/02/99 8:32p Jeff
 * added system event queue.  Arrow keys work in main menu and ship
 * selection
 *
 * 23    2/17/99 6:55p Jeff
 * added jump button type.  Added no early render flag for bitmaps.  Fixed
 * color bug for type text
 *
 * 22    2/03/99 12:14a Jeff
 * updated single player ship selection to full functional...needs effects
 * added in.  Had to add flag to buttons for gain focus to register as a
 * click
 *
 * 21    2/01/99 4:52p Jeff
 * screenshots work in telcom
 *
 * 20    10/12/98 8:32p Jeff
 * changed the way focus is handled
 *
 * 19    9/17/98 2:29p Jeff
 * added focus filenames to button effect
 *
 * 18    8/27/98 2:51p Jeff
 * New TelCom finally checked in
 *
 * 17    7/11/98 9:17p Jeff
 * moved wordwrap functions out so they can be used externally
 *
 * 16    6/17/98 4:15p Jeff
 * adjusted scroll button positions
 *
 * 15    5/19/98 3:40p Jeff
 * poly models functional
 *
 * 14    5/15/98 5:16p Jeff
 * use regular draw chunk bitmap
 *
 * 13    5/11/98 6:21p Jeff
 * fixed setting glitch offsets
 *
 * 12    5/07/98 3:31p Jeff
 * various optimizations, removed Copy and similar functions
 *
 * 11    5/05/98 6:50p Jeff
 * Telcom doesn't use rend_DrawLFBitmap anymore...more speed!
 *
 * 10    5/04/98 5:29p Jeff
 * Added sounds to TelCom events
 *
 * 9     5/04/98 1:35p Jeff
 * Changes made for mouse handling
 *
 * 8     5/04/98 11:01a Jeff
 * added scroll parameter to text
 *
 * 7     5/03/98 7:57p Jeff
 * Got scrolling textboxes working
 *
 * 6     5/01/98 2:17p Jeff
 * Added sound effect support
 *
 * 5     4/29/98 4:36p Jeff
 * added auto word wrap
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

#ifndef __TELCOM_EFFECTS_H_
#define __TELCOM_EFFECTS_H_

#include "pstypes.h"
#include "renderer.h"
#include "grdefs.h"
#include "cinematics.h"
#include "TelComEfxStructs.h"
#include "TelCom.h"
#include "vecmat_external.h"

/*
***************************************************************************************
                                                        Start of New TelCom
***************************************************************************************
*/

#define MAX_TCEFFECTS 256
#define MAX_EFFECT_EVENTS 5
#define INVALID_EFFECT_HANDLE 0xFFFFFFFF

// Effect types
#define EFX_NONE 0
#define EFX_TEXT_STATIC 1
#define EFX_TEXT_TYPE 2
#define EFX_TEXT_FADE 3
#define EFX_BMP_STATIC 4
#define EFX_BMP_BLUR 5
#define EFX_BMP_SCANLINE 6
#define EFX_BMP_INVERT 7
#define EFX_BMP_STRETCH 8
#define EFX_BACKGROUND 9
#define EFX_MOVIE 10
#define EFX_POLYMODEL 11
#define EFX_SOUND 12
#define EFX_BUTTON 13

// Events
#define TEVT_GAINFOCUS 0
#define TEVT_LOSTFOCUS 1
#define TEVT_MOUSEOVER 2
#define TEVT_MOUSECLICK 3
#define TEVT_MOUSEDOWN 4
#define TEVT_MOUSEUP 5
#define TEVT_TCQUIT 6
#define TEVT_TCNEXT 7
#define TEVT_TCPREV 8
#define TEVT_TCJUMP 9
#define TEVT_SCROLLDOWN 10
#define TEVT_SCROLLUP 11
#define TEVT_SCREENSHOT 12
#define TEVT_FAKEKEYPRESS 13
#define TEVT_MOUSEENTER 14

// others
#define BLUR_STAGE_MULTIPLE 0.12f
#define BLUR_STAGES 5
#define INVALID_EFFECT_ID 0x6FEFCD01

// Button types
#define BUTT_UPARROW 0
#define BUTT_DOWNARROW 1
#define BUTT_NEXTPAGE 2
#define BUTT_PREVPAGE 3
#define BUTT_QUIT 4
#define BUTT_INTERNAL 5
#define BUTT_JUMP 6

// Button Click types
#define CLICKTYPE_CLICKDOWN 0
#define CLICKTYPE_CLICKUP 1
#define CLICKTYPE_DOWN 2

struct tTextInfo {
  float last_letter;       // Last letter printed
  int sound_index;         // handle to sound for action
  int font_index;          // Font index
  int line_index;          // index of first text line (scrolling purposes)
  bool scroll_u, scroll_d; // if the scrollup down buttons are appearing
  bool scroll;             // set to true if it should be allowed to scroll
};

struct tBitmapInfo {
  int temp_bmhandle;        // handle of temporary bitmap
  float stage;              // stage that bitmap effect is in
  int *bitmaps;             // array of bitmaps (for scale effect)
  int bm_count;             // how many bitmaps (for scale effect)
  int bm_handle;            // handle to the bitmap
  chunked_bitmap chunk_bmp; // the chunk bitmap
};

struct tPolyInfo {
  vector m_Rot;     // current rotation
  vector m_Pos;     // current position
  vector m_Ori;     // current orientation
  matrix m_mOrient; // orientation
  int handle;       // handle to polymodel
};

struct tMovieInfo {
  tCinematic *handle; // handle to the movie
  char *filename;     // filename of the movie
  float fps;          // fps
};

struct tSoundInfo {
  bool started; // has the sound started?
  int handle;   // handle of the sound
};

struct tButtonInfo {
  bool flash_state; // Is the button glowing?
  uint8_t click_type; // CLICKTYPE_DOWN or CLICKTYPE_CLICK (what the button responds to)
  uint8_t button_type;
  int sibling; // Sibling effect (for down/up arrows), -1 if none
  int parent;  // Parent effect (text for down/up arrows), -1 if it works with TelCom System
  int bm_handle;
  int bmfocus_handle;
  int flash_handle;
  int flashfocus_handle;
  int real_x, real_y;
  int jump_page;
  float flash_time;
  void (*internal)(int);
  chunked_bitmap chunk_bmp;
  chunked_bitmap chunkfocus_bmp;
  chunked_bitmap flash_chunk;
  chunked_bitmap flashfocus_chunk;
};

struct tceffect {
  uint8_t type;     // Type of effect
  uint8_t monitor;  // Which monitor it belongs to
  uint8_t screen;   // What screen the effect belongs to
  bool tab_stop;  // This effect can have focus
  bool has_focus; // The effect currently has focus

  int id; // user given ID of effect (should be unique)

  char *text_buffer; // Pointer to allocated memory for text string

  int flags; // flags for effect

  int pos_x, pos_y; // Position of upper left corner
  int w, h;         // width and height
  ddgr_color color; // Color of effect

  float speed;      // speed of effect
  float alpha;      // alpha of effect
  float age;        // current age of effect
  float start_time; // how long until the effect starts

  union {
    tTextInfo textinfo;
    tBitmapInfo bmpinfo;
    tPolyInfo polyinfo;
    tMovieInfo movieinfo;
    tSoundInfo soundinfo;
    tButtonInfo buttoninfo;
  };

  tTCEvent event_queue[MAX_EFFECT_EVENTS]; // event queue for an effect

  int prev, next; // Links to previous and next effect
};

extern int glitch_dx, glitch_dy;

extern tceffect TCEffects[MAX_TCEFFECTS];
extern int Screen_roots[MAX_TELCOM_SCREENS];

/////////////////////////////////////////////////
// Prototypes

// Initializes the effects
void EfxInit(void);

// Shutdown of effects
void EfxClose(void);

// Creates an instance of an Effect
int EfxCreate(int type, int monitor, int screen, int id = INVALID_EFFECT_ID, bool is_tab_stop = false);

// Destroys all the screens
void EfxDestroyAllScreens(void);

// Destroys the screens within a monitor
void EfxDestroyScreen(int screen);

// Frees up any memory allocated for effect, sets type to EFX_NONE
void EfxFreeEffect(tceffect *tce);

// Initializes an effect based on it's type
void EfxInit(tceffect *tce, bool tab_stop, bool gets_focus);

// Renders a Monitor
void RenderScreen(int screen, tTelComInfo *tsys, float Frametime);

// Renders an effect
void RenderEffect(tceffect *tce, float frametime, int xoff, int yoff, bool ok_to_render);

// Sends an event to an effect
void SendEventToEffect(int effect, int event, int parm1 = 0, int parm2 = 0);

//	returns the efxnum of the button that contains the X,Y on the given monitor/screen, -1 if none
int FindButtonEffectByXY(int x, int y, int screen);

//	Returns the efxnum given an id & screen, -1 if not found
int GetEfxNumFromID(int id, int screen);

//	Pops off an event for processesing, returns -1 if no event available
bool PopEvent(tceffect *tce, tTCEvent *event);
void RenderTextStatic(tceffect *tce, float frametime, int xoff, int yoff, bool ok_to_render);
void RenderTextType(tceffect *tce, float frametime, int xoff, int yoff, bool ok_to_render);
void RenderTextFade(tceffect *tce, float frametime, int xoff, int yoff, bool ok_to_render);
void RenderBmpStatic(tceffect *tce, float frametime, int xoff, int yoff, bool ok_to_render);
void RenderBmpBlur(tceffect *tce, float frametime, int xoff, int yoff, bool ok_to_render);
void RenderBmpScanline(tceffect *tce, float frametime, int xoff, int yoff, bool ok_to_render);
void RenderBmpInvert(tceffect *tce, float frametime, int xoff, int yoff, bool ok_to_render);
void RenderBmpStretch(tceffect *tce, float frametime, int xoff, int yoff, bool ok_to_render);
void RenderBackground(tceffect *tce, float frametime, int xoff, int yoff, bool ok_to_render);
void RenderMovie(tceffect *tce, float frametime, int xoff, int yoff, bool ok_to_render);
void RenderPolyModel(tceffect *tce, float frametime, int xoff, int yoff, bool ok_to_render);
void RenderSound(tceffect *tce, float frametime, int xoff, int yoff, bool ok_to_render);
void RenderButton(tceffect *tce, float frametime, int xoff, int yoff, bool ok_to_render);

bool CreateTextEffect(TCTEXTDESC* desc, const char *text, int monitor, int screen, int id = INVALID_EFFECT_ID);
bool CreateBitmapEffect(TCBMPDESC* desc, int monitor, int screen, int id = INVALID_EFFECT_ID);
bool CreateMovieEffect(TCMOVIEDESC* desc, int monitor, int screen, int id = INVALID_EFFECT_ID);
bool CreateBackgroundEffect(TCBKGDESC* desc, int monitor, int screen, int id = INVALID_EFFECT_ID);
bool CreatePolyModelEffect(TCPOLYDESC* desc, int monitor, int screen, int id = INVALID_EFFECT_ID);
bool CreateSoundEffect(TCSNDDESC* desc, int monitor, int screen, int id = INVALID_EFFECT_ID);
int CreateButtonEffect(TCBUTTONDESC* desc, int monitor, int screen, int id = INVALID_EFFECT_ID);
bool CreateTextStatic(tceffect *tce, const char *text);
bool CreateTextFade(tceffect *tce, const char *text);
bool CreateTextType(tceffect *tce, const char *text);
bool CreateBmpStatic(tceffect *tce, const char *filename);
bool CreateBmpBlur(tceffect *tce, const char *filename);
bool CreateBmpScanline(tceffect *tce, const char *filename);
bool CreateBmpInvert(tceffect *tce, const char *filename);
bool CreateBmpStretch(tceffect *tce, const char *filename);
bool CreateMovie(tceffect *tce, const char *filename);
bool CreateBackground(tceffect *tce);
bool CreateSound(tceffect *tce, const char *filename);

void BltBmpToScreen(int dx, int dy, chunked_bitmap *src_bmp);
void BltBmpToScreen(int dx, int dy, chunked_bitmap *src_bmp, int sx, int sy, int sw, int sh);
// forces a chunk bitmap to be uploaded
void UploadChunk(chunked_bitmap *bmp);

char *format(const char *format, ...);

#endif
