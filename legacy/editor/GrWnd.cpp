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
 * $Logfile: /DescentIII/Main/editor/GrWnd.cpp $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:57:38 $
 * $Author: kevinb $
 *
 * Base class of all GrWnds.  Does screen and viewport allocation
 *
 * $Log: not supported by cvs2svn $
 * 
 * 2     8/19/97 10:46a Samir
 * Fixed unusual problem when for some reason during  GrWnd creation, the
 * height or width is 0, causing an error.
 * 
 * 10    1/30/97 6:43p Samir
 * stupid forgetting an argument
 * 
 * 9     1/30/97 6:23p Samir
 * Reflect changes in grScreen, where there is no viewing rect, so we blt
 * to the entire window
 * 
 * 8     1/21/97 12:47p Samir
 * Added OnSize and GRWND_STYLE constant to adjust window look. 
 *
 * $NoKeywords: $
 */


#include "stdafx.h"
#include "editor.h"
#include "GrWnd.h"

#include <assert.h>

#include "mono.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGrWnd

CGrWnd::CGrWnd(const char *name)
{
	m_grScreen = NULL;
	m_grViewport = NULL;
	if (name) strncpy(m_Name, name, 15);
	m_Name[31] = 0;

	m_Active = FALSE;
}

CGrWnd::~CGrWnd()
{
}


BEGIN_MESSAGE_MAP(CGrWnd, CWnd)
	//{{AFX_MSG_MAP(CGrWnd)
	ON_WM_DESTROY()
	ON_WM_CREATE()
	ON_WM_MOVE()
	ON_WM_SIZE()
	ON_WM_NCACTIVATE()
	ON_WM_NCPAINT()
	ON_WM_CHILDACTIVATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CGrWnd message handlers

BOOL CGrWnd::PreCreateWindow(CREATESTRUCT& cs) 
{
	return CWnd::PreCreateWindow(cs);
}



void CGrWnd::OnDestroy() 
{
	CWnd::OnDestroy();
	
// TODO: Add your message handler code here
//	destroy screen element
	if (m_grViewport) {	 delete m_grViewport; m_grViewport = NULL; }
	if (m_grScreen) { delete m_grScreen; m_grScreen = NULL; }

	m_grCreated = FALSE;
}


int CGrWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
// TODO: Add your specialized creation code here
//	Create a screen the size of the new client area
	RECT rect;
	int client_width, client_height;

	GetClientRect(&rect);
	client_width = (rect.right-rect.left);
	client_height = (rect.bottom-rect.top);
	if (client_width && client_height) {
		ASSERT(m_grScreen == NULL);
		m_grScreen = new grScreen(client_width, client_height, BPP_16, m_Name);
		m_grScreen->attach_to_window((unsigned)m_hWnd);
		m_grViewport = new grViewport(m_grScreen);
	}

	return 0;
}


void CGrWnd::OnMove(int x, int y) 
{
	CWnd::OnMove(x, y);
	
// TODO: Add your message handler code here
}

void CGrWnd::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	
// TODO: Add your message handler code here
	if (m_grViewport) {
		delete m_grViewport;
		delete m_grScreen;
		m_grViewport = NULL;
		m_grScreen = NULL;
	}
	if (cx && cy && !m_grViewport) {
		RECT rect;
		int client_width, client_height;

		GetClientRect(&rect);
		client_width = (rect.right-rect.left);
		client_height = (rect.bottom-rect.top);

		m_grScreen = new grScreen(client_width, client_height, BPP_16, m_Name);
		m_grScreen->attach_to_window((unsigned)m_hWnd);
		m_grViewport = new grViewport(m_grScreen);
	}
}


//	---------------------------------------------------------------------------
//	private functions
//	---------------------------------------------------------------------------

void CGrWnd::DrawCaption(BOOL active)
{
	CDC *dc;
	DWORD dwstyle;
	RECT winrect, caprect;
	int c_left=0, c_top=0;				// caption bar's coordinates in window

//	get needed info
	GetWindowRect(&winrect);
	dwstyle = GetStyle();
	if ((dwstyle & WS_CAPTION) != WS_CAPTION) return;
	dc = GetWindowDC();

//	calculate c_left, c_top
	if (dwstyle & WS_THICKFRAME) { 
		c_left += GetSystemMetrics(SM_CXSIZEFRAME);
		c_top += GetSystemMetrics(SM_CYSIZEFRAME);
	}
	if (dwstyle & WS_SYSMENU) {
		c_left += GetSystemMetrics(SM_CXSIZE);
	}
	caprect.left = c_left;
	caprect.top = c_top;
	
//	calucation c_right, c_bottom.		
	caprect.right = winrect.right - winrect.left - (2*c_left);
	if (dwstyle & WS_THICKFRAME) {
		caprect.right -= GetSystemMetrics(SM_CXSIZEFRAME);
	}
	caprect.bottom = GetSystemMetrics(SM_CYSIZE);

	if (active) {
		dc->SetBkColor(GetSysColor(COLOR_ACTIVECAPTION));
		dc->SetTextColor(GetSysColor(COLOR_CAPTIONTEXT));
	}
	else {
		dc->SetBkColor(GetSysColor(COLOR_INACTIVECAPTION));
		dc->SetTextColor(GetSysColor(COLOR_INACTIVECAPTIONTEXT));
	}

    dc->DrawText(m_Name, -1, &caprect, DT_LEFT );
	
	ReleaseDC(dc);
}


BOOL CGrWnd::OnNcActivate(BOOL bActive) 
{
	CWnd::OnNcActivate(bActive);

/*	m_Active = bActive;

	OnNcPaint();
*/
	return TRUE;

}


void CGrWnd::OnNcPaint() 
{
	CWnd::OnNcPaint();					// We may have to do this.
//	DrawCaption(m_Active);
}


void CGrWnd::OnChildActivate() 
{
	CWnd::OnChildActivate();
}
