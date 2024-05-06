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

 * $Logfile: /DescentIII/Main/D3Launch/DirectXTab.cpp $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:56:51 $
 * $Author: kevinb $
 *
 * 
 *
 * $Log: not supported by cvs2svn $
 * 
 * 9     2/16/99 6:09p Nate
 * Changed DX version output from V.0 to V.x
 * 
 * 8     10/14/98 5:52p Nate
 * More fixes
 * 
 * 7     10/08/98 6:23p Nate
 * Fixed a few bugs.
 * 
 * 6     9/22/98 3:33p Nate
 * Added conditional compiling to help system (can toggle between HTML and
 * standard windows help)
 * 
 * 5     9/21/98 5:40p Nate
 * Incorporated the new HTML help system
 * 
 * 4     9/13/98 2:38p Nate
 * Changed button to launch DirectX download webpage.
 * 
 * 3     9/01/98 7:15p Nate
 * Major Revision #2
 * 
 * 2     8/31/98 6:44p Nate
 * Major Revision
 *
 * $NoKeywords: $
 */

// DirectXTab.cpp : implementation file
//

#include "stdafx.h"
#include "afxpriv.h"
#include "afxext.h"
#include "d3launch.h"
#include "DirectXTab.h"
#include "PsTypes.h"
#include <direct.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern HINSTANCE Dd_dll_handle;

/////////////////////////////////////////////////////////////////////////////
// CDirectXTab property page

IMPLEMENT_DYNCREATE(CDirectXTab, CPropertyPage)

CDirectXTab::CDirectXTab() : CPropertyPage(CDirectXTab::IDD)
{
	//{{AFX_DATA_INIT(CDirectXTab)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CDirectXTab::~CDirectXTab()
{
}

void CDirectXTab::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDirectXTab)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDirectXTab, CPropertyPage)
	//{{AFX_MSG_MAP(CDirectXTab)
	ON_BN_CLICKED(IDC_GET_DX, OnGetDx)
	ON_MESSAGE(WM_COMMANDHELP,OnCommandHelp)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDirectXTab message handlers

BOOL CDirectXTab::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here
	LONG lResult;
	HKEY hKey = NULL;

	if (Dd_dll_handle) {
		lResult = RegOpenKeyEx(
			HKEY_LOCAL_MACHINE,					// Where it is
			"Software\\Microsoft\\DirectX",	// name of key
			NULL,										// DWORD reserved
			KEY_QUERY_VALUE,						// Allows all changes
			&hKey										// Location to store key
		);

		if (lResult == ERROR_SUCCESS) {
			char version[32];
			DWORD dwType, dwLen;

			dwLen = 32;
			lResult = RegQueryValueEx(
				hKey,									// Handle to key
				"Version",							// The values name
				NULL,									// DWORD reserved
				&dwType,								// What kind it is
				(ubyte *) version, 				// value to set
				&dwLen								// How many bytes to set
			);

			if (lResult == ERROR_SUCCESS) {
				CString str;

				Dx_version = atoi(strstr(version, ".") + 1);
				str.Format("%d.x (%s)", Dx_version, version);
				GetDlgItem(IDC_DX_VERSION)->SetWindowText(str);

			} else {
				int val;
				DWORD dwType, dwLen;

				dwLen = 4;
				lResult = RegQueryValueEx(
					hKey,									// Handle to key
					"InstalledVersion",				// The values name
					NULL,									// DWORD reserved
					&dwType,								// What kind it is
					(ubyte *) &val,					// value to set
					&dwLen								// How many bytes to set
				);

				if (lResult == ERROR_SUCCESS) {
					CString str;

					Dx_version = val;
					str.Format("%d.x", Dx_version);
					GetDlgItem(IDC_DX_VERSION)->SetWindowText(str);

				}
			}

			RegCloseKey(hKey);
		}
	}

	if (Dx_version >= 6) {
		CString msg;
		msg.LoadString(IDS_DIRECTXTAB_OK);
		GetDlgItem(IDC_DX_STATUS)->SetWindowText(msg.GetBuffer(0));
	} 
	else if (Dx_version >= 3) {
		CString msg;
		msg.LoadString(IDS_DIRECTXTAB_UPD_REC);
		GetDlgItem(IDC_DX_STATUS)->SetWindowText(msg.GetBuffer(0));
	} 
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


// Takes user to DirectX web page
void CDirectXTab::OnGetDx() 
{
	url_launch("http://www.microsoft.com/directx/download.asp");	
}


void CDirectXTab::OnOK() 
{
	// TODO: Add your specialized code here and/or call the base class

	CPropertyPage::OnOK();
}

BOOL CDirectXTab::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	// TODO: Add your message handler code here and/or call default
#ifdef USE_HTML_HELP_SYSTEM
	CWaitCursor wc;
	help_launch(DIRECTXTAB_HELP);
	return 1;	
#else
	return CPropertyPage::OnHelpInfo(pHelpInfo);
#endif
}

// Display the html help file
afx_msg LRESULT CDirectXTab::OnCommandHelp(WPARAM wParam, LPARAM lParam)
{
#ifdef USE_HTML_HELP_SYSTEM
	help_launch(DIRECTXTAB_HELP);
	return 1;
#else
	return CPropertyPage::OnCommandHelp(wParam,lParam);
#endif
}
