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

#ifndef _TABLE_FILE_FILTER_MNG_H_
#define _TABLE_FILE_FILTER_MNG_H_

#include "pstypes.h"
#include "manage.h"

#define SOURCE_TABLE_FILENAME		"F:\\Descent3\\data\\tables\\table.gam"

#define DEFAULT_TABLEPARSE_FNAME	"main\\table_parse.out"

// Control Column header info
#define PAGE_NAME_COLUMN		0
#define PAGE_TYPE_COLUMN		1

#define PAGE_NAME_COLUMN_WIDTH		0.7
#define PAGE_TYPE_COLUMN_WIDTH		0.3

// Data Node Flags
#define ADDED_FOR_LEVEL_FLAG	0x01
#define ADDED_FOR_USER_FLAG		0x02
#define PAGE_WRITTEN_FLAG		0x04

// Page Processing types
#define ADD_PAGES		0
#define REMOVE_PAGES	1

#define ALWAYS_WRITE_GAMEFILE_PAGES	0

///////////////////////
// Page Data Node class
///////////////////////
class PageDataNode {
friend class PageDataList;
private:
	PageDataNode *prev;
	PageDataNode *next;

	// The page data
	uint8_t type;					// stores the page type
	char name[PAGENAME_LEN];	// stores the page name

	int flags;		// Stores whether file was added from level (or just added as extra)
	int tag_count;	// Stores number of level files requiring this page

public:
	PageDataNode::PageDataNode();
	PageDataNode::~PageDataNode();

	bool operator < (const PageDataNode &node);

	bool Load(CFILE *infile);
	bool Save(CFILE *outfile);
};


///////////////////////////////////////////////////////////
// Generic Page List class (maintains linked list of pages)
///////////////////////////////////////////////////////////

#define MAX_LEVEL_FILENAMES	256

class PageDataList {
private:
	PageDataNode *m_head;
	PageDataNode *m_tail;
	int m_size;

	char m_LevelFilenames[MAX_LEVEL_FILENAMES][_MAX_PATH];
	int m_NumLevelFilenames;

	// Dialog controls
	CDialog		*m_Dlg;
	CEdit			*m_TableParseFile;
	CListCtrl	*m_PageDataListCtrl;	
	CListBox		*m_LevelListBox;		
	CString		*m_NumPagesText;
	CString		*m_StatusText;

	CString m_DataListFilename;
	bool	m_DataListLoaded;
	bool	m_DataListModified;

public:
	PageDataList::PageDataList();
	PageDataList::~PageDataList();

	void SetModified(bool mMod) { m_DataListModified=mMod; }
	bool IsModified(void) { return(m_DataListModified); }
	bool IsLoaded(void) { return(m_DataListLoaded); }
	bool FilenameSpecified(void) { return(!m_DataListFilename.IsEmpty()); }

	void ClearList(void);
	bool AddToList(PageDataNode *node);
	bool AddToList(char *page_name, uint8_t page_type, int page_flags);
	bool RemoveFromList(PageDataNode *goner_node);
	bool RemoveFromList(char *page_name, uint8_t page_type);
	PageDataNode *FindNode(char *page_name, uint8_t page_type);

	bool LoadList(char *list_filename);
	bool SaveList(char *list_filename);

	bool AddLevelFilename(char *level_filename);
	bool RemoveSelLevelFilename(void);

	bool ProcessPagesForLevel(char *level_filename, int process_type);
	bool DetermineRequiredPages(int process_type);
	bool ProcessPage(char *page_name, uint8_t page_type, int process_type, int flags=ADDED_FOR_LEVEL_FLAG);

	void AddTexturePage (int id,int process_type);
	void AddSoundPage (int id,int process_type);
	void AddGenericPage (int id,int process_type);
	void AddWeaponPage (int id,int process_type);
	void AddDoorPage (int id,int process_type);
	void AddShipPage (int id,int process_type);

	bool AddPageFromUser(char *page_name, uint8_t page_type);
	bool ProcessPagesInTextFile(char *filename, int process_type);
	bool ProcessPageFromFile(char *page_name, uint8_t page_type, int process_type);
	uint8_t GetPageTypeFromString(char *text);

	bool CreateNewTableFile(char *new_table_filename, char *src_table_filename);

	// Dialog control functions
	void BindDlg(CDialog *dlg) { m_Dlg=dlg; }
	void BindOutputFileEdit(CEdit *edit) { m_TableParseFile=edit; }
	void BindPageDataListCtrl(CListCtrl *list) { m_PageDataListCtrl=list; }
	void BindLevelListBox(CListBox *box) { m_LevelListBox=box; }
	void BindNumPagesText(CString *str) { m_NumPagesText=str; }
	void BindStatusText(CString *str) { m_StatusText=str; }

	bool AddAllGamefilePages(void);

	bool AddToListCtrl(int index, PageDataNode *node);
	bool RemoveSelFromListCtrl(void);

	int ModifiedPrompt(void);
	void SetTitleString(void);
};

#endif /* _TABLE_FILE_FILTER_MNG_H_ */