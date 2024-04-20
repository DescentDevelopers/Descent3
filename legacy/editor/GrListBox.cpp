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
 * $Logfile: /DescentIII/Main/editor/GrListBox.cpp $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:57:38 $
 * $Author: kevinb $
 *
 * New graphical listbox
 *
 * $Log: not supported by cvs2svn $
 * 
 * 8     6/10/98 1:57p Samir
 * fixed some listbox problems.
 * 
 * 7     6/10/98 12:18p Matt
 * Added hack to fix bug until Samir can fix it for real.
 * 
 * 6     2/10/98 3:16p Craig
 * Samir
 * 
 * 5     9/30/97 5:35p Samir
 * Fixed hangup bug when masking out textures.
 * 
 * 4     8/26/97 11:39a Samir
 * Check editor_active to see whether editor graphics are active or not.
 * 
 * 3     8/26/97 10:49a Samir
 * Fixed some small bugs with the scroll bar.
 * 
 * $NoKeywords: $
 */

// GrListBox.cpp : implementation file
//

#include "stdafx.h"
#include "editor.h"
#include "GrListBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGrListBox

CGrListBox::CGrListBox()
{
	items_max_row = 0;
	items_per_row = 0;
	m_ListLen = 0;
	m_ListItem = 0;
	m_SurfDimension = 0;
	m_ItemSel = -1;							// -1 because item number can be 0 and a valid item.
	m_HitX = 0;
	m_HitY = 0;
	m_CheckHit = false;
	m_IsItemSelInBox = false;
}

CGrListBox::~CGrListBox()
{
}


BOOL CGrListBox::Create(CWnd *parent_wnd, int id, const RECT& rect, int surf_dim, bool selbox)
{
	m_SurfDimension = surf_dim;
	m_SelectionBox = selbox;

	return CWnd::Create(NULL, "GrListBox", WS_CHILD | WS_VISIBLE | WS_VSCROLL, rect, parent_wnd, id);
}


void CGrListBox::SelectItem(int item)
{
//	don't do a damn thing if the item selected equals the requested selected item
	if (item != m_ItemSel) {
		m_ItemSel = item;
		OnItemSelected(item);
		ListBoxProc(false);					// update list box data but don't draw yet.

	// scroll window until item wanted is in view.
		if (m_IsItemSelInBox == false) {
			//SAMIR: fix this!!
			int count=0;

			while (m_ListItem < m_ItemSel && (++count < 100))
				if (RowDown(1,false)) break;

			while (m_ListItem > m_ItemSel && (++count < 100))
				if (RowUp(1,false)) break;
		}

		CWnd::Invalidate();
	}
}


void CGrListBox::SetItemFirst(int item)
{
	m_ListItem = item;
}


int CGrListBox::GetItemFirst() const
{
	return m_ListItem;
}


BEGIN_MESSAGE_MAP(CGrListBox, CWnd)
	//{{AFX_MSG_MAP(CGrListBox)
	ON_WM_LBUTTONDOWN()
	ON_WM_PAINT()
	ON_WM_VSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CGrListBox message handlers

void CGrListBox::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	ListBoxProc(true);
}


void CGrListBox::OnLButtonDown(UINT nFlags, CPoint point) 
{
	int old_sel;

	m_CheckHit = true;
	m_HitX = point.x;
	m_HitY = point.y;

//	should we have to redraw box if we haven't changed a selection?
	old_sel = m_ItemSel;
	ListBoxProc(false);
	if (old_sel != m_ItemSel) 
		CWnd::Invalidate(FALSE);
	
	CWnd::OnLButtonDown(nFlags, point);
}


