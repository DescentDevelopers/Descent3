/*
 * $Logfile: /DescentIII/main/huddisplay.cpp $
 * $Revision: 59 $
 * $Date: 4/19/00 5:16p $
 * $Author: Matt $
 *
 * Hud message and stat display file.
 *
 * $Log: /DescentIII/main/huddisplay.cpp $
 *
 * 59    4/19/00 5:16p Matt
 * From Duane for 1.4
 * Removed Mac OpenGL hack
 *
 * 58    10/21/99 6:39p Matt
 * Mac merge
 * This change is mostly setting & clearing the variable
 * use_opengl_1555_format around some blocks of code.  Duane says this
 * won't affect the Windows version.
 *
 * 57    7/28/99 4:11p Kevin
 * Mac
 *
 * 56    6/10/99 9:07p Samir
 * fixed weapon string name localization issues.
 *
 * 55    5/20/99 12:38p Matt
 * Fixed centering of the timer HUD display.
 *
 * 54    5/20/99 2:26a Matt
 * Don't use sprintf() for custom_text2 type items, so imbedded percents
 * work.
 *
 * 53    5/19/99 1:23p Matt
 * Moved the timer display down a bit, and made the time round up.
 *
 * 52    5/19/99 11:25a Matt
 * Added multisafe functions & Dallas actions for showing a timer on the
 * screen and adding custom HUD messages.
 *
 * 51    5/13/99 7:00p Samir
 * print score in co-op or robo-anarchy
 *
 * 50    5/10/99 10:22p Ardussi
 * changes to compile on Mac
 *
 * 49    5/09/99 6:21a Jeff
 * added missing restore of ZBuffer state
 *
 * 48    5/05/99 5:46p Samir
 * fixed RenderHUDGetTextWidth.  Hopefully didn't break for higher
 * resolutions.
 *
 * 47    4/29/99 5:08p Jason
 * fixed wrapping bug with RenderHudQuad
 *
 * 46    4/28/99 5:23a Jeff
 * use new inventory function that gets a list of inventory items to
 * display inventory and countermeasure on the hud (fixes weird selection
 * bug)
 *
 * 45    4/24/99 10:38p Samir
 * cleaned up hud text problems in 'small mode'
 *
 * 44    4/24/99 8:43p Samir
 * when shrinking screen hud messages get rendered in black region.
 *
 * 43    4/22/99 11:19a Samir
 * render hud text functions shouldn't nest formatted text.
 *
 * 42    4/21/99 9:29p Samir
 * don't draw score in multiplayer.
 *
 * 41    4/20/99 3:06p Jeff
 * fixed inventory displaying of non-usable items
 *
 * 40    4/20/99 1:14p Samir
 * non usable hud inventory.
 *
 * 39    4/20/99 11:47a Samir
 * shrinking hud fixes.
 *
 * 38    4/16/99 10:54p Jeff
 * fixed min() for Linux
 *
 * 37    4/06/99 6:02p Matt
 * Added score system
 *
 * 36    4/06/99 11:39a Samir
 * added more formatting options for hud items (added two other full
 * screen hud infs for different ships)
 *
 * 35    3/02/99 6:26p Samir
 * hires font madness.
 *
 * 34    3/01/99 12:55a Matt
 * Deleted some obsolete strings from the string table, and moved some
 * formatting info from the table to the code.
 *
 * 33    2/06/99 6:59p Jeff
 * created RenderHUDGetTextLineWidth and RenderHUDGetTextHeight
 *
 * 32    1/31/99 7:26p Matt
 * Renamed a bunch of functions to have HUD capitalized
 *
 * 31    1/29/99 2:08p Jeff
 * localization
 *
 * 30    11/01/98 1:57a Jeff
 * converted the vsprintf calls to use the Pvsprintf, which is a safe
 * vsprintf, no buffer overflows allowed
 *
 * 29    10/22/98 11:30p Matt
 * Centered the names of non-ammo weapon names with respect to the weapon
 * icon.
 *
 * 28    10/20/98 1:46p Samir
 * made text version of inventory.
 *
 * 27    10/14/98 4:27p Samir
 * improved HUD_ITEM_CUSTOMTEXT
 *
 * 26    10/13/98 4:16p Matt
 * Fixed stupid bug
 *
 * 25    10/13/98 12:56p Matt
 * Finished (hopefully) with the ammo system.  Added support for napalm
 * fuel.
 *
 * 24    9/30/98 4:34p Samir
 * hack to prevent display of lock warning at Gametime = 0.
 *
 * 23    8/25/98 7:08p Samir
 * hud primary weapon ammo.
 *
 * 22    8/07/98 12:54p Jeff
 * removed () in countermeasures :)
 *
 * 21    8/07/98 12:54p Jeff
 * inventory doesn't show count if count==1 countermeasure have () around
 * count
 *
 * 20    7/06/98 7:34p Samir
 * added countermeasures.
 *
 * 19    6/24/98 7:38p Samir
 * redid graphical/text/cockpit hud item management.
 *
 * 18    6/17/98 6:31p Samir
 * Added anti-grav warning when dying.
 *
 * 17    6/16/98 11:10a Jeff
 * fixed merges goofs
 *
 * 16    6/16/98 10:54a Jeff
 *
 * 15    6/15/98 6:54p Samir
 * added invulnerability and cloak effect.
 *
 * 14    6/15/98 2:14p Samir
 * afterburner hud image scale modified.
 *
 * 13    6/10/98 4:03p Samir
 * restored old hud config.
 *
 * 12    5/26/98 8:01p Samir
 * don't display placeholder small icon for weapon.
 *
 * 11    5/26/98 5:06p Samir
 * repositioned shield and energy sub hud items due to new design.
 *
 * 10    5/25/98 8:31p Samir
 * added call to render hud text with cerntering options.
 *
 * 9     5/25/98 6:38p Matt
 * Added needed include.
 *
 * 8     5/22/98 6:25p Samir
 * implemented a lot of hud items.
 *
 * 7     5/07/98 12:23p Samir
 * RenderHUDText functions added to hud.h
 *
 * 6     5/05/98 6:28p Samir
 * moved hud_x and hud_y macros to hud.h.
 *
 * 5     4/27/98 1:14p Jason
 * cleaned up afterburner stuff
 *
 * 4     4/24/98 5:32p Samir
 * added saturation option to DrawHudQuad.
 *
 * 3     4/24/98 8:02a Samir
 * display weapon names properly.
 *
 * 2     4/23/98 4:13a Samir
 * new hud system.
 *
 * 1     4/23/98 4:05a Samir
 * initial revision.
 *
 * $NoKeywords: $
 */

