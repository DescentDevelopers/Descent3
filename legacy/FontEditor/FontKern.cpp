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

 * $Source: /home/kevin/cvsstuff/descent3/descent3/Main/FontEditor/FontKern.cpp,v $
 * $Revision: 1.1.1.1 $
 * $Author: kevinb $
 * $Date: 2003-08-26 03:57:45 $
 *
 * Kerning
 *
 * $Log: not supported by cvs2svn $
 * 
 * 4     11/16/99 3:18p Samir
 * updated fonteditor to include tracking.
 * 
 * 3     5/19/99 8:48p Samir
 * added kerning dump utilitiy.
 * 
 * 2     4/17/99 4:05p Samir
 * complete font editor.
 * 
 */

#include "FontEditor.h"
#include "pserror.h"
#include "renderer.h"
#include "ddio.h"
#include "grtext.h"
#include "bitmap.h"
#include "mem.h"

#include <stdio.h>
#include <stdlib.h>


chunked_bitmap Editor_bkg;
tFontTemplate Font_template;

char *SampleText = "This is some sample text that is here to\n" \
"Show you how the antialiasing will\n"	\
"look over different color backgrounds\n" \
"KERN PAIRS: VaWaVeWeVAV-LyT.T,TyTvTcYe\n";


// this function determines if a kerning pair exists in the current font template
// if there is no kerning structure, add it. if no kerning in font, add it to the template
#define KERNINFO_PAIR_SIZE		3

int fonttool_get_kerning( int c1, int c2, int *pairnum=NULL);

int fonttool_get_kerning( int c1, int c2, int *pairnum)
{
	int i;
	uint8_t *kern_data;


	kern_data= Font_template.kern_data;
	if (kern_data) {
		i=0;
		while (kern_data[i] != 255)
		{
			if (kern_data[i] == (uint8_t)c1 && kern_data[i+1] == (uint8_t)c2) {
				if (pairnum) *pairnum = i/3;
				return (int)((int8_t)kern_data[i+2]);
			}
			i+=KERNINFO_PAIR_SIZE;
		}
	}

	return 0;	
}


void fonttool_set_kerning( int c1, int c2, int dist )
{
	int i,j;
	uint8_t *kern_data;

	if (c1 == 255 || c2 == 255) {
		mprintf((0, "seting illegal kerning of 255!\n"));
		return;
	}

	kern_data= Font_template.kern_data;

	if (!kern_data) {
	// create a one entry kern table.
		kern_data = (uint8_t *)mem_malloc(KERNINFO_PAIR_SIZE*2);
		kern_data[0] = (uint8_t)c1;
		kern_data[1] = (uint8_t)c2;
		kern_data[2] = (int8_t)dist;
		kern_data[3] = 255;
		kern_data[4] = 255;
		kern_data[5] = 0;
		Font_template.kern_data = kern_data;
		mprintf((0, "adding first kerning pair %d,%d\n", c1,c2));
		return;
	}

//	check for match in current list.
	i=0;
	while (kern_data[i] != 255)
	{
		if (kern_data[i] == (uint8_t)c1 && kern_data[i+1] == (uint8_t)c2) {
			kern_data[i+2] = (int8_t)dist;
			if (dist == 0) {
			//	remove this kerning pair.
				j = i;
				do
				{
					kern_data[j] = kern_data[j+3];
					kern_data[j+1] = kern_data[j+4];
					kern_data[j+2] = kern_data[j+5];
					j+= KERNINFO_PAIR_SIZE;
				}
				while (kern_data[j] != 255);
				if (i == 0) {
				// last pair, deallocate kern_data and end.
					mprintf((0, "removing last kerning pair\n"));
					mem_free(kern_data);
					kern_data = NULL;
				}
				else {
					mprintf((0, "removing kerning pair %d,%d\n", c1,c2));
				}
			}
			Font_template.kern_data = kern_data;
			return;
		}
		i+=KERNINFO_PAIR_SIZE;
	}

// new entry.
	uint8_t *new_kern_data;
	int n_pairs = i/KERNINFO_PAIR_SIZE;

	new_kern_data = (uint8_t *)mem_malloc((n_pairs+2)*sizeof(uint8_t)*KERNINFO_PAIR_SIZE);
	for (i = 0; i < n_pairs; i++)
	{
		new_kern_data[i*KERNINFO_PAIR_SIZE] = kern_data[i*3];
		new_kern_data[i*KERNINFO_PAIR_SIZE+1] = kern_data[i*3+1];
		new_kern_data[i*KERNINFO_PAIR_SIZE+2] = kern_data[i*3+2];
	}
	new_kern_data[i*KERNINFO_PAIR_SIZE] = (uint8_t)c1;
	new_kern_data[i*KERNINFO_PAIR_SIZE+1] = (uint8_t)c2;
	new_kern_data[i*KERNINFO_PAIR_SIZE+2] = (int8_t)dist;
	i++;
	new_kern_data[i*KERNINFO_PAIR_SIZE] = 255;
	new_kern_data[i*KERNINFO_PAIR_SIZE+1] = 255;
	new_kern_data[i*KERNINFO_PAIR_SIZE+2] = 0;
	
	mem_free(kern_data);

	Font_template.kern_data = new_kern_data;

	mprintf((0, "adding first kerning pair %d,%d (total=%d)\n", c1,c2, n_pairs+1));
}


