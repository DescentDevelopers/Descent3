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

 * $Logfile: /DescentIII/main/gauges.cpp $
 * $Revision: 47 $
 * $Date: 4/19/00 5:13p $
 * $Author: Matt $
 *
 *	Gauge rendering
 *
 * $Log: /DescentIII/main/gauges.cpp $
 *
 * 47    4/19/00 5:13p Matt
 * From Duane for 1.4
 * Added checks, asserts, and fixes for bad return values
 * Removed Mac OpenGL hack
 *
 * 46    10/21/99 6:37p Matt
 * Mac merge
 * This change is mostly setting & clearing the variable
 * use_opengl_1555_format around some blocks of code.  Duane says this
 * won't affect the Windows version. (BTW, I think it would have been
 * cleaner to use a rend_set() function to do this.)
 *
 * 45    4/24/99 10:38p Samir
 * cleaned up hud text problems in 'small mode'
 *
 * 44    4/20/99 11:47a Samir
 * shrinking hud fixes.
 *
 * 43    3/04/99 8:07p Samir
 * render gauge text at correct scale.
 *
 * 42    1/31/99 7:26p Matt
 * Renamed a bunch of functions to have HUD capitalized
 *
 * 41    10/23/98 12:52p Samir
 * fixed bug in secondary monitors, projecting wrong point.
 *
 * 40    10/20/98 12:42p Matt
 * Made the small views work on the cockpit.
 *
 * 39    10/13/98 12:56p Matt
 * Finished (hopefully) with the ammo system.  Added support for napalm
 * fuel.
 *
 * 38    8/25/98 7:07p Samir
 * display ammo in primary monitor.
 *
 * 37    8/25/98 12:22p Jason
 * more changes for cockpits
 *
 * 36    8/24/98 3:25p Samir
 * took out reordering.
 *
 * 35    8/24/98 3:04p Samir
 * added reordering of monitor verts.
 *
 * 34    6/24/98 7:37p Samir
 * fixed energy/shield gauges (must still do invulnerability!)
 *
 * 33    6/16/98 10:38a Jeff
 * localization, strings pulled out to stringtable.h and d3.str
 *
 * 32    5/26/98 5:05p Samir
 * shield frames now contained in hud structure and share common graphics
 * between gauges and hud.
 *
 * 31    5/25/98 8:30p Samir
 * fixed bug when weapon gauges drawn in multiplayer.
 *
 * 30    5/25/98 6:38p Matt
 * Added needed include.
 *
 * 29    5/22/98 6:26p Samir
 * moved text over a bit on left monitor.
 *
 * 28    5/18/98 4:52p Samir
 * fixed hud image for weapons.
 *
 * 27    5/15/98 5:36p Samir
 * use correct font scale for lores screens.
 *
 * 26    5/05/98 6:28p Samir
 * gauges should render properly when cockpit is resized.
 *
 * 25    4/24/98 8:03a Samir
 * draw weapon names using short names, and pass alpha array to
 * drawgaugemonitor.
 *
 * 24    3/27/98 11:59a Samir
 * moved weapon hud text from hud to weapon monitor gauges.
 *
 * 23    3/22/98 5:03p Samir
 * updated cockpit.
 *
 * 22    3/20/98 8:23p Samir
 * Started implementing images.
 *
 * 21    3/18/98 6:24p Samir
 * Implemented monitor system.
 *
 * 20    3/17/98 2:40p Samir
 * reorg of cockpit/gauges and hud systems.
 *
 * 19    3/16/98 3:30p Samir
 * incremental checkin.
 *
 * 18    2/17/98 3:46p Matt
 * Revamped weapon system and got sounds for spray and fusion weapons
 * working.  Still need to implements getting continuous & cutoff sounds
 * from player ship.
 *
 * 17    2/11/98 4:54p Jeff
 * Moved the inventory into the Player struct
 *
 * 16    2/09/98 4:58p Jeff
 * Connected inventory to hud
 *
 * 15    2/03/98 12:13p Samir
 * Font shadowing support in grtext.
 *
 * 14    1/28/98 1:11p Jason
 * took shields field out of Player struct
 *
 * 13    1/26/98 2:38p Samir
 * Assert alpha is opaque for hud font.
 *
 * 12    1/23/98 12:07p Jason
 * made framerate display toggleable
 *
 * 11    1/12/98 5:40p Samir
 * Gauges placed differently.
 *
 * 10    1/12/98 5:23p Samir
 * Use new hud font for gauges.
 *
 * 9     12/29/97 5:47p Samir
 * Uses new text system and took out references to Game_viewport.
 *
 * 8     11/16/97 6:55p Samir
 * Initiated 'init' flag for gauge structure.
 *
 * 7     11/14/97 5:29p Samir
 * Now we pass four world coordinates of the 4 vertex monitor to a gauge
 * rendering function.
 *
 * 6     11/11/97 1:25p Samir
 * Added modified flag to tGauge and cleaned up gauge renderering a
 * little.
 *
 * 5     11/04/97 6:23p Samir
 * New Gauge interface.
 *
 * 4     10/29/97 12:33p Samir
 * Framework for RenderHudGauges
 *
 * 3     10/28/97 6:36p Samir
 * Gauge system for text controls functioning.
 *
 * 2     10/28/97 12:47p Samir
 * Gauges are drawn once per frame from GameLoop to a call to gauges.cpp
 *
 * 1     10/28/97 12:27p Samir
 * Initial revision
 *
 * $NoKeywords: $
 */