#include "pstring.h"
#include "hud.h"
#include "grtext.h"
#include "gamefont.h"
#include "renderer.h"
#include "pserror.h"
#include "player.h"
#include "game.h"
#include "weapon.h"
#include "gametexture.h"
#include "stringtable.h"
#include "ship.h"
#include "config.h"
#include "multi.h"
#include "render.h"

#include <stdio.h>
#include <stdarg.h>

#if defined(MACINTOSH)
#include "Macros.h"
#endif

#include <algorithm>

//////////////////////////////////////////////////////////////////////////////
//	Data

// renders the description for the current inventory item
void RenderHUDInventory(tHUDItem *item);

// renders the shield rating for the ship
void RenderHUDShieldValue(tHUDItem *item);

// renders the energy rating for the ship
void RenderHUDEnergyValue(tHUDItem *item);

//	draws the afterburner hud gauge.
void RenderHUDAfterburner(tHUDItem *item);

//	draws the primary weapon current in.
void RenderHUDPrimary(tHUDItem *item);

//	draw secondary weapon current in.
void RenderHUDSecondary(tHUDItem *item);

//	renders ship status.
void RenderHUDShipStatus(tHUDItem *item);

//	renders warnings like system failures or missile locks.
void RenderHUDWarnings(tHUDItem *item);

// render hud countermeasures
void RenderHUDCountermeasures(tHUDItem *item);

//	returns the weapon's icon.
inline int get_weapon_icon(int player, int type) {
  player_weapon *pw = &Players[player].weapon[type];
  weapon *wpn = GetWeaponFromIndex(player, pw->index);
  int bmp;

  if (wpn) {
    bmp = GetTextureBitmap(wpn->icon_handle, 0);
    if (bmp > 0)
      return bmp;
  }

  return HUD_resources.wpn_bmp;
}

void RenderHUDTextFlagsNoFormat(int flags, ddgr_color col, ubyte alpha, int sat_count, int x, int y, char *str);

//////////////////////////////////////////////////////////////////////////////
//	Hud item display routines.

void HudDisplayRouter(tHUDItem *item) {
  grtext_SetAlpha(192);
  grtext_SetFlags(0);

  switch (item->type) {
  case HUD_ITEM_PRIMARY:
    RenderHUDPrimary(item);
    break;

  case HUD_ITEM_SECONDARY:
    RenderHUDSecondary(item);
    break;

  case HUD_ITEM_SHIELD:
    RenderHUDShieldValue(item);
    break;

  case HUD_ITEM_ENERGY:
    RenderHUDEnergyValue(item);
    break;

  case HUD_ITEM_AFTERBURNER:
    RenderHUDAfterburner(item);
    break;

  case HUD_ITEM_INVENTORY:
    RenderHUDInventory(item);
    break;

  case HUD_ITEM_SHIPSTATUS:
    RenderHUDShipStatus(item);
    break;

  case HUD_ITEM_WARNINGS:
    RenderHUDWarnings(item);
    break;

  case HUD_ITEM_CNTRMEASURE:
    RenderHUDCountermeasures(item);
    break;

  case HUD_ITEM_CUSTOMTEXT:
    if (item->data.text) {
      RenderHUDText(item->color, item->alpha, item->saturation_count, item->x, item->y, item->data.text);
    }
    break;

  case HUD_ITEM_CUSTOMTEXT2:
    if (item->data.text) {
      RenderHUDTextFlagsNoFormat(0, item->color, item->alpha, item->saturation_count, 2, Game_window_h / 2,
                                 item->data.text);
    }
    break;

  case HUD_ITEM_CUSTOMIMAGE:
    break;

  default:
    Int3();
  }
}

//////////////////////////////////////////////////////////////////////////////
//	Hud item functions.

