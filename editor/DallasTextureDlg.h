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

#if !defined(AFX_DALLASTEXTUREDLG_H__68211402_B2C7_11D2_A4E0_00A0C96ED60D__INCLUDED_)
#define AFX_DALLASTEXTUREDLG_H__68211402_B2C7_11D2_A4E0_00A0C96ED60D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DallasTextureDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDallasTextureDlg dialog

class CDallasTextureDlg : public CDialog
{
// Construction
public:
	CString	m_TextureName;
	int		m_TextureIndex;


	CDallasTextureDlg(CWnd* pParent = NULL);   // standard constructor

	void FillTextureList(void);

// Dialog Data
	//{{AFX_DATA(CDallasTextureDlg)
	enum { IDD = IDD_DALLAS_TEXTURE_DIALOG };
	CListBox	m_TextureList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDallasTextureDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDallasTextureDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DALLASTEXTUREDLG_H__68211402_B2C7_11D2_A4E0_00A0C96ED60D__INCLUDED_)