#include "gauges.h"
#include "gamefont.h"
#include "hud.h"
#include "game.h"
#include "renderer.h"
#include "render.h"
#include "player.h"
#include "polymodel.h"
#include "gametexture.h"
#include "bitmap.h"
#include "vclip.h"
#include "Inventory.h"
#include "cockpit.h"
#include "player.h"
#include "ship.h"
#include "3d.h"
#include "weapon.h"
#include "stringtable.h"

//////////////////////////////////////////////////////////////////////////////

struct tGauge {
  poly_model *cockpit; // cockpit model of gauge
  bsp_info *monitor;   // monitor in cockpit.
  int first_vert;
  bool functional; // is this gauge working?
  bool just_init;  // gauge just initialized?
  uint16_t mask;     // mask of stat for gauge
  int state;       // gauge dependent state data.
  union {          // data for gauge.
    int i;
    float f;
  } data;
  g3Point pts[4]; // points of monitor in view coordinates. (may be cached from last frame)
};

//	which monitors correspond to what faces on the cockpit model.
#define PRIMARY_MONITOR SOF_MONITOR1
#define SHIP_MONITOR SOF_MONITOR2
#define SECONDARY_MONITOR SOF_MONITOR3
#define ENERGY1_MONITOR SOF_MONITOR4
#define ENERGY2_MONITOR SOF_MONITOR5
#define SHIELD_MONITOR SOF_MONITOR6
#define AFTERBURN_MONITOR SOF_MONITOR7

// Hack vars for turning off the monitors
bool Disable_primary_monitor = 0, Disable_secondary_monitor = 0;

//////////////////////////////////////////////////////////////////////////////

//	Available gauges to be rendered.
static tStatMask Gauge_mask;

//	sets for next frame which gauges have been modified.
static tStatMask Gauge_mask_modified;

//	sets for next frame the functional state of each gauge (on/off).
static tGauge Gauge_list[NUM_GAUGES];

//	position of cockpit
static vector *Render_gauge_pos;

// matrix of cockpit
static matrix *Render_gauge_matrix;

//	normalized times of cockpit
static float *Render_normalized_times;

//	if gauges are moving in cockpit.
static bool Render_gauge_moving; // gauge is moving, but temporarily off.
static bool Render_gauge_reset;  //	set this if gauges will be moving, but are still active

// TODO: MTS: function not found
//	loads all shield bitmaps into memory
static void FreeShieldFrames();

// TODO: MTS: function not found
//	loads all ship bitmaps into memory
static void FreeShipFrames();

//	projects monitor coordinates to screen coordinates
static void RotateMonitorPosition(tGauge *gauge);

// TODO: MTS: function not found
// renders the current inventory item name at the x,y position
static void InventoryRenderGauge(int x, int y);

//	renders the primary monitor gauge
static void RenderPrimaryMonitor(tGauge *gauge, bool modified);

//	renders the secondary monitor gauge
static void RenderSecondaryMonitor(tGauge *gauge, bool modified);

//	renders the shield gauge monitor
static void RenderShieldMonitor(tGauge *gauge, bool modified);

//	renders the ship gauge monitor
static void RenderShipMonitor(tGauge *gauge, bool modified);

//	renders the ship gauge monitor
static void RenderEnergyMonitor(tGauge *gauge, int orient, bool modified);

//	renders the ship gauge monitor
static void RenderAfterburnMonitor(tGauge *gauge, bool modified);

//	renders a monitor style quad
static void DrawGaugeMonitor(g3Point *pts, int bm, float brightness, float *alphas);

//	renders a square texture onto the screen.
static void DrawGaugeQuad(g3Point *pts, int bm, float u0, float v0, float u1, float v1, uint8_t alpha, bool saturate);

//	renders a square texture onto the screen.
static void DrawGaugeQuad(g3Point *pts, int bm, uint8_t alpha = 255, bool saturate = false);

//	renders a flat poly onto the screen with given color
static void DrawGaugeQuadFlat(g3Point *pts, float r, float g, float b, uint8_t alpha);

//	renders a flat poly onto the screen with 4 colors (for each vertex)
static void DrawGaugeQuadFlat(g3Point *pts, float *r, float *g, float *b, uint8_t alpha);

// correctly orders monitor vertices based off of UVs
static int GetFirstVert(bsp_info *sm);

//	tells what gauge index is the gauge stat item.
static inline int GAUGE_INDEX(tStatMask mask) {
  int i;

  for (i = 0; i < NUM_GAUGES; i++)
    if (Gauge_list[i].mask == mask)
      return i;

  return -1;
}

//////////////////////////////////////////////////////////////////////////////
//	Initialization routines