// renders the description for the current inventory item
void RenderHUDInventory(tHUDItem *item) {
  tInvenList ilist[MAX_UNIQUE_INVEN_ITEMS];
  int cur_sel;
  int count = Players[Player_num].inventory.GetInventoryItemList(ilist, MAX_UNIQUE_INVEN_ITEMS, &cur_sel);
  if (!count)
    return;

  int y;
  float img_w;

  if (cur_sel != -1) {
    ASSERT(cur_sel < MAX_UNIQUE_INVEN_ITEMS);

    // render currently selected item
    if (ilist[cur_sel].hud_name) {
      img_w = bm_w(HUD_resources.arrow_bmp, 0) * HUD_ARROW_SCALE;

      RenderHUDQuad(item->x, item->y + 2, img_w, bm_h(HUD_resources.arrow_bmp, 0) * HUD_ARROW_SCALE, 0, 0, 1, 1,
                    HUD_resources.arrow_bmp, item->alpha, item->saturation_count);

      if (ilist[cur_sel].amount > 1)
        RenderHUDText(item->color, item->alpha, item->saturation_count, item->x + img_w, item->y, "%s (%d)",
                      ilist[cur_sel].hud_name, ilist[cur_sel].amount);
      else
        RenderHUDText(item->color, item->alpha, item->saturation_count, item->x + img_w, item->y, "%s",
                      ilist[cur_sel].hud_name);
    }
  }

  // render non usable list.
  if (cur_sel != -1 && ilist[cur_sel].hud_name) {
    y = item->y + RenderHUDGetTextHeight(ilist[cur_sel].hud_name) + 6;
  } else {
    y = item->y + 16;
  }

  for (int i = 0; i < count; i++) {
    if (cur_sel == i)
      continue;
    if (ilist[i].selectable)
      continue;
    if (!ilist[i].hud_name)
      continue;

    if (ilist[i].amount > 1) {
      RenderHUDText(item->color, item->alpha, item->saturation_count, item->x + img_w, y, "%s (%d)", ilist[i].hud_name,
                    ilist[i].amount);
    } else {
      RenderHUDText(item->color, item->alpha, item->saturation_count, item->x + img_w, y, "%s", ilist[i].hud_name);
    }

    y += RenderHUDGetTextHeight(ilist[i].hud_name);
  }

  /*
  Inventory *inventory = &Players[Player_num].inventory;

  if(inventory->Size())	//make sure there is at least one item in the inventory
  {
          int size, y;
          int current_inv_pos = inventory->GetPos();
          float img_w = bm_w(HUD_resources.arrow_bmp,0)*HUD_ARROW_SCALE;
          char *name = inventory->GetPosName();

          if(name && inventory->IsUsable()) {
                  RenderHUDQuad(item->x, item->y+2, img_w, bm_h(HUD_resources.arrow_bmp,0)*HUD_ARROW_SCALE,
  0,0,1,1,HUD_resources.arrow_bmp, item->alpha, item->saturation_count); int count = inventory->GetPosCount();
                  if(count>1)
                          RenderHUDText(item->color, item->alpha, item->saturation_count, item->x+img_w, item->y, "%s
  (%d)", name,count); else RenderHUDText(item->color, item->alpha, item->saturation_count, item->x+img_w, item->y, "%s",
  name);
          }


  // render non usable list.
          inventory->ResetPos();
          size = inventory->Size();
          if (name) {
                  y = item->y + RenderHUDGetTextHeight(name) + 6;
          }
          else {
                  y = item->y + 16;
          }
          while (size)
          {
                  if (!inventory->IsUsable()) {
                          name = inventory->GetPosName();
                          if(name) {
                                  int count = inventory->GetPosCount();
                                  if(count>1) {
                                          RenderHUDText(item->color, item->alpha, item->saturation_count, item->x+img_w,
  y, "%s (%d)", name,count);
                                  }
                                  else {
                                          RenderHUDText(item->color, item->alpha, item->saturation_count, item->x+img_w,
  y, "%s", name);
                                  }
                                  y+= RenderHUDGetTextHeight(name);
                          }
                  }
                  inventory->NextPos(true);
                  size--;
          }

  // restore inventory position
          inventory->GotoPos(current_inv_pos);
  }
  */
}

