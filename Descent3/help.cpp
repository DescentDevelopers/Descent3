/*
 * $Logfile: /DescentIII/main/help.cpp $
 * $Revision: 32 $
 * $Date: 3/20/00 12:10p $
 * $Author: Matt $
 *
 * Help functions (when user presses F1 in the game)
 *
 * $Log: /DescentIII/main/help.cpp $
 *
 * 32    3/20/00 12:10p Matt
 * Merge of Duane's post-1.3 changes.
 * Added help for PageDown pause key.
 *
 * 31    5/21/99 11:14p Samir
 * changed some text in help dialog. (SHIFT-TAB)
 *
 * 30    5/21/99 12:45p Samir
 * added help strings for F8 and F9 keys.
 *
 * 29    4/16/99 12:17p Samir
 * new help menu with new ui.
 *
 * 28    4/16/99 4:25a Jeff
 * removed ifdef of linux since new compiler compiles it
 *
 * 27    4/15/99 1:39a Jeff
 * changes for linux compile
 *
 * 26    4/14/99 2:50a Jeff
 * fixed some case mismatched #includes
 *
 * 25    4/06/99 12:11p Doug
 * Made ESC key work (Samir)
 *
 * 24    4/03/99 9:26p Jeff
 * changed dialogs that weren't using UID_OK and UID_CANCEL to use and
 * handle them properly
 *
 * 23    3/04/99 11:41a Jeff
 * fixed text going off window
 *
 * 22    3/03/99 7:21p Jeff
 * adjusted help file window text to make room for new text
 *
 * 21    3/03/99 1:18p Jeff
 * added telcom main menu item
 *
 * 20    2/28/99 9:19p Jeff
 * added some strings to help window
 *
 * 19    10/20/98 12:15p Jeff
 * removed SHIFT-F2 Center Missile view
 *
 * 18    10/19/98 7:08p Jeff
 * added "F6 Multiplayer On-Screen Menu"
 *
 * 17    10/08/98 7:27p Samir
 * made multiplayer friendly.
 *
 * 16    10/07/98 2:54p Jeff
 * General UI fixes and additions
 *
 * 15    10/06/98 5:34p Jeff
 * various UI changes/improvements
 *
 * 14    9/21/98 4:34p Jeff
 * updates to Help menu
 *
 * 13    9/02/98 2:54p Jeff
 * added defines for text colors to be used throughout the game...fixed up
 * buddy menu too
 *
 * 12    8/15/98 2:49p Matt
 * Took out unneeded include
 *
 * 11    7/09/98 12:33p Jeff
 * f1 closes help screen
 *
 * 10    7/08/98 4:58p Jeff
 * a bit more updated
 *
 * 9     6/17/98 3:27p Jeff
 * Changes made for localization
 *
 * 8     6/16/98 10:38a Jeff
 * localization, strings pulled out to stringtable.h and d3.str
 *
 * 7     5/05/98 6:26p Samir
 * adjusted to work in 512 384
 *
 * 6     4/24/98 3:27p Jeff
 * removed briefing.h dependency
 *
 * 5     3/05/98 2:51p Jeff
 * Made help system use UI
 *
 * 4     3/04/98 3:09p Jeff
 * Now gives key descriptions
 *
 * 3     3/03/98 4:01p Jeff
 * Got it displaying a "cheap" help screen
 *
 * 2     3/02/98 5:14p Jeff
 * initial help system created
 *
 * $NoKeywords: $
 */

#include "help.h"
#include "mono.h"
#include "renderer.h"
#include "render.h"
#include "ddio.h"
#include "descent.h"
#include "game.h"
#include "cfile/cfile.h"
#include "application.h"
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "newui.h"
#include "grtext.h"
#include "gamefont.h"
#include "stringtable.h"

