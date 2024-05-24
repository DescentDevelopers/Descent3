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

 * $Logfile: /DescentIII/Main/GameCheat.cpp $
 * $Revision: 32 $
 * $Date: 10/20/99 5:40p $
 * $Author: Chris $
 *
 * Code for in-game cheats
 *
 * $Log: /DescentIII/Main/GameCheat.cpp $
 *
 * 32    10/20/99 5:40p Chris
 * Added the Red Guidebot
 *
 * 31    7/15/99 6:38p Jeff
 * added outlinemode cheat code and rendering stats cheat
 *
 * 30    5/21/99 10:43p Jeff
 * don't delete guidebot on kill robots cheat
 *
 * 29    5/10/99 1:00a Jeff
 * release cheat codes
 *
 * 28    5/09/99 10:48p Jason
 * change game cheats slightly
 *
 * 27    5/04/99 9:27p Jeff
 * quad super lasers on cheat
 *
 * 26    5/04/99 8:45p Jeff
 * quad super lasers on cheat
 *
 * 25    5/01/99 12:53a Jeff
 * made fullmap cheat a 'cheater' cheat
 *
 * 24    4/30/99 8:31p Jeff
 * added fullmap cheat
 *
 * 23    4/19/99 12:09a Matt
 * Added Teletubbies cheat.
 *
 * 22    3/05/99 5:36p Kevin
 * Changed framrate cheat back to 'frametime'
 *
 * 21    3/04/99 7:08p Jeff
 * only give max ammo on weapon cheat
 *
 * 20    3/04/99 3:20p Jeff
 * include only whats allowed for oem
 *
 * 19    3/03/99 4:07p Jeff
 * new oem cheat codes
 *
 * 18    2/28/99 9:28p Jeff
 *
 * 17    2/24/99 11:21a Jason
 * Fixed bugs for OEM build
 *
 * 16    2/10/99 6:55p Jeff
 * added proxmines to the game
 *
 * 15    2/02/99 8:43a Chris
 * I made buildings with AI work correctly (ie really big robots should be
 * buildings)
 * anim to and from states are now shorts instead of bytes
 *
 * 14    1/31/99 7:25p Matt
 * Renamed a bunch of functions to have HUD capitalized
 *
 * 13    1/29/99 2:08p Jeff
 * localization
 *
 * 12    1/21/99 11:15p Jeff
 * pulled out some structs and defines from header files and moved them
 * into seperate header files so that multiplayer dlls don't require major
 * game headers, just those new headers.  Side effect is a shorter build
 * time.  Also cleaned up some header file #includes that weren't needed.
 * This affected polymodel.h, object.h, player.h, vecmat.h, room.h,
 * manage.h and multi.h
 *
 * 11    12/15/98 4:28p Jeff
 * added mission data information to the pilot files to save what the
 * highest level they achieved on a mission is.  Added level select dialog
 * (not hooked up) and level warp cheat.
 *
 * 10    12/11/98 10:36a Jason
 * fixed external
 *
 * 9     10/20/98 6:33p Jeff
 * added cool texture cheat (badtexture)
 *
 * 8     10/19/98 10:23p Jeff
 * added cheats to start/stop rt log profiling
 *
 * 7     10/19/98 7:18p Matt
 * Added system to support different types of damage to the player and
 * have these different types make different sounds.
 *
 * 6     10/18/98 9:24p Jeff
 * fixed frametime cheat
 *
 * 5     10/17/98 6:08p Jeff
 * weapon cheat gives quad
 *
 * 4     10/15/98 1:32p Jeff
 * added suicide cheat
 *
 * 3     10/14/98 1:02p Jason
 * fixed FindSoundName issues
 *
 * 2     10/11/98 3:01a Jeff
 * moved from TelCom into here
 *
 * $NoKeywords: $
 */

#include "player.h"
#include "object.h"
#include "hud.h"
#include "weapon.h"
#include "hlsoundlib.h"
#include "game.h"
#include "ddio.h"
#include "soundload.h"
#include "sounds.h"
#include "damage.h"
#include "rtperformance.h"
#include "object_lighting.h"
#include "gamesequence.h"
#include "multi.h"
#include "stringtable.h"
#include "ship.h"
#include "render.h"
#include "renderer.h"