// renders the shield rating for the ship
void RenderHUDShieldValue(tHUDItem *item) {
  const int SHIELD_NUM_X = 86, SHIELD_NUM_Y = 14;
  const int SHIELD_IMG_X = 21, SHIELD_IMG_Y = 57;
  const int WARNING_IMG_X = 110, WARNING_IMG_Y = 8;
  float alpha_mod = (Objects[Players[Player_num].objnum].shields) / (float)INITIAL_SHIELDS;

  if (alpha_mod > 1.0f)
    alpha_mod = 1.0f;

  if (item->stat & STAT_GRAPHICAL) {
    // GRAPHICAL VERSION
    //	display text and warning dot
    int img = (int)ceil((1.0f - alpha_mod - 0.1f) * NUM_SHIELD_GAUGE_FRAMES);
    float grscalex = (item->grscalex != 0.0f) ? item->grscalex : HUD_SHIELD_SCALE;
    float grscaley = (item->grscaley != 0.0f) ? item->grscaley : HUD_SHIELD_SCALE;

    RenderHUDText(item->color, item->alpha, item->saturation_count, item->x + SHIELD_NUM_X, item->y + SHIELD_NUM_Y,
                  "%03d", (int)Objects[Players[Player_num].objnum].shields);

    if (alpha_mod <= 0.20f) {
      RenderHUDQuad(item->x + WARNING_IMG_X, item->y + WARNING_IMG_Y, bm_w(HUD_resources.dot_bmp, 0) * HUD_DOT_SCALE,
                    bm_h(HUD_resources.dot_bmp, 0) * HUD_DOT_SCALE, 0, 0, 1, 1, HUD_resources.dot_bmp, item->alpha, 1);
    }

    if (img < NUM_SHIELD_GAUGE_FRAMES) {
      if (img < 0)
        img = 0;
      img = HUD_resources.shield_bmp[img];
      RenderHUDQuad(item->x + SHIELD_IMG_X, item->y + SHIELD_IMG_Y, bm_w(img, 0) * grscalex, bm_h(img, 0) * grscaley, 0,
                    0, 1, 1, img, item->alpha, item->saturation_count);
    }
  } else if (item->stat & STAT_SPECIAL) {
    RenderHUDText(item->color, item->alpha, item->saturation_count, item->x, item->y, "%03d",
                  (int)Objects[Players[Player_num].objnum].shields);

    if (alpha_mod <= 0.20f) {
      RenderHUDQuad(item->x + 30, item->y - 10, bm_w(HUD_resources.dot_bmp, 0) * HUD_DOT_SCALE,
                    bm_h(HUD_resources.dot_bmp, 0) * HUD_DOT_SCALE, 0, 0, 1, 1, HUD_resources.dot_bmp, item->alpha, 1);
    }
  } else {
    // TEXT VERSION
    RenderHUDText(item->tcolor, item->alpha, (alpha_mod <= .20f) ? item->saturation_count + 1 : item->saturation_count,
                  item->tx, item->ty, "%s: %03d", TXT_HUD_SHIELDS, (int)Objects[Players[Player_num].objnum].shields);
  }
}

// renders the energy rating for the ship
void RenderHUDEnergyValue(tHUDItem *item) {
  float normalized_energy = (float)Players[Player_num].energy / (float)INITIAL_ENERGY;

  // cap off energy to 1.0 normalized
  if (normalized_energy > 1.0f)
    normalized_energy = 1.0f;

  //	display text and warning dot
  if (item->stat & STAT_GRAPHICAL) {
    const int ENERGY_NUM_X = 15, ENERGY_NUM_Y = 14;
    //	int ENERGY_IMG_X = 3, ENERGY_IMG_Y = 74;
    //	int ENERGY_IMG_X2 = 119;
    const int WARNING_IMG_X = 6, WARNING_IMG_Y = 8;
    float grscalex = (item->grscalex != 0.0f) ? item->grscalex : HUD_ENERGY_SCALE;
    float grscaley = (item->grscaley != 0.0f) ? item->grscaley : HUD_ENERGY_SCALE;

    float img_w = bm_w(HUD_resources.energy_bmp, 0) * grscalex;
    float img_h = bm_h(HUD_resources.energy_bmp, 0) * grscaley;
    int img_energy_h;

    RenderHUDText(item->color, item->alpha, item->saturation_count, item->x + ENERGY_NUM_X, item->y + ENERGY_NUM_Y,
                  "%03d", (int)Players[Player_num].energy);

    if (normalized_energy <= 0.20f) {
      RenderHUDQuad(item->x + WARNING_IMG_X, item->y + WARNING_IMG_Y, bm_w(HUD_resources.dot_bmp, 0) * HUD_DOT_SCALE,
                    bm_h(HUD_resources.dot_bmp, 0) * HUD_DOT_SCALE, 0, 0, 1, 1, HUD_resources.dot_bmp, item->alpha, 1);
    }

    img_energy_h = (int)floor((normalized_energy * img_h) + 0.5f);

    //	draw energy gauge, showing how much energy below 100% you have. draw energy spent faded.
    RenderHUDQuad(item->x + item->xa, item->y + item->ya, img_w, img_h - img_energy_h, 0, 0, 1.0,
                  1.0f - normalized_energy, HUD_resources.energy_bmp, item->alpha / 4, 0);

    RenderHUDQuad(item->x + item->xb - img_w, item->y + item->yb, img_w, img_h - img_energy_h, 1.0, 0, 0,
                  1.0f - normalized_energy, HUD_resources.energy_bmp, item->alpha / 4, 0);

    RenderHUDQuad(item->x + item->xa, item->y + item->ya + (int)(img_h - img_energy_h), img_w, img_energy_h, 0,
                  1.0f - normalized_energy, 1, 1, HUD_resources.energy_bmp, item->alpha, item->saturation_count);

    RenderHUDQuad(item->x + item->xb - img_w, item->y + item->yb + (int)(img_h - img_energy_h), img_w, img_energy_h, 1,
                  1.0f - normalized_energy, 0, 1, HUD_resources.energy_bmp, item->alpha, item->saturation_count);
  } else if (item->stat & STAT_SPECIAL) {
    RenderHUDText(item->color, item->alpha, item->saturation_count, item->x, item->y, "%03d",
                  (int)Players[Player_num].energy);

    if (normalized_energy <= 0.20f) {
      RenderHUDQuad(item->x - 10, item->y - 10, bm_w(HUD_resources.dot_bmp, 0) * HUD_DOT_SCALE,
                    bm_h(HUD_resources.dot_bmp, 0) * HUD_DOT_SCALE, 0, 0, 1, 1, HUD_resources.dot_bmp, item->alpha, 1);
    }
  } else {
    // TEXT VERSION
    RenderHUDText(item->tcolor, item->alpha,
                  (normalized_energy <= .20f) ? item->saturation_count + 1 : item->saturation_count, item->tx, item->ty,
                  "%s: %03d", TXT_HUD_ENERGY, (int)Players[Player_num].energy);
  }
}

