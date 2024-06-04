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

--- HISTORICAL COMMENTS FOLLOW ---

 * $Logfile: /DescentIII/Main/hogedit/HogEditView.cpp $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:57:56 $
 * $Author: kevinb $
 *
 * 
 *
 * $Log: not supported by cvs2svn $
 * 
 * 17    5/05/99 12:53p Nate
 * Added support for multiple file extraction
 * 
 * 16    2/10/99 1:03p Nate
 * Fixed infinite loop when importing duplicate directories
 * 
 * 15    12/03/98 10:38a Nate
 * Added support for loading/saving hogs with extensions other than '.hog'
 * 
 * 14    10/30/98 11:15a Nate
 * Added support for modification of hog files.
 * 
 * 13    10/28/98 11:24a Nate
 * Added command line auto-processing (create hog from a rib)
 * 
 * 12    10/15/98 2:53p Nate
 * Fixed title bar filename bug
 * 
 * 11    9/17/98 4:29p Nate
 * Added Import Directory option.
 * 
 * 10    8/16/98 4:22p Nate
 * Added message deferal and new hog info dialog
 * 
 * 9     8/14/98 7:47p Nate
 * Removed "duplicate file" error message (upon File Inserts)
 * 
 * 8     8/14/98 6:04p Nate
 * Added number of files field to status bar.
 * 
 * 7     8/14/98 4:38p Nate
 * Fixed a few minor bugs and added better error reporting
 * 
 * 6     8/14/98 1:01p Nate
 * Added better error reporting for the HogEditor
 * 
 * 5     7/22/98 2:38p Nate
 * Added Modified File prompt when exiting
 * 
 * 4     7/22/98 2:10p Nate
 * Added MB field to Hog Create message prompt
 * 
 * 3     7/20/98 3:35p Nate
 * Added more Editing functionality
 * 
 * 2     7/15/98 12:31p Nate
 * Initial version
 *
 * $NoKeywords: $
 */

// HogEditView.cpp : implementation of the CHogEditView class
//

#include "stdafx.h"
#include <afxtempl.h>
#include <io.h>
#include <direct.h>
#include "HogEdit.h"

#include "hogfile.h"
#include "HogEditDoc.h"
#include "listvwex.h"
#include "HogEditView.h"
#include "HogInfo.h"
#include "DirDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHogEditView

IMPLEMENT_DYNCREATE(CHogEditView, CListView)

BEGIN_MESSAGE_MAP(CHogEditView, CListView)
	//{{AFX_MSG_MAP(CHogEditView)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_COMMAND(ID_ACTION_INSERT, OnActionInsert)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
	ON_COMMAND(ID_ACTION_DELETE, OnActionDelete)
	ON_COMMAND(ID_ACTION_CREATE, OnActionCreate)
	ON_COMMAND(ID_VIEW_FULL_ROW, OnViewFullRow)
	ON_UPDATE_COMMAND_UI(ID_VIEW_FULL_ROW, OnUpdateViewFullRow)
	ON_COMMAND(ID_ACTION_UPDATE, OnActionUpdate)
	ON_COMMAND(ID_ACTION_EXTRACT, OnActionExtract)
	ON_UPDATE_COMMAND_UI(ID_ACTION_DELETE, OnUpdateActionDelete)
	ON_UPDATE_COMMAND_UI(ID_ACTION_EXTRACT, OnUpdateActionExtract)
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnclick)
	ON_COMMAND(ID_ACTION_IMPORT, OnActionImport)
	ON_MESSAGE(WM_STRAIGHT_TO_FILE_OPEN, OnStraightToFileOpen)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHogEditView construction/destruction

CHogEditView::CHogEditView()
{
	// TODO: add construction code here
}

CHogEditView::~CHogEditView()
{
}

BOOL CHogEditView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	cs.style|=LVS_SHOWSELALWAYS|LVS_REPORT|LVS_SORTASCENDING;// | LVS_NOSORTHEADER;

	return CListView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CHogEditView drawing

void CHogEditView::OnDraw(CDC* pDC)
{
	CHogEditDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// TODO: add draw code for native data here
}

