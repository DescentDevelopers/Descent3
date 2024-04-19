/*
 * $Logfile: /descent3/main/editor/GrWnd.h $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:57:38 $
 * $Author: kevinb $
 *
 * Base class of all GrWnds. 
 *
 * $Log: not supported by cvs2svn $
 * 
 * 10    2/03/97 5:58p Matt
 * Added missing include
 * 
 * 9     1/30/97 6:26p Samir
 * Attempt to implement change in caption bar drawing.
 * 
 * 8     1/23/97 4:56p Samir
 * Added Static window style constant
 * 
 * 7     1/21/97 12:47p Samir
 * Added OnSize and GRWND_STYLE constant to adjust window look. 
 *
 * $NoKeywords: $
 */


#ifndef M_GRWND_H
#define M_GRWND_H

#include "gr.h"
#include "stdafx.h"

const DWORD GRWND_STYLE = WS_VISIBLE | WS_CHILD | WS_CAPTION | WS_CLIPSIBLINGS |
							WS_THICKFRAME | WS_SYSMENU;
const DWORD GRWND_STATIC_STYLE = WS_VISIBLE | WS_CHILD | WS_BORDER;


/////////////////////////////////////////////////////////////////////////////
// CGrWnd window

class CGrWnd : public CWnd
{
// Construction
public:
	CGrWnd(const char *name);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGrWnd)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CGrWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CGrWnd)
	afx_msg void OnDestroy();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnMove(int x, int y);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnNcActivate(BOOL bActive);
	afx_msg void OnNcPaint();
	afx_msg void OnChildActivate();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

//	Variables
protected:
	grScreen *m_grScreen;				// screen belonging to window
	grViewport *m_grViewport;			// Viewport belonging to window
	char m_Name[32];					// screen window name

	BOOL m_grCreated;					// Create/Destroyed window
	BOOL m_Active;						// is it active?

private:
	void DrawCaption(BOOL active);
};

/////////////////////////////////////////////////////////////////////////////
#endif
