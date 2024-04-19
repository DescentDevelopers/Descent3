#if !defined(AFX_BRIEFSOUNDEDIT_H__01F120A5_427B_11D2_B359_006008BF0B09__INCLUDED_)
#define AFX_BRIEFSOUNDEDIT_H__01F120A5_427B_11D2_B359_006008BF0B09__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// BriefSoundEdit.h : header file
//
#include "telcomefxstructs.h"
#include "TelComEffects.h"

/////////////////////////////////////////////////////////////////////////////
// CBriefSoundEdit dialog

class CBriefSoundEdit : public CDialog
{
// Construction
public:
	CBriefSoundEdit(LPTCSNDDESC desc=NULL,CWnd* pParent = NULL);   // standard constructor
	TCSNDDESC desc;

// Dialog Data
	//{{AFX_DATA(CBriefSoundEdit)
	enum { IDD = IDD_BRIEF_ADDSOUND };
	CString	m_sFilename;
	BOOL	m_bPlayOnce;
	float	m_fStartTime;
	CString	m_sDesc;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBriefSoundEdit)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CBriefSoundEdit)
	afx_msg void OnBriefSChoose();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnMissionflags();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BRIEFSOUNDEDIT_H__01F120A5_427B_11D2_B359_006008BF0B09__INCLUDED_)
