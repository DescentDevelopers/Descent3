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
 * $Logfile: /DescentIII/Main/editor/GrListBox.h $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:57:38 $
 * $Author: kevinb $
 *
 * New graphical listbox
 *
 * $Log: not supported by cvs2svn $
 * 
 * 4     9/04/97 4:39p Matt
 * Added includes needed as a result of removing includes from d3edit.h
 * 
 * 3     8/26/97 10:49a Samir
 * Fixed some small bugs with the scroll bar.
 * 
 * $NoKeywords: $
 */

#if !defined(AFX_GRLISTBOX_H__12166D61_1D56_11D1_BDA4_00A0C96ED595__INCLUDED_)
#define AFX_GRLISTBOX_H__12166D61_1D56_11D1_BDA4_00A0C96ED595__INCLUDED_

#include "gr.h"

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// GrListBox.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGrListBox window

class CGrListBox : public CWnd
{
// Construction
public:
	CGrListBox();

//	if selbox is true, then we use a rectangle to hilite the selection.
	BOOL Create(CWnd *parent_wnd, int id, const RECT& rect, int surf_dim, bool selbox = false);

//	Internal management functions
private:
// draws all the items in the list (or not). invoking DrawItem and uses the list management functions
//	also performs check of mouse selection (the reason we give an option NOT to draw it.)
	void ListBoxProc(bool do_draw);						

//	these functions move the list pointer (m_ListItem) up or down a few rows.
//	optional to refresh display, and returns true if m_ItemSel is in row.
	bool RowUp(int rows, bool draw=true);
	bool RowDown(int rows, bool draw=true);
	int CalcRowOfItem(int item);

private: 
	int m_ItemSel;								// selected item number.
	int m_HitX, m_HitY;						// used to select an item in box via mouse down.
	bool m_CheckHit;							// check for a hit.
	int m_ScrollPos;							// used to check scroll bar
	bool m_SelectionBox;						// do we use the selection box, or scale selection aid.
	bool m_IsItemSelInBox;					// is item selected being displayed?

// Attributes
public:
	int GetSelectedItem() const { return m_ItemSel; };
	void SelectItem(int item);
	void SetItemFirst(int item);			// these are accessor functions to the variable m_ListItem.
	int GetItemFirst() const;				// this value is the first item to be drawn in the list box currently.

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGrListBox)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CGrListBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CGrListBox)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

//	List management overridables
protected:
//	enumerates items maximum in the list box.  this is here to insure that any changes in
//	the user-managed list.
	virtual int ListEnumerate() = 0;

// Specify the first item to be drawn in the box by DrawItem. returns the item #
//	return -1 if there is no first item (list is empty, maybe)
	virtual int ListFirstItem(int firstitem) = 0;			

// Specify the next/prev item to be drawn in the box by DrawItem. 
//	Returns the item # following or preceding the passed the item #
	virtual int ListNextItem(int curitem) = 0;
	virtual int ListPrevItem(int curitem) = 0;

// Current first item in box displayed.  We are allowed to modify this.
	int m_ListItem;							

private:
	int m_ListLen;								// number of list items in list box total
	int items_per_row, items_max_row;	// dimensions of listbox in listbox items.

//	Drawing overridables
protected:
	virtual void DrawItem(int x, int y, grHardwareSurface *itemsurf, int item) = 0;

private:
	int m_SurfDimension;						// Square dimensions of visible item.

//	miscellaneous overridables
protected:
//	if item was selected from the list box, this function is invoked.
	virtual void OnItemSelected(int item) = 0;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GRLISTBOX_H__12166D61_1D56_11D1_BDA4_00A0C96ED595__INCLUDED_)
