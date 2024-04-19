/*
 * $Logfile: /DescentIII/Main/editor/TableFileFilter.cpp $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:57:39 $
 * $Author: kevinb $
 *
 * 
 *
 * $Log: not supported by cvs2svn $
 * 
 * 8     2/09/99 4:19p Nate
 * Added to the TableFileFilter interface
 * 
 * 7     10/09/98 3:16a Jason
 * more changes for demo
 * 
 * 6     10/08/98 10:48p Nate
 * 
 * 5     10/08/98 10:30a Nate
 * Initial version
 *
 * $NoKeywords: $
 */

// TableFileFilter.cpp : implementation file
//

#include "stdafx.h"
#include "editor.h"
#include "TableFileFilter.h"
#include "TableFileFilterMng.h"
#include "TableFileFilterAddDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CTableFileFilter dialog


TableFileFilter::TableFileFilter(CWnd* pParent /*=NULL*/)
	: CDialog(TableFileFilter::IDD, pParent)
{
	//{{AFX_DATA_INIT(TableFileFilter)
	m_NumPagesText = _T("");
	m_StatusText = _T("");
	//}}AFX_DATA_INIT
}


void TableFileFilter::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(TableFileFilter)
	DDX_Control(pDX, IDC_OUTPUTFILE_EDIT, m_OutputFileEdit);
	DDX_Control(pDX, IDC_TABLEFILTER_LEVEL_LIST, m_LevelList);
	DDX_Control(pDX, IDC_TABLEFILTER_DATA_LIST, m_DataList);
	DDX_Text(pDX, IDC_TABLEFILTER_NUMPAGESTEXT, m_NumPagesText);
	DDX_Text(pDX, IDC_TABLEFILTER_STATUSTEXT, m_StatusText);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(TableFileFilter, CDialog)
	//{{AFX_MSG_MAP(TableFileFilter)
	ON_COMMAND(ID_TABLEFILTER_FILE_EXIT, OnFileExit)
	ON_COMMAND(ID_TABLEFILTER_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_TABLEFILTER_FILE_SAVEAS, OnFileSaveAs)
	ON_COMMAND(ID_TABLEFILTER_FILE_LOAD, OnFileLoad)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_TABLEFILTER_BTN_ADDLEVEL, OnBtnAddLevel)
	ON_BN_CLICKED(IDC_TABLEFILTER_BTN_ADDPAGE, OnBtnAddPage)
	ON_BN_CLICKED(IDC_TABLEFILTER_BTN_CREATENEWTABLEFILE, OnBtnCreateNewTableFile)
	ON_BN_CLICKED(IDC_TABLEFILTER_BTN_REMOVELEVEL, OnBtnRemoveLevel)
	ON_BN_CLICKED(IDC_TABLEFILTER_BTN_REMOVEPAGE, OnBtnRemovePage)
	ON_COMMAND(ID_TABLEFILTER_FILE_NEW, OnFileNew)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// TableFileFilter message handlers
void TableFileFilter::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	if(m_PageList.ModifiedPrompt()==IDYES)
		CDialog::OnClose();
}

BOOL TableFileFilter::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	// Setup the data list columns
	CString column_name;
	column_name="Page Name";
	m_DataList.InsertColumn(PAGE_NAME_COLUMN,column_name.GetBuffer(0),LVCFMT_LEFT,
							-1,PAGE_NAME_COLUMN);
	column_name="Page Type";
	m_DataList.InsertColumn(PAGE_TYPE_COLUMN,column_name.GetBuffer(0),LVCFMT_LEFT,
							-1,PAGE_TYPE_COLUMN);
	SetColumnWidths();

	// Set the default file path for the edit box
	char *d3dir = getenv("D3_LOCAL");
	char LocalD3Dir[_MAX_PATH];

	strcpy(LocalD3Dir,d3dir);

	if(d3dir[strlen(d3dir)-1] != '\\' ){
		strcat(LocalD3Dir,"\\");
	}
	strcat(LocalD3Dir,DEFAULT_TABLEPARSE_FNAME);

	m_OutputFileEdit.SetWindowText(LocalD3Dir);

	// Link the page data control to the data list
	m_PageList.BindDlg(this);
	m_PageList.BindOutputFileEdit(&m_OutputFileEdit);
	m_PageList.BindPageDataListCtrl(&m_DataList);
	m_PageList.BindLevelListBox(&m_LevelList);
	m_PageList.BindNumPagesText(&m_NumPagesText);
	m_PageList.BindStatusText(&m_StatusText);

	// Wipe out the list
	OnFileNew();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

// Calculates column widths and sets them
void TableFileFilter::SetColumnWidths(void)
{
	unsigned int w0, w1;

	RECT rect;
    m_DataList.GetClientRect(&rect);

	w0=unsigned int(rect.right*PAGE_NAME_COLUMN_WIDTH);
	w1=unsigned int(rect.right*PAGE_TYPE_COLUMN_WIDTH);

	m_DataList.SetColumnWidth(PAGE_NAME_COLUMN,w0);
	m_DataList.SetColumnWidth(PAGE_TYPE_COLUMN,w1);
}

/////////////////////////////////////////////////////
// Menu Command Handlers
void TableFileFilter::OnFileExit() 
{
	// TODO: Add your command handler code here
	if(m_PageList.ModifiedPrompt()==IDYES)
		EndDialog(0);	
}

