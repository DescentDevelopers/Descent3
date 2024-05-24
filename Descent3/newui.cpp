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

 * $Logfile: /DescentIII/main/newui.cpp $
 * $Revision: 85 $
 * $Date: 4/29/99 3:20a $
 * $Author: Samir $
 *
 *	New UI Enhancements for D3
 *
 * $Log: /DescentIII/main/newui.cpp $
 *
 * 85    4/29/99 3:20a Samir
 * took out Newui_wait_flag.  Not needed.
 *
 * 84    4/28/99 1:55a Samir
 * interface change for UIEdit.
 *
 * 83    4/25/99 3:35a Jeff
 * added a please wait popup dialog when entering telcom
 *
 * 82    4/22/99 7:41p Matt
 * Use new art for old UI.
 *
 * 81    4/21/99 2:15p Samir
 * table file filter fixes.
 *
 * 80    4/20/99 3:32p Kevin
 * made it possible for an edit box to not have a cancel button... used it
 * for choosing team count for server
 *
 * 79    4/20/99 11:46a Samir
 * numerous ui 'feel' fixes.
 *
 * 78    4/18/99 7:55p Samir
 * new progress indicator for delays when loading data.
 *
 * 77    4/15/99 5:28p Samir
 * added advanced messagebox
 *
 * 76    4/06/99 10:44p Samir
 * Fixed residue from changes to UID_OK and UID_CANCEL.
 *
 * 75    3/23/99 9:05p Samir
 * don't do sound system calls in wait dialog. time with main menu, since
 * that's the place where we have to sync sound initialization with menu
 * selection.
 *
 * 74    3/18/99 10:13a Samir
 * wait dialog tied more to menu system.
 *
 * 73    3/04/99 6:09p Samir
 * changed DoWaitMessage to properly center (works for single lines, not
 * sure about multiple lines, but since most messages are single lined, it
 * works for now.)
 *
 * 72    3/03/99 5:13a Jeff
 * added DoWaitMessage()
 *
 * 71    3/03/99 4:16a Samir
 * added fix for DoEditDialog, setting focus on edit gadget by default for
 * all sheets with editboxes (1st one.)
 *
 * 70    3/01/99 6:55p Samir
 * fixed saturation problems with old ui text.
 *
 * 69    2/27/99 8:34p Samir
 * NewuiGameWindow inherits newuiTiledWindow.
 *
 * 68    2/27/99 4:17p Jeff
 * crude fix for text wrapping in DoEdit
 *
 * 67    2/21/99 6:39p Samir
 * new edit box.
 *
 * 66    2/16/99 12:06p Samir
 * added new ui api.
 *
 * 65    2/07/99 1:17a Jeff
 * peppered UI dialogs that were missing NEWUIRES_FORCEQUIT to handle it
 *
 * 64    1/28/99 6:49p Samir
 * frame limit ui system.
 *
 * 63    12/09/98 4:17p Samir
 * messageboxes use new art.
 *
 * 62    11/19/98 5:40p Kevin
 * Demo system
 *
 * 61    11/02/98 6:00p Jeff
 * fixed MessageBox to use new UI artwork...had to redo the way it
 * calculates button centers
 *
 * 60    10/30/98 1:34p Samir
 * better sizable UIwindow.
 *
 * 59    10/23/98 12:53p Samir
 * bail from DoUIFrameWithoutInput for multiplayer too.
 *
 * 58    10/18/98 2:59p Jason
 * fixes for beta4
 *
 * 57    10/16/98 4:20p Samir
 * hotspots replaced by keys Y or N for yesno messagebox.
 *
 * 56    10/14/98 11:24p Jeff
 * fixed DoMessageBox (so keys work in it)
 *
 * 55    10/12/98 10:23a Samir
 * made UI_frame_result non-static.
 *
 * 54    10/11/98 3:01a Jeff
 * made some dialogs multiplayer friendly with multi_bail_ui_menu
 *
 * 53    10/08/98 7:31p Samir
 * moved multi_bail_menu flag to gamesequence.cpp.
 *
 * 52    10/07/98 6:29p Samir
 * bail if multiplayer game ended in ui.
 *
 * 51    10/07/98 11:24a Jeff
 * fixed text centering function
 *
 * 50    10/06/98 5:34p Jeff
 * various UI changes/improvements
 *
 * 49    9/23/98 6:19p Jeff
 * finished up (hopefully) updating the config/ui dialogs to meet our
 * standard.  Keyboard/joystick config still needs some work
 *
 * 48    9/23/98 3:07p Jeff
 * updated the colors and various other items of config and UI
 *
 * 47    9/22/98 3:55p Samir
 * moved simple callback to NewUI system.
 *
 * 46    9/14/98 11:22a Samir
 * enhanced editboxes
 *
 * 45    9/10/98 6:33p Samir
 * messageboxes should use up to 3 lines of text without a problem.
 *
 * 44    9/02/98 6:50p Samir
 * added new combo box.
 *
 * 43    9/01/98 4:12p Samir
 * added screenshot caparbilities.
 *
 * 42    9/01/98 12:29p Jeff
 * error handling in DoMessageBox (handles extra long strings)
 *
 * 41    8/31/98 12:38p Samir
 * took out hotspots in messageboxes.  functionality implented internally.
 *
 * 40    8/27/98 5:03p Kevin
 * Prettied up multiplayer screens and fixed some bugs.
 *
 * 39    8/27/98 2:51p Samir
 * you must specify UIF_BORDER to print the border for a NewUIWindow.
 *
 * 38    8/24/98 3:12p Samir
 * fixed listbox text clipping.
 *
 * 37    8/20/98 6:46p Samir
 * added DoEditDialog.
 *
 * 36    8/14/98 7:37p Matt
 * Since PSFILENAME_LEN is now considered to be the max length of the
 * string itself (not counting the terminating null), buffers for file
 * name need to be PSFILENAME_LEN+1 bytes long.
 *
 * 35    7/22/98 2:38p Jeff
 * added Y and N keys for the DoMessageBox
 *
 * 34    6/25/98 12:34p Jeff
 * added enter and esc key to DoMessageBox
 *
 * 33    6/24/98 5:16p Matt
 * Don't reset screen mode in DoMessageBox(), so we still see the game
 * screen when the pause message is up.
 *
 * 32    6/22/98 2:34p Samir
 * added ability to use a bitmap as background for NewUIWindows.
 *
 * 31    6/16/98 10:54a Jeff
 *
 * 30    6/01/98 10:57a Jeff
 * (Samir) Fixed bug if a file couldn't be loaded
 *
 * 29    5/26/98 8:15p Samir
 * implemented UIFileDialog's ONDestroy function.
 *
 * 28    5/25/98 8:18p Samir
 * File dialogs implemented.
 *
 * 27    5/24/98 2:57a Jeff
 * updated DoMessageBox to use NewUIButton
 *
 * 26    5/19/98 6:23p Samir
 * added NewUIButton
 *
 * 25    5/08/98 12:55p Samir
 * improved ui handling when switch screen modes.
 *
 * 24    4/29/98 12:55p Samir
 * added titile bar to NewUIWindow.
 *
 * 23    4/18/98 2:09a Samir
 * error checking.
 *
 * 22    4/13/98 7:01p Samir
 * added snazzy listbox and edit box art.
 *
 * 21    4/10/98 7:51p Samir
 * added ui_Flush
 *
 * 20    4/02/98 12:39p Samir
 * don't clear screen in NULL callback.
 *
 * 19    4/02/98 11:12a Samir
 * Always set screen mode to menu for message boxes, and clear screen if
 * UICallback if 0.
 *
 * 18    3/24/98 10:45a Samir
 * Added GetUICallback.
 *
 * 17    3/13/98 12:00p Jeff
 * Fixed up DoMessageBox so the text position is correct, it prints a
 * window title, and you can specify text color
 *
 * 16    3/11/98 2:35p Samir
 * New message box implemented.
 *
 * 15    3/10/98 7:26p Samir
 * Added new slider.
 *
 * 14    3/10/98 12:49p Samir
 * Bitmapped dialog  supported.
 *
 * 13    3/05/98 6:38p Samir
 * Use UI_FONT now.
 *
 * 12    3/03/98 12:09p Samir
 * Clear out keyboard buffer after UI is done.
 *
 * 11    3/02/98 5:53p Samir
 * Default value for alpha of windows is set.
 *
 * 10    2/15/98 7:07p Samir
 * Updated NewUIWindow to UIWindow's changes.
 *
 * 9     2/13/98 6:37p Samir
 * Added DoUIFrameWithoutInput.
 *
 * 8     1/30/98 7:33p Samir
 * Took out 'ok' button.
 *
 * 7     1/30/98 7:04p Samir
 * Added the first NewUIWindow class.
 *
 * 6     1/26/98 2:15p Samir
 * Message box params are char*.
 *
 * 5     1/18/98 4:22p Samir
 * Implemented new UIItem system.
 *
 * 4     1/13/98 6:29p Samir
 * Moved Large Bitmap code from Menu.cpp to newui.cpp.
 *
 * 3     1/08/98 12:18p Samir
 * New menu interface.
 *
 * 2     1/05/98 10:56a Samir
 * New menu stuff.
 *
 * 1     1/02/98 2:09p Samir
 * Initial revision.
 *
 *
 * $NoKeywords: $
 */

