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

// HogDialog.cpp : implementation file
//

#include "stdafx.h"
#include <afxtempl.h>

#include "editor.h"

#include "hogfile.h"
#include "HogDialog.h"
#include "pserror.h"

#include <stdio.h>
#include <malloc.h>
#include <string.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
//	data structures
/*
struct hog_library {
	CList <hog_library_entry, hog_library_entry&> table;
	FILE *fp;
};
*/


/////////////////////////////////////////////////////////////////////////////
//	Hogfile manipulation functions
/*
//	creates a new hogfile
//		does nothing else really
hog_library *new_hogfile(char *hogname)
{
	FILE *fp;
	hog_library *hogfile;

	fp = fopen( hogname, "wb" );
	if ( fp == NULL ) return NULL;

	fwrite(LIBRARY_TAG, 4, 1, fp);

	hogfile = new hog_library;
	hogfile->fp = fp;

	return hogfile;
}	


//	this opens an existing hogfile
//		loads in file listing and hog info.
hog_library *open_hogfile(char *hogname)
{
	FILE *fp, *fplst;
	char *strptr;
	char filename[_MAX_PATH];
	int id, nfiles;
	int i,offset,t;
	index_entry *index;
	hog_library *hogfile = NULL;

	strcpy(filename, hogname);
	strptr = strstr(filename, ".hog");
	if (strptr) {
		strptr[0] = 0;					// terminate this string
	}
	strcat(filename, ".lst");			// load in list file too.

	fp = fopen( hogname, "rb" );
	if ( fp == NULL ) return NULL;
	fplst = fopen(filename, "rb");
	if ( fplst == NULL ) {
		fclose(fp);
		OutrageMessageBox("Unable to open hogfile's database. <%s>\n", filename);
		return (hog_library *)0xffffffff;
	}

	fread( &id, sizeof(int), 1, fp );
	if ( strncmp( (char *) &id, LIBRARY_TAG, strlen(LIBRARY_TAG) ) ) {
		fclose(fp);
		fclose(fplst);
		OutrageMessageBox("Invalid hog file <%s>\n", hogname);
		return (hog_library *)0xffffffff;
	}

	fread(&nfiles,sizeof(int),1,fp);		// get number of files
	index = new index_entry[nfiles];		// allocate file index.
 	hogfile = new hog_library;	
	hogfile->fp = fp;
	
//	id + nfiles + nfiles * (filename + size)
	offset = sizeof(int)+sizeof(int)+nfiles*(LIB_FILENAME_LEN+sizeof(int));	

//	read in index table
	t = fread( index, sizeof(index_entry), nfiles, fp );
	if ( t != nfiles )	{					//eof here is ok
		fclose(fp);
		fclose(fplst);
		OutrageMessageBox("Hogfile's file index corrupt <%s>.\n", hogname);
		return (hog_library *)0xffffffff;	//CF_BAD_LIB;
	}

//	copy disk index to our internal structure (generate our dynamic list.)
	for (i=0;i<nfiles;i++) 
	{
		hog_library_entry entry;

	// get database path for filename
		fgets(entry.path, LIB_PATHNAME_LEN, fplst);
		if (entry.path[strlen(entry.path)-1] == '\n') 
			entry.path[strlen(entry.path)-1] = 0;	// get rid of newline, if there

	// create library entry from index entry.
		strcpy(entry.name, index[i].name);
		entry.length = index[i].len;
		entry.offset = offset;
	#if LIB_HAS_DATE
		entry.timestamp = index[i].timestamp;
	#else
		entry.timestamp = 0;
	#endif
	#if LIB_HAS_FLAGS
		entry.flags = index[i].flags;
	#else
		entry.flags = 0;
	#endif
		hogfile->table.AddTail(entry);

		offset += entry.length;
	}

	delete[] index;

	fclose(fplst);

	return hogfile;
}


//	deallocates all info in library struct
void close_hogfile(hog_library *hogfile)
{
	fclose(hogfile->fp);
	delete hogfile;
}


//	copies a file to the hogfile!
int copy_file_to_hog(hog_library *hogfile, FILE *srcfp)
{



	return 1;
}


//	save hog file
//		writes out complete hogfile copying info
//		returns 0 if standard error

int write_hogfile(hog_library *library)
{
	index_entry *index = NULL;
	int nfiles, index_pos;	//, pos
	
	nfiles = library->table.GetCount();
	if (nfiles != 0) {
		index = new index_entry[nfiles];
		if (index == NULL) {
			OutrageMessageBox("Unable to allocate memory for hogfile write.");
			return 0;
		}
	}

//	write out num elements
	fseek(library->fp, sizeof(int), SEEK_SET);
	fwrite(&nfiles, sizeof(int), 1, library->fp);

	if (nfiles == 0) return 1;				// if no files, we are done.

//	write out file dummy index
	index_entry dummy;
	index_pos= ftell(library->fp);
	ZeroMemory(&dummy, sizeof(dummy));
	if (fwrite(&dummy, sizeof(index_entry), nfiles, library->fp) != (unsigned)nfiles) {
		OutrageMessageBox("Out of space!!!");
		delete[] index;
		return 0;
	}

//	copy the files to the hogfile!
	
	

	return 1;
}
*/

/////////////////////////////////////////////////////////////////////////////
// CHogDialog dialog


CHogDialog::CHogDialog(CWnd* pParent, char *filename)
	: CDialog(CHogDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CHogDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_HogName = filename;
}


void CHogDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHogDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CHogDialog, CDialog)
	//{{AFX_MSG_MAP(CHogDialog)
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHogDialog message handlers

BOOL CHogDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
// TODO: Add extra initialization here
	m_ListCtrl = (CListCtrl *)GetDlgItem(IDC_HOGLIST);	
	m_ListCtrl->InsertColumn(0, "Filename", LVCFMT_LEFT, 140);
	m_ListCtrl->InsertColumn(1, "Date", LVCFMT_LEFT, 80);
	m_ListCtrl->InsertColumn(2, "Length", LVCFMT_LEFT, 80);
	m_ListCtrl->InsertColumn(3, "Attributes", LVCFMT_LEFT, 120);

// open hogfile or create new one.
/*	m_Library = open_hogfile(m_HogName);
	if (!m_Library) {
		int res = MessageBox("Hogfile does not exist.  Do you wish to create a new one?", "Hogmaker", MB_YESNO);
		if (res == IDNO) {
			EndDialog(IDCANCEL);
			return TRUE;
		}
		else {
			m_Library = new_hogfile(m_HogName);
			if (!m_Library) {
				MessageBox("Unable to create hogfile.", "Error", MB_OK);
				EndDialog(IDCANCEL);
				return TRUE;
			}
		}
	}
	else if (m_Library == (hog_library *)0xffffffff) {
		MessageBox("Hogmaker failure.  Sorry...", "Hogmaker", MB_OK);
		EndDialog(IDCANCEL);
		return TRUE;
	}
*/
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CHogDialog::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	
// TODO: Add your message handler code here
}



