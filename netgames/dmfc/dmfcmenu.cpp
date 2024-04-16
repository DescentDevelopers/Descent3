/*
 * $Logfile: /DescentIII/Main/dmfc/dmfcmenu.cpp $
 * $Revision: 1.1.1.1 $
 * $Date: 2003/08/26 03:57:21 $
 * $Author: kevinb $
 *
 * DMFC in-game Graphical menu functions
 *
 * $Log: dmfcmenu.cpp,v $
 * Revision 1.1.1.1  2003/08/26 03:57:21  kevinb
 * initial 1.5 import
 *
 *
 * 42    3/20/00 12:25p Matt
 * Merge of Duane's post-1.3 changes.
 * Initialize m_cPtrs in altenate menuItem constructor.
 *
 * 41    10/26/99 10:32a Jeff
 * fixed COM interface bug
 *
 * 40    7/07/99 5:00p Jeff
 * removed vararg functions from interface functions, just made different
 * versions of them
 *
 * 39    5/13/99 4:55p Ardussi
 * changes for compiling on the Mac
 *
 * 38    5/09/99 6:20a Jeff
 * improved Entropy (added sounds, max virii per room).  Fixed rendering
 * bugs for other multiplayer dlls.
 *
 * 37    4/03/99 4:06p Jeff
 * added loss/ping gauge
 *
 * 36    3/22/99 6:21p Jeff
 * added 2 more audio taunts.  a mulitplayer event when someone plays an
 * audio taunt.  option to disable audio taunts.
 *
 * 35    3/17/99 12:24p Jeff
 * converted DMFC to be COM interface
 *
 * 34    3/05/99 1:30p Jeff
 * fixed 99% of the high res issues
 *
 * 33    2/11/99 12:51a Jeff
 * changed names of exported variables
 *
 * 32    1/31/99 7:26p Matt
 * Renamed a bunch of functions to have HUD capitalized
 *
 * 31    1/19/99 3:55a Jeff
 * all strings localized out
 *
 * 30    1/04/99 12:21p Jeff
 * added support for hosts.allow/deny and updates stats manager a little
 *
 * 29    12/13/98 5:32p Jeff
 * fixed ugly crash due to freeing memory allocated in another heap
 *
 * 28    12/08/98 12:17p Jeff
 * various changes that include an improved Team Control dialog (doesn't
 * switch teams until exit..) and spew/respawn players that change teams
 *
 * 27    11/19/98 5:56p Jeff
 * added slider exported and improved Hoard
 *
 * 26    11/17/98 6:29p Jeff
 * mod can specify whether or not to display the team setup dialog on team
 * game start.  Added a menu item to display team setup dialog in mid-game
 *
 * 25    11/17/98 12:36p Jeff
 * remove onscreen menu when endlevel selected
 *
 * 24    11/13/98 6:36p Jeff
 * created dmfc_dll (a DLL version of DMFC) and converted current mods to
 * use it
 *
 * 23    11/11/98 7:19p Jeff
 * changes made so that a dedicated server's team is always -1 (team game
 * or not)
 *
 * 22    10/20/98 5:39p Jeff
 * fixed MIF_INCLUDEONE bug with team games
 *
 * 21    10/20/98 4:35p Jeff
 * added a flag for menu to add a <None> to MIT_PLIST...
 *
 * 20    10/20/98 12:16p Jeff
 * added death message filter, hud callsign filter
 *
 * 19    10/15/98 1:34p Jeff
 * added scrollable onscreen menu.  Remove ban in dmfc.  prejoin event
 *
 * 18    10/14/98 11:26p Jeff
 * added scrollable menus
 *
 * 17    10/05/98 2:50p Jeff
 *
 * 16    9/30/98 3:50p Jeff
 * general improvements (many)
 *
 * 15    9/29/98 3:04p Jeff
 * added time in game and start_time support
 *
 * 14    9/28/98 5:05p Jeff
 * made the statisitical death messages an option in the menu
 *
 * 13    9/21/98 7:11p Jeff
 * made InputCommand interface API and moved existing input commands to
 * the interface.  Changed mprintf/ASSERT so they are valid in DMFC
 *
 * $NoKeywords: $
 */

#include "DMFC.h"
#include "gamedll_header.h"
#include "dmfcinternal.h"

#ifdef MACINTOSH
#include <stdlib.h>
#endif

extern char **DMFCStringTable;
extern int DMFCStringTableSize;
extern char *_DMFCErrorString;
extern DMFCBase *basethis;
char *DMFCGetString(int d);

MenuItem::MenuItem() {
  m_cPtrs = 0;
  m_bMoreToScroll = false;
  m_bAtBottom = false;
  m_iTopIndex = 0;
  SubMenuCount = 0;
  CurrSubMenu = -1;
  m_iState = 0;
  SubMenus = NULL;
  func = NULL;
  HasInputFocus = false;
  m_cType = MIT_NORMAL;

  m_Alpha = 0;
  m_AlphaDir = true;

  memset(&m_cmInfo, 0, sizeof(tCustomMenu));

  strcpy(m_sTitle, " ");
}

