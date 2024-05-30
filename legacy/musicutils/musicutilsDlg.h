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

// musicutilsDlg.h : header file
//

#if !defined(AFX_MUSICUTILSDLG_H__D94CC635_1CA3_11D2_AF2D_0060089A8025__INCLUDED_)
#define AFX_MUSICUTILSDLG_H__D94CC635_1CA3_11D2_AF2D_0060089A8025__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "pstypes.h"
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CMusicutilsDlg dialog

class OSFArchive;

class CMusicutilsDlg : public CDialog
{
// Construction
public:
	CMusicutilsDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CMusicutilsDlg)
	enum { IDD = IDD_MUSICUTILS_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMusicutilsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual void OnOK();
	HICON m_hIcon;

	virtual void OnCancel();

	// Generated message map functions
	//{{AFX_MSG(CMusicutilsDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnToolsMakeStream();
	afx_msg void OnDestroy();
	afx_msg void OnFileQuit();
	afx_msg void OnToolsPlaySong();
	afx_msg void OnFileNew();
	afx_msg void OnKeydownThemeTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnFileOpen();
	afx_msg void OnFileSave();
	afx_msg void OnFileSaveas();
	afx_msg void OnReturnThemeTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkThemeTree(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	bool SaveModified();
	void NewTheme();
	void OpenThemeFile(const CString& filename);
	void SaveThemeFile();
	bool SaveDigitalStream(OSFArchive *osf, const CString& rawfilename, uint8_t compression, uint8_t format, uint32_t samples, 
			const CString& realname, uint8_t xforms, int measure);
	void GetStreamInfo(const CString &filename, CString& realname);

private:
	enum tSongProp
	{
		SONGPROP_FILE,
		SONGPROP_REPEAT,
		SONGPROP_ROOMLINK
	};

	struct tSongItem
	{
		HTREEITEM hTreeItem;
		CString strSongPath;

		HTREEITEM hTI_repeat;
		bool repeat;

		HTREEITEM hTI_lvllink;
		int roomlink;
	}
	tSongItem;

	HTREEITEM hIntroLeaf;
	HTREEITEM hBkgLeaf;
	HTREEITEM hCombatLeaf;
	HTREEITEM hDangerLeaf;
	HTREEITEM hEndCombatLeaf;

	bool m_modified;
	CString m_themefilename;
	CString m_themename;

private:
	tSongItem *InsertSong(HTREEITEM hTree, const tSongItem *songitm);
	LPARAM GetLeafParam(HTREEITEM hItem);
	HTREEITEM InsertLeaf(HTREEITEM hParent, const char *text, LPARAM param);
	void DoTreeItem(HTREEITEM hItem);
	void InsertTreeItem(HTREEITEM hParentItem);
	HTREEITEM FreeTreeItem(HTREEITEM hItem);		// returns next tree item if a sibling.
	void FreeTreeItems(HTREEITEM hLeaf);
	void FreeTree();
	tSongItem *GetSongFromTreeItem(HTREEITEM hLeaf);
	void SetSongProperty(tSongProp property, tSongItem *song);
};


#define LEAFTYPE_PROPERTY		((LPARAM)0)
#define LEAFTYPE_CATEGORY		((LPARAM)-1)
#define LEAFTYPE_INVALID		((LPARAM)-2)

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MUSICUTILSDLG_H__D94CC635_1CA3_11D2_AF2D_0060089A8025__INCLUDED_)
