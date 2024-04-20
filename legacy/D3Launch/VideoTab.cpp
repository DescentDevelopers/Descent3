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
 * $Logfile: /DescentIII/Main/D3Launch/VideoTab.cpp $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:56:51 $
 * $Author: kevinb $
 *
 * 
 *
 * $Log: not supported by cvs2svn $
 * 
 * 18    5/19/99 12:29p Nate
 * Fixed openGL crash, changed Network speed default, changed OpenGL
 * detection to default to unchecked, and removed config file parsing from
 * US version
 * 
 * 17    4/26/99 11:38a Nate
 * Added warning message for opengl or direct3d renderer selection when
 * user has glide as an option.
 * 
 * 16    3/02/99 5:46p Nate
 * Lots of little changes/fixes
 * 
 * 15    2/05/99 3:51p Nate
 * Added conditional compilation directives for OEM support
 * 
 * 14    10/19/98 9:03a Nate
 * Video tab now only auto-detects when "None selected" 
 * 
 * 13    10/13/98 3:03p Nate
 * More fixes and changes.
 * 
 * 12    10/12/98 7:13p Nate
 * Fixed several bugs.
 * 
 * 11    10/10/98 4:05p Nate
 * Added detection/scanning window messages
 * 
 * 10    10/08/98 6:23p Nate
 * Fixed a few bugs.
 * 
 * 9     9/22/98 3:33p Nate
 * Added conditional compiling to help system (can toggle between HTML and
 * standard windows help)
 * 
 * 8     9/21/98 5:40p Nate
 * Incorporated the new HTML help system
 * 
 * 7     9/02/98 9:48a Nate
 * Fixed hardware accelerator display bug in the Speed Tab
 * 
 * 6     9/01/98 7:15p Nate
 * Major Revision #2
 * 
 * 5     8/31/98 6:44p Nate
 * Major Revision
 * 
 * 4     8/10/98 10:44a Nate
 * Added Language selection support
 * 
 * 3     8/05/98 4:52p Nate
 * Added registry reading and writing.
 * 
 * 2     8/05/98 11:54a Nate
 * Initial Version
 *
 * $NoKeywords: $
 */

// VideoTab.cpp : implementation file
//

#include "stdafx.h"
#include "afxpriv.h"
#include "afxext.h"
#include "D3Launch.h"
#include "VideoTab.h"
#include "3D_detect.h"
#include "PsTypes.h"
#include "OS_Config.h"
#include "VideoDetectDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CVideoTab * CurrentVideoTab = NULL;
BOOL VideoTabInitialized = FALSE;

/////////////////////////////////////////////////////////////////////////////
// CVideoTab property page

IMPLEMENT_DYNCREATE(CVideoTab, CPropertyPage)

CVideoTab::CVideoTab() : CPropertyPage(CVideoTab::IDD)
{
	//{{AFX_DATA_INIT(CVideoTab)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	CurrentVideoTab = NULL;
}

CVideoTab::~CVideoTab()
{
	CurrentVideoTab = NULL;
}

int CVideoTab::GetCurrentCard()
{
	int current = m_videocard_list.GetCurSel();

	if ( (current < 0) || (current>=Num_cards))	{
		current = 0;
	}
	return current;
}

