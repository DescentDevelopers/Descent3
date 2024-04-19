/*
 * $Logfile: /DescentIII/Main/D3Launch/DirectXTab.h $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:56:51 $
 * $Author: kevinb $
 *
 * 
 *
 * $Log: not supported by cvs2svn $
 * 
 * 4     9/21/98 5:40p Nate
 * Incorporated the new HTML help system
 * 
 * 3     9/01/98 7:15p Nate
 * Major Revision #2
 * 
 * 2     8/31/98 6:44p Nate
 * Major Revision
 *
 * $NoKeywords: $
 */

#if !defined(AFX_DIRECTXTAB_H__42877762_40B8_11D2_8CBD_00A0C96ED60D__INCLUDED_)
#define AFX_DIRECTXTAB_H__42877762_40B8_11D2_8CBD_00A0C96ED60D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DirectXTab.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDirectXTab dialog

class CDirectXTab : public CPropertyPage
{
	DECLARE_DYNCREATE(CDirectXTab)

// Construction
public:
	CDirectXTab();
	~CDirectXTab();

// Dialog Data
	//{{AFX_DATA(CDirectXTab)
	enum { IDD = IDD_PROPPAGE_DIRECTX };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CDirectXTab)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDirectXTab)
	virtual BOOL OnInitDialog();
	afx_msg void OnGetDx();
	afx_msg LRESULT OnCommandHelp(WPARAM wParam, LPARAM lParam);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIRECTXTAB_H__42877762_40B8_11D2_8CBD_00A0C96ED60D__INCLUDED_)