//	draws the afterburner hud gauge.
void RenderHUDAfterburner(tHUDItem *item) {
  float val = (Players[Player_num].afterburn_time_left / AFTERBURN_TIME);
  char str[8];

  if (item->stat & STAT_GRAPHICAL) {
    const int BURNER_NUM_X = 24;
    float grscalex = (item->grscalex != 0.0f) ? item->grscalex : HUD_BURN_SCALE;
    float grscaley = (item->grscaley != 0.0f) ? item->grscaley : HUD_BURN_SCALE;
    float img_w = bm_w(HUD_resources.afterburn_bmp, 0) * grscalex;
    float img_h = bm_h(HUD_resources.afterburn_bmp, 0) * grscaley;
    int img_burn_w = (int)floor((val * img_w) + 0.5f);

    //	draw afterburn spent.
    RenderHUDQuad(item->x + item->xa + img_burn_w, item->y + item->ya, img_w - img_burn_w, img_h, val, 0, 1, 1,
                  HUD_resources.afterburn_bmp, item->alpha / 4, 0);

    //	draw afterburn left
    RenderHUDQuad(item->x + item->xa, item->y + item->ya, img_burn_w, img_h, 0, 0, val, 1, HUD_resources.afterburn_bmp,
                  item->alpha, item->saturation_count);

    sprintf(str, "%d%%", (int)(val * 100.0f));
    RenderHUDText(item->color, item->alpha, item->saturation_count, item->x + BURNER_NUM_X, item->y, str);
  } else if (item->stat & STAT_SPECIAL) {
    sprintf(str, "%d%%", (int)(val * 100.0f));
    RenderHUDText(item->color, item->alpha, item->saturation_count, item->x, item->y, str);
  } else {
    // TEXT VERSION
    RenderHUDText(item->tcolor, item->alpha, (val <= .30f) ? item->saturation_count + 1 : item->saturation_count,
                  item->tx, item->ty, "%s: %d%%", TXT_HUD_AFTERBURNER, (int)(val * 100.0f));
  }
}

//	draws the primary weapon current in.
void RenderHUDPrimary(tHUDItem *item) {
  int index = Players[Player_num].weapon[PW_PRIMARY].index;
  ship *ship = &Ships[Players[Player_num].ship_index];
  otype_wb_info *wb = &ship->static_wb[index];
  const char *text = TXT(Static_weapon_names_msg[index]);
  float txt_w = (int)(grtext_GetLineWidth(text) / Hud_aspect_x) + 4;
  char ammo_string[10];

  if (!wb)
    return;

  if (wb->ammo_usage) {
    int ammo = Players[Player_num].weapon_ammo[index];
    if (ship->fire_flags[index] & SFF_TENTHS)
      sprintf(ammo_string, "%d.%d", ammo / 10, ammo % 10);
    else
      sprintf(ammo_string, "%d", ammo);
  }

  if (item->stat & STAT_GRAPHICAL) {
    const int WPN_INFO_W = 100;
    float grscalex = (item->grscalex != 0.0f) ? item->grscalex : HUD_WPN_SCALE;
    float grscaley = (item->grscaley != 0.0f) ? item->grscaley : HUD_WPN_SCALE;

    float img_w = bm_w(HUD_resources.wpn_bmp, 0) * grscalex;
    int icon;

    int y = item->y;

    if (wb->ammo_usage) {
      int w2 = (int)(grtext_GetLineWidth("00000") / Hud_aspect_x) + 2;
      RenderHUDText(item->color, item->alpha, item->saturation_count, item->x + WPN_INFO_W - img_w - w2, y + 13,
                    ammo_string);
    } else
      y += 13 / 2; // If just one line, move it down

    RenderHUDText(item->color, item->alpha, item->saturation_count, item->x + WPN_INFO_W - img_w - txt_w, y, "%s",
                  text);

    icon = get_weapon_icon(Player_num, PW_PRIMARY);

    if (icon != HUD_resources.wpn_bmp) {
      RenderHUDQuad(item->x + WPN_INFO_W - img_w, item->y, img_w, bm_h(icon, 0) * grscaley, 0, 0, 1, 1, icon,
                    item->alpha, item->saturation_count);
    }
  } else if (item->stat & STAT_SPECIAL) {

  } else {
    // TEXT VERSION
    if (wb->ammo_usage) {
      RenderHUDText(item->tcolor, item->alpha, item->saturation_count, item->tx, item->ty, "%s %s", text, ammo_string);
    } else {
      RenderHUDText(item->tcolor, item->alpha, item->saturation_count, item->tx, item->ty, "%s", text);
    }
  }
}