//	initializes cockpit gauges
void InitGauges(uint16_t gauge_mask) {
  Gauge_mask = gauge_mask;
  Gauge_mask_modified = Gauge_mask;
  int gauge = 0;

  for (int i = 0; i < NUM_GAUGES; i++) {
    uint16_t mask = Gauge_mask & (1 << i);

    if (mask == STAT_SHIELDS) {
      Gauge_list[gauge].monitor = CockpitGetMonitorSubmodel(SHIELD_MONITOR);
    }
    if (mask == STAT_PRIMARYLOAD)
      Gauge_list[gauge].monitor = CockpitGetMonitorSubmodel(PRIMARY_MONITOR);
    else if (mask == STAT_SECONDARYLOAD)
      Gauge_list[gauge].monitor = CockpitGetMonitorSubmodel(SECONDARY_MONITOR);
    else if (mask == STAT_SHIP) {
      Gauge_list[gauge].monitor = CockpitGetMonitorSubmodel(SHIP_MONITOR);
    } else if (mask == STAT_ENERGY) {
      // initialize two gauges. one for left, and one for right energy monitor.
      Gauge_list[gauge].monitor = CockpitGetMonitorSubmodel(ENERGY1_MONITOR);
      Gauge_list[gauge].functional = false;
      Gauge_list[gauge].state = 0;
      Gauge_list[gauge].cockpit = CockpitGetPolyModel();
      Gauge_list[gauge].just_init = true;
      Gauge_list[gauge].mask = mask;
      Gauge_list[gauge].first_vert = GetFirstVert(Gauge_list[gauge].monitor);
      gauge++;
      Gauge_list[gauge].monitor = CockpitGetMonitorSubmodel(ENERGY2_MONITOR);
    } else if (mask == STAT_AFTERBURN) {
      Gauge_list[gauge].monitor = CockpitGetMonitorSubmodel(AFTERBURN_MONITOR);
    }

    if (mask) {
      Gauge_list[gauge].functional = false;
      Gauge_list[gauge].state = 0;
      Gauge_list[gauge].cockpit = CockpitGetPolyModel();
      Gauge_list[gauge].just_init = true;
      Gauge_list[gauge].mask = mask;
      Gauge_list[gauge].first_vert = GetFirstVert(Gauge_list[gauge].monitor);

      gauge++;
    }
  }
}

//	deinitializes cockpit gauges
void CloseGauges() {
  for (int i = 0; i < NUM_GAUGES; i++) {
    uint16_t mask = Gauge_mask & (1 << i);
    Gauge_list[i].monitor = NULL;
  }
  Gauge_mask = 0;
  Gauge_mask_modified = 0;
}

//	renders gauges
void RenderGauges(vector *cockpit_pos, matrix *cockpit_mat, float *normalized_time, bool moving, bool reset) {
  tGauge *gauge;
  float font_aspect_x;
  float font_aspect_y;

  Render_gauge_pos = cockpit_pos;
  Render_gauge_matrix = cockpit_mat;
  Render_normalized_times = normalized_time;
  Render_gauge_moving = moving;
  Render_gauge_reset = reset;

  grtext_Reset();
  grtext_SetFont(HUD_FONT);
  //	for lores screens, we use different fonts, so DONT SCALE.
  font_aspect_x = (float)Game_window_w / Max_window_w;
  font_aspect_y = (float)Game_window_h / Max_window_h;

  if (font_aspect_x <= 0.60) {
    grtext_SetFontScale(0.60f);
  } else if (font_aspect_x <= 0.80) {
    grtext_SetFontScale(0.80f);
  } else {
    grtext_SetFontScale(1.0f);
  }

  if (Gauge_mask & STAT_PRIMARYLOAD) {
    ASSERT(GAUGE_INDEX(STAT_PRIMARYLOAD) != -1);
    gauge = &Gauge_list[GAUGE_INDEX(STAT_PRIMARYLOAD)];
    if (gauge->monitor) {
      RotateMonitorPosition(gauge);
      RenderPrimaryMonitor(gauge, (Gauge_mask_modified & STAT_PRIMARYLOAD) ? true : false);
    }
    gauge->just_init = false;
  }
  if (Gauge_mask & STAT_SECONDARYLOAD) {
    ASSERT(GAUGE_INDEX(STAT_SECONDARYLOAD) != -1);
    gauge = &Gauge_list[GAUGE_INDEX(STAT_SECONDARYLOAD)];
    if (gauge->monitor) {
      RotateMonitorPosition(gauge);
      RenderSecondaryMonitor(gauge, (Gauge_mask_modified & STAT_SECONDARYLOAD) ? true : false);
    }
    gauge->just_init = false;
  }
  if (Gauge_mask & STAT_SHIELDS) {
    ASSERT(GAUGE_INDEX(STAT_SHIELDS) != -1);
    gauge = &Gauge_list[GAUGE_INDEX(STAT_SHIELDS)];
    if (gauge->monitor) {
      RotateMonitorPosition(gauge);
      RenderShieldMonitor(gauge, (Gauge_mask_modified & STAT_SHIELDS) ? true : false);
    }
    gauge->just_init = false;
  }
  if (Gauge_mask & STAT_SHIP) {
    ASSERT(GAUGE_INDEX(STAT_SHIP) != -1);
    gauge = &Gauge_list[GAUGE_INDEX(STAT_SHIP)];
    if (gauge->monitor) {
      RotateMonitorPosition(gauge);
      RenderShipMonitor(gauge, (Gauge_mask_modified & STAT_SHIP) ? true : false);
    }
    gauge->just_init = false;
  }
  if (Gauge_mask & STAT_ENERGY) {
    ASSERT(GAUGE_INDEX(STAT_ENERGY) != -1);
    gauge = &Gauge_list[GAUGE_INDEX(STAT_ENERGY)];
    if (gauge->monitor) {
      RotateMonitorPosition(gauge);
      RenderEnergyMonitor(gauge, 0, (Gauge_mask_modified & STAT_ENERGY) ? true : false);
    }
    gauge->just_init = false;
    gauge = &Gauge_list[GAUGE_INDEX(STAT_ENERGY) + 1];
    if (gauge->monitor) {
      RotateMonitorPosition(gauge);
      RenderEnergyMonitor(gauge, 1, (Gauge_mask_modified & STAT_ENERGY) ? true : false);
    }
    gauge->just_init = false;
  }
  if (Gauge_mask & STAT_AFTERBURN) {
    ASSERT(GAUGE_INDEX(STAT_AFTERBURN) != -1);
    gauge = &Gauge_list[GAUGE_INDEX(STAT_AFTERBURN)];
    if (gauge->monitor) {
      RotateMonitorPosition(gauge);
      RenderAfterburnMonitor(gauge, (Gauge_mask_modified & STAT_ENERGY) ? true : false);
    }
    gauge->just_init = false;
  }

  //	render all text
  grtext_Flush();

  Gauge_mask_modified = 0;
}