MenuItem::MenuItem(char *title, char type, ubyte flags, void (*fp)(int), ...) {
  m_bMoreToScroll = false;
  m_bAtBottom = false;
  m_iTopIndex = 0;
  SubMenuCount = 0;
  CurrSubMenu = -1;
  m_iState = 0;
  SubMenus = NULL;
  func = fp;
  HasInputFocus = false;
  m_cType = type;
  m_Alpha = 0;
  m_AlphaDir = true;
  m_iFlags = flags;
  m_cPtrs = 0; // DAJ Do this or fail the second netgame start

  memset(&m_cmInfo, 0, sizeof(tCustomMenu));

  if (title) {
    strncpy(m_sTitle, title, MAX_STRING_LEN - 1);
    m_sTitle[MAX_STRING_LEN - 1] = '\0';
  } else {
    strcpy(m_sTitle, " ");
  }

  // if the type is MIT_PLIST than add a dummy submenu so it will register
  if (type == MIT_PLIST) {
    SubMenuCount = 1;
    SubMenus = (IMenuItem **)malloc(sizeof(IMenuItem *) * 1);
    SubMenus[0] = new MenuItem("Pfuncer", MIT_STATEITEM, 0, fp);
  }

  // if the type is MIT_CUSTOM than add a dummy submenu so it will register
  // the only variable parameter should be a pointer to a tCustomMenu struct that
  // contains the needed callbacks
  if (type == MIT_CUSTOM) {
    SubMenuCount = 1;
    SubMenus = (IMenuItem **)malloc(sizeof(IMenuItem *) * 1);
    SubMenus[0] = new MenuItem("Pfuncer", MIT_STATEITEM, 0, fp);

    va_list marker;
    va_start(marker, fp);
    memcpy(&m_cmInfo, va_arg(marker, tCustomMenu *), sizeof(tCustomMenu));
    va_end(marker);
  }

  // if the type is MIT_STATE than we need to pull off the extra parameters for the states
  if (type == MIT_STATE) {
    va_list marker;
    va_start(marker, fp);
    SubMenuCount = va_arg(marker, int);
    m_iState = va_arg(marker, int);
    if ((SubMenuCount > 0) && (SubMenuCount < MAX_STATE_SUBMENUS)) {
      SubMenus = (IMenuItem **)malloc(sizeof(IMenuItem *) * SubMenuCount);
      if (!SubMenus)
        return;
      char *string;
      for (int i = 0; i < SubMenuCount; i++) {
        string = va_arg(marker, char *);
        SubMenus[i] = new MenuItem(string, MIT_STATEITEM, 0, fp);
      }
    } else
      SubMenuCount = 0;
    va_end(marker);
  }
}

MenuItem::~MenuItem() {
  MenuItem *p;

  if (SubMenus) {
    if ((m_cType == MIT_PLIST) || (m_cType == MIT_CUSTOM)) {
      p = (MenuItem *)SubMenus[0];
      if (p) {
        SubMenus[0] = NULL;
        delete p;
      }
    } else {
      for (int i = 0; i < SubMenuCount; i++) {
        p = (MenuItem *)SubMenus[i];
        if (p) {
          SubMenus[i] = NULL;
          delete p;
        }
      }
    }

    free(SubMenus);
    SubMenus = NULL;
  }

  SubMenuCount = 0;
}

bool MenuItem::AddSubMenu(IMenuItem *p) {
  // No submenus for a Player list submenu or custom or if we are working with a stateitem submenu item
  if ((m_cType == MIT_PLIST) || (m_cType == MIT_STATEITEM) || (m_cType == MIT_STATE) || (m_cType == MIT_CUSTOM))
    return false;

  MenuItem **temp;
  temp = (MenuItem **)SubMenus;

  // create a new array of submenus and copy the old to it
  SubMenus = (IMenuItem **)malloc(sizeof(IMenuItem *) * (SubMenuCount + 1));
  for (int i = 0; i < SubMenuCount; i++) {
    SubMenus[i] = temp[i];
  }

  if (temp)
    free(temp);

  // Put the new submenu item at the end of this one
  SubMenus[SubMenuCount] = p;
  SubMenuCount++;
  return true;
}

bool MenuItem::DetachSubMenu(IMenuItem *p) {
  // No submenus for a Player list submenu or custom or if we are working with a stateitem submenu item
  if ((m_cType == MIT_PLIST) || (m_cType == MIT_STATEITEM) || (m_cType == MIT_STATE) || (m_cType == MIT_CUSTOM))
    return false;

  if (SubMenuCount < 1)
    return false;

  int i;
  bool found = false;

  // make sure there is this menu somewhere in our list
  for (i = 0; i < SubMenuCount; i++) {
    if (SubMenus[i] == p) {
      found = true;
      break;
    }
  }
  if (!found) {
    // doesn't exist
    return false;
  }

  MenuItem **temp;
  temp = (MenuItem **)SubMenus;

  if (SubMenuCount > 1) {
    // we need to resize our list
    // create a new array of submenus and copy the old to it
    int c;

    SubMenus = (IMenuItem **)malloc(sizeof(IMenuItem *) * (SubMenuCount - 1));
    for (i = 0, c = 0; i < SubMenuCount; i++) {
      // make sure it isn't the one we are trying to detach
      if (temp[i] != p) {
        ASSERT((c < (SubMenuCount - 1)));
        SubMenus[c] = temp[i];
        c++;
      }
    }

    if (temp)
      free(temp);

    SubMenuCount--;
  } else {
    // this is the only remaining submenu
    SubMenuCount = 0;

    if (temp)
      free(temp);
    SubMenus = NULL;
  }

  return true;
}

