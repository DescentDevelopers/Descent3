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
 * $Logfile: /DescentIII/Main/editor/FloatingKeypadDialog.cpp $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:57:38 $
 * $Author: kevinb $
 *
 * Floating keypad dialog
 *
 * $Log: not supported by cvs2svn $
 * 
 * 3     9/17/97 12:58p Samir
 * Got rid of Curve Keypad.
 * 
 * 2     8/29/97 2:49p Samir
 * Nuked SegmentKeypad.
 * 
 * 14    6/27/97 12:18p Samir
 * Added Room Keypad dialog.
 * 
 * 13    6/05/97 3:37p Samir
 * Added megacell keypad.
 * 
 * 12    4/08/97 10:42a Jason
 * implemented path following interface
 * 
 * 11    4/04/97 3:15p Samir
 * Added paths keypad, removed player keypad since it's joined with object
 * keypad.
 * 
 * 10    3/26/97 6:46p Samir
 * Fixed order of tabs in dialog box.
 * 
 * 9     3/20/97 11:55a Jason
 * changes for terrain editing/drawing
 * 
 * 8     3/18/97 12:16p Samir
 * Keypad tab is set to correct selection now when initializing.
 * 
 * 7     3/05/97 11:49a Samir
 * Added doorway and terrain keypad tabs.
 * 
 * 6     3/04/97 11:37a Samir
 * Fixed goofup in tab ordering for trigger keypad.
 * 
 * 5     2/20/97 6:05p Samir
 * Allow for clean activation and deactivation of surfaces in editor when
 * going into fullscreen mode.
 * 
 * 4     2/19/97 2:14p Samir
 * Added a RunKeypadFunction function.
 * 
 * 3     2/17/97 11:31a Samir
 * Tab now resizes if keypad dialog is resized.
 * 
 * 2     1/23/97 4:16p Samir
 * Saves width and height, position of floating keypad.
 *
 * $NoKeywords: $
 */

// FloatingKeypadDialog.cpp : implementation file
//

#include "stdafx.h"
#include "editor.h"
#include "TextureDialog.h"
#include "ObjectDialog.h"
#include "LightingDialog.h"
#include "TriggerDialog.h"
#include "DoorwayDialog.h"
#include "TerrainDialog.h"
#include "RoomKeypadDialog.h"
#include "PathPadDialog.h"
#include "MegacellKeypad.h"
#include "FloatingKeypadDialog.h"

#include "pserror.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFloatingKeypadDialog dialog


CFloatingKeypadDialog::CFloatingKeypadDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CFloatingKeypadDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFloatingKeypadDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_Parent = pParent;
	m_Initializing = FALSE;
	m_KeypadTabDialog = NULL;
}


void CFloatingKeypadDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFloatingKeypadDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BOOL CFloatingKeypadDialog::Create()
{
	return CDialog::Create(CFloatingKeypadDialog::IDD, m_Parent);
}


void CFloatingKeypadDialog::RunKeypadFunction(int code)
{
	if (m_KeypadTabDialog)	m_KeypadTabDialog->RunKeypadFunction(code);
}


void CFloatingKeypadDialog::Activate()
{
	if (m_KeypadTabDialog) m_KeypadTabDialog->Activate();
}


void CFloatingKeypadDialog::Deactivate()
{
	if (m_KeypadTabDialog) m_KeypadTabDialog->Deactivate();
}


BEGIN_MESSAGE_MAP(CFloatingKeypadDialog, CDialog)
	//{{AFX_MSG_MAP(CFloatingKeypadDialog)
	ON_WM_SHOWWINDOW()
	ON_WM_SIZE()
	ON_WM_MOVE()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFloatingKeypadDialog message handlers

void CFloatingKeypadDialog::OnCancel()
{
//	We may force closing this dialog from the parent.
//	DestroyWindow();
}


void CFloatingKeypadDialog::PostNcDestroy() 
{
	delete this;
}


BOOL CFloatingKeypadDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
// TODO: Add extra initialization here
//	Initialize keypad dialogs here.
	CTabCtrl *tab_ctrl;
	TC_ITEM tc_item;

	tab_ctrl = (CTabCtrl *)GetDlgItem(IDC_FLOATING_KEYPAD_TAB);

