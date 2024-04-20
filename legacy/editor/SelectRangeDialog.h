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

#if !defined(AFX_SELECTRANGEDIALOG_H__533831C1_0A78_11D1_9797_006097D5770E__INCLUDED_)
#define AFX_SELECTRANGEDIALOG_H__533831C1_0A78_11D1_9797_006097D5770E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// SelectRangeDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// SelectRangeDialog dialog

class SelectRangeDialog : public CDialog
{
// Construction
public:
	SelectRangeDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(SelectRangeDialog)
	enum { IDD = IDD_SELECTRANGE_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SelectRangeDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(SelectRangeDialog)
	afx_msg void OnKillfocusLowerBoundEdit();
	afx_msg void OnKillfocusUpperBoundEdit();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnSelectSlopeCheck();
	afx_msg void OnKillfocusSlopeEdit();
	afx_msg void OnInverseCheck();
	afx_msg void OnTextureGridCheck();
	afx_msg void OnRandomCheck();
	afx_msg void OnSelectCurTextureCheck();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

int UpperBound;
int LowerBound;
int SlopeAngle;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELECTRANGEDIALOG_H__533831C1_0A78_11D1_9797_006097D5770E__INCLUDED_)
