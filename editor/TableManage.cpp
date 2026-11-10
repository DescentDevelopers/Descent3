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

#include "stdafx.h"
#include "resource.h"
#include "TableFileEdit.h"

#include "PsTypes.h"
// #include "descent.h"
#include "TableManage.h"
#include "manage.h"
// #include "pserror.h"
// #include "gametexture.h"
// #include "texpage.h"
// #include "doorpage.h"
// #include "soundpage.h"
// #include "megapage.h"
// #include "shippage.h"
// #include "weaponpage.h"
// #include "gamefilepage.h"
#include "mono.h"
#include "genericpage.h"
// #include "object.h"
// #include "ddio.h"
#include "cfile.h"
// #include "AppDatabase.h"
#include "mem.h"
#include "pserror.h"

////////////////////////////////////////////////////////////////
// GenericPageNode Class Members
////////////////////////////////////////////////////////////////
GenericPageNode::GenericPageNode() {
  prev = nullptr;
  page_id = 0;
}

GenericPageNode::~GenericPageNode() {
  // Free up the object description memory
  genericpage.objinfo_struct.description.reset();
}

bool GenericPageNode::operator<(const GenericPageNode &node) {
  return (stricmp(genericpage.objinfo_struct.name, node.genericpage.objinfo_struct.name) < 0);
}

////////////////////////////////////////////////////////////////
// GenericPageList Class Members
////////////////////////////////////////////////////////////////
GenericPageList::GenericPageList() {
  m_tail = nullptr;
  m_size = 0;
  m_TableFilename = "";
  m_TableLoaded = FALSE;
  m_TableModified = FALSE;
  m_SelectedNode = nullptr;
  m_SelectedIndex = 0;
}

GenericPageList::~GenericPageList() { ClearList(); }

void GenericPageList::ClearList(void) {
  // Unlink one node at a time so a long list isn't destroyed recursively
  while (m_head != nullptr) {
    m_head = std::move(m_head->next);
  }
  m_size = 0;
  m_tail = nullptr;
  m_TableFilename = "";
  m_TableLoaded = FALSE;
  m_TableModified = FALSE;
  m_SelectedNode = nullptr;
  m_SelectedIndex = 0;
}

// Adds the node to the list in ascending alphabetical order
// according to the page names of the nodes
bool GenericPageList::AddToList(std::unique_ptr<GenericPageNode> new_node) {
  GenericPageNode *node;

  if (new_node == nullptr)
    return FALSE;

  // Handle empty list case
  if (m_head == nullptr) {
    m_tail = new_node.get();
    new_node->prev = nullptr;
    m_head = std::move(new_node);

    m_size++;
    return TRUE;
  }

  // Handle (new node < list head) case
  if ((*new_node) < (*m_head)) {
    new_node->prev = nullptr;
    m_head->prev = new_node.get();
    new_node->next = std::move(m_head);
    m_head = std::move(new_node);

    m_size++;
    return TRUE;
  }

  // Search through list until end of list or until the new node is less
  // than the next node, then add the new node after the current node
  node = m_head.get();
  while (node->next != nullptr && (*(node->next)) < (*new_node)) {
    node = node->next.get();
  }

  new_node->prev = node;

  if (node->next == nullptr) {
    m_tail = new_node.get();
  } else {
    node->next->prev = new_node.get();
  }

  new_node->next = std::move(node->next);
  node->next = std::move(new_node);

  m_size++;

  return TRUE;
}