//	flags certain gauges to be modified next frame.
void FlagGaugesModified(tStatMask mask_modified) { Gauge_mask_modified |= mask_modified; }

//	sets whether the gauges are functional
void FlagGaugesFunctional(tStatMask mask) {
  uint16_t i = 0x8000, j = NUM_GAUGES;

  while (i) {
    if (mask & i)
      Gauge_list[j - 1].functional = true;
    i = i >> 1;
    j--;
  }
}

//	sets whether the gauges are functional
void FlagGaugesNonfunctional(tStatMask mask) {
  uint16_t i = 0x8000, j = NUM_GAUGES;

  while (i) {
    if (mask & i)
      Gauge_list[j - 1].functional = false;
    i = i >> 1;
    j--;
  }
}

//	renders the primary monitor gauge
//		this will normally render the primary weapon currently selected
//		can render another view though.
//	renders the primary monitor gauge

static inline int get_weapon_hud_image(int player, int type) {
  player_weapon *pw = &Players[player].weapon[type];
  weapon *wpn = GetWeaponFromIndex(player, pw->index);

  return (wpn ? wpn->hud_image_handle : BAD_BITMAP_HANDLE);
}

void RenderPrimaryMonitor(tGauge *gauge, bool modified) {
  if (Disable_primary_monitor)
    return;

  if (gauge->functional) {
    float alphas[4];
    alphas[0] = 1.0f;
    alphas[1] = 1.0f;
    alphas[2] = 1.0f;
    alphas[3] = 1.0f;

    int hud_image = get_weapon_hud_image(Player_num, PW_PRIMARY);

    DrawGaugeMonitor(gauge->pts, hud_image, 0.5f, alphas);

    if (!Render_gauge_moving) {
      int x1, y;
      int index = Players[Player_num].weapon[PW_PRIMARY].index;
      ship *ship = &Ships[Players[Player_num].ship_index];
      otype_wb_info *wb = &ship->static_wb[index];

      gauge->pts[0].p3_flags &= (~PF_PROJECTED);
      g3_ProjectPoint(&gauge->pts[0]);
      x1 = gauge->pts[0].p3_sx;
      y = gauge->pts[0].p3_sy;

      grtext_SetFlags(0);
      grtext_SetFancyColor(GR_RGB(0, 180, 0), GR_RGB(0, 180, 0), GR_RGB(0, 180, 0), GR_RGB(0, 180, 0));
      grtext_Printf(x1 + HUD_X(30), y + HUD_Y(10), "%s", TXT(Static_weapon_ckpt_names[index][0]));
      grtext_SetFlags(GRTEXTFLAG_SATURATE);
      grtext_Printf(x1 + HUD_X(30), y + HUD_Y(10), "%s", TXT(Static_weapon_ckpt_names[index][0]));

      if (strlen(TXT(Static_weapon_ckpt_names[Players[Player_num].weapon[PW_PRIMARY].index][1]))) {
        grtext_SetFlags(0);
        grtext_Printf(x1 + HUD_X(35), y + HUD_Y(20), "%s", TXT(Static_weapon_ckpt_names[index][1]));
        grtext_SetFlags(GRTEXTFLAG_SATURATE);
        grtext_Printf(x1 + HUD_X(35), y + HUD_Y(20), "%s", TXT(Static_weapon_ckpt_names[index][1]));
        y = y + HUD_Y(10);
      }

      if (wb && wb->ammo_usage) {
        int ammo = Players[Player_num].weapon_ammo[index];
        grtext_SetFlags(0);
        if (ship->fire_flags[index] & SFF_TENTHS) {
          grtext_Printf(x1 + HUD_X(40), y + HUD_Y(20), "%d.%d", ammo / 10, ammo % 10);
          grtext_SetFlags(GRTEXTFLAG_SATURATE);
          grtext_Printf(x1 + HUD_X(40), y + HUD_Y(20), "%d.%d", ammo / 10, ammo % 10);
        } else {
          grtext_Printf(x1 + HUD_X(40), y + HUD_Y(20), "%d", ammo);
          grtext_SetFlags(GRTEXTFLAG_SATURATE);
          grtext_Printf(x1 + HUD_X(40), y + HUD_Y(20), "%d", ammo);
        }
      }
    }
  }
}