#define CHEATSPOT 14
bool IsCheater = false;
bool Display_renderer_stats = false;
uint8_t Outline_release_mode = 0;

char CheatBuffer[] = "AAAAAAAAAAAAAAA";
char OldCheatBuffer[] = "AAAAAAAAAAAAAAA";
extern bool Force_one_texture;
extern uint8_t AutomapVisMap[MAX_ROOMS];

const char *LamerCheats[] = {
    "?E9FI=()",  // gabbagabbahey
    "=-OQESN1",  // motherlode
    "C<G2DAIV",  // zingermans
    "%>OQB9T@",  // eatangelos
    ",/,JCQ/7",  // alifalafel
    "-4ACVPFF",  // delshiftb
    "-JQ\"B(F$", // farmerjoe
    "TIHB;B&H",  // freespace
    "=!9:S>!?",  // gowingnut
    ":3FGG1MP",  // honestbob
    "$'R&?FTU",  // rockrgrl
    "EP=$>I<%",  // blimpiebest
    "#+'T50IS",  // mightyaphrodite
    "J4D!JG56",  // freeitup
    "-KAM'<VQ",  // yummyfunyon
    "=8AB4C8=",  // longchimp
    "AVAC>2:)",  // zodisgod
    "U1/C;>AH",  // chowyunfat
    "JRF0H(!P",  // phantasm
    "!@3/L5KE",  // blackdove
    "F1*GOG2(",  // mortality
    "6\"BI<-BE", // monkeydance
    "#/;=3J8F",  // nosferatu
    "J#.DEJCT",  // pandorasbox
};
#define N_LAMER_CHEATS (sizeof(LamerCheats) / sizeof(*LamerCheats))

/*
Demo Cheat Codes
================
BLIMPIEBEST = all weapons
MIGHTYAPHRODITE = cloak on/off
FREEITUP = all robots dead
YUMMYFUNYON = invulnerable on/off
FRAMETIME = framerate
LONGCHIMP = 3rd person view
BADTEXTURE = destroy all textures, use bad bitmap
ZODISGOD = suicide
*/

/*
OEM Cheat Codes
===============
CHOWYUNFAT = all weapons
PHANTASM = cloak on/off
BLACKDOVE = all robots dead
MORTALITY = invulnerable on/off
MRSHOWBIZ = framerate
MONKEYDANCE = 3rd person view
GARBARGE = destroy all textures, use bad bitmap
NOSFERATU = suicide
PANDORASBOX = level warp
*/

/*
Release Cheat Codes
===================
IVEGOTIT = all weapons
TESTICUS = cloak on/off
DEADOFNIGHT = all robots dead
BURGERGOD = invulnerable on/off
FRAMELENGTH = framerate
BYEBYEMONKEY = 3rd person view
SHENANIGANS = destroy all textures, use bad bitmap
TUBERACER = suicide
MORECLANG = level warp
TELETUBBIES = TeletubbiesCheat
TREESQUID = Fullmap
RENDERSTAT = Rendering stats
OUTLINEM = Cycle outline modes
*/

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// NOTE TO ADD A CHEATCODE
// GRAB THE LATEST VERSION OF WINCRYPT FROM JEFF
// BELOW, ONLY THE FIRST QUOTED STRING PER CHEAT MATTERS
// THE OTHER QUOTED STRINGS ARE TO TOSS THE HACKERS
// SOME FUN.
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
const char *WeaponsCheat = "BH;URJH,"
                     "8DS#";
const char *CloakCheat = "L_QM[=^)"
                   "@-:!(";
const char *KillRobotsCheat = "KMGKKG4D"
                        "~";
const char *InvulnCheat = "BBG\\Q90L"
                    "}FQC.>$";
const char *FrametimeCheat = ";;24E1)]"
                       "==";
const char *CameraCheat = "X2OHN*2("
                    "K<?@!";
