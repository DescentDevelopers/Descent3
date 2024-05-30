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

 * $Logfile: /DescentIII/Main/hud.h $
 * $Revision: 67 $
 * $Date: 7/13/99 12:22a $
 * $Author: Jeff $
 *
 * <hud.h>
 *
 * $Log: /DescentIII/Main/hud.h $
 *
 * 67    7/13/99 12:22a Jeff
 * correctly externed HudInputMessage, increased size for new tokenized
 * text macros
 *
 * 66    5/21/99 11:15p Samir
 * savegames save hud timer and special text states. restore too.
 *
 * 65    5/21/99 4:06p Matt
 * For customtext2 HUD items, store the buffer length in the HUD item.
 *
 * 64    5/21/99 2:58p Matt
 * Fixed mis-defined flag that caused the multiplayer menus to not render.
 *
 * 63    5/20/99 5:48p Matt
 * Added a HUD item flag, for use by Dallas-created items, that makes a
 * HUD item persisitent for the duration of one level, but get cleared
 * between levels.
 *
 * 62    5/19/99 11:25a Matt
 * Added multisafe functions & Dallas actions for showing a timer on the
 * screen and adding custom HUD messages.
 *
 * 61    5/17/99 6:22p Jeff
 * added filtered HUD messages
 *
 * 60    5/08/99 1:04a Samir
 * added timers for key presses in message list boxes.
 *
 * 59    5/07/99 10:48p Matt
 * Made add-HUD functions return true if the message was added, and false
 * if it wasn't (because the previous message was the same).
 *
 * 58    4/28/99 2:14a Samir
 * save and load Game messages in savegame.
 *
 * 57    4/24/99 8:43p Samir
 * when shrinking screen hud messages get rendered in black region.
 *
 * 56    4/06/99 11:39a Samir
 * added more formatting options for hud items (added two other full
 * screen hud infs for different ships)
 *
 * 55    3/30/99 2:39p Samir
 * added custom config file for fullscreen hud and hacked mass driver
 * reticle in special reticle code.
 *
 * 54    3/26/99 12:46p Samir
 * configurable reticle.
 *
 * 53    3/04/99 7:39p Matt
 * Added sound effects to FreeSpace-style persistent HUD messages.
 *
 * 52    3/03/99 5:34p Matt
 * Added fade-out for goal complete messages
 *
 * 51    2/19/99 2:18p Samir
 * AddGameMessage implicitly figures out time and level of message.
 *
 * 50    2/18/99 5:27p Matt
 * Added color parm to AddPersistentHUDMessage() and fixed the timeout.
 *
 * 49    2/16/99 6:15p Jeff
 * pause hud messages when in cinematics
 *
 * 48    2/06/99 6:59p Jeff
 * created RenderHUDGetTextLineWidth and RenderHUDGetTextHeight
 *
 * 47    1/31/99 7:26p Matt
 * Renamed a bunch of functions to have HUD capitalized
 *
 * 46    1/29/99 6:29p Samir
 * implemented hud scrollback for hud messages.
 *
 * 45    1/27/99 6:05p Samir
 * added scrollback for game messages on HUD.
 *
 * 44    1/23/99 2:33p Kevin
 * Increased hud message length, because we clip to the real width of the
 * hud. Also fixed up the multiline code a bit
 *
 * 43    1/22/99 4:06p Jeff
 * added hud messages that can be sent to just teammates or individual
 * people
 *
 * 42    1/11/99 4:08p Jason
 * added multiplayer taunt macros
 *
 * 41    10/22/98 2:40p Samir
 * redid HUD sequencing so multiplayer hud stuff works.
 *
 * 40    10/19/98 11:22p Samir
 * added hud observer mode and fixed problems with hud switching and
 * screen size.
 *
 * 39    10/14/98 4:26p Samir
 * added persistent hud messages.
 *
 * 38    10/05/98 11:08a Jason
 * implemented player message log
 *
 * 37    9/08/98 10:28a Samir
 * added function to reset hud messages.
 *
 * 36    8/15/98 10:51p Matt
 * Added function ToggleHUDMode(), and made StartHudInputMessage() public.
 *
 * 35    7/28/98 3:17p Jason
 * fixed buffer overrun problem
 *
 * 34    7/06/98 7:34p Samir
 * added countermeasures.
 *
 * 33    6/24/98 7:38p Samir
 * redid graphical/text/cockpit hud item management.
 *
 * 32    6/17/98 6:31p Samir
 * Added anti-grav warning when dying.
 *
 * 31    6/15/98 6:54p Samir
 * added invulnerability and cloak effect.
 *
 * 30    6/15/98 2:14p Samir
 * afterburner hud image scale modified.
 *
 * 29    5/26/98 5:05p Samir
 * cockpit and hud config file now 'unified'.   cockpit adds to hudconfig
 * load's funtionality, so cockpit info files can contain same syntax as
 * hud file.
 *
 * 28    5/25/98 8:30p Samir
 * guided missile reticle and added a hud timer.
 *
 * 27    5/22/98 6:25p Samir
 * implemented a lot of hud items.
 *
 * 26    5/07/98 6:01p Samir
 * allow for certain hud items to  not be resettable.
 *
 * 25    5/07/98 12:23p Samir
 * RenderHudText functions added to hud.h
 *
 * 24    5/06/98 4:33p Samir
 * added fixed screen width and height to grdefs.h (default res.)
 *
 * 23    5/05/98 6:27p Samir
 * added hud aspect ratio values and reticle is scaled correctly.
 *
 * 22    4/28/98 4:26p Samir
 * brightened hud.,
 *
 * 21    4/24/98 5:32p Samir
 * added reset reticle function.
 *
 * 20    4/24/98 8:01a Samir
 * don't pass zoom argument to Render functions.
 *
 * 19    4/23/98 4:13a Samir
 * new hud system.
 *
 * 18    4/14/98 9:19p Samir
 * made hacked good looking reticle for mag demo.
 *
 * 17    4/13/98 7:02p Samir
 * beginning reticle code.
 *
 * 16    3/25/98 11:59a Samir
 * implemented energy analog fully.
 *
 * 15    3/23/98 5:36p Jason
 * changed number of hud messages displayed at once
 *
 * 14    3/23/98 4:51p Jason
 * incremental checkin for multiplay
 *
 * 13    3/20/98 8:23p Samir
 * new hud and cockpit customization system.
 *
 * 12    3/19/98 9:19p Samir
 * created a hud item type.
 *
 * 11    3/18/98 6:25p Samir
 * Added new STAT_ constants/
 *
 * 10    3/17/98 2:37p Samir
 * reorg of hud/gauge/cockpit dependencies.
 *
 * 9     3/16/98 3:53p Jason
 * added hud input message stuff
 *
 * 8     3/16/98 3:30p Samir
 * incremental checkin.
 *
 * 7     3/13/98 12:09p Samir
 * made changes to reflect new cockpit.cpp and cockpit.h
 *
 * 6     12/29/97 5:44p Samir
 * Took out references to grViewport and old 2d library.
 *
 * 5     11/11/97 1:26p Samir
 * Added function to modify a hud gauge and initialized some gauges for
 * first cockpit.
 *
 * 4     11/04/97 6:23p Samir
 * Lighting cockpit, initializes gauges.
 *
 * 3     10/28/97 6:37p Samir
 * 3d Cockpit draws, bad lighting and no gauges.
 *
 * 5     5/14/97 11:29a Jason
 * added energy and shield gauges to hud
 *
 * 4     3/05/97 3:03p Jason
 * added blinking messages
 *
 * 3     3/04/97 12:58p Jason
 * made hud messages scroll and fade
 *
 * 2     2/13/97 4:12p Jason
 * added hud message test
 *
 * $NoKeywords: $
 */

