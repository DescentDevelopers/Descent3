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
 * $Logfile: /DescentIII/Main/editor/GrFontDialog.h $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:57:38 $
 * $Author: kevinb $
 *
 * Font editor
 *
 * $Log: not supported by cvs2svn $
 * 
 * 3     5/08/98 5:23p Samir
 * added font brightness and grayscale caps.
 * 
 * 2     1/12/98 5:22p Samir
 * Fixed up font reading.
 * 
 * 5     6/03/97 5:37p Mark
 * 
 * 4     2/06/97 3:08p Samir
 * Improved font saving for monochrome fonts
 *
 * $NoKeywords: $
 */

// GrFontDialog.h : header file
//
#include "gr.h"

#define MAX_FONT_CHARS	256

/////////////////////////////////////////////////////////////////////////////
// CGrFontDialog dialog

class CGrFontDialog : public CDialog
{
// Construction
public:
	CGrFontDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CGrFontDialog)
	enum { IDD = IDD_FONT_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGrFontDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGrFontDialog)
	afx_msg void OnNew();
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL OnInitDialog();
	afx_msg void OnSaveAs();
	afx_msg void OnSave();
	afx_msg void OnDestroy();
	afx_msg void OnKillfocusEditMinascii();
	afx_msg void OnKillfocusEditFontname();
	afx_msg void OnLock();
	afx_msg void OnCheckin();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnKillfocusBrightness();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void UpdateDialog();

private:
	gr_font_file_record m_FontRecord;
	int m_FontMinAscii, m_FontMaxAscii;	
	int m_FontType;							// taken from Radio buttons
	char m_FontName[32];
	char m_FontFileName[256];

private:
//	these are used when extracting fonts
	BOOL extract_font(gr_font_file_record *ft);
	int read_font_line(int cur_char, int& bmx, int& bmy);
	int read_font_char(int cur_char, int& bmx, int& bmy);
	BOOL save_font_file(gr_font_file_record *ft);

private:
	int m_FontPicBm;						// Font picture bitmap handle
	int m_FontBmW, m_FontBmH;
	int m_CharHeight;						// current character height.
	short m_CharWidths[MAX_FONT_CHARS];
	short m_CharMaxWidth;
	ushort *m_FontBmData;
	ushort *m_DataBuffer, *m_DataPtr;
	ushort m_BgColor, m_BoxColor;
	ushort m_FgColor;
	BOOL m_FontProp;						// Proportional font.
	ubyte m_Brightness;					// brightness * 10
};