const char *CoolTextures = "/Q/NGQ\\,"
                     "%*HD@";
const char *SuicideCheat = "\\S3YGK=N"
                     "&D+#";
const char *LevelWarpCheat = "*JYO]9<G"
                       "()()";
const char *TeletubbiesCheat = "I@\\LQOKM"
                         ":):/;)>:(";
const char *FullmapCheat = "4V\\9EI:'"
                     "\"#$NNW@";
const char *OutlineModeCheat = "*HAFW1ZI"
                         " (-'\"#";
const char *PolygonCountCheat = "MMQ6TXHU"
                          "hj@(x`";

#ifdef USE_RTP
const char *StartLogCheat = ";IT.*+E3"
                      "%^VXKS;JKS"; // startrtlog
const char *StopLogCheat = "GE8FHH6*"
                     "_=JJDK"; // stoprtlog
#endif

// these are cheat codes from the demo/oem carried over as they aren't cheats
const char *OLDDEMO_FrametimeCheat = "B'&>;6V4"; // frametime
const char *OLDOEM_FrametimeCheat = "F+NDOJ'<";  // mrshowbiz
const char *OLDDEMO_CoolTextures = "QG1EG+H+";   // badtexture
const char *OLDOEM_CoolTextures = "5UU(I'0=";    // garbarge

const char *jcrypt(const char *plainstring) {
  int i, t, len;
  static char cryptstring[20];

  len = strlen(plainstring);
  if (len > 8)
    len = 8;

  for (i = 0; i < len; i++) {
    cryptstring[i] = 0;

    for (t = 0; t < 8; t++) {
      cryptstring[i] ^= (plainstring[t] ^ plainstring[i % (t + 2)]);
      cryptstring[i] %= 57;
      cryptstring[i] += 39;
    }
  }
  cryptstring[i] = 0;
  return cryptstring;
}

const char *oldjcrypt(const char *plainstring) {
  int i, t, len;
  static char cryptstring[20];

  len = strlen(plainstring);
  if (len > 8)
    len = 8;

  for (i = 0; i < len; i++) {
    cryptstring[i] = 0;

    for (t = 0; t < 8; t++) {
      cryptstring[i] ^= (plainstring[t] ^ plainstring[i % (t + 1)]);
      cryptstring[i] %= 54;
      cryptstring[i] += 33;
    }
  }
  cryptstring[i] = 0;
  return cryptstring;
}

// Tells others that we are cheating
void SendCheaterAttemptText() {
  char str[255];
  snprintf(str, sizeof(str), TXT_CHEATATTEMPT, Players[Player_num].callsign);

  if (Netgame.local_role == LR_SERVER)
    MultiSendMessageFromServer(GR_RGB(255, 0, 0), str);
  else
    MultiSendMessageToServer(0, str);
}

