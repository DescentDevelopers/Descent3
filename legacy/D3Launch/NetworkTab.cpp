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
 * $Logfile: /DescentIII/Main/D3Launch/NetworkTab.cpp $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:56:51 $
 * $Author: kevinb $
 *
 * 
 *
 * $Log: not supported by cvs2svn $
 * 
 * 7     5/19/99 12:29p Nate
 * Fixed openGL crash, changed Network speed default, changed OpenGL
 * detection to default to unchecked, and removed config file parsing from
 * US version
 * 
 * 6     9/22/98 3:33p Nate
 * Added conditional compiling to help system (can toggle between HTML and
 * standard windows help)
 * 
 * 5     9/21/98 5:40p Nate
 * Incorporated the new HTML help system
 * 
 * 4     9/21/98 12:35p Nate
 * Default network connection is now "Dialup" and default speed is "28K"
 * 
 * 3     8/10/98 10:44a Nate
 * Added Language selection support
 * 
 * 2     8/05/98 11:54a Nate
 * Initial Version
 *
 * $NoKeywords: $
 */

// NetworkTab.cpp : implementation file
//

#include "stdafx.h"
#include "afxpriv.h"
#include "afxext.h"
#include "D3Launch.h"
#include "NetworkTab.h"
#include "PsTypes.h"
#include "OS_Config.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define CONNECTION_DIALUP	"Dialup"
#define CONNECTION_LAN		"LAN"
#define CONNECTION_NONE     "None"

// defines for connection type
#define CONNECTION_SPEED_28K		"28K"
#define CONNECTION_SPEED_33K		"33K"
#define CONNECTION_SPEED_56K		"56K"
#define CONNECTION_SPEED_SISDN		"ISDN"
#define CONNECTION_SPEED_CABLE		"Cable"
#define CONNECTION_SPEED_FAST		"Fast"

/////////////////////////////////////////////////////////////////////////////
// CNetworkTab property page

IMPLEMENT_DYNCREATE(CNetworkTab, CPropertyPage)

CNetworkTab::CNetworkTab() : CPropertyPage(CNetworkTab::IDD)
{
	//{{AFX_DATA_INIT(CNetworkTab)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CNetworkTab::~CNetworkTab()
{
}

void CNetworkTab::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNetworkTab)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNetworkTab, CPropertyPage)
	//{{AFX_MSG_MAP(CNetworkTab)
	ON_BN_CLICKED(IDC_DIALUP, OnDialup)
	ON_BN_CLICKED(IDC_LAN, OnLan)
	ON_BN_CLICKED(IDC_NONE, OnNone)
	ON_MESSAGE(WM_COMMANDHELP,OnCommandHelp)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNetworkTab message handlers

void CNetworkTab::OnOK() 
{
	// TODO: Add your specialized code here and/or call the base class
	char *internet_connection = CONNECTION_NONE;
	char *connection_speed = CONNECTION_SPEED_28K;

	if ( ((CButton *) GetDlgItem(IDC_DIALUP))->GetCheck() == 1 )
		internet_connection = CONNECTION_DIALUP;
	else if ( ((CButton *) GetDlgItem(IDC_LAN))->GetCheck() == 1 )
		internet_connection = CONNECTION_LAN;

	os_config_write_string(szSectionName, "NetworkConnection", internet_connection);

	// deal with the connection speed
	if ( ((CButton *) GetDlgItem(IDC_FAST_CONNECTION))->GetCheck() == 1 )
		connection_speed = CONNECTION_SPEED_FAST;
	else if ( ((CButton *) GetDlgItem(IDC_CABLE_CONNECTION))->GetCheck() == 1 )
		connection_speed = CONNECTION_SPEED_CABLE;
	else if ( ((CButton *) GetDlgItem(IDC_SINGLE_ISDN_CONNECTION))->GetCheck() == 1 )
		connection_speed = CONNECTION_SPEED_SISDN;
	else if ( ((CButton *) GetDlgItem(IDC_56K_CONNECTION))->GetCheck() == 1 )
		connection_speed = CONNECTION_SPEED_56K;
	else if ( ((CButton *) GetDlgItem(IDC_33K_CONNECTION))->GetCheck() == 1 )
		connection_speed = CONNECTION_SPEED_33K;
	else if ( ((CButton *) GetDlgItem(IDC_28K_CONNECTION))->GetCheck() == 1 )
		connection_speed = CONNECTION_SPEED_28K;

	os_config_write_string(szSectionName, "ConnectionSpeed", connection_speed);
	
	CPropertyPage::OnOK();
}

