// musicutils.h : main header file for the MUSICUTILS application
//

#if !defined(AFX_MUSICUTILS_H__D94CC633_1CA3_11D2_AF2D_0060089A8025__INCLUDED_)
#define AFX_MUSICUTILS_H__D94CC633_1CA3_11D2_AF2D_0060089A8025__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CMusicutilsApp:
// See musicutils.cpp for the implementation of this class
//

class CMusicutilsApp : public CWinApp
{
public:
	CMusicutilsApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMusicutilsApp)
	public:
	virtual BOOL InitInstance();
	virtual BOOL OnIdle(LONG lCount);
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CMusicutilsApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


class oeApplication;

extern oeApplication *MusicUtils;
extern CMusicutilsApp theApp;


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MUSICUTILS_H__D94CC633_1CA3_11D2_AF2D_0060089A8025__INCLUDED_)
