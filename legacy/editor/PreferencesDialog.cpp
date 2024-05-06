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

 * $Logfile: /DescentIII/Main/editor/PreferencesDialog.cpp $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:57:38 $
 * $Author: kevinb $
 *
 * Preferences
 *
 * $Log: not supported by cvs2svn $
 * 
 * 18    4/02/99 11:32p Jeff
 * able to turn off IGC in debug mode.  Better handling of cinematics
 * quick exit if playing via alt-p
 * 
 * 17    3/15/99 9:24p Gwar
 * 
 * 16    3/15/99 6:02p Jeff
 * PltClearList no longer takes an arg
 * 
 * 15    11/30/98 11:56a Jeff
 * fixed bug, allowing unlimited pilots
 * 
 * 14    9/04/98 3:16p Samir
 * control whether to allow joystick slewing.
 * 
 * 13    7/16/98 12:06p Jason
 * added special flags to room structure
 * 
 * 12    5/20/98 6:59p Samir
 * added slew speed slider.
 * 
 * 11    3/20/98 1:20p Jeff
 * Hooked Default Pilot into the code and database
 * 
 * 10    3/19/98 6:57p Jeff
 * Added in a default pilot section for Preferences
 * 
 * 9     2/14/98 10:48p Jason
 * got preferred rendering working
 * 
 * 8     8/01/97 11:59a Samir
 * Added a preference for debugging in fullscreen.
 * 
 * 7     7/25/97 4:00p Matt
 * Was swapping zbuffer and bilinear filter states
 * 
 * 6     7/24/97 3:03p Matt
 * Got rid of D3EditState fields that duplicated other variables
 * 
 * 5     7/23/97 10:18a Matt
 * Cleaned up D3EditState, moving some vars in and some out, and renaming
 * and changing a few others
 * 
 * 6     6/16/97 2:34p Jason
 * added 3dfx support
 * 
 * 5     6/05/97 6:10p Jason
 * added support for zbuffering and bilinear filtering
 * 
 * 4     5/27/97 5:41p Jason
 * added toggling of opengl renderer
 * 
 * 3     5/12/97 2:50p Jason
 * fixed dumb bug with 16bit mode setting
 * 
 * 
 * 2     4/30/97 5:43p Jason
 * added ability to toggle whether or not you want to run in 16 bit mode
 * 
 * 1     2/05/97 11:58a Samir
 * 
 * 2     2/05/97 11:45a Samir
 * Baisc functionality for D3windowed toggle
 *
 * $NoKeywords: $
 */

// PreferencesDialog.cpp : implementation file
//

#include "stdafx.h"
#include "editor.h"
#include "PreferencesDialog.h"
#include "game.h"
#include "pilot.h"
#include "slew.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern bool Cinematics_enabled;

/////////////////////////////////////////////////////////////////////////////
// CPreferencesDialog dialog


CPreferencesDialog::CPreferencesDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CPreferencesDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPreferencesDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CPreferencesDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPreferencesDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPreferencesDialog, CDialog)
	//{{AFX_MSG_MAP(CPreferencesDialog)
	ON_BN_CLICKED(IDC_BILINEAR_CHECK, OnBilinearCheck)
	ON_BN_CLICKED(IDC_WINDOWED, OnWindowed)
	ON_BN_CLICKED(IDC_FULLSCREEN_HW, OnFullScreenHW)
	ON_BN_CLICKED(IDC_FULLSCREEN_SW, OnFullScreenSW)
	ON_BN_CLICKED(IDC_MIPPING_CHECK, OnMippingCheck)
	ON_BN_CLICKED(IDC_IGC_ENABLED, OnIgcEnabled)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPreferencesDialog message handlers

#define GET_CHECK(id) (((CButton *)GetDlgItem(id))->GetCheck() != 0)

void CPreferencesDialog::OnOK() 
{
	CSliderCtrl *slew_slider = (CSliderCtrl *)GetDlgItem(IDC_SLEWSLIDER);
	bool joyslew;

	if (GET_CHECK(IDC_WINDOWED))
		D3EditState.game_render_mode = GM_WINDOWED;
	else if (GET_CHECK(IDC_FULLSCREEN_SW))
		D3EditState.game_render_mode = GM_FULLSCREEN_SW;
	else if (GET_CHECK(IDC_FULLSCREEN_HW))
		D3EditState.game_render_mode = GM_FULLSCREEN_HW;
	else
		Int3();		//unknown game render mode

//	Do we want fullscreen debug mode?
	if (GET_CHECK(IDC_DEBUG_CHECK)) 
		D3EditState.fullscreen_debug_state = true;
	else 
		D3EditState.fullscreen_debug_state = false;

	if (GET_CHECK(IDC_USE_OPENGL))
		PreferredRenderer = RENDERER_OPENGL;
	else if (GET_CHECK(IDC_USE_GLIDE))
		PreferredRenderer = RENDERER_GLIDE;
	else if (GET_CHECK(IDC_USE_D3D))
		PreferredRenderer = RENDERER_DIRECT3D;
	else
		Int3();		//unknown hardware renderer

	
	joyslew = (GET_CHECK(IDC_JOYENABLE)) ? true : false;

	if (joyslew != D3EditState.joy_slewing) {
		D3EditState.joy_slewing = joyslew;
		SlewControlInit();
	}

	Render_preferred_state.filtering = GET_CHECK(IDC_BILINEAR_CHECK);
	Render_preferred_state.mipping = GET_CHECK(IDC_MIPPING_CHECK);

	CComboBox *combo = (CComboBox *)GetDlgItem(IDC_DEFAULT_PILOT);
	if(combo->GetCount())
	{
		combo->GetLBText(combo->GetCurSel(),Default_pilot);
	}
	else
	{
		strcpy(Default_pilot," ");
	}

	Slew_key_speed = (slew_slider->GetPos() * 0.5) + 0.5;

	CDialog::OnOK();
}