void TableFileFilter::OnFileNew() 
{
	// TODO: Add your command handler code here
	if(m_PageList.ModifiedPrompt()==IDNO)
		return;

	CWaitCursor wc;
	m_PageList.ClearList();
	//m_PageList.AddAllGamefilePages();
	m_PageList.SetTitleString();	
}

void TableFileFilter::OnFileSave() 
{
	// TODO: Add your command handler code here
	if(!m_PageList.FilenameSpecified()) {
		OnFileSaveAs();
		return;
	}

	CWaitCursor wc;
	if(!m_PageList.SaveList(NULL)) {
		MessageBox("The save was unsuccessfull","Save Table Error!");
		return;
	}	
	m_PageList.SetTitleString();	
	
}

void TableFileFilter::OnFileSaveAs() 
{
	// TODO: Add your command handler code here
	CString filters = "Page Data List File (*.pdl)|*.pdl|All Files (*.*)|*.*||";

	CFileDialog dlg_open(FALSE, ".pdl", NULL, 0, (LPCTSTR)filters, this);
	if (dlg_open.DoModal() == IDCANCEL) {
		return;
	}

	CWaitCursor wc;
	if(!m_PageList.SaveList(dlg_open.GetPathName().GetBuffer(0))) {
		MessageBox("The save was unsuccessfull","Save Data List Error!");
		return;
	}
	
	m_PageList.SetTitleString();
}

void TableFileFilter::OnFileLoad() 
{
	// TODO: Add your command handler code here
	if(m_PageList.ModifiedPrompt()==IDNO)
		return;

	CString filters = "Page Data List File (*.pdl)|*.pdl|All Files (*.*)|*.*||";

	CFileDialog dlg_open(TRUE, ".pdl", NULL, OFN_FILEMUSTEXIST, (LPCTSTR)filters, this);
	if (dlg_open.DoModal() == IDCANCEL) {
		return;
	}

	CWaitCursor wc;
	if(!m_PageList.LoadList(dlg_open.GetPathName().GetBuffer(0))) {
		MessageBox("The load was unsuccessfull","Load Data List Error!");
		m_PageList.ClearList();
		return;
	}

	m_PageList.SetTitleString();	
}

///////////////////////////////////////////////////////////
// Button Command Handlers
#define MAX_FILENAME_BUFFER	4096
void TableFileFilter::OnBtnAddLevel() 
{
	// TODO: Add your control notification handler code here
	char title[]="Add Level Files";
	char filename_buffer[MAX_FILENAME_BUFFER+1];
	CString path;

	CString filter = "Outrage Level Files (*.d3l)|*.d3l|All Files (*.*)|*.*||";
	CFileDialog dlg_open(TRUE, NULL, NULL, OFN_ALLOWMULTISELECT|OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST, filter.GetBuffer(0), this);

	memset(filename_buffer, '\0', MAX_FILENAME_BUFFER);

	dlg_open.m_ofn.lpstrTitle=title;
	dlg_open.m_ofn.lpstrFileTitle=NULL;
	dlg_open.m_ofn.lpstrFile=filename_buffer;
	dlg_open.m_ofn.nMaxFile=MAX_FILENAME_BUFFER;

	if (dlg_open.DoModal() == IDCANCEL) {
		return;
	}

	CWaitCursor wc;

	//	iterate through and add selected level filenames.
	POSITION pos;
	pos = dlg_open.GetStartPosition();
	while (pos!=NULL)
	{
		path = dlg_open.GetNextPathName(pos);

		if(!m_PageList.AddLevelFilename(path.GetBuffer(0))) {
			//MessageBox("Error: Could not add level!\n","Add Level Error!");
			//m_PageList.ClearList();
			//return;
		}
	}

	m_PageList.SetTitleString();	
}


void TableFileFilter::OnBtnAddPage() 
{
	// TODO: Add your control notification handler code here
	CTableFileFilterAddDlg dlg;

	int nResponse = dlg.DoModal();
	if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
		return;
	}

	// Add the new page to the list
	if(dlg.page_name.IsEmpty()) return; 

	if(!m_PageList.AddPageFromUser(dlg.page_name.GetBuffer(0), dlg.page_type))
		MessageBox("This page was not added!","Page Add Error!");

	m_PageList.SetTitleString();	
}


void TableFileFilter::OnBtnCreateNewTableFile() 
{
	// TODO: Add your control notification handler code here
	CString filters = "D3 Table File (*.gam)|*.gam|All Files (*.*)|*.*||";
	char title[]="Save New Table File As";

	CFileDialog dlg_open(FALSE, ".gam", NULL, 0, (LPCTSTR)filters, this);
	dlg_open.m_ofn.lpstrTitle=title;
	if (dlg_open.DoModal() == IDCANCEL) {
		return;
	}

	CWaitCursor wc;
	if(!m_PageList.CreateNewTableFile(dlg_open.GetPathName().GetBuffer(0),SOURCE_TABLE_FILENAME)) {
		MessageBox("The save was unsuccessfull","Save Table File Error!");
		return;
	}
	
	m_PageList.SetTitleString();
}


void TableFileFilter::OnBtnRemoveLevel() 
{
	// TODO: Add your control notification handler code here
	m_PageList.RemoveSelLevelFilename();

	m_PageList.SetTitleString();	
}

void TableFileFilter::OnBtnRemovePage() 
{
	// TODO: Add your control notification handler code here
	m_PageList.RemoveSelFromListCtrl();

	m_PageList.SetTitleString();
}