void fonttool_remove_kerning()
{
	if (Font_template.kern_data) {
		mem_free(Font_template.kern_data);
		Font_template.kern_data = NULL;
	}
}


/////////////////////////////////////////////////////////////////////////////

void FontKern(const char *fnt_file_name)
{
	int bm_handle;
	int font_handle;
	int c1 = 'b', c2 = 'u', d;
	int last_good_pair, current_pair;
	int first_item = 0;
	int current_item = 0;
	int num_items_displayed = 1;
	bool done = false;
	int16_t text_alpha=255;
	int red_comp , green_comp, blue_comp;
	ddgr_color text_color;

	red_comp = green_comp = blue_comp = 255;

	bm_handle = bm_AllocLoadFileBitmap("FontTool.ogf",0);
	if (bm_handle <= BAD_BITMAP_HANDLE) {
		Error("Error loading FontEditor art.");
	}
	if (!bm_CreateChunkedBitmap(bm_handle, &Editor_bkg)) {
		Error("Error chunking FontEditor art.");
	}
	bm_FreeBitmap(bm_handle);

// load font into memory.
	font_handle = grfont_Load((char *)fnt_file_name);
	if (font_handle < 0) {
		Error("Error loading font %s", fnt_file_name);
	}
	if (!grfont_LoadTemplate((char *)fnt_file_name, &Font_template)) {
		Error("Error loading font template %s", fnt_file_name);
	}

	if (Font_template.ffi2 == false) {
		Font_template.ffi2 = true;
		Font_template.ch_tracking = 0;
	}

	if (Font_template.kern_data) {
		current_pair = 0;
	}
	else {
		current_pair = -1;
	}

	last_good_pair = current_pair;

	while(!done)
	{
		int key;

		FontDoIO();

		key = ddio_KeyInKey();
		switch (key)
		{
		case KEY_ESC:
			done = true;
			break;

		case KEY_F5:
			grfont_Free(font_handle);
			grfont_FreeTemplate(&Font_template);
			font_handle = grfont_Load((char *)fnt_file_name);
			if (font_handle == -1) {
				Error("Font failed reload test!");
			}
			grfont_LoadTemplate((char *)fnt_file_name, &Font_template);
			break;

		case KEY_MINUS:
			if (Font_template.ch_tracking > -80) {
				Font_template.ch_tracking--;
				grfont_SetTracking(font_handle, Font_template.ch_tracking);
			}
			break;

		case KEY_EQUAL:
			if (Font_template.ch_tracking < 80) {
				Font_template.ch_tracking++;
				grfont_SetTracking(font_handle, Font_template.ch_tracking);
			}
			break;

		case KEY_F10:
			grfont_Free(font_handle);
			if (!grfont_SetTemplate(fnt_file_name, &Font_template)) {
				Error("Failed to save font correctly!");
			}
			grfont_FreeTemplate(&Font_template);
			font_handle = grfont_Load((char *)fnt_file_name);
			if (font_handle == -1) {
				Error("Font failed reload test on save!");
			}
			grfont_LoadTemplate((char *)fnt_file_name, &Font_template);
			mprintf((0, "font saved and reloaded correctly?\n"));
			done = true;
			break;

		case KEY_F6:
			fonttool_remove_kerning();
			break;

		case KEY_COMMA:
			text_alpha -= 16;
			if (text_alpha < 0) text_alpha = 0;
			break;

		case KEY_PERIOD:
			text_alpha += 16;
			if (text_alpha > 255) text_alpha = 255;
			break;

		case KEY_PAD7:
			c1++;
			if (c1 > Font_template.max_ascii) c1 = Font_template.min_ascii;
			break;

		case KEY_PAD9:
			c2++;
			if (c2 > Font_template.max_ascii) c2 = Font_template.min_ascii;
			break;

		case KEY_PAD1:
			c1--;
			if (c1 < Font_template.min_ascii) c1 = Font_template.max_ascii;
			break;

		case KEY_PAD3:
			c2--;
			if (c2 < Font_template.min_ascii) c2 = Font_template.max_ascii;
			break;

		case KEY_PAD4:
			d = fonttool_get_kerning(c1,c2);
			fonttool_set_kerning(c1, c2, d-1);
			grfont_SetKerning(font_handle, Font_template.kern_data);
			break;

		case KEY_PAD5:
			fonttool_set_kerning(c1,c2,0);
			break;

		case KEY_PAD6:
			d = fonttool_get_kerning(c1,c2);
			fonttool_set_kerning(c1,c2,d+1);
			grfont_SetKerning(font_handle, Font_template.kern_data);
			break;

		case KEY_PAD2:
			if ( current_pair < 0 ) {
				current_pair = last_good_pair;
			}

			if (current_pair >= 0) {
				uint8_t *kern_data = &Font_template.kern_data[current_pair*3];
				if (kern_data[3] != 255) {
					current_pair++;
					kern_data = &Font_template.kern_data[current_pair*3];
				}
				c1 = kern_data[0];
				c2 = kern_data[1];
			}
			break;

		case KEY_PAD8:
			if ( current_pair < 0 ) {
				current_pair = last_good_pair;
			}

			if (current_pair >= 0) {
				uint8_t *kern_data = &Font_template.kern_data[current_pair*3];
				if (kern_data != Font_template.kern_data) {
				// gosh, comparing pointers like this may not be the best method...this is shitty code...
					current_pair--;
					kern_data = &Font_template.kern_data[current_pair*3];
				}
				c1 = kern_data[0];
				c2 = kern_data[1];
			}
			break;

		case KEY_R:
			red_comp = red_comp ? 0 : 255;
			break;
		case KEY_G:
			green_comp = green_comp ? 0 : 255;
			break;
		case KEY_B:
			blue_comp = blue_comp ? 0 : 255;
			break;
		}

	//	draw.
		text_color = GR_RGB(red_comp, green_comp, blue_comp);

		rend_StartFrame(0,0,FIXED_SCREEN_WIDTH,FIXED_SCREEN_HEIGHT,0);
		grtext_SetParameters(0,0,FIXED_SCREEN_WIDTH,FIXED_SCREEN_HEIGHT);	
		grtext_SetFont(font_handle);
		grtext_SetColor(text_color);
		grtext_SetAlpha((uint8_t)text_alpha);

		rend_ClearScreen(GR_BLACK);
		rend_DrawChunkedBitmap(&Editor_bkg, 0, 0, 255);
		
		grtext_Printf(240,210,"%c (%d)", c1, c1);
		grtext_Printf(340,210,"%c (%d)", c2, c2);

		grtext_CenteredPrintf(0, 240, "Ham%c%crger", c1, c2);

		grtext_CenteredPrintf(0, 270, "HAM%c%cRGER", c1, c2);

		grtext_CenteredPrintf(0, 300, "Offset: %d pixels", fonttool_get_kerning(c1, c2));
		grtext_CenteredPrintf(0, 320, "Tracking: %d pixels", (int)Font_template.ch_tracking);
		{
			int th;
			th = grtext_GetTextHeight(SampleText);

			grtext_Printf(10,350,SampleText);
			grtext_Printf(10,350+th+10, SampleText);
		}

	// display kerned pairs.
		int x = 5, y = 200, i, n, widest=0;
		int n_pairs=0, stop;
		int tmpp=-1;
		char kerntext[16];

		if (Font_template.kern_data) {
			i=0;
			while (Font_template.kern_data[i] != 255)
			{
				n_pairs++;
				i+=KERNINFO_PAIR_SIZE;
			}
		}

	// update current pair values
		if ( current_pair >= n_pairs )	{
			current_pair = -1;
		}

		fonttool_get_kerning(c1, c2, &tmpp );

		if ( tmpp != -1 )	{
			current_pair = tmpp;
		} else	{
			if ( current_pair > -1 )
				last_good_pair = current_pair;
			current_pair = -1;
		}
		if (current_pair > -1) { current_item = current_pair;	}
		if (current_item < 0) {	current_item = 0;	}
		if (current_item >= n_pairs) { current_item = n_pairs-1;	}
		if (current_item < first_item) { first_item = current_item; }
		if (current_item >= (first_item+num_items_displayed)) { first_item = current_item-num_items_displayed+1; }
		if (n_pairs <= num_items_displayed) { first_item = 0; }
		stop = first_item+num_items_displayed;
		if (stop>n_pairs) stop = n_pairs;

		for (i=first_item,n=0; i<stop; i++)
		{
			int tw,th;
			ASSERT(Font_template.kern_data);
			uint8_t *kern_data = &Font_template.kern_data[i*3];
			if (i==current_pair) {
				rend_SetFlatColor(GR_RGB(255,0,0));
				rend_DrawLine(x,y,x+6,y+5);
				rend_DrawLine(x+6,y+5,x,y+8);
				rend_DrawLine(x,y+8,x,y);
			}
			sprintf(kerntext, "%c%c", kern_data[0], kern_data[1]);
			grtext_Puts(x+8,y,kerntext);
			n++;
			tw = grtext_GetTextLineWidth(kerntext) + 8;
			th = grtext_GetTextHeight(kerntext);
			if (tw > widest) {widest = tw; }
			y += th;
			if (y > 330) {
				y = 200;
				x += widest+5;
				if (x > 150) {
					num_items_displayed = n;
					break;
				}
				widest = 0;
			}
		}
		if (i == stop) {	num_items_displayed++; }
		if (num_items_displayed < 1) {num_items_displayed = 1;}
		if (num_items_displayed > n_pairs) { num_items_displayed = n_pairs; }


		grtext_Flush();
		rend_EndFrame();
		rend_Flip();
	}

	grfont_FreeTemplate(&Font_template);
	grfont_Free(font_handle);
	bm_DestroyChunkedBitmap(&Editor_bkg);
}



void FontKernDump(const char *fnt_file_name)
{
	if (!grfont_LoadTemplate((char *)fnt_file_name, &Font_template)) {
		Error("Error loading font template %s", fnt_file_name);
	}

	if (Font_template.kern_data) {
		FILE *fp;
		char filename[_MAX_PATH];

		sprintf(filename, "%s.kern", fnt_file_name);
		fp = fopen(filename, "wt");
		if (fp) {
			uint8_t *kern_data = Font_template.kern_data;
			while (kern_data[0] != 255)
			{
				uint8_t c1 = kern_data[0];
				uint8_t c2 = kern_data[1];
				int8_t delta = (int8_t)kern_data[2];

				fprintf(fp, "%c %c  => %d spacing\n", c1,c2,(int)delta);
				kern_data += 3;
			}
			fclose(fp);
		}
	}

	grfont_FreeTemplate(&Font_template);
}