//	Add tabs to tabcontrol in the keypad dialog bar.
	tc_item.mask = TCIF_TEXT;
	tc_item.pszText = "Texture";
	tc_item.cchTextMax = lstrlen(tc_item.pszText)+1;
	tab_ctrl->InsertItem(TAB_TEXTURE_KEYPAD, &tc_item);

	tc_item.mask = TCIF_TEXT;
	tc_item.pszText = "Megacell";
	tc_item.cchTextMax = lstrlen(tc_item.pszText)+1;
	tab_ctrl->InsertItem(TAB_MEGACELL_KEYPAD, &tc_item);

	tc_item.mask = TCIF_TEXT;
	tc_item.pszText = "Terrain";
	tc_item.cchTextMax = lstrlen(tc_item.pszText)+1;
	tab_ctrl->InsertItem(TAB_TERRAIN_KEYPAD, &tc_item);

	tc_item.mask = TCIF_TEXT;
	tc_item.pszText = "Object";
	tc_item.cchTextMax = lstrlen(tc_item.pszText)+1;
	tab_ctrl->InsertItem(TAB_OBJECTS_KEYPAD, &tc_item);

	tc_item.mask = TCIF_TEXT;
	tc_item.pszText = "Room";
	tc_item.cchTextMax = lstrlen(tc_item.pszText)+1;
	tab_ctrl->InsertItem(TAB_ROOM_KEYPAD, &tc_item);

	tc_item.mask = TCIF_TEXT;
	tc_item.pszText = "Doorway";
	tc_item.cchTextMax = lstrlen(tc_item.pszText)+1;
	tab_ctrl->InsertItem(TAB_DOORWAY_KEYPAD, &tc_item);

	tc_item.mask = TCIF_TEXT;
	tc_item.pszText = "Trigger";
	tc_item.cchTextMax = lstrlen(tc_item.pszText)+1;
	tab_ctrl->InsertItem(TAB_TRIGGER_KEYPAD, &tc_item);

	tc_item.mask = TCIF_TEXT;
	tc_item.pszText = "Lighting";
	tc_item.cchTextMax = lstrlen(tc_item.pszText)+1;
	tab_ctrl->InsertItem(TAB_LIGHTING_KEYPAD, &tc_item);

	tc_item.mask = TCIF_TEXT;
	tc_item.pszText = "Paths";
	tc_item.cchTextMax = lstrlen(tc_item.pszText)+1;
	tab_ctrl->InsertItem(TAB_PATHS_KEYPAD, &tc_item);
  
//	fit keypad dialogs in tab control
	RECT tab_rect, tab_winrect;
	int tx, ty, tw, th;

	tab_ctrl->GetWindowRect(&tab_winrect);
	::CopyRect(&tab_rect, &tab_winrect);
	tab_ctrl->AdjustRect(FALSE, &tab_rect);
	tx = tab_rect.left - tab_winrect.left;
	ty = tab_rect.top - tab_winrect.top;
	tw = tab_rect.right - tab_rect.left;
	th = tab_rect.bottom - tab_rect.top;

//	Create tab dialogs
//	resize them to fit the tab control
//	set scroll bars accordingly
	m_TextureDialog = new CTextureDialog(tab_ctrl);
	m_TextureDialog->Create();
	m_TextureDialog->SetWindowPos(tab_ctrl, tx, ty, tw, th, SWP_NOZORDER);

	m_MegacellKeypad = new CMegacellKeypad(tab_ctrl);
	m_MegacellKeypad->Create();
	m_MegacellKeypad->SetWindowPos(tab_ctrl, tx, ty, tw, th, SWP_NOZORDER);

	m_ObjectDialog = new CObjectDialog(tab_ctrl);
	m_ObjectDialog->Create();
	m_ObjectDialog->SetWindowPos(tab_ctrl, tx, ty, tw, th, SWP_NOZORDER);

	m_LightingDialog = new CLightingDialog(tab_ctrl);
	m_LightingDialog->Create();
	m_LightingDialog->SetWindowPos(tab_ctrl, tx, ty, tw, th, SWP_NOZORDER);

	m_RoomDialog = new CRoomKeypadDialog(tab_ctrl);
	m_RoomDialog->Create();
	m_RoomDialog->SetWindowPos(tab_ctrl, tx, ty, tw, th, SWP_NOZORDER);

	m_TriggerDialog = new CTriggerDialog(tab_ctrl);
	m_TriggerDialog->Create();
	m_TriggerDialog->SetWindowPos(tab_ctrl, tx, ty, tw, th, SWP_NOZORDER);

	m_DoorwayDialog = new CDoorwayDialog(tab_ctrl);
	m_DoorwayDialog->Create();
	m_DoorwayDialog->SetWindowPos(tab_ctrl, tx, ty, tw, th, SWP_NOZORDER);

	m_TerrainDialog = new CTerrainDialog(tab_ctrl);
	m_TerrainDialog->Create();
	m_TerrainDialog->SetWindowPos(tab_ctrl, tx, ty, tw, th, SWP_NOZORDER);

	m_PathPadDialog = new CPathPadDialog(tab_ctrl);
	m_PathPadDialog->Create();
	m_PathPadDialog->SetWindowPos(tab_ctrl, tx, ty, tw, th, SWP_NOZORDER);


