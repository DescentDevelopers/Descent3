/*
 * $Logfile: /DescentIII/Main/editor/ScriptEditorDlg.h $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:57:38 $
 * $Author: kevinb $
 *
 *	The script editor dialog.
 *
 * $Log: not supported by cvs2svn $
 * 
 * 9     8/29/97 2:21p Samir
 * Add some rich edit functionality.
 *
 * $NoKeywords: $
 */

#if !defined(AFX_SCRIPTEDITORDLG_H__940F1B41_FF57_11D0_8901_0020A90CB6F2__INCLUDED_)
#define AFX_SCRIPTEDITORDLG_H__940F1B41_FF57_11D0_8901_0020A90CB6F2__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ScriptEditorDlg.h : header file
//

#include "d3x.h"

/////////////////////////////////////////////////////////////////////////////
// CScriptEditorDlg dialog

#ifndef _SCRIPTEDITOR_H_
#define _SCRIPTEDITOR_H_

extern char Default_external_editor[256];

class CScriptEditorDlg : public CDialog
{
// Construction
public:
	CScriptEditorDlg(CWnd* pParent = NULL);   // standard constructor
	void SetScript(char *script, tD3XProgram *d3x);
	void SetExternalEditor(char *external);
	char *GetScript(void);
	int GetScriptLength(void);
	void InitFindReplace(BOOL bFind);
	bool FindNext(char *w);
	void ReplaceSelected(char *replace_word);
	void ReplaceAll(char *find_word,char *replace_word);
	void ReplaceCurrent(char *replace_word);
	void SetCurrentIndex(int index);
	bool CheckWord(int index,char *text,char *w);
	void SetSelection(int index,int length);
	void RemoveAllSelections();
	void AdjustExternalName();
	void ExportScript(char *file);
	void ImportScript(char *file);
	bool m_FindMatchCase;


	CFindReplaceDialog*  m_pdlgFindReplace;   
	CString m_LastWord;
	CString m_ExternalEditor;
	tD3XProgram *m_Program;

// Dialog Data
	//{{AFX_DATA(CScriptEditorDlg)
	enum { IDD = IDD_SCRIPTEDITOR };
	CString	m_sScript;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScriptEditorDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

   	// Generated message map functions
	//{{AFX_MSG(CScriptEditorDlg)
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnSetExternalViewer();
	afx_msg void OnUseExternal();
	virtual BOOL OnInitDialog();
	afx_msg void OnScripteditorExport();
	afx_msg void OnScripteditorImport();
	afx_msg void OnScriptCompile();
	afx_msg void OnOsirisCompileScript();
	afx_msg void OnOsirisCopy();
	afx_msg void OnOsirisCut();
	afx_msg void OnOsirisExportScript();
	afx_msg void OnOsirisFind();
	afx_msg void OnOsirisFindAndReplace();
	afx_msg void OnOsirisImportScript();
	afx_msg void OnOsirisPaste();
	afx_msg void OnOsirisSaveExit();
	afx_msg void OnOsirisSetExternal();
	afx_msg void OnOsirisUseExternal();
	afx_msg void OnUpdateOsirisPaste(CCmdUI* pCmdUI);
	//}}AFX_MSG
	afx_msg LONG OnFindReplace(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

private:
	CRichEditCtrl m_RichEdit;
};
#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCRIPTEDITORDLG_H__940F1B41_FF57_11D0_8901_0020A90CB6F2__INCLUDED_)
