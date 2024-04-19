// WorldObjectsDoorDialog.h : header file
//

#include "gr.h"

/////////////////////////////////////////////////////////////////////////////
// CWorldObjectsDoorDialog dialog

class CWorldObjectsDoorDialog : public CDialog
{
// Construction
public:
	CWorldObjectsDoorDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWorldObjectsDoorDialog)
	enum { IDD = IDD_WORLDOBJECTSDOOR };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWorldObjectsDoorDialog)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation

private:

	void UpdateDialog (void);
	void SaveDoorsOnClose();
	void UpdateDoorView();

	grHardwareSurface m_DoorSurf;

protected:

	// Generated message map functions
	//{{AFX_MSG(CWorldObjectsDoorDialog)
	afx_msg void OnAddDoor();
	afx_msg void OnCheckinDoor();
	afx_msg void OnKillfocusCloseTime();
	afx_msg void OnDeleteDoor();
	afx_msg void OnDoorNext();
	afx_msg void OnKillfocusDoorOpenTime();
	afx_msg void OnDoorPrev();
	afx_msg void OnKillfocusDoorStaysOpen();
	afx_msg void OnDoorsOut();
	afx_msg void OnLoadDoorModel();
	afx_msg void OnLockDoor();
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSelendokDoorPulldown();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnDoorChangeName();
	afx_msg void OnTransparency();
	afx_msg void OnDestroy();
	afx_msg void OnSelendokDoorCloseSound();
	afx_msg void OnSelendokDoorOpenSound();
	afx_msg void OnBrowse();
	afx_msg void OnKillfocusScriptname();
	afx_msg void OnDoorBlastable();
	afx_msg void OnKillfocusDoorHitpointsEdit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
