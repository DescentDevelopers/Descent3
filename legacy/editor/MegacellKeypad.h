// MegacellKeypad.h : header file
//

#include "gr.h"
#include "KeypadDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CMegacellKeypad dialog

class CMegacellKeypad : public CKeypadDialog
{
// Construction
public:
	CMegacellKeypad(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMegacellKeypad)
	enum { IDD = IDD_MEGAKEYPAD };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMegacellKeypad)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMegacellKeypad)
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnNextMegaSet();
	afx_msg void OnPrevMegaSet();
	afx_msg void OnPaint();
	afx_msg void OnFillWithMegacell();
	afx_msg void OnRandomizeMegacellCheck();
	afx_msg void OnSelectFromMegacell();
	afx_msg void OnUnifyMegacell();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnKillfocusXGranularEdit();
	afx_msg void OnKillfocusYGranularEdit();
	afx_msg void OnHighlightBorders();
	afx_msg void OnMegacellSkydome();
	afx_msg void OnMegacellSkyband();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
//	loads and saves settings for this keypad
	void LoadSettings();
	void SaveSettings();

	void UpdateDialog();
	void UpdateMegaSetView();
	grHardwareSurface m_TextureSurf;
	int m_xgran,m_ygran;

	static int m_Instances;				// number of instances of this keypad.
};