#include "newui.h"
#include "game.h"
#include "descent.h"
#include "renderer.h"
#include "3d.h"
#include "gamefont.h"
#include "bitmap.h"
#include "ddio.h"
#include "stringtable.h"
#include "textaux.h"
#include "newui_core.h"
#include "hlsoundlib.h"
#include "dedicated_server.h"

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#define MSGBOX_HEIGHT msgbox.H()
#define BTN_WIDTH 96
#define NEWUI_FRAMETIME (1.0f / 25.0f)

//////////////////////////////////////////////////////////////////////////////
//	Declarations

static struct {
  int bmp;
  chunked_bitmap chunk;
} NewUI_bitmap_list[NEWUIBMP_NUM];

static bool NewUI_init = false;

//	closes New UI system
void NewUIClose();

//////////////////////////////////////////////////////////////////////////////
//	Functions

// returns x coordinates to use to determine center position of text items for in a window/dialog
//	width	- total width of dialog/window
//	gap		- size (in pixels) of the gap you want between the left and right item
//	left_item_width - the width (in pixels) of the left item
//	right_item_width - the widht (in pixels) of the right item
//	lx		- will contain the left item's x coord
//	rx		- will contain the right item's x coord
void GetCenteredTextPos(int width, int gap, int left_item_width, int right_item_width, int *lx, int *rx) {
  int center_x = width / 2;

  // the left side of the left item should be same distance as the right side of the right item is from the middle

  int width_of_items = gap + left_item_width + right_item_width;
  int center_of_items = width_of_items / 2;

  int left_gap = 0, right_gap = 0;

  left_gap = center_of_items - left_item_width;
  right_gap = center_of_items - right_item_width;

  *lx = center_x - (left_gap + left_item_width);
  *rx = center_x + right_gap;
}

// returns x coordinates to use to determine center position of text items for in a window/dialog
//	width	- total width of dialog/window
//	gap		- size (in pixels) of the gap you want between the left and right item
//	left_item_width - the width (in pixels) of the left item
//	middle_item_width - the width (in pixels) of the middle item
//	right_item_width - the width (in pixels) of the right item
//	lx		- will contain the left item's x coord
//	rx		- will contain the right item's x coord
//	mx		- will contain the middle item's x coord
void GetCenteredTextPos(int width, int gap, int left_item_width, int middle_item_width, int right_item_width, int *lx,
                        int *mx, int *rx) {
  int center_x = width / 2;
  int width_of_items = 2 * gap + left_item_width + middle_item_width + right_item_width;
  int center_of_items = width_of_items / 2;

  *lx = center_x - (width_of_items / 2);
  *mx = (*lx) + gap + left_item_width;
  *rx = (*mx) + gap + middle_item_width;
}