void DemoCheats(int key) {
  static int snd_cheat = -1;
  int i;
  const char *cryptstring, *oldcryptstring;

  if (snd_cheat == -1) {
    snd_cheat = SOUND_CHEATER;
  }

  key = ddio_KeyToAscii(key);

  for (i = 0; i < 15; i++) {
    CheatBuffer[i] = CheatBuffer[i + 1];
    OldCheatBuffer[i] = OldCheatBuffer[i + 1];
  }

  CheatBuffer[CHEATSPOT] = key;
  OldCheatBuffer[CHEATSPOT] = key;

  cryptstring = jcrypt(&CheatBuffer[7]);
  oldcryptstring = oldjcrypt(&OldCheatBuffer[7]);

  for (i = 0; i < N_LAMER_CHEATS; i++) {
    if (!(strcmp(oldcryptstring, LamerCheats[i]))) {
      if (snd_cheat != -1)
        Sound_system.Play2dSound(snd_cheat);

      if (Game_mode & GM_MULTI) {
        SendCheaterAttemptText();
      } else {
        AddHUDMessage(TXT_LAMER);
        Objects[Players[Player_num].objnum].shields = 1;
        Players[Player_num].energy = 1;
      }
    }
  }

  if (!(memcmp(cryptstring, FrametimeCheat, 8)) || !(memcmp(oldcryptstring, OLDDEMO_FrametimeCheat, 8)) ||
      !(memcmp(oldcryptstring, OLDOEM_FrametimeCheat, 8))) {
    Hud_stat_mask ^= STAT_FPS;
    bool enabled = (bool)((Hud_stat_mask & STAT_FPS) != 0);
    AddHUDMessage(TXT_FRAMETIMEMSG, (enabled) ? TXT_ENABLED : TXT_DISABLED);
  }

  if (!(memcmp(cryptstring, CoolTextures, 8)) || !(memcmp(oldcryptstring, OLDDEMO_CoolTextures, 8)) ||
      !(memcmp(oldcryptstring, OLDOEM_CoolTextures, 8))) {
    Force_one_texture = !Force_one_texture;
    AddHUDMessage("%s", (Force_one_texture) ? TXT_COOLTEXTURES : TXT_NORMALTEXTURES);
  }

  if (!(memcmp(cryptstring, CameraCheat, 8))) {
    Player_has_camera = !Player_has_camera;
  }

#ifdef USE_RTP
  if (!(memcmp(cryptstring, StartLogCheat, 8))) {
    rtp_StartLog();
  }

  if (!(memcmp(cryptstring, StopLogCheat, 8))) {
    rtp_StopLog();
  }
#endif

  if (!(memcmp(cryptstring, TeletubbiesCheat, 8))) {
    int merc_sun = FindTextureName("MercSun11");
    int camile_sun = FindTextureName("CamileSun");
    if ((merc_sun != -1) && (camile_sun != -1))
      GameTextures[merc_sun].bm_handle = GameTextures[camile_sun].bm_handle;
  }

  static int state = 0;
  if (!(memcmp(cryptstring, OutlineModeCheat, 8))) {
    uint8_t new_mode;
    char buffer[128];
    switch (state) {
    case 0:
      new_mode = 1;
      // state = 1;
      state = 3;
      strcpy(buffer, "Mine Outline Enabled");
      break;
    case 1:
      new_mode = 2;
      strcpy(buffer, "Object Outline Enabled");
      state = 2;
      break;
    case 2:
      new_mode = 3;
      strcpy(buffer, "Mine and Object Outline Enabled");
      state = 3;
      break;
    case 3:
      new_mode = 0;
      strcpy(buffer, "Disabled");
      state = 0;
      break;
    default:
      new_mode = 1;
      strcpy(buffer, "Mine Outline Enabled");
      state = 1;
      break;
    }

    Outline_release_mode = new_mode;

    AddHUDMessage("Outline mode: %s", buffer);
  }

  if (!(memcmp(cryptstring, PolygonCountCheat, 8))) {
    Display_renderer_stats = !Display_renderer_stats;
    AddHUDMessage("Renderer Stats: %s", (Display_renderer_stats) ? "On" : "Off");
  }

  if (Game_mode & GM_MULTI)
    return;

  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  //                 Anything below this line won't be called
  //					in a multiplayer game
  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

  if (!(memcmp(cryptstring, FullmapCheat, 8))) {
    AddHUDMessage(TXT_FULLMAP);

    for (int i = 0; i < MAX_ROOMS; i++) {
      if (!(Rooms[i].flags & RF_SECRET))
        AutomapVisMap[i] = 1;
    }

    if (snd_cheat != -1)
      Sound_system.Play2dSound(snd_cheat);
    IsCheater = true;
    Players[Player_num].score = 0;
  }

  if (!(memcmp(cryptstring, LevelWarpCheat, 8))) {
    // gets highest level flown for mission
    Game_interface_mode = GAME_LEVEL_WARP;
    IsCheater = true;
    Players[Player_num].score = 0;
  }

  if (!(memcmp(cryptstring, InvulnCheat, 8))) {
    if (Players[Player_num].flags & PLAYER_FLAGS_INVULNERABLE) {
      MakePlayerVulnerable(Player_num);
      AddHUDMessage(TXT_INVULNCHEATON);
    } else {
      MakePlayerInvulnerable(Player_num, 10000);
      AddHUDMessage(TXT_INVULNCHEAT);
      if (snd_cheat != -1)
        Sound_system.Play2dSound(snd_cheat);
    }
    IsCheater = true;
    Players[Player_num].score = 0;
  }

  if (!(memcmp(cryptstring, CloakCheat, 8))) {
    if (Player_object->effect_info->type_flags & EF_CLOAKED) {
      MakeObjectVisible(Player_object);
      AddHUDMessage(TXT_CLOAKCHEATOFF);
    } else {
      MakeObjectInvisible(Player_object, 10000);
      AddHUDMessage(TXT_CLOAKCHEATON);
      if (snd_cheat != -1)
        Sound_system.Play2dSound(snd_cheat);
    }
    IsCheater = true;
    Players[Player_num].score = 0;
  }

  if (!(memcmp(cryptstring, WeaponsCheat, 8))) {

#ifdef DEMO
    Players[Player_num].weapon_flags = HAS_FLAG(LASER_INDEX) | HAS_FLAG(VAUSS_INDEX) | HAS_FLAG(SUPER_LASER_INDEX) |
                                       HAS_FLAG(NAPALM_INDEX) | HAS_FLAG(CONCUSSION_INDEX) | HAS_FLAG(HOMING_INDEX) |
                                       HAS_FLAG(FRAG_INDEX) | HAS_FLAG(FLARE_INDEX);
#elif OEM
    Players[Player_num].weapon_flags =
        HAS_FLAG(LASER_INDEX) | HAS_FLAG(VAUSS_INDEX) | HAS_FLAG(MICROWAVE_INDEX) | HAS_FLAG(PLASMA_INDEX) |
        HAS_FLAG(SUPER_LASER_INDEX) | HAS_FLAG(MASSDRIVER_INDEX) | HAS_FLAG(NAPALM_INDEX) | HAS_FLAG(CONCUSSION_INDEX) |
        HAS_FLAG(HOMING_INDEX) | HAS_FLAG(IMPACTMORTAR_INDEX) | HAS_FLAG(FRAG_INDEX) | HAS_FLAG(GUIDED_INDEX) |
        HAS_FLAG(NAPALMROCKET_INDEX) | HAS_FLAG(CYCLONE_INDEX);

#else
    Players[Player_num].weapon_flags = 0xFFFFFFFF;
#endif

    int i;
    for (i = 0; i < MAX_PLAYER_WEAPONS; i++) {
      if (HAS_FLAG(i)) {
        Players[Player_num].weapon_ammo[i] = Ships[Players[Player_num].ship_index].max_ammo[i];
      }
    }

    Players[Player_num].energy = 200.0f;
    Objects[Players[Player_num].objnum].shields = 200.0f;
    Players[Player_num].afterburn_time_left = AFTERBURN_TIME;
    Objects[Players[Player_num].objnum].dynamic_wb[LASER_INDEX].flags |= DWBF_QUAD;
    Objects[Players[Player_num].objnum].dynamic_wb[SUPER_LASER_INDEX].flags |= DWBF_QUAD;

    int quad_id = FindObjectIDName("QuadLaser");
    if (quad_id > -1) {
      Players[Player_num].inventory.Add(OBJ_POWERUP, quad_id, NULL, -1, -1, INVF_MISSIONITEM | INVF_TIMEOUTONSPEW);
    }

    int c_id, b_id, s_id, g_id, p_id;
    int c_pid, b_pid, s_pid, g_pid, p_pid;
    int amount;
#ifdef DEMO
    c_id = -1;
    b_id = -1;
    s_id = -1;
    g_id = FindWeaponName("Gunboy");
    p_id = -1;

    c_pid = -1;
    b_pid = -1;
    s_pid = -1;
    g_pid = FindObjectIDName("gunboypowerup");
    p_pid = -1;

    amount = 12;
#elif OEM
    c_id = -1;
    b_id = FindWeaponName("Betty");
    s_id = FindWeaponName("SeekerMine");
    g_id = FindWeaponName("Gunboy");
    p_id = -1;

    c_pid = -1;
    b_pid = FindObjectIDName("Betty4pack");
    s_pid = FindObjectIDName("Seeker3pack");
    g_pid = FindObjectIDName("gunboypowerup");
    p_pid = -1;

    amount = 12;
#else
    c_id = FindWeaponName("Chaff");
    b_id = FindWeaponName("Betty");
    s_id = FindWeaponName("SeekerMine");
    g_id = FindWeaponName("Gunboy");
    p_id = FindWeaponName("ProxMine");

    c_pid = FindObjectIDName("chaff");
    b_pid = FindObjectIDName("Betty4pack");
    s_pid = FindObjectIDName("Seeker3pack");
    g_pid = FindObjectIDName("gunboypowerup");
    p_pid = FindObjectIDName("ProxMinepowerup");

    amount = 30;

#endif

    if (c_pid == -1)
      c_pid = FindObjectIDName("Shield");
    if (b_pid == -1)
      b_pid = FindObjectIDName("Energy");
    if (s_pid == -1)
      s_pid = FindObjectIDName("Shield");
    if (g_pid == -1)
      g_pid = FindObjectIDName("Energy");
    if (p_pid == -1)
      p_pid = FindObjectIDName("Shield");

    object *objp = &Objects[Players[Player_num].objnum];
    for (i = 0; i < amount; i++) {
      if (c_id != -1)
        Players[Player_num].counter_measures.Add(OBJ_WEAPON, c_id, objp, OBJ_POWERUP, c_pid);
      if (b_id != -1)
        Players[Player_num].counter_measures.Add(OBJ_WEAPON, b_id, objp, OBJ_POWERUP, b_pid);
      if (s_id != -1)
        Players[Player_num].counter_measures.Add(OBJ_WEAPON, s_id, objp, OBJ_POWERUP, s_pid);
      if (g_id != -1)
        Players[Player_num].counter_measures.Add(OBJ_WEAPON, g_id, objp, OBJ_POWERUP, g_pid);
      if (p_id != -1)
        Players[Player_num].counter_measures.Add(OBJ_WEAPON, p_id, objp, OBJ_POWERUP, p_pid);
    }

    AddHUDMessage(TXT_ALLWEAPONS);
    if (snd_cheat != -1)
      Sound_system.Play2dSound(snd_cheat);
    IsCheater = true;
    Players[Player_num].score = 0;
  }

  if (!(memcmp(cryptstring, KillRobotsCheat, 8))) {
    AddHUDMessage(TXT_ALLROBOTSDEAD);
    if (snd_cheat != -1)
      Sound_system.Play2dSound(snd_cheat);

    for (int i = 0; i < MAX_OBJECTS; i++) {
      if (Objects[i].type == OBJ_ROBOT || (Objects[i].type == OBJ_BUILDING && Objects[i].ai_info)) {
        if (Objects[i].id != ROBOT_GUIDEBOT && Objects[i].id != ROBOT_GUIDEBOTRED) {
          // spare the guidebots
          SetObjectDeadFlag(&Objects[i]);
        }
      }
    }
    IsCheater = true;
    Players[Player_num].score = 0;
  }

  if (!(memcmp(cryptstring, SuicideCheat, 8))) {
    //"ZOD Strikes Revenge"
    static uint8_t buffer[20] = {0x80, 0x95, 0x9E, 0xFA, 0x89, 0xAE, 0xA8, 0xB3, 0xB1, 0xBF,
                               0xA9, 0xFA, 0x88, 0xBF, 0xAC, 0xBF, 0xB4, 0xBD, 0xBF, 0xDA};
    char tb[20];

    for (int i = 0; i < 20; i++) {
      tb[i] = buffer[i] ^ 0xDA;
    }

    AddHUDMessage(tb);
    ApplyDamageToPlayer(&Objects[Players[Player_num].objnum], NULL, PD_ENERGY_WEAPON,
                        Objects[Players[Player_num].objnum].shields + 10);
    memset(tb, 0, 20);
  }
}
