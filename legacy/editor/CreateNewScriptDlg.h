#if !defined(AFX_CREATENEWSCRIPTDLG_H__7CFE839E_90E9_11D2_AB2B_006008BF0B09__INCLUDED_)
#define AFX_CREATENEWSCRIPTDLG_H__7CFE839E_90E9_11D2_AB2B_006008BF0B09__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CreateNewScriptDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCreateNewScriptDlg dialog

class CCreateNewScriptDlg : public CDialog
{
// Construction
public:
	CCreateNewScriptDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCreateNewScriptDlg)
	enum { IDD = IDD_CREATESCRIPT };
	CString	m_Filename;
	int		m_ScriptType;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCreateNewScriptDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCreateNewScriptDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CREATENEWSCRIPTDLG_H__7CFE839E_90E9_11D2_AB2B_006008BF0B09__INCLUDED_)
