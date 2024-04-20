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

// briefinglocalizer.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "briefinglocalizer.h"
#include "briefinglocalizerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBriefinglocalizerApp

BEGIN_MESSAGE_MAP(CBriefinglocalizerApp, CWinApp)
	//{{AFX_MSG_MAP(CBriefinglocalizerApp)
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBriefinglocalizerApp construction

CBriefinglocalizerApp::CBriefinglocalizerApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CBriefinglocalizerApp object

CBriefinglocalizerApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CBriefinglocalizerApp initialization

BOOL CBriefinglocalizerApp::InitInstance()
{
	// Standard initialization

	SetRegistryKey(_T("Descent 3 Briefing Localizer"));
	AfxInitRichEdit( );


#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	CBriefinglocalizerDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
	}
	else if (nResponse == IDCANCEL)
	{
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