bool MenuItem::Forward(void) {
  if ((m_cType == MIT_PLIST) || (m_cType == MIT_STATE) || (m_cType == MIT_CUSTOM))
    return false; // short circuit on a plist and state since it has no submenus in reality

  char type = SubMenus[CurrSubMenu]->GetType();

  // We can only move forward if there is a submenu
  if ((type == MIT_CUSTOM && SubMenus[CurrSubMenu]->GetCustomSubMenuCount() > 0) ||
      (type != MIT_CUSTOM && (SubMenuCount > 0))) {
    // If this submenu has input focus, than we need to check the currently selected submenu
    // and set that as the new item with focus, and remove focus from this one
    if (HasInputFocus) {
      if ((CurrSubMenu != -1) && (SubMenus[CurrSubMenu]->HasSubMenus())) {
        SubMenus[CurrSubMenu]->SetInputFocus();
        HasInputFocus = false;
      }
      return true;
    }
    // This submenu doesn't have input focus, so move to it's currently selected submenu and
    // Tell it to go forward if it can
    if (CurrSubMenu != -1)
      return SubMenus[CurrSubMenu]->Forward();
  }
  return false;
}
bool MenuItem::Back(void) {
  // We need to look ahead and see if the next submenu has input focus
  // if it does have focus, than we need to make it lose focus, and make this one have focus
  if (SubMenuCount > 0) {
    if (CurrSubMenu != -1) {
      if (SubMenus[CurrSubMenu]->GetFocus()) {
        // The next submenu has focus so we can remove it and set this guy to have focus
        SubMenus[CurrSubMenu]->LoseInputFocus();
        HasInputFocus = true;
        return true;
      }
      // The next submenu doesn't have focus, but one of it's submenus might (they better),
      // so move on to the next submenu and call Back()
      return SubMenus[CurrSubMenu]->Back();
    }
  }
  return false;
}
bool MenuItem::Up(void) {
  // See if this MenuItem has focus, if it does, and we can move up, then move up
  if (HasInputFocus) {
    if (CurrSubMenu > 0)
      CurrSubMenu--;
    if (m_iTopIndex > 0 && m_iTopIndex >= CurrSubMenu) {
      // move the top and index's up
      m_iTopIndex--;
    }

    return true;
  }
  // Ok, one of the submenus better have focus, so move on to the next submenu and call Up()
  if (CurrSubMenu != -1)
    return SubMenus[CurrSubMenu]->Up();
  return false;
}
bool MenuItem::Down(void) {
  // See if this MenuItem has focus, if it does, and we can move down, then move down
  if (HasInputFocus) {
    if (CurrSubMenu < SubMenuCount - 1)
      CurrSubMenu++;
    if (m_bMoreToScroll && m_bAtBottom) {
      m_iTopIndex++;
    }
    return true;
  }
  // Ok, one of the submenus better have focus, so move on to the next submenu and call Down()
  if (CurrSubMenu != -1)
    return SubMenus[CurrSubMenu]->Down();
  return false;
}
void MenuItem::Execute(void) {
  switch (m_cType) {
  case MIT_NORMAL: {
    // We need to go through the Submenus and check to see if the submenu has focus
    // if it does, than we need to execute it's function
    if (SubMenuCount > 0) {
      if (CurrSubMenu != -1) {
        if (HasInputFocus) {
          // The submenu has focus, so call it's handler as long as it's not a plist type
          char type = SubMenus[CurrSubMenu]->GetType();
          if ((type != MIT_PLIST) && (type != MIT_STATE) && (type != MIT_CUSTOM))
            SubMenus[CurrSubMenu]->CallFunc(CurrSubMenu);
          return;
        }
        // The next submenu doesn't have focus, so move on and check it's submenu to see if we
        // can Execute() that
        SubMenus[CurrSubMenu]->Execute();
      }
    } else {
      // We don't have any SubMenus so try and call our handler
      CallFunc(0);
    }
  } break;
  case MIT_PLIST: {
    // We need to go through the Submenus and check to see if the submenu has focus
    // if it does, than we need to execute it's function
    if ((SubMenuCount > 0) && (CurrSubMenu != -1) && (HasInputFocus)) {
      if (m_iFlags & MIF_INCLUDENONE) {
        // there is a <None> so take that into consideration
        if (CurrSubMenu == 0) {
          //<None> is selected
          CallFunc(-1);
          return;
        }

        if (basethis->CheckPlayerNum(Pnums[CurrSubMenu - 1])) {
          // The submenu has focus, so call it's handler
          CallFunc(Pnums[CurrSubMenu - 1]);
          return;
        }
      } else {
        // a regular plist
        if (basethis->CheckPlayerNum(Pnums[CurrSubMenu])) {
          // The submenu has focus, so call it's handler
          CallFunc(Pnums[CurrSubMenu]);
          return;
        }
      }
    }
  } break;
  case MIT_CUSTOM: {
    // We need to go through the Submenus and check to see if the submenu has focus
    // if it does, than we need to execute it's function
    int count = (m_cmInfo.GetListCount) ? (*m_cmInfo.GetListCount)() : 0;
    if (count > 0) {
      if (CurrSubMenu != -1) {
        if (HasInputFocus) {
          // The submenu has focus, so call it's handler
          CallFunc(CurrSubMenu);
          return;
        }
      }
    }
  }
  case MIT_STATE: {
    // We need to go through the Submenus and check to see if the submenu has focus
    // if it does, than we need to execute it's function
    if (SubMenuCount > 0) {
      if (CurrSubMenu != -1) {
        if (HasInputFocus) {
          // The submenu has focus, so call it's handler
          CallFunc(CurrSubMenu);
          m_iState = CurrSubMenu;
          return;
        }
      }
    }
  } break;
  }
}
void MenuItem::Draw(int x, int y, int height, int bmp, float *not_used) {
  int old_font = DLLgrtext_GetFont();
  DLLgrtext_SetFont(basethis->Game_fonts[HUD_FONT_INDEX]);

  /*
  static float aspratio;
  if(!ratio){
          //we need to calculate a ratio
          aspratio = DEFAULT_HUD_WIDTH / ((float)*base->Game_window_w);
  }

  float useratio;
  if(!ratio) useratio = aspratio; else useratio = *ratio;
  */

  ubyte alpha_to_use = 255;

#define FLASHRATE 700.0 // alphas per second

  signed short a = m_Alpha;
  int units = (int)FLASHRATE * (*basethis->Frametime);

  // do alphaing effect
  while (units > 0) {
    if (m_AlphaDir) {
      // find the amount to adjust
      ubyte amount = min(255 - a, units);
      units -= amount;
      a += amount;
      if (a >= 255) {
        a = 255;
        m_AlphaDir = false;
      }
    } else {
      // find the amount to adjust
      ubyte amount = min(a, units);
      units -= amount;
      a -= amount;
      if (a <= 0) {
        a = 0;
        m_AlphaDir = true;
      }
    }
  }

  m_Alpha = alpha_to_use = (ubyte)a;

  ddgr_color color;
  int temp, maxx;
  temp = maxx = 0;
  char buff[256];
  char checked[3];
  char buffer[100];

  if (SubMenuCount <= 0) {
    DLLgrtext_SetFont(old_font);
    return;
  }

  bool nodisp_custom = false;

  // First find the max width of the submenu items
  switch (m_cType) {
  case MIT_NORMAL:
  case MIT_STATE: {
    for (int i = 0; i < SubMenuCount; i++) {
      // temp = DLLRenderHUDGetTextLineWidth(SubMenus[i]->GetTitle());
      temp = DLLgrtext_GetTextLineWidth(SubMenus[i]->GetTitle());
      if (temp > maxx)
        maxx = temp;
    }
    if (m_cType == MIT_NORMAL) {
      maxx += (x + 10);
    } else {
      sprintf(checked, "%c ", CHAR_RIGHT_ARROW);
      // maxx += (x + 10 + (useratio * ((float)DLLRenderHUDGetTextLineWidth(checked))));
      // maxx += (x + 10 + DLLRenderHUDGetTextLineWidth(checked));
      maxx += (x + 10 + DLLgrtext_GetTextLineWidth(checked));
    }
  } break;
  case MIT_PLIST: {
    // First create/update the playerlist
    SubMenuCount = 0;
    int i;
    for (i = 0; i < DLLMAX_PLAYERS; i++) {
      if (basethis->CheckPlayerNum(i)) {
        // we have a valid pnum, fill it in
        Pnums[SubMenuCount] = i;
        SubMenuCount++;
      }
    }

    if (m_iFlags & MIF_INCLUDENONE)
      SubMenuCount++; // account for <None>

    // Do some error checking and make sure values for the currsub are ok
    if (CurrSubMenu >= SubMenuCount)
      CurrSubMenu = SubMenuCount - 1;

    if (CurrSubMenu < m_iTopIndex)
      m_iTopIndex = (CurrSubMenu >= 0) ? CurrSubMenu : 0;

    int end_i = SubMenuCount;

    // Now we have a list of pnums, so draw them
    if (m_iFlags & MIF_INCLUDENONE) {
      // temp = useratio * ((float)DLLRenderHUDGetTextLineWidth(DTXT_NONE));
      // temp = DLLRenderHUDGetTextLineWidth(DTXT_NONE);
      temp = DLLgrtext_GetTextLineWidth(DTXT_NONE);
      end_i = SubMenuCount - 1;
    }

    // go through all the names and get the longest name
    for (i = 0; i < end_i; i++) {
      // temp = useratio * ((float)DLLRenderHUDGetTextLineWidth(base->Players[Pnums[i]].callsign));
      // temp = DLLRenderHUDGetTextLineWidth(base->Players[Pnums[i]].callsign);
      temp = DLLgrtext_GetTextLineWidth(basethis->Players[Pnums[i]].callsign);
      if (temp > maxx)
        maxx = temp;
    }
    // maxx += (x + 10 + (useratio * ((float)DLLRenderHUDGetTextLineWidth("[33]"))));
    // maxx += (x + 10 + DLLRenderHUDGetTextLineWidth("[33]"));
    maxx += (x + 10 + DLLgrtext_GetTextLineWidth("[33]"));
  } break;
  case MIT_CUSTOM: {
    // First find out how many submenus there are going to be
    SubMenuCount = (m_cmInfo.GetListCount) ? (*m_cmInfo.GetListCount)() : 0;

    // Do some error checking and make sure values are ok
    if (CurrSubMenu >= SubMenuCount)
      CurrSubMenu = SubMenuCount - 1;
    if (CurrSubMenu < m_iTopIndex)
      m_iTopIndex = (CurrSubMenu >= 0) ? CurrSubMenu : 0;

    if (SubMenuCount <= 0) {
      nodisp_custom = true;
    }

    // now find out the biggest string
    if (m_cmInfo.GetItem) {
      for (int i = 0; i < SubMenuCount; i++) {
        char *ptr = (*m_cmInfo.GetItem)(i);
        if (ptr) {
          // temp = useratio * ((float)DLLRenderHUDGetTextLineWidth(ptr));
          // temp = DLLRenderHUDGetTextLineWidth(ptr);
          temp = DLLgrtext_GetTextLineWidth(ptr);
          if (temp > maxx)
            maxx = temp;
        }
      }
    }
    maxx += (x + 10);
  } break;
  }

  int end_index = min(SubMenuCount, m_iTopIndex + ((400 - y) / height));
  int num_teams = basethis->GetNumTeams();

  if (SubMenuCount && bmp > BAD_BITMAP_HANDLE) {
    DLLrend_SetAlphaValue(210);
    DLLrend_SetZBufferState(0);
    DLLrend_SetTextureType(TT_LINEAR);
    DLLrend_SetLighting(LS_NONE);
    DLLrend_SetAlphaType(AT_CONSTANT_TEXTURE);
    DLLrend_DrawScaledBitmap(x, y, maxx, y + (height * (end_index - m_iTopIndex + 1)), bmp, 0, 0, 1, 1, 1.0, 1, NULL);
    DLLrend_SetZBufferState(1);
  }

  x += 5;

  ubyte al;

  m_bMoreToScroll = (end_index < SubMenuCount) ? true : false;
  // m_bAtBottom = (m_bMoreToScroll)?((CurrSubMenu<end_index-1)?false:true):((CurrSubMenu<end_index)?false:true);
  m_bAtBottom =
      (m_bMoreToScroll) ? ((CurrSubMenu < end_index - 2) ? false : true) : ((CurrSubMenu < end_index) ? false : true);

  // we have something to draw
  for (int i = m_iTopIndex; i < end_index; i++) {
    switch (m_cType) {
    case MIT_PLIST: {
      if (num_teams <= 1) {
        if ((i == CurrSubMenu) && (HasInputFocus)) {
          al = alpha_to_use;
          color = GR_RGB(180, 255, 180);
        } else {
          al = 255;
          color = GR_RGB(0, 180, 0);
        }
      } else {
        if ((i == CurrSubMenu) && (HasInputFocus)) {
          al = alpha_to_use;
          color = GR_RGB(180, 255, 180);
        } else {
          al = 255;
          if (m_iFlags & MIF_INCLUDENONE) {
            if (i == 0)
              color = GR_RGB(0, 180, 0);
            else
              color = basethis->GetTeamColor(basethis->Players[Pnums[i - 1]].team);
          } else {
            color = basethis->GetTeamColor(basethis->Players[Pnums[i]].team);
          }
        }
      }
    } break;
    case MIT_CUSTOM:
    case MIT_NORMAL:
    case MIT_STATE: {
      if ((i == CurrSubMenu) && (HasInputFocus)) {
        al = alpha_to_use;
        color = GR_RGB(180, 255, 180);
      } else {
        al = 255;
        color = GR_RGB(0, 180, 0);
      }
    } break;
    }

    bool draw_text = true;
    if (i == m_iTopIndex && m_iTopIndex > 0) {
      // we need to draw the UP arrow
      char b[2];
      sprintf(b, "%c", CHAR_UP_ARROW);
      // DLLRenderHUDText(color,al,0,(maxx-x)/2+x,y,b);
      DLLgrtext_SetColor(color);
      DLLgrtext_SetAlpha(al);
      DLLgrtext_Printf((maxx - x) / 2 + x, y, b);
      draw_text = false;
    }

    if (m_bMoreToScroll && i == (end_index - 1)) {
      // we need to draw the DOWN arrow
      char b[2];
      sprintf(b, "%c", CHAR_DOWN_ARROW);
      // DLLRenderHUDText(color,al,0,(maxx-x)/2+x,y,b);
      DLLgrtext_SetColor(color);
      DLLgrtext_SetAlpha(al);
      DLLgrtext_Printf((maxx - x) / 2 + x, y, b);
      draw_text = false;
    }

    if (draw_text) {
      switch (m_cType) {
      case MIT_NORMAL: {
        // DLLRenderHUDText(color,al,0,x,y,SubMenus[i]->GetTitle());
        DLLgrtext_SetColor(color);
        DLLgrtext_SetAlpha(al);
        DLLgrtext_Printf(x, y, SubMenus[i]->GetTitle());

        if (i == CurrSubMenu) { // go through the submenus
          // if(ratio){
          SubMenus[i]->Draw(maxx, y, height, bmp, not_used);
          //}else{
          //	SubMenus[i]->Draw(maxx,y,height,base,bmp,&aspratio);
          //}
        }
      } break;
      case MIT_PLIST: {
        if (m_iFlags & MIF_INCLUDENONE) {

          if (i == 0) {
            // include the <None> if index is 0
            strcpy(buffer, DTXT_NONE);
          } else {
            // do the regular
            sprintf(buffer, "%s[%d]", basethis->Players[Pnums[i - 1]].callsign, Pnums[i - 1]);
          }
        } else {
          // do the regular
          sprintf(buffer, "%s[%d]", basethis->Players[Pnums[i]].callsign, Pnums[i]);
        }

        // DLLRenderHUDText(color,al,0,x,y,buffer);
        DLLgrtext_SetColor(color);
        DLLgrtext_SetAlpha(al);
        DLLgrtext_Printf(x, y, buffer);

      } break;
      case MIT_CUSTOM: {
        if (m_cmInfo.GetItem) {
          char *ptr = (*m_cmInfo.GetItem)(i);
          if (ptr) {
            sprintf(buffer, "%s", ptr);
            // DLLRenderHUDText(color,al,0,x,y,buffer);
            DLLgrtext_SetColor(color);
            DLLgrtext_SetAlpha(al);
            DLLgrtext_Printf(x, y, buffer);
          }
        }
      } break;
      case MIT_STATE: {
        if (i == m_iState)
          strcpy(buff, checked);
        else
          strcpy(buff, "  ");
        strcat(buff, SubMenus[i]->GetTitle());
        // DLLRenderHUDText(color,al,0,x,y,buff);
        DLLgrtext_SetColor(color);
        DLLgrtext_SetAlpha(al);
        DLLgrtext_Printf(x, y, buff);

      } break;
      }
    }

    y += height;
  }

  if (nodisp_custom) {
    // check to see if we have focus...if we do...we got to move back to the previous menu
    if (HasInputFocus) {
      basethis->Menu.Back();
    }
    SubMenuCount = 1;
  }

  DLLgrtext_SetFont(old_font);
}
char *MenuItem::GetTitle(void) { return m_sTitle; }
void MenuItem::SetInputFocus(void) {
  // Sets this MenuItem with Input focus, reseting it's Current Sub Menu to 0 if it has a SubMenu
  HasInputFocus = true;
  if (SubMenuCount > 0)
    CurrSubMenu = (m_iTopIndex == 0) ? 0 : m_iTopIndex + 1;
  else
    CurrSubMenu = -1;
}
void MenuItem::LoseInputFocus(void) {
  // Make this guy lose focus, Set it's Current Submenu to -1
  HasInputFocus = false;
  CurrSubMenu = -1;
}
bool MenuItem::GetFocus(void) { return HasInputFocus; }
void MenuItem::CallFunc(int value) {
  if (func)
    (*func)(value);
}
bool MenuItem::HasSubMenus(void) { return (SubMenuCount > 0); }

