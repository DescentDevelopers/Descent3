// CustomObjectDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCustomObjectDialog dialog

class CCustomObjectDialog : public CDialog
{
// Construction
public:
	CCustomObjectDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCustomObjectDialog)
	enum { IDD = IDD_CUSTOMIZE_OBJECT };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCustomObjectDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCustomObjectDialog)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
