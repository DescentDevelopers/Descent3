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

// SiteSelectDlg.cpp : implementation file
//

#include "stdafx.h"
#include "d3launch.h"
#include "SiteSelectDlg.h"
#include "LaunchNames.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Column header info
#define SITE_NAME_COLUMN		0
#define SITE_ADDRESS_COLUMN		1
#define SITE_LOCATION_COLUMN	2

#define SITE_NAME_COLUMN_WIDTH		0.3
#define SITE_ADDRESS_COLUMN_WIDTH	0.4
#define SITE_LOCATION_COLUMN_WIDTH	0.3


/////////////////////////////////////////////////////////////////////////////
// CSiteSelectDlg dialog


CSiteSelectDlg::CSiteSelectDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSiteSelectDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSiteSelectDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CSiteSelectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSiteSelectDlg)
	DDX_Control(pDX, IDC_SITE_LIST, m_SiteList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSiteSelectDlg, CDialog)
	//{{AFX_MSG_MAP(CSiteSelectDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSiteSelectDlg message handlers

void CSiteSelectDlg::OnOK() 
{
	// TODO: Add extra validation here
	int num_items, j, flags;

	// If no item is selected, return (user MUST select an item)
	if(m_SiteList.GetSelectedCount()==0) {
		LocalizedMessageBox(IDS_SITESELECTDLG_MUST_SELECT, IDS_SITESELECTDLG_SELECT_TITLE, MB_OK|MB_ICONEXCLAMATION);
		return;
	}

	// How many items in the control list?
	num_items=m_SiteList.GetItemCount();
	
	// scan the item list for the selected item
	chosen_site=NO_SITE_CHOSEN;
	j=0;
	while(j<num_items) {
		
		flags=m_SiteList.GetItemState(j,LVIS_SELECTED);
		if(flags & LVIS_SELECTED)
			chosen_site=j;

		j++;
	}

	// If no site is selected, get outta here
	if(chosen_site==NO_SITE_CHOSEN) {
		LocalizedMessageBox(IDS_SITESELECTDLG_MUST_SELECT, IDS_SITESELECTDLG_SELECT_TITLE, MB_OK|MB_ICONEXCLAMATION);
		return;
	}

	// Create the selected download path
	sprintf(download_path,"%s%s",site_entries[chosen_site].url,site_entries[chosen_site].path);
	
	CDialog::OnOK();
}


BOOL CSiteSelectDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CString column_name;
	
	// Setup the columns
	column_name.LoadString(IDS_SITESELECTDLG_SITE_NAME);
	m_SiteList.InsertColumn(SITE_NAME_COLUMN,column_name.GetBuffer(0),LVCFMT_LEFT,
							-1,SITE_NAME_COLUMN);
	column_name.LoadString(IDS_SITESELECTDLG_SITE_ADDRESS);
	m_SiteList.InsertColumn(SITE_ADDRESS_COLUMN,column_name.GetBuffer(0),LVCFMT_LEFT,
							-1,SITE_ADDRESS_COLUMN);
	column_name.LoadString(IDS_SITESELECTDLG_SITE_LOCATION);
	m_SiteList.InsertColumn(SITE_LOCATION_COLUMN,column_name.GetBuffer(0),LVCFMT_LEFT,
							-1,SITE_LOCATION_COLUMN);
	SetColumnWidths();

	// Init the site entries
	m_SiteList.SetFullRowSel(TRUE);
	m_SiteList.DeleteAllItems();
	num_sites=0;
	chosen_site=NO_SITE_CHOSEN;
	download_path[0]='\0';

	// Read in the site entry data
	if(!ParseSiteFile()) {
		LocalizedMessageBox(IDS_SITESELECTDLG_PARSE_ERROR, IDS_SITESELECTDLG_SELECT_TITLE, MB_OK | MB_ICONERROR);
		return FALSE;
	}

	m_SiteList.SetItemState(0,LVIS_SELECTED|LVIS_FOCUSED,LVIS_SELECTED|LVIS_FOCUSED);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

// Calculates column widths and sets them
void CSiteSelectDlg::SetColumnWidths(void)
{
	uint32_t w0, w1, w2;

	RECT rect;
    m_SiteList.GetClientRect(&rect);

	w0=uint32_t(rect.right*SITE_NAME_COLUMN_WIDTH);
	w1=uint32_t(rect.right*SITE_ADDRESS_COLUMN_WIDTH);
	w2=uint32_t(rect.right*SITE_LOCATION_COLUMN_WIDTH);

	m_SiteList.SetColumnWidth(SITE_NAME_COLUMN,w0);
	m_SiteList.SetColumnWidth(SITE_ADDRESS_COLUMN,w1);
	m_SiteList.SetColumnWidth(SITE_LOCATION_COLUMN,w2);
}

// Reads in site entries from the site info file, 
// and adds them to the lists
BOOL CSiteSelectDlg::ParseSiteFile(void)
{
	SITE_ENTRY site_entry;
	char filebuffer[_MAX_PATH+1];
	int field;
	FILE *f;

	f = fopen(SITES_LOC_FNAME, "rt");
	if(f == NULL){
		return FALSE;
	}

	field=0;
	strcpy(filebuffer,"");
	while (!feof(f)) {
		fgets(filebuffer, _MAX_PATH, f);
		if(strlen(filebuffer)>0 && filebuffer[strlen(filebuffer) - 1] == '\n')
			filebuffer[strlen(filebuffer) - 1] = '\0';

		// if line is a comment, or empty, discard it
		if(strlen(filebuffer)==0 || filebuffer[0]==SITE_FILE_COMMENT_CHAR) 
			continue;

		switch(field) {
			case 0:
				strcpy(site_entry.name,filebuffer);
				break;
			case 1:
				strcpy(site_entry.location,filebuffer);
				break;
			case 2:
				strcpy(site_entry.url,filebuffer);
				break;
			case 3:
				strcpy(site_entry.path,filebuffer);
				break;
		}

		field++;
		if(field==4) {
			AddSiteEntry(&site_entry);
			field=0;
		}
	}
	fclose(f);

	return TRUE;
}

// Adds an entry struct to the list array and the list control
BOOL CSiteSelectDlg::AddSiteEntry(SITE_ENTRY *entry)
{
	int index;

	// Check if too many sites have been read in
	if(num_sites>=MAX_SITES) return FALSE;

	// Add the entry to the list control
	index=m_SiteList.GetItemCount();
	if((index=m_SiteList.InsertItem(index,entry->name))==-1)
		OutputDebugString("Didn't work!");

	m_SiteList.SetItem(index,SITE_ADDRESS_COLUMN,LVIF_TEXT,entry->url,0,0,0,0);
	m_SiteList.SetItem(index,SITE_LOCATION_COLUMN,LVIF_TEXT,entry->location,0,0,0,0);

	// Add the entry to the list array
	strcpy(site_entries[num_sites].name,entry->name);
	strcpy(site_entries[num_sites].location,entry->location);
	strcpy(site_entries[num_sites].url,entry->url);
	strcpy(site_entries[num_sites].path,entry->path);
	num_sites++;

	return TRUE;
}

void CSiteSelectDlg::LocalizedMessageBox(UINT msgID, UINT titleID, UINT type /*=MB_OK*/)
{
	CString msg, title;

	msg.LoadString(msgID);
	title.LoadString(titleID);
	MessageBox(msg,title,type);
}

