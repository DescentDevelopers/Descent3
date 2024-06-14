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

#if !defined(AFX_VIRUTALCOMPILERCONFIG_H__F34D3DA0_9220_11D2_AB2B_006008BF0B09__INCLUDED_)
#define AFX_VIRUTALCOMPILERCONFIG_H__F34D3DA0_9220_11D2_AB2B_006008BF0B09__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VirtualCompilerConfig.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CVirtualCompilerConfig dialog

class CVirtualCompilerConfig : public CDialog
{
// Construction
public:
	CVirtualCompilerConfig(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CVirtualCompilerConfig)
	enum { IDD = IDD_CONFIGCOMPILER };
	CString	m_Compiler;
	int		m_DebugType;
	int		m_WarningLevel;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVirtualCompilerConfig)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CVirtualCompilerConfig)
	afx_msg void OnBrowse();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIRUTALCOMPILERCONFIG_H__F34D3DA0_9220_11D2_AB2B_006008BF0B09__INCLUDED_)