//	renders the secondary monitor gauge
void RenderSecondaryMonitor(tGauge *gauge, bool modified) {
  if (Disable_secondary_monitor)
    return;

  if (gauge->functional) {
    float alphas[4];

    alphas[0] = 1.0f;
    alphas[1] = 1.0f;
    alphas[2] = 1.0f;
    alphas[3] = 1.0f;

    int hud_image = get_weapon_hud_image(Player_num, PW_SECONDARY);

    DrawGaugeMonitor(gauge->pts, hud_image, 0.5f, alphas);

    if (!Render_gauge_moving) {
      int x1, y;

      gauge->pts[0].p3_flags &= (~PF_PROJECTED);
      g3_ProjectPoint(&gauge->pts[0]);
      x1 = gauge->pts[0].p3_sx;
      y = gauge->pts[0].p3_sy;

      grtext_SetFancyColor(GR_RGB(0, 180, 0), GR_RGB(0, 180, 0), GR_RGB(0, 180, 0), GR_RGB(0, 180, 0));
      grtext_SetFlags(0);
      grtext_Printf(x1 + HUD_X(5), y + HUD_Y(10), "%s",
                    TXT(Static_weapon_ckpt_names[Players[Player_num].weapon[PW_SECONDARY].index][0]));
      grtext_SetFlags(GRTEXTFLAG_SATURATE);
      grtext_Printf(x1 + HUD_X(5), y + HUD_Y(10), "%s",
                    TXT(Static_weapon_ckpt_names[Players[Player_num].weapon[PW_SECONDARY].index][0]));
      if (strlen(TXT(Static_weapon_ckpt_names[Players[Player_num].weapon[PW_SECONDARY].index][1]))) {
        grtext_SetFlags(0);
        grtext_Printf(x1 + HUD_X(5), y + HUD_Y(20), "%s",
                      TXT(Static_weapon_ckpt_names[Players[Player_num].weapon[PW_SECONDARY].index][1]));
        grtext_SetFlags(GRTEXTFLAG_SATURATE);
        grtext_Printf(x1 + HUD_X(5), y + HUD_Y(20), "%s",
                      TXT(Static_weapon_ckpt_names[Players[Player_num].weapon[PW_SECONDARY].index][1]));
        y = y + HUD_Y(10);
      }
      grtext_SetFlags(0);
      grtext_Printf(x1 + HUD_X(15), y + HUD_Y(20), "%d",
                    Players[Player_num].weapon_ammo[Players[Player_num].weapon[PW_SECONDARY].index]);
      grtext_SetFlags(GRTEXTFLAG_SATURATE);
      grtext_Printf(x1 + HUD_X(15), y + HUD_Y(20), "%d",
                    Players[Player_num].weapon_ammo[Players[Player_num].weapon[PW_SECONDARY].index]);
    }
  }
}

//	renders the shield gauge monitor
void RenderShieldMonitor(tGauge *gauge, bool modified) {
  float alpha_mod = (Objects[Players[Player_num].objnum].shields) / (float)INITIAL_SHIELDS;

  if (alpha_mod > 1.0f)
    alpha_mod = 1.0f;

  int img = (int)ceil((1.0f - alpha_mod - 0.1f) * NUM_SHIELD_GAUGE_FRAMES);
  if (img < NUM_SHIELD_GAUGE_FRAMES) {
    if (img < 0)
      img = 0;
    img = HUD_resources.shield_bmp[img];

    if (gauge->functional)
      DrawGaugeQuad(gauge->pts, img, 255);
  }
}

