#if !defined(AFX_SELECTRANGEDIALOG_H__533831C1_0A78_11D1_9797_006097D5770E__INCLUDED_)
#define AFX_SELECTRANGEDIALOG_H__533831C1_0A78_11D1_9797_006097D5770E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// SelectRangeDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// SelectRangeDialog dialog

class SelectRangeDialog : public CDialog
{
// Construction
public:
	SelectRangeDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(SelectRangeDialog)
	enum { IDD = IDD_SELECTRANGE_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SelectRangeDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(SelectRangeDialog)
	afx_msg void OnKillfocusLowerBoundEdit();
	afx_msg void OnKillfocusUpperBoundEdit();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnSelectSlopeCheck();
	afx_msg void OnKillfocusSlopeEdit();
	afx_msg void OnInverseCheck();
	afx_msg void OnTextureGridCheck();
	afx_msg void OnRandomCheck();
	afx_msg void OnSelectCurTextureCheck();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

int UpperBound;
int LowerBound;
int SlopeAngle;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELECTRANGEDIALOG_H__533831C1_0A78_11D1_9797_006097D5770E__INCLUDED_)