//	draw secondary weapon current in.
void RenderHUDSecondary(tHUDItem *item) {
  int index = Players[Player_num].weapon[PW_SECONDARY].index;
  ship *ship = &Ships[Players[Player_num].ship_index];
  otype_wb_info *wb = &ship->static_wb[index];
  weapon *wpn = GetWeaponFromIndex(Player_num, index);
  const char *text = TXT(Static_weapon_names_msg[index]);
  float txt_w = (int)(grtext_GetLineWidth(text) / Hud_aspect_x) + 2;

  if (item->stat & STAT_GRAPHICAL) {
    int icon;
    float grscalex = (item->grscalex != 0.0f) ? item->grscalex : HUD_WPN_SCALE;
    float grscaley = (item->grscaley != 0.0f) ? item->grscaley : HUD_WPN_SCALE;

    float img_w = bm_w(HUD_resources.wpn_bmp, 0) * grscalex;

    RenderHUDText(item->color, item->alpha, item->saturation_count, item->x + img_w + 2, item->y, "%s", text);
    if (wb->ammo_usage) {
      RenderHUDText(item->color, item->alpha, item->saturation_count, item->x + img_w + 16, item->y + 13, "%d",
                    Players[Player_num].weapon_ammo[Players[Player_num].weapon[PW_SECONDARY].index]);
    }

    icon = get_weapon_icon(Player_num, PW_SECONDARY);

    if (icon != HUD_resources.wpn_bmp) {
      RenderHUDQuad(item->x, item->y, img_w, bm_h(icon, 0) * grscaley, 0, 0, 1, 1, icon, item->alpha,
                    item->saturation_count);
    }
  } else if (item->stat & STAT_SPECIAL) {

  } else {
    //	TEXT VERSION
    RenderHUDText(item->tcolor, item->alpha, item->saturation_count, item->tx, item->ty, "%s %d", text,
                  Players[Player_num].weapon_ammo[Players[Player_num].weapon[PW_SECONDARY].index]);
  }
}

void RenderHUDShipStatus(tHUDItem *item) {
  float clk_time_frame, inv_time_frame;
  ubyte clk_alpha, inv_alpha;

  //	render text status
  if (Objects[Players[Player_num].objnum].effect_info->type_flags & EF_CLOAKED) {
    clk_time_frame = Objects[Players[Player_num].objnum].effect_info->cloak_time;
    if (clk_time_frame < HUD_CLOAKEND_TIME)
      clk_alpha = 128 - 127 * FixCos(65536.0f * (clk_time_frame - (int)clk_time_frame));
    else
      clk_alpha = 0;
  }
  if (Players[Player_num].flags & PLAYER_FLAGS_INVULNERABLE) {
    // do invulnerablity animation.
    inv_time_frame = (Gametime - (int)Gametime);
    inv_alpha = (255 * inv_time_frame);
  }

  if (item->stat & STAT_GRAPHICAL) {
    const int STATUS_IMG_X = 0, STATUS_IMG_Y = 68;
    const int STATUS_TXT_X = 8;
    const int STATUS_TXT_Y = -1, STATUS_TXT_Y2 = 9;
    float grscalex = (item->grscalex != 0.0f) ? item->grscalex : HUD_SHIP_SCALE;
    float grscaley = (item->grscaley != 0.0f) ? item->grscaley : HUD_SHIP_SCALE;

    int x = item->x + STATUS_IMG_X;
    int y = item->y + STATUS_IMG_Y;
    int w = bm_w(HUD_resources.ship_bmp, 0) * grscalex;
    int h = bm_w(HUD_resources.ship_bmp, 0) * grscaley;

    if (Objects[Players[Player_num].objnum].effect_info->type_flags & EF_CLOAKED) {
      RenderHUDQuad(x, y, w, h, 0, 0, 1, 1, HUD_resources.ship_bmp, clk_alpha, item->saturation_count);
      RenderHUDText(item->color, 255 - clk_alpha, item->saturation_count, item->x + STATUS_TXT_X,
                    item->y + STATUS_TXT_Y2, TXT_CLK);
    } else {
      // non cloaked ship
      RenderHUDQuad(x, y, w, h, 0, 0, 1, 1, HUD_resources.ship_bmp, item->alpha, item->saturation_count);
    }

    if (Players[Player_num].flags & PLAYER_FLAGS_INVULNERABLE) {
      x = x - (w * inv_time_frame * 0.5f);
      y = y - (h * inv_time_frame * 0.5f);

      RenderHUDQuad(x, y, (1.0f + inv_time_frame) * w, (1.0f + inv_time_frame) * h, 0, 0, 1, 1,
                    HUD_resources.invpulse_bmp, inv_alpha, item->saturation_count);
      RenderHUDText(item->color, item->alpha, item->saturation_count, item->x + STATUS_TXT_X + 2,
                    item->y + STATUS_TXT_Y, TXT_INV);
    }
  } else if (item->stat & STAT_SPECIAL) {

  } else {
    if (Players[Player_num].flags & PLAYER_FLAGS_INVULNERABLE)
      RenderHUDText(item->tcolor, 255 - clk_alpha, item->saturation_count, item->tx, item->ty, TXT_HUD_INVULN);
    if (Objects[Players[Player_num].objnum].effect_info->type_flags & EF_CLOAKED)
      RenderHUDText(item->tcolor, 255 - clk_alpha, item->saturation_count, item->tx, item->ty + 12, TXT_HUD_CLOAKED);
  }
}

