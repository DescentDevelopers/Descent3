/*
 * $Logfile: /DescentIII/Main/editor/PreferencesDialog.h $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:57:38 $
 * $Author: kevinb $
 *
 * Preferences
 *
 * $Log: not supported by cvs2svn $
 * 
 * 7     4/02/99 11:33p Jeff
 * able to turn off IGC in debug mode.  Better handling of cinematics
 * quick exit if playing via alt-p
 * 
 * 6     3/15/99 9:24p Gwar
 * 
 * 5     7/16/98 12:06p Jason
 * added special flags to room structure
 * 
 * 4     3/19/98 6:57p Jeff
 * Added in a default pilot section for Preferences
 * 
 * 3     2/14/98 10:48p Jason
 * got preferred rendering working
 * 
 * 2     6/05/97 6:10p Jason
 * added support for zbuffering and bilinear filtering
 * 
 * 1     2/05/97 11:58a Samir
 * 
 * 2     2/05/97 11:45a Samir
 * Baisc functionality for D3windowed toggle
 *
 * $NoKeywords: $
 */

/////////////////////////////////////////////////////////////////////////////
// CPreferencesDialog dialog

class CPreferencesDialog : public CDialog
{
// Construction
public:
	CPreferencesDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPreferencesDialog)
	enum { IDD = IDD_PREFERENCES };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPreferencesDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPreferencesDialog)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnBilinearCheck();
	afx_msg void OnWindowed();
	afx_msg void OnFullScreenHW();
	afx_msg void OnFullScreenSW();
	afx_msg void OnMippingCheck();
	afx_msg void OnIgcEnabled();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void EnableHardwareOptions();
	void DisableHardwareOptions();
};
