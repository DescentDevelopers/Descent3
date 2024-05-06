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

 * $Logfile: /DescentIII/Main/editor/PictListBox.cpp $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:57:38 $
 * $Author: kevinb $
 *
 * Picture list box
 *
 * $Log: not supported by cvs2svn $
 * 
 * 2     8/12/97 10:48p Matt
 * Force redraw when SelectItem() is called, though this causes a double
 * redraw when a picture of an item is clicked on.
 * 
 * 8     5/28/97 6:16p Samir
 * When NextListItem returns the same itemnum as before, we are done also.
 * 
 * 7     4/25/97 11:52a Samir
 * Took out annoying mprintfs
 * 
 * 6     4/01/97 3:26p Samir
 * Now when scrolling up, always refresh if at zero the get any
 * undisplayed objects.
 * 
 * 5     3/25/97 6:30p Samir
 * Fixed problem when no items in listbox
 * 
 * 4     3/21/97 11:21a Samir
 * Fixed scroll up problems when adding new poweups.
 * 
 * 3     2/28/97 3:05p Samir
 * Moved StartIterateItems to before checking size of list.
 * 
 * 2     2/21/97 6:24p Samir
 * Fixed scrolling probs.
 *
 * $NoKeywords: $
 */

#include "stdafx.h"
#include "PictListBox.h"

#include "d3edit.h"
#include "pserror.h"
#include "gr.h"



//	Construction and destruction

editorPictListBox::editorPictListBox()
{
	m_ItemStart = 0;
	m_ListPos = 0;
	m_ItemsPerRow = 0;
	m_ItemsMaxRow = 0;
	m_ItemsMax = 0;
	m_PageLen = 0;
	m_Redraw = 0;

	m_CheckForSelect = 0;
	m_CheckX = m_CheckY = 0;

	m_ItemSelected = -1;
	m_OldItemCount = -1;
	m_ParentWnd = NULL;
}


//	Initialization

void editorPictListBox::Create(CWnd *parent, int static_id, int scroll_id, int item_dim, int init_item)
{
	m_SurfDim = item_dim;
	m_ParentWnd = parent;

	m_StaticID = static_id;
	m_ScrollbarID = scroll_id;
	m_ItemSelected = init_item;
}
	

//	Operations

void editorPictListBox::Invalidate()
{
	if (!m_ParentWnd) return;
	m_Redraw = 1;						// force a complete redraw of the listbox
	Update();
}


//	Listbox handlers
// updates the display of the listbox if needed.

void editorPictListBox::SelectItem(int item) 
{ 
	if (!m_ParentWnd) return;
	m_ItemSelected = item; 
	OnItemSelected(item); 

	//Need to redraw to show new selected item
	Invalidate();
}


void editorPictListBox::Update()
{
	RECT rect;
	int do_update;

	if (!m_ParentWnd) return;

//	create necessary graphics objects
	Desktop_surf->attach_to_window((unsigned)m_ParentWnd->GetDlgItem(m_StaticID)->m_hWnd);
	m_ParentWnd->GetDlgItem(m_StaticID)->GetClientRect(&m_ListRect);
	::CopyRect(&rect, &m_ListRect);

//	determine if we need to update the list box.
	StartIterateListItems();

	if (m_OldItemCount==m_ItemsMax && !m_Redraw) {
		do_update = 0;
	}
	else {
		do_update = 1;
		m_Redraw = 0;
		m_OldItemCount = m_ItemsMax;
	}

//	draw listbox
	if (do_update) {
		int itemnum=m_ItemStart, count=0, q,r;
		int left,top;
		bool done=0;

		Desktop_surf->clear(rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top);
		m_ItemSurf.create(m_SurfDim,m_SurfDim,BPP_16);
		m_ItemSurfSel.create(m_SurfDim+8,m_SurfDim+8,BPP_16);

	// determine number of contents that can fit in the listbox.
		q = rect.right-rect.left;
		r = rect.bottom-rect.top;
		m_ItemsPerRow = q/(m_SurfDim+8);
		m_ItemsMaxRow = r/(m_SurfDim+8);
		ASSERT(m_ItemsPerRow && m_ItemsMaxRow);	// No no no no...

		left = (q/2) - ((m_ItemsPerRow*(m_SurfDim+8))/2);
		top = (r/2) - ((m_ItemsMaxRow*(m_SurfDim+8))/2);

		if (m_ItemsMax == 0) done = 1;			// if no items, then we just wont draw.
		
	//	okay draw each item.
		while (!done)
		{
			int x, y;
			int olditemnum;
			
			x = left+rect.left + ((count%m_ItemsPerRow)*(m_SurfDim+8));
			y = top+rect.top + ((count/m_ItemsPerRow)*(m_SurfDim+8));

			if (m_CheckForSelect) {
			//	note that if we've detected a switch in textures, we need to redraw the box
			//	yet again to prevent multiple selected textures.  This should be okay, since
			//	m_CheckForTextureList is cleared.
				if (m_CheckX>=x && m_CheckX<=(x+48) && m_CheckY>=y && m_CheckY<=(y+48))  {
					m_CheckForSelect=0;
					SelectItem(itemnum);
					m_Redraw = 1;
					Update();
					return;
				}
			}

			olditemnum = itemnum;
			DrawItem(x,y,itemnum);
			itemnum = NextListItem(itemnum);

		//	repeated itemnum, means there is no more.
			if (itemnum == olditemnum) done = 1;
			else count++;

			if (count==m_ItemsPerRow*m_ItemsMaxRow) done = 1;
			if (itemnum<=m_ItemStart) done = 1;
		}

		m_ItemSurfSel.free();
		m_ItemSurf.free();

	//	must define the vertical scroll bar dimensions
		CScrollBar *bar = (CScrollBar *)m_ParentWnd->GetDlgItem(m_ScrollbarID);
		SCROLLINFO si;

		m_PageLen = m_ItemsMaxRow; //(count/m_ItemsPerRow)+ ((count%m_ItemsPerRow) ? 1 : 0);
		m_ListPos = bar->GetScrollPos();
		si.cbSize = sizeof(si);
		si.fMask = SIF_ALL;
		si.nMin = 0;
		si.nMax = (m_ItemsMax / m_ItemsPerRow) + ((m_ItemsMax % m_ItemsPerRow) ? 1: 0)-1;
		si.nPos = m_ListPos;
		si.nPage = m_PageLen;
		bar->SetScrollInfo(&si);
		bar->ShowScrollBar(TRUE);
	}

//	restore graphics state
	Desktop_surf->attach_to_window((unsigned)NULL);
}