BOOL CNetworkTab::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here
	char *internet_connection, *connection_speed;
	
	internet_connection = os_config_read_string(szSectionName, "NetworkConnection", CONNECTION_DIALUP);
	if (!stricmp(internet_connection, CONNECTION_DIALUP))
		((CButton *) GetDlgItem(IDC_DIALUP))->SetCheck(1);
	else if (!stricmp(internet_connection, CONNECTION_LAN))
		((CButton *) GetDlgItem(IDC_LAN))->SetCheck(1);
	else {
		((CButton *) GetDlgItem(IDC_NONE))->SetCheck(1);
		EnableSpeedGroup(FALSE);
	}

	// set up the connect speed radio buttons
	connection_speed = os_config_read_string(szSectionName, "ConnectionSpeed", CONNECTION_SPEED_56K);
	if ( !stricmp(connection_speed, CONNECTION_SPEED_FAST) )
		((CButton *) GetDlgItem(IDC_FAST_CONNECTION))->SetCheck(1);
	else if ( !stricmp(connection_speed, CONNECTION_SPEED_CABLE) )
		((CButton *) GetDlgItem(IDC_CABLE_CONNECTION))->SetCheck(1);
	else if ( !stricmp(connection_speed, CONNECTION_SPEED_SISDN) )
		((CButton *) GetDlgItem(IDC_SINGLE_ISDN_CONNECTION))->SetCheck(1);
	else if ( !stricmp(connection_speed, CONNECTION_SPEED_56K) )
		((CButton *) GetDlgItem(IDC_56K_CONNECTION))->SetCheck(1);
	else if ( !stricmp(connection_speed, CONNECTION_SPEED_33K) )
		((CButton *) GetDlgItem(IDC_33K_CONNECTION))->SetCheck(1);
	else
		((CButton *) GetDlgItem(IDC_28K_CONNECTION))->SetCheck(1);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CNetworkTab::EnableSpeedGroup(bool bEnable)
{
	HWND hwnd;

	GetDlgItem(IDC_28K_CONNECTION,&hwnd);
	::EnableWindow(hwnd,bEnable);
	GetDlgItem(IDC_33K_CONNECTION,&hwnd);
	::EnableWindow(hwnd,bEnable);
	GetDlgItem(IDC_56K_CONNECTION,&hwnd);
	::EnableWindow(hwnd,bEnable);
	GetDlgItem(IDC_SINGLE_ISDN_CONNECTION,&hwnd);
	::EnableWindow(hwnd,bEnable);
	GetDlgItem(IDC_CABLE_CONNECTION,&hwnd);
	::EnableWindow(hwnd,bEnable);
	GetDlgItem(IDC_FAST_CONNECTION,&hwnd);
	::EnableWindow(hwnd,bEnable);
	GetDlgItem(IDC_CONNECT_SPEED_GROUP,&hwnd);
	::EnableWindow(hwnd,bEnable);
}

void CNetworkTab::OnDialup() 
{
	// TODO: Add your control notification handler code here
	EnableSpeedGroup(TRUE);	
}

void CNetworkTab::OnLan() 
{
	// TODO: Add your control notification handler code here
	EnableSpeedGroup(TRUE);	
}

void CNetworkTab::OnNone() 
{
	// TODO: Add your control notification handler code here
	EnableSpeedGroup(FALSE);	
}

BOOL CNetworkTab::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	// TODO: Add your message handler code here and/or call default
#ifdef USE_HTML_HELP_SYSTEM
	CWaitCursor wc;
	help_launch(NETWORKTAB_HELP);
	return 1;
#else
	return CPropertyPage::OnHelpInfo(pHelpInfo);
#endif
}

// Display the html help file
afx_msg LRESULT CNetworkTab::OnCommandHelp(WPARAM wParam, LPARAM lParam)
{
#ifdef USE_HTML_HELP_SYSTEM
	help_launch(NETWORKTAB_HELP);
	return 1;
#else
	return CPropertyPage::OnCommandHelp(wParam,lParam);
#endif
}

