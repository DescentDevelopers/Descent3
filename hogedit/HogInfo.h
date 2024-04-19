/*
 * $Logfile: /DescentIII/Main/hogedit/HogInfo.h $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:57:56 $
 * $Author: kevinb $
 *
 * 
 *
 * $Log: not supported by cvs2svn $
 * 
 * 2     8/16/98 4:22p Nate
 * Added message deferal and new hog info dialog
 *
 * $NoKeywords: $
 */

#if !defined(AFX_HOGINFO_H__2BF70B62_351D_11D2_8CBD_00A0C96ED60D__INCLUDED_)
#define AFX_HOGINFO_H__2BF70B62_351D_11D2_8CBD_00A0C96ED60D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// HogInfo.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CHogInfo dialog

class CHogInfo : public CDialog
{
// Construction
public:
	CHogInfo(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CHogInfo)
	enum { IDD = IDD_HOGINFO };
	CString	m_NumFiles;
	CString	m_Location;
	CString	m_SizeBytes;
	CString	m_SizeMB;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHogInfo)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CHogInfo)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HOGINFO_H__2BF70B62_351D_11D2_8CBD_00A0C96ED60D__INCLUDED_)
