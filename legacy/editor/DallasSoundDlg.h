#if !defined(AFX_DALLASSOUNDDLG_H__54A05001_AEE2_11D2_A4E0_00A0C96ED60D__INCLUDED_)
#define AFX_DALLASSOUNDDLG_H__54A05001_AEE2_11D2_A4E0_00A0C96ED60D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DallasSoundDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDallasSoundDlg dialog

class CDallasSoundDlg : public CDialog
{
// Construction
public:

	CString	m_SoundName;
	int		m_SoundIndex;


	CDallasSoundDlg(CWnd* pParent = NULL);   // standard constructor

	void FillSoundList(void);

// Dialog Data
	//{{AFX_DATA(CDallasSoundDlg)
	enum { IDD = IDD_DALLAS_SOUND_DIALOG };
	CListBox	m_SoundList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDallasSoundDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDallasSoundDlg)
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnPlaySoundButton();
	afx_msg void OnStopSoundsButton();
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkSoundList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DALLASSOUNDDLG_H__54A05001_AEE2_11D2_A4E0_00A0C96ED60D__INCLUDED_)