int MenuItem::GetCustomSubMenuCount(void) {
  if (m_cType != MIT_CUSTOM)
    return -1;

  if (!m_cmInfo.GetListCount)
    return -1;

  return (*m_cmInfo.GetListCount)();
}

char MenuItem::GetType(void) { return m_cType; }
void MenuItem::SetState(int state) {
  if (m_cType != MIT_STATE)
    return;

  if ((state < 0) || (state >= SubMenuCount))
    return;

  m_iState = state;
}

bool MenuItem::SetStateItemList(int count, ...) {
  if (m_cType != MIT_STATE)
    return false;

  // remove original states
  for (int i = 0; i < SubMenuCount; i++) {
    if (SubMenus[i]) {
      delete SubMenus[i];
      SubMenus[i] = NULL;
    }
  }
  if (SubMenus) {
    free(SubMenus);
    SubMenus = NULL;
  }

  va_list marker;
  va_start(marker, count);
  SubMenuCount = count;
  m_iState = 0;

  if ((SubMenuCount > 0) && (SubMenuCount < MAX_STATE_SUBMENUS)) {
    SubMenus = (IMenuItem **)malloc(sizeof(IMenuItem *) * SubMenuCount);
    if (!SubMenus)
      return false;
    char *string;
    for (int i = 0; i < SubMenuCount; i++) {
      string = va_arg(marker, char *);
      SubMenus[i] = new MenuItem(string, MIT_STATEITEM, 0, func);
    }
  } else
    SubMenuCount = 0;
  va_end(marker);
  return true;
}