//	renders warnings like system failures or missile locks.
void RenderHUDWarnings(tHUDItem *item) {
  object *playerobj = &Objects[Players[Player_num].objnum];

  if (item->stat & STAT_GRAPHICAL) {
    const int LOCK_IMG_X = 8, LOCK_IMG_Y = 2;
    const int ANTIGRAV_X = 8, ANTIGRAV_Y = 64;

    float img_w, img_h;
    float grscalex = (item->grscalex != 0.0f) ? item->grscalex : HUD_LOCK_SCALE;
    float grscaley = (item->grscaley != 0.0f) ? item->grscaley : HUD_LOCK_SCALE;

    if (Players[Player_num].last_homing_warning_sound_time == Gametime && Gametime > 0.0f) {
      //	draw graphical rep of missile lock warning.
      img_w = bm_w(HUD_resources.lock_bmp[0], 0) * grscalex;
      img_h = bm_h(HUD_resources.lock_bmp[1], 0) * grscaley;

      RenderHUDQuad(item->x + LOCK_IMG_X, item->y + LOCK_IMG_Y, img_w, img_h, 0, 0, 1, 1, HUD_resources.lock_bmp[0],
                    item->alpha, item->saturation_count);
      RenderHUDQuad(item->x + LOCK_IMG_X + img_w, item->y + LOCK_IMG_Y, img_w, img_h, 0, 0, 1, 1,
                    HUD_resources.lock_bmp[1], item->alpha, item->saturation_count);
    }

    if (playerobj->mtype.phys_info.flags & PF_GRAVITY) {
      //	draw graphical rep of missile lock warning.
      img_w = bm_w(HUD_resources.antigrav_bmp[0], 0) * grscalex;
      img_h = bm_h(HUD_resources.antigrav_bmp[1], 0) * grscaley;

      RenderHUDQuad(item->x + ANTIGRAV_X, item->y + ANTIGRAV_Y, img_w, img_h, 0, 0, 1, 1, HUD_resources.antigrav_bmp[0],
                    item->alpha, item->saturation_count);
      RenderHUDQuad(item->x + ANTIGRAV_X + img_w, item->y + ANTIGRAV_Y, img_w, img_h, 0, 0, 1, 1,
                    HUD_resources.antigrav_bmp[1], item->alpha, item->saturation_count);
    }
  } else {
    if (Players[Player_num].last_homing_warning_sound_time == Gametime)
      RenderHUDText(item->tcolor, item->alpha, item->saturation_count, item->tx, item->ty, TXT_HUDITM_LOCK);
    if (playerobj->mtype.phys_info.flags & PF_GRAVITY)
      RenderHUDText(item->tcolor, item->alpha, item->saturation_count, item->tx, item->ty + 12, TXT_HUDITM_ANTIGRAV);
  }
}

// render hud countermeasures
void RenderHUDCountermeasures(tHUDItem *item) {
  tInvenList ilist[MAX_UNIQUE_INVEN_ITEMS];
  int cur_sel;
  int count = Players[Player_num].counter_measures.GetInventoryItemList(ilist, MAX_UNIQUE_INVEN_ITEMS, &cur_sel);
  if (!count)
    return;

  if (cur_sel != -1) {
    ASSERT(cur_sel < MAX_UNIQUE_INVEN_ITEMS);

    // render currently selected item
    if (ilist[cur_sel].hud_name) {
      if (item->stat & STAT_GRAPHICAL)
        RenderHUDText(item->color, item->alpha, item->saturation_count, item->x, item->y, "%s %d",
                      ilist[cur_sel].hud_name, ilist[cur_sel].amount);
      else
        RenderHUDText(item->color, item->alpha, item->saturation_count, item->tx, item->ty, "%s %d",
                      ilist[cur_sel].hud_name, ilist[cur_sel].amount);
    }
  }
}

//////////////////////////////////////////////////////////////////////////////
//	Hud rendering functions

extern tFontTemplate Hud_font_template; // retain hud font template

// returns scaled line width
int RenderHUDGetTextLineWidth(char *string) {
  float aspect_x;
  int str_width_curfont = grtext_GetLineWidth(string);

  if (grtext_GetFont() == HUD_FONT) {
    aspect_x = ((float)grtext_GetLineWidthTemplate(&Hud_font_template, string)) / ((float)str_width_curfont);
  } else {
    aspect_x = 1.0f;
  }

  if (aspect_x == 1.0f) {
    aspect_x = aspect_x * DEFAULT_HUD_WIDTH / Max_window_w;
  }

  return (int)((str_width_curfont * aspect_x));
}

// returns scaled text height
int RenderHUDGetTextHeight(char *string) {
  float aspect_y;
  int str_height_curfont = grtext_GetHeight(string);

  if (grtext_GetFont() == HUD_FONT) {
    aspect_y = ((float)grtext_GetHeightTemplate(&Hud_font_template, string)) / ((float)str_height_curfont);
  } else {
    aspect_y = 1.0f;
  }

  return (int)((str_height_curfont * aspect_y));
}

void RenderHUDQuad(int x, int y, int w, int h, float u0, float v0, float u1, float v1, int bm, ubyte alpha,
                   int sat_count) {
  int i;

  rend_SetZBufferState(0);
  rend_SetTextureType(TT_LINEAR);
  rend_SetAlphaValue(alpha);
  rend_SetLighting(LS_NONE);
  rend_SetWrapType(WT_CLAMP);

  if (sat_count)
    rend_SetAlphaType(AT_SATURATE_TEXTURE);
  else
    rend_SetAlphaType(AT_CONSTANT_TEXTURE);

  x = HUD_X(x);
  y = HUD_Y(y);
  w = HUD_X(w);
  h = HUD_Y(h);

  for (i = 0; i < sat_count + 1; i++) {
    rend_DrawScaledBitmap(x, y, x + w, y + h, bm, u0, v0, u1, v1);
  }

  rend_SetZBufferState(1);
}

