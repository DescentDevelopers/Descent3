/*
 * $Logfile: /DescentIII/Main/editor/D3XStringEditor.h $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:57:37 $
 * $Author: kevinb $
 *
 * D3XString Editor
 *
 * $Log: not supported by cvs2svn $
 * 
 * 2     9/09/98 12:48p Samir
 * added script localizer.
 * 
 * 1     9/09/98 10:37a Samir
 * Initial revision.
 *
 * $NoKeywords: $
 */

#if !defined(AFX_D3XSTRINGEDITOR_H__627A6E21_47EE_11D2_B2D5_00A0C94B8467__INCLUDED_)
#define AFX_D3XSTRINGEDITOR_H__627A6E21_47EE_11D2_B2D5_00A0C94B8467__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// D3XStringEditor.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CD3XStringEditor dialog

class CD3XStringEditor : public CDialog
{
// Construction
public:
	CD3XStringEditor(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CD3XStringEditor)
	enum { IDD = IDD_LOCALIZESCRIPT };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CD3XStringEditor)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CD3XStringEditor)
	afx_msg void OnLoadscript();
	afx_msg void OnSelchangeStringlist();
	afx_msg void OnDblclkStringlist();
	afx_msg void OnKillfocusStringedit();
	afx_msg void OnSave();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	int m_cursel;													// current selection
	bool m_modified;
	CString m_pathname;

	bool PromptSave();											// returns false if cancelled
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_D3XSTRINGEDITOR_H__627A6E21_47EE_11D2_B2D5_00A0C94B8467__INCLUDED_)
