/*
 * $Logfile: /DescentIII/Main/D3Launch/D3LaunchDlg.h $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:56:51 $
 * $Author: kevinb $
 *
 * 
 *
 * $Log: not supported by cvs2svn $
 * 
 * 28    3/02/99 5:45p Nate
 * Lots of little changes/fixes
 * 
 * 27    2/26/99 12:50p Nate
 * Changed OEM_Voodoo3 names
 * 
 * 26    2/24/99 8:37p Nate
 * Various little dialog changes, added "Install Drivers" dialog
 * 
 * 25    2/24/99 1:46p Nate
 * Added multi-language support
 * 
 * 24    2/15/99 1:41p Nate
 * Added DirectX installation through DirectSetup
 * 
 * 23    1/14/99 11:06a Nate
 * Added tournament prompt dialog.
 * 
 * 22    11/30/98 3:01p Nate
 * The dedicated server can now be run without a video card selected.
 * 
 * 21    11/06/98 4:56p Nate
 * User can now select highlighted button by pressing Enter.
 * 
 * 20    11/05/98 2:16p Nate
 * Added keyboard support
 * 
 * 19    10/18/98 5:53p Nate
 * 
 * 18    10/10/98 2:33p Nate
 * More fixes.
 * 
 * 17    10/02/98 7:09p Nate
 * Added animation button
 * 
 * 16    9/21/98 5:40p Nate
 * Incorporated the new HTML help system
 * 
 * 15    9/16/98 3:23p Nate
 * Added straight to setup processing
 * 
 * 14    9/13/98 2:38p Nate
 * 
 * 13    9/10/98 5:17p Nate
 * Added text message capability to bitmap buttons
 * 
 * 12    9/04/98 10:18a Nate
 * Made it so light won't flicker for a certain time after startup.
 * 
 * 11    9/03/98 6:57p Nate
 * Fixed StretchBlt() problem by doing some 256 color conversions
 * 
 * 10    9/02/98 6:42p Nate
 * Added improved sound support.
 * 
 * 9     9/01/98 7:15p Nate
 * Major Revision #2
 * 
 * 8     8/31/98 6:44p Nate
 * Major Revision
 * 
 * 7     8/25/98 6:15p Nate
 * Added the Message of the Day to the Auto Update Dialog
 * 
 * 6     8/24/98 7:06p Nate
 * Added new AutoUpdate features, and fixed display glitches
 * 
 * 5     8/17/98 10:35a Nate
 * Added Keyboard Type page to Setup
 * 
 * 4     8/12/98 9:21a Nate
 * Fixed "gray-button-background" flicker
 * 
 * 3     8/10/98 10:44a Nate
 * Added Language selection support
 * 
 * 2     8/05/98 11:54a Nate
 * Initial Version
 *
 * $NoKeywords: $
 */

// D3LaunchDlg.h : header file
//

#if !defined(AFX_D3LAUNCHDLG_H__9149CF3A_207D_11D2_8CBD_00A0C96ED60D__INCLUDED_)
#define AFX_D3LAUNCHDLG_H__9149CF3A_207D_11D2_8CBD_00A0C96ED60D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CD3LaunchDlg dialog
#include "BitmapButtonEx.h"

// Main timer delay (in msecs)
#define MAIN_TIMER_DELAY	50

// Launcher shutdown code
#define IDSHUTDOWN				-1

// Time between animation frames (in msecs)
#define MAX_ANIM_FRAME_TIME		100

// Number of animation frames
#define NUM_ANIM_FRAMES			9

// Chance of flicker is 1 in [ODDS VALUE]
#define LIGHT_FLICKER_OFF_ODDS	40

// Time light is off during flicker (in msecs)
#define MAX_LIGHT_OFF_TIME		100

// Delay time for light (it can't flicker for this long during startup)
#define STARTUP_DELAY_TIME		1500

#define MOUSE_TIMER_ID			1

#define NUM_BITMAP_BUTTONS		10


class CD3LaunchDlg : public CDialog
{
// Construction
public:
	CD3LaunchDlg(CWnd* pParent = NULL);		// standard constructor
	bool MouseOverButton(CButton *button);	// checks if mouse is over button
	void RedrawButton(CButton *button);		// marks button region for redraw
	void RefreshDialog(void);					// sets up launcher for new language
	bool SetLanguageDLL(int lang_type);		// sets up the new language resource DLL

	void PlaySound(LPSTR lpName, bool OnlyPlayIfActiveWnd, bool WaitUntilDone, bool WaitForStartup=TRUE);	// Plays a sound

	void HideButton(CBitmapButtonEx *button, bool mHide);	// Hides (or un-hides) a button

	void DisplayTournamentPrompt(void);		// displays tournament contest web link prompt

	int launcher_get_CD_path(char *path, int prompt_for_cd, char *subdir);
	int find_cd_drive(char *subdir);

	void InstallDirectX(void);					// runs DirectXSetup from CD#1

	void RunOpenGLSetup(void);					// runs the GLSetup executable from CD#1

	//CDC m_dcMem;		// Compatible memory DC for dialog
	//CBrush m_brush;		// Handle of hollow brush
	//CBrush m_bkBrush;	// Handle of background brush
	BITMAP m_bmInfo;	// Bitmap info 
	CPoint m_pt;		// Position of bitmap
	CSize m_size;		// Size of bitmap

	// Define all "pushable" bitmap buttons
	CBitmapButtonEx m_Buttons[NUM_BITMAP_BUTTONS];

	// Keep blinking light bitmap separate
	CBitmapButtonEx m_Light;

	// Keep animation bitmap separate
	CBitmapButtonEx m_Anim;

	// Timer ID variable
	UINT m_timerID;

// Dialog Data
	//{{AFX_DATA(CD3LaunchDlg)
	enum { IDD = IDD_D3LAUNCH_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CD3LaunchDlg)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CD3LaunchDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnBtnQuit();
	afx_msg void OnBtnSetup();
	afx_msg void OnBtnWebpage();
	afx_msg void OnBtnReadme();
	afx_msg void OnBtnPlay();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnBtnUpdate();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnBtnPxo();
	afx_msg void OnBtnUninstall();
	afx_msg void OnBtnLight();
	afx_msg void OnBtnDirectx();
	afx_msg BOOL OnQueryNewPalette();
	afx_msg void OnPaletteChanged( CWnd* pFocusWnd );
	afx_msg LONG OnStraightToSetup(UINT, LONG);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg LRESULT OnCommandHelp(WPARAM wParam, LPARAM lParam);
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

extern CString szTitle;
extern CD3LaunchDlg *pLaunchDlg;

// pass this message to myself when initializing if I should go straight to the
// update dialog
#define WM_STRAIGHT_TO_SETUP				(WM_USER + 10)

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_D3LAUNCHDLG_H__9149CF3A_207D_11D2_8CBD_00A0C96ED60D__INCLUDED_)
