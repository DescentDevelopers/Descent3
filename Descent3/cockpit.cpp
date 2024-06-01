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

 * $Logfile: /DescentIII/main/cockpit.cpp $
 * $Revision: 50 $
 * $Date: 8/10/99 5:10p $
 * $Author: Jeff $
 *
 * Cockpit code
 *
 * $Log: /DescentIII/main/cockpit.cpp $
 *
 * 50    8/10/99 5:10p Jeff
 * added missing call to FreeReticle()
 *
 * 49    7/28/99 4:21p Kevin
 * Mac!
 *
 * 48    5/10/99 10:17p Ardussi
 * changes to compile on Mac
 *
 * 47    5/05/99 5:48p Samir
 * initialize reticle in  InitCockpit.
 *
 * 46    4/21/99 2:15p Samir
 * table file filter fixes.
 *
 * 45    4/17/99 5:43a Jeff
 * Changes to get it to compile in Linux
 *
 * 44    4/06/99 1:50p Jason
 * added new mass driver effect
 *
 * 43    4/01/99 4:50p Matt
 * Took out Warning() function, chaning those calls to mprintf()
 *
 * 42    3/30/99 2:39p Samir
 * added custom config file for fullscreen hud and hacked mass driver
 * reticle in special reticle code.
 *
 * 41    3/23/99 10:25a Samir
 * shake cockpit a little more.
 *
 * 40    2/11/99 8:28p Matt
 * Fixed stupid bugs
 *
 * 39    2/11/99 4:02p Matt
 * Flaged a bunch of files for TableParse
 *
 * 38    2/05/99 7:03p Jeff
 * table file parsing macros put in
 *
 * 37    1/31/99 7:25p Matt
 * Renamed a bunch of functions to have HUD capitalized
 *
 * 36    10/20/98 12:42p Matt
 * Made the small views work on the cockpit.
 *
 * 35    10/14/98 1:02p Jason
 * fixed FindSoundName issues
 *
 * 34    9/03/98 4:26p Samir
 * gauges are off this time until cockpit is in functional mode.
 *
 * 33    8/28/98 11:54a Samir
 * turn on monitors only if the cockpit is on.
 *
 * 32    8/19/98 5:47p Samir
 * increased cockpit spee.d
 *
 * 31    8/18/98 1:11a Samir
 * fixed some stuff for multi cockpit configs.
 *
 * 30    7/28/98 5:39p Samir
 * basic cockpit sounds.  need work on sync.
 *
 * 29    6/24/98 1:11p Samir
 * dont draw monitors
 *
 * 28    6/19/98 4:14p Jason
 * made multicolor lighting work with volume lights
 *
 * 27    6/15/98 4:00p Jason
 * replaced monochromatic polymodel lighting with rgb lighting
 *
 * 26    6/12/98 5:13p Jason
 * made cockpit work with dynamic lighting system
 *
 * 25    5/26/98 10:47p Samir
 * cockpit shake a little better.
 *
 * 24    5/26/98 5:05p Samir
 * cockpit and hud config file now 'unified'.   cockpit adds to hudconfig
 * load's funtionality, so cockpit info files can contain same syntax as
 * hud file.
 *
 * 23    5/22/98 6:26p Samir
 * cockpit lighting primatives.
 *
 * 22    5/18/98 5:29p Samir
 * don't move cockpit while moving.
 *
 * 21    5/18/98 4:52p Samir
 * refined cockpit motion.
 *
 * 20    5/15/98 12:01p Samir
 * cockpit buffet more jarring. fixed red crap bug.
 *
 * 19    5/14/98 7:40p Samir
 * shaking cockpit.
 *
 * 18    5/14/98 12:07p Sean
 * Removed an annoying mprintf
 *
 * 17    5/13/98 4:01p Samir
 * cockpit closing and opening secure.
 *
 * 16    5/05/98 6:27p Samir
 * function that tells the cockpit system that it was resized
 *
 * 15    5/01/98 4:24p Samir
 * added function to quickly open the cockpit.
 *
 * 14    4/24/98 8:02a Samir
 * simpler cockpit animation.
 *
 * 13    4/23/98 4:13a Samir
 * new hud system.
 *
 * 12    4/06/98 7:35p Samir
 * added glare over monitor effect.
 *
 * 11    4/02/98 3:54p Jason
 * first pass in getting polymodel paging to work
 *
 * 10    3/27/98 11:59a Samir
 * moved weapon hud text from hud to weapon monitor gauges.
 *
 * 9     3/25/98 11:59a Samir
 * implemented energy analog fully.
 *
 * 8     3/24/98 4:28p Samir
 * fixed bug when calling OpenCockpit immediately after calling
 * CloseCockpit.
 *
 * 7     3/23/98 11:14a Samir
 * maybe better cockpit lighting.
 *
 * 6     3/22/98 5:03p Samir
 * updated cockpit.
 *
 * 5     3/20/98 8:23p Samir
 * new hud and cockpit customization system.
 *
 * 4     3/18/98 6:24p Samir
 * Better view eye positioning for cockpit.
 *
 * 3     3/17/98 2:40p Samir
 * reorg of cockpit/gauges and hud systems.
 *
 * 2     3/16/98 3:30p Samir
 * incremental checkin.
 *
 * 1     3/13/98 11:22a Samir
 * initial revision.
 *
 * $NoKeywords: $
 */