//	renders the ship gauge monitor
void RenderEnergyMonitor(tGauge *gauge, int orient, bool modified) {
  if (gauge->functional) {
    float img_h = bm_h(HUD_resources.energy_bmp, 0);
    float normalized_energy = (float)Players[Player_num].energy / (float)INITIAL_ENERGY;
    g3Point used_pts[4], left_pts[4];
    float img_energy_h;
    float hgt_scalar;
    int i;

    if (normalized_energy > 1.0f)
      normalized_energy = 1.0f;

    img_energy_h = (normalized_energy * img_h);

    for (i = 0; i < 4; i++) {
      used_pts[i] = gauge->pts[i];
      left_pts[i] = gauge->pts[i];
    }

    //	draw energy gauge, showing how much energy below 100% you have. draw energy spent faded.
    hgt_scalar = img_energy_h / img_h;
    used_pts[3].p3_vec = used_pts[3].p3_vec - (used_pts[3].p3_vec - used_pts[0].p3_vec) * hgt_scalar;
    used_pts[2].p3_vec = used_pts[2].p3_vec - (used_pts[2].p3_vec - used_pts[1].p3_vec) * hgt_scalar;
    used_pts[3].p3_vecPreRot =
        used_pts[3].p3_vecPreRot - (used_pts[3].p3_vecPreRot - used_pts[0].p3_vecPreRot) * hgt_scalar;
    used_pts[2].p3_vecPreRot =
        used_pts[2].p3_vecPreRot - (used_pts[2].p3_vecPreRot - used_pts[1].p3_vecPreRot) * hgt_scalar;

    if (!orient)
      DrawGaugeQuad(used_pts, HUD_resources.energy_bmp, 0, 0, 1, 1.0f - normalized_energy, 64, 0);
    else
      DrawGaugeQuad(used_pts, HUD_resources.energy_bmp, 1, 0, 0, 1.0f - normalized_energy, 64, 0);

    left_pts[0].p3_vec = left_pts[0].p3_vec + (left_pts[3].p3_vec - left_pts[0].p3_vec) * (1.0f - hgt_scalar);
    left_pts[1].p3_vec = left_pts[1].p3_vec + (left_pts[2].p3_vec - left_pts[1].p3_vec) * (1.0f - hgt_scalar);
    left_pts[0].p3_vecPreRot =
        left_pts[0].p3_vecPreRot + (left_pts[3].p3_vecPreRot - left_pts[0].p3_vecPreRot) * (1.0f - hgt_scalar);
    left_pts[1].p3_vecPreRot =
        left_pts[1].p3_vecPreRot + (left_pts[2].p3_vecPreRot - left_pts[1].p3_vecPreRot) * (1.0f - hgt_scalar);

    if (!orient)
      DrawGaugeQuad(left_pts, HUD_resources.energy_bmp, 0, 1.0f - normalized_energy, 1, 1, 255, 0);
    else
      DrawGaugeQuad(left_pts, HUD_resources.energy_bmp, 1, 1.0f - normalized_energy, 0, 1, 255, 0);
  }
}

//	renders the ship gauge monitor
void RenderAfterburnMonitor(tGauge *gauge, bool modified) {
  g3Point used_pts[4], left_pts[4];

  if (gauge->functional) {
    float img_w = bm_w(HUD_resources.afterburn_bmp, 0);
    float val = (Players[Player_num].afterburn_time_left / AFTERBURN_TIME);
    float img_burn_w = (val * img_w);
    float w_scalar;
    int i;

    for (i = 0; i < 4; i++) {
      used_pts[i] = gauge->pts[i];
      left_pts[i] = gauge->pts[i];
    }

    //	draw energy gauge,
    //	showing how much energy below 100% you have. draw energy spent faded.
    w_scalar = img_burn_w / img_w;

    used_pts[3].p3_vec = used_pts[3].p3_vec + (used_pts[2].p3_vec - used_pts[3].p3_vec) * w_scalar;
    used_pts[0].p3_vec = used_pts[0].p3_vec + (used_pts[1].p3_vec - used_pts[0].p3_vec) * w_scalar;
    used_pts[3].p3_vecPreRot =
        used_pts[3].p3_vecPreRot + (used_pts[2].p3_vecPreRot - used_pts[3].p3_vecPreRot) * w_scalar;
    used_pts[0].p3_vecPreRot =
        used_pts[0].p3_vecPreRot + (used_pts[1].p3_vecPreRot - used_pts[0].p3_vecPreRot) * w_scalar;
    DrawGaugeQuad(used_pts, HUD_resources.afterburn_bmp, val, 0, 1, 1, 64, 0);

    left_pts[1].p3_vec = left_pts[1].p3_vec - (left_pts[1].p3_vec - left_pts[0].p3_vec) * (1.0f - w_scalar);
    left_pts[2].p3_vec = left_pts[2].p3_vec - (left_pts[2].p3_vec - left_pts[3].p3_vec) * (1.0f - w_scalar);
    left_pts[1].p3_vecPreRot =
        left_pts[1].p3_vecPreRot - (left_pts[1].p3_vecPreRot - left_pts[0].p3_vecPreRot) * (1.0f - w_scalar);
    left_pts[2].p3_vecPreRot =
        left_pts[2].p3_vecPreRot - (left_pts[2].p3_vecPreRot - left_pts[3].p3_vecPreRot) * (1.0f - w_scalar);
    DrawGaugeQuad(left_pts, HUD_resources.afterburn_bmp, 0, 0, val, 1, 255, 0);
  }
}