//	renders text, scaled, alphaed, saturated,
void RenderHUDText(ddgr_color col, ubyte alpha, int sat_count, int x, int y, char *fmt, ...) {
  va_list arglist;
  char buf[128];

  va_start(arglist, fmt);
  Pvsprintf(buf, 128, fmt, arglist);
  va_end(arglist);

  RenderHUDTextFlagsNoFormat(0, col, alpha, sat_count, x, y, buf);
}

//	renders text, scaled, alphaed, saturated,
void RenderHUDTextFlagsNoFormat(int flags, ddgr_color col, ubyte alpha, int sat_count, int x, int y, char *str) {
  int i;

  grtext_SetAlpha(alpha);

  if (sat_count)
    grtext_SetFlags(GRTEXTFLAG_SATURATE);
  else
    grtext_SetFlags(0);

  grtext_SetColor(col);

  x = HUD_X(x);
  y = HUD_Y(y);

  for (i = 0; i < sat_count + 1; i++) {
    if (flags & HUDTEXT_CENTERED)
      grtext_CenteredPrintf(0, y, str);
    else
      grtext_Puts(x, y, str);
  }
}

//	renders text, scaled, alphaed, saturated,
void RenderHUDTextFlags(int flags, ddgr_color col, ubyte alpha, int sat_count, int x, int y, char *fmt, ...) {
  va_list arglist;
  char buf[128];
  int i;

  va_start(arglist, fmt);
  Pvsprintf(buf, 128, fmt, arglist);
  va_end(arglist);

  grtext_SetAlpha(alpha);

  if (sat_count)
    grtext_SetFlags(GRTEXTFLAG_SATURATE);
  else
    grtext_SetFlags(0);

  grtext_SetColor(col);

  x = HUD_X(x);
  y = HUD_Y(y);

  for (i = 0; i < sat_count + 1; i++) {
    if (flags & HUDTEXT_CENTERED)
      grtext_CenteredPrintf(0, y, buf);
    else
      grtext_Puts(x, y, buf);
  }
}

// Show the score
void RenderHUDScore(tHUDItem *item) {
  char buf[100];
  int win_w;

  if (Game_mode & GM_MULTI) {
    if (!(Netgame.flags & NF_USE_ROBOTS)) {
      return;
    }
  }

  sprintf(buf, "%s: %d ", TXT_SCORE, Players[Player_num].score);

  win_w = (Max_window_w - Game_window_w) * (Hud_aspect_x);
  //	if (Game_video_resolution==RES_512X384) { win_w = win_w + 10; }

  int w = RenderHUDGetTextLineWidth(buf); // * win_w)/(Game_window_w);
  RenderHUDText(item->color, HUD_ALPHA, 0, item->x - w - win_w, item->y, buf);

  if (Score_added_timer > 0.0) {
    int text_height = grfont_GetHeight(HUD_FONT);
    sprintf(buf, "%d   ", Score_added);
    w = RenderHUDGetTextLineWidth(buf); // * win_w/Game_window_w;
    ubyte alpha = std::min<float>(HUD_ALPHA, HUD_ALPHA * 4 * Score_added_timer / SCORE_ADDED_TIME);
    RenderHUDText(item->color, alpha, 0, item->x - w - win_w, item->y + text_height, buf);
    Score_added_timer -= Frametime;
  }
}

extern float Osiris_TimerTimeRemaining(int handle);

// Show the timer
void RenderHUDTimer(tHUDItem *item) {
  float time;
  int min, secs;
  char buf[100];

  time = Osiris_TimerTimeRemaining(item->data.timer_handle);

  // If no more time, kill item
  if (time < 0) {
    item->stat = 0;
    return;
  }

  // round up
  time = ceil(time);

  min = time / 60;
  secs = time - (60 * min);

  sprintf(buf, "T-%d:%02d", min, secs);

  int h = grfont_GetHeight(HUD_FONT);
  RenderHUDTextFlagsNoFormat(HUDTEXT_CENTERED, item->color, HUD_ALPHA, 0, 0, h * 4, buf);
}

////////////////////////////////////////////////////////////////////////////////////////
void t_dirty_rect::fill(ddgr_color col) {
  int i;
  for (i = 2; i >= 0; i--) {
    rend_SetAlphaValue(255);
    if (r[i].l >= 0 && r[i].r >= 0) {
      rend_FillRect(col, r[i].l, r[i].t, r[i].r, r[i].b);
    }
    if (i > 0) {
      r[i].l = r[i - 1].l;
      r[i].t = r[i - 1].t;
      r[i].r = r[i - 1].r;
      r[i].b = r[i - 1].b;
    }
  }
  r[0].l = -1;
  r[0].t = -1;
  r[0].r = -1;
  r[0].b = -1;
}

void t_dirty_rect::reset() {
  int i;
  for (i = 0; i < 3; i++) {
    r[i].l = -1;
    r[i].t = -1;
    r[i].b = -1;
    r[i].r = -1;
  }
}