bool MenuItem::SetStateItemListArray(int count, char **array) {
  if (m_cType != MIT_STATE)
    return false;

  // remove original states
  for (int i = 0; i < SubMenuCount; i++) {
    if (SubMenus[i]) {
      delete SubMenus[i];
      SubMenus[i] = NULL;
    }
  }
  if (SubMenus) {
    free(SubMenus);
    SubMenus = NULL;
  }

  SubMenuCount = count;
  m_iState = 0;

  if ((SubMenuCount > 0) && (SubMenuCount < MAX_STATE_SUBMENUS)) {
    SubMenus = (IMenuItem **)malloc(sizeof(IMenuItem *) * SubMenuCount);
    if (!SubMenus)
      return false;
    char *string;
    for (int i = 0; i < SubMenuCount; i++) {
      string = array[i];
      SubMenus[i] = new MenuItem(string, MIT_STATEITEM, 0, func);
    }
  } else
    SubMenuCount = 0;
  return true;
}

///////////////////////////////////////////////////
// Menu Function handlers

int GetBanPlayerList(void) { return basethis->GetNumBannedPlayers(); }

char *GetBannedPlayerString(int index) {
  static char r[MAX_CALLSIGN_SIZE + 8];
  char *ptr = basethis->GetBannedPlayerCallsign(index);
  if (ptr) {
    sprintf(r, "[%d]%s", index, ptr);
    return r;
  } else
    return NULL;
}