void CGrListBox::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
  
		SCROLLINFO si;
		int cur_pos, new_pos;

		GetScrollInfo(SB_VERT, &si, SIF_ALL);
		cur_pos = m_ScrollPos;

		switch (nSBCode)
		{
			case SB_LINEDOWN: new_pos = cur_pos + 1; break;
			case SB_LINEUP:	new_pos = cur_pos - 1;	break;
			case SB_THUMBPOSITION:	new_pos = nPos;  break;
			case SB_THUMBTRACK:	new_pos = nPos; break;
			case SB_PAGEDOWN: new_pos = cur_pos + si.nPage; break;
			case SB_PAGEUP: new_pos = cur_pos - si.nPage; break;
			default: new_pos = cur_pos;
		}

		if (new_pos < 0) new_pos = 0;
		if (new_pos > (int)(si.nMax - si.nPage+1)) new_pos = (int)(si.nMax - si.nPage+1);
 //		if (new_pos == 0) RowUp((cur_pos-new_pos)+1);	// list up until start of list. prevents missing items
		if (new_pos != cur_pos) {
		//	ok, new_pos will be an index into the listbox, NOT the texture list.
		//	so we will get the number of texture rows we went up or down and change 
		//	tex_start accordingly.
			SCROLLINFO si;

		//	mprintf((0, "scroll new_pos = %d.  original_pos = %d\n", new_pos, cur_pos));
			if (new_pos < cur_pos) RowUp(cur_pos-new_pos);
			if (new_pos > cur_pos) RowDown(new_pos-cur_pos);

			cur_pos = new_pos;
			
			si.cbSize = sizeof(si); 
			si.fMask  = SIF_POS; 
			si.nPos   = m_ScrollPos = cur_pos; 
			CWnd::SetScrollInfo(SB_VERT,&si); 

		}
	
	CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}


/////////////////////////////////////////////////////////////////////////////
//	internal functions


//	these functions move the list pointer (m_ListItem) up or down a few rows.
bool CGrListBox::RowUp(int rows, bool draw)
{
	int old=m_ListItem;
	int done=0,count=0;
	bool retval=false;

	while ((count<(items_per_row*rows)) && !done)
	{
		int olditem;

		if (m_ListItem == m_ItemSel) retval = true;

		olditem = m_ListItem;
		m_ListItem = ListPrevItem(m_ListItem);
		//mprintf((0, "m_ListItem = %d\n", m_ListItem));
		if (m_ListItem > olditem) {
			m_ListItem = olditem;
			done = 1;
		//	mprintf((0, "m_ListItem = %d\n", m_ListItem));
		}
		else if (old<=m_ListItem)	{
			m_ListItem=old;
			done=1;
		}
		else
			count++;
	}
	
	if (draw) CWnd::Invalidate();		

	return retval;
}


bool CGrListBox::RowDown(int rows, bool draw)
{
	int old=m_ListItem;
	int done=0,count=0;
	bool retval = false;

	while ((count<(items_per_row*rows)) && !done)
	{
		if (m_ListItem == m_ItemSel) retval = true;

		m_ListItem = ListNextItem(m_ListItem);

		if (old>=m_ListItem)
		{
			m_ListItem=old;
			done=1;
		}
		else
			count++;
	}
		
	if (draw) CWnd::Invalidate();

	return retval;
}


int CGrListBox::CalcRowOfItem(int item)
{
	int curitem = 0, oldcuritem=0;
	int column = 0;
	int row = 0;

  	do 
	{
		curitem = ListNextItem(curitem);
		column++;
		if (column == items_per_row) { row++; column = 0; }
		if (oldcuritem >= curitem) return 0;
		oldcuritem = curitem;
	}
	while (curitem != item);

	return row;
}


