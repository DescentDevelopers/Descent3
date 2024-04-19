#if !defined(AFX_RAWCVTDLG_H__D94CC63D_1CA3_11D2_AF2D_0060089A8025__INCLUDED_)
#define AFX_RAWCVTDLG_H__D94CC63D_1CA3_11D2_AF2D_0060089A8025__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// RawCvtDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRawCvtDlg dialog

class CRawCvtDlg : public CDialog
{
// Construction
public:
	int m_Xforms;
	CRawCvtDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CRawCvtDlg)
	enum { IDD = IDD_RAWCVTDLG };
	int		m_Frequency;
	int		m_Resolution;
	int		m_Channels;
	CString	m_VolScale;
	CString	m_StreamName;
	int		m_MeasureSize;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRawCvtDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual void OnOK();

	// Generated message map functions
	//{{AFX_MSG(CRawCvtDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RAWCVTDLG_H__D94CC63D_1CA3_11D2_AF2D_0060089A8025__INCLUDED_)
