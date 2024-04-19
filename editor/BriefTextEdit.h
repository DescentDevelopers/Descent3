#if !defined(AFX_BRIEFTEXTEDIT_H__01F120A6_427B_11D2_B359_006008BF0B09__INCLUDED_)
#define AFX_BRIEFTEXTEDIT_H__01F120A6_427B_11D2_B359_006008BF0B09__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// BriefTextEdit.h : header file
//
#include "telcomefxstructs.h"
#include "TelComEffects.h"

/////////////////////////////////////////////////////////////////////////////
// CBriefTextEdit dialog

class CBriefTextEdit : public CDialog
{
// Construction
public:
	CBriefTextEdit(int curr_screen,LPTCTEXTDESC desc=NULL,char *text=NULL,int id=0,CWnd* pParent = NULL);   // standard constructor
	TCTEXTDESC desc;
	int m_Screen;
	CString m_Text;
	CRichEditCtrl m_RichEdit;
	bool m_RichEditInit;

// Dialog Data
	//{{AFX_DATA(CBriefTextEdit)
	enum { IDD = IDD_BRIEF_ADDTEXT };
	int		m_iID;
	int		m_iBY;
	int		m_iRX;
	float	m_fSpeed;
	float	m_fStartTime;
	int		m_iTY;
	int		m_iLX;
	int		m_iEffectType;
	CString	m_sDesc;
	BYTE	m_iBlue;
	BYTE	m_iGreen;
	BYTE	m_iRed;
	BOOL	m_TabStop;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBriefTextEdit)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CBriefTextEdit)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnBriefColorPal();
	afx_msg void OnSelchangeBriefTPredef();
	afx_msg void OnMissionflags();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BRIEFTEXTEDIT_H__01F120A6_427B_11D2_B359_006008BF0B09__INCLUDED_)
