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
 * $Source: /home/kevin/cvsstuff/descent3/descent3/Main/FontEditor/FontCreate.cpp,v $
 * $Revision: 1.1.1.1 $
 * $Author: kevinb $
 * $Date: 2003-08-26 03:57:45 $
 *
 * Font Creation
 *
 * $Log: not supported by cvs2svn $
 * 
 * 3     11/16/99 3:18p Samir
 * updated fonteditor to include tracking.
 * 
 * 2     4/24/99 5:43p Samir
 * incremental checkin
 * 
 * 1     4/17/99 4:04p Samir
 * new font creator.
 * 
 */

#include <cstdarg>
#include <cstdio>

#include "FontEditor.h"
#include "pstypes.h"
#include "pserror.h"
#include "bitmap.h"
#include "renderer.h"
#include "grtext.h"
#include "ddio.h"
#include "mem.h"

#include <string.h>

//	Font File stuff

typedef FILE* FONTFILE;

#define BITS_TO_BYTES(x)    (((x)+7)>>3)

inline int WRITE_FONT_INT(FONTFILE ffile, int i) {
	return fwrite(&i, sizeof(i), 1, (FILE *)ffile);
}

inline int WRITE_FONT_SHORT(FONTFILE ffile, short s) {
	return fwrite(&s, sizeof(s), 1, (FILE *)ffile);
}

inline int WRITE_FONT_BYTE(FONTFILE ffile, ubyte c) {
	return fwrite(&c, sizeof(c), 1, (FILE *)ffile);
}

inline int WRITE_FONT_DATA(FONTFILE ffile, void *buf, int size, int nelem)
{
	int i;
	i = (int)fwrite(buf, size, nelem, (FILE *)ffile);

	ASSERT(i == nelem);

	return i;
}  

inline FONTFILE OPEN_FONT(char *filename) {
	FONTFILE fp;

	fp = (FONTFILE)fopen(filename, "wb");
	if (!fp) return NULL;

	return fp;
}

inline void CLOSE_FONT(FONTFILE ffile) {
	fclose((FILE *)ffile);
}


void message_box(const char *fmt, ...)
{
#define FIXED_MSGBOX_WIDTH  (512)
#define FIXED_MSGBOX_HEIGHT (128)

	std::va_list arglist;
	char buf[512];
	int len;

	va_start(arglist,fmt);
	len = std::vsnprintf(buf,512,fmt,arglist);
	va_end(arglist);
	if (len < 0) return;

	while(1)
	{
		int key;
		int l = (FIXED_SCREEN_WIDTH-FIXED_MSGBOX_WIDTH)/2;
		int t = (FIXED_SCREEN_HEIGHT-FIXED_MSGBOX_HEIGHT)/2;
		int r = l+FIXED_MSGBOX_WIDTH;
		int b = t+FIXED_MSGBOX_HEIGHT;

		FontDoIO();
		key = ddio_KeyInKey();
		if (key == KEY_ENTER || key == KEY_ESC) {
			break;
		}

		rend_StartFrame(0,0,FIXED_SCREEN_WIDTH,FIXED_SCREEN_HEIGHT,0);
		grtext_SetParameters(0,0,FIXED_SCREEN_WIDTH,FIXED_SCREEN_HEIGHT);	
		grtext_SetFont(System_font_handle);
		grtext_SetColor(GR_WHITE);

		rend_ClearScreen(GR_BLACK);
		rend_SetFlatColor(GR_WHITE);
		rend_DrawLine(l,t,r,t);
		rend_DrawLine(r,t,r,b);
		rend_DrawLine(l,b,r,b);
		rend_DrawLine(l,t,l,b);

		grtext_CenteredPrintf(0,t+30, buf);
		grtext_CenteredPrintf(0,b-30, "Press ENTER or ESC");
		grtext_Flush();
		rend_EndFrame();
		rend_Flip();
	}
}
	


//////////////////////////////////////////////////////////////////////////////////////
#define COLOR_FONT			1
#define MONOCHROME_FONT		2
#define MAX_FONT_CHARS		256