//	load in D3 user interface resources
void NewUIInit() {
  int i;

  if (NewUI_init) {
    NewUIClose();
  } else {
    NewUI_init = true;
    atexit(NewUIClose);
  }

  for (i = 0; i < NEWUIBMP_NUM; i++) {
    NewUI_bitmap_list[i].bmp = -3;
  }

  //	load in bitmap handles here.
  int bm_handle;

  NewUI_bitmap_list[NEWUIBMP_DIALOG_CORNER1].bmp = bm_AllocLoadFileBitmap("d3dlg_nw.ogf", 0);
  NewUI_bitmap_list[NEWUIBMP_DIALOG_CORNER2].bmp = bm_AllocLoadFileBitmap("d3dlg_ne.ogf", 0);
  NewUI_bitmap_list[NEWUIBMP_DIALOG_CORNER3].bmp = bm_AllocLoadFileBitmap("d3dlg_se.ogf", 0);
  NewUI_bitmap_list[NEWUIBMP_DIALOG_CORNER4].bmp = bm_AllocLoadFileBitmap("d3dlg_sw.ogf", 0);
  NewUI_bitmap_list[NEWUIBMP_DIALOG_HORIZTOP].bmp = bm_AllocLoadFileBitmap("d3dlg_n.ogf", 0);
  NewUI_bitmap_list[NEWUIBMP_DIALOG_VERTRIGHT].bmp = bm_AllocLoadFileBitmap("d3dlg_e.ogf", 0);
  NewUI_bitmap_list[NEWUIBMP_DIALOG_HORIZBOT].bmp = bm_AllocLoadFileBitmap("d3dlg_s.ogf", 0);
  NewUI_bitmap_list[NEWUIBMP_DIALOG_VERTLEFT].bmp = bm_AllocLoadFileBitmap("d3dlg_w.ogf", 0);
  NewUI_bitmap_list[NEWUIBMP_DIALOG_BACK].bmp = bm_AllocLoadFileBitmap("d3dlg_bk.ogf", 0);
  //	NewUI_bitmap_list[NEWUIBMP_DIALOG_PANEL].bmp= bm_AllocLoadFileBitmap("d3dlg_tb.ogf",0);
  NewUI_bitmap_list[NEWUIBMP_MSGBOX_LEFT].bmp = bm_AllocLoadFileBitmap("d3mbox_l.ogf", 0);
  NewUI_bitmap_list[NEWUIBMP_MSGBOX_RIGHT].bmp = bm_AllocLoadFileBitmap("d3mbox_r.ogf", 0);
  NewUI_bitmap_list[NEWUIBMP_MSGBOX_CENTER].bmp = bm_AllocLoadFileBitmap("d3mbox_c.ogf", 0);
  NewUI_bitmap_list[NEWUIBMP_LIST_CORNER1].bmp = bm_AllocLoadFileBitmap("lbTopLeft32.ogf", 0);
  NewUI_bitmap_list[NEWUIBMP_LIST_CORNER2].bmp = bm_AllocLoadFileBitmap("lbTopRight32.ogf", 0);
  NewUI_bitmap_list[NEWUIBMP_LIST_CORNER3].bmp = bm_AllocLoadFileBitmap("lbBottomRight32.ogf", 0);
  NewUI_bitmap_list[NEWUIBMP_LIST_CORNER4].bmp = bm_AllocLoadFileBitmap("lbBottomLeft32.ogf", 0);
  NewUI_bitmap_list[NEWUIBMP_LIST_HORIZTOP].bmp = bm_AllocLoadFileBitmap("lbTop32.ogf", 0);
  NewUI_bitmap_list[NEWUIBMP_LIST_VERTRIGHT].bmp = bm_AllocLoadFileBitmap("lbRight32.ogf", 0);
  NewUI_bitmap_list[NEWUIBMP_LIST_HORIZBOT].bmp = bm_AllocLoadFileBitmap("lbBottom32.ogf", 0);
  NewUI_bitmap_list[NEWUIBMP_LIST_VERTLEFT].bmp = bm_AllocLoadFileBitmap("lbLeft32.ogf", 0);
  NewUI_bitmap_list[NEWUIBMP_LIST_BACK].bmp = bm_AllocLoadFileBitmap("d3_lb_bk.ogf", 0);
  NewUI_bitmap_list[NEWUIBMP_EDIT_LEFT].bmp = bm_AllocLoadFileBitmap("tbLeft.ogf", 0);
  NewUI_bitmap_list[NEWUIBMP_EDIT_RIGHT].bmp = bm_AllocLoadFileBitmap("tbRight.ogf", 0);
  NewUI_bitmap_list[NEWUIBMP_EDIT_CENTER].bmp = bm_AllocLoadFileBitmap("tbCenter.ogf", 0);
  NewUI_bitmap_list[NEWUIBMP_BTN_UP].bmp = bm_AllocLoadFileBitmap("d3_btn_up.ogf", 0);
  NewUI_bitmap_list[NEWUIBMP_BTN_DOWN].bmp = bm_AllocLoadFileBitmap("d3_btn_down.ogf", 0);
  NewUI_bitmap_list[NEWUIBMP_BTN_HILITE].bmp = bm_AllocLoadFileBitmap("d3_btn_hilite.ogf", 0);
  NewUI_bitmap_list[NEWUIBMP_BTN_UP_L].bmp = bm_AllocLoadFileBitmap("d3_btn_up_l.ogf", 0);
  NewUI_bitmap_list[NEWUIBMP_BTN_DOWN_L].bmp = bm_AllocLoadFileBitmap("d3_btn_down_l.ogf", 0);
  NewUI_bitmap_list[NEWUIBMP_BTN_HILITE_L].bmp = bm_AllocLoadFileBitmap("d3_btn_hilite_l.ogf", 0);
  NewUI_bitmap_list[NEWUIBMP_BTN_UP_R].bmp = bm_AllocLoadFileBitmap("d3_btn_up_r.ogf", 0);
  NewUI_bitmap_list[NEWUIBMP_BTN_DOWN_R].bmp = bm_AllocLoadFileBitmap("d3_btn_down_r.ogf", 0);
  NewUI_bitmap_list[NEWUIBMP_BTN_HILITE_R].bmp = bm_AllocLoadFileBitmap("d3_btn_hilite_r.ogf", 0);

  bm_handle = bm_AllocLoadFileBitmap("d3slider.ogf", 0);
  if (bm_handle > 0) {
    NewUI_bitmap_list[NEWUIBMP_SLIDER_BUTTON].bmp = -2;
    bm_CreateChunkedBitmap(bm_handle, &NewUI_bitmap_list[NEWUIBMP_SLIDER_BUTTON].chunk);
    bm_FreeBitmap(bm_handle);
  } else {
    NewUI_bitmap_list[NEWUIBMP_SLIDER_BUTTON].bmp = -1;
  }
  bm_handle = bm_AllocLoadFileBitmap("d3sliderbar.ogf", 0);
  if (bm_handle > 0) {
    NewUI_bitmap_list[NEWUIBMP_SLIDER_BAR].bmp = -2;
    bm_CreateChunkedBitmap(bm_handle, &NewUI_bitmap_list[NEWUIBMP_SLIDER_BAR].chunk);
    bm_FreeBitmap(bm_handle);
  } else {
    NewUI_bitmap_list[NEWUIBMP_SLIDER_BAR].bmp = -1;
  }

  for (i = 0; i < NEWUIBMP_NUM; i++) {
    if (NewUI_bitmap_list[i].bmp == 0 || NewUI_bitmap_list[i].bmp == -1)
      Error("Unable to load new user interface bitmap #%d.\n", i);
  }

  UITextItem::SetSaturationFactor(0);

  //	new ui init 2
  newuiCore_Init();
}

