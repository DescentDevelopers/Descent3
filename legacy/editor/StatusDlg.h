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

#if !defined(AFX_STATUSDLG_H__7321FFC1_FE93_11D0_8901_0020A90CB6F2__INCLUDED_)
#define AFX_STATUSDLG_H__7321FFC1_FE93_11D0_8901_0020A90CB6F2__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// StatusDlg.h : header file
//

#ifndef _STATUS_DLG_H_
#define _STATUS_DLG_H_

#include "fix.h"

/////////////////////////////////////////////////////////////////////////////
// CStatusDlg dialog

class CStatusDlg : public CDialog
{
// Construction
public:
	CStatusDlg(CWnd* pParent = NULL);   // standard constructor
	int Step();
	void Init(int min,int max,int delta);
	void Text(char *s);
	void SetTo(int set);
// Dialog Data
	//{{AFX_DATA(CStatusDlg)
	enum { IDD = IDD_STATUSDLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStatusDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CStatusDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

class CProgress
{
public:
	bool InitProgress(fix min,fix max,long int iterations,CWnd *parent=NULL);
	bool InitProgress(int min,int max,long int iterations,CWnd *parent);
	bool InitProgress(CWnd *parent=NULL);
	void DestroyProgress();
	bool IncreaseProgress();
	void SetProgressText(char *string);
	void SetProgressPercentage(int percent);
	void SetProgressPercentage(float percent);
private:
	int m_Max,m_Min;
	CStatusDlg *m_StatusDlg;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STATUSDLG_H__7321FFC1_FE93_11D0_8901_0020A90CB6F2__INCLUDED_)



#endif
