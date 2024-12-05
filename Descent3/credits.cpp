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

 * $Logfile: /DescentIII/main/credits.cpp $
 * $Revision: 17 $
 * $Date: 10/05/99 3:00p $
 * $Author: Kevin $
 *
 * Credits system
 *
 * $Log: /DescentIII/main/credits.cpp $
 *
 * 17    10/05/99 3:00p Kevin
 * No longer display static credits for demo
 *
 * 16    5/13/99 6:41p Samir
 * don't reset mouse when ending credits.  causes problems.
 *
 * 15    5/08/99 1:43p Jason
 * made credits scroll a little faster
 *
 * 14    5/05/99 7:38p Jason
 * added scrolling credits system
 *
 * 13    5/02/99 12:08a Jason
 * changes to credits
 *
 * 12    4/30/99 5:42p Jason
 * more updates to credits system
 *
 * 11    4/29/99 11:57a Samir
 * added music.
 *
 * 10    4/19/99 12:46p Jason
 * fixed credits
 *
 * 9     4/17/99 3:29p Jason
 * first pass at credit system
 *
 * 8     4/15/99 2:32p Kevin
 * Added some code for the Demo
 *
 * 7     4/14/99 4:19a Jeff
 * more case mismatch fixes in #includes
 *
 * 6     4/13/99 12:24p Jason
 * made credits warp faster
 *
 * 5     4/02/99 5:47p Jeff
 * text effect in
 *
 * 4     4/02/99 6:03a Jeff
 * testing a new text effect...not done yet
 *
 * 3     3/31/99 3:04p Jeff
 * added _real_ temporary credits system...
 *
 * 2     3/31/99 12:11p Jeff
 * added empty credits file
 *
 * $NoKeywords: $
 */

#include <cstring>
#include <vector>

#include "bitmap.h"
#include "crossplat.h"
#include "d3music.h"
#include "ddio.h"
#include "descent.h"
#include "game.h"
#include "gamefont.h"
#include "grtext.h"
#include "hlsoundlib.h"
#include "log.h"
#include "pserror.h"
#include "psrand.h"
#include "renderer.h"

/*
$$TABLE_GAMEFILE "GameCredits.txt"
$$TABLE_GAMEFILE "credits.omf"
*/

// music regions for credits
#define CREDITS1_MUSIC_REGION 0

// Speed of scrolling
#define CREDIT_PIXELS_PER_SECOND 22

#define MAX_CREDIT_LEN 200

// other stuff
#define CLTYPE_HEADING 0
#define CLTYPE_TEXT 1
#define CLTYPE_END 2
#define CLTYPE_BLANK 3

struct creditline {
  ddgr_color color{};
  uint8_t type{};
  std::string text;
  uint16_t startx{};
  uint16_t starty{};
  uint16_t endx{};
  uint16_t endy{};
  float displaytime{};
};

static chunked_bitmap Credits_bm;

void Credits_Render(std::vector<creditline>::iterator credit, std::vector<creditline>::iterator end, float pixels_in);

static bool Credits_IsKeyPressed() {
  if (ddio_KeyInKey())
    return true;

  return false;
}

// Parses a credit line
// Returns 1 if new text was read, else 0
static bool Credits_LoadCredits(const char *filename, std::vector<creditline> &credit_lines) {
  char curline[MAX_CREDIT_LEN];

  // Reset our data
  ddgr_color CreditTextColor = GR_RGB(0, 128, 255);
  ddgr_color CreditHeadingColor = GR_RGB(255, 255, 255);

  creditline cur_credit;

  // Open the file, bail on failure
  CFILE *infile = cfopen(filename, "rt");

  if (!infile) {
    LOG_ERROR << "Couldn't open credit file to read credits!";
    return false;
  }

  // Read in each line of the credit file, allocing memory and putting it into a correct category
  while (!cfeof(infile)) {
    cf_ReadString(curline, MAX_CREDIT_LEN, infile);
    if (strnicmp("END", curline, 3) == 0) {
      // End of credits, don't parse file further.
      break;
    }

    size_t line_len = strlen(curline);

    if (line_len < 1) {
      cur_credit.type = CLTYPE_BLANK;
      cur_credit.text = "";
      credit_lines.push_back(cur_credit);
    } else {
      int num_found;
      int r, g, b;
      switch (curline[0]) {
      case ';':
        // Comment
        break;
      case '*':
        // Movement data
        int x1, x2, y1, y2;
        float timedelay;

        num_found = sscanf(curline, "*%d %d %d %d %f", &x1, &y1, &x2, &y2, &timedelay);
        if (num_found != 5) {
          LOG_WARNING.printf("Bad movement data in credit text (%s)", curline);
          break;
        } else {
          cur_credit.startx = x1;
          cur_credit.starty = y1;
          cur_credit.endx = x2;
          cur_credit.endy = y2;
          cur_credit.displaytime = timedelay;
        }
        break;
      case '!':
        // Heading
        cur_credit.text = &curline[1];
        LOG_DEBUG.printf("Read header %s", curline);
        cur_credit.color = CreditHeadingColor;
        cur_credit.type = CLTYPE_HEADING;
        credit_lines.push_back(cur_credit);
        break;
      case '$':
        // Text color
        num_found = sscanf(curline, "$%d %d %d", &r, &g, &b);
        if (num_found != 3) {
          LOG_WARNING.printf("Bad color in credit text (%s)", curline);
          break;
        } else {
          CreditTextColor = GR_RGB(r, g, b);
        }
        break;
      case '^':
        // Heading color
        num_found = sscanf(curline, "^%d %d %d", &r, &g, &b);
        if (num_found != 3) {
          LOG_WARNING.printf("Bad color in credit text (%s)", curline);
          break;
        } else {
          CreditHeadingColor = GR_RGB(r, g, b);
        }
        break;
      default:
        if (!strnicmp("Jason Leighton", curline, 14) && (ps_rand() % 100) == 0) {
          strcat(curline, " (Hi mom!)");
        }

        cur_credit.text = curline;
        cur_credit.color = CreditTextColor;
        cur_credit.type = CLTYPE_TEXT;
        credit_lines.push_back(cur_credit);
      }
    }
  }

  cfclose(infile);
  return true;
}