//	renders the ship gauge monitor
#define GAUGE_INVSHIPRING_DELTA 0.06f
void RenderShipMonitor(tGauge *gauge, bool modified) {
  if (!gauge->functional)
    return;

  uint8_t alpha = 255;
  if (Objects[Players[Player_num].objnum].effect_info->type_flags & EF_CLOAKED) {
    float time_frame = Objects[Players[Player_num].objnum].effect_info->cloak_time;
    if (time_frame < HUD_CLOAKEND_TIME) {
      alpha = 128 - 127 * FixCos(65536.0f * (time_frame - (int)time_frame));
    } else {
      alpha = 0;
    }
  }

  DrawGaugeQuad(gauge->pts, HUD_resources.ship_bmp, alpha);
  DrawGaugeQuad(gauge->pts, HUD_resources.ship_bmp, alpha, true);

  if (Players[Player_num].flags & PLAYER_FLAGS_INVULNERABLE) {
    // get to the view matrix so we have direction vectors to move along
    matrix view_matrix;
    g3_GetUnscaledMatrix(&view_matrix);

    float inv_time_frame = (Gametime - (int)Gametime);
    float inv_alpha = (uint8_t)(255 * (1.0f - (inv_time_frame / 2.0f)));
    g3Point invpts[4];
    int i;

    // do invulnerability animation.
    for (i = 0; i < 4; i++) {
      invpts[i] = gauge->pts[i];
    }

    float amount = (inv_time_frame * GAUGE_INVSHIPRING_DELTA);
    invpts[0].p3_x -= amount;
    invpts[0].p3_y += amount;
    invpts[1].p3_x += amount;
    invpts[1].p3_y += amount;
    invpts[2].p3_x += amount;
    invpts[2].p3_y -= amount;
    invpts[3].p3_x -= amount;
    invpts[3].p3_y -= amount;

    invpts[0].p3_vecPreRot = invpts[0].p3_vecPreRot - (view_matrix.rvec * amount) + (view_matrix.uvec * amount);
    invpts[1].p3_vecPreRot = invpts[1].p3_vecPreRot + (view_matrix.rvec * amount) + (view_matrix.uvec * amount);
    invpts[2].p3_vecPreRot = invpts[2].p3_vecPreRot + (view_matrix.rvec * amount) - (view_matrix.uvec * amount);
    invpts[3].p3_vecPreRot = invpts[3].p3_vecPreRot - (view_matrix.rvec * amount) - (view_matrix.uvec * amount);

    DrawGaugeQuad(invpts, HUD_resources.invpulse_bmp, inv_alpha);
    DrawGaugeQuad(invpts, HUD_resources.invpulse_bmp, inv_alpha, true);
  }
}

//	projects monitor coordinates to screen coordinates
void RotateMonitorPosition(tGauge *gauge) {
  ASSERT(gauge->monitor);
  ASSERT(gauge->monitor->num_faces == 1);
  int subnum = gauge->monitor - gauge->cockpit->submodel;

  if (gauge->monitor->faces[0].nverts != 4)
    Int3();

  for (int j = 0; j < gauge->monitor->faces[0].nverts; j++) {
    //	saved the view coordinates of the monitor if the gauge is moving and/or was just initialized.
    //	if gauge is not moving, then just use these saved points and make sure to mark them as NOT PROJECTED.
    vector vpt;
    vector vert;
    vpt = gauge->monitor->verts[gauge->monitor->faces[0].vertnums[(gauge->first_vert + j) % 4]];
    // JEFFNOTE: Since we are using hardware transforms, we want to update the points all the time
    // if (Render_gauge_moving || gauge->just_init || Render_gauge_reset)
    {
      GetPolyModelPointInWorld(&vert, gauge->cockpit, Render_gauge_pos, Render_gauge_matrix, subnum,
                               Render_normalized_times, &vpt);
      g3_RotatePoint(&gauge->pts[j], &vert);
    }
  }
}

//////////////////////////////////////////////////////////////////////////////
//	Graphic primatives for hud

//	renders a monitor style quad
void DrawGaugeMonitor(g3Point *pts, int bm, float brightness, float *alphas) {
  g3Point *pntlist[4];
  g3Point pnts[4];

  if (bm == -1)
    bm = BAD_BITMAP_HANDLE;

  for (int i = 0; i < 4; i++) {
    pnts[i] = pts[i];
    pnts[i].p3_flags = PF_UV | PF_RGBA | (pts[i].p3_flags & PF_ORIGPOINT);
    pntlist[i] = &pnts[i];
  }

  pnts[0].p3_u = 0;
  pnts[0].p3_v = 0;
  pnts[0].p3_a = alphas[0];

  pnts[1].p3_u = 1;
  pnts[1].p3_v = 0;
  pnts[1].p3_a = alphas[1];

  pnts[2].p3_u = 1;
  pnts[2].p3_v = 1;
  pnts[2].p3_a = alphas[2];

  pnts[3].p3_u = 0;
  pnts[3].p3_v = 1;
  pnts[3].p3_a = alphas[3];

  rend_SetZBufferState(0);
  rend_SetTextureType(TT_LINEAR);
  rend_SetLighting(LS_NONE);
  rend_SetAlphaValue(brightness * 255);

  rend_SetAlphaType(AT_CONSTANT_VERTEX);

  g3_DrawPoly(4, pntlist, bm);
}