void CHogEditView::OnInitialUpdate()
{
	CListView::OnInitialUpdate();


	// TODO: You may populate your ListView with items by directly accessing
	//  its list control through a call to GetListCtrl().
	CListCtrl& lcp = GetListCtrl();

	// Add column names
	lcp.InsertColumn(FILE_NAME_COLUMN,FILE_NAME_TITLE,LVCFMT_LEFT,
					 -1,FILE_NAME_COLUMN);
	lcp.InsertColumn(FILE_TYPE_COLUMN,FILE_TYPE_TITLE,LVCFMT_LEFT,
					 -1,FILE_TYPE_COLUMN);
	lcp.InsertColumn(FILE_DIR_COLUMN,FILE_DIR_TITLE,LVCFMT_LEFT,
					 -1,FILE_DIR_COLUMN);
	lcp.InsertColumn(FILE_SIZE_COLUMN,FILE_SIZE_TITLE,LVCFMT_RIGHT,
					 -1,FILE_SIZE_COLUMN);
	lcp.InsertColumn(FILE_TIME_COLUMN,FILE_TIME_TITLE,LVCFMT_RIGHT,
					 -1,FILE_TIME_COLUMN);
	SetColumnWidths();

	OutputDebugString("Posting Message...\n");
	if(DoAutoProcess){
		PostMessage(WM_STRAIGHT_TO_FILE_OPEN);
	}
}

// Calculates column widths and sets them
void CHogEditView::SetColumnWidths(void)
{
	CListCtrl& lcp = GetListCtrl();
	uint32_t w0, w1, w2, w3, w4;

	RECT rect;
    lcp.GetClientRect(&rect);

	w0=uint32_t(rect.right*FILE_NAME_COLUMN_WIDTH);
	w1=uint32_t(rect.right*FILE_TYPE_COLUMN_WIDTH);
	w2=uint32_t(rect.right*FILE_DIR_COLUMN_WIDTH);
	w3=uint32_t(rect.right*FILE_SIZE_COLUMN_WIDTH);
	w4=rect.right-(w0+w1+w2+w3);

	lcp.SetColumnWidth(FILE_NAME_COLUMN,w0);
	lcp.SetColumnWidth(FILE_TYPE_COLUMN,w1);
	lcp.SetColumnWidth(FILE_DIR_COLUMN,w2);
	lcp.SetColumnWidth(FILE_SIZE_COLUMN,w3);
	lcp.SetColumnWidth(FILE_TIME_COLUMN,w4);
}

/////////////////////////////////////////////////////////////////////////////
// CHogEditView diagnostics

#ifdef _DEBUG
void CHogEditView::AssertValid() const
{
	CListView::AssertValid();
}

void CHogEditView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}

