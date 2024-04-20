/*
* Descent 3 
* Copyright (C) 2024 Parallax Software
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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