//	renders a square texture onto the screen.
void DrawGaugeQuad(g3Point *pts, int bm, uint8_t alpha, bool saturate) {
  DrawGaugeQuad(pts, bm, 0, 0, 1, 1, alpha, saturate);
}

//	renders a square texture onto the screen.
void DrawGaugeQuad(g3Point *pts, int bm, float u0, float v0, float u1, float v1, uint8_t alpha, bool saturate) {
  g3Point *pntlist[4];
  g3Point pnts[4];

  if (bm == -1)
    bm = BAD_BITMAP_HANDLE;

  for (int i = 0; i < 4; i++) {
    pnts[i] = pts[i];
    pnts[i].p3_flags = PF_UV | (pts[i].p3_flags & PF_ORIGPOINT);
    pntlist[i] = &pnts[i];
  }

  pnts[0].p3_u = u0;
  pnts[0].p3_v = v0;

  pnts[1].p3_u = u1;
  pnts[1].p3_v = v0;

  pnts[2].p3_u = u1;
  pnts[2].p3_v = v1;

  pnts[3].p3_u = u0;
  pnts[3].p3_v = v1;

  rend_SetZBufferState(0);
  rend_SetTextureType(TT_LINEAR);
  rend_SetLighting(LS_NONE);
  rend_SetAlphaValue(alpha);

  if (!saturate)
    rend_SetAlphaType(AT_CONSTANT_TEXTURE);
  else
    rend_SetAlphaType(AT_SATURATE_TEXTURE);

  g3_DrawPoly(4, pntlist, bm);
}

//	renders a flat poly onto the screen with given color
void DrawGaugeQuadFlat(g3Point *pts, float r, float g, float b, uint8_t alpha) {
  float ar[4] = {r, r, r, r}, ag[4] = {g, g, g, g}, ab[4] = {b, b, b, b};

  DrawGaugeQuadFlat(pts, ar, ag, ab, alpha);
}

//	renders a flat poly onto the screen with 4 colors (for each vertex)
void DrawGaugeQuadFlat(g3Point *pts, float *r, float *g, float *b, uint8_t alpha) {
  g3Point *pntlist[4];
  g3Point pnts[4];

  for (int i = 0; i < 4; i++) {
    pnts[i] = pts[i];
    pnts[i].p3_flags = PF_RGBA | (pts[i].p3_flags & PF_ORIGPOINT);
    pntlist[i] = &pnts[i];
  }

  pnts[0].p3_r = r[0];
  pnts[0].p3_g = g[0];
  pnts[0].p3_b = b[0];

  pnts[1].p3_r = r[1];
  pnts[1].p3_g = g[1];
  pnts[1].p3_b = b[1];

  pnts[2].p3_r = r[2];
  pnts[2].p3_g = g[2];
  pnts[2].p3_b = b[2];

  pnts[3].p3_r = r[3];
  pnts[3].p3_g = g[3];
  pnts[3].p3_b = b[3];

  rend_SetZBufferState(0);
  rend_SetLighting(LS_GOURAUD);
  rend_SetTextureType(TT_FLAT);
  rend_SetColorModel(CM_RGB);
  rend_SetAlphaType(AT_CONSTANT);
  rend_SetAlphaValue(alpha);

  g3_DrawPoly(4, pntlist, 0);
}

// Returns the vertex index that corresponds to the upper-left vertex
int GetFirstVert(bsp_info *sm) {
  ASSERT(sm->num_faces == 1 && sm->faces[0].nverts == 4);
  int i;
  polyface *face = &sm->faces[0];
  int first_vert = -1;

  for (i = 0; i < 4 && first_vert == -1; i++) {
    if (sm->alpha[face->vertnums[i]] < .5)
      first_vert = i;
  }

  if (first_vert < 0)
    first_vert = 0; // If no alpha info, just set to first vertex

  return first_vert;
}

// Returns the coordinates of the specified cockpit monitor
// Parameter:	window - 0 means primary monitor, 1 means secondary
//					x0,y0,x1,y1 - these are filled in with the coordinates of the montiro
// Returns: true if got coords, false if the monitor was animating
bool GetCockpitWindowCoords(int window, int *left, int *top, int *right, int *bot) {
  if (Render_gauge_moving)
    return 0;

  tGauge *gauge;

  gauge = &Gauge_list[GAUGE_INDEX(window ? STAT_SECONDARYLOAD : STAT_PRIMARYLOAD)];

  if (gauge->monitor) {

#define HUD_RENDER_ZOOM 0.56f

    StartFrame(false);
    g3_StartFrame(&Viewer_object->pos, &Viewer_object->orient, HUD_RENDER_ZOOM);

    for (int i = 0; i < 2; i++) {
      gauge->pts[i * 2].p3_flags &= (~PF_PROJECTED);
      g3_ProjectPoint(&gauge->pts[i * 2]);
    }

    *left = gauge->pts[0].p3_sx;
    *top = gauge->pts[0].p3_sy;
    *right = gauge->pts[2].p3_sx + 2;
    *bot = gauge->pts[2].p3_sy + 2;

    g3_EndFrame();
    EndFrame();

    return 1;
  }

  return 0;
}
