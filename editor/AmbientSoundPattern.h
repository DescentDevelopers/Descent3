#if !defined(AFX_AMBIENTSOUNDPATTERN_H__D3451741_3680_11D2_A13A_0060089A8026__INCLUDED_)
#define AFX_AMBIENTSOUNDPATTERN_H__D3451741_3680_11D2_A13A_0060089A8026__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// AmbientSoundPattern.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAmbientSoundPattern dialog

class CAmbientSoundPattern : public CDialog
{
// Construction
public:
	CAmbientSoundPattern(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAmbientSoundPattern)
	enum { IDD = IDD_AMBIENTSOUNDPATTERNS };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAmbientSoundPattern)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAmbientSoundPattern)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelendokASPList();
	afx_msg void OnASPCheckIn();
	afx_msg void OnASPDelete();
	afx_msg void OnASPEditElement();
	afx_msg void OnASPLock();
	afx_msg void OnASPNew();
	afx_msg void OnASPNewElement();
	afx_msg void OnASPRename();
	afx_msg void OnSelchangeASPElementList();
	afx_msg void OnASPDeleteElement();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnASPUndoLock();
	afx_msg void OnKillfocusASPMaxDelay();
	afx_msg void OnKillfocusASPMinDelay();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void UpdateDialog();

	bool m_locked;
	int m_current_asp;
	int m_current_element;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AMBIENTSOUNDPATTERN_H__D3451741_3680_11D2_A13A_0060089A8026__INCLUDED_)
