#if !defined(AFX_BRIEFMISSIONFLAGSDLG_H__C79F9EC0_A3E9_11D2_AB2B_006008BF0B09__INCLUDED_)
#define AFX_BRIEFMISSIONFLAGSDLG_H__C79F9EC0_A3E9_11D2_AB2B_006008BF0B09__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BriefMissionFlagsDlg.h : header file
//
#include "pstypes.h"
/////////////////////////////////////////////////////////////////////////////
// CBriefMissionFlagsDlg dialog

class CBriefMissionFlagsDlg : public CDialog
{
// Construction
public:
	CBriefMissionFlagsDlg(uint setflags,uint unsetflags,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CBriefMissionFlagsDlg)
	enum { IDD = IDD_BRIEFMISSIONFLAGS };
	int		m_1;
	int		m_10;
	int		m_11;
	int		m_12;
	int		m_13;
	int		m_14;
	int		m_15;
	int		m_16;
	int		m_17;
	int		m_18;
	int		m_19;
	int		m_2;
	int		m_20;
	int		m_21;
	int		m_22;
	int		m_23;
	int		m_24;
	int		m_25;
	int		m_26;
	int		m_27;
	int		m_28;
	int		m_29;
	int		m_3;
	int		m_30;
	int		m_31;
	int		m_32;
	int		m_4;
	int		m_5;
	int		m_6;
	int		m_7;
	int		m_9;
	int		m_8;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBriefMissionFlagsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

public:
	uint m_Set,m_UnSet;

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CBriefMissionFlagsDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BRIEFMISSIONFLAGSDLG_H__C79F9EC0_A3E9_11D2_AB2B_006008BF0B09__INCLUDED_)
