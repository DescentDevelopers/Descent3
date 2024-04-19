#if !defined(AFX_DALLASIMPORTDLG_H__B9D24521_BC4B_11D2_A4E0_00A0C96ED60D__INCLUDED_)
#define AFX_DALLASIMPORTDLG_H__B9D24521_BC4B_11D2_A4E0_00A0C96ED60D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DallasImportDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDallasImportDlg dialog

class CDallasImportDlg : public CDialog
{
// Construction
public:
	CDallasImportDlg(CWnd* pParent = NULL);   // standard constructor

	CString	m_ScriptName;
	CString	m_Filename;

	BOOL FillScriptListBox(char *filename);

// Dialog Data
	//{{AFX_DATA(CDallasImportDlg)
	enum { IDD = IDD_DALLAS_IMPORT_DIALOG };
	CListBox	m_ScriptListBox;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDallasImportDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDallasImportDlg)
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DALLASIMPORTDLG_H__B9D24521_BC4B_11D2_A4E0_00A0C96ED60D__INCLUDED_)
