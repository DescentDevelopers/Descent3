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

 * $Logfile: /DescentIII/Main/editor/GrFontDialog.cpp $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:57:38 $
 * $Author: kevinb $
 *
 * Font Editor
 *
 * $Log: not supported by cvs2svn $
 * 
 * 11    4/17/99 6:15p Samir
 * replaced gr.h with grdefs.h and fixed resulting compile bugs.
 * 
 * 10    5/08/98 5:23p Samir
 * added font brightness and grayscale caps.
 * 
 * 9     4/30/98 3:02p Samir
 * better error checking.
 * 
 * 8     4/29/98 3:13p Samir
 * fixed font translation.
 * 
 * 7     4/23/98 6:38p Jason
 * made bitmaps use 1555 format
 * 
 * 6     3/31/98 3:49p Jason
 * added memory lib
 * 
 * 5     3/06/98 3:34p Samir
 * Fixed font image scanning to scan multiple rows of fonts.
 * 
 * 4     1/28/98 3:29p Samir
 * Increased max height of font.
 * 
 * 3     1/12/98 5:22p Samir
 * Fixed up font reading.
 * 
 * 2     9/02/97 3:28p Matt
 * Got rid of warnings
 * 
 * 6     6/03/97 5:37p Mark
 * 
 *	6	6/03/97	5:32p	Jeff
 * Added Context Sensitive Help
 *
 * 5     2/06/97 3:08p Samir
 * Improved font saving for monochrome fonts
 *
 * $NoKeywords: $
 */
// GrFontDialog.cpp : implementation file
//

#include "stdafx.h"
#include <stdlib.h>
#include <stdio.h>

#include "editor.h"
#include "GrFontDialog.h"

#include "gr.h"
#include "mono.h"
#include "bitmap.h"
#include "gametexture.h"
#include "manage.h"
#include "mem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define COLOR_FONT		1
#define MONOCHROME_FONT		2
#define GRADIENT_FONT	3

#define FT_COLOR				1
#define FT_PROPORTIONAL		2
#define FT_KERNED				4
#define FT_GRADIENT			8


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


/////////////////////////////////////////////////////////////////////////////
// CGrFontDialog dialog


CGrFontDialog::CGrFontDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CGrFontDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGrFontDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_FontPicBm = -1;
	m_DataBuffer = NULL;

	ZeroMemory(&m_FontRecord, sizeof(m_FontRecord));
}


void CGrFontDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGrFontDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGrFontDialog, CDialog)
	//{{AFX_MSG_MAP(CGrFontDialog)
	ON_BN_CLICKED(IDC_NEW, OnNew)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_SAVEAS, OnSaveAs)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	ON_WM_DESTROY()
	ON_EN_KILLFOCUS(IDC_EDIT_MINASCII, OnKillfocusEditMinascii)
	ON_EN_KILLFOCUS(IDC_EDIT_FONTNAME, OnKillfocusEditFontname)
	ON_BN_CLICKED(IDC_LOCK, OnLock)
	ON_BN_CLICKED(IDC_CHECKIN, OnCheckin)
	ON_WM_HELPINFO()
	ON_EN_KILLFOCUS(IDC_BRIGHTNESS, OnKillfocusBrightness)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CGrFontDialog message handlers


void CGrFontDialog::OnNew() 
{
// TODO: Add your control notification handler code here
//	Bring up Open TGA dialog.
	CString filter = "D3 bitmap files (*.tga; *lbm; *.bbm)|*.tga; *.lbm; *.bbm|Targa files (*.tga)|*.tga|IFF files (*.bbm; *.lbm)|*.bbm; *.lbm||";

	CFileDialog dlg_open(TRUE, 0,0, OFN_FILEMUSTEXIST, (LPCTSTR)filter, this);

	if (dlg_open.DoModal() == IDCANCEL) return;

//	Okay, load in the proper picture for the font
	char filename[255];
	int bm_handle;

	lstrcpy(filename, dlg_open.GetPathName());

	if (m_FontPicBm > -1) bm_FreeBitmap(m_FontPicBm);
	m_FontPicBm = -1;
	
	mprintf((0, "Loading font template: %s...", filename));
	bm_handle = bm_AllocLoadFileBitmap(filename, 0);
	if (bm_handle<0) {
		mprintf((0, "shit!\n"));
		OutrageMessageBox ("Couldn't open the font template file.");
		return;
	}
	mprintf((0, "finished!\n"));

	m_FontPicBm = bm_handle;

//	Get MinAscii and other needed info before extracting font.

	if (!extract_font(&m_FontRecord)) {
		OutrageMessageBox("Unable to generate font from template %s.", filename);
		return;
	}

	UpdateDialog();

//	Display font on screen.
	CWnd::Invalidate();	
}