//	D3 Cockpit system
//		needs a model with textures.
//		major functions:
//			activate cockpit
//			render cockpit
//			deactivate cockpit.
#include "cockpit.h"
#include "game.h"
#include "polymodel.h"
#include "hud.h"
#include "gauges.h"
#include "ship.h"
#include "player.h"
#include "room.h"
#include "hlsoundlib.h"
#include "soundload.h"
#include "sounds.h"

#include <string.h>

#include <algorithm>

#define COCKPIT_ANIM_TIME 2.0f
#define COCKPIT_DORMANT_FRAME 0.0
#define COCKPIT_START_FRAME 3.0
#define COCKPIT_MIDWAY_FRAME 7.0
#define COCKPIT_COMPLETE_FRAME ((float)Cockpit_info.model->frame_max)
//	cockpit FX modifiers
#define MAX_BUFFET_STRENGTH 0.75f
#define BUFFET_PERIOD 0.25f
#define COCKPIT_SHIFT_DELTA 0.02f
struct tCockpitCfgInfo {
  char modelname[PSFILENAME_LEN + 1];
  char shieldrings[NUM_SHIELD_GAUGE_FRAMES][PSFILENAME_LEN + 1];
  char shipimg[PSFILENAME_LEN + 1];
  char burnimg[PSFILENAME_LEN + 1];
  char energyimg[PSFILENAME_LEN + 1];
  char invpulseimg[PSFILENAME_LEN + 1];
};

struct tCockpitInfo {
  int state;                          // current state of cockpit on screen.
  int ship_index;                     // index into ships page.
  int model_num;                      // this should be retreived from the ship info.
  int snd_open, snd_close;            // sound open and close.
  float frame_time;                   // current time of animation.
  float next_keyframe, this_keyframe; // current animation of cockpit.
  poly_model *model;                  // model of cockpit.
  unsigned nonlayered_mask;           // non layered submodel mask
  unsigned layered_mask;              // layered submodel mask
  bool animating;                     // is cockpit moving?
  bool resized;                       // if cockpit is being resized....
  vector buffet_vec;                  // buffet direction
  float buffet_amp;                   // used to calculate real magnitude
  float buffet_wave;                  // current sin angle of buffet
  float buffet_time;                  // current position in buffet wave along time axis.

  matrix orient; // orientation of cockpit
};