//	closes New UI system
void NewUIClose() {
  //	new ui init 2
  newuiCore_Close();

  for (int i = 0; i < NEWUIBMP_NUM; i++) {
    if (NewUI_bitmap_list[i].bmp >= BAD_BITMAP_HANDLE) {
      bm_FreeBitmap(NewUI_bitmap_list[i].bmp);
      NewUI_bitmap_list[i].bmp = -1;
    } else if (NewUI_bitmap_list[i].bmp == -2) {
      bm_DestroyChunkedBitmap(&NewUI_bitmap_list[i].chunk);
      NewUI_bitmap_list[i].bmp = -1;
    }
  }
}

// Displays a 'temporary' dialog with a message
// for example:
//	...
//	DoWaitMessage(true,"Please Wait...");
//	... //some code
//	DoWaitMessage(false);
void DoWaitMessage(bool enable, const char *message) {
  static bool opened = false;
  static uint8_t cur_pos = 0;
  float curpos_f;

  //	if (1) return;

  if (Dedicated_server)
    return;
  if (!enable && !opened)
    return;
  if (GetScreenMode() != SM_MENU) {
    opened = false;
    return;
  }
  if (enable && !opened) {
    opened = true;
    cur_pos = -1;
  }
  if (!enable && opened) {
    opened = false;
    return;
  }

  // render a ui frame
  DoUIFrame();
  cur_pos++;
  if (cur_pos > 10)
    cur_pos = 0;
  curpos_f = (cur_pos / 10.0f);

  // print our downloading data message.
  g3Point *pntlist[4], points[4];
  int i, rx, ry, rw, rh;

  StartFrame(0, 0, Max_window_w, Max_window_h);
  grtext_SetFont(MONITOR9_NEWUI_FONT);
  rw = Max_window_w;
  rh = grfont_GetHeight(MONITOR9_NEWUI_FONT) + 8;
  rx = 0;
  ry = Max_window_h - rh;
  points[0].p3_sx = rx;
  points[0].p3_sy = ry;
  points[1].p3_sx = rx + rw;
  points[1].p3_sy = ry;
  points[2].p3_sx = rx + rw;
  points[2].p3_sy = ry + rh;
  points[3].p3_sx = rx;
  points[3].p3_sy = ry + rh;

  for (i = 0; i < 4; i++) {
    points[i].p3_z = 0;
    points[i].p3_flags = PF_PROJECTED;
    pntlist[i] = &points[i];
  }

  rend_SetZBufferState(0);
  rend_SetTextureType(TT_FLAT);
  rend_SetAlphaType(AT_CONSTANT);
  rend_SetLighting(LS_NONE);
  rend_SetFlatColor(GR_BLACK);
  rend_SetAlphaValue(230);
  rend_DrawPolygon2D(0, pntlist, 4);

  if (cur_pos > 0) {
    points[0].p3_sx = rx;
    points[0].p3_sy = ry + rh - (rh / 2) + 2;
    points[1].p3_sx = rx + ((float)rw * curpos_f);
    points[1].p3_sy = ry + rh - (rh / 2) + 2;
    points[2].p3_sx = rx + ((float)rw * curpos_f);
    points[2].p3_sy = ry + rh;
    points[3].p3_sx = rx;
    points[3].p3_sy = ry + rh;

    rend_SetFlatColor(GR_RGB(255, 0, 0));
    rend_SetAlphaValue(230);
    rend_DrawPolygon2D(0, pntlist, 4);
  }

  grtext_SetAlpha(255);
  grtext_SetColor(NEWUI_MONITORFONT_COLOR);
  if (message) {
    grtext_Puts(0, ry, message);
  } else {
    grtext_Puts(0, ry, TXT_INITDATA);
  }
  grtext_Flush();

  EndFrame();
  rend_Flip();
}

void DoWaitPopup(bool enable, const char *message) {
#define FDWM_HEIGHT 128
#define FDWM_WIDTH 256

  static newuiTiledWindow *msgbox = NULL;
  static UIText text;
  static bool opened = false;
  newuiSheet *sheet;

  if (opened == enable) {
    Int3();
  }

  opened = enable;

  if (enable) {
    msgbox = new newuiTiledWindow;

    if (msgbox) {

      msgbox->Create(NULL, 0, 0, FDWM_WIDTH, FDWM_HEIGHT);

      sheet = msgbox->GetSheet();
      if (message) {
        char msg[512];
        int w, h;
        textaux_WordWrap(message, msg, FDWM_WIDTH, MONITOR9_NEWUI_FONT);
        grtext_SetFont(MONITOR9_NEWUI_FONT);
        w = grtext_GetTextLineWidth(msg);
        h = grtext_GetTextHeight(msg);
        UITextItem item{MONITOR9_NEWUI_FONT, msg, NEWUI_MONITORFONT_COLOR};
        text.Create(msgbox, &item, (msgbox->W() - w) / 2, (msgbox->H() - h) / 2);

        //				sheet->NewGroup(NULL, , 25);
        //				sheet->AddText(msg);
      }

      msgbox->Open();
      //			sheet->Realize();

      // fake a DoUI
      DoUIFrame();
      rend_Flip();
    }
  } else {

    if (msgbox) {
      msgbox->Close();
      msgbox->Destroy();
      delete msgbox;
    }
    msgbox = NULL;
  }
}