//	Handlers called by external source

void editorPictListBox::OnVScroll(UINT nSBCode, int nPos, CScrollBar* pScrollBar) 
{
	int id = -1, min, max;
	int new_pos = 0;

	if (!m_ParentWnd) return;

	ASSERT(pScrollBar == (CScrollBar *)m_ParentWnd->GetDlgItem(m_ScrollbarID));
	
	if (pScrollBar) {
		id = pScrollBar->GetDlgCtrlID(); 
		pScrollBar->GetScrollRange(&min, &max);

		switch (nSBCode)
		{
			case SB_LINEDOWN: new_pos = m_ListPos + 1; break;
			case SB_LINEUP:	new_pos = m_ListPos - 1;	break;
			case SB_THUMBPOSITION:	new_pos = nPos;  break;
			case SB_THUMBTRACK:	new_pos = nPos; break;
			case SB_PAGEDOWN: new_pos = m_ListPos + m_PageLen; break;
			case SB_PAGEUP: new_pos = m_ListPos - m_PageLen; break;
			default: new_pos = m_ListPos;
		}

		if (new_pos < 0) new_pos = 0;
		if (new_pos > (max - m_PageLen)) new_pos = (max-m_PageLen)+1;
		if (new_pos == 0) ListUp((m_ListPos-new_pos)+1);	// list up until start of list. prevents missing items
		if (new_pos != m_ListPos) {
		//	ok, new_pos will be an index into the listbox, NOT the texture list.
		//	so we will get the number of texture rows we went up or down and change 
		//	tex_start accordingly.
			SCROLLINFO si;

		//	mprintf((0, "scroll new_pos = %d.  original_pos = %d\n", new_pos, m_ListPos));
			if (new_pos < m_ListPos) ListUp(m_ListPos-new_pos);
			if (new_pos > m_ListPos) ListDown(new_pos-m_ListPos);

			m_ListPos = new_pos;
			
			si.cbSize = sizeof(si); 
			si.fMask  = SIF_POS; 
			si.nPos   = m_ListPos; 
			pScrollBar->SetScrollInfo(&si); 

		}
	}
}


void editorPictListBox::OnLButtonDown(CPoint point) 
{
	if (!m_ParentWnd) return;

	m_CheckForSelect=1;
	m_CheckX=point.x;
	m_CheckY=point.y;

	Invalidate();
}


void editorPictListBox::ListUp(int rows) 
{
	int old=m_ItemStart;
	int done=0,count=0;

	while ((count<(m_ItemsPerRow*rows)) && !done)
	{
		int olditem;

		olditem = m_ItemStart;
		m_ItemStart = PrevListItem(m_ItemStart);
		//mprintf((0, "m_ItemStart = %d\n", m_ItemStart));
		if (m_ItemStart > olditem) {
			m_ItemStart = olditem;
			done = 1;
		//	mprintf((0, "m_ItemStart = %d\n", m_ItemStart));
		}
		else if (old<=m_ItemStart)	{
			m_ItemStart=old;
			done=1;
		}
		else
			count++;
	}
	
	Invalidate();		
}


void editorPictListBox::ListDown(int rows) 
{
	int old=m_ItemStart;
	int done=0,count=0;

	while ((count<(m_ItemsPerRow*rows)) && !done)
	{
		m_ItemStart = NextListItem(m_ItemStart);

		if (old>=m_ItemStart)
		{
			m_ItemStart=old;
			done=1;
		}
		else
			count++;
	}
		
	Invalidate();
}