CHogEditDoc* CHogEditView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CHogEditDoc)));
	return (CHogEditDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CHogEditView message handlers

void CHogEditView::OnFileNew() 
{
	// TODO: Add your command handler code here
	CHogEditDoc *doc=GetDocument();
	CListCtrl& list=GetListCtrl();

	// Set full row selection
	SetFullRowSel(m_bFullRowSel);

	// If current file has been modified, check with user first...
	if(ModifiedPrompt(this)==IDNO) return;	

	// Display Wait Cursor
	CWaitCursor wc;

	// Clear out the document's information
	doc->OnNewDocument();

	// Wipeout the CtrlList items
	list.DeleteAllItems();

	// Put new doc name on title bar
	UpdateTitle(doc->m_DocumentName,DocModified);
	UpdateTotalFiles(list.GetItemCount());
}


// Returns a type string for the given filename extension
void CHogEditView::GetFileType(char *type, char *filename)
{
	char ext[PSFILENAME_LEN+1];
	_splitpath(filename, NULL, NULL, NULL, ext);

	if(!strcmp(ext,".ogf")) {
		strcpy(type,"Outrage Graphics File");
		return;
	}
	if(!strcmp(ext,".oof")) {
		strcpy(type,"Outrage Object File");
		return;
	}
	if(!strcmp(ext,".oaf")) {
		strcpy(type,"Outrage Animation File");
		return;
	}
	if(!strcmp(ext,".orf")) {
		strcpy(type,"Outrage Room File");
		return;
	}
	if(!strcmp(ext,".wav")) {
		strcpy(type,"Sound File");
		return;
	}
	if(!strcmp(ext,".scr")) {
		strcpy(type,"Script File");
		return;
	}
	if(!strcmp(ext,".d3x")) {
		strcpy(type,"D3X Object Code");
		return;
	}
	if(!strcmp(ext,".inf")) {
		strcpy(type,"Misc Config File");
		return;
	}

	// Default
	strcpy(type,"Unknown");
}

void CHogEditView::OnActionInsert() 
{
	// TODO: Add your command handler code here
	CListCtrl& list=GetListCtrl();
	CHogEditDoc *doc=GetDocument();
	hog_library_entry entry;
	int rv;
	CString path;
	char title[]="Add Files";
	char *filename_buffer;

	// Set full row selection
	SetFullRowSel(m_bFullRowSel);

	CString filter = "All files (*.*)|*.*||";
	CFileDialog dlg_open(TRUE, NULL, NULL, OFN_ALLOWMULTISELECT|OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST, filter.GetBuffer(0), this);

	filename_buffer=new char[MAX_FILENAME_BUFFER];
	if(!filename_buffer) {
		OutputDebugString("Could not allocate filename buffer!\n");
		return;
	}
	memset(filename_buffer, '\0', MAX_FILENAME_BUFFER);

	dlg_open.m_ofn.lpstrTitle=title;
	dlg_open.m_ofn.lpstrFileTitle=NULL;
	dlg_open.m_ofn.lpstrFile=filename_buffer;
	dlg_open.m_ofn.nMaxFile=MAX_FILENAME_BUFFER;
		
	rv=dlg_open.DoModal();
	if (rv == IDCANCEL) {
		char error_type[50];
		int err=CommDlgExtendedError();
		sprintf(error_type,"Error: %d",err);
		if (err!=0) MessageBox(error_type);
		delete[] filename_buffer;
		return;
	}
	if (rv==0) {
		MessageBox("Unknown error has occurred!");
		delete[] filename_buffer;
		return;
	}

	// Display wait cursor
	CWaitCursor wc;
	ProcessMessages();
	wc.Restore();

	//	iterate through filename list.
	POSITION pos;

	pos = dlg_open.GetStartPosition();

	while (pos!=NULL)
	{
		path = dlg_open.GetNextPathName(pos);

		rv=doc->AddFile(path,&entry);
		if(rv==ADDFILE_OK) {
			InsertItem(0,&entry);
			UpdateTotalFiles(list.GetItemCount());
			wc.Restore();	// Get the Wait cursor back
		}
		else {
			char msg[1024];
			switch(rv) {
				case ADDFILE_LONG_FNAME_ERROR:
					sprintf(msg,"The following filename exceeded %d characters:\n",
							PSFILENAME_LEN);
					break;
				case ADDFILE_DUP_FILE_ERROR:
					continue;
					//sprintf(msg,"The following filename is already in the list:\n");
					//break;
				case ADDFILE_STAT_ERROR:
					sprintf(msg,"Could not get stats for following file:\n");
					break;
				default:
					sprintf(msg,"An unidentified error has occurred with the following file:\n");
					break;
			}
			strcat(msg,path);
			strcat(msg,"\n\nThis file will not be added to the file list.");
			MessageBox(
				msg,
				"Insert File Error!");

			wc.Restore();	// Get the Wait cursor back
		}
	}
	delete[] filename_buffer;

	// Update title bar
	if(DocModified)
		UpdateTitle(doc->m_DocumentName,DocModified);

	UpdateTotalFiles(list.GetItemCount());
}

// Inserts the given entry data into the CListCtrl 
bool CHogEditView::InsertItem(int index, hog_library_entry *entry)
{
	CListCtrl& list=GetListCtrl();
	int p;
	char str[256];

	if((p=list.InsertItem(index,entry->name))==-1)
		OutputDebugString("Didn't work!");

	list.SetItem(p,FILE_DIR_COLUMN,LVIF_TEXT,entry->path,0,0,0,0);

	sprintf(str,"%.1fk",double(entry->length/1024.0));
	list.SetItem(p,FILE_SIZE_COLUMN,LVIF_TEXT,str,0,0,0,0);

	struct tm *newtime;
	newtime = localtime( &entry->timestamp );  /* Convert time to struct */
	sprintf(str,"%s", asctime(newtime));
	str[strlen(str)-1]='\0';  // Get rid of trailing newline
	list.SetItem(p,FILE_TIME_COLUMN,LVIF_TEXT,str,0,0,0,0);

	GetFileType(str,entry->name);
	list.SetItem(p,FILE_TYPE_COLUMN,LVIF_TEXT,str,0,0,0,0);

	return true;
}

// Inserts all items from the document Library into CListCtrl
void CHogEditView::UpdateList(void)
{
	CHogEditDoc *doc=GetDocument();
	POSITION pos;
	hog_library_entry entry;
	
	// Insert an item into the list for every entry in the document's library
	pos = doc->Library.filelist.GetHeadPosition();
	while (pos)
	{
		entry = doc->Library.filelist.GetNext(pos);
		InsertItem(0,&entry);
	}
}

// Put the title and current document on the title bar
void CHogEditView::UpdateTitle(CString &path, bool mod_flag)
{
	CFrameWnd *main_window;
	char filename[PSFILENAME_LEN+1], ext[12];
	char title[_MAX_PATH+1];

	_splitpath(path.GetBuffer(0), NULL, NULL, filename, ext);
	sprintf(title, "%s (%s%s%s)", HOG_EDIT_TITLE, filename, ext,
		(mod_flag) ? "*" : "");

	if((main_window=GetParentFrame())!=NULL)
		main_window->SetWindowText(title);
	else
		OutputDebugString("Can't get main window!\n");
}

void CHogEditView::OnFileOpen() 
{
	// TODO: Add your command handler code here
	CHogEditDoc *doc=GetDocument();
	CListCtrl& list=GetListCtrl();
	CString open_filename;

	// Set full row selection
	SetFullRowSel(m_bFullRowSel);

	// If current file has been modified, check with user first...
	if(ModifiedPrompt(this)==IDNO) return;	

	if(!DoAutoProcess) {
		// Create Open File Box
		CString filter = "Outrage hog template (*.rib)|*.rib|Outrage hogfiles (*.hog)|*.hog|All Files (*.*)|*.*||";
		CFileDialog dlg_open(TRUE, NULL, NULL, OFN_FILEMUSTEXIST, (LPCTSTR)filter, this);
		if (dlg_open.DoModal() == IDCANCEL) {
			return;
		}
		open_filename=dlg_open.GetPathName();
	}
	else {
		open_filename=AutoRibFilename;
	}

	// Display Wait Cursor
	CWaitCursor wc;

	// Clear out the document's information
	doc->OnNewDocument();

	// Wipeout the CtrlList items
	list.DeleteAllItems();

	// Load the document, and fill the control list with the new entries
	if(!doc->LoadDocument(open_filename))
		MessageBox("The file was not opened successfully.",
				   "File Read Error");

	// Refresh the CListCtrl with the new loaded items
	UpdateList();

	// Put new doc name on title bar
	doc->m_DocumentName = open_filename;
	doc->m_NoNameChosen = FALSE;
	UpdateTitle(doc->m_DocumentName,DocModified);
	UpdateTotalFiles(list.GetItemCount());

	// If we're automatically processing the rib file, go straight to the
	// hog file creation process
	if(DoAutoProcess)
		OnActionCreate();

}

void CHogEditView::OnFileSave() 
{
	// TODO: Add your command handler code here
	CHogEditDoc *doc=GetDocument();
	CListCtrl& list=GetListCtrl();

	// If the current document is a hog file, then call the hog
	// creation routine to save it
	if(doc->m_StaticHog) {
		UseCurrentHogFilename=TRUE;
		OnActionCreate();
		UseCurrentHogFilename=FALSE;
		return;
	}

	// If a filename has not yet been specified by the user,
	// Make them do a "Save As" instead
	if(doc->m_NoNameChosen) { 
		OnFileSaveAs();
		return;
	}

	// Display Wait Cursor
	CWaitCursor wc;

	if(!doc->SaveDocument(doc->m_DocumentName))	{
		MessageBox("The file was not written successfully.",
				   "File Write Error");
		return;
	}

	DocModified=false;
	UpdateTitle(doc->m_DocumentName,DocModified);
	UpdateTotalFiles(list.GetItemCount());
}

void CHogEditView::OnFileSaveAs() 
{
	
	// TODO: Add your command handler code here
	CHogEditDoc *doc=GetDocument();
	CListCtrl& list=GetListCtrl();

	// If the current document is a hog file, then call the hog
	// creation routine to save it
	if(doc->m_StaticHog) {
		OnActionCreate();
		return;
	}
	
	CString filter = "Outrage hog template (*.rib)|*.rib||";
	CFileDialog dlg_save(FALSE, ".rib", NULL,0, (LPCTSTR)filter, this);

	if (dlg_save.DoModal() == IDCANCEL) {
		return;
	}

	// Display Wait Cursor
	CWaitCursor wc;

	//	save out rib
	if(!doc->SaveDocument(dlg_save.GetPathName())) {
		MessageBox("The file was not written successfully.",
				   "File Write Error");
		return;
	}

	//	name dialog
	doc->m_DocumentName = dlg_save.GetPathName();

	// Put new doc name on title bar
	doc->m_NoNameChosen=FALSE;
	DocModified=FALSE;
	UpdateTitle(doc->m_DocumentName,DocModified);

	UpdateTotalFiles(list.GetItemCount());
}

void CHogEditView::OnActionDelete() 
{
	// TODO: Add your command handler code here
	CHogEditDoc *doc=GetDocument();
	CListCtrl& list=GetListCtrl();
	int j, num_items;
	LV_ITEM item_info;
	POSITION pos, last_pos;
	hog_library_entry entry;

	char name[PSFILENAME_LEN+1];
	char path[_MAX_PATH+1];

	// Display Wait Cursor
	CWaitCursor wc;

	// How many items in the control list?
	num_items=list.GetItemCount();
	
	j=0;
	while(j<num_items) {

		// Grab the info for item #j
		item_info.iItem=j;
		item_info.iSubItem=0;
		item_info.mask= LVIF_TEXT | LVIF_STATE;
		item_info.stateMask=LVIS_SELECTED;
		item_info.pszText=name;
		item_info.cchTextMax=PSFILENAME_LEN+1;
		list.GetItem(&item_info);

		// Is item at index j selected?		
		if(item_info.state & LVIS_SELECTED) {	// yes, delete it
			
			// Get Directory name for item #j
			item_info.iSubItem=FILE_DIR_COLUMN;
			item_info.pszText=path;
			item_info.cchTextMax=_MAX_PATH;
			list.GetItem(&item_info);

			// Search for and remove corresponding entry from Document Library
			pos = doc->Library.filelist.GetHeadPosition();
			while (pos!=NULL)
			{
				last_pos=pos;
				entry = doc->Library.filelist.GetNext(pos);
				if (!stricmp(entry.name, name) && !stricmp(entry.path, path)) { 
					doc->Library.filelist.RemoveAt(last_pos);
					break;
				}
			}
			
			// Delete entry/item from control list
			list.DeleteItem(j);
			num_items=list.GetItemCount();

			// Document has been modified
			DocModified=true;
		}
		else
			j++;
	}

	// Update title bar
	if(DocModified)
		UpdateTitle(doc->m_DocumentName,DocModified);

	UpdateTotalFiles(list.GetItemCount());
}

void CHogEditView::OnActionCreate() 
{
	// TODO: Add your command handler code here
	CListCtrl& list=GetListCtrl();
	CHogEditDoc *doc=GetDocument();
	char **hog_filenames;
	int ret, num_files;
	char title[]="Save Hog File As";
	CString hog_name;
	bool check_name;

	num_files=doc->Library.filelist.GetCount();
	if(num_files<=0) return;

	check_name=FALSE;
	if(!DoAutoProcess) {

		if(!UseCurrentHogFilename) {
			CString filter = "Outrage hog file (*.hog)|*.hog|All files (*.*)|*.*||";
			CFileDialog dlg_save(FALSE, NULL, doc->Library.filename, 0, (LPCTSTR)filter, this);

			dlg_save.m_ofn.lpstrTitle=title;

			// Hog file name box
			if (dlg_save.DoModal() == IDCANCEL) {
				return;
			}

			hog_name = dlg_save.GetPathName();

			// If no extension was specified, add ".hog" by default
			if(strchr(hog_name.GetBuffer(0),'.')==NULL)
				hog_name+=".hog";

			check_name=TRUE;
		}
		else {
			hog_name = doc->m_DocumentName;
			UseCurrentHogFilename=FALSE;
		}

		// Fill and display the Hog Info dialog
		CHogInfo info_dlg;

		info_dlg.m_NumFiles.Format("%d",num_files);
		info_dlg.m_SizeBytes.Format("%d bytes",doc->CalcHogFileSize());
		info_dlg.m_SizeMB.Format("(%.2f MB)",double(doc->CalcHogFileSize())/double(1024*1024));
		info_dlg.m_Location.Format("%s",hog_name);

		if(info_dlg.DoModal()==IDCANCEL) return;
	
	}
	else {
		hog_name = AutoHogFilename;
	}

	// Display Wait Cursor
	CWaitCursor wc;

	if(check_name) {
		if(stricmp(doc->Library.filename,hog_name.GetBuffer(0))!=0) {
			DocModified=TRUE;
			UpdateTitle(doc->m_DocumentName,DocModified);
		}
	}

	// name dialog
	strcpy(doc->Library.filename,hog_name.GetBuffer(0));

	// Create the list of filenames (w/full paths) to be in the hog file
	if(!doc->CreateFilenameList(&hog_filenames))
		return;

	// Sort the filenames
	doc->SortFilenameList(hog_filenames);

	/*
	// Display the list of names (to check sorting)
	for(int j=0; j<doc->Library.num_entries; j++) {
		char full_name[_MAX_PATH];
		sprintf(full_name,"%d <-> %s\n",j,hog_filenames[j]);
		OutputDebugString(full_name);
	}
	*/

	// Try to create the hog file and display results in message
	WaitCursor=&wc;

	ret=doc->CreateNewHogFromCurrentHog(
		doc->m_DocumentName.GetBuffer(0),
		doc->Library.filename,
		doc->Library.num_entries,
		(const char **)hog_filenames,
		UpdateStatusBar);

	/*
	ret=CreateNewHogFile( 
		doc->Library.filename,
		doc->Library.num_entries,
		(const char **)hog_filenames,
		UpdateStatusBar);
	*/
	WaitCursor=NULL;

	if(ret!=HOGMAKER_OK) {
		char msg[1024];
		switch(ret) {
			case HOGMAKER_ERROR:
				sprintf(msg,"No files were chosen for the HOG file!\n");
				break;
			case HOGMAKER_MEMORY:
				sprintf(msg,"Out of memory error!\n");
				break;
			case HOGMAKER_OPENOUTFILE:
				sprintf(msg,"\"%s\" could not be opened!\n",hogerr_filename);
				break;
			case HOGMAKER_OUTFILE:
				sprintf(msg,"An error occurred writing to \"%s\"!\n",hogerr_filename);
				sprintf(msg,"");
				break;
			case HOGMAKER_INFILE:
				sprintf(msg,"The file \"%s\" could not be opened!\n",hogerr_filename);
				break;
			case HOGMAKER_COPY:
				sprintf(msg,"An error occurred copying from \"%s\"!\n",hogerr_filename);
				break;
			default:
				sprintf(msg,"An unidentified error has occurred!\n");
				break;
		}
		strcat(msg,"\nThe HOG file could not be created.");
		MessageBox(
			msg,
			"HOG File Error!");
	}
	else if(!DoAutoProcess)
		MessageBox(
			"The HOG file was created successfully.\n",
			"HOG File Created");

	// Cleanup the filename list
	doc->DeleteFilenameList(hog_filenames);
	UpdateTotalFiles(list.GetItemCount());

	// If a hog has been saved, reload it and update the title bar
	if(doc->m_StaticHog) {

		CString current_filename=doc->Library.filename;

		CWaitCursor wc;

		// Clear out the document's information
		doc->OnNewDocument();

		// Wipeout the CtrlList items
		list.DeleteAllItems();

		// Set the document's name
		doc->m_DocumentName = current_filename;

		// Load the document, and fill the control list with the new entries
		if(!doc->LoadDocument(doc->m_DocumentName))
			MessageBox("The file was not opened successfully.",
					   "File Read Error");

		// Refresh the CListCtrl with the new loaded items
		UpdateList();

		// Put new doc name on title bar
		DocModified=FALSE;
		UpdateTitle(doc->m_DocumentName,DocModified);

		UpdateTotalFiles(list.GetItemCount());
	}

	// Close down hogedit if auto-processing
	if(DoAutoProcess) {
		CFrameWnd *main_window;

		if((main_window=GetParentFrame())!=NULL)
			main_window->PostMessage(WM_CLOSE);
	}
}


void CHogEditView::OnViewFullRow() 
{
	// TODO: Add your command handler code here
	m_bFullRowSel = (m_bFullRowSel) ? false : true;
	SetFullRowSel(m_bFullRowSel);
}

void CHogEditView::OnUpdateViewFullRow(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(m_bFullRowSel);
}


// will notify user if any files in .rib have been modified (may need to rebuild .hog)
void CHogEditView::OnActionUpdate() 
{
	// TODO: Add your command handler code here
	CHogEditDoc *doc=GetDocument();
	CListCtrl& list=GetListCtrl();
	POSITION pos, prev_pos;
	hog_library_entry temp_entry;
	int res;
	char msg[256];
	char full_name[_MAX_PATH];
	int missing;
	int modified;
	bool show_mod_msgs, show_gone_msgs;

	// Check each file in the hog entry library
	missing=0;
	modified=0;
	show_mod_msgs=true;
	show_gone_msgs=true;

	// Display wait cursor
	CWaitCursor wc;

	pos = doc->Library.filelist.GetHeadPosition();
	while (pos!=NULL)
	{
		prev_pos=pos;
		temp_entry = doc->Library.filelist.GetNext(pos);
		res=doc->UpdatedFileCheck(&temp_entry);

		sprintf(full_name,"%s%s",temp_entry.path,temp_entry.name);
		if(res==FILE_IS_GONE) {
			if(show_gone_msgs) {
				sprintf(msg,"The following file could not be found:\n\n%s",full_name);
				if(MessageBox(
					msg,
					"Warning! File Missing",
					MB_OKCANCEL)==IDCANCEL) show_gone_msgs=false;
				wc.Restore();
			}
			missing++;
		}
		else if(res==FILE_HAS_CHANGED) {
			if(show_mod_msgs) {
				sprintf(msg,"The following file has been modified:\n\n%s",full_name);
				if(MessageBox(
					msg,
					"A File Has Changed",
					MB_OKCANCEL)==IDCANCEL) show_mod_msgs=false;
				wc.Restore();
			}
			doc->Library.filelist.SetAt(prev_pos,temp_entry);
			modified++;
		}
	}

	// Refresh list display if necessary
	if(modified) {
		list.DeleteAllItems();
		UpdateList();
		DocModified=true;
	}

	// Display update status message
	char str[_MAX_PATH];
	sprintf(str,"%s\n\n%s%d\n\n%s%d",
		"Here are the results for the update:",
		"Missing file(s):\t\t", missing,
		"Modified file(s):\t\t", modified);

	MessageBox(str,
	    "Update Results");

	// Update title bar
	if(DocModified)
		UpdateTitle(doc->m_DocumentName,DocModified);

	UpdateTotalFiles(list.GetItemCount());
}

// Copies a file from the hogfile to disk
void CHogEditView::OnActionExtract() 
{
	// TODO: Add your command handler code here
	CHogEditDoc *doc=GetDocument();
	CListCtrl& list=GetListCtrl();
	int j, num_items;
	LV_ITEM item_info;
	POSITION pos;
	hog_library_entry entry;
	char name[PSFILENAME_LEN+1];
	static CString default_dir_path="";
	CDirDialog dir_dialog;
	char current_dir[_MAX_PATH+1];

	CString output_msg, output_title;
	int num_errors;

	// Setup and display the directory selection dialog
	if(!default_dir_path.IsEmpty())
		dir_dialog.m_strSelDir=default_dir_path;
	dir_dialog.m_strWindowTitle="Destination Directory";
	dir_dialog.m_strTitle="Select the directory that you would like to extract the selected files into:";
	if(dir_dialog.DoBrowse(this)==FALSE) return;

	// Try to change to the specified directory
	_getcwd(current_dir,_MAX_PATH);
	if ( _chdir(dir_dialog.m_strPath) ) {
		CString error_msg;
		error_msg.Format("Unable to find directory: %s",dir_dialog.m_strPath);
		MessageBox(error_msg, "Error", MB_ICONHAND);
		_chdir(current_dir);
		return;
	}

	// Display Wait Cursor
	CWaitCursor wc;

	// How many items in the control list?
	num_items=list.GetItemCount();

	num_errors=0;

	j=0;
	while(j<num_items) {

		// Grab the info for item #j
		item_info.iItem=j;
		item_info.iSubItem=0;
		item_info.mask= LVIF_TEXT | LVIF_STATE;
		item_info.stateMask=LVIS_SELECTED;
		item_info.pszText=name;
		item_info.cchTextMax=PSFILENAME_LEN+1;
		list.GetItem(&item_info);

		// Is item at index j selected?		
		if(item_info.state & LVIS_SELECTED) {

			// Search for corresponding entry in Document Library
			pos = doc->Library.filelist.GetHeadPosition();
			while (pos!=NULL)
			{
				entry = doc->Library.filelist.GetNext(pos);
				if (!stricmp(entry.name, name)) { 

					if(!doc->ExtractFile(entry.offset,entry.length,entry.name)) {
						if(num_errors==0) {
							output_msg.Format("The following files were not successfully extracted:\n\n%s",entry.name);
						}
						else {
							CString temp_str;
							temp_str.Format(", %s",entry.name);
							output_msg+=temp_str;
						}
						num_errors++;
					}

					/*
					// Prompt user for file name
					char title[]="Save Extracted File As";
					CString filter = "All files (*.*)|*.*||";
					
					CFileDialog dlg_save(FALSE, NULL, name,0, (LPCTSTR)filter, this);
					dlg_save.m_ofn.lpstrTitle=title;

					if (dlg_save.DoModal() == IDCANCEL) {
						break;
					}
					
					wc.Restore();

					CString out_fname = dlg_save.GetPathName();
					if(doc->ExtractFile(entry.offset,entry.length,out_fname.GetBuffer(0)))
						MessageBox(
							"The file was extracted successfully.",
							"File Extraction Successful");
					else
						MessageBox(
							"The file could not be extracted.",
							"File Extraction Error");
					break;

					wc.Restore();
					*/
				}
			}
		}
		j++;
	}

	// Save the selected dir for next time
	default_dir_path=dir_dialog.m_strPath;

	// Change back to the original directory
	_chdir(current_dir);

	// Display the results
	if(num_errors==0) {
		output_msg="The selected files were extracted successfully.";
		output_title="File Extraction Successful";
	}
	else {
		output_title="File Extraction Error";
	}
	MessageBox(output_msg,output_title);
}

void CHogEditView::OnUpdateActionDelete(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	CHogEditDoc *doc=GetDocument();
	CListCtrl& list=GetListCtrl();

	pCmdUI->Enable(/*!doc->m_StaticHog &&*/ list.GetSelectedCount()>0);	
}

void CHogEditView::OnUpdateActionExtract(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	CHogEditDoc *doc=GetDocument();
	CListCtrl& list=GetListCtrl();

	pCmdUI->Enable(doc->m_StaticHog && list.GetSelectedCount()>0);		
}

void CHogEditView::OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	
	//OutputDebugString("A column was clicked\n");

	*pResult = 0;
}