//	puts up a message box with a title and message.
int DoMessageBox(const char *title, const char *msg, int type, ddgr_color title_color, ddgr_color text_color) {
  newuiMessageBox mbox;
  newuiSheet *sheet;

  int old_screen_mode, res;
  char msgbuf[256];

  //	force menu mode
  old_screen_mode = GetScreenMode();

  textaux_WordWrap(msg, msgbuf, NEWUI_MSGBOX_SHEET_W, MONITOR9_NEWUI_FONT);

  // make message box
  mbox.Create(title, type);

  sheet = mbox.GetSheet();
  sheet->NewGroup(NULL, 0, 0);
  sheet->AddText(msgbuf);

  mbox.Open();
  res = mbox.DoUI();
  mbox.Close();

  mbox.Destroy();

  //	restore screen
  SetScreenMode(old_screen_mode);

  //	return correct value.
  return ((res == UID_OK) ? 1 : 0);
}

//	puts up a message box with a title and message.
//	define text for buttons, btn0_title, btn1_title, ..., until NULL.
// also define hotkey for button immediately after title is defined.
//
//	DoMessageBoxAdvanced("Title", "Hi", "Abort", KEY_A, "Retry", KEY_R, "Cancel", KEY_ESC, NULL)
//
// if button with btn0_title pressed, returns 0, btn1_title, returns 1, etc.
// safe for up to three buttons.
int DoMessageBoxAdvanced(const char *title, const char *msg, const char *btn0_title, int key0, ...) {
  newuiMessageBox mbox;
  newuiSheet *sheet;
  va_list args;
  const char *curbtntitle;
  int i;

  int old_screen_mode, res;
  char msgbuf[256];

  //	force menu mode
  old_screen_mode = GetScreenMode();

  textaux_WordWrap(msg, msgbuf, NEWUI_MSGBOX_SHEET_W, MONITOR9_NEWUI_FONT);

  // make message box
  mbox.Create(title, MSGBOX_NULL);
  i = 0;
  if (btn0_title) {
    mbox.AddButton(btn0_title, i, key0);
    i++;
  }

  // go through all titles.
  va_start(args, key0);
  while ((curbtntitle = va_arg(args, char *)) != NULL) {
    int key = va_arg(args, int);

    mbox.AddButton(curbtntitle, i, key);
    i++;
    if (i == NEWUI_MSGBOX_MAXBTNS)
      break;
  }

  va_end(args);

  sheet = mbox.GetSheet();
  sheet->NewGroup(NULL, 0, 0);
  sheet->AddText(msgbuf);

  mbox.Open();
  res = mbox.DoUI();
  mbox.Close();

  mbox.Destroy();

  //	restore screen
  SetScreenMode(old_screen_mode);

  //	return correct value.
  return res;
}

bool DoEditDialog(const char *title, char *buffer, int buflen, bool showcancel) {
  newuiMessageBox mbox;
  newuiSheet *sheet;

  int old_screen_mode, res;
  char msgbuf[256];
  char *edit;

  //	force menu mode
  old_screen_mode = GetScreenMode();

  textaux_WordWrap(title, msgbuf, NEWUI_MSGBOX_SHEET_W, MONITOR9_NEWUI_FONT);

  // make message box
  mbox.Create("", showcancel ? MSGBOX_OKCANCEL : MSGBOX_OK);

  sheet = mbox.GetSheet();
  sheet->NewGroup(NULL, 0, 25);
  edit = sheet->AddEditBox("", buflen, 0, DEFAULT_NEWUID, 0, true);
  strcpy(edit, buffer);

  sheet->NewGroup(NULL, 0, 0);
  sheet->AddText(msgbuf);

  mbox.Open();
  res = mbox.DoUI();
  mbox.Close();

  if (res != UID_CANCEL && res != NEWUIRES_FORCEQUIT) {
    if (strcmp(edit, NEWUI_EDIT_CANCELED_STR) != 0) {
      strcpy(buffer, edit);
      mprintf(0, "editdialog=%s\n", buffer);
    } else {
      res = UID_CANCEL;
    }
  }

  mbox.Destroy();

  //	restore screen
  SetScreenMode(old_screen_mode);

  //	return correct value.
  return ((res != UID_CANCEL && res != NEWUIRES_FORCEQUIT) ? true : false);
}

// puts up a file selector box
// Parameters:	max_filename_len - the max length for the filename. filebuf have a length of at least max_filename_len+1
bool DoFileDialog(const char *title, const char *search_path, const char *ext, char *filebuf,
                  unsigned max_filename_len) {
  NewUIFileDialog dlg;
  int w = 256, h = 256;
  int x = Game_window_w / 2 - w / 2;
  int y = Game_window_h / 2 - h / 2;

  dlg.Create(title, x, y, w, h, search_path, ext);

  if (dlg.DoModal()) {
    if (strlen(dlg.GetFilename()) > max_filename_len)
      Error("Runtime error. NewUI system DoFileDialog: (%d,%d)", strlen(dlg.GetFilename()), max_filename_len);
    strcpy(filebuf, dlg.GetFilename());
    return true;
  }

  filebuf[0] = 0;
  return false;
}

//	shows or hides windows
void OpenUIWindow(UIWindow *wnd) {
  ASSERT(wnd);
  wnd->Open();
}

void CloseUIWindow(UIWindow *wnd) {
  ASSERT(wnd);
  wnd->Close();
}

///////////////////////////////////////////////////////////////////////////////
//	large bitmap library

int LoadLargeBitmap(const char *filename, tLargeBitmap *bmp) {
  chunked_bitmap chunk;
  int bm_handle = bm_AllocLoadFileBitmap(IGNORE_TABLE(filename), 0);
  if (bm_handle == -1) {
    bmp->bm_array = NULL;
    bmp->bmps_w = 0;
    bmp->bmps_h = 0;
    return 0;
  }

  bm_CreateChunkedBitmap(bm_handle, &chunk);

  if (bm_handle > BAD_BITMAP_HANDLE)
    bm_FreeBitmap(bm_handle);

  bmp->bmps_w = chunk.w;
  bmp->bmps_h = chunk.h;
  bmp->bm_array = chunk.bm_array;

  return 1;
}

void FreeLargeBitmap(tLargeBitmap *bmp) {
  chunked_bitmap chunk;

  chunk.w = bmp->bmps_w;
  chunk.h = bmp->bmps_h;
  chunk.bm_array = bmp->bm_array;
  bm_DestroyChunkedBitmap(&chunk);
}

void DrawLargeBitmap(tLargeBitmap *bmp, int x, int y, float alpha) {
  chunked_bitmap chunk;

  chunk.w = bmp->bmps_w;
  chunk.h = bmp->bmps_h;
  chunk.bm_array = bmp->bm_array;
  rend_DrawChunkedBitmap(&chunk, x, y, alpha * 255);
}