//	Display current tab dialog	
	switch (D3EditState.keypad_current) 
	{
		case TAB_TEXTURE_KEYPAD:	m_KeypadTabDialog = m_TextureDialog; break;
		case TAB_OBJECTS_KEYPAD:	m_KeypadTabDialog = m_ObjectDialog; break;
		case TAB_LIGHTING_KEYPAD:	m_KeypadTabDialog = m_LightingDialog; break;
		case TAB_ROOM_KEYPAD:		m_KeypadTabDialog = m_RoomDialog; break;
		case TAB_PATHS_KEYPAD:		m_KeypadTabDialog = m_PathPadDialog; break;
		case TAB_TRIGGER_KEYPAD:	m_KeypadTabDialog = m_TriggerDialog; break;
		case TAB_DOORWAY_KEYPAD:	m_KeypadTabDialog = m_DoorwayDialog; break;
		case TAB_TERRAIN_KEYPAD:	m_KeypadTabDialog = m_TerrainDialog; break;
		case TAB_MEGACELL_KEYPAD:	m_KeypadTabDialog = m_MegacellKeypad; break;
		default: Int3();
	}

	tab_ctrl->SetCurSel(D3EditState.keypad_current);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CFloatingKeypadDialog::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);

	m_Initializing = FALSE;
	
	if (bShow) m_KeypadTabDialog->ShowWindow(SW_SHOW);
}


BOOL CFloatingKeypadDialog::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
// TODO: Add your specialized code here and/or call the base class
//	Perform notification requests on the keypad tab dialog bar and other 
//	custom control bars.
	NMHDR *nmhdr;

	assert(lParam != 0);
	
	nmhdr = (NMHDR *)lParam;

	switch (wParam)
	{
		case IDC_FLOATING_KEYPAD_TAB:
			DoKeypadTabNotify(nmhdr);
			break;
	}
	
	return CDialog::OnNotify(wParam, lParam, pResult);
}


void CFloatingKeypadDialog::DoKeypadTabNotify(NMHDR *nmhdr)
{
	CTabCtrl *tab_ctrl;
		
	tab_ctrl = (CTabCtrl *)GetDlgItem(IDC_FLOATING_KEYPAD_TAB);

	switch (nmhdr->code)
	{
		case TCN_SELCHANGE:		
		// Tab control changed. So update the controls currently displayed.
			D3EditState.keypad_current = (int)tab_ctrl->GetCurSel(); 
			assert(D3EditState.keypad_current > -1);

			switch (D3EditState.keypad_current)
			{
				case TAB_TEXTURE_KEYPAD:
					if (m_KeypadTabDialog) m_KeypadTabDialog->ShowWindow(SW_HIDE);
					m_KeypadTabDialog = m_TextureDialog;
					m_KeypadTabDialog->ShowWindow(SW_SHOW);
					break;

				case TAB_OBJECTS_KEYPAD:
					if (m_KeypadTabDialog) m_KeypadTabDialog->ShowWindow(SW_HIDE);
					m_KeypadTabDialog = m_ObjectDialog;
					m_KeypadTabDialog->ShowWindow(SW_SHOW);
					break;

				case TAB_LIGHTING_KEYPAD:
					if (m_KeypadTabDialog) m_KeypadTabDialog->ShowWindow(SW_HIDE);
					m_KeypadTabDialog = m_LightingDialog;
					m_KeypadTabDialog->ShowWindow(SW_SHOW);
					break;

				case TAB_ROOM_KEYPAD:
					if (m_KeypadTabDialog) m_KeypadTabDialog->ShowWindow(SW_HIDE);
					m_KeypadTabDialog = m_RoomDialog;
					m_KeypadTabDialog->ShowWindow(SW_SHOW);
					break;

				case TAB_PATHS_KEYPAD:
					if (m_KeypadTabDialog) m_KeypadTabDialog->ShowWindow(SW_HIDE);
					m_KeypadTabDialog = m_PathPadDialog;
					m_KeypadTabDialog->ShowWindow(SW_SHOW);
					break;

				case TAB_TRIGGER_KEYPAD:
					if (m_KeypadTabDialog) m_KeypadTabDialog->ShowWindow(SW_HIDE);
					m_KeypadTabDialog = m_TriggerDialog;
					m_KeypadTabDialog->ShowWindow(SW_SHOW);
					break;

				case TAB_DOORWAY_KEYPAD:
					if (m_KeypadTabDialog) m_KeypadTabDialog->ShowWindow(SW_HIDE);
					m_KeypadTabDialog = m_DoorwayDialog;
					m_KeypadTabDialog->ShowWindow(SW_SHOW);
					break;

				case TAB_TERRAIN_KEYPAD:
					if (m_KeypadTabDialog) m_KeypadTabDialog->ShowWindow(SW_HIDE);
					m_KeypadTabDialog = m_TerrainDialog;
					m_KeypadTabDialog->ShowWindow(SW_SHOW);
					break;

				case TAB_MEGACELL_KEYPAD:
					if (m_KeypadTabDialog) m_KeypadTabDialog->ShowWindow(SW_HIDE);
					m_KeypadTabDialog = m_MegacellKeypad;
					m_KeypadTabDialog->ShowWindow(SW_SHOW);
					break;
			}
			break;
	}
}