void CHogEditView::OnActionImport() 
{
	// TODO: Add your command handler code here
	static CString default_dir_path="";
	CDirDialog dir_dialog;

	// Setup and display the directory selection dialog
	if(!default_dir_path.IsEmpty())
		dir_dialog.m_strSelDir=default_dir_path;
	dir_dialog.m_strWindowTitle="Import Directory";
	dir_dialog.m_strTitle="Select the directory which contains all the files that you would like to import:";
	if(dir_dialog.DoBrowse(this)==FALSE) return;

	OutputDebugString(dir_dialog.m_strPath);
	OutputDebugString("\n");

	// Process the directory
	CListCtrl& list=GetListCtrl();
	CHogEditDoc *doc=GetDocument();
	hog_library_entry entry;
	int rv;
	CString path;

	// Set full row selection
	SetFullRowSel(m_bFullRowSel);


	// Display wait cursor
	CWaitCursor wc;
	ProcessMessages();
	wc.Restore();

	struct _finddata_t cfiletype;
	int hfile;
	int findnext_ret;
	char current_dir[_MAX_PATH+1];

	// Check and see if the directory is valid
	_getcwd(current_dir,_MAX_PATH);
	if ( _chdir(dir_dialog.m_strPath) ) {
		CString error_msg;
		error_msg.Format("Unable to find directory: %s",dir_dialog.m_strPath);
		MessageBox(error_msg, "Error", MB_ICONHAND);
		_chdir(current_dir);
		return;
	}

	// Get the first file
	hfile = _findfirst("*.*", &cfiletype);
	if ( hfile == -1 )
		return;

	//	iterate through filename list.
	findnext_ret=0;
	while (findnext_ret==0)
	{
		// If it's current/previous dir, then get next file
		if ( !strcmp(cfiletype.name, ".") || !strcmp(cfiletype.name, "..") ) {
			findnext_ret=_findnext(hfile, &cfiletype);
			continue;
		}

		// If it's a sub dir, then get next file
		if ( cfiletype.attrib & _A_SUBDIR ) {
			findnext_ret=_findnext(hfile, &cfiletype);
			continue;
		}

		// Create the full file path
		path.Format("%s\\%s",dir_dialog.m_strPath,cfiletype.name);
	
		// Add the file to the list
		rv=doc->AddFile(path,&entry);
		if(rv==ADDFILE_OK) {
			InsertItem(0,&entry);
			UpdateTotalFiles(list.GetItemCount());
			wc.Restore();	// Get the Wait cursor back
		}
		else {
			char msg[1024];
			switch(rv) {
				case ADDFILE_LONG_FNAME_ERROR:
					sprintf(msg,"The following filename exceeded %d characters:\n",
							PSFILENAME_LEN);
					break;
				case ADDFILE_DUP_FILE_ERROR:
					findnext_ret=_findnext(hfile, &cfiletype);
					continue;
					//sprintf(msg,"The following filename is already in the list:\n");
					//break;
				case ADDFILE_STAT_ERROR:
					sprintf(msg,"Could not get stats for following file:\n");
					break;
				default:
					sprintf(msg,"An unidentified error has occurred with the following file:\n");
					break;
			}
			strcat(msg,path);
			strcat(msg,"\n\nThis file will not be added to the file list.");
			MessageBox(
				msg,
				"Insert File Error!");

			wc.Restore();	// Get the Wait cursor back
		}

		// Get the next file
		findnext_ret=_findnext(hfile, &cfiletype);
	}
	_findclose( hfile );
	_chdir(current_dir);
	default_dir_path=dir_dialog.m_strPath;

	// Update title bar
	if(DocModified)
		UpdateTitle(doc->m_DocumentName,DocModified);

	UpdateTotalFiles(list.GetItemCount());

}

LONG CHogEditView::OnStraightToFileOpen(UINT, LONG)
{	
	OnFileOpen();

	return 0;
}