void CVideoTab::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CVideoTab)
	DDX_Control(pDX, IDC_VIDEOCARD_LIST, m_videocard_list);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CVideoTab, CPropertyPage)
	//{{AFX_MSG_MAP(CVideoTab)
	ON_BN_CLICKED(IDC_VIDEOCARD_DETECT, OnVideocardDetect)
	ON_MESSAGE(WM_COMMANDHELP,OnCommandHelp)
	ON_WM_HELPINFO()
	ON_CBN_SELCHANGE(IDC_VIDEOCARD_LIST, OnSelchangeVideocardList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVideoTab message handlers

extern HINSTANCE Dd_dll_handle;

void CVideoTab::OnVideocardDetect() 
{
	// TODO: Add your control notification handler code here
	if(VideoCardsDetected) return;

#if (!defined(OEM_VOODOO3) || defined(USE_ALL_VIDEO_OPTIONS))
	// Display the scan selection options to user
	CVideoDetectDlg dlg;
	dlg.DoModal();
#endif

	// Set the wait cursor
	CWaitCursor wc;

	// Clear the box
	m_videocard_list.ResetContent();

	// Display the scanning window
	m_MsgDlg.m_ScanningMsg.LoadString(IDS_VIDEOTAB_SCANNING);
	m_MsgDlg.m_WaitMsg.LoadString(IDS_PLEASE_WAIT_MSG);
	m_MsgDlg.Create(IDD_MSG_DLG,this);
	m_MsgDlg.ShowWindow(SW_SHOW);
	m_MsgDlg.UpdateData(FALSE);

#if (defined(OEM_VOODOO3) && !defined(USE_ALL_VIDEO_OPTIONS))
	// Detect only 3dfx cards
	detect_3dcards(0,1,0);
#else
	// Detect the cards
	detect_3dcards(dlg.DetectDirect3D,dlg.DetectGlide,dlg.DetectOpenGL);
#endif

	// Remove the scanning window
	m_MsgDlg.DestroyWindow();

	// Now that the window is gone, release the opengl dll (fixes crash)
	if(opengl_dll_handle!=NULL) {
		FreeLibrary(opengl_dll_handle);
		opengl_dll_handle=NULL;
	}

	int i;
	int current=0;

	// Add cards to listbox
	for (i=0; i<Num_cards; i++ )	{
		// Construct the listbox name
		m_videocard_list.InsertString( i, GetFullName(&Cards[i]) );
	}

	// If only one card (in addition to NONE option) has been detected, select it
	if(Num_cards==2) current=1;

	// If more than one card, then display the "you must select one" message
	if(Num_cards>2) {
		current=0;	// set it to "none", make user pick

		CString mtod_msg, mtod_title;
		mtod_msg.LoadString(IDS_VIDEOTAB_MTOD_MSG);
		mtod_title.LoadString(IDS_VIDEOTAB_MTOD_TITLE);
		MessageBox(mtod_msg,mtod_title,MB_OK|MB_ICONINFORMATION);
	}

	// Set the determined selection
	m_videocard_list.SetCurSel(current);
	
	// Desensitize detection button since cards have been detected
	//HWND hwnd;
	//GetDlgItem(IDC_VIDEOCARD_DETECT,&hwnd);
	//::EnableWindow(hwnd,FALSE);
}


BOOL CVideoTab::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here
	CDialog::OnInitDialog();

	CurrentVideoTab = this;

	// Get the PreferredRenderer
	RendererType renderer_id = (RendererType)os_config_read_uint(NULL, "PreferredRenderer", RENDERER_NONE);

	// Get the name of the selected rendering device
	char *current_card = os_config_read_string(szSectionName, "RenderingDeviceName", "" );
		
	// Clear the box
	m_videocard_list.ResetContent();

	// if renderer none is selected, automatically display the detection dialog
	int current=-1;
	if(renderer_id==RENDERER_NONE) {

#if (!defined(OEM_VOODOO3) || defined(USE_ALL_VIDEO_OPTIONS))
		// Display the scan selection options to user
		CVideoDetectDlg dlg;
		dlg.DoModal();
#endif

		// Set the wait cursor
		CWaitCursor wc;

		// Display the scanning window
		m_MsgDlg.m_ScanningMsg.LoadString(IDS_VIDEOTAB_SCANNING);
		m_MsgDlg.m_WaitMsg.LoadString(IDS_PLEASE_WAIT_MSG);
		m_MsgDlg.Create(IDD_MSG_DLG,this);
		m_MsgDlg.ShowWindow(SW_SHOW);
		m_MsgDlg.UpdateData(FALSE);
	
#if (defined(OEM_VOODOO3) && !defined(USE_ALL_VIDEO_OPTIONS))
		// Detect only 3dfx cards
		detect_3dcards(0,1,0);
#else
		// Detect the cards
		detect_3dcards(dlg.DetectDirect3D,dlg.DetectGlide,dlg.DetectOpenGL);
#endif

		// Remove the scanning window
		m_MsgDlg.DestroyWindow();

		// Now that the window is gone, release the opengl dll (fixes crash)
		if(opengl_dll_handle!=NULL) {
			FreeLibrary(opengl_dll_handle);
			opengl_dll_handle=NULL;
		}

		// Put the detected cards into the list, and see if the selected card is one of them
		for(int j=0; j<Num_cards; j++) {
			m_videocard_list.InsertString( j, GetFullName(&Cards[j]) );
			if ( Cards[j].renderer_type==renderer_id && !stricmp(Cards[j].name, current_card) ) 
				current=j;
		}
	}
	else {	// just put the selected card in the list (and none selected)
		detect_3dcards(0,0,0);
		m_videocard_list.InsertString( 0, GetFullName(&Cards[0]) );
		Cards[Num_cards].renderer_type=renderer_id;
		strcpy( Cards[Num_cards].name, current_card );
		m_videocard_list.InsertString( Num_cards, GetFullName(&Cards[Num_cards]) );
		current = Num_cards;
		Num_cards++;
	}

	// Only do "auto-selection" when renderer_id is set to RENDERER_NONE
	if(renderer_id==RENDERER_NONE || current==-1) {

		if(Num_cards<=2) current=Num_cards-1;

		// If more than one card, then display the "you must select one" message
		if(Num_cards>2) {
			current=0;	// set it to "none", make user pick

			CString mtod_msg, mtod_title;
			mtod_msg.LoadString(IDS_VIDEOTAB_MTOD_MSG);
			mtod_title.LoadString(IDS_VIDEOTAB_MTOD_TITLE);
			MessageBox(mtod_msg,mtod_title,MB_OK|MB_ICONINFORMATION);
		}
	}

	/*
	// If the selected card isn't in the current list, put it in the list
	if(current==-1) {
		Cards[Num_cards].renderer_type=renderer_id;
		strcpy( Cards[Num_cards].name, current_card );
		m_videocard_list.InsertString( Num_cards, GetFullName(&Cards[Num_cards]) );
		current = Num_cards;
		Num_cards++;
	}
	*/

	// Desensitize detection button since cards have been detected
	//HWND hwnd;
	//GetDlgItem(IDC_VIDEOCARD_DETECT,&hwnd);
	//::EnableWindow(hwnd,FALSE);

		/*
		detect_3dcards(0);
		m_videocard_list.InsertString( 0, GetFullName(&Cards[0]) );

		if ( renderer_id != RENDERER_NONE ) {
			Cards[Num_cards].renderer_type=renderer_id;
			strcpy( Cards[Num_cards].name, current_card );
			m_videocard_list.InsertString( Num_cards, GetFullName(&Cards[Num_cards]) );
			current = Num_cards;
			Num_cards++;
		}
		*/

	m_videocard_list.SetCurSel(current);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CVideoTab::OnOK() 
{
	// TODO: Add your specialized code here and/or call the base class
	int current = m_videocard_list.GetCurSel();

	if ( (current < 0) || (current>=Num_cards))	{
		current = 0;
	}

	OutputDebugString("Setting the videocard!\n");

	// Get the PreferredRenderer
	os_config_write_uint(NULL, "PreferredRenderer", Cards[current].renderer_type);

	// Write out the name of the rendering device
	os_config_write_string(szSectionName, "RenderingDeviceName", Cards[current].name);
	
	CPropertyPage::OnOK();
}


BOOL CVideoTab::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	// TODO: Add your message handler code here and/or call default

#ifdef USE_HTML_HELP_SYSTEM
	CWaitCursor wc;
	help_launch(VIDEOTAB_HELP);
	return 1;	
#else
	return CPropertyPage::OnHelpInfo(pHelpInfo);
#endif
}