#ifndef HUD_H
#define HUD_H

#include "pstypes.h"
#include "grdefs.h"

#define MAX_HUD_MESSAGES 3
#define HUD_MESSAGE_LENGTH 200
#define HUD_TEXT_LENGTH 32
#define DEFAULT_HUD_WIDTH (float)FIXED_SCREEN_WIDTH
#define DEFAULT_HUD_HEIGHT (float)FIXED_SCREEN_HEIGHT

struct object;
struct poly_model;
struct bsp_info;
struct g3Point;

//	type to pass to functions that draw stats
typedef uint16_t tStatMask;

struct CFILE;

//	hud (or gauge) stat flag.
#define STAT_FPS 0x0001
#define STAT_SHIELDS 0x0002
#define STAT_ENERGY 0x0004
#define STAT_PRIMARYLOAD 0x0008
#define STAT_SECONDARYLOAD 0x0010
#define STAT_MESSAGES 0x0020
#define STAT_INVENTORY 0x0040
#define STAT_SHIP 0x0080
#define STAT_AFTERBURN 0x0100
#define STAT_WARNING 0x0200
#define STAT_GOALS 0x0400
#define STAT_TIMER 0x0800
#define STAT_CUSTOM 0x1000
#define STAT_CNTRMEASURE 0x2000
#define STAT_STANDARD 0x3ffe
#define STAT_ALL 0x3fff