void CGrFontDialog::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	CWnd *fontwnd;
	RECT rect;
	int w, h;

	if (m_FontPicBm > -1) {
		grHardwareSurface surf(bm_w(m_FontPicBm, 0), bm_h(m_FontPicBm, 0), bm_bpp(m_FontPicBm));

		fontwnd = GetDlgItem(IDC_STATIC_FONTVIEW);
		fontwnd->GetWindowRect(&rect);
		ScreenToClient(&rect);

		w = rect.right-rect.left-4;
		h = rect.bottom-rect.top-4;

		if (w > (surf.width()-4)) w = surf.width() - 4;
		if (h > (surf.height()-4)) h = surf.height() - 4;

	//	load bitmap, convert transparency to pure black and then blt
		surf.load(m_FontPicBm);
		surf.replace_color(TRANSPARENT_COLOR32, 0);
			
		Desktop_surf->attach_to_window((unsigned)m_hWnd);
		Desktop_surf->blt(rect.left+2,rect.top+2,&surf, 0, 0, w, h);	
		Desktop_surf->attach_to_window((unsigned)NULL);
	}
}


int CGrFontDialog::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	return 0;
}


BOOL CGrFontDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
// TODO: Add extra initialization here
	CButton *radio;
	CButton *gray_check = (CButton *)GetDlgItem(IDC_PROP_GRADIANT);
	CEdit *brightness = (CEdit *)GetDlgItem(IDC_BRIGHTNESS);
	char str[8];

	gray_check->EnableWindow(FALSE);

	radio = (CButton *)GetDlgItem(IDC_PROP_MONO);
	radio->SetCheck(0);

	lstrcpy(m_FontName, "Untitled");
	lstrcpy(m_FontFileName, "Untitled");
	
	m_Brightness = 10;

	sprintf(str, "%.1f", m_Brightness/10.0f);
	brightness->SetWindowText(str);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CGrFontDialog::OnSaveAs() 
{
	CString filter = "Outrage font files (*.fnt)|*.fnt||";
	
	CFileDialog dlg(FALSE, 0, 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, (LPCTSTR)filter, this);
	
	if (dlg.DoModal() == IDCANCEL) return;
	
	lstrcpy(m_FontFileName, dlg.GetPathName());
	
	save_font_file(&m_FontRecord);

	UpdateDialog();
}


void CGrFontDialog::OnSave() 
{
	if (strcmp(m_FontFileName, "Untitled") == 0) 
		OnSaveAs();
	else {
		save_font_file(&m_FontRecord);
	}
}


void CGrFontDialog::OnDestroy() 
{
	CDialog::OnDestroy();
	
	if (m_DataBuffer) delete[] m_DataBuffer;
	if (m_FontPicBm > -1) bm_FreeBitmap(m_FontPicBm);
	m_FontPicBm = -1;	
}


void CGrFontDialog::UpdateDialog()
{
	CButton *radio;
	CEdit *edit;
	CStatic *text;
	char buf[64];

//	Setup any necessary changes to control states in the dialog reflecting extraction of font.
	radio = (CButton *)GetDlgItem(IDC_PROP_MONO); radio->SetCheck(0);
	radio = (CButton *)GetDlgItem(IDC_PROP_BITMAPPED); radio->SetCheck(0);
	radio = (CButton *)GetDlgItem(IDC_PROP_GRADIANT); radio->SetCheck(0);

	switch (m_FontType)
	{
		case COLOR_FONT: radio = (CButton *)GetDlgItem(IDC_PROP_BITMAPPED); radio->SetCheck(1); break;
		case MONOCHROME_FONT: radio = (CButton *)GetDlgItem(IDC_PROP_MONO); radio->SetCheck(1); break;
		default: Int3();
	}

//	Set filename static text.  Set 'Untitled' name.  Change static text font info.
	edit = (CEdit *)GetDlgItem(IDC_EDIT_MINASCII);
	_itoa(m_FontRecord.min_ascii, buf, 10);
	edit->SetWindowText(buf);
	text = (CStatic *)GetDlgItem(IDC_STATIC_MAXASCII);
	_itoa(m_FontRecord.max_ascii, buf, 10);
	text->SetWindowText(buf);
	text = (CStatic *)GetDlgItem(IDC_STATIC_FONTNAME);
	text->SetWindowText(m_FontFileName);

	edit = (CEdit *)GetDlgItem(IDC_EDIT_FONTNAME);
	edit->SetWindowText(m_FontName);

	if (m_FontProp) {
		text = (CStatic *)GetDlgItem(IDC_STATIC_PROP);
		text->SetWindowText("Proportional");
	}
	else {
		text = (CStatic *)GetDlgItem(IDC_STATIC_PROP);
		text->SetWindowText("");
	}
}