static bool m_FontProp;
static ubyte m_CharWidths[MAX_FONT_CHARS];
static short m_CharHeight;
static short m_CharMaxWidth;
static ushort *m_FontBmData;
static ushort *m_DataBuffer, *m_DataPtr;
static int m_FontType;
static int m_FontBmW, m_FontBmH;
static ushort m_FgColor, m_BgColor, m_BoxColor;

#define PIX(_x,_y) m_FontBmData[(_x) + (_y) * m_FontBmW]


int read_font_char(int cur_char, int& bmx, int& bmy)
{
	int w,h,x,y;
	bool box_edge = false;

	mprintf((0," %d-(%d,%d)",cur_char,bmx,bmy));

	while (PIX(bmx+1,bmy+1) == m_BoxColor)
		bmx++;		//deal with double-thick vertical lines

	for (w=0;PIX(bmx+1+w,bmy+1)!=m_BoxColor;w++)
	{
		if (PIX(bmx+1+w,bmy)!=m_BoxColor) {
			if (box_edge) {
				message_box("Read probable premature eol at (%d,%d).\n", bmx+1+w, bmy);
				w--;
				return 2;
			}
			else {
				message_box("Suspected error at pixel coordindates %d,%d (char %d)",bmx+1+w,bmy, cur_char);
				return 0;
			}
		}
		else {
			box_edge = true;
		}
	}

	box_edge = false;

	for (h=0;PIX(bmx+1,bmy+1+h)!=m_BoxColor;h++)
	{
		if (PIX(bmx,bmy+1+h)!=m_BoxColor) {
			if (box_edge) {
				message_box("Read probable premature end of row at (%d,%d) (pix=%04x,box=%04x).\n", bmx, bmy+1+h, PIX(bmx,bmy+1+h), m_BoxColor);
				h--;
				return 2;
			}
			else {
				message_box("Suspected error at pixel coordindates %d,%d (char %d)",bmx,bmy+1+h, cur_char);
				return 0;
			}
		}
		else {
			box_edge = true;
		}
	}

	if (w==0 || h==0) {
		message_box("Character %d has width of %d and height of %d",cur_char,w,h);
		return 0;
	}
	if (w > 255 || w< 0) {
		message_box("Character %d has bad width %d (maximum=255)", cur_char, w);
		return 0;
	}

	m_CharWidths[cur_char] = w;

	if (m_CharMaxWidth !=0 && w != m_CharMaxWidth)
		m_FontProp = true;

	if (w > m_CharMaxWidth) m_CharMaxWidth = w;

	if (m_CharHeight == 0)
		m_CharHeight = h;
	else if (m_CharHeight != h) {
		message_box("Char %d is wrong height (height = %d, correct height = %d)",cur_char,h,m_CharHeight);
		return 0;
	}

	for (y=0;y<h;y++)
		for (x=0;x<w;x++) 
		{
			ushort c;

			if ((c=PIX(bmx+1+x,bmy+1+y)) == m_BgColor)
				c = 0;										// OLD COMMENT-must go back 565 pure green (old transparent)
			else if (! (m_FgColor & 0xf000))			// 4444 check for alpha
				m_FgColor = c; 
			else if (m_FgColor != c) { 				// OLD_COMMENT-c here is 1555 !! remember this.
				m_FontType = COLOR_FONT;				//	mprintf((0, "%d,%d\n", bmx+1+x, bmy+1+y));
			}

//			if (c != 0) 
//				*m_DataPtr++ = c;							// OLD COMMENT-here 555 pure green will turn into 565 pure green transparent.
//			else
				*m_DataPtr++ = c;

			ASSERT(m_DataPtr < m_DataBuffer+(1024*256));
		}

	return 1;
}


int read_font_line(int cur_char, int& bmx, int& bmy)
{
	while (bmx < m_FontBmW && PIX(bmx+1,bmy) == m_BoxColor) 
	{
		int res;
		res = read_font_char(cur_char, bmx, bmy);
		if (res == 0) return -1;
		else if (res == 1) {
			bmx += m_CharWidths[cur_char]+1;
			cur_char++;
		}
		else if (res == 2) {
			cur_char = -1;
			break;
		}
		else break;
	}

	return cur_char;
}