// is this the special version? (defined by huddisplay.cpp or user equiv.)
#define STAT_SPECIAL 0x4000
//	is this the graphical version?
#define STAT_GRAPHICAL 0x8000

//	hud modes
enum tHUDMode {

  HUD_FULLSCREEN,
  HUD_LETTERBOX,
  HUD_COCKPIT,
  HUD_OBSERVER
};

#define MAX_HUD_ITEMS 32
#define HUD_COLOR GR_RGB(0, 255, 0)
#define HUD_ALPHA 192
#define HUD_ZOOM 0.56f

#define HUD_MSG_PERSISTENT_INFINITE 10000.0f
#define HUD_MSG_PERSISTENT_CENTER -1

/////////////////////////////////////////////////////////////////////////////////

// Max characters in a hud input message
#define MAX_HUD_INPUT_LEN 80

extern char HudInputMessage[MAX_HUD_INPUT_LEN];
extern int HudInputMessageLen;

extern char Hud_messages[MAX_HUD_MESSAGES][HUD_MESSAGE_LENGTH];
extern int Num_hud_messages;

// Whether or not we're in a input message frame
extern int Doing_input_message;

//	determines what's displayable on the hud
extern tStatMask Hud_stat_mask;

//	hud aspect ratio
extern float Hud_aspect_x;
extern float Hud_aspect_y;

extern bool Hud_show_controls;

//	normalize hud coordinates
#define HUD_X(_x) ((float)(_x) * Hud_aspect_x)
#define HUD_Y(_y) ((float)(_y) * Hud_aspect_y)

//	number of shield frames in gauge
#define NUM_SHIELD_GAUGE_FRAMES 5
#define SHIELD_GAUGE_FRAME_FULL 0
#define SHIELD_GAUGE_FRAME_PARTIAL 1
#define SHIELD_GAUGE_FRAME_HALF 2
#define SHIELD_GAUGE_FRAME_SERIOUS 3
#define SHIELD_GAUGE_FRAME_CRITICAL 4

/////////////////////////////////////////////////////////////////////////////////

// Adds a message to the HUD message list.  If the list is already full, punt the
// top one and move the others up
// Returns true if message added, or false if message not (because the previous message was the same)
bool AddHUDMessage(const char *format, ...);
bool AddBlinkingHUDMessage(const char *format, ...);

// Adds a HUD message (similar to AddHUDMessage), however can be filtered out by
// a "-playermessages" command line.
bool AddFilteredHUDMessage(const char *format, ...);

//	initializes other hud stuff
void SetHUDEnergyImage(const char *energy_img);
void SetHUDAfterburnImage(const char *afterburn_img);

// Renders all the messages we have in the message list.  If HUD_MESSAGE_TIME has
// elapsed, then punt the oldest message and move the others up one
void RenderHUDMessages();

//	renders ship stats
void RenderHUDStats(tStatMask stat_mask);

// Handles all incoming keys for an inputted hud message
void DoHUDInputMessageKey(int key);

// Called when the player hit the Multiplayer message key.
// Puts the player in input mode, or if already inputting resets the hud input message.
void StartHUDInputMessage();

// Called when the player hits the Multiplayer team-message key.
// Puts the player in input mode, or if already inputting resets the hud input message.
void StartTeamHUDInputMessage();

// Adds a colored message to the hud
// Returns true if message added, or false if message not (because the previous message was the same)
bool AddColoredHUDMessage(ddgr_color color, const char *format, ...);

// Adds a HUD message (similar to AddColoredHUDMessage), however can be filtered out by
// a "-playermessages" command line.
bool AddFilteredColoredHUDMessage(ddgr_color color, const char *format, ...);

//	Initializes Reticle on Hud.  Usually called when weapon changes.
void InitReticle(int primary_slots, int secondary_slots);

//	resets reticle to current weapon.
void ResetReticle();

//  frees up reticle bitmaps
void FreeReticle();

// Flags for persistent HUD messages
#define HPF_FADEOUT 1        // message fades out when done
#define HPF_FREESPACE_DRAW 2 // message draws with the FreeSpace briefing effect

//	adds a persistent hud message that is timed, or infinite until removed
// for infinite, time = HUD_MSG_PERSISTENT_INFINITE
// for centering on an axis,set either x or y to HUD_MSG_PERSISTENT_CENTER
void AddPersistentHUDMessage(ddgr_color color, int x, int y, float time, int flags, int sound_index, const char *fmt,
                             ...);

// removes persistent hud message
void ResetPersistentHUDMessage();