void CGrFontDialog::OnKillfocusEditMinascii() 
{
	CEdit *edit = (CEdit *)GetDlgItem(IDC_EDIT_MINASCII);
	char buf[4];
	int num_char = m_FontRecord.max_ascii - m_FontRecord.min_ascii + 1;;
											  
	edit->GetWindowText(buf, 4);
	m_FontRecord.min_ascii = atoi(buf);		
	m_FontRecord.max_ascii = m_FontRecord.min_ascii + num_char-1;

	UpdateDialog();
}


void CGrFontDialog::OnKillfocusEditFontname() 
{
	CEdit *edit = (CEdit *)GetDlgItem(IDC_EDIT_FONTNAME);
	edit->GetWindowText(m_FontName,32);
}



//	========================================================================================
//	Font Extraction from Bitmap routines
//	========================================================================================

#define PIX(_x,_y) m_FontBmData[(_x) + (_y) * m_FontBmW]

inline ushort MAKE_565_FROM_555(ushort jcol)
{
	ddgr_color pix32=GR_16_TO_COLOR (jcol);
	int red=GR_COLOR_RED(pix32);
	int green=GR_COLOR_GREEN (pix32);
	int blue=GR_COLOR_BLUE (pix32);

	ushort newpix=((red>>3)<<11)|((green>>2)<<5)|(blue>>3);
	return newpix;
}


BOOL CGrFontDialog::extract_font(gr_font_file_record *ft)
{
	int bmx, bmy;
	int cur_char;

//	grab font template bitmap data from the bitmap library and retrieve characters
//	store relevent information in file record.
	m_FontBmData = bm_data(m_FontPicBm, 0);
	m_FontBmW = bm_w(m_FontPicBm,0); m_FontBmH = bm_h(m_FontPicBm, 0);
	m_CharMaxWidth = m_CharHeight = 0;
	m_FontProp = FALSE;
	m_FontType = MONOCHROME_FONT;
	m_FgColor = NEW_TRANSPARENT_COLOR;

	if (m_DataBuffer) delete[] m_DataBuffer;
	m_DataBuffer = new ushort[1024*MAX_FONT_CHARS];
	m_DataPtr = m_DataBuffer;

//	assume upper left pixel is background color, and first-found other
//	color is box color
	m_BgColor = PIX(0,0);

	for (bmy = 0;bmy  < m_FontBmH; bmy++) 
	{
		for (bmx = 0;bmx < m_FontBmW && PIX(bmx,bmy)==m_BgColor; bmx++) ;
		if (PIX(bmx,bmy)!= m_BgColor) break;
	}				

	m_BoxColor = PIX(bmx,bmy);

//	Get all data from current line.
	mprintf((0, "Parsing font..."));
	mprintf((0," read_characters: "));

	for (cur_char=0;bmy < m_FontBmH && cur_char<MAX_FONT_CHARS;) 
	{
		cur_char = read_font_line(cur_char, bmx, bmy);
		if (cur_char == -1) {
			delete[] m_DataBuffer;
			m_DataBuffer = NULL;
			mprintf((0, "fuck!\n"));
			return FALSE;
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

	CButton *gray_check = (CButton *)GetDlgItem(IDC_PROP_GRADIANT);
	if (ft->flags & FT_COLOR) 
		gray_check->EnableWindow(TRUE);
	else
		gray_check->EnableWindow(FALSE);

	return TRUE;
}


int CGrFontDialog::read_font_line(int cur_char, int& bmx, int& bmy)
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
			break;
		}
		else break;
	}

	return cur_char;
}
 
 
int CGrFontDialog::read_font_char(int cur_char, int& bmx, int& bmy)
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
				OutrageMessageBox("Read probable premature eol at (%d,%d).\n", bmx+1+w, bmy);
				w--;
				return 2;
			}
			else {
				OutrageMessageBox("Suspected error at pixel coordindates %d,%d (char %d)",bmx+1+w,bmy, cur_char);
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
				OutrageMessageBox("Read probable premature end of row at (%d,%d).\n", bmx, bmy+1+h);
				h--;
				return 2;
			}
			else {
				OutrageMessageBox("Suspected error at pixel coordindates %d,%d (char %d)",bmx,bmy+1+h, cur_char);
				return 0;
			}
		}
		else {
			box_edge = true;
		}
	}

	if (w==0 || h==0) {
		OutrageMessageBox("Character %d has width of %d and height of %d",cur_char,w,h);
		return 0;
	}

	m_CharWidths[cur_char] = w;

	if (m_CharMaxWidth !=0 && w != m_CharMaxWidth)
		m_FontProp = TRUE;

	if (w > m_CharMaxWidth) m_CharMaxWidth = w;

	if (m_CharHeight == 0)
		m_CharHeight = h;
	else if (m_CharHeight != h) {
		OutrageMessageBox("Char %d is wrong height (height = %d, correct height = %d)",cur_char,h,m_CharHeight);
		return 0;
	}

	for (y=0;y<h;y++)
		for (x=0;x<w;x++) 
		{
			ushort c;

			if ((c=PIX(bmx+1+x,bmy+1+y)) == m_BgColor)
				c = 0x07e0;									// must go back 565 pure green (old transparent)
			else if (! (m_FgColor & OPAQUE_FLAG))
				m_FgColor = c; 
			else if (m_FgColor != c) { 	// c here is 1555 !! remember this.
				m_FontType = COLOR_FONT;				
			 //	mprintf((0, "%d,%d\n", bmx+1+x, bmy+1+y));
			}

			if (c != 0x07e0) 
				*m_DataPtr++ = MAKE_565_FROM_555(c);  // here 555 pure green will turn into 565 pure green transparent.
			else
				*m_DataPtr++ = 0x07e0;

			ASSERT(m_DataPtr < m_DataBuffer+(1024*256));
		}

	return 1;
}


