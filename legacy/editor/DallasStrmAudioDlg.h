#if !defined(AFX_DALLASSTRMAUDIODLG_H__5D495B20_C7F1_11D2_A4E0_00A0C96ED60D__INCLUDED_)
#define AFX_DALLASSTRMAUDIODLG_H__5D495B20_C7F1_11D2_A4E0_00A0C96ED60D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DallasStrmAudioDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDallasStrmAudioDlg dialog

class CDallasStrmAudioDlg : public CDialog
{
// Construction
public:
	CString	m_Filename;

	CDallasStrmAudioDlg(CWnd* pParent = NULL);   // standard constructor

	void FillStrmAudioList(void);

// Dialog Data
	//{{AFX_DATA(CDallasStrmAudioDlg)
	enum { IDD = IDD_DALLAS_STRM_AUDIO_DIALOG };
	CListBox	m_StrmAudioBox;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDallasStrmAudioDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDallasStrmAudioDlg)
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkStrmAudioList();
	afx_msg void OnPlayStrmAudioButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DALLASSTRMAUDIODLG_H__5D495B20_C7F1_11D2_A4E0_00A0C96ED60D__INCLUDED_)
