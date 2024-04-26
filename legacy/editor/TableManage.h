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

#ifndef _TABLE_MANAGE_H_
#define _TABLE_MANAGE_H_

#include "genericpage.h"
#include "pstypes.h"

#define NO_DESCRIPTION_STRING	"<None>"
#define TEMP_TABLE_FILENAME		"TempTable.loc"

// Generic Page Node class
class GenericPageNode {
friend class GenericPageList;
private:
	GenericPageNode *prev;
	GenericPageNode *next;

	mngs_generic_page	genericpage;	// Generic Page Data
	uint				page_id;		// Page's file index (i.e. the page_id'th page in file)

public:
	GenericPageNode::GenericPageNode();
	GenericPageNode::~GenericPageNode();

	bool operator < (const GenericPageNode &node);
};

// Generic Page List class (maintains linked list of pages)
class GenericPageList {
private:
	GenericPageNode *m_head;
	GenericPageNode *m_tail;
	uint m_size;

	CString m_TableFilename;
	bool	m_TableLoaded;

	bool	m_TableModified;

	GenericPageNode *m_SelectedNode;
	uint			m_SelectedIndex;
	
public:
	GenericPageList::GenericPageList();
	GenericPageList::~GenericPageList();

	void SetModified(bool mMod) { m_TableModified=mMod; }
	bool IsModified(void) { return(m_TableModified); }
	bool IsLoaded(void) { return(m_TableLoaded); }

	void ClearList(void);
	bool AddToList(GenericPageNode *node);
	bool LoadTable(char *table_filename);
	bool SaveTable(char *table_filename);
	bool FillFromList(CComboBox *list);

	GenericPageNode *FindNode(uint page_id);

	void SelectNext(void);
	void SelectPrev(void);
	void SelectNode(uint list_index);
	void DisplaySelected(CComboBox *list, CEdit *description);
	void SaveSelected(CEdit *description);

	int ModifiedPrompt(CDialog *wnd);
	void SetTitleString(CDialog *wnd);
};

#endif /* _TABLE_MANAGE_H_ */