//	Given a hud message, it will determine who should get this message.
//	"name: message" = the player with the callsign name should get the message
//	"team: message" = the Player_num's team should get the message
//	"0-32: message" = the player with player num of 0-32 should get the message
//	all other messages are for everyone else
//
//	returns the starting position of the real message (past the :)
//	destination will receive one of the following values:
//		MULTI_SEND_MESSAGE_ALL			=	everyone should get this message
//		MULTI_SEND_MESSAGE_RED_TEAM		= only red team (0) should get this message
//		MULTI_SEND_MESSAGE_BLUE_TEAM	= only blue team (1) should get this message
//		MULTI_SEND_MESSAGE_GREEN_TEAM	= only green team (2) should get this message
//		MULTI_SEND_MESSAGE_YELLOW_TEAM	= only yellow team (3) should get this message
//		0-32							= player num of the player to get the message
const char *GetMessageDestination(const char *message, int *destination);

// HUD System

//	predefined types (tHUDItem::type)
#define HUD_ITEM_PRIMARY 1
#define HUD_ITEM_SECONDARY 2
#define HUD_ITEM_SHIELD 3
#define HUD_ITEM_ENERGY 4
#define HUD_ITEM_AFTERBURNER 5
#define HUD_ITEM_INVENTORY 6
#define HUD_ITEM_SHIPSTATUS 7
#define HUD_ITEM_WARNINGS 8
#define HUD_ITEM_GOALS 9
#define HUD_ITEM_GOALSTATES 10
#define HUD_ITEM_CNTRMEASURE 11
#define HUD_ITEM_SCORE 12
#define HUD_ITEM_CUSTOMTEXT 20
#define HUD_ITEM_CUSTOMIMAGE 21
#define HUD_ITEM_TIMER 22
#define HUD_ITEM_CUSTOMTEXT2 23 // like custom text, keeps string pointer passed in

#define HUD_FLAG_PERSISTANT 1 // this hud item will not be resetted.
#define HUD_FLAG_SMALL 2      // when small version of hud is rendered, this will render in the AUX frame
#define HUD_FLAG_LEVEL 4      // persistent for the current level, but cleared between levels

#define HUD_INVALID_ID 255 // hud invalid id constant.

struct t_dirty_rect {
  struct {
    int16_t l, t, r, b;
  } r[3]; // three rectangles for each frame buffer (3 max)

  void set(int16_t l0, int16_t t0, int16_t r0, int16_t b0) {
    r[0].l = l0;
    r[0].t = t0;
    r[0].r = r0;
    r[0].b = b0;
  };
  void reset();
  void fill(ddgr_color col);
}; // dirty rectangle for hud item (small hud version)

struct tHUDItem {
  int16_t x, y;
  int16_t xa, ya; // auxillary points
  int16_t xb, yb;
  int16_t tx, ty;             // text version x and y.
  float grscalex, grscaley; // used to scale graphics.

  uint8_t id;    // id number.
  uint8_t type;  // custom of predefined hud item type.
  uint16_t stat; // stat mask (what class of hud items does this one belong to)

  uint16_t flags;           // more flags.
  uint8_t alpha;            // alpha of hud item
  uint8_t saturation_count; // how saturated is this hud item (how bright).

  ddgr_color color;  // color of hud item.
  ddgr_color tcolor; // text color of item.

  void (*render_fn)(struct tHUDItem *); // called by hud system to draw the hud item.

  int buffer_size; // for customtext2 item

  // data is initialized depending on 'type'.
  // custom types must have either element of data already initialized.
  //	all predefined types can ignore this data field externally.
  union {
    int bm_handle;    // custom images
    int timer_handle; // timer
    char *text;       // custom text.
  } data;

  t_dirty_rect dirty_rect; // used in small version of hud to clear only 'dirty' area
};

//	hud resources
struct sHUDResources {
  char hud_inf_name[PSFILENAME_LEN + 1];
  int arrow_bmp;
  int goal_complete_bmp;
  int goal_bmp;
  int lock_bmp[2];
  int wpn_bmp;
  int ship_bmp;
  int energy_bmp;
  int shield_bmp[NUM_SHIELD_GAUGE_FRAMES];
  int invpulse_bmp;
  int dot_bmp;
  int afterburn_bmp;
  int antigrav_bmp[2];
};

#define HUD_ARROW_SCALE 0.20f
#define HUD_WPN_SCALE 0.75f
#define HUD_SHIP_SCALE 0.33f
#define HUD_ENERGY_SCALE 0.50f
#define HUD_SHIELD_SCALE 0.625f
#define HUD_DOT_SCALE 0.20f
#define HUD_LOCK_SCALE 0.5f
#define HUD_BURN_SCALE 0.5f

