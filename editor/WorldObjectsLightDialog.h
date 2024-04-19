// WorldObjectsLightDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWorldObjectsLightDialog dialog

class CWorldObjectsLightDialog : public CDialog
{
// Construction
public:
	CWorldObjectsLightDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWorldObjectsLightDialog)
	enum { IDD = IDD_WORLDOBJECTSLIGHT };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWorldObjectsLightDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWorldObjectsLightDialog)
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
