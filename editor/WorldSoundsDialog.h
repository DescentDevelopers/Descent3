// WorldSoundsDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWorldSoundsDialog dialog

class CWorldSoundsDialog : public CDialog
{
// Construction
public:
	CWorldSoundsDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWorldSoundsDialog)
	enum { IDD = IDD_WORLDSOUNDS };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWorldSoundsDialog)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation

private:

	void UpdateDialog (void);
	void SaveSoundsOnClose();

protected:

	// Generated message map functions
	//{{AFX_MSG(CWorldSoundsDialog)
	afx_msg void OnAddSound();
	afx_msg void OnCheckinSound();
	afx_msg void OnDeleteSound();
	afx_msg void OnLockSound();
	afx_msg void OnLoadSound();
	afx_msg void OnNextSound();
	afx_msg void OnPrevSound();
	afx_msg void OnKillfocusSoundNameEdit();
	afx_msg void OnSelendokSoundPulldown();
	virtual BOOL OnInitDialog();
	afx_msg void OnPlaysound();
	afx_msg void OnKillfocusSoundinnerconeangleEdit();
	afx_msg void OnKillfocusSoundloopendEdit();
	afx_msg void OnKillfocusSoundloopstartEdit();
	afx_msg void OnKillfocusSoundmaxdistEdit();
	afx_msg void OnKillfocusSoundmindistEdit();
	afx_msg void OnKillfocusSoundouterconeangleEdit();
	afx_msg void OnKillfocusSoundouterconevolEdit();
	afx_msg void OnSoundhalleffectCheck();
	afx_msg void OnLoopingCheck();
	afx_msg void OnSoundbackwardRadio();
	afx_msg void OnSounddownwardRadio();
	afx_msg void OnSoundexclusiveCheck();
	afx_msg void OnSoundforeverCheck();
	afx_msg void OnSoundforwardRadio();
	afx_msg void OnSoundobjattachRadio();
	afx_msg void OnSoundobjectRadio();
	afx_msg void OnSoundonceCheck();
	afx_msg void OnSoundposattachRadio();
	afx_msg void OnSoundturret1Radio();
	afx_msg void OnSoundturret2Radio();
	afx_msg void OnSoundturret3Radio();
	afx_msg void OnSoundupwardRadio();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnSoundForceHarwareCheck();
	afx_msg void OnSoundNoUpdate();
	afx_msg void OnKillsounds();
	afx_msg void OnSoundOncePerObjCheck();
	afx_msg void OnKillfocusSoundImportVolumeEdit();
	afx_msg void OnSoundChangeName();
	afx_msg void OnOverride();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
