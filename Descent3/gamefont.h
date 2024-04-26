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
*/

/*
 * $Logfile: /DescentIII/Main/gamefont.h $
 * $Revision: 17 $
 * $Date: 5/02/99 11:04p $
 * $Author: Matt $
 *
 *	Game fonts
 *
 * $Log: /DescentIII/Main/gamefont.h $
 *
 * 17    5/02/99 11:04p Matt
 * Cleaned up HUD font code, now that we have only two sizes of HUD fonts.
 *
 * 16    4/27/99 2:13p Matt
 * Replaced the two (new) new UI fonts with one new font from Doug.
 *
 * 15    4/26/99 10:34p Matt
 * Deleted now-unused UI font.
 *
 * 14    4/25/99 5:19p Matt
 * Revised font definition & loading
 *
 * 13    4/23/99 9:25p Matt
 * Killed 'SMALL_FONT', since it was never being used.
 *
 * 12    4/16/99 5:06p Matt
 * Deleted big UI font, which was unused.
 *
 * 11    3/02/99 6:26p Samir
 * hires font madness.
 *
 * 10    2/28/99 3:23a Samir
 * added superhires fonts.
 *
 * 9     2/16/99 12:08p Samir
 * added new fonts.
 *
 * 8     10/20/98 3:20p Samir
 * added big ui font.
 *
 * 7     5/15/98 5:35p Samir
 * allow usage of hires and lores fonts.
 *
 * 6     4/16/98 6:51a Samir
 * added big briefing font.
 *
 * 5     3/05/98 6:40p Samir
 * New UI Font.
 *
 * 4     1/12/98 5:23p Samir
 * New fonts.
 *
 * 3     12/29/97 5:46p Samir
 * Modified gamefont system.
 *
 * 2     11/10/97 12:56p Samir
 * Macros for font rendering using the renderer.
 *
 * 1     11/04/97 2:03p Samir
 * Game font manager.
 *
 * $NoKeywords: $
 */

#ifndef GAMEFONT_H
#define GAMEFONT_H

#include "grtext.h"

extern int Game_fonts[]; // D3 font handles

// Font defines

#define NUM_FONTS 6 // How many fonts we have

// Indices for the font handles
#define SMALL_FONT_INDEX 0    // Basic small font, used everywhere. Originally designed for briefing
#define BIG_FONT_INDEX 1      // Basic big Font, used for titles.
#define HUD_FONT_INDEX 2      // Dropshadowed font so can be seen over variable background
#define MENU_FONT_INDEX 3     // Main menu font, also used for some multiplayer messages
#define SMALL_UI_FONT_INDEX 4 // For the new UI
#define BIG_UI_FONT_INDEX 5   // For the new UI

// Handles to the fonts
#define SMALL_FONT Game_fonts[SMALL_FONT_INDEX]
#define BIG_FONT Game_fonts[BIG_FONT_INDEX]
#define HUD_FONT Game_fonts[HUD_FONT_INDEX]
#define MENU_FONT Game_fonts[MENU_FONT_INDEX]
#define SMALL_UI_FONT Game_fonts[SMALL_UI_FONT_INDEX]
#define BIG_UI_FONT Game_fonts[BIG_UI_FONT_INDEX]

// These are equivalencies for the base fonts
#define BRIEFING_FONT SMALL_FONT
#define BIG_BRIEFING_FONT BIG_FONT
#define BRIEF_FONT_INDEX SMALL_FONT_INDEX
#define BBRIEF_FONT_INDEX BIG_FONT_INDEX
#define MONITOR9_NEWUI_FONT SMALL_UI_FONT
#define MONITOR15_NEWUI_FONT BIG_UI_FONT
#define GADGET9_NEWUI_FONT SMALL_UI_FONT
#define GADGET15_NEWUI_FONT BIG_UI_FONT

//	loads all game fonts.
void LoadAllFonts();

// Set the HUD font resolution based on the render width
void SelectHUDFont(int rend_with);

#endif
