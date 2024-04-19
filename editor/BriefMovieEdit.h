#if !defined(AFX_BRIEFMOVIEEDIT_H__01F120A3_427B_11D2_B359_006008BF0B09__INCLUDED_)
#define AFX_BRIEFMOVIEEDIT_H__01F120A3_427B_11D2_B359_006008BF0B09__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// BriefMovieEdit.h : header file
//

#include "telcomefxstructs.h"
#include "TelcomEffects.h"

/////////////////////////////////////////////////////////////////////////////
// CBriefMovieEdit dialog

class CBriefMovieEdit : public CDialog
{
// Construction
public:
	CBriefMovieEdit(LPTCMOVIEDESC desc=NULL,CWnd* pParent = NULL);   // standard constructor
	TCMOVIEDESC desc;


// Dialog Data
	//{{AFX_DATA(CBriefMovieEdit)
	enum { IDD = IDD_BRIEF_ADDMOVIE };
	CString	m_sFilename;
	float	m_fFPS;
	BOOL	m_bLooping;
	float	m_fStartTime;
	int		m_iX;
	int		m_iY;
	CString	m_sDesc;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBriefMovieEdit)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CBriefMovieEdit)
	afx_msg void OnBriefMChoose();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnMissionflags();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BRIEFMOVIEEDIT_H__01F120A3_427B_11D2_B359_006008BF0B09__INCLUDED_)