static tCockpitInfo Cockpit_info;
static float KeyframeAnimateCockpit();
//	loads cockpit. model_name = NULL, then will not load in model name.
static void LoadCockpitInfo(const char *ckt_file, tCockpitCfgInfo *info);
//////////////////////////////////////////////////////////////////////////////
//	Initializes the cockpit by loading it in and initializing all it's gauges.
//	initialization of cockpit.
void InitCockpit(int ship_index) {
  tCockpitCfgInfo cfginfo;
  int i;
  mprintf(0, "Initializing cockpit.\n");
  LoadCockpitInfo(Ships[ship_index].cockpit_name, &cfginfo);
  // initialize special hud/cockpit images unique to this ship
  for (i = 0; i < NUM_SHIELD_GAUGE_FRAMES; i++) {
    if (cfginfo.shieldrings[i][0])
      HUD_resources.shield_bmp[i] = bm_AllocLoadFileBitmap(IGNORE_TABLE(cfginfo.shieldrings[i]), 0);
  }
  HUD_resources.ship_bmp = bm_AllocLoadFileBitmap(IGNORE_TABLE(cfginfo.shipimg), 0);
  HUD_resources.energy_bmp = bm_AllocLoadFileBitmap(IGNORE_TABLE(cfginfo.energyimg), 0);
  HUD_resources.afterburn_bmp = bm_AllocLoadFileBitmap(IGNORE_TABLE(cfginfo.burnimg), 0);
  HUD_resources.invpulse_bmp = bm_AllocLoadFileBitmap(IGNORE_TABLE(cfginfo.invpulseimg), 0);
  if (cfginfo.modelname[0] == 0) {
    Cockpit_info.model_num = -1;
    mprintf(0, "No cockpit found for ship.\n");
    return;
  }
  // initialize cockpit.
  Cockpit_info.ship_index = ship_index;
  Cockpit_info.model_num = LoadPolyModel(IGNORE_TABLE(cfginfo.modelname), 0);
  Cockpit_info.model = GetPolymodelPointer(Cockpit_info.model_num);
  Cockpit_info.frame_time = 0.0f;
  Cockpit_info.state = COCKPIT_STATE_DORMANT;
  Cockpit_info.this_keyframe = COCKPIT_DORMANT_FRAME;
  Cockpit_info.next_keyframe = COCKPIT_DORMANT_FRAME;
  Cockpit_info.resized = false;
  Cockpit_info.buffet_amp = 0.0f;
  Cockpit_info.snd_open = SOUND_COCKPIT;
  vm_MakeZero(&Cockpit_info.buffet_vec);
  //	find layered submodel mask
  Cockpit_info.layered_mask = 0x00000000;
  Cockpit_info.nonlayered_mask = 0x00000000;
  for (i = 0; i < Cockpit_info.model->n_models; i++) {
    if ((Cockpit_info.model->submodel[i].flags & SOF_VIEWER) ||
        (Cockpit_info.model->submodel[i].flags & SOF_MONITOR_MASK))
      continue;
    if (Cockpit_info.model->submodel[i].flags & SOF_LAYER)
      Cockpit_info.layered_mask |= (1 << i);
    else
      Cockpit_info.nonlayered_mask |= (1 << i);
  }
  InitGauges(STAT_SHIELDS | STAT_SHIP | STAT_ENERGY | STAT_PRIMARYLOAD | STAT_SECONDARYLOAD | STAT_AFTERBURN);
  //	initialize reticle
  FreeReticle();
  InitReticle(-1, -1);
}
//	Forces freeing of cockpit
void FreeCockpit() {
  int i;
  CloseGauges();

  if (Cockpit_info.model_num > -1) {
    FreePolyModel(Cockpit_info.model_num);
    Cockpit_info.model = NULL;
    Cockpit_info.model_num = -1;
  }
  Cockpit_info.ship_index = -1;
  Cockpit_info.frame_time = 0.0f;
  Cockpit_info.state = COCKPIT_STATE_DORMANT;
  //	free ship specific stuff for hud-cockpit shared.
  bm_FreeBitmap(HUD_resources.invpulse_bmp);
  bm_FreeBitmap(HUD_resources.afterburn_bmp);
  bm_FreeBitmap(HUD_resources.energy_bmp);
  for (i = 0; i < NUM_SHIELD_GAUGE_FRAMES; i++)
    bm_FreeBitmap(HUD_resources.shield_bmp[i]);
  bm_FreeBitmap(HUD_resources.ship_bmp);
}
//	check if cockpit exists
bool IsValidCockpit() { return (Cockpit_info.model_num > -1) ? true : false; }
bool CockpitFileParse(const char *command, const char *operand, void *data) {
  tCockpitCfgInfo *cfginf = (tCockpitCfgInfo *)data;
  if (!strcmp(command, "ckptmodel")) {
    // cockpit model name
    if (cfginf)
      strcpy(cfginf->modelname, operand);
  } else if (!strncmp(command, "shieldimg", strlen("shieldimg"))) {
    //	cockpit shield ring names
    char buf[16];
    int i;
    for (i = 0; i < NUM_SHIELD_GAUGE_FRAMES; i++) {
      snprintf(buf, sizeof(buf), "shieldimg%d", i);
      if (!stricmp(command, buf)) {
        if (cfginf)
          strcpy(cfginf->shieldrings[i], operand);
        break;
      }
    }
  } else if (!strcmp(command, "shipimg")) {
    // ship image name
    if (cfginf)
      strcpy(cfginf->shipimg, operand);
  } else if (!strcmp(command, "afterburnimg")) {
    // ship image name
    if (cfginf)
      strcpy(cfginf->burnimg, operand);
  } else if (!strcmp(command, "energyimg")) {
    // ship image name
    if (cfginf)
      strcpy(cfginf->energyimg, operand);
  } else if (!strcmp(command, "invpulseimg")) {
    // ship image name
    if (cfginf)
      strcpy(cfginf->invpulseimg, operand);
  } else if (!strcmp(command, "fullhudinf")) {
    if (cfginf)
      strcpy(HUD_resources.hud_inf_name, operand);
  } else {
    return false;
  }
  return true;
}
//	loads pertinent information about cockpit.
void LoadCockpitInfo(const char *ckt_file, tCockpitCfgInfo *cfginfo) {
  //	clear out return values.
  if (cfginfo) {
    memset(cfginfo, 0, sizeof(tCockpitCfgInfo));
    ASSERT(NUM_SHIELD_GAUGE_FRAMES == 5);
    strcpy(cfginfo->shieldrings[0], TBL_GAMEFILE("shieldring01.ogf"));
    strcpy(cfginfo->shieldrings[1], TBL_GAMEFILE("shieldring02.ogf"));
    strcpy(cfginfo->shieldrings[2], TBL_GAMEFILE("shieldring03.ogf"));
    strcpy(cfginfo->shieldrings[3], TBL_GAMEFILE("shieldring04.ogf"));
    strcpy(cfginfo->shieldrings[4], TBL_GAMEFILE("shieldring05.ogf"));
    strcpy(cfginfo->shipimg, TBL_GAMEFILE("hudship.ogf"));
    strcpy(cfginfo->burnimg, TBL_GAMEFILE("hudburn.ogf"));
    strcpy(cfginfo->energyimg, TBL_GAMEFILE("hudenergy.ogf"));
    strcpy(cfginfo->invpulseimg, TBL_GAMEFILE("shieldinv.ogf"));
  }
  if (ckt_file[0] == 0)
    return;

  LoadHUDConfig(ckt_file, CockpitFileParse, cfginfo);
}
//	forces opening of cockpit.
void OpenCockpit() {
  //	this should allow for opening cockpit while in closing mode.
  //	do this to insure that an immediate call to CloseCockpit will force it to close again.
  if (Cockpit_info.this_keyframe <= COCKPIT_COMPLETE_FRAME) {
    Cockpit_info.state = COCKPIT_STATE_QUASI;
    Cockpit_info.this_keyframe = COCKPIT_DORMANT_FRAME;
    Cockpit_info.next_keyframe = COCKPIT_COMPLETE_FRAME;
    if (Cockpit_info.frame_time > 0.0f)
      Cockpit_info.frame_time = COCKPIT_ANIM_TIME - Cockpit_info.frame_time;
  }
  Sound_system.Play2dSound(Cockpit_info.snd_open);
  //	load hud information for cockpit.
  LoadCockpitInfo(Ships[Cockpit_info.ship_index].cockpit_name, NULL);
}
//	forces complete closing of cockpit
void CloseCockpit() {
  //	this should allow for closing cockpit while in opening mode.
  //	do this to insure that an immediate call to OpenCockpit will force it to open again.
  if (Cockpit_info.this_keyframe >= COCKPIT_DORMANT_FRAME) {
    Cockpit_info.next_keyframe = COCKPIT_DORMANT_FRAME;
    Cockpit_info.this_keyframe = COCKPIT_COMPLETE_FRAME;
    if (Cockpit_info.frame_time > 0.0f)
      Cockpit_info.frame_time = COCKPIT_ANIM_TIME - Cockpit_info.frame_time;
  }
  FlagGaugesNonfunctional(STAT_ALL);
  Sound_system.Play2dSound(Cockpit_info.snd_open);
}
//	forces quick opening of cockpit
void QuickOpenCockpit() {
  Cockpit_info.frame_time = 0.0f;
  Cockpit_info.state = COCKPIT_STATE_FUNCTIONAL;
  Cockpit_info.this_keyframe = Cockpit_info.next_keyframe = COCKPIT_COMPLETE_FRAME;
  FlagGaugesFunctional(STAT_ALL);
  //	load hud information for cockpit.
  LoadCockpitInfo(Ships[Cockpit_info.ship_index].cockpit_name, NULL);
}
//	forces quick closing of cockpit
void QuickCloseCockpit() {
  Cockpit_info.frame_time = 0.0f;
  Cockpit_info.state = COCKPIT_STATE_DORMANT;
  Cockpit_info.this_keyframe = Cockpit_info.next_keyframe = COCKPIT_DORMANT_FRAME;
  FlagGaugesNonfunctional(STAT_ALL);
}
//	resizes cockpit.
void ResizeCockpit() { Cockpit_info.resized = true; }
//	cockpit orientation.
void StartCockpitShake(float mag, vector *vec) {
  ASSERT(vec);
  if (mag > MAX_BUFFET_STRENGTH)
    mag = MAX_BUFFET_STRENGTH;
  Cockpit_info.buffet_amp = mag;
  Cockpit_info.buffet_vec = (*vec);
  Cockpit_info.buffet_wave = FixSin(0);
  Cockpit_info.buffet_time = 0.0f;
}
//////////////////////////////////////////////////////////////////////////////
//	renders the cockpit.
extern float GetTerrainDynamicScalar(vector *pos, int seg);
extern void GetRoomDynamicScalar(vector *pos, room *rp, float *r, float *g, float *b);
void RenderCockpit() {
  object *player_obj = &Objects[Players[Player_num].objnum];
  physics_info *player_phys = &player_obj->mtype.phys_info;
  vector view_pos, light_vec;
  matrix view_tmat;
  float view_z, view_y, view_x, keyframe;
  float light_scalar_r, light_scalar_g, light_scalar_b;
  float normalized_time[MAX_SUBOBJECTS];
  bool gauge_reset = false;
  //	draw cockpit depending on current state
  if (Cockpit_info.state == COCKPIT_STATE_DORMANT || Cockpit_info.model_num == -1)
    return;
  //	position cockpit correctly.
  bsp_info *viewer_subobj = CockpitGetMonitorSubmodel(SOF_VIEWER);
  if (!viewer_subobj) {
    mprintf(0, "Cockpit missing viewer!\n");
    return;
  }
  view_z =
      viewer_subobj->offset.z - Cockpit_info.buffet_vec.z * Cockpit_info.buffet_amp * Cockpit_info.buffet_wave * 1.1f;
  view_y = viewer_subobj->offset.y + Cockpit_info.buffet_vec.y * Cockpit_info.buffet_amp * Cockpit_info.buffet_wave;
  view_x = viewer_subobj->offset.x + Cockpit_info.buffet_vec.x * Cockpit_info.buffet_amp * Cockpit_info.buffet_wave;
  //@@	if (player_phys->rotthrust.x !=0.0f || player_phys->rotthrust.y != 0.0f) {
  //@@		gauge_reset = true;
  //@@		view_x += (player_phys->rotthrust.y*COCKPIT_SHIFT_DELTA/player_phys->full_rotthrust);
  //@@		view_y -= (player_phys->rotthrust.x*COCKPIT_SHIFT_DELTA/player_phys->full_rotthrust);
  //@@	}
  //	create new orientation, so the cockpit should be facing the viewer
  view_tmat = Viewer_object->orient;
  view_pos = (view_tmat.fvec * view_z) + (view_tmat.uvec * view_y) + (view_tmat.rvec * view_x) + Viewer_object->pos;
  view_tmat.fvec = -view_tmat.fvec;
  view_tmat.rvec = -view_tmat.rvec;
  //	lighting.
  light_vec = -Viewer_object->orient.uvec;
  if (OBJECT_OUTSIDE(player_obj)) {
    float light_scalar = GetTerrainDynamicScalar(&player_obj->pos, CELLNUM(player_obj->roomnum));
    light_scalar_r = light_scalar;
    light_scalar_g = light_scalar;
    light_scalar_b = light_scalar;
  } else {
    GetRoomDynamicScalar(&player_obj->pos, &Rooms[player_obj->roomnum], &light_scalar_r, &light_scalar_g,
                         &light_scalar_b);
  }
  if (light_scalar_r < 0.1f)
    light_scalar_r = 0.1f;
  if (light_scalar_g < 0.1f)
    light_scalar_g = 0.1f;
  if (light_scalar_b < 0.1f)
    light_scalar_b = 0.1f;
  // Samir, I moved this from the DrawPolygonModel call below -JL
  light_scalar_r *= .8f;
  light_scalar_g *= .8f;
  light_scalar_b *= .8f;

  if (player_obj->effect_info) {
    light_scalar_r = std::min(1.0f, light_scalar_r + (player_obj->effect_info->dynamic_red));
    light_scalar_g = std::min(1.0f, light_scalar_g + (player_obj->effect_info->dynamic_green));
    light_scalar_b = std::min(1.0f, light_scalar_b + (player_obj->effect_info->dynamic_blue));
  }
  if (Players[player_obj->id].flags & PLAYER_FLAGS_HEADLIGHT) {
    light_scalar_r = 1.0;
    light_scalar_g = 1.0;
    light_scalar_b = 1.0;
  }

  // animate and draw
  keyframe = KeyframeAnimateCockpit();
  SetNormalizedTimeAnim(keyframe, normalized_time, Cockpit_info.model);
  //	must put after animation.
  if (Cockpit_info.buffet_amp > 0.04f) {
    angle buffet_angle;
    Cockpit_info.buffet_time += Frametime;
    if (Cockpit_info.buffet_time > BUFFET_PERIOD) {
      Cockpit_info.buffet_time = 0.0f;
      Cockpit_info.buffet_amp *= 0.5f;
    }
    buffet_angle = (angle)(65536.0 * Cockpit_info.buffet_time /
                           (BUFFET_PERIOD - ((BUFFET_PERIOD - Cockpit_info.buffet_time) * 0.5f)));
    Cockpit_info.buffet_wave = FixSin(buffet_angle);
    if (Cockpit_info.buffet_wave > 0.5f)
      Cockpit_info.buffet_wave = 1.0f;
    else if (Cockpit_info.buffet_wave < -0.5f)
      Cockpit_info.buffet_wave = -1.0f;
    else
      Cockpit_info.buffet_wave = 0.0f;
    Cockpit_info.animating = true;
  } else if (Cockpit_info.buffet_amp > 0.0f) {
    Cockpit_info.animating = true;
    Cockpit_info.buffet_amp = 0.0f;
  }
  //	draws lower z cockpit, and monitor glares after gauge renderering
  rend_SetZBufferState(0);
  DrawPolygonModel(&view_pos, &view_tmat, Cockpit_info.model_num, normalized_time, 0, &light_vec, light_scalar_r,
                   light_scalar_g, light_scalar_b, Cockpit_info.nonlayered_mask, 0, 1);
  RenderGauges(&view_pos, &view_tmat, normalized_time, (Cockpit_info.animating || Cockpit_info.resized), gauge_reset);
  rend_SetZBufferState(0);
  DrawPolygonModel(&view_pos, &view_tmat, Cockpit_info.model_num, normalized_time, 0, &light_vec, light_scalar_r,
                   light_scalar_g, light_scalar_b, Cockpit_info.layered_mask, 0, 1);

  Cockpit_info.resized = false;
}
//////////////////////////////////////////////////////////////////////////////
//
//	adjusts current keyframe and dormancy
float KeyframeAnimateCockpit() {
  float newkeyframe;

  newkeyframe = Cockpit_info.this_keyframe + (Cockpit_info.next_keyframe - Cockpit_info.this_keyframe) *
                                                 (Cockpit_info.frame_time / COCKPIT_ANIM_TIME);
  // mprintf(0, "this=%.1f next=%.1f ft=%.1f\n", Cockpit_info.this_keyframe, Cockpit_info.next_keyframe, Cockpit_info.frame_time)); // going up in keyframes
  if (Cockpit_info.this_keyframe < Cockpit_info.next_keyframe) {
    if (newkeyframe >= Cockpit_info.next_keyframe) {
      Cockpit_info.frame_time = 0.0f;
      Cockpit_info.this_keyframe = Cockpit_info.next_keyframe;
    }
  }
  //	going down in keyframes
  else if (Cockpit_info.this_keyframe > Cockpit_info.next_keyframe) {
    if (newkeyframe <= Cockpit_info.next_keyframe) {
      Cockpit_info.frame_time = 0.0f;
      Cockpit_info.this_keyframe = Cockpit_info.next_keyframe;
    }
  } else {
    Cockpit_info.animating = false;
    return newkeyframe;
  }

  //	decide if we can process cockpit
  Cockpit_info.animating = true;
  if (Cockpit_info.this_keyframe != Cockpit_info.next_keyframe) {
    Cockpit_info.frame_time += Frametime;
  }
  if (Cockpit_info.this_keyframe == COCKPIT_COMPLETE_FRAME &&
      Cockpit_info.next_keyframe == Cockpit_info.this_keyframe) {
    FlagGaugesFunctional(STAT_ALL);
    Cockpit_info.state = COCKPIT_STATE_FUNCTIONAL;
  } else if (Cockpit_info.this_keyframe == COCKPIT_DORMANT_FRAME &&
             Cockpit_info.next_keyframe == COCKPIT_DORMANT_FRAME) {
    Cockpit_info.state = COCKPIT_STATE_DORMANT;
  } else {
    Cockpit_info.state = COCKPIT_STATE_QUASI;
  }
  return newkeyframe;
}
//////////////////////////////////////////////////////////////////////////////
//
//	returns the submodel number of the monitor requested
bsp_info *CockpitGetMonitorSubmodel(uint16_t monitor_flag) {
  int i;
  ASSERT(Cockpit_info.model_num > -1);
  for (i = 0; i < Poly_models[Cockpit_info.model_num].n_models; i++) {
    if (Poly_models[Cockpit_info.model_num].submodel[i].flags & monitor_flag)
      return &Poly_models[Cockpit_info.model_num].submodel[i];
  }
  return NULL;
}
//	returns the polymodel for the hud
poly_model *CockpitGetPolyModel() {
  ASSERT(Cockpit_info.model_num > -1);

  return &Poly_models[Cockpit_info.model_num];
}
// Tell whether the cockpit is animating
int CockpitState() { return Cockpit_info.state; }