///////////////////////////////////////////////////////////////////////////////
//	Listbox procedure.

uint8_t NewUIWindow_alpha = 192;

NewUIWindow::NewUIWindow() : UIWindow() {}

//	initializes the window
void NewUIWindow::Create(int x, int y, int w, int h, int flags) {
  UIWindow::Create(x, y, w, h, flags);
  UIPrimativeItem item{GR_BLACK, NewUIWindow_alpha};
  UIWindow::SetBackItem(&item);
  memset(&m_Chunk, 0, sizeof(m_Chunk));
}

//	settings and state info

//	ui system overridables
// overridable draws the window background before gadgets
void NewUIWindow::OnDraw() {
  UIWindow::OnDraw();

  if (m_Flags & UIF_BORDER) {
    ui_DrawBox(GR_DARKGRAY, 0, 0, m_W, m_H);
    ui_DrawBox(GR_LIGHTGRAY, 2, 2, m_W - 2, m_H - 2);
  }
}

void NewUIWindow::OnDestroy() { bm_DestroyChunkedBitmap(&m_Chunk); }

void NewUIWindow::LoadBackgroundImage(const char *image_name) {
  UIBitmapItem back_bm;

  bm_DestroyChunkedBitmap(&m_Chunk);
  memset(&m_Chunk, 0, sizeof(m_Chunk));

  if (image_name) {
    int bm_handle = bm_AllocLoadFileBitmap(IGNORE_TABLE(image_name), 0);

    if (bm_handle <= BAD_BITMAP_HANDLE) {
      Error("Unable to load image %s for background.", image_name);
    }

    //	takes a large static bitmap and breaks it into smaller managable bitmaps
    if (!bm_CreateChunkedBitmap(bm_handle, &m_Chunk)) {
      Error("Failed to chunk background bitmap %s.", image_name);
    }

    back_bm.set_chunked_bitmap(&m_Chunk);
    UIWindow::SetBackItem(&back_bm);
    bm_FreeBitmap(bm_handle);
  }
}

///////////////////////////////////////////////////////////////////////////////
//	NewUIGameWindow implementation.

NewUIGameWindow::NewUIGameWindow() {}

//	initializes the window
void NewUIGameWindow::Create(int x, int y, int w, int h, int flags) {
  newuiTiledWindow::Create(NULL, x, y, w, h, flags);
}

///////////////////////////////////////////////////////////////////////////////
//	NewUIMessageBox implementation.

NewUIMessageBox::NewUIMessageBox() : NewUIGameWindow() {}

//	initializes the window
void NewUIMessageBox::Create(int x, int y, int w, int flags) {
  NewUIGameWindow::Create(x, y, w, 128, flags | UIF_PROCESS_ALL);
}

//	settings and state info

///////////////////////////////////////////////////////////////////////////////
//	NewUISlider implementation.

//	settings

void NewUISlider::Create(UIWindow *parent, int id, int x, int y, int flags) {
  m_Slider.set_chunked_bitmap(&NewUI_bitmap_list[NEWUIBMP_SLIDER_BUTTON].chunk);
  m_SliderBar.set_chunked_bitmap(&NewUI_bitmap_list[NEWUIBMP_SLIDER_BAR].chunk);

  UISlider::Create(parent, id, x, y, 5, 5, flags);

  UISlider::SetSliderSwitchItem(&m_Slider);
  UISlider::SetSliderItem(&m_SliderBar);
}

///////////////////////////////////////////////////////////////////////////////
//	NewUIEdit implementation.

//	settings

void NewUIEdit::Create(UIWindow *parent, int id, int x, int y, int w, int h, int flags) {
  m_EditLeft.set_bitmap(NewUI_bitmap_list[NEWUIBMP_EDIT_LEFT].bmp);
  m_EditCen.set_bitmap(NewUI_bitmap_list[NEWUIBMP_EDIT_CENTER].bmp);
  m_EditRight.set_bitmap(NewUI_bitmap_list[NEWUIBMP_EDIT_RIGHT].bmp);

  //	adjust w and height acoordingly with bitmap background width and height.
  int bk_width = (w - m_EditLeft.width() - m_EditRight.width()) / m_EditCen.width();
  int bk_height = 1;

  if (bk_width < 0)
    bk_width = 0;
  else {
    if ((w - m_EditLeft.width() - m_EditRight.width()) % m_EditCen.width())
      bk_width++;
  }

  w = (bk_width * m_EditCen.width()) + m_EditLeft.width() + m_EditRight.width();
  h = (bk_height * m_EditLeft.height());
  m_TileWidth = bk_width;

  UIEdit::Create(parent, id, x, y, w, h, flags);
  UIEdit::SetTextBounds(m_EditLeft.width() / 2, m_W - m_EditRight.height() / 2, 0);
}

void NewUIEdit::OnDraw() {
  //	draw tiles.
  int x, tx;

  tx = x = 0;

  // top row.
  m_EditLeft.draw(x, 0);
  x += m_EditRight.width();
  while (tx++ < m_TileWidth) {
    m_EditCen.draw(x, 0);
    x += m_EditCen.width();
  }
  m_EditRight.draw(x, 0);

  DrawText();
}

///////////////////////////////////////////////////////////////////////////////
//	NewUIListBox Implementation