bool extract_font(int bm_handle, tFontFileInfo *ft)
{
	int bmx, bmy;
	int cur_char;

//	grab font template bitmap data from the bitmap library and retrieve characters
//	store relevent information in file record.
	m_FontBmData = bm_data(bm_handle, 0);
	m_FontBmW = bm_w(bm_handle,0); 
	m_FontBmH = bm_h(bm_handle, 0);
	m_CharMaxWidth = m_CharHeight = 0;
	m_FontProp = false;
	m_FontType = MONOCHROME_FONT;
	m_FgColor = NEW_TRANSPARENT_COLOR;

	m_DataBuffer = new ushort[1024*MAX_FONT_CHARS];
	m_DataPtr = m_DataBuffer;

//	assume upper left pixel is background color, and first-found other
//	color is box color
	m_BgColor = PIX(0,0);
	mprintf((0, "m_BgColor=%04x\n", m_BgColor));

	for (bmy = 0;bmy  < m_FontBmH; bmy++) 
	{
		for (bmx = 0;bmx < m_FontBmW && PIX(bmx,bmy)==m_BgColor; bmx++) ;
		if (PIX(bmx,bmy)!= m_BgColor) break;
	}				

	m_BoxColor = PIX(bmx,bmy);
	mprintf((0, "m_BoxColor=%04x\n", m_BoxColor));

//	Get all data from current line.
	mprintf((0, "Parsing font..."));
	mprintf((0," read_characters: "));

	for (cur_char=0;bmy < m_FontBmH && cur_char<MAX_FONT_CHARS;) 
	{
		cur_char = read_font_line(cur_char, bmx, bmy);
		if (cur_char == -1) {
			delete[] m_DataBuffer;
			m_DataBuffer = NULL;
			return false;
		}
		bmy += m_CharHeight+1;

	//search for start of next box
		for (;bmy < m_FontBmH;bmy++) {
			for (bmx = 0;bmx<m_FontBmW && PIX(bmx,bmy)!=m_BoxColor;bmx++) ;
			if ((PIX(bmx,bmy)==m_BoxColor) && (PIX(bmx,bmy+1) == m_BoxColor)) 
				break;
		}

		if (bmy == m_FontBmH)
			break;
//		if (PIX(bmx,bmy+1) != m_BoxColor) 
//			break;
	}  
	mprintf((0, "\nDone!\n"));			

//	now fill in font information & store it into our file record. this information
//	can be used to save the font
	ft->width = m_CharMaxWidth;
	ft->height = m_CharHeight;
	ft->flags = (m_FontProp?FT_PROPORTIONAL:0) + ((m_FontType==COLOR_FONT)?FT_COLOR:0);
	ft->baseline = m_CharHeight;
	ft->min_ascii = 0;
	ft->max_ascii = cur_char-1;
	ft->raw_data = (ubyte *)m_DataBuffer;
	ft->char_data = NULL;
	ft->char_widths = m_CharWidths;
	ft->kern_data= NULL;

	ft->flags |= FT_FMT4444;

	return true;
}



//////////////////////////////////////////////////////////////////////////////////////////
// Font file format.
//
//	id=0xfeedbaba
//	maximum width
//	maximum height
//	font flags
//	baseline value (bashed brightness value)
// min and max ascii
//	name
//	for proportional fonts, store all widths
//	no kerning.
// store font data.