BOOL CGrFontDialog::save_font_file(gr_font_file_record *ft)
{
	ubyte *tmp_data;
	short *tmp_widths;
	ubyte **tmp_cdata;
	FONTFILE ffile;
	uint id = 0xfeedbaba;
	int num_char;
	bool grayed_font;
	CButton *gray_check = (CButton *)GetDlgItem(IDC_PROP_GRADIANT);

	if (gray_check->GetCheck() == 1) 
		grayed_font = true;
	else
		grayed_font = false;

//	save data pointers.  We don't write these out to the file.
	tmp_data = ft->raw_data;
	tmp_widths = ft->char_widths;
	tmp_cdata = ft->char_data;

//	Adjust font record for file write.
	ft->flags = (m_FontProp?FT_PROPORTIONAL:0) + ((m_FontType==COLOR_FONT)?FT_COLOR:0) + 
		((m_FontType==GRADIENT_FONT)?FT_GRADIENT:0) + (grayed_font?FT_GRADIENT:0);
	ft->raw_data = NULL;
	ft->char_data = NULL;
	ft->char_widths = NULL;
	ft->kern_data= NULL;

//	write to file
	mprintf((0, "Saving font data..."));
   //	sprintf(filename, "%s\\%s", LocalManageGraphicsDir, m_FontFileName);
	ffile = OPEN_FONT(m_FontFileName);
	if (!ffile) {
		OutrageMessageBox("Unable to save font %s.", m_FontFileName);
		return FALSE;
	}
	
//	Write file id.
	WRITE_FONT_INT(ffile, id);

//	Write Header
//	baseline brightness located in upper 8 bits.
	ft->baseline = ft->baseline & 0x00ff;
	ft->baseline = ft->baseline | (m_Brightness << 8);

	WRITE_FONT_SHORT(ffile, ft->width);
	WRITE_FONT_SHORT(ffile, ft->height);
	WRITE_FONT_SHORT(ffile, ft->flags);
	WRITE_FONT_SHORT(ffile, ft->baseline);
	WRITE_FONT_BYTE(ffile, ft->min_ascii);
	WRITE_FONT_BYTE(ffile, ft->max_ascii);
	WRITE_FONT_DATA(ffile, m_FontName, 32, 1);
	
	num_char = ft->max_ascii - ft->min_ascii + 1;

//	Write widths now if necessary.(FT_PROPORTIONAL)
	if (ft->flags & FT_PROPORTIONAL) {
		for (int i = 0; i < num_char; i++)
			WRITE_FONT_SHORT(ffile, tmp_widths[i]);
	}

	if (ft->flags & FT_COLOR) {
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
			for (y = 0; y < ft->height; y++) 
			{
				ubyte mask, datum;

				w = (ft->flags & FT_PROPORTIONAL) ? tmp_widths[i] : ft->width;

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
	mprintf((0, "Done.\n"));

	ft->raw_data = tmp_data;
	ft->char_widths = tmp_widths;
	ft->char_data = tmp_cdata;

	return TRUE;
}


void CGrFontDialog::OnLock() 
{
	// TODO: Add your control notification handler code here
	
}


void CGrFontDialog::OnCheckin() 
{
	// TODO: Add your control notification handler code here
	
}

BOOL CGrFontDialog::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	// TODO: Add your message handler code here and/or call default
	WinHelp(HID_FONTEDITORDLG,HELP_CONTEXT);
	return TRUE;
	//return CDialog::OnHelpInfo(pHelpInfo);
}


void CGrFontDialog::OnKillfocusBrightness() 
{
	CEdit *edit = (CEdit *)GetDlgItem(IDC_BRIGHTNESS);
	char str[8];

	edit->GetWindowText(str, 7);

	m_Brightness = (int)(atof(str) * 10.0);
}