void NewUIListBox::Create(UIWindow *parent, int id, int x, int y, int w, int h, int flags) {
  m_BackNW.set_bitmap(NewUI_bitmap_list[NEWUIBMP_LIST_CORNER1].bmp);
  m_BackN.set_bitmap(NewUI_bitmap_list[NEWUIBMP_LIST_HORIZTOP].bmp);
  m_BackNE.set_bitmap(NewUI_bitmap_list[NEWUIBMP_LIST_CORNER2].bmp);
  m_BackE.set_bitmap(NewUI_bitmap_list[NEWUIBMP_LIST_VERTRIGHT].bmp);
  m_BackSE.set_bitmap(NewUI_bitmap_list[NEWUIBMP_LIST_CORNER3].bmp);
  m_BackS.set_bitmap(NewUI_bitmap_list[NEWUIBMP_LIST_HORIZBOT].bmp);
  m_BackSW.set_bitmap(NewUI_bitmap_list[NEWUIBMP_LIST_CORNER4].bmp);
  m_BackW.set_bitmap(NewUI_bitmap_list[NEWUIBMP_LIST_VERTLEFT].bmp);
  m_Back.set_bitmap(NewUI_bitmap_list[NEWUIBMP_LIST_BACK].bmp);

  //	adjust w and height acoordingly with bitmap background width and height.
  int bk_width = (w - m_BackNW.width() - m_BackNE.width()) / m_BackN.width();
  int bk_height = (h - m_BackNW.height() - m_BackSW.height()) / m_BackW.height();

  if (bk_width < 0)
    bk_width = 0;
  else {
    if ((w - m_BackNW.width() - m_BackNE.width()) % m_BackN.width())
      bk_width++;
  }

  if (bk_height < 0)
    bk_height = 0;
  else {
    if ((h - m_BackNW.height() - m_BackSW.height()) % m_BackW.height())
      bk_height++;
  }

  w = (bk_width * m_BackN.width()) + m_BackNW.width() + m_BackNE.width();
  h = (bk_height * m_BackW.height()) + m_BackNW.height() + m_BackSW.height();
  m_TileWidth = bk_width;
  m_TileHeight = bk_height;

  UIListBox::Create(parent, id, x, y, w, h, flags);

  // redefine special paramaeters for the new listbox
  m_TextOffX = m_BackW.width() / 2;
  m_TextOffY = m_BackN.height() / 2;
  m_CX = m_TextOffX;
  m_CY = m_TextOffY;
  m_CX2 = m_W - m_TextOffX;
  m_CY2 = m_H - m_TextOffY;
}

void NewUIListBox::OnDraw() {
  int x, y, tx, ty;

  //	draw tiles.
  tx = x = 0;
  ty = y = 0;

  // top row.
  m_BackNW.draw(x, y);
  x += m_BackNW.width();
  while (tx++ < m_TileWidth) {
    m_BackN.draw(x, y);
    x += m_BackN.width();
  }
  m_BackNE.draw(x, y);

  x = 0;
  y += m_BackNW.height();

  //	mid rows.
  while (ty++ < m_TileHeight) {
    m_BackW.draw(x, y);
    x += m_BackW.width();
    tx = 0;
    while (tx++ < m_TileWidth) {
      m_Back.draw(x, y);
      x += m_Back.width();
    }
    m_BackE.draw(x, y);
    x = 0;
    y += m_BackW.height();
  }

  //	bottom row.
  m_BackSW.draw(x, y);
  x += m_BackSW.width();
  tx = 0;
  while (tx++ < m_TileWidth) {
    m_BackS.draw(x, y);
    x += m_BackS.width();
  }
  m_BackSE.draw(x, y);

  //	draw text!
  UIListBox::OnDraw();
}

//	NewUIComboBox
//		displays a listbox just one item.

void NewUIComboBox::Create(UIWindow *parent, int id, int x, int y, int w, int flags) {
  int h;

  m_CmbLeft.set_bitmap(NewUI_bitmap_list[NEWUIBMP_EDIT_LEFT].bmp);
  m_CmbCen.set_bitmap(NewUI_bitmap_list[NEWUIBMP_EDIT_CENTER].bmp);
  m_CmbRight.set_bitmap(NewUI_bitmap_list[NEWUIBMP_EDIT_RIGHT].bmp);

  //	adjust w and height acoordingly with bitmap background width and height.
  int bk_width = (w - m_CmbLeft.width() - m_CmbRight.width()) / m_CmbCen.width();
  int bk_height = 1;

  if (bk_width < 0)
    bk_width = 0;
  else {
    if ((w - m_CmbLeft.width() - m_CmbRight.width()) % m_CmbCen.width())
      bk_width++;
  }

  w = (bk_width * m_CmbCen.width()) + m_CmbLeft.width() + m_CmbRight.width();
  h = (bk_height * m_CmbLeft.height());
  m_TileWidth = bk_width;

  UIComboBox::Create(parent, id, x, y, w, h, flags);
  m_CX = m_CmbLeft.width() / 2;
  m_CY = 1 + m_CmbLeft.height() / 4;
  m_CX2 = m_W - m_CmbRight.width() / 2;
  m_CY2 = m_H - m_CmbRight.height() / 4;
}

void NewUIComboBox::OnDraw() {
  //	draw tiles.
  int x, tx;

  tx = x = 0;

  // top row.
  m_CmbLeft.draw(x, 0);
  x += m_CmbRight.width();
  while (tx++ < m_TileWidth) {
    m_CmbCen.draw(x, 0);
    x += m_CmbCen.width();
  }
  m_CmbRight.draw(x, 0);

  //	draw text!
  UIComboBox::OnDraw();
}

///////////////////////////////////////////////////////////////////////////////
//	NewUIButton Implementation

#define NEWBTN_HEIGHT 25

void NewUIButton::Create(UIWindow *parent, int id, UIItem *item, int x, int y, int w, int h, int flags) {
  flags |= UIF_FIT;

  m_BtnUpC.set_bitmap(NewUI_bitmap_list[NEWUIBMP_BTN_UP].bmp);
  m_BtnDownC.set_bitmap(NewUI_bitmap_list[NEWUIBMP_BTN_DOWN].bmp);
  m_BtnHiliteC.set_bitmap(NewUI_bitmap_list[NEWUIBMP_BTN_HILITE].bmp);
  m_BtnUpL.set_bitmap(NewUI_bitmap_list[NEWUIBMP_BTN_UP_L].bmp);
  m_BtnDownL.set_bitmap(NewUI_bitmap_list[NEWUIBMP_BTN_DOWN_L].bmp);
  m_BtnHiliteL.set_bitmap(NewUI_bitmap_list[NEWUIBMP_BTN_HILITE_L].bmp);
  m_BtnUpR.set_bitmap(NewUI_bitmap_list[NEWUIBMP_BTN_UP_R].bmp);
  m_BtnDownR.set_bitmap(NewUI_bitmap_list[NEWUIBMP_BTN_DOWN_R].bmp);
  m_BtnHiliteR.set_bitmap(NewUI_bitmap_list[NEWUIBMP_BTN_HILITE_R].bmp);

  //	adjust w and height acoordingly with bitmap background width and height.
  UIButton::Create(parent, id, item, x, y, w, h, flags);
  OnFormat();
}