#define HUD_CLOAKEND_TIME 3.0f

extern struct sHUDResources HUD_resources;
extern bool Small_hud_flag;

//	initializes hud system.
void InitHUD();

//	initializes hud for ship.
void InitShipHUD(int ship);

//	closes hud.
void CloseHUD();

// closes hud for current ship
void CloseShipHUD();

//	manually sets the cockpit display.
void SetHUDMode(tHUDMode mode);

// toggle the hud between cockput & fullscreen modes
void ToggleHUDMode();

// sets the hud item state(what's visible, how it's drawn, etc)
void SetHUDState(uint16_t hud_mask, uint16_t hud_gr_mask);

//	the current cockpit mode;
tHUDMode GetHUDMode();

//	places an item on the hud
void AddHUDItem(tHUDItem *item);

//	frees hud items based off their type class.
void FreeHUDItem(int item);

// Updates the customtext2 item, if there is one
void UpdateCustomtext2HUDItem(char *text);

// Returns the item number if there's a customtext2 item, else -1
int FindCustomtext2HUDItem();

//	resets hud
void ResetHUD();

//	loads in hud configuration file, adds hud items.
void LoadHUDConfig(const char *filename, bool (*fn)(const char *, const char *, void *) = NULL, void *data = NULL);

//	render cockpit and gauges frame
void RenderHUDFrame();

// renders hud frame before any graphics are drawn
void RenderPreHUDFrame();

// render auxillary hud
void RenderAuxHUDFrame();

// savegame system hooks
void SGSHudState(CFILE *fp);
bool LGSHudState(CFILE *fp);

// returns scaled line width
int RenderHUDGetTextLineWidth(const char *string);

// returns scaled text height
int RenderHUDGetTextHeight(const char *string);

//	renders a bitmap onto the hud
void RenderHUDQuad(int x, int y, int w, int h, float u0, float v0, float u1, float v1, int bm, uint8_t alpha,
                   int sat_count = 0);

//	renders text, scaled, alphaed, saturated,
void RenderHUDText(ddgr_color col, uint8_t alpha, int sat_count, int x, int y, const char *fmt, ...);

//	flags for RenderHudText.
#define HUDTEXT_CENTERED 1

//	renders text, scaled, alphaed, saturated,
void RenderHUDTextFlags(int flags, ddgr_color col, uint8_t alpha, int sat_count, int x, int y, const char *fmt, ...);

// reset hud messages.
void ResetHUDMessages();

// Sends off the input message the player was typing
void SendOffHUDInputMessage();

//	reset hud message list.
void ResetHUDMessages();

// opens a hud rollback console.
void OpenHUDMessageConsole();

// closes hud rollback console
void CloseHUDMessageConsole();

//	toggles hud rollback console
void ToggleHUDMessageConsole();

// resets game message list to no messages
void ResetGameMessages();
void AddGameMessage(const char *msg);
void SGSGameMessages(CFILE *fp);
void LGSGameMessages(CFILE *fp);

// toggles game message console (you usually call this.)
void ToggleGameMessageConsole();

// game message console
void OpenGameMessageConsole();
void CloseGameMessageConsole();

// pause/unpause hud message rendering
void HUDPauseMessages(void);
void HUDUnpauseMessages(void);

/////////////////////////////////////////////////////////////////////////
// Message Console System
/////////////////////////////////////////////////////////////////////////

struct tMsgList {
  int m_nmsg, m_limit;
  char **m_msg;

  tMsgList();
  void set_limit(int limit) { m_limit = limit; };
  bool add(const char *msg, uint8_t lvl = 0, uint8_t hr = 0, uint8_t min = 0, uint8_t sec = 0);
  const char *get(int i);
  void reset();
};

//////////////////////////////////////////////////////////////////////////////

class MsgListConsole {
  tMsgList *m_list;
  int m_x, m_y, m_w, m_h;
  int m_bufline, m_buflen, m_curmsgs, m_numlines;
  bool m_opened;
  char *m_buffer;
  char m_title[32];
  char **m_conlines;
  int n_conlines;
  int m_keydownstate; // -1 for up key, 1 for down key, 0 for none.
  float m_keydowntime;

public:
  MsgListConsole();
  ~MsgListConsole();
  bool Open(const char *title, int x, int y, int w, int h); // registers dimensions of box
  void Close();
  void AttachMsgList(tMsgList *msglist);
  void Draw();
  void DoInput(); // do scrolling ui on this console
};

#endif
