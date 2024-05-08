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

 * $Logfile: /DescentIII/Main/D3Launch/BitmapButtonEx.h $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:56:51 $
 * $Author: kevinb $
 *
 * 
 *
 * $Log: not supported by cvs2svn $
 * 
 * 13    11/05/98 2:16p Nate
 * Added keyboard support
 * 
 * 12    10/30/98 12:09p Nate
 * Fixed clicking on light problem.
 * 
 * 11    10/02/98 7:08p Nate
 * Added animation button support.
 * 
 * 10    9/22/98 11:33a Nate
 * Added support for logo button text.
 * 
 * 9     9/18/98 5:19p Nate
 * Made the bitmap button font look a *little* better
 * 
 * 8     9/10/98 5:17p Nate
 * Added text messages which can be drawn on top of the button.
 * 
 * 7     9/03/98 6:57p Nate
 * Fixed StretchBlt() problem by doing some 256 color conversions
 * 
 * 6     9/01/98 7:15p Nate
 * Major Revision #2
 * 
 * 5     8/31/98 6:44p Nate
 * Major Revision
 * 
 * 4     8/12/98 9:21a Nate
 * Fixed "gray-button-background" flicker
 * 
 * 3     8/10/98 10:44a Nate
 * Added Language selection support
 * 
 * 2     8/05/98 11:54a Nate
 * Initial Version
 *
 * $NoKeywords: $
 */

#if !defined(AFX_BITMAPBUTTONEX_H__04FA7822_2AB0_11D2_8CBD_00A0C96ED60D__INCLUDED_)
#define AFX_BITMAPBUTTONEX_H__04FA7822_2AB0_11D2_8CBD_00A0C96ED60D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// BitmapButtonEx.h : header file
//

// Button orientation values (only used for option button text alignment)
#define LEFT_ORIENTED_BTN	0
#define RIGHT_ORIENTED_BTN	1
#define TOP_ORIENTED_BTN	2


/////////////////////////////////////////////////////////////////////////////
// CBitmapButtonEx window

class CBitmapButtonEx : public CBitmapButton
{
// Construction
public:
	CBitmapButtonEx();

// Attributes
public:
	// Button state bitmaps
	CBitmap m_UpBitmap;
	CBitmap m_DownBitmap;
	CBitmap m_DisabledBitmap;
	CBitmap m_MouseBitmap;

	// Bitmaps for an animated button (doesn't behave as a button however)
	CBitmap *m_AnimBitmaps;
	int m_NumAnimFrames;
	int m_CurrAnimFrame;

private:
	CString m_TextMessage;	// The text to be displayed over the button
	RECT m_TextClipBox;		// The clipping rectangle for the text
	POINT m_TextStartPoint;	// The starting point offset for the text

	bool m_MouseOverBtn;	// Is the mouse currently over the button?
	bool m_Hidden;			// Is the button hidden?
	int m_Orientation;		// Is the button a left or right option button

	CFont m_btn_font;		// Font to be used by the button text
	CFont m_logobtn_font;	// Font to be used by the logo buttons

	bool m_IgnoreDisabled;	// Allows buttons to be disabled functionality-wise,
							// but not bitmap-wise

// Operations
public:
	// Sets up and loads the hi-color bitmaps and palettes
	bool Setup(UINT nID, CWnd *parent, int orient=LEFT_ORIENTED_BTN);
	bool AnimSetup(UINT nID, CWnd *parent, int num_frames);

	bool Load(UINT nID, CWnd *parent);		// Loads the hi-color bitmaps and palettes
	bool AnimLoad(UINT nID, CWnd *parent);	// Loads bitmaps for animation
	void Light(bool bLight);				// Lights/un-Lights the button
	void Hide(bool bHide);					// Hides/un-Hides a button
	void SetText(UINT nID);					// Sets the button's text message
	void DoNextAnimFrame(void);				// Points button to next animation frame

	bool IsLit(void)	{ return(m_MouseOverBtn); }		// checks if button is lit
	bool IsHidden(void)	{ return(m_Hidden); }			// checks if button is hidden

	void IgnoreDisabled(bool value) { m_IgnoreDisabled=value; }  // sets the ignore disabled flag

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBitmapButtonEx)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CBitmapButtonEx();

	// Generated message map functions
protected:
	//{{AFX_MSG(CBitmapButtonEx)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BITMAPBUTTONEX_H__04FA7822_2AB0_11D2_8CBD_00A0C96ED60D__INCLUDED_)
