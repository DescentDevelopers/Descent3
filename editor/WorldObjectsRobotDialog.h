// WorldObjectsRobotDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWorldObjectsRobotDialog dialog

class CWorldObjectsRobotDialog : public CDialog
{
// Construction
public:
	CWorldObjectsRobotDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWorldObjectsRobotDialog)
	enum { IDD = IDD_WORLDOBJECTSROBOT };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWorldObjectsRobotDialog)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation

private:

	void UpdateDialog (void);
	void SaveRobotsOnClose();
	void UpdateRobotView();

	grHardwareSurface m_RobotSurf;

protected:

	// Generated message map functions
	//{{AFX_MSG(CWorldObjectsRobotDialog)
	afx_msg void OnAddNewRobot();
	afx_msg void OnCheckinRobot();
	afx_msg void OnDeleteRobot();
	afx_msg void OnLockRobot();
	afx_msg void OnNextRobot();
	afx_msg void OnPrevRobot();
	afx_msg void OnRobotModel();
	afx_msg void OnKillfocusRobotNameEdit();
	afx_msg void OnSelendokRobotPulldown();
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnKillfocusRobotSizeEdit();
	afx_msg void OnRobotsOut();
	afx_msg void OnDefineAnimstates();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnRobotEditPhysics();
	afx_msg void OnRobotUsesPhysics();
	afx_msg void OnRobotUsesAI();
	afx_msg void OnRobotEditAI();
	afx_msg void OnKillfocusScriptid();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
