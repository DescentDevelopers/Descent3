/*
 * $Logfile: /DescentIII/Main/D3Launch/NetworkTab.h $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:56:51 $
 * $Author: kevinb $
 *
 * 
 *
 * $Log: not supported by cvs2svn $
 * 
 * 3     9/21/98 5:40p Nate
 * Incorporated the new HTML help system
 * 
 * 2     8/05/98 11:54a Nate
 * Initial Version
 *
 * $NoKeywords: $
 */

#if !defined(AFX_NETWORKTAB_H__FAB3D184_22D6_11D2_8CBD_00A0C96ED60D__INCLUDED_)
#define AFX_NETWORKTAB_H__FAB3D184_22D6_11D2_8CBD_00A0C96ED60D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// NetworkTab.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNetworkTab dialog

class CNetworkTab : public CPropertyPage
{
	DECLARE_DYNCREATE(CNetworkTab)

// Construction
public:
	CNetworkTab();
	~CNetworkTab();
	void EnableSpeedGroup(bool bEnable);

// Dialog Data
	//{{AFX_DATA(CNetworkTab)
	enum { IDD = IDD_PROPPAGE_NETWORK };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CNetworkTab)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CNetworkTab)
	virtual BOOL OnInitDialog();
	afx_msg void OnDialup();
	afx_msg void OnLan();
	afx_msg void OnNone();
	afx_msg LRESULT OnCommandHelp(WPARAM wParam, LPARAM lParam);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NETWORKTAB_H__FAB3D184_22D6_11D2_8CBD_00A0C96ED60D__INCLUDED_)