void CFloatingKeypadDialog::OnSize(UINT nType, int cx, int cy) 
{
	RECT rect;
	CTabCtrl *tab_ctrl;

	CDialog::OnSize(nType, cx, cy);
		
	if (m_Initializing) return;

	SetRect(&rect, 0,0,cx,cy);
	::AdjustWindowRect(&rect, GetStyle(), FALSE);

	D3EditState.float_keypad_w = rect.right-rect.left;
	D3EditState.float_keypad_h = rect.bottom - rect.top;

	SetWindowPos(&wndTopMost, 0,0,0,0,SWP_NOSIZE | SWP_NOMOVE);

//	Resize tab to fit current modeless dialog size
	RECT tab_rect, tab_winrect;
	int tx, ty, tw, th;

	tab_ctrl = (CTabCtrl *)GetDlgItem(IDC_FLOATING_KEYPAD_TAB);

	GetClientRect(&tab_rect);
	tab_ctrl->MoveWindow(&tab_rect);

	tab_ctrl->GetWindowRect(&tab_winrect);
	::CopyRect(&tab_rect, &tab_winrect);
	tab_ctrl->AdjustRect(FALSE, &tab_rect);
	tx = tab_rect.left - tab_winrect.left;
	ty = tab_rect.top - tab_winrect.top;
	tw = tab_rect.right - tab_rect.left;
	th = tab_rect.bottom - tab_rect.top;

	m_TextureDialog->SetWindowPos(tab_ctrl, tx, ty, tw, th, SWP_NOZORDER);
	m_ObjectDialog->SetWindowPos(tab_ctrl, tx, ty, tw, th, SWP_NOZORDER);
	m_LightingDialog->SetWindowPos(tab_ctrl, tx, ty, tw, th, SWP_NOZORDER);
	m_RoomDialog->SetWindowPos(tab_ctrl, tx, ty, tw, th, SWP_NOZORDER);
	m_PathPadDialog->SetWindowPos(tab_ctrl, tx, ty, tw, th, SWP_NOZORDER);
	m_TriggerDialog->SetWindowPos(tab_ctrl, tx, ty, tw, th, SWP_NOZORDER);
	m_DoorwayDialog->SetWindowPos(tab_ctrl, tx, ty, tw, th, SWP_NOZORDER);
	m_TerrainDialog->SetWindowPos(tab_ctrl, tx, ty, tw, th, SWP_NOZORDER);
	m_MegacellKeypad->SetWindowPos(tab_ctrl, tx, ty, tw, th, SWP_NOZORDER);
}


void CFloatingKeypadDialog::OnMove(int x, int y) 
{
	CDialog::OnMove(x, y);
	
// TODO: Add your message handler code here
//	if we haven't placed this dialog anywhere, wait till parent does it for us.
	if (m_Initializing) return;

	RECT rect;
	GetWindowRect(&rect);
	GetParent()->ScreenToClient(&rect);
	D3EditState.float_keypad_x = rect.left;
	D3EditState.float_keypad_y = rect.top;	

	SetWindowPos(&wndTopMost, 0,0,0,0,SWP_NOSIZE | SWP_NOMOVE);
}


int CFloatingKeypadDialog::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
// TODO: Add your specialized creation code here
	m_Initializing = TRUE;	

	return 0;
}
