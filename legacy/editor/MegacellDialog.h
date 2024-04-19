// MegacellDialog.h : header file
//

#include "gr.h"

/////////////////////////////////////////////////////////////////////////////
// CMegacellDialog dialog

class CMegacellDialog : public CDialog
{
// Construction
public:
	CMegacellDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMegacellDialog)
	enum { IDD = IDD_MEGACELL };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMegacellDialog)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation

 private:

	void SaveMegacellsOnClose ();
	void UpdateDialog();
	void UpdateTextureView();
	void UpdateMegacellView();
	void TextureListUp(int);
	void TextureListDown(int);
	void CheckinTexture(int n,int tracklock_num);


	grHardwareSurface m_TextureSurf;
	int m_TexStart;						// Start of texture list.
	int m_TexListPos;					// Current position of textture list.
	int m_TexPageLen;					// length of one view of textures in listbox
	bool m_RedrawListbox;
	int m_CheckX, m_CheckY, m_CheckForTextureList;

protected:

	// Generated message map functions
	//{{AFX_MSG(CMegacellDialog)
	afx_msg void OnNewMegacell();
	afx_msg void OnLockMegacell();
	afx_msg void OnCheckinMegacell();
	afx_msg void OnDeleteMegacell();
	afx_msg void OnNextMegacell();
	afx_msg void OnPreviousMegacell();
	afx_msg void OnSelendokMegacellPulldown();
	afx_msg void OnMegacellsOut();
	afx_msg void OnKillfocusMegacellNameEdit();
	afx_msg void OnKillfocusMegaWidthEdit();
	afx_msg void OnKillfocusMegaHeightEdit();
	virtual BOOL OnInitDialog();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnPaint();
	afx_msg void OnRemapMegacell();
	afx_msg void OnRotateMegacell();
	afx_msg void OnCopyMegacell();
	afx_msg void OnPasteMegacell();
	afx_msg void OnImportTiny();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnImportSky();
	afx_msg void OnImportSkyBand();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
