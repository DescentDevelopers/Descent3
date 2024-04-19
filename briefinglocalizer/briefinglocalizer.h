// briefinglocalizer.h : main header file for the BRIEFINGLOCALIZER application
//

#if !defined(AFX_BRIEFINGLOCALIZER_H__3C009903_F349_11D2_8F49_00104B27BFF0__INCLUDED_)
#define AFX_BRIEFINGLOCALIZER_H__3C009903_F349_11D2_8F49_00104B27BFF0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CBriefinglocalizerApp:
// See briefinglocalizer.cpp for the implementation of this class
//

class CBriefinglocalizerApp : public CWinApp
{
public:
	CBriefinglocalizerApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBriefinglocalizerApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CBriefinglocalizerApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BRIEFINGLOCALIZER_H__3C009903_F349_11D2_8F49_00104B27BFF0__INCLUDED_)