void NewUIButton::SetTitle(UIItem *item) {
  UIButton::SetStateItem(UI_BTS_DISABLED, item);
  UIButton::SetStateItem(UI_BTS_INACTIVE, item);
  UIButton::SetStateItem(UI_BTS_HILITE, item);
  UIButton::SetStateItem(UI_BTS_ACTIVATED, item);
}

//	drawing behavior of the new bitmap.
void NewUIButton::OnDraw() {
  //	draw tiles.
  int x, tx;
  UIBitmapItem *ltile = NULL, *ctile = NULL, *rtile = NULL;

  tx = x = 0;

  // top row.
  switch (m_State) {
  case UI_BTS_DISABLED:
  case UI_BTS_INACTIVE:
    ltile = &m_BtnUpL;
    rtile = &m_BtnUpR;
    ctile = &m_BtnUpC;
    break;

  case UI_BTS_HILITE:
    ltile = &m_BtnHiliteL;
    rtile = &m_BtnHiliteR;
    ctile = &m_BtnHiliteC;
    break;

  case UI_BTS_ACTIVATED:
    ltile = &m_BtnDownL;
    rtile = &m_BtnDownR;
    ctile = &m_BtnDownC;
    break;

  default:
    ASSERT(0);
  }

  ltile->draw(x, 0);
  x += ltile->width();
  while (tx++ < m_TileWidth) {
    ctile->draw(x, 0);
    x += ctile->width();
  }
  rtile->draw(x, 0);

  // draw title
  if (GetStateItem(m_State)) {
    UIItem *item = GetStateItem(m_State);
    int x = (m_W / 2 - item->width() / 2);
    int y = (m_H / 2 - item->height() / 2);
    item->draw(x, y);
  }
}

// override: called when resized or before drawing.
void NewUIButton::OnFormat() {
  int bk_width;
  int bk_height;

  // check fit flag than resize according to bitmap aligned coords, then call UIGadget's one that does
  //	centering!
  if ((m_Flags & UIF_FIT) && m_BtnTitle) {
    m_W = m_BtnTitle->width() + 10;
  }

  bk_width = (m_W - m_BtnUpL.width() - m_BtnUpR.width()) / m_BtnUpC.width();
  bk_height = 1;

  if (bk_width < 0)
    bk_width = 0;
  else {
    if ((m_W - m_BtnUpL.width() - m_BtnUpR.width()) % m_BtnUpC.width())
      bk_width++;
  }

  m_W = (bk_width * m_BtnUpC.width()) + m_BtnUpL.width() + m_BtnUpR.width();
  m_H = NEWBTN_HEIGHT;
  m_TileWidth = bk_width;

  UIGadget::OnFormat();
}

///////////////////////////////////////////////////////////////////////////////
//	NewUIFileDialog
//		this draws a file lister.

#define UID_FILELIST 0x100

void NewUIFileDialog::Create(const char *title, int x, int y, int w, int h, const char *path, const char *filecard) {
  int list_y, list_w, list_h;

  NewUIGameWindow::Create(x, y, w, h);

  strcpy(m_SearchExt, filecard);
  SetSearchPath(path);

  list_w = w - 48;
  list_h = h - 64;
  list_y = 24;

  UITextItem itemTitle{(char *)title};
  m_TitleStr.Create(this, &itemTitle, 0, 5, UIF_CENTER);
  m_ListBox.Create(this, UID_FILELIST, 0, list_y, list_w, list_h, UIF_CENTER);
  UITextItem itemList{TXT_OK};
  m_Ok.Create(this, UID_OK, &itemList, 32, m_H - 32, 0, 0, UIF_FIT);
  UITextItem itemCancel{TXT_CANCEL};
  m_Cancel.Create(this, UID_CANCEL, &itemCancel, m_W - m_Ok.W() - 32, m_H - 32, 0, 0, UIF_FIT);

  m_FileItems = NULL;

  UpdateList();
}

void NewUIFileDialog::SetSearchPath(const char *path) { strcpy(m_SearchPath, path); }

const char *NewUIFileDialog::GetFilename() { return m_NewPath; }

void NewUIFileDialog::UpdateList() {
  char search_str[_MAX_PATH];
  char filename[_MAX_PATH];

  // remove items from listbox, free them.
  while (m_ListBox.GetNumItems()) {
    UIItem *item = m_ListBox.GetItem(0);
    m_ListBox.RemoveItem(item);
    delete item;
  }

  //	read in all files that have all search parameters met
  ddio_MakePath(search_str, m_SearchPath, m_SearchExt, NULL);

  if (ddio_FindFileStart(search_str, filename)) {
    UIItem *item = new UITextItem(filename);
    m_ListBox.AddItem(item);

    while (ddio_FindNextFile(filename)) {
      item = new UITextItem(filename);
      m_ListBox.AddItem(item);
    }

    ddio_FindFileClose();
  }
}

bool NewUIFileDialog::DoModal() {
  bool exit_menu = false, return_value = false;

  this->Open();

  //	update file list in listbox.
  UpdateList();

  while (!exit_menu) {
    const char *filename;
    int index;
    int result;

    result = DoUI();

    switch (result) {
    case NEWUIRES_FORCEQUIT:
    case UID_CANCEL:
      exit_menu = true;
      return_value = false;
      break;

    case UID_OK:
    case UID_FILELIST:
      index = m_ListBox.GetSelectedIndex();
      filename = ((UITextItem *)m_ListBox.GetItem(index))->GetBuffer();
      ASSERT(strlen(filename) < (_MAX_PATH));
      strcpy(m_NewPath, filename);
      return_value = true;
      exit_menu = true;
      break;
    }
  }

  this->Close();
  this->Destroy();

  return return_value;
}

void NewUIFileDialog::OnDestroy() {
  // remove items from listbox, free them.
  while (m_ListBox.GetNumItems()) {
    UIItem *item = m_ListBox.GetItem(0);
    m_ListBox.RemoveItem(item);
    delete item;
  }

  UIWindow::OnDestroy();
}

//@@class NewUIFileDialog: public NewUIGameWindow
//@@{
//@@	NewUIListBox m_ListBox;
//@@	NewUIButton m_Ok;
//@@
//@@	char m_SearchPath[_MAX_PATH];
//@@	char m_SearchExt[PSFILENAME_LEN];
//@@	char m_NewPath[_MAX_PATH];
//@@
//@@public:
//@@	void Create(int x, int y, int w, int h, const char *path, const char *ext);
//@@
//@@};