// Removes a ban for a player, give the ban index #
void RemoveBanByIndex(int index) { basethis->RemoveBan(index); }

// Server Only.  Kicks a player from the game
void KickPlayer(int pnum) {
  if (basethis->GetLocalRole() != LR_SERVER) {
    mprintf((0, "KickPlayer Error: You're Not The Server\n"));
    return;
  }
  if (pnum == basethis->GetPlayerNum()) {
    mprintf((0, "KickPlayer Error: Server can't kick themself\n"));
    DLLAddHUDMessage(DTXT_SERVERCANTKICK);
    return;
  }
  if (!basethis->CheckPlayerNum(pnum)) {
    mprintf((0, "KickPlayer Error: Pnum %d not in game\n", pnum));
    DLLAddHUDMessage(DTXT_INVALIDPNUM, pnum);
    return;
  }
  basethis->SendControlMessageToPlayer(pnum, CM_KICKED);
  DLLMultiDisconnectPlayer(pnum);
}
// Server Only. Temp bans a player from the game
void BanPlayer(int pnum) {
  if (basethis->GetLocalRole() != LR_SERVER) {
    mprintf((0, "BanPlayer Error: You're Not The Server\n"));
    return;
  }
  if (pnum == basethis->GetPlayerNum()) {
    mprintf((0, "BanPlayer Error: Server can't ban themself\n"));
    DLLAddHUDMessage(DTXT_SERVERCANTBAN);
    return;
  }
  if (!basethis->CheckPlayerNum(pnum)) {
    mprintf((0, "BanPlayer Error: Pnum %d not in game\n", pnum));
    DLLAddHUDMessage(DTXT_INVALIDPNUM, pnum);
    return;
  }
  basethis->SendControlMessageToPlayer(pnum, CM_BANNED);
  DLLMultiDisconnectPlayer(pnum);
  basethis->BanPlayerFromGame(pnum);
}
// Server Only. Ends the current level
void EndMultiLevel(int i) {
  if (basethis->GetLocalRole() != LR_SERVER) {
    mprintf((0, "You're Not The Server\n"));
    return;
  }
  DLLMultiEndLevel();
  basethis->EnableOnScreenMenu(false);
}
// Server Only. Automatically balances the teams
void BalanceTeams(int i) {
  if (basethis->GetLocalRole() != LR_SERVER) {
    mprintf((0, "You're Not The Server\n"));
    return;
  }

  mprintf((0, "Balancing Teams\n"));
  DPrintf(DTXT_BALANCINGTEAMS);

  // First we need to get a count of how many are on each team
  int TeamCount[DLLMAX_TEAMS];
  for (i = 0; i < DLLMAX_TEAMS; i++)
    TeamCount[i] = 0;

  for (i = 0; i < DLLMAX_PLAYERS; i++) {
    if (basethis->CheckPlayerNum(i)) {
      if (basethis->Players[i].team != -1) { // don't count dedicated server
        TeamCount[basethis->Players[i].team]++;
      }
    }
  }

  // Now determine how many should be on each team
  int optcount = 0;
  int ingame = 0;
  int numteams = basethis->GetNumTeams();

  for (i = 0; i < MAX_TEAMS; i++)
    ingame += TeamCount[i];

  optcount = ingame / numteams;
  int leftover = ingame - (optcount * numteams);

  // fill in the number of players each team should have
  int ShouldHave[MAX_TEAMS];
  for (i = 0; i < MAX_TEAMS; i++) {
    if (i < numteams) {
      ShouldHave[i] = optcount;
    } else {
      ShouldHave[i] = 0;
    }
  }

  // Make adjustments for the leftover players
  for (i = 0; i < leftover; i++)
    ShouldHave[i]++;

  for (i = 0; i < MAX_TEAMS; i++) {
    mprintf((0, "%s team should have %d players and has %d now\n", basethis->GetTeamString(i), ShouldHave[i],
             TeamCount[i]));
  }

  // Now move the players around until the teams are of the right size
  int j;
  for (i = 0; i < MAX_TEAMS; i++) {
    while (ShouldHave[i] > TeamCount[i]) {
      // We need to adjust this team by adding more players
      int pn;
      float lowseniority;
      int lowpn;

      lowseniority = 0;
      lowpn = -1;

      for (j = 0; j < MAX_TEAMS; j++) {
        if (ShouldHave[j] < TeamCount[j]) {
          // We can remove a player from this team

          // Determine which player has least seniority
          for (pn = 0; pn < DLLMAX_PLAYERS; pn++) {
            if ((basethis->CheckPlayerNum(pn)) && (basethis->Players[pn].team == j) &&
                (basethis->Players[pn].team != -1)) {
              // get the pnum, and seniority info here
              if (lowseniority < basethis->Players[pn].time_in_game) {
                lowpn = pn;
                lowseniority = basethis->Players[pn].time_in_game;
              }
            } // end if
          }   // end for
          if (lowpn != -1) {
            TeamCount[j]--;
            break;
          }

        } // end if
      }   // end for
      if (lowpn != -1) {
        // Change this player to the new team
        TeamCount[i]++;
        basethis->RequestTeamChange(i, lowpn, true);
      }
    } // end while
  }   // end for
}
// Server Only.  Switches AutoTeamSelect On/Off
void SwitchAutoTeamSelect(int i) {
  if (basethis->GetLocalRole() != LR_SERVER) {
    mprintf((0, "You're Not The Server\n"));
    return;
  }
  bool on = (i == 1) ? true : false;
  basethis->AutoTeamSelect(on);
}