// draws all the items in the list, or don't draw. invoking DrawItem and uses the list management functions
void CGrListBox::ListBoxProc(bool do_draw)
{
	static bool in_listboxproc = false;
	grHardwareSurface m_ItemSurf;
	grHardwareSurface m_ItemSurfSel;
	RECT box_rect;
	int left, top, draw_x, draw_y;
	int box_width, box_height;
	int num_items, item;

//	don't draw if editor is in background.
	if (in_listboxproc) return;
	in_listboxproc = true;
	if (!Editor_active) do_draw = false;

//	initialize graphic objects
	CWnd::GetClientRect(&box_rect);
	box_height = box_rect.bottom - box_rect.top;
	box_width = box_rect.right - box_rect.left;

	if (do_draw) {
		Desktop_surf->attach_to_window((unsigned)CWnd::m_hWnd);
		Desktop_surf->clear(0,0,box_width,box_height);
		m_ItemSurf.create(m_SurfDimension,m_SurfDimension,BPP_16);
		m_ItemSurfSel.create(m_SurfDimension+8,m_SurfDimension+8,BPP_16);
	}

//	initialize list manager.
	items_per_row = box_width/(m_SurfDimension+8);
	items_max_row = box_height/(m_SurfDimension+8);
	m_ListLen = this->ListEnumerate();
	m_ListItem = (m_ListItem/items_per_row)*items_per_row; // align rows correctly
	m_ListItem = this->ListFirstItem(m_ListItem);

//	start drawing items in list box.
	left = (box_width/2) - (items_per_row*(m_SurfDimension+8))/2;
	top = (box_height/2) - (items_max_row*(m_SurfDimension+8))/2;
	num_items = 0;
	item = m_ListItem;
	m_IsItemSelInBox = false;					// this will turn true during the drawing loop

//	drawing and item selection check all purpose loop
//	if our start list function returned a -1, this means that there are no list items to draw!
//	also stop if we've exceeded the number of items that can exist in the limited box of visible
//	space.
	while ((m_ListItem > -1) && (num_items < (items_per_row * items_max_row)))
	{
		int previtem;

	// draw at draw_x, draw_y.  go to next item in list and check if we are at end of list.
		draw_x = left + ((num_items % items_per_row) * (m_SurfDimension+8));
		draw_y = top + ((num_items / items_per_row) * (m_SurfDimension+8));

		previtem = item;
		
		if (m_CheckHit) 
			if ( m_HitX>=draw_x && m_HitX<=(draw_x+m_SurfDimension+8) 
				&& m_HitY>=draw_y && m_HitY<=(draw_y+m_SurfDimension+8)) {
				m_ItemSel = item;
				OnItemSelected(m_ItemSel);
				m_CheckHit = false;
			}

		if (m_ItemSel == item) m_IsItemSelInBox = true;

		if (do_draw) {
			if (m_ItemSel == item) {
				if (m_SelectionBox) {
					grViewport vp(&m_ItemSurfSel);
					vp.clear();
					vp.lock();
					vp.rect(GR_RGB(255,0,255), 0,0,vp.width()-1,vp.height()-1);
					vp.rect(GR_RGB(255,0,255), 1,1,vp.width()-2,vp.height()-2);
					vp.rect(GR_RGB(255,0,255), 2,2,vp.width()-3,vp.height()-3);
					vp.unlock();
					Desktop_surf->blt(draw_x-4,draw_y-4,vp.get_parent());
					this->DrawItem(draw_x, draw_y, &m_ItemSurf, item);
				}
				else {
					this->DrawItem(draw_x-4, draw_y-4, &m_ItemSurfSel, item);
				}
			}
			else
				this->DrawItem(draw_x, draw_y, &m_ItemSurf, item);
		}

		item = this->ListNextItem(item);

	//	break if we hit end of list.
		if (item <= previtem) break;

		num_items++;
	}

//	update the scroll bar
//	free up any graphic resources initialized here.
	if (do_draw) {
		SCROLLINFO si;

		si.cbSize = sizeof(si);
		si.fMask = SIF_ALL;
		si.nMin = 0;
		si.nMax = (m_ListLen / items_per_row) + ((m_ListLen % items_per_row) ? 0: -1);
		si.nPos = m_ScrollPos = CalcRowOfItem(m_ListItem);
		si.nPage = (si.nMax >= items_max_row) ? items_max_row : (si.nMax+1);
		CWnd::SetScrollInfo(SB_VERT, &si);
		CWnd::ShowScrollBar(SB_VERT);

		m_ItemSurfSel.free();
		m_ItemSurf.free();

		Desktop_surf->attach_to_window(NULL);
	}

	in_listboxproc = false;
}