// Display the html help file
afx_msg LRESULT CVideoTab::OnCommandHelp(WPARAM wParam, LPARAM lParam)
{
#ifdef USE_HTML_HELP_SYSTEM
	help_launch(VIDEOTAB_HELP);
	return 1;
#else
	return CPropertyPage::OnCommandHelp(wParam,lParam);
#endif
}

void CVideoTab::OnSelchangeVideocardList() 
{
	int current = m_videocard_list.GetCurSel();

	if ( (current < 0) || (current>=Num_cards))	{
		current = 0;
	}

	// If user's DirectX version is less than 6.0,
	// then don't let them select Direct 3D
	if(Dx_version<6) {
		if(Cards[current].renderer_type==RENDERER_DIRECT3D) {
			CString title, msg;
			title.LoadString(IDS_VIDEOTAB_DXVERSION_TITLE);
			msg.LoadString(IDS_VIDEOTAB_DXVERSION_MSG);
			MessageBox(msg,title,MB_OK|MB_ICONEXCLAMATION);
			m_videocard_list.SetCurSel(0);
			return;
		}
	}

	// See if user has the option of selecting 3Dfx glide
	int glide = -1;
	for(int j=0;j<Num_cards;j++) {
		if(Cards[j].renderer_type==RENDERER_GLIDE) {
			glide=j;
			break;
		}
	}

	// Display warning if user has glide and is choosing opengl or direct3d
	if(glide>=0 && glide<Num_cards) {
		if(Cards[current].renderer_type==RENDERER_DIRECT3D || Cards[current].renderer_type==RENDERER_OPENGL) {
			CString title, msg;
			title.LoadString(IDS_NOTGLIDE_TITLE);
			msg.LoadString(IDS_NOTGLIDE_MSG);
			MessageBox(msg,title,MB_OK|MB_ICONEXCLAMATION);
		}
	}
}
