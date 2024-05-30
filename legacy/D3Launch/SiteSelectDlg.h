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

#if !defined(AFX_SITESELECTDLG_H__65671E24_38F7_11D2_8CBD_00A0C96ED60D__INCLUDED_)
#define AFX_SITESELECTDLG_H__65671E24_38F7_11D2_8CBD_00A0C96ED60D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// SiteSelectDlg.h : header file
//

#include "PsTypes.h"
#include "ListCtrlEx.h"

#define MAX_SITES	50

#define SITE_FILE_COMMENT_CHAR	';'

// Structure for the site entries
typedef struct SITE_ENTRY {
	char name[PSPATHNAME_LEN+1];		// The name/description of the site
	char url[PSPATHNAME_LEN+1];			// Just the base http/ftp address
	char location[PSPATHNAME_LEN+1];	// Where the site is located
	char path[PSPATHNAME_LEN+1];		// The remote directory and patch filename
};

#define NO_SITE_CHOSEN	-1

/////////////////////////////////////////////////////////////////////////////
// CSiteSelectDlg dialog

class CSiteSelectDlg : public CDialog
{
// Construction
public:
	CSiteSelectDlg(CWnd* pParent = NULL);   // standard constructor
	void SetColumnWidths(void);
	BOOL AddSiteEntry(SITE_ENTRY *entry);
	BOOL ParseSiteFile(void);
	void LocalizedMessageBox(UINT msgID, UINT titleID, UINT type=MB_OK);

	SITE_ENTRY site_entries[MAX_SITES];		// array of site entries
	int num_sites;							// the number of sites in memory
	int chosen_site;						// the index of the chosen site
	char download_path[PSPATHNAME_LEN*2+1];	// the full patch download path

// Dialog Data
	//{{AFX_DATA(CSiteSelectDlg)
	enum { IDD = IDD_SITE_SELECTION_DIALOG };
	CListCtrlEx	m_SiteList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSiteSelectDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSiteSelectDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SITESELECTDLG_H__65671E24_38F7_11D2_8CBD_00A0C96ED60D__INCLUDED_)
