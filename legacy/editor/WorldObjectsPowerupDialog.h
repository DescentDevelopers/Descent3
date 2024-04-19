// WorldObjectsPowerupDialog.h : header file
//

#include "gr.h"

/////////////////////////////////////////////////////////////////////////////
// CWorldObjectsPowerupDialog dialog

class CWorldObjectsPowerupDialog : public CDialog
{
// Construction
public:
	CWorldObjectsPowerupDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWorldObjectsPowerupDialog)
	enum { IDD = IDD_WORLDOBJECTSPOWERUP };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWorldObjectsPowerupDialog)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

private:
	void UpdateDialog();
	void SavePowerupsOnClose();
	void UpdatePowerupView();

	grHardwareSurface m_PowerupSurf;

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWorldObjectsPowerupDialog)
	afx_msg void OnAddNewPowerup();
	afx_msg void OnCheckin();
	afx_msg void OnDeletePowerup();
	afx_msg void OnLockPowerup();
	afx_msg void OnNextPowerup();
	afx_msg void OnPrev();
	afx_msg void OnPowerupsOut();
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnKillfocusPowerNameEdit();
	afx_msg void OnKillfocusPowerupSizeEdit();
	afx_msg void OnSelendokPowerPulldown();
	afx_msg void OnLoadImage();
	afx_msg void OnGravityCheck();
	afx_msg void OnMagnetismCheck();
	afx_msg void OnWindCheck();
	afx_msg void OnKillfocusPowMassEdit();
	afx_msg void OnKillfocusPowDragEdit();
	afx_msg void OnSelendokPowerupAmbientSoundList();
	afx_msg void OnSelendokPowerupPickupSoundList();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
