#if !defined(AFX_PLAYSONGDLG_H__932821D1_1D86_11D2_B2B3_00A0C94B8467__INCLUDED_)
#define AFX_PLAYSONGDLG_H__932821D1_1D86_11D2_B2B3_00A0C94B8467__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PlaySongDlg.h : header file
//

#include "streamaudio.h"

/////////////////////////////////////////////////////////////////////////////
// CPlaySongDlg dialog


class CPlaySongDlg : public CDialog
{
// Construction
public:
	AudioStream m_Stream;
	CString m_StreamName;
	CPlaySongDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPlaySongDlg)
	enum { IDD = IDD_PLAYSONG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPlaySongDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPlaySongDlg)
	afx_msg void OnTimer(UINT nIDEvent);
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLAYSONGDLG_H__932821D1_1D86_11D2_B2B3_00A0C94B8467__INCLUDED_)