void FontCreate(const char *fnt_file_source, const char *fnt_file_dest, int min_ascii)
{
//	import font
	FONTFILE ffile;
	tFontFileInfo ft;
	int bm_handle;
	char fontname[32];

	strcpy(fontname, "d3font");

	bm_handle = bm_AllocLoadFileBitmap(fnt_file_source, 0, BITMAP_FORMAT_4444);
	if (bm_handle < 0) {
		Error("Couldn't open %s for extraction.", fnt_file_source);
	}

//	Get MinAscii and other needed info before extracting font.
	if (!extract_font(bm_handle, &ft)) {
		Error("Unable to generate font from %s.", fnt_file_source);
		return;
	}

///////////////////////////////////////////////////////////////////////////////
//	Adjust font record for file write.
	int num_char;

	ft.kern_data= NULL;
	ft.min_ascii = min_ascii;
	ft.max_ascii = ft.min_ascii + ft.max_ascii;

// add newstyle variables here.
	ft.flags |= FT_FFI2; 
	ft.ffi2.tracking = 0;

//	write to file
	mprintf((0, "Saving font data..."));
	ffile = OPEN_FONT((char *)fnt_file_dest);
	if (!ffile) {
		Error("Unable to save font %s.", fnt_file_dest);
	}
	
//	Write file id.
	WRITE_FONT_INT(ffile, 0xfeedbaba);

//	Write Header
	ft.baseline = 0;

	WRITE_FONT_SHORT(ffile, ft.width);
	WRITE_FONT_SHORT(ffile, ft.height);
	WRITE_FONT_SHORT(ffile, ft.flags);
	WRITE_FONT_SHORT(ffile, ft.baseline);
	WRITE_FONT_BYTE(ffile, ft.min_ascii);
	WRITE_FONT_BYTE(ffile, ft.max_ascii);
	WRITE_FONT_DATA(ffile, fontname, 32, 1);

	WRITE_FONT_SHORT(ffile, (short)ft.ffi2.tracking);
	WRITE_FONT_DATA(ffile, ft.ffi2.reserved, sizeof(ft.ffi2.reserved),1);
	
	num_char = (int)(ft.max_ascii - ft.min_ascii + 1);

//	Write widths now if necessary.(FT_PROPORTIONAL)
	if (ft.flags & FT_PROPORTIONAL) {
		for (int i = 0; i < num_char; i++)
			WRITE_FONT_SHORT(ffile, ft.char_widths[i]);
	}

	if (ft.flags & FT_COLOR) {
		WRITE_FONT_INT(ffile, (int)(m_DataPtr-m_DataBuffer)*sizeof(ushort));
		WRITE_FONT_DATA(ffile, (ubyte *)m_DataBuffer, (m_DataPtr-m_DataBuffer)*sizeof(ushort), 1);
	}
	else {
	// bitpack for mono font storage:: 16bpp -> 8 bits/1 byte
		int i,x,y,w,cnt=0;
		ushort *p = m_DataBuffer;
		ubyte *bits;

		bits = (ubyte *)mem_malloc(256 * MAX_FONT_CHARS);

		for (i = 0; i < num_char; i++)
		{
			for (y = 0; y < ft.height; y++) 
			{
				ubyte mask, datum;

				w = (ft.flags & FT_PROPORTIONAL) ? ft.char_widths[i] : ft.width;

				ASSERT(w <= 48);			// Max width size shall be 32 pixels 

				mask = 0x80; datum = 0;

				for (x = 0; x < w; x++) 
				{
					if (mask == 0) {
						bits[cnt++] = datum;
						datum = 0;
						mask = 0x80;
					}
					if (*p++ != 0x07e0) datum |= mask;
					mask >>= 1;
				}

				bits[cnt++] = datum;
			}
		}

		WRITE_FONT_INT(ffile, cnt);
		WRITE_FONT_DATA(ffile, bits, cnt, 1);
		mem_free(bits);
	}

//	We should add kerning support here.

//	Close font.
	CLOSE_FONT(ffile);

	delete[] m_DataBuffer;			// allocated in extract_font!
	bm_FreeBitmap(bm_handle);

	int new_font_handle = grfont_Load((char*)fnt_file_dest);
	if (new_font_handle < 0) {
		message_box("New font failed to load!");
	}


// print out stats.
	while (1)
	{
		int x,y,i;
		FontDoIO();

		if (ddio_KeyInKey() == KEY_ENTER) {
			break;
		}

		rend_StartFrame(0,0,FIXED_SCREEN_WIDTH, FIXED_SCREEN_HEIGHT);
		grtext_SetParameters(0,0,FIXED_SCREEN_WIDTH,FIXED_SCREEN_HEIGHT);	
		grtext_SetFont(System_font_handle);
		grtext_SetColor(GR_WHITE);
		rend_ClearScreen(GR_BLACK);

		grtext_Printf(0,12, "Created font:\2\65 %s", fnt_file_dest);
		grtext_Printf(0,28, "Type:\2\65 %s, %s", (ft.flags & FT_COLOR) ? "COLOR" : "MONO", (ft.flags & FT_PROPORTIONAL) ? "PROPORTIONAL" : "FIXED WIDTH");
		grtext_Printf(0,40, "Ascii:\2\65 %d to %d", ft.min_ascii,ft.max_ascii);
		grtext_Printf(0,52, "Maxwidth:\2\65 %d", ft.width);
		grtext_Printf(0,64, "Maxheight:\2\65 %d", ft.height);

		grtext_Printf(0,FIXED_SCREEN_HEIGHT-20, "Press ENTER to quit.");

	// spew new font.
		x=10; y = 120;
		grtext_SetFont(new_font_handle);
	
		for (i = ft.min_ascii; i <= ft.max_ascii; i++)
		{
			char str[2];
			str[0] = i;
			str[1] = 0;

			if ((x+grfont_GetCharWidth(new_font_handle, i)) > FIXED_SCREEN_WIDTH) {
				x = 10;
				y += grfont_GetHeight(new_font_handle)+1;
			}

			grtext_Puts(x,y,str);
			x+= grfont_GetCharWidth(new_font_handle, i)+1;
		}

		grtext_Flush();
		rend_EndFrame();
		rend_Flip();
	}

	grfont_Free(new_font_handle);
}