bool GenericPageList::LoadTable(char *table_filename) {
  CFILE *infile;
  uint8_t pagetype;
  int done;
  uint32_t page_id;

  std::unique_ptr<GenericPageNode> generic_page_node;

  infile = cfopen(table_filename, "rb");

  if (!infile) {
    // mprintf(0,"Couldn't open table file to reorder!\n");
    // Int3();
    return FALSE;
  }

  CWaitCursor wc;

  // Wipeout the list first
  ClearList();

  done = 0;
  page_id = 0;

  while (!done) {
    if (cfeof(infile)) {
      done = 1;
      continue;
    }
    pagetype = cf_ReadByte(infile);

    // If not a generic page, just read it in and ignore it
    if (pagetype != PAGETYPE_GENERIC) {
      mng_ReadDummyPage(infile, pagetype);
      page_id++;
      continue;
    }

    generic_page_node = std::make_unique<GenericPageNode>();
    mng_ReadNewGenericPage(infile, &(generic_page_node->genericpage));
    generic_page_node->page_id = page_id;
    AddToList(std::move(generic_page_node));

    page_id++;
  }

  cfclose(infile);

  // Set Loaded tabel data
  m_TableFilename = table_filename;
  m_TableLoaded = TRUE;

  m_TableModified = FALSE;

  return TRUE;
}

bool GenericPageList::SaveTable(char *table_filename) {
  CFILE *infile, *outfile;
  uint8_t pagetype, replaced = 0;
  int done = 0;
  uint32_t page_id;
  GenericPageNode *new_generic_page;
  mngs_generic_page genericpage;

  CWaitCursor wc;

  // If no table is loaded, get outta here
  if (!m_TableLoaded)
    return FALSE;

  // First make sure we can open the table file and the temp table file
  infile = cfopen(m_TableFilename.GetBuffer(0), "rb");
  if (!infile) {
    mprintf(0, "Couldn't open table file to replace generic!\n");
    return FALSE;
  }

  outfile = cfopen(TEMP_TABLE_FILENAME, "wb");
  if (!outfile) {
    mprintf(0, "Couldn't open temp table file to replace generic!\n");
    cfclose(infile);
    return FALSE;
  }

  // Clear out the generic page
  genericpage = {};

  // Read through the entire pagefile and replace the page we want replaced
  // If we can't find the one we want, we simply append it to the end of the file
  page_id = 0;
  while (!done) {
    if (cfeof(infile)) {
      done = 1;
      continue;
    }
    pagetype = cf_ReadByte(infile);

    // If not a generic page, just read it in and write it right back out
    if (pagetype != PAGETYPE_GENERIC) {
      mng_ReadWriteDummyPage(infile, outfile, pagetype);
      page_id++;
      continue;
    }

    mng_ReadNewGenericPage(infile, &genericpage);

    // Look for the generic page with the current page position in list
    new_generic_page = FindNode(page_id);
    if (new_generic_page == nullptr) {
      // There is a page incompatibility!!!!
      // so, just write out the one that was read instead
      mng_WriteNewGenericPage(outfile, &genericpage);
      OutrageMessageBox("File incompatibility Error!", "Error!");
    } else {
      // Found the page for this position in the list,
      // So write out the new generic page
      mng_WriteNewGenericPage(outfile, &(new_generic_page->genericpage));
    }

    // Free up the description memory for the temp page
    genericpage.objinfo_struct.description.reset();

    page_id++;
  }

  cfclose(infile);
  cfclose(outfile);

  if (table_filename == nullptr)
    // if no filename specified, just save it to the currently loaded file
    table_filename = m_TableFilename.GetBuffer(0);
  else {
    // We need to create this one just so it can get deleted alright by
    // Jason's Switcheroo function
    outfile = cfopen(table_filename, "wb");
    if (!outfile)
      return FALSE;
    cfclose(outfile);
  }

  // Now, perform the switcheroo by deleting the real table file and renaming the
  // temp table file to be the new "real" table file.
  if (!SwitcherooFiles(table_filename, TEMP_TABLE_FILENAME))
    return FALSE;

  m_TableModified = FALSE;
  m_TableFilename = table_filename;

  return TRUE; // successful!
}

bool GenericPageList::FillFromList(CComboBox *list) {
  GenericPageNode *node;

  list->ResetContent();

  node = m_head.get();
  while (node != nullptr) {
    list->InsertString(-1, node->genericpage.objinfo_struct.name);
    node = node->next.get();
  }

  // Select the start of the list
  SelectNode(0);

  return TRUE;
}

