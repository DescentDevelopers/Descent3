#if !defined(AFX_DALLASVECTORPROMPTDIALOG_H__9E6377E1_95AF_11D2_A4E0_00A0C96ED60D__INCLUDED_)
#define AFX_DALLASVECTORPROMPTDIALOG_H__9E6377E1_95AF_11D2_A4E0_00A0C96ED60D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DallasVectorPromptDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDallasVectorPromptDialog dialog

class CDallasVectorPromptDialog : public CDialog
{
// Construction
public:
	CDallasVectorPromptDialog(CWnd* pParent = NULL);   // standard constructor

	float m_PromptData1;
	float m_PromptData2;
	float m_PromptData3;

// Dialog Data
	//{{AFX_DATA(CDallasVectorPromptDialog)
	enum { IDD = IDD_DALLAS_VECTOR_DIALOG };
	CEdit	m_Data3Edit;
	CEdit	m_Data2Edit;
	CEdit	m_Data1Edit;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDallasVectorPromptDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDallasVectorPromptDialog)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DALLASVECTORPROMPTDIALOG_H__9E6377E1_95AF_11D2_A4E0_00A0C96ED60D__INCLUDED_)