void FontView(const char *fnt_file_name)
{
	tFontTemplate ft;
	int new_font_handle;

	new_font_handle = grfont_Load((char *)fnt_file_name);
	if (new_font_handle < 0) {
		Error("Couldn't load font %s.", fnt_file_name);
	}
	grfont_LoadTemplate((char *)fnt_file_name, &ft);

	ddio_KeyFlush();
// print out stats.
	while (1)
	{
		int x,y,i;

		FontDoIO();

		if (ddio_KeyInKey() == KEY_ENTER) {
			break;
		}

		rend_StartFrame(0,0,FIXED_SCREEN_WIDTH, FIXED_SCREEN_HEIGHT);
		grtext_SetParameters(0,0,FIXED_SCREEN_WIDTH,FIXED_SCREEN_HEIGHT);	
		grtext_SetFont(System_font_handle);
		grtext_SetColor(GR_WHITE);
		rend_ClearScreen(GR_BLACK);

		grtext_Printf(0,12, "Font:\2\65 %s", fnt_file_name);
		grtext_Printf(0,28, "Type:\2\65 %s, %s, %s", (ft.monochromatic ? "MONO" : "COLOR"), (ft.proportional ? "PROPORTIONAL" : "FIXED WIDTH"), 
			(ft.newstyle ? "4444" : "1555"));
		grtext_Printf(0,40, "Ascii:\2\65 %d to %d", ft.min_ascii,ft.max_ascii);
		grtext_Printf(0,52, "Maxwidth:\2\65 %d", ft.ch_maxwidth);
		grtext_Printf(0,64, "Maxheight:\2\65 %d", ft.ch_height);

		if (ft.ffi2) {
			grtext_Printf(0,76, "Tracking:\2\65 %d", (int)ft.ch_tracking);
		}


		grtext_Printf(0,FIXED_SCREEN_HEIGHT-20, "Press ENTER to quit.");

	// spew new font.
		x=10; y = 120;
		grtext_SetFont(new_font_handle);
	
		for (i = ft.min_ascii; i <= ft.max_ascii; i++)
		{
			char str[2];
			str[0] = i;
			str[1] = 0;

			if ((x+grfont_GetCharWidth(new_font_handle, i)) > FIXED_SCREEN_WIDTH) {
				x = 10;
				y += grfont_GetHeight(new_font_handle)+1;
			}

			grtext_Puts(x,y,str);
			x+= grfont_GetCharWidth(new_font_handle, i)+1;
		}

		grtext_Flush();

		rend_EndFrame();
		rend_Flip();

	}

	grfont_Free(new_font_handle);
}
