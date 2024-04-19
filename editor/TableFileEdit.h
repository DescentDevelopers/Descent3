#if !defined(AFX_TABLEFILEEDIT_H__DA8D6602_3037_11D2_8CBD_00A0C96ED60D__INCLUDED_)
#define AFX_TABLEFILEEDIT_H__DA8D6602_3037_11D2_8CBD_00A0C96ED60D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// TableFileEdit.h : header file
//

#include "TableManage.h"

#define TITLE_NAME	"Table File Text Editor v1.0"

/////////////////////////////////////////////////////////////////////////////
// CTableFileEdit dialog

class CTableFileEdit : public CDialog
{
// Construction
public:
	CTableFileEdit(CWnd* pParent = NULL);   // standard constructor
	
	void EnableAll(bool flag);				// Enables or disables all dialog items

	GenericPageList m_PageList;				// the generic page list

// Dialog Data
	//{{AFX_DATA(CTableFileEdit)
	enum { IDD = IDD_TABLE_FILE_EDITOR };
	CEdit	m_ScreenNameBox;
	CEdit	m_DescriptionBox;
	CComboBox	m_NameList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTableFileEdit)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTableFileEdit)
	afx_msg void OnTableEditFileLoad();
	afx_msg void OnTableEditFileExit();
	afx_msg void OnSelChangeTableEditNameList();
	afx_msg void OnTableEditBtnNext();
	afx_msg void OnTableEditBtnPrevious();
	virtual BOOL OnInitDialog();
	afx_msg void OnTableEditFileSave();
	afx_msg void OnTableEditFileSaveAs();
	afx_msg void OnUpdateTableEditFileSave(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTableEditFileSaveAs(CCmdUI* pCmdUI);
	afx_msg void OnClose();
	afx_msg void OnChangeTableEditDescriptionEdit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABLEFILEEDIT_H__DA8D6602_3037_11D2_8CBD_00A0C96ED60D__INCLUDED_)
