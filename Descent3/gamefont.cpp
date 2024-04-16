/*
 * $Logfile: /DescentIII/main/gamefont.cpp $
 * $Revision: 28 $
 * $Date: 9/23/99 12:02p $
 * $Author: Jeff $
 *
 *	Game Font!
 *
 * $Log: /DescentIII/main/gamefont.cpp $
 * 
 * 28    9/23/99 12:02p Jeff
 * include stdlib for atexit
 * 
 * 27    5/10/99 10:22p Ardussi
 * changes to compile on Mac
 * 
 * 26    5/02/99 11:04p Matt
 * Cleaned up HUD font code, now that we have only two sizes of HUD fonts.
 * 
 * 25    4/27/99 2:13p Matt
 * Replaced the two (new) new UI fonts with one new font from Doug. 
 * 
 * 24    4/26/99 10:34p Matt
 * Deleted now-unused UI font.
 * 
 * 23    4/25/99 5:19p Matt
 * Revised font definition & loading
 * 
 * 22    4/23/99 9:25p Matt
 * Killed 'SMALL_FONT', since it was never being used.
 * 
 * 21    4/16/99 5:06p Matt
 * Deleted big UI font, which was unused.
 * 
 * 20    3/02/99 6:26p Samir
 * hires font madness.
 * 
 * 19    2/28/99 3:23a Samir
 * added superhires fonts.
 * 
 * 18    2/16/99 12:08p Samir
 * added new fonts.
 * 
 * 17    10/20/98 3:20p Samir
 * added big ui font.
 * 
 * 16    6/16/98 10:38a Jeff
 * localization, strings pulled out to stringtable.h and d3.str
 * 
 * 15    5/15/98 5:35p Samir
 * allow usage of hires and lores fonts.
 * 
 * 14    4/28/98 11:15a Samir
 * added big briefing load font.
 * 
 * 13    4/27/98 10:50a Samir
 * new hud font added.
 * 
 * 12    4/16/98 6:51a Samir
 * added big briefing font.
 * 
 * 11    4/13/98 7:01p Samir
 * added snazzy listbox and edit box art.
 * 
 * 10    4/13/98 4:15p Samir
 * hud now has new font, deal with it.
 * 
 * 9     4/13/98 3:32p Samir
 * Replaced HUD font with old UI font.
 * 
 * 8     4/03/98 10:27a Samir
 * commented out code to load a separate UI font.
 * 
 * 7     3/06/98 3:24p Samir
 * uifont = briefing font.
 * 
 * 6     3/05/98 6:40p Samir
 * New UI Font.
 * 
 * 5     1/12/98 5:23p Samir
 * New fonts.
 * 
 * 4     12/29/97 5:46p Samir
 * Modified gamefont system.
 * 
 * 3     12/19/97 11:01a Samir
 * Stubbed out functions.
 * 
 * 2     11/10/97 12:56p Samir
 * Macros for font rendering using the renderer.
 * 
 * 1     11/04/97 2:03p Samir
 * Game fonts.
 *
 * $NoKeywords: $
 */

#include <stdlib.h>
#include "gamefont.h"
#include "game.h"
#include "grtext.h"
#include "pserror.h"
#include "stringtable.h"

int Game_fonts[NUM_FONTS] = {0,0,0,0,0,0,};

//HUD font sizes
#define HUD_FONT_LOWRES		0
#define HUD_FONT_HIGHRES	1

char *HUD_font_files[] = {"lohud.fnt","hihud.fnt"};

#define HUD_FONT_FILE HUD_font_files[HUD_font_resolution]

static int HUD_font_resolution = HUD_FONT_LOWRES;
static bool Hud_font_template_init = false;
tFontTemplate Hud_font_template;								// retain hud font template

void FreeAuxFontData()
{
	if (Hud_font_template_init) {
		Hud_font_template_init = false;
		grfont_FreeTemplate(&Hud_font_template);
	}
}

//Loads the font and returns the handle.  Exits with an error if the font can't be found.
int LoadFont(char *font_name)
{
	int handle = grfont_Load(font_name);
	if (handle == -1)
		Error(TXT_ERRNOFONT,font_name);

	mprintf((0,"Font <%s> height = %d\n",font_name,grfont_GetHeight(handle)));

	return handle;
}

//	loads all game fonts.
void LoadAllFonts()
{
	grfont_Reset();

	atexit(FreeAuxFontData);

	if (Hud_font_template_init) {
		grfont_FreeTemplate(&Hud_font_template);
	}

	//Load the fonts (except HUD)
	SMALL_FONT				= LoadFont("briefing.fnt");
   BIG_FONT					= LoadFont("bbriefing.fnt");
	MENU_FONT				= LoadFont("newmenu.fnt");
	SMALL_UI_FONT			= LoadFont("smallui.fnt");
	BIG_UI_FONT				= LoadFont("largeui.fnt");

	//Load the HUD font
	HUD_font_resolution = HUD_FONT_LOWRES;
	HUD_FONT = LoadFont(HUD_FONT_FILE);
	if (!grfont_LoadTemplate(HUD_FONT_FILE, &Hud_font_template))
		Error(TXT_ERRNOFONT, HUD_FONT_FILE);
	Hud_font_template_init = true;
}

//If screen width greater than this, use the superhires fonts
#define SUPERHIRES_THRESHOLD_W		1024

//Set the HUD font resolution based on the render width
void SelectHUDFont(int rend_width)
{
   int fontres = (rend_width >= SUPERHIRES_THRESHOLD_W) ? HUD_FONT_HIGHRES : HUD_FONT_LOWRES;

	if (fontres == HUD_font_resolution)
		return;

	HUD_font_resolution = fontres;

	grfont_Free(HUD_FONT);

	HUD_FONT = LoadFont(HUD_FONT_FILE);
}