#define TITLETEXT TXT_HELP
int HelpText[] = {TXI_ESC,
                  TXI_HLPQUIT,
#ifndef DEMO // Do Not include in the PC Gamer Demo
                  TXI_HLPALTF2,
                  TXI_HLPSAVEGAME,
                  TXI_HLPALTF3,
                  TXI_HLPLOADGAME,
#endif
                  TXI_F2,
                  TXI_HLPCONFIG,
                  TXI_F3,
                  TXI_HLPCOCKPIT,
                  TXI_HLPF4,
                  TXI_HLPGUIDEBOT,
                  TXI_F5,
                  TXI_TOGGLEDEMO,
                  TXI_F6,
                  TXI_MLTMENU,
                  TXI_F8,
                  TXI_HLP_MULTIMSG,
                  TXI_F9,
                  TXI_HLP_QUIKSAVE,
                  TXI_F12,
                  TXI_DROPSMARKER,
                  TXI_SHFTTAB,
                  TXI_TCMM,
#ifdef MACINTOSH
                  TXI_PAGE_DOWN,
                  TXI_HLPPAUSEDESC,
#else
                  TXI_HLPPAUSE,
                  TXI_HLPPAUSEDESC,
#endif
                  TXI_PLUSMINUS,
                  TXI_HLPSCRNSIZE,
                  TXI_HLPPRNTSCRN,
                  TXI_HLPTAKESCRNSHT,
                  TXI_HLP1_5,
                  TXI_HLPSELPRIM,
                  TXI_HLP6_0,
                  TXI_HLPSELSECN,
                  TXI_SF1,
                  TXI_HLPREARLEFT,
                  TXI_SF2,
                  TXI_HLPREARRIGHT,
                  TXI_SHFTF8,
                  TXI_DISPLAYGAMEMSGCONSOLE,
                  TXI_SHFTF9,
                  TXI_DISPLAYHUDMSGCONSOLE,
                  0};

#define HELP_X_KEY_POS 60
#define HELP_X_DESC_POS 160
#define HELP_Y 32

#define IDH_QUIT UID_CANCEL
#define WND_HELP_W 448
#define WND_HELP_H 384
#define WND_HELP_X (Game_window_w - WND_HELP_W) / 2
#define WND_HELP_Y (Game_window_h - WND_HELP_H) / 2

void HelpDisplay(void) {
  newuiTiledWindow help_wnd;
  newuiSheet *sheet;
  int strs_to_print = 0;
  int index, res;
  bool exit_menu = false;

  help_wnd.Create(TITLETEXT, 0, 0, WND_HELP_W, WND_HELP_H);
  sheet = help_wnd.GetSheet();

  // add commands
  help_wnd.AddAcceleratorKey(KEY_F1, UID_CANCEL);

  strs_to_print = 0;

  // find out how many strings to print out
  while (HelpText[strs_to_print] > 0)
    strs_to_print++;

  sheet->NewGroup(NULL, 30, 10);
  for (index = 0; index < strs_to_print; index += 2) {
    if (index < strs_to_print) {
      sheet->AddText(TXT(HelpText[index]));
    }
  }

  sheet->NewGroup(NULL, 130, 10);
  for (index = 1; index < strs_to_print; index += 2) {
    if (index < strs_to_print) {
      sheet->AddText(TXT(HelpText[index]));
    }
  }

  sheet->NewGroup(NULL, WND_HELP_W - 160, WND_HELP_H - 96, NEWUI_ALIGN_HORIZ);
  sheet->AddButton(TXT_OK, UID_CANCEL);

  //	quit_hot.Create(&help_wnd, UID_CANCEL, 0, &UITextItem(TXT_PRESSESCRET, UICOL_HOTSPOT_LO,UIALPHA_HOTSPOT_LO),
  //									&UITextItem(TXT_PRESSESCRET,UICOL_HOTSPOT_HI,UIALPHA_HOTSPOT_HI),
  //									HELP_X_KEY_POS, WND_HELP_H - OKCANCEL_YOFFSET,
  // 0,0,UIF_FIT|UIF_CENTER);
  help_wnd.Open();

  while (!exit_menu) {
    res = help_wnd.DoUI();

    // handle all UI results.
    switch (res) {
    case UID_CANCEL:
    case NEWUIRES_FORCEQUIT:
      exit_menu = true;
      break;
    }
  }

  help_wnd.Close();
  help_wnd.Destroy();
}
