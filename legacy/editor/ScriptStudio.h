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

/*
 * $Logfile: /DescentIII/Main/editor/ScriptStudio.h $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:57:39 $
 * $Author: kevinb $
 *
 * Script Studio
 *
 * $Log: not supported by cvs2svn $
 * 
 * 7     3/24/98 6:35p Samir
 * added script studio exit 
 * 
 * 6     3/02/98 4:03p Samir
 * Created a function to open up a script studio window.
 * 
 * 5     9/26/97 4:15p Samir
 * Mostly working system to go to a certain script in a file.
 * 
 * 4     9/25/97 5:28p Samir
 * Even newer script code due to more changes in ObjCScript.cpp
 * 
 * 3     8/20/97 3:35p Samir
 * Enhanced ScriptStudio to use tabs!  Use new ScriptStudio instead of
 * ScriptEditor.
 * 
 * $NoKeywords: $
 */
 											 

#if !defined(AFX_SCRIPTSTUDIO_H__949F5802_189E_11D1_BD9B_00A0C96ED595__INCLUDED_)
#define AFX_SCRIPTSTUDIO_H__949F5802_189E_11D1_BD9B_00A0C96ED595__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ScriptStudio.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CScriptStudio dialog

struct tD3XProgram;

class CScriptStudio : public CDialog
{
// Construction
public:
	CScriptStudio(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CScriptStudio)
	enum { IDD = IDD_SCRIPTSTUDIO };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScriptStudio)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

private:
   CRichEditCtrl m_RichEdit;
	CString m_EditText;
	CString m_EditInitial;
	CFindReplaceDialog* m_pdlgFindReplace;   
	CString m_LastWord;
	CString m_SelectText;
	tD3XProgram *m_D3XProgram;
	bool m_FindMatchCase;
	bool m_RichEditInit;

private:
	void UpdateEditText();
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
	void HandleEditControl(int id, int code);

//	Interface
public:
	void SelectText(const char *txt);
	void SetText(const CString& text);
	void SetD3XObject(tD3XProgram *prog);
	void GetText(CString& text); 
	tD3XProgram *GetD3XObject() const { return m_D3XProgram; }
	
// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CScriptStudio)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
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
	afx_msg void OnMsgfilterScriptView(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDestroy();
	afx_msg void OnExit();
	//}}AFX_MSG
	afx_msg LONG OnFindReplace(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};


//	runs script studio for a certain script file and points to the first instance of scrname in the file
bool InvokeScriptStudio(const char *filename, const char *scrname);


//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCRIPTSTUDIO_H__949F5802_189E_11D1_BD9B_00A0C96ED595__INCLUDED_)
