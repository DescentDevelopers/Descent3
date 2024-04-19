#if !defined(AFX_FILEPAGEADDDLG_H__F3555122_C5AB_11D2_AB2B_006008BF0B09__INCLUDED_)
#define AFX_FILEPAGEADDDLG_H__F3555122_C5AB_11D2_AB2B_006008BF0B09__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FilePageAddDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFilePageAddDlg dialog

#define DRIVES 0
#define DIRECTORIES 1
#define FILES 2

extern char *DirectoryNames[];
extern int NumDirectoryNames;

/////////////////////////////////////////////////////////////////////////////
// CFileList window
class CFileList : public CListBox
{
// Construction
public:
	BOOL RefreshMask(CString mask);
	BOOL RefreshBox(CString path);
	//*.* is default filemask
	BOOL RefreshBox(CString path, CString mask);
	void setFileMask(CString mask);
	CString getFileMask();
	void setPath(CString path);
	CString getPath();
	BOOL InitBox(UINT res, CWnd *parent, LPSTR lpszPath, LPSTR lpszFileMask, UINT flags, UINT msg,void (*cb)(char *path)=NULL);
	CFileList();

// Attributes
public:

// Operations
protected:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFileList)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CFileList();

	// Generated message map functions
protected:
	//{{AFX_MSG(CFileList)
	afx_msg void OnDblclk();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	CWnd *m_cwndParent;
	UINT m_nMsg;
	UINT m_nFlag;
	CString m_csPath;
	CString m_csFileMask;
	void CheckEntry(LPSTR name, DWORD flag, BOOL dir); //dir==TRUE for directories
	BOOL FillBox();
	void (*callback)(char *path);
};


class CFilePageAddDlg : public CDialog
{
// Construction
public:
	CFilePageAddDlg(CWnd* pParent = NULL);   // standard constructor
	~CFilePageAddDlg(void);
	
	void SetInitialPath(char *path);

	int m_NumberOfSelectedFiles;
	char **m_SelectedFiles;
	char *m_SelectedDestDir;
private:
	bool Quit(void);
	CString initial_path;

// Dialog Data
	//{{AFX_DATA(CFilePageAddDlg)
	enum { IDD = IDD_FILEPAGEADD };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFilePageAddDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFilePageAddDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeListDrives();
	afx_msg void OnSelchangeListFolders();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	//the three boxes in this example    
	CFileList m_cflDrive;
	CFileList m_cflDir;
	CFileList m_cflFile;
    //user-defined message for communication between the boxes
    //and dialog (msg is trapped in WindowProc)    
	UINT m_nBoxMsg;	
};



//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILEPAGEADDDLG_H__F3555122_C5AB_11D2_AB2B_006008BF0B09__INCLUDED_)
