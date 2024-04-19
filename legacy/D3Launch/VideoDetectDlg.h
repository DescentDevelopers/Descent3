#if !defined(AFX_VIDEODETECTDLG_H__11362FC3_61E3_11D2_A4E0_00A0C96ED60D__INCLUDED_)
#define AFX_VIDEODETECTDLG_H__11362FC3_61E3_11D2_A4E0_00A0C96ED60D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VideoDetectDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CVideoDetectDlg dialog

class CVideoDetectDlg : public CDialog
{
// Construction
public:
	CVideoDetectDlg(CWnd* pParent = NULL);   // standard constructor

	int DetectOpenGL;
	int DetectDirect3D;
	int DetectGlide;

// Dialog Data
	//{{AFX_DATA(CVideoDetectDlg)
	enum { IDD = IDD_VIDEODETECT_DLG };
	CButton	m_OpenGLCheck;
	CButton	m_GlideCheck;
	CButton	m_Direct3DCheck;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVideoDetectDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CVideoDetectDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIDEODETECTDLG_H__11362FC3_61E3_11D2_A4E0_00A0C96ED60D__INCLUDED_)
