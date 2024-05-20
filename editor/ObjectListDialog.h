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

#if !defined(AFX_OBJECTLISTDIALOG_H__3DEE8311_972E_11D1_BFC6_00A0C94B8467__INCLUDED_)
#define AFX_OBJECTLISTDIALOG_H__3DEE8311_972E_11D1_BFC6_00A0C94B8467__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ObjectListDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CObjectListDialog dialog

#define CLIPBOARD_LEAF 0
#define POWERUP_LEAF 1
#define ROBOT_LEAF 2
#define BUILDING_LEAF 3
#define DOOR_LEAF 4
#define PLAYER_LEAF 5
#define CLUTTER_LEAF 6
#define CAMERA_LEAF	7
#define SOUNDSOURCE_LEAF 8
#define MISC_LEAF 9
#define MAX_OBJ_LEAFS 10

struct object;

class CObjectListDialog : public CDialog
{
// Construction
public:
	CObjectListDialog(CWnd* pParent = NULL);   // standard constructor

	void Refresh();
	void ClearTree();
	int GetObjectSelected() const;

// Dialog Data
	//{{AFX_DATA(CObjectListDialog)
	enum { IDD = IDD_OBJTREEDLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CObjectListDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CObjectListDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnSelchangedObjtree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnGoto();
	afx_msg void OnDelete();
	afx_msg void OnClear();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	struct tObjNode
	{
		HTREEITEM hItem;
		int handle;
	}
	tObjNode;

	tObjNode *m_Leafs[MAX_OBJ_LEAFS];

	HTREEITEM hClipboardTree;
	HTREEITEM hRobotTree;
	HTREEITEM hPowerupTree;
	HTREEITEM hBuildingTree;
	HTREEITEM hDoorTree;
	HTREEITEM hPlayerTree;
	HTREEITEM hClutterTree;
	HTREEITEM hCameraTree;
	HTREEITEM hSoundSourceTree;
	HTREEITEM hMiscTree;

	int m_ObjectHandle;								// current object handle selected.
	int nclipboard, npowerup, nrobot, nbuilding, nclutter, ndoor, nplayer, ncamera, nsndsrc, nmisc;

private:
	void MakeInfoStr(object *obj,  char *str);
	int GetObjHandleFromItem(HTREEITEM hItem);
};


//	modeless implementation of object fialog.
class CObjectListModeless: public CObjectListDialog
{

public:
	CObjectListModeless(CWnd* pParent = NULL);

	BOOL Create();

// Dialog Data
	//{{AFX_DATA(CObjectListModeless)
	enum { IDD = IDD_OBJTREEDLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CObjectListModeless)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CObjectListModeless)
	afx_msg void OnMove(int x, int y);
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CWnd *m_wndParent;
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OBJECTLISTDIALOG_H__3DEE8311_972E_11D1_BFC6_00A0C94B8467__INCLUDED_)