// Server Only. Switches allowing Team Changing on/off
void SwitchTeamChange(int i) {
  if (basethis->GetLocalRole() != LR_SERVER) {
    mprintf((0, "You're not the server\n"));
    return;
  }
  bool on = (i == 1) ? true : false;
  basethis->SwitchAllowTeamChange(on);
}

// Switch's on/off Displaying a player's info
void SwitchPlayerInfo(int pnum) {
  if (pnum == -1) {
    // the <None> was selected, so make sure that it's turned off
    int curr_disp = basethis->DisplayingPlayerInfo();

    if (curr_disp == -1) // we're not displaying anyone now anyway
      return;

    // turn off the pnum given to us
    basethis->SwitchPlayerInfoDisplay(curr_disp);

    return;
  }

  if (!basethis->CheckPlayerNum(pnum)) {
    mprintf((0, "%d is invalid for playerinfo\n", pnum));
    return;
  }

  int curr_disp = basethis->DisplayingPlayerInfo();

  if (curr_disp != -1 && curr_disp != pnum)
    basethis->SwitchPlayerInfoDisplay(pnum); // turn it off

  basethis->SwitchPlayerInfoDisplay(pnum);

  // Make the call to DisplayPlayerInfo now with passing true to dedicated_server so it will
  // print the info to the console once
  basethis->DisplayPlayerInfo(true);
}

// Switch's on/off Displaying a player's name on the HUD
void SwitchHudPlayerNum(int i) { basethis->SwitchShowHudCallsignLevel(i); }