void Credits_Display() {
#ifdef DEMO
  // ShowStaticScreen("democredits1.ogf");
  // ShowStaticScreen("democredits2.ogf");
  // return;
#else
  // Load background image
  int bm_handle = bm_AllocLoadFileBitmap("CreditsBackground.tga", 0);

  if (bm_handle < 0) {
    LOG_WARNING << "Failed to load background credit screen!";
    return;
  }

  if (!bm_CreateChunkedBitmap(bm_handle, &Credits_bm)) {
    Error("Failed to slice up credits screen!");
  }

  bm_FreeBitmap(bm_handle);

  // start up the music
  D3MusicStart("credits.omf");
  D3MusicSetRegion(CREDITS1_MUSIC_REGION);

  LOG_INFO << "Chillin in credits";

  std::vector<creditline> credit_lines;

  // Generate blank lines for the start
  creditline cur_credit;
  cur_credit.type = CLTYPE_BLANK;
  for (int i = 0; i < 27; i++) {
    credit_lines.push_back(cur_credit);
  }

  // Load our credits
  if (!Credits_LoadCredits("gamecredits.txt", credit_lines) || !Credits_LoadCredits("oscredits.txt", credit_lines)) {
    LOG_WARNING << "There was an error loading game credits!";
    // Free bitmaps
    bm_DestroyChunkedBitmap(&Credits_bm);

    // end the music.
    D3MusicStop();
    ddio_KeyFlush();

    return;
  }

  // End directive to be sure
  cur_credit.type = CLTYPE_END;
  cur_credit.text = "";
  credit_lines.push_back(cur_credit);

  auto credit = credit_lines.begin();

  int font_height = grfont_GetHeight(BIG_BRIEFING_FONT);

  float cur_pixel_count = 0;
  float last_time = timer_GetTime();

  // Now show these guys
  while (credit != credit_lines.end()) {
    StartFrame(0, 0, Game_window_w, Game_window_h);
    rend_DrawChunkedBitmap(&Credits_bm, 0, 0, 255);
    Credits_Render(credit, credit_lines.end(), cur_pixel_count);
    EndFrame();
    rend_Flip();

    float frametime = timer_GetTime() - last_time;
    last_time = timer_GetTime();

    cur_pixel_count += frametime * CREDIT_PIXELS_PER_SECOND;

    while (cur_pixel_count >= font_height) {
      credit++;
      if (credit == credit_lines.end() || credit->type == CLTYPE_END) {
        cur_pixel_count = -1;
      } else {
        cur_pixel_count -= font_height;
      }
    }

    // Player music for this frame
    tMusicSeqInfo music_info{};
    Sound_system.BeginSoundFrame(false);
    music_info.frametime = frametime;
    music_info.player_dead = false;
    music_info.started_level = false;
    D3MusicDoFrame(&music_info);
    Sound_system.EndSoundFrame();

    // Check for keys
    Descent->defer();
    if (Credits_IsKeyPressed()) {
      break;
    }
  }

  // Free bitmaps
  bm_DestroyChunkedBitmap(&Credits_bm);

  // end the music.
  D3MusicStop();
  ddio_KeyFlush();
#endif
}

void Credits_Render(std::vector<creditline>::iterator credit, std::vector<creditline>::iterator end, float pixels_in) {
  bool done = false;

  grtext_Reset();
  grtext_SetFont(BIG_BRIEFING_FONT);
  grtext_SetFontScale(1.0);
  int text_height = grfont_GetHeight(BIG_BRIEFING_FONT);

  float pixels_this_frame = 0;
  int cur_pixel = -pixels_in;

  while ((pixels_this_frame < 480 + text_height) && !done) {
    if (credit->type == CLTYPE_BLANK) {
      credit++;

      pixels_this_frame += text_height;
      cur_pixel += text_height;

      if ((credit == end) || (credit->type == CLTYPE_END))
        done = true;

      continue;
    }
    grtext_SetColor(credit->color);

    // Figure out alpha
    if (cur_pixel < 0) {
      float norm = 1.0f - ((-cur_pixel) / (float)text_height);
      grtext_SetAlpha(norm * 255);
    } else if ((cur_pixel + text_height) > 480) {
      float norm = 1.0f - (((cur_pixel + text_height) - 480) / (float)text_height);
      grtext_SetAlpha(norm * 255);
    } else
      grtext_SetAlpha(255);

    grtext_CenteredPrintf(0, cur_pixel, "%s", credit->text.c_str());

    credit++;

    if ((credit == end) || (credit->type == CLTYPE_END))
      done = true;

    pixels_this_frame += text_height;
    cur_pixel += text_height;
  }

  grtext_Flush();
}