void GenericPageList::SelectNext(void) {
  if (m_SelectedNode == nullptr)
    return;

  if (m_SelectedNode->next == nullptr) {
    m_SelectedNode = m_head.get();
    m_SelectedIndex = 0;
  } else {
    m_SelectedNode = m_SelectedNode->next.get();
    m_SelectedIndex++;
  }
}

void GenericPageList::SelectPrev(void) {
  if (m_SelectedNode == nullptr)
    return;

  if (m_SelectedNode->prev == nullptr) {
    m_SelectedNode = m_tail;
    m_SelectedIndex = (m_size - 1);
  } else {
    m_SelectedNode = m_SelectedNode->prev;
    m_SelectedIndex--;
  }
}

void GenericPageList::SelectNode(uint32_t list_index) {
  uint32_t j;
  GenericPageNode *node;

  if (list_index < 0 || list_index > (m_size - 1))
    return;

  node = m_head.get();
  for (j = 0; j < list_index; j++) {
    if (node != nullptr) {
      node = node->next.get();
    }
  }

  m_SelectedNode = node;
  m_SelectedIndex = list_index;
}

void GenericPageList::DisplaySelected(CComboBox *list, CEdit *description) {
  char *text;

  if (m_SelectedNode == nullptr)
    return;

  list->SetCurSel(m_SelectedIndex);

  text = m_SelectedNode->genericpage.objinfo_struct.description.get();
  if (text == nullptr)
    description->SetWindowText(NO_DESCRIPTION_STRING);
  else
    description->SetWindowText(text);
}

void GenericPageList::SaveSelected(CEdit *description) {
  CString new_descrip;
  char *old_text;

  if (m_SelectedNode == nullptr)
    return;

  // Get pointer to old text
  old_text = m_SelectedNode->genericpage.objinfo_struct.description.get();

  // Get Copy of new Description text
  description->GetWindowText(new_descrip);
  std::unique_ptr<char[]> new_text = mem_strdup_unique(new_descrip.GetBuffer(0));

  // Check if new description is empty, if it is, make it NULL
  if (strlen(new_text.get()) == 0 || stricmp(new_text.get(), NO_DESCRIPTION_STRING) == 0) {
    new_text.reset();
  }

  // If both new and old are empty, get outta here
  if (old_text == nullptr && new_text == nullptr) {
    return;
  }

  // If both have descriptions and they match, get outta here
  if (old_text != nullptr && new_text != nullptr) {
    if (strcmp(old_text, new_text.get()) == 0) {
      return;
    }
  }

  // Replace the old text (freeing it) with the new text
  m_SelectedNode->genericpage.objinfo_struct.description = std::move(new_text);
  m_TableModified = TRUE;

  return;
}

GenericPageNode *GenericPageList::FindNode(uint32_t page_id) {
  GenericPageNode *node, *found_node;

  found_node = nullptr;
  node = m_head.get();
  while (node != nullptr) {
    if (node->page_id == page_id) {
      found_node = node;
      break;
    }
    node = node->next.get();
  }

  return (found_node);
}

int GenericPageList::ModifiedPrompt(CDialog *wnd) {
  // If current file has been modified, check with user first...
  if (!m_TableModified)
    return (IDYES);
  if (wnd->MessageBox("Changes to table file will be lost. Proceed?", "Table File Has Been Modified", MB_YESNO) ==
      IDYES)
    return (IDYES);
  return (IDNO);
}

void GenericPageList::SetTitleString(CDialog *wnd) {
  char title[1024];
  char loaded_file[1024];

  if (m_TableLoaded)
    sprintf(loaded_file, " - [%s%s]", m_TableFilename.GetBuffer(0), (m_TableModified) ? "*" : "");
  else
    sprintf(loaded_file, "");

  sprintf(title, "%s%s", TITLE_NAME, loaded_file);
  wnd->SetWindowText(title);
}
