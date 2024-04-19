// FilePageDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFilePageDialog dialog

class CFilePageDialog : public CDialog
{
// Construction
public:
	CFilePageDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFilePageDialog)
	enum { IDD = IDD_FILEPAGE };
	CCheckListBox	m_CheckedOutListCtrl;
	CCheckListBox	m_FileListCtrl;
	CString	m_DirectoryFilter;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFilePageDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
	int CreateIndexMap(int **index_map,int listbox_id);

private:
	void UpdateDialog();
protected:

	// Generated message map functions
	//{{AFX_MSG(CFilePageDialog)
	afx_msg void OnAddFile();
	afx_msg void OnCheckinFile();
	afx_msg void OnDeleteFile();
	afx_msg void OnLockFile();
	virtual BOOL OnInitDialog();
	afx_msg void OnOverride();
	afx_msg void OnFilesOut();
	afx_msg void OnSelchangeDirectoryFilter();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