BOOL CPreferencesDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	char **pilotlist;
	int pilot_count;
	
	// TODO: Add extra initialization here
	CSliderCtrl *slew_slider = (CSliderCtrl *)GetDlgItem(IDC_SLEWSLIDER);

	((CButton *)GetDlgItem(IDC_WINDOWED))->SetCheck(D3EditState.game_render_mode == GM_WINDOWED);
	((CButton *)GetDlgItem(IDC_FULLSCREEN_SW))->SetCheck(D3EditState.game_render_mode == GM_FULLSCREEN_SW);
	((CButton *)GetDlgItem(IDC_FULLSCREEN_HW))->SetCheck(D3EditState.game_render_mode == GM_FULLSCREEN_HW);
	((CButton *)GetDlgItem(IDC_DEBUG_CHECK))->SetCheck(D3EditState.fullscreen_debug_state);
	((CButton *)GetDlgItem(IDC_USE_OPENGL))->SetCheck(PreferredRenderer == RENDERER_OPENGL);
	((CButton *)GetDlgItem(IDC_USE_GLIDE))->SetCheck(PreferredRenderer == RENDERER_GLIDE);
	((CButton *)GetDlgItem(IDC_USE_D3D))->SetCheck(PreferredRenderer == RENDERER_DIRECT3D);
	((CButton *)GetDlgItem(IDC_BILINEAR_CHECK))->SetCheck(Render_preferred_state.filtering);
	((CButton *)GetDlgItem(IDC_MIPPING_CHECK))->SetCheck(Render_preferred_state.mipping);
	((CButton *)GetDlgItem(IDC_JOYENABLE))->SetCheck(D3EditState.joy_slewing);
	((CButton *)GetDlgItem(IDC_IGC_ENABLED))->SetCheck(Cinematics_enabled);

	slew_slider->SetRange(0,3);
	slew_slider->SetPos((Slew_key_speed - 0.5)/0.5);

	CComboBox *combo = (CComboBox *)GetDlgItem(IDC_DEFAULT_PILOT);
	combo->ResetContent();
	pilotlist = PltGetPilots(&pilot_count);
	for(int i=0;i<pilot_count;i++)
	{
		combo->AddString(pilotlist[i]);
	}
	if(pilot_count)
	{
		combo->SelectString(-1,Default_pilot);
	}
	PltClearList();

	if (D3EditState.game_render_mode == GM_FULLSCREEN_HW)
		EnableHardwareOptions();
	else
		DisableHardwareOptions();

	PltGetPilotsFree();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CPreferencesDialog::OnBilinearCheck() 
{
	// TODO: Add your control notification handler code here
	
}

void CPreferencesDialog::DisableHardwareOptions()
{
	((CButton *)GetDlgItem(IDC_HARDWARE_ACC))->EnableWindow(0);
	((CButton *)GetDlgItem(IDC_USE_OPENGL))->EnableWindow(0);
	((CButton *)GetDlgItem(IDC_USE_GLIDE))->EnableWindow(0);
	((CButton *)GetDlgItem(IDC_USE_D3D))->EnableWindow(0);
	((CButton *)GetDlgItem(IDC_BILINEAR_CHECK))->EnableWindow(0);
	((CButton *)GetDlgItem(IDC_MIPPING_CHECK))->EnableWindow(0);
}

void CPreferencesDialog::EnableHardwareOptions()
{
	((CButton *)GetDlgItem(IDC_HARDWARE_ACC))->EnableWindow(1);
	((CButton *)GetDlgItem(IDC_USE_OPENGL))->EnableWindow(1);
	((CButton *)GetDlgItem(IDC_USE_GLIDE))->EnableWindow(1);
	((CButton *)GetDlgItem(IDC_USE_D3D))->EnableWindow(1);
	((CButton *)GetDlgItem(IDC_BILINEAR_CHECK))->EnableWindow(1);
	((CButton *)GetDlgItem(IDC_MIPPING_CHECK))->EnableWindow(1);
}

void CPreferencesDialog::OnWindowed() 
{
	if (((CButton *)GetDlgItem(IDC_WINDOWED))->GetCheck())
		DisableHardwareOptions();
	else
		EnableHardwareOptions();
}

void CPreferencesDialog::OnFullScreenSW() 
{
	if (((CButton *)GetDlgItem(IDC_FULLSCREEN_SW))->GetCheck())
		DisableHardwareOptions();
	else
		EnableHardwareOptions();
}

void CPreferencesDialog::OnFullScreenHW() 
{
	if (((CButton *)GetDlgItem(IDC_FULLSCREEN_HW))->GetCheck())
		EnableHardwareOptions();
	else
		DisableHardwareOptions();
}

void CPreferencesDialog::OnMippingCheck() 
{
	// TODO: Add your control notification handler code here
	
}

void CPreferencesDialog::OnIgcEnabled() 
{
	CButton *button = (CButton *)GetDlgItem(IDC_IGC_ENABLED);
	Cinematics_enabled = (bool)(button->GetCheck()!=0);	
}
