/*
 * $Logfile: /DescentIII/Main/editor/TableFileFilterAddDlg.h $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:57:39 $
 * $Author: kevinb $
 *
 * 
 *
 * $Log: not supported by cvs2svn $
 * 
 * 3     10/08/98 10:30a Nate
 * Initial version
 *
 * $NoKeywords: $
 */

#if !defined(AFX_TABLEFILEFILTERADDDLG_H__432EBD82_5D4C_11D2_A4E0_00A0C96ED60D__INCLUDED_)
#define AFX_TABLEFILEFILTERADDDLG_H__432EBD82_5D4C_11D2_A4E0_00A0C96ED60D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TableFileFilterAddDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTableFileFilterAddDlg dialog

class CTableFileFilterAddDlg : public CDialog
{
// Construction
public:
	CTableFileFilterAddDlg(CWnd* pParent = NULL);   // standard constructor

	CString page_name;
	int		page_type;

// Dialog Data
	//{{AFX_DATA(CTableFileFilterAddDlg)
	enum { IDD = IDD_TABLEFILEFILTER_ADDPAGE_DLG };
	CComboBox	m_TypeCombo;
	CEdit	m_NameEdit;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTableFileFilterAddDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTableFileFilterAddDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABLEFILEFILTERADDDLG_H__432EBD82_5D4C_11D2_A4E0_00A0C96ED60D__INCLUDED_)
