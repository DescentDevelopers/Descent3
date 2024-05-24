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

#include <algorithm>

#ifdef NEWEDITOR
	#include "../neweditor/stdafx.h"
	#include "../neweditor/globals.h"	
#endif

#include "SelManager.h"
#include "stdafx.h"

#include "macros.h"

#ifndef NEWEDITOR
	#include "ddio.h"
#endif

editorSelectorManager SelManager;


editorSelectorManager::editorSelectorManager()
{
	m_l = m_t = m_r = m_b = -1;
	m_OwnerWnd = NULL;
	m_DragX = m_DragY = -1;
	m_DragState = 0;
	m_EndFunc = NULL;
}


//	sets the owner window of selection
//	starts a selection at x,y
void editorSelectorManager::StartSelection(CWnd *wnd, void (*func)(editorSelectorManager *), int x, int y)
{
	ASSERT(wnd != NULL);

	if (m_DragState != 1) {
		POINT pt;
		RECT rect;
		SIZE size = {2,2};

		if (m_DragState == 2)  {
			CClientDC dc(m_OwnerWnd);
			int r_l, r_b, r_t, r_r;
			RECT client_rect;

			m_OwnerWnd->GetClientRect(&client_rect);
			r_l = m_l; r_t = m_t; r_r = m_r; r_b = m_b;
                        if (r_r < r_l) std::swap(r_l, r_r);
                        if (r_b < r_t) std::swap(r_t, r_b);
			if (r_l < client_rect.left) r_l = client_rect.left;
			if (r_t < client_rect.top) r_t = client_rect.top;
			if (r_r > client_rect.right) r_r = client_rect.right;
			if (r_b > client_rect.bottom) r_b = client_rect.bottom;
			SetRect(&rect, r_l, r_t, r_r, r_b);
			dc.DrawDragRect(&rect, size, NULL, size, NULL, NULL);
		}

		CClientDC dc(wnd);
		m_OwnerWnd = wnd;

		m_DragState = 1;
		m_EndFunc = func;

		m_l = x; m_t = y;
		m_r = x; m_b = y;
		SetRect(&rect, m_l, m_t, m_r, m_b);
		dc.DrawDragRect(&rect, size, NULL, size, NULL, NULL);

		GetCursorPos(&pt);
		m_DragX = pt.x;
		m_DragY = pt.y;
	}
}


//	returns the selected rectangle.
void editorSelectorManager::GetSelectedRect(int *l, int *t, int *r, int *b)
{
	RECT client_rect;

//	make sure selected rectangle is clipped to the owner window and normalized.
	m_OwnerWnd->GetClientRect(&client_rect);

        if (m_r < m_l) std::swap(m_l, m_r);
        if (m_b < m_t) std::swap(m_t, m_b);
	if (m_l < client_rect.left) m_l = client_rect.left;
	if (m_t < client_rect.top) m_t = client_rect.top;
	if (m_r > client_rect.right) m_r = client_rect.right;
	if (m_b > client_rect.bottom) m_b = client_rect.bottom;

	*l = m_l;
	*t = m_t;
	*r = m_r;
	*b = m_b;
}


//	defers control to the selector manager
void editorSelectorManager::Defer()
{
	int mx, my, mdx, mdy;
#ifndef NEWEDITOR
	int mouse_state;
#endif
	POINT pt;

	if (m_DragState != 1) return;
	
//	get mouse deltas and button state
#ifdef NEWEDITOR
	mx = mouse.x;
	my = mouse.y;
	mdx = mouse.x-mouse.oldx;
	mdy = mouse.y-mouse.oldy;
#else
	mouse_state = ddio_MouseGetState(&mx, &my, &mdx, &mdy);
#endif
	GetCursorPos(&pt);
	mx = pt.x; my = pt.y;

	mdx = mx - m_DragX;
	mdy = my - m_DragY;

#ifdef NEWEDITOR
	if (mouse.left && m_DragState == 1) {
#else
	if ((mouse_state & MOUSE_LB) && m_DragState == 1) {
#endif
	// if dragging mouse, then draw drag rectangle.  if dragging outside owner window
	//	then still do sizing of window within bounds of owner (why we use deltas for mouse)
		CClientDC dc(m_OwnerWnd);
		RECT old_draw_rect, draw_rect, client_rect;
		SIZE size = {2,2};
		int r_l, r_b, r_t, r_r;

		m_OwnerWnd->GetClientRect(&client_rect);

	//	get current selected rect, normalize rectangle if left,top is invalid for drawing.
		r_l = m_l; r_t = m_t; r_r = m_r; r_b = m_b;
                if (r_r < r_l) std::swap(r_l, r_r);
                if (r_b < r_t) std::swap(r_t, r_b);
		if (r_l < client_rect.left) r_l = client_rect.left;
		if (r_t < client_rect.top) r_t = client_rect.top;
		if (r_r > client_rect.right) r_r = client_rect.right;
		if (r_b > client_rect.bottom) r_b = client_rect.bottom;
		SetRect(&old_draw_rect, r_l, r_t, r_r, r_b);

		m_r = m_r + mdx;
		m_b = m_b + mdy;

		r_l = m_l; r_t = m_t; r_r = m_r; r_b = m_b;
                if (r_r < r_l) std::swap(r_l, r_r);
                if (r_b < r_t) std::swap(r_t, r_b);
		if (r_l < client_rect.left) r_l = client_rect.left;
		if (r_t < client_rect.top) r_t = client_rect.top;
		if (r_r > client_rect.right) r_r = client_rect.right;
		if (r_b > client_rect.bottom) r_b = client_rect.bottom;
		SetRect(&draw_rect, r_l, r_t, r_r, r_b);

		dc.DrawDragRect(&draw_rect, size, &old_draw_rect, size, NULL, NULL);
	}
#ifdef NEWEDITOR
	else if (mouse.left && m_DragState == 1) {
#else
	else if (!(mouse_state & MOUSE_LB) && m_DragState == 1) {
#endif
	// if mouse up then call our ending function
		m_DragState = 2;
		if (m_EndFunc) (*m_EndFunc)(this);
	}

	m_DragX = mx;
	m_DragY = my;
}


void editorSelectorManager::EndSelection()
{
	RECT rect;
	SIZE size = {2,2};

	CClientDC dc(m_OwnerWnd);
	int r_l, r_b, r_t, r_r;
	RECT client_rect;

// clip drawing rectangle to owner window.
	m_OwnerWnd->GetClientRect(&client_rect);
	r_l = m_l; r_t = m_t; r_r = m_r; r_b = m_b;
        if (r_r < r_l) std::swap(r_l, r_r);
        if (r_b < r_t) std::swap(r_t, r_b);
	if (r_l < client_rect.left) r_l = client_rect.left;
	if (r_t < client_rect.top) r_t = client_rect.top;
	if (r_r > client_rect.right) r_r = client_rect.right;
	if (r_b > client_rect.bottom) r_b = client_rect.bottom;
	SetRect(&rect, r_l, r_t, r_r, r_b);
	dc.DrawDragRect(&rect, size, NULL, size, NULL, NULL);
	m_l = -1; m_t = -1; m_r = -1; m_b = -1;
	m_DragState = 0;
}