// Switch the server's Max HUD Callsign level
void SwitchServerHudPlayerName(int i) {
  if (basethis->GetLocalRole() != LR_SERVER)
    return;
  basethis->SwitchServerHudCallsignLevel(i);
}

// Change teams (or more specifically request to change teams)
void ChangeTeams(int team) {
  if ((team < 0) || (team > 3)) {
    mprintf((0, "Invalid team requested (%d)\n", team));
    return;
  }
  int curr_team = basethis->Players[basethis->GetPlayerNum()].team;

  if (curr_team == -1) {
    mprintf((0, "Dedicated Server Can't Change Teams\n"));
    DPrintf(DTXT_NODEDICATEDTEAM);
    return;
  }

  if (team == curr_team) {
    mprintf((0, "You're already on the %s team\n", basethis->GetTeamString(team)));
    DPrintf(DTXT_SAMETEAMCHANGE, basethis->GetTeamString(team));
    return;
  }
  basethis->RequestTeamChange(team, basethis->GetPlayerNum(), true);
}

// Send a request to DMFC to save the game stats to file
void SaveStatsToFile(int i) { basethis->CallOnSaveStatsToFile(); }

// Turns on/off Observer Mode
void SwitchObserverMode(int i) {
  object *pobj;
  int pnum = basethis->GetPlayerNum();

  if ((pnum < 0) || (pnum >= DLLMAX_PLAYERS))
    return;

  pobj = &basethis->Objects[basethis->Players[pnum].objnum];

  mprintf((0, "SwitchObserverMode %s [%d]\n", (i == 1) ? "On" : "Off", pnum));

  if (pobj->type == OBJ_OBSERVER) {
    if ((i == 0) && (basethis->CallOnAllowObserverChange(false)))
      DLLMultiSendRequestToObserve(OBSERVER_MODE_ROAM, 0, 0);
  } else {
    if ((i == 1) && (basethis->CallOnAllowObserverChange(true)))
      DLLMultiSendRequestToObserve(OBSERVER_MODE_ROAM, 1, 0);
  }
}

// Turns on/off PiggyBack mode
void SwitchPiggyBack(int pnum) {
  if (!basethis->CheckPlayerNum(pnum)) {
    return;
  }

  object *pobj;
  pobj = &basethis->Objects[basethis->Players[basethis->GetPlayerNum()].objnum];

  if (pobj->type == OBJ_OBSERVER) {
    if (basethis->CallOnAllowObserverChange(false))
      DLLMultiSendRequestToObserve(OBSERVER_MODE_ROAM, 0, 0);
  }
  if (pnum == basethis->GetPlayerNum()) {
    mprintf((0, "Returning to self\n"));
    return;
  }

  if (basethis->CallOnAllowObserverChange(true)) {
    mprintf((0, "Switching to piggyback for player %d\n", pnum));
    DLLMultiSendRequestToObserve(OBSERVER_MODE_PIGGYBACK, 1, basethis->Players[pnum].objnum);
  }
}

//	Turns on/off the statistical hud messages
void SwitchStatHUDMessages(int i) {
  bool on = (i) ? true : false;
  basethis->EnableStatisticalMessages(on);
}

//	Exits the on-screen menu
void MenuExitMenu(int i) { basethis->EnableOnScreenMenu(false); }

//	Switches on/off auto saving of the stats to file on level end
void SwitchSaveStatsLevelEnd(int i) { basethis->EnableAutoSaveLevelEnd((i) ? true : false); }

//	Switches on/off auto saving of the stats to file on disconnect from the game
void SwitchSaveStatsDisconnect(int i) { basethis->EnableAutoSaveDisconnect((i) ? true : false); }

//	Switches on/off displaying the background
void SwitchMenuBackground(int i) {
  bool on = (i) ? true : false;
  basethis->EnableOnScreenMenuBackground(on);
}

//	Switches on/off displaying the netgame info
void SwitchNetGameInfo(int i) { basethis->SwitchNetGameInfoDisplay(i); }

// Switches the death message filter
void SwitchDeathMessageFilter(int i) { basethis->SetDeathMessageFilter(i); }

// brings up the team config dialog for the server
void OnScreenDisplayTeamConfig(int i) {
  if (basethis->GetLocalRole() != LR_SERVER)
    return;
  if (basethis->GetNumTeams() < 2)
    return;

  static char enable;
  enable = 2;
  basethis->StartUIWindow(UIID_TEAMPLACEMENT, &enable);
  basethis->EnableOnScreenMenu(false);
}

// Switches turning on/off displaying ship logos
void SwitchShipLogoEnable(int i) {
  bool enable = (i) ? true : false;
  basethis->EnableShipLogos(enable);
  DLLAddHUDMessage(DTXT_SHIPLOGOSMSG, (enable) ? DTXT_ENABLED : DTXT_DISABLED);
}

void SwitchAudioTauntsEnable(int i) {
  bool enable = (i) ? true : false;
  basethis->EnableAudioTaunts(enable);
  DLLAddHUDMessage(DTXT_AUDIOTAUNTSMSG, (enable) ? DTXT_ENABLED : DTXT_DISABLED);
}

// Rehashs the hosts.allow/deny lists
void RehashAllowDenyLists(int i) {
  if (basethis->GetLocalRole() != LR_SERVER)
    return;

  DLLAddHUDMessage(DTXT_REHASHINGMSG);

  basethis->RehashAllowDeny();
}

void SwitchLossPingIndicator(int i) {
  bool enable = (i) ? true : false;
  basethis->EnableLossGuage(enable);
  DLLAddHUDMessage(DTXT_LOSSINDICATOR, (enable) ? DTXT_ENABLED : DTXT_DISABLED);
}
