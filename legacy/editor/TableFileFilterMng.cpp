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

 * $Logfile: /DescentIII/Main/editor/TableFileFilterMng.cpp $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:57:39 $
 * $Author: kevinb $
 *
 * 
 *
 * $Log: not supported by cvs2svn $
 * 
 * 49    10/22/99 6:05p Jeff
 * fixed bugs and compiler errors resulting from mac code merge 
 * 
 * 48    4/21/99 4:49p Kevin
 * 
 * 47    4/21/99 4:18p Jason
 * check texture sounds when determining pages
 * 
 * 46    4/08/99 5:11p Nate
 * Added logging of Remaining and Removed Pages during a filtering
 * 
 * 45    3/30/99 4:42p Kevin
 * For Matt T.  -- Silly bug.
 * 
 * 44    3/28/99 4:03p Matt
 * Added code to handle soundsource objects
 * 
 * 43    3/05/99 6:54p Jason
 * fixed table file filter for sound bands
 * 
 * 42    3/04/99 5:27p Jason
 * added matcen stuff to table file
 * 
 * 41    2/19/99 12:01p Jason
 * took out sky band
 * 
 * 40    2/17/99 7:06p Kevin
 * OEM build setup
 * 
 * 39    2/11/99 5:46p Kevin
 * changes for jason
 * 
 * 38    2/11/99 12:50p Kevin
 * Changes for jason -- to get fireball stuff in
 * 
 * 37    2/09/99 4:58p Nate
 * Fixed up the TableParse file stuff
 * 
 * 36    2/09/99 4:20p Nate
 * Added TableParse file support
 * 
 * 35    2/08/99 10:23a Jason
 * made table file filter work again
 * 
 * 34    1/22/99 3:59p Jason
 * added 256x256 textures to help with terrain skies
 * 
 * 33    1/21/99 11:15p Jeff
 * pulled out some structs and defines from header files and moved them
 * into seperate header files so that multiplayer dlls don't require major
 * game headers, just those new headers.  Side effect is a shorter build
 * time.  Also cleaned up some header file #includes that weren't needed.
 * This affected polymodel.h, object.h, player.h, vecmat.h, room.h,
 * manage.h and multi.h
 * 
 * 32    1/20/99 10:50a Jason
 * added new terrain
 * 
 * 31    1/08/99 2:56p Samir
 * Ripped out OSIRIS1.
 * 
 * 30    11/05/98 7:54p Jason
 * changes for new manage system
 * 
 * 29    10/23/98 1:37p Nate
 * Fixed spelling of sound page.
 * 
 * 28    10/23/98 12:39p Nate
 * Added a few more pages.
 * 
 * 27    10/21/98 8:53a Nate
 * 
 * 26    10/19/98 11:33p Nate
 * Yet more hard coded pages to add
 * 
 * 25    10/19/98 7:21p Nate
 * 
 * 24    10/19/98 7:19p Matt
 * Added system to support different types of damage to the player and
 * have these different types make different sounds.
 * 
 * 23    10/16/98 6:01p Nate
 * added more hard-coded pages
 * 
 * 22    10/16/98 11:35a Jason
 * added some more defaults
 * 
 * 21    10/14/98 1:02p Jason
 * fixed FindSoundName issues
 * 
 * 20    10/14/98 11:32a Jason
 * added gunboy
 * 
 * 19    10/13/98 8:41p Jason
 * fixed cockpit problem
 * 
 * 18    10/13/98 5:25p Nate
 * Added ship flags
 * 
 * 17    10/13/98 4:44p Jason
 * added more primitives
 * 
 * 16    10/13/98 2:32p Chris
 * BuddyBot to GuideBot
 * 
 * 15    10/12/98 11:38p Jeff
 * wrapped all the Object_info[].description whenever freed...trying to
 * find an obscure bug.  Added icon_name to manage page of Generic
 * 
 * 14    10/09/98 10:45p Jeff
 * renamed FlagYellow bug
 * 
 * 13    10/09/98 7:40p Jason
 * fixed gamefile .oofs to read in their textures
 * 
 * 
 * 12    10/09/98 1:41p Jason
 * Hard coded more stuff for demo
 * 
 * 11    10/09/98 10:55a Nate
 * Fixed bug, now correctly displays which pages weren't written out to
 * the table file
 * 
 * 10    10/09/98 3:16a Jason
 * more changes for demo
 * 
 * 9     10/09/98 2:40a Jason
 * fixed table file issues with demo
 * 
 * 8     10/08/98 10:48p Nate
 * 
 * 7     10/08/98 2:26p Jason
 * added table file filter functionality
 * 
 * 6     10/08/98 10:48a Nate
 * Fixed memory leaks by freeing level script when done with loaded level.
 * 
 * 5     10/08/98 10:30a Nate
 * Initial version
 *
 * $NoKeywords: $
 */

#include "stdafx.h"
#include "resource.h"
#include "TableFileFilterMng.h"
#include "manage.h"
#include "mono.h"
#include "LoadLevel.h"
#include "object.h"
#include "objinfo.h"
//@@#include "d3x.h"

#include "texpage.h"
#include "doorpage.h"
#include "soundpage.h"
#include "megapage.h"
#include "shippage.h"
#include "weaponpage.h"
#include "gamefilepage.h"
#include "genericpage.h"
#include "sounds.h"
#include "gametexture.h"
#include "terrain.h"
#include "fireball.h"
#include "mem.h"
#include "polymodel.h"
#include "ddio.h"
#include "DallasUtilities.h"
#include "matcen.h"
#include "game.h"

#define TITLE_NAME	"Table File Filter v1.0"

// Function Prototypes
void DeferMessages(void);
void OpenFilterLogFiles(void);
void WriteToRemainingLog(char *name, int type);
void WriteToRemovedLog(char *name, int type);
void CloseFilterLogFiles(void);

////////////////////////////////////////////////////////////////
// PageDataNode Class Members
////////////////////////////////////////////////////////////////
PageDataNode::PageDataNode()
{
	prev=NULL;
	next=NULL;
	
	name[0]='\0';
	type=PAGETYPE_UNKNOWN;

	flags=0;
	tag_count=0;
}

PageDataNode::~PageDataNode()
{
}

// Performs a comparison based upon the page name
bool PageDataNode::operator < (const PageDataNode &node)
{
	return(stricmp(name,node.name) <0 );
}

// Loads a Page Data Node from a given stream
bool PageDataNode::Load(CFILE *infile)
{
	cf_ReadString(name,PAGENAME_LEN,infile);
	type=(uint8_t)cf_ReadByte(infile);
	flags=cf_ReadInt(infile);
	tag_count=cf_ReadInt(infile);

	return TRUE;
}

// Saves a Page Data Node to a given stream
bool PageDataNode::Save(CFILE *outfile)
{
	cf_WriteString(outfile,name);
	cf_WriteByte(outfile,(int8_t)type);
	cf_WriteInt(outfile,flags);
	cf_WriteInt(outfile,tag_count);

	return TRUE;
}


////////////////////////////////////////////////////////////////
// PageDataList Class Members
////////////////////////////////////////////////////////////////
PageDataList::PageDataList()
{
	int j;

	// Init the list to be empty
	m_head=NULL;
	m_tail=NULL;
	m_size=0;

	// Init the level filename list to be empty
	m_NumLevelFilenames=0;
	for(j=0;j<MAX_LEVEL_FILENAMES;j++) 
		m_LevelFilenames[j][0]='\0';

	// Init the dialog control pointers
	m_Dlg=NULL;
	m_TableParseFile=NULL;
	m_PageDataListCtrl=NULL;
	m_LevelListBox=NULL;
	m_NumPagesText=NULL;
	m_StatusText=NULL;

	m_DataListFilename="";
	m_DataListLoaded=FALSE;
	m_DataListModified=FALSE;
}

PageDataList::~PageDataList()
{
	// Detach the dialog control pointers
	m_Dlg=NULL;
	m_TableParseFile=NULL;
	m_PageDataListCtrl=NULL;
	m_LevelListBox=NULL;
	m_NumPagesText=NULL;
	m_StatusText=NULL;

	ClearList();
}


// Wipes out the data list
void PageDataList::ClearList(void)
{
	PageDataNode *temp, *node;
	int j;

	CWaitCursor wc;

	node=m_head;
	while(node!=NULL) {
		temp=node;
		node=node->next;
		delete temp;
	}
	m_size=0;
	m_head=NULL;
	m_tail=NULL;

	// Init the level filename list to be empty
	m_NumLevelFilenames=0;
	for(j=0;j<MAX_LEVEL_FILENAMES;j++) 
		m_LevelFilenames[j][0]='\0';

	m_DataListFilename="";
	m_DataListLoaded=FALSE;
	m_DataListModified=FALSE;

	// Clear out the dialog data
	if(m_PageDataListCtrl) m_PageDataListCtrl->DeleteAllItems();
	if(m_LevelListBox) m_LevelListBox->ResetContent();
	if(m_NumPagesText) m_NumPagesText->Format("0");
	if(m_StatusText) m_StatusText->Format("Blank Page Data List Created");
	if(m_Dlg) m_Dlg->UpdateData(FALSE);
	DeferMessages();
	wc.Restore();
}


// Adds the node to the list in ascending alphabetical order
// according to the page names of the nodes
bool PageDataList::AddToList(PageDataNode *new_node)
{
	PageDataNode *node;

	CWaitCursor wc;

	if(new_node==NULL) return FALSE;

	// Handle empty list case
	if(m_head==NULL) {
		m_head=new_node;
		m_tail=new_node;
		new_node->next=NULL;
		new_node->prev=NULL;

		m_size++;
		goto UpdateControls;
	}

	// Handle (new node < list head) case
	if((*new_node)<(*m_head)) {
		new_node->next=m_head;
		new_node->prev=NULL;
		m_head->prev=new_node;
		m_head=new_node;

		m_size++;
		goto UpdateControls;
	}

	// Search through list until end of list or until the new node is less
	// than the next node, then add the new node after the current node
	node=m_head;
	while(node->next!=NULL && (*(node->next))<(*new_node) )
		node=node->next;

	new_node->next=node->next;
	new_node->prev=node;

	if(node->next==NULL)
		m_tail=new_node;
	else 
		node->next->prev=new_node;

	node->next=new_node;

	m_size++;

UpdateControls:

	// Add it to the data list control
	AddToListCtrl(0,new_node);
	
	// Update the count
	if(m_NumPagesText) m_NumPagesText->Format("%d",m_size);
	if(m_Dlg) m_Dlg->UpdateData(FALSE);
	DeferMessages();
	wc.Restore();

	m_DataListModified=TRUE;

	return TRUE;
}


// Creates a new node and adds it to the list (if not already in it)
bool PageDataList::AddToList(char *page_name, uint8_t page_type, int page_flags) 
{
	PageDataNode *new_node, *found_node;
	CWaitCursor wc;

	// If the node is already in the list, don't add it!
	found_node=FindNode(page_name,page_type);
	if(found_node!=NULL) {

		// If the new node has been added for a level, tag it once
		if(page_flags & ADDED_FOR_LEVEL_FLAG) {
			found_node->flags |= ADDED_FOR_LEVEL_FLAG;
			found_node->tag_count++;
		}

		// If it's been added directly by user, set that flag
		if(page_flags & ADDED_FOR_USER_FLAG) 
			found_node->flags |= ADDED_FOR_USER_FLAG;

		return TRUE;
	}

	// Allocate memory for the new node
	new_node=new PageDataNode;
	if(new_node==NULL) {
		mprintf(0,"ERROR: Could not allocate new PageDataNode!\n");
		return FALSE;
	}

	// Initialize the data
	strcpy(new_node->name,page_name);
	new_node->type=page_type;
	new_node->flags=page_flags;
	new_node->tag_count=0;

	// If the new node has been added for a level, tag it once
	if(new_node->flags & ADDED_FOR_LEVEL_FLAG) new_node->tag_count++;

	// Add the new node to the list
	AddToList(new_node);

	return TRUE;
}


// Searches for and removes matching node from the list 
bool PageDataList::RemoveFromList(char *page_name, uint8_t page_type) 
{
	int index;
	LVFINDINFO find_info;
	CWaitCursor wc;
	
	if(m_PageDataListCtrl==NULL) return FALSE;

	// Find and remove node from the data list
	PageDataNode *found_node;
	found_node=FindNode(page_name,page_type);
	if(found_node!=NULL) {
		// If node has been "level tagged", decrement it
		if(found_node->tag_count > 0)
			found_node->tag_count--;

		// If node is still tagged by another level, leave it
		if(found_node->tag_count > 0)
			return TRUE;

		// Otherwise, continue and get rid of it
		RemoveFromList(found_node);
	}

	// Find and remove item from the list control
	find_info.flags=LVFI_STRING;
	find_info.psz=page_name;
	index=m_PageDataListCtrl->FindItem(&find_info,-1);
	if(index==-1) return FALSE;
	m_PageDataListCtrl->DeleteItem(index);

	return TRUE;
}


// Removes the given node from the list
bool PageDataList::RemoveFromList(PageDataNode *goner_node)
{
	CWaitCursor wc;

	if(goner_node==NULL) return FALSE;
	
	// Connect previous node to the next node
	if(goner_node->prev != NULL)
		goner_node->prev->next=goner_node->next;

	// Connect the next node to the previous node
	if(goner_node->next != NULL)
		goner_node->next->prev=goner_node->prev;

	// If node to be removed is the head, make the next node the head
	if(goner_node==m_head)
		m_head=goner_node->next;

	// If node to be removed is the tail, make the previous node the tail
	if(goner_node==m_tail)
		m_tail=goner_node->prev;

	delete goner_node;
	m_size--;

	// Update the count
	if(m_NumPagesText) m_NumPagesText->Format("%d",m_size);
	if(m_Dlg) m_Dlg->UpdateData(FALSE);
	DeferMessages();
	wc.Restore();

	m_DataListModified=TRUE;
	
	return TRUE;
}


// Loads a Page Data List from a file
bool PageDataList::LoadList(char *list_filename)
{
	int i, list_size;
	CFILE *infile;
	PageDataNode *new_node;

	CWaitCursor wc;

	// Wipe out the current data list
	ClearList();
	wc.Restore();

	// Try to open the file for loading
	infile=cfopen (list_filename,"rb");
	if (!infile)
	{
		mprintf(0,"Couldn't open the input list file!\n");
		return FALSE;
	}

	// Display "saving" message
	m_StatusText->Format("Loading %s...",list_filename);
	m_Dlg->UpdateData(FALSE);
	DeferMessages();
	wc.Restore();

	// Read in the number of nodes in the list
	list_size=cf_ReadInt(infile);

	// Read in each data node to the list
	for(i=0; i<list_size; i++) {

		// Allocate memory for the new node
		new_node=new PageDataNode;
		if(new_node==NULL) {
			mprintf(0,"ERROR: Could not allocate new PageDataNode!\n");
			return FALSE;
		}

		// Read in the node's data
		new_node->Load(infile);

		// Add the new node to the list
		AddToList(new_node);
		wc.Restore();
	}

	// Read in the number of level filenames
	m_NumLevelFilenames=cf_ReadInt(infile);
	
	// Read in each level filename
	for(i=0; i<m_NumLevelFilenames; i++) {
		cf_ReadString(m_LevelFilenames[i],_MAX_PATH,infile);

		// Add the filename to the list box
		m_LevelListBox->InsertString(-1,m_LevelFilenames[i]);
		DeferMessages();
		wc.Restore();
	}

	// Close the saved file
	cfclose(infile);

	// Save the current filename
	m_DataListFilename=list_filename;
	m_DataListModified=FALSE;

	// Display "saving" message
	m_StatusText->Format("Load Complete");
	m_Dlg->UpdateData(FALSE);
	DeferMessages();
	wc.Restore();

	return TRUE;
}


// Saves a Page Data List to a file
bool PageDataList::SaveList(char *list_filename)
{
	int i;
	CFILE *outfile;
	PageDataNode *node;

	CWaitCursor wc;

	if(list_filename==NULL) {
		if(!FilenameSpecified()) return FALSE;
		list_filename=m_DataListFilename.GetBuffer(0);
	}

	// Try to open the file for saving
	outfile=cfopen (list_filename,"wb");
	if (!outfile)
	{
		mprintf(0,"Couldn't open the output list file!\n");
		return FALSE;
	}

	// Display "saving" message
	m_StatusText->Format("Saving %s...",list_filename);
	m_Dlg->UpdateData(FALSE);
	DeferMessages();
	wc.Restore();

	// Write out the number of nodes in the list
	cf_WriteInt(outfile,m_size);

	// Write out each data node in the list
	node=m_head;
	for(i=0; i<m_size; i++) {
		node->Save(outfile);
		node=node->next;
	}

	// Write out the number of level filenames
	cf_WriteInt(outfile,m_NumLevelFilenames);
	
	// Write out each level filename
	for(i=0; i<m_NumLevelFilenames; i++)
		cf_WriteString(outfile,m_LevelFilenames[i]);

	// Close the saved file
	cfclose(outfile);

	// Save the current filename
	m_DataListFilename=list_filename;
	m_DataListModified=FALSE;

	// Display "saving" message
	m_StatusText->Format("Save Complete");
	m_Dlg->UpdateData(FALSE);
	DeferMessages();
	wc.Restore();

	return TRUE;
}


// Scans the list for a certain page, and returns a pointer to the
// matching data node (or NULL if not found)
PageDataNode *PageDataList::FindNode(char *page_name, uint8_t page_type)
{
	PageDataNode *node, *found_node;

	found_node=NULL;
	node=m_head;
	while(node!=NULL) {
		if(node->type==page_type && stricmp(node->name,page_name)==0) {
			found_node=node;
			break;
		}
		node=node->next;
	}

	return(found_node);
}


// Adds a level filename to the filename list (and to the dialog list box)
bool PageDataList::AddLevelFilename(char *level_filename)
{
	int i, level_found;

	CWaitCursor wc;

	if(level_filename==NULL) return FALSE;

	// Make sure the level hasn't already been added
	level_found=-1;
	for(i=0; i<m_NumLevelFilenames; i++)
		if(stricmp(m_LevelFilenames[i],level_filename)==0) 
			level_found=i;

	if(level_found >= 0) return FALSE;

	// Add the filename to the list
	if(m_NumLevelFilenames >= MAX_LEVEL_FILENAMES) return FALSE;
	strcpy(m_LevelFilenames[m_NumLevelFilenames],level_filename);
	m_NumLevelFilenames++;

	// Add the filename to the list box
	m_LevelListBox->InsertString(-1,level_filename);

	// Add the pages required by this level
	ProcessPagesForLevel(level_filename,ADD_PAGES);
	wc.Restore();

	m_DataListModified=TRUE;

	return TRUE;
}

// Removes the selected level from the filename list (and from the dialog list box)
bool PageDataList::RemoveSelLevelFilename(void)
{
	int i, level_found;
	int index;
	CString level_filename;

	CWaitCursor wc;

	if(m_LevelListBox==NULL) return FALSE;

	index=m_LevelListBox->GetCurSel();
	if(index==LB_ERR) return FALSE;
	m_LevelListBox->GetText(index,level_filename);

	// Find the level position in the filename list
	level_found=-1;
	for(i=0; i<m_NumLevelFilenames; i++)
		if(stricmp(m_LevelFilenames[i],level_filename.GetBuffer(0))==0) 
			level_found=i;

	if(level_found < 0) return FALSE;

	// Remove the filename from the list
	for(i=level_found; i<m_NumLevelFilenames-1; i++)
		strcpy(m_LevelFilenames[i],m_LevelFilenames[i+1]);
	m_NumLevelFilenames--;

	// Remove the filename from the list box
	m_LevelListBox->DeleteString(index);

	// Remove the pages required by this level
	ProcessPagesForLevel(level_filename.GetBuffer(0),REMOVE_PAGES);
	wc.Restore();

	m_DataListModified=TRUE;

	return TRUE;
}

char *gamefile_level_ext[] = {
	".d3l",
	".cpp",
	".dll",
	".msg",
	".omf",
	".brf",
	".str",
	".msn",
	NULL
};

// Processes all pages needed by the given level
bool PageDataList::ProcessPagesForLevel(char *level_filename, int process_type)
{
	CWaitCursor wc;

	if(level_filename==NULL) return FALSE;

	m_StatusText->Format("Loading level - %s...",level_filename);
	m_Dlg->UpdateData(FALSE);
	DeferMessages();
	wc.Restore();

	// Load the given level
	if(!LoadLevel(level_filename)) {
		m_Dlg->MessageBox("Error loading level","Level Load Error");
		return FALSE;
	}

	m_StatusText->Format("Processing level - %s...",level_filename);
	m_NumPagesText->Format("%d",m_size);
	m_Dlg->UpdateData(FALSE);
	DeferMessages();
	wc.Restore();

	// Process level, adding pages that it requires
	DetermineRequiredPages(process_type);

	// Add Level's gamefile pages
	char filename[_MAX_FNAME+1];
	char pagename[_MAX_PATH+1];	

	_splitpath(level_filename,NULL,NULL,filename,NULL);
	for(int j=0;gamefile_level_ext[j]!=NULL;j++) {
		sprintf(pagename,"%s%s",filename,gamefile_level_ext[j]);
		ProcessPage(pagename,PAGETYPE_GAMEFILE,process_type);
	}

	wc.Restore();

	m_StatusText->Format("Processing Complete",level_filename);
	m_Dlg->UpdateData(FALSE);
	DeferMessages();
	wc.Restore();


	return TRUE;
}


// Either adds the given level page to the list, or removes it
bool PageDataList::ProcessPage(char *page_name, uint8_t page_type, int process_type, int flags)
{
	CWaitCursor wc;

	switch(process_type) {
		case ADD_PAGES:
			AddToList(page_name,page_type,ADDED_FOR_LEVEL_FLAG);
			break;
		case REMOVE_PAGES:
			RemoveFromList(page_name,page_type);
			break;
		default:
			break;
	}

	m_NumPagesText->Format("%d",m_size);
	m_Dlg->UpdateData(FALSE);
	DeferMessages();
	wc.Restore();

	return TRUE;
}



// Get extension of a file
bool GetFileExt(char *file, char *ext)
{
	char *temp;
	if(strlen(file)<4) return FALSE;

	temp=(file+strlen(file)-4);
	strcpy(ext,temp);

	if(ext[0]!='.') return FALSE;

	return TRUE;
}

// Determines what pages are required by the currently loaded level,
// and processes each one appropriately

void PageDataList::AddTexturePage (int id,int process_type)
{
	if (id==-1)
		return;
	
	CWaitCursor wc;
	ProcessPage (GameTextures[id].name,PAGETYPE_TEXTURE,process_type);
	wc.Restore();

	if (GameTextures[id].flags & TF_DESTROYABLE && GameTextures[id].destroy_handle!=-1)
	{
		AddTexturePage (GameTextures[id].destroy_handle,process_type);
	}

	if (GameTextures[id].sound!=-1)
		AddSoundPage (GameTextures[id].sound,process_type);
}

void PageDataList::AddSoundPage (int id,int process_type)
{
	if (id==-1)
		return;
	
	CWaitCursor wc;
	ProcessPage (Sounds[id].name,PAGETYPE_SOUND,process_type);
	wc.Restore();
}

void PageDataList::AddDoorPage (int id,int process_type)
{
	CWaitCursor wc;
	//Set sounds
	door *doorpointer=&Doors[id];

	ProcessPage (Doors[id].name,PAGETYPE_DOOR,process_type);
	wc.Restore();

	PageInPolymodel (doorpointer->model_handle);
	poly_model *pm=&Poly_models[doorpointer->model_handle];
	for (int t=0;t<pm->n_textures;t++)
		AddTexturePage (pm->textures[t],process_type);
	

	if (doorpointer->open_sound!=-1 && doorpointer->open_sound!=SOUND_NONE_INDEX)
		AddSoundPage (doorpointer->open_sound,process_type);
	if (doorpointer->close_sound!=-1 && doorpointer->close_sound!=SOUND_NONE_INDEX)
		AddSoundPage (doorpointer->close_sound,process_type);
}

void PageDataList::AddWeaponPage (int id,int process_type)
{
	CWaitCursor wc;
	weapon *weaponpointer=&Weapons[id];
	
	if (id==-1)
		return;

	int i;
	ProcessPage (Weapons[id].name,PAGETYPE_WEAPON,process_type);
	wc.Restore();

	if (!(weaponpointer->flags & (WF_IMAGE_BITMAP|WF_IMAGE_VCLIP)))
	{
		PageInPolymodel (weaponpointer->fire_image_handle);
		poly_model *pm=&Poly_models[weaponpointer->fire_image_handle];
		for (int t=0;t<pm->n_textures;t++)
			AddTexturePage (pm->textures[t],process_type);
	}
	
	// Load the various textures associated with this weapon
	if (weaponpointer->explode_image_handle!=-1)
	{
		AddTexturePage (weaponpointer->explode_image_handle,process_type);
	}

	if (weaponpointer->particle_handle!=-1)
	{
		AddTexturePage (weaponpointer->particle_handle,process_type);
	}

	if (weaponpointer->smoke_handle!=-1)
	{
		AddTexturePage (weaponpointer->smoke_handle,process_type);
	}

	if (weaponpointer->scorch_handle!=-1)
	{
		AddTexturePage (weaponpointer->scorch_handle,process_type);
	}

	if (weaponpointer->icon_handle!=-1)
	{
		AddTexturePage (weaponpointer->icon_handle,process_type);
	}

	// Try to load spawn weapons
	if (weaponpointer->spawn_handle!=-1 && weaponpointer->spawn_count>0 && weaponpointer->spawn_handle!=id)
	{
		AddWeaponPage (weaponpointer->spawn_handle,process_type);
	}
	
	if (weaponpointer->alternate_spawn_handle!=-1 && weaponpointer->spawn_count>0 && weaponpointer->alternate_spawn_handle!=id)
	{
		AddWeaponPage (weaponpointer->alternate_spawn_handle,process_type);
	}

	if (weaponpointer->robot_spawn_handle!=-1)
	{
		AddGenericPage (weaponpointer->robot_spawn_handle,process_type);
	}

	// Try and load the various sounds
	for (i=0;i<MAX_WEAPON_SOUNDS;i++)
	{
		if (weaponpointer->sounds[i]!=SOUND_NONE_INDEX)
		{
			AddSoundPage (weaponpointer->sounds[i],process_type);
		}
	}
}

void PageDataList::AddShipPage (int id,int process_type)
{
	int i,t;
	CWaitCursor wc;

	ProcessPage (Ships[id].name,PAGETYPE_SHIP,process_type);
	wc.Restore();

	ship *shippointer=&Ships[id];

	// Page in all textures for this object

	PageInPolymodel (shippointer->model_handle);
	poly_model *pm=&Poly_models[shippointer->model_handle];

	for (t=0;t<pm->n_textures;t++)
		AddTexturePage (pm->textures[t],process_type);

	if (shippointer->med_render_handle!=-1)
	{
		PageInPolymodel (shippointer->med_render_handle);
		pm=&Poly_models[shippointer->med_render_handle];
		for (t=0;t<pm->n_textures;t++)
			AddTexturePage (pm->textures[t],process_type);
	}

	if (shippointer->lo_render_handle!=-1)
	{
		PageInPolymodel (shippointer->lo_render_handle);
		pm=&Poly_models[shippointer->lo_render_handle];
		for (t=0;t<pm->n_textures;t++)
			AddTexturePage (pm->textures[t],process_type);
	}

	if (shippointer->dying_model_handle!=-1)
	{
		PageInPolymodel (shippointer->dying_model_handle);
		pm=&Poly_models[shippointer->dying_model_handle];
		for (t=0;t<pm->n_textures;t++)
			AddTexturePage (pm->textures[t],process_type);
	}

	// Try and load the various weapons
	int j;
	for(i = 0; i < MAX_PLAYER_WEAPONS; i++)
	{
		for(j = 0; j < MAX_WB_GUNPOINTS; j++)
		{
			if (shippointer->static_wb[i].gp_weapon_index[j] != LASER_INDEX)
			{	
				AddWeaponPage (shippointer->static_wb[i].gp_weapon_index[j],process_type);

			}
		}
	}

	// Try and load the various weapons
	for(i = 0; i < MAX_PLAYER_WEAPONS; i++)
	{
		for(j = 0; j < MAX_WB_FIRING_MASKS; j++)
		{
			if (shippointer->static_wb[i].fm_fire_sound_index[j] != SOUND_NONE_INDEX)
				AddSoundPage (shippointer->static_wb[i].fm_fire_sound_index[j],process_type);
		}
	}

	for(i = 0; i < MAX_PLAYER_WEAPONS; i++)
	{
		if (shippointer->firing_sound[i]!=-1)
			AddSoundPage (shippointer->firing_sound[i],process_type);
		
		if(shippointer->firing_release_sound[i] != -1)
			AddSoundPage (shippointer->firing_release_sound[i],process_type);
		
		if (shippointer->spew_powerup[i]!=-1)
			AddGenericPage (shippointer->spew_powerup[i],process_type);
		
	}
}

void PageDataList::AddGenericPage (int id,int process_type)
{
	int i,t;
	CWaitCursor wc;

	if (id==-1)
		return;

	ProcessPage (Object_info[id].name,PAGETYPE_GENERIC,process_type);
	wc.Restore();

	object_info *objinfopointer=&Object_info[id];

	// Page in all textures for this object

	PageInPolymodel (objinfopointer->render_handle);
	poly_model *pm=&Poly_models[objinfopointer->render_handle];

	for (t=0;t<pm->n_textures;t++)
		AddTexturePage (pm->textures[t],process_type);

	if (objinfopointer->med_render_handle!=-1)
	{
		PageInPolymodel (objinfopointer->med_render_handle);
		pm=&Poly_models[objinfopointer->med_render_handle];
		for (t=0;t<pm->n_textures;t++)
			AddTexturePage (pm->textures[t],process_type);
	}

	if (objinfopointer->lo_render_handle!=-1)
	{
		PageInPolymodel (objinfopointer->lo_render_handle);
		pm=&Poly_models[objinfopointer->lo_render_handle];
		for (t=0;t<pm->n_textures;t++)
			AddTexturePage (pm->textures[t],process_type);
	}

	// Process all sounds for this object
	for (i=0;i<MAX_OBJ_SOUNDS;i++)
	{
		if (objinfopointer->sounds[i]!=SOUND_NONE_INDEX)
		{
			AddSoundPage (objinfopointer->sounds[i],process_type);
		}
	}

	for (i=0;i<MAX_AI_SOUNDS;i++)
	{
		if (objinfopointer->ai_info->sound[i]!=SOUND_NONE_INDEX)
		{
			AddSoundPage (objinfopointer->ai_info->sound[i],process_type);
		}
	}

	// Try and load the various wb sounds
	int j;
	for(i = 0; i < MAX_WBS_PER_OBJ; i++)
	{
		for(j = 0; j < MAX_WB_FIRING_MASKS; j++)
		{
			if(objinfopointer->static_wb[i].fm_fire_sound_index[j]!=SOUND_NONE_INDEX)
			{
				AddSoundPage (objinfopointer->static_wb[i].fm_fire_sound_index[j],process_type);
			}
		}
	}

	// Try and load the various wb sounds
	for(i = 0; i < NUM_MOVEMENT_CLASSES; i++)
	{
		for(j = 0; j < NUM_ANIMS_PER_CLASS; j++)
		{
			if(objinfopointer->anim[i].elem[j].anim_sound_index!=SOUND_NONE_INDEX)
			{
				AddSoundPage (objinfopointer->anim[i].elem[j].anim_sound_index,process_type);
			}
		}
	}

	// Load the spew types
	for(i=0;i<MAX_DSPEW_TYPES;i++)
	{
		if (objinfopointer->dspew_number[i]>0 && objinfopointer->dspew[i]!=0 && objinfopointer->dspew[i]!=id)
		{
			AddGenericPage (objinfopointer->dspew[i],process_type);
		
		}
	}

	// Try and load the various weapons

	// Automatically include laser
	AddWeaponPage (LASER_INDEX,process_type);

	for(i = 0; i < MAX_WBS_PER_OBJ; i++)
	{
		for(j = 0; j < MAX_WB_GUNPOINTS; j++)
		{
			if (objinfopointer->static_wb[i].gp_weapon_index[j]!=LASER_INDEX)
			{
				AddWeaponPage (objinfopointer->static_wb[i].gp_weapon_index[j],process_type);
			}
		}
	}
}

extern const char *Static_sound_names[];
bool PageDataList::DetermineRequiredPages(int process_type)
{
	CWaitCursor wc;
	CString filename;
	int i;

	////////////////////////////////////////////////////////////
	// Note to Jason:
	//
	// For every page needed by the level call these functions:
	//		ProcessPage(page_name,page_type,process_type);
	//		wc.Restore();
	////////////////////////////////////////////////////////////

	// Processes all the pages in the TableParse output file
	m_TableParseFile->GetWindowText(filename);
	ProcessPagesInTextFile(filename.GetBuffer(0),process_type);

/* SHOULDN'T NEED THE FOLLOWING ANYMORE

	// Add all the other files
	char ext[12];
	for(i=0;i<MAX_GAMEFILES;i++) 
	{
		if(Gamefiles[i].used != 0)
		{
			GetFileExt(Gamefiles[i].name,ext);
			
			if (stricmp(ext,".d3l")) 
			{
				AddToList(Gamefiles[i].name,PAGETYPE_GAMEFILE,process_type);
			}

			if (!stricmp(ext,".oof")) 
			{
				int pid=LoadPolyModel (Gamefiles[i].name,0);
				if (pid!=-1)
				{
					PageInPolymodel (pid);
					poly_model *pm=&Poly_models[pid];
					for (int t=0;t<pm->n_textures;t++)
						AddTexturePage (pm->textures[t],process_type);
				}
				else
				{
					mprintf(0,"Couldn't find polymodel %s!\n",Gamefiles[i].name);
					Int3();
				}
			}
		}
	}

	// Get misc pages

	AddSoundPage (FindSoundName ("AmbSwitch31"),process_type);
	AddSoundPage (FindSoundName ("AmbLavaLoop1"),process_type);

	AddTexturePage (FindTextureName("Ready1"),process_type);
	AddTexturePage (FindTextureName("Online"),process_type);


	AddTexturePage (FindTextureName("hilite0"),process_type);
	AddTexturePage (FindTextureName("hilite1"),process_type);
	AddTexturePage (FindTextureName("hilite2"),process_type);
	AddTexturePage (FindTextureName("hilite3"),process_type);
	AddTexturePage (FindTextureName("hilite4"),process_type);
	AddTexturePage (FindTextureName("hilite5"),process_type);

	AddTexturePage (FindTextureName("smhilite01"),process_type);
	AddTexturePage (FindTextureName("smhilite11"),process_type);
	AddTexturePage (FindTextureName("smhilite21"),process_type);
	AddTexturePage (FindTextureName("smhilite31"),process_type);
	AddTexturePage (FindTextureName("smhilite41"),process_type);
	AddTexturePage (FindTextureName("smhilite51"),process_type);
	AddTexturePage (FindTextureName("smhilite61"),process_type);
	AddTexturePage (FindTextureName("smhilite71"),process_type);

	AddWeaponPage (FindWeaponName ("GunBoy"),process_type);
	AddWeaponPage (FindWeaponName ("FragBarrel"),process_type);
	AddWeaponPage (FindWeaponName ("NapalmBarrel"),process_type);
	AddWeaponPage (FindWeaponName ("TubbsHitBlast"),process_type);
	AddWeaponPage (FindWeaponName ("ChaffSpark"),process_type);
	AddWeaponPage (FindWeaponName ("NapalmBlob"),process_type);
	
	AddGenericPage (FindObjectIDName ("ShipRedFlag"),process_type);
	AddGenericPage (FindObjectIDName ("ShipBlueFlag"),process_type);
	AddGenericPage (FindObjectIDName ("ShipYellowFlag"),process_type);
	AddGenericPage (FindObjectIDName ("ShipGreenFlag"),process_type);
	AddGenericPage (FindObjectIDName ("GuideBot"),process_type);
	AddGenericPage (FindObjectIDName ("SmallDebris"),process_type);
	AddGenericPage (FindObjectIDName ("BigDebris"),process_type);
	AddGenericPage (FindObjectIDName ("VirusHousing"),process_type);
	AddGenericPage (FindObjectIDName ("Afterburner"),process_type);
	AddGenericPage (FindObjectIDName ("attached light bulb"),process_type);
	AddGenericPage (FindObjectIDName ("GuideBot"),process_type);
	AddGenericPage (FindObjectIDName ("ChaffChunk"),process_type);
	AddGenericPage (FindObjectIDName ("Jugghead"),process_type);
	AddGenericPage (FindObjectIDName ("Jugg main turret"),process_type);
	AddGenericPage (FindObjectIDName ("Juggbelly"),process_type);
	AddGenericPage (FindObjectIDName ("Jugg flame turret"),process_type);
	AddGenericPage (FindObjectIDName ("Trackerhatch"),process_type);
	AddGenericPage (FindObjectIDName ("Trackerturret"),process_type);
	AddGenericPage (FindObjectIDName ("Pesttail"),process_type);
	AddGenericPage (FindObjectIDName ("Aliencuplinkhousing"),process_type);

	AddTexturePage (FindTextureName("LightFlareStar"),process_type);
	AddTexturePage (FindTextureName("TelcomPowerBar"),process_type);
	AddTexturePage (FindTextureName("Matcen Lightning"),process_type);
	AddTexturePage (FindTextureName("WhiteGlowingBall"),process_type);
	AddTexturePage (FindTextureName("SkyDome10"),process_type);
	AddTexturePage (FindTextureName("SkyBand10"),process_type);
	AddTexturePage (FindTextureName("Multicolor"),process_type);

	AddSoundPage (FindSoundName ("Energy Converter"),process_type);
	AddSoundPage (FindSoundName ("CockpitSequenceTest1"),process_type);
	AddSoundPage (FindSoundName ("Player hit by weapon"),process_type);
	AddSoundPage (FindSoundName ("Metallic Door Hit"),process_type);
	AddSoundPage (FindSoundName ("Wall Fade ???"),process_type);
	AddSoundPage (FindSoundName ("RainDrop"),process_type);
	AddSoundPage (FindSoundName ("Lightning"),process_type);
	AddSoundPage (FindSoundName ("AmbSwitch31"),process_type);
	AddSoundPage (FindSoundName ("AmbLavaLoop1"),process_type);
	AddSoundPage (FindSoundName ("Invulnerability on"),process_type);
	AddSoundPage (FindSoundName ("Invulnerability off"),process_type);
	AddSoundPage (FindSoundName ("Cloak on"),process_type);
	AddSoundPage (FindSoundName ("Cloak off"),process_type);
	AddSoundPage (FindSoundName ("GBotConcern1"),process_type);
	AddSoundPage (FindSoundName ("Drop bomb"),process_type);
	AddSoundPage (FindSoundName ("RbtStingeattack1"),process_type);

	AddSoundPage (FindSoundName ("AmbDroneV"),process_type);
	AddSoundPage (FindSoundName ("AmbDroneR"),process_type);
	AddSoundPage (FindSoundName ("AmbDroneO"),process_type);
	AddSoundPage (FindSoundName ("AmbDroneN"),process_type);
	AddSoundPage (FindSoundName ("AmbDroneI"),process_type);
	AddSoundPage (FindSoundName ("AmbDroneK"),process_type);
	AddSoundPage (FindSoundName ("AmbDroneM"),process_type);
	AddSoundPage (FindSoundName ("AmbDroneX"),process_type);
	AddSoundPage (FindSoundName ("AmbDroneQ"),process_type);
	AddSoundPage (FindSoundName ("AmbDroneL"),process_type);
	AddSoundPage (FindSoundName ("AmbDroneF"),process_type);
	AddSoundPage (FindSoundName ("AmbDroneS"),process_type);
	AddSoundPage (FindSoundName ("AmbDroneD"),process_type);
	AddSoundPage (FindSoundName ("AmbDroneU"),process_type);
	AddSoundPage (FindSoundName ("AmbDroneH"),process_type);
	AddSoundPage (FindSoundName ("rainhigh1"),process_type);
	AddSoundPage (FindSoundName ("rainlow1"),process_type);
	AddSoundPage (FindSoundName ("AmbWindGust"),process_type);
	AddSoundPage (FindSoundName ("AmbVirusSwitch"),process_type);
	AddSoundPage (FindSoundName ("VoxCompAutoSecurity.osf"),process_type);
	AddSoundPage (FindSoundName ("CTFHatTrick"),process_type);

	AddSoundPage (FindSoundName ("CTFPickupFlag1"),process_type);
	AddSoundPage (FindSoundName ("CTFScore1"),process_type);
	AddSoundPage (FindSoundName ("CTFLostFlag1"),process_type);
	AddSoundPage (FindSoundName ("CTFReturnedFlag1"),process_type);
	AddSoundPage (FindSoundName ("Cheater!"),process_type);
	AddSoundPage (FindSoundName ("Hostage pickup"),process_type);
	
	AddGenericPage (FindObjectIDName ("Converter"),process_type);
	AddGenericPage (FindObjectIDName ("Shield"),process_type);
	AddGenericPage (FindObjectIDName ("Energy"),process_type);

	AddGenericPage (FindObjectIDName ("Afterburner"),process_type);
	AddGenericPage (FindObjectIDName ("Attached light bulb"),process_type);
	AddGenericPage (FindObjectIDName ("ChaffChunk"),process_type);
	AddGenericPage (FindObjectIDName ("TrackerHatch"),process_type);
	AddGenericPage (FindObjectIDName ("TrackerTurret"),process_type);
	AddGenericPage (FindObjectIDName ("PestTail"),process_type);
	AddGenericPage (FindObjectIDName ("Virushousing"),process_type);
	AddGenericPage (FindObjectIDName ("Viruscartridge"),process_type);

	AddWeaponPage (FindWeaponName ("TubbsHitBlast"),process_type);

	AddShipPage (FindShipName ("Pyro-SE"),process_type);

	AddGenericPage (FindObjectIDName ("FlagRed"),process_type);
	AddGenericPage (FindObjectIDName ("FlagBlue"),process_type);
	AddGenericPage (FindObjectIDName ("FlagGreen"),process_type);
	AddGenericPage (FindObjectIDName ("FlagYellow"),process_type);

	AddTexturePage (FindTextureName("RedBase"),process_type);
	AddTexturePage (FindTextureName("BlueBase"),process_type);
	AddTexturePage (FindTextureName("YellowBase"),process_type);
	AddTexturePage (FindTextureName("GreenBase"),process_type);

	AddTexturePage (FindTextureName("Multicolor"),process_type);
*/
	
	// Add in matcen stuff
	for (i=0;i<Num_matcens;i++)
	{
		int t;
		AddTexturePage (Matcen[i]->GetCreationTexture(),process_type);
		for (t=0;t<MAX_MATCEN_SOUNDS;t++)
			AddSoundPage (Matcen[i]->GetSound(t),process_type);

		for (t=0;t<Matcen[i]->GetNumProdTypes();t++)
		{
			int index;
			Matcen[i]->GetProdInfo (t,&index,NULL,NULL,NULL);
			if (index!=MATCEN_ERROR)
				AddGenericPage (index,process_type);
		}
	}

	for (i=0;i<NUM_TERRAIN_SOUND_BANDS;i++)
	{
		if (Terrain_sound_bands[i].sound_index!=-1)
			AddSoundPage (Terrain_sound_bands[i].sound_index,process_type);
	}


	
	// Get static fireballs
	for (i=0;i<NUM_FIREBALLS;i++)
	{
		char name[PAGENAME_LEN];
		strcpy (name,Fireballs[i].name);

		name[strlen(name)-4]=0;
		int id=FindTextureName (name);
		if (id!=-1)
			AddTexturePage (id,process_type);
		else
			AddToList(Fireballs[i].name,PAGETYPE_GAMEFILE,process_type);
	}

	// Get static sounds
	for (i=0;i<NUM_STATIC_SOUNDS;i++)
	{
		int sid=FindSoundName(Static_sound_names[i]);

		if (sid!=-1)
			AddSoundPage (sid,process_type);
	}

	// First get textures	
	for (i=0;i<=Highest_room_index;i++)
	{
		if (!Rooms[i].used)
			continue;

		room *rp=&Rooms[i];
		for (int t=0;t<rp->num_faces;t++)
		{
			AddTexturePage (rp->faces[t].tmap,process_type);
		}
	}

	// Touch all terrain textures
	for (i=0;i<TERRAIN_TEX_WIDTH*TERRAIN_TEX_DEPTH;i++)
	{
		AddTexturePage (Terrain_tex_seg[i].tex_index,process_type);
	}
	
	AddTexturePage (Terrain_sky.dome_texture,process_type);

	for (i=0;i<Terrain_sky.num_satellites;i++)
	{	
		AddTexturePage (Terrain_sky.satellite_texture[i],process_type);
	}

	// Touch all objects
	for (i=0;i<=Highest_object_index;i++)
	{
		object *obj=&Objects[i];

		if (obj->type==OBJ_NONE)
		{
			continue;
		}
		if (obj->type==OBJ_POWERUP || obj->type==OBJ_ROBOT || obj->type==OBJ_CLUTTER || obj->type==OBJ_BUILDING)
		{
			AddGenericPage (obj->id,process_type);
			continue;
		}

		if (obj->type==OBJ_DOOR)
		{
			AddDoorPage (obj->id,process_type);
			continue;
		}

		if (obj->control_type == CT_SOUNDSOURCE) {
			ASSERT(obj->type == OBJ_SOUNDSOURCE);
			if (obj->ctype.soundsource_info.sound_index != -1)
				AddSoundPage (obj->ctype.soundsource_info.sound_index,process_type);
		}
	}

	return TRUE;
}

// Creates the new table file by filtering out any pages that
// are not in the current page data list
bool PageDataList::CreateNewTableFile(char *new_table_filename, char *src_table_filename)
{
	CFILE *infile,*outfile;
	uint8_t pagetype,replaced=0;
	int done=0,len;
	int pages_written;
	PageDataNode *node;
	CString summary_msg;
	bool unwritten_page_found;
	char ext[12];

	mngs_texture_page texpage;
	//@@mngs_power_page powpage;
	mngs_door_page	doorpage;
	//@@mngs_robot_page	robotpage;
	mngs_generic_page	genericpage;
	mngs_gamefile_page	gamefilepage;
	mngs_sound_page	soundpage;
	mngs_ship_page	shippage;
	mngs_weapon_page weaponpage;
	mngs_megacell_page megacellpage;

	CWaitCursor wc;

	if(new_table_filename==NULL || src_table_filename==NULL) return FALSE;

	if(m_StatusText) m_StatusText->Format("Creating new table file - %s...",new_table_filename);
	if(m_Dlg) m_Dlg->UpdateData(FALSE);
	DeferMessages();
	wc.Restore();

	// First make sure we can open the table file and the temp table file
	infile=cfopen (src_table_filename,"rb");
	if (!infile)
	{
		mprintf(0,"Couldn't open input table file!\n");
		return FALSE;
	}

	outfile=cfopen (new_table_filename,"wb");
	if (!outfile)
	{
		mprintf(0,"Couldn't open output table file!\n");
		cfclose (infile);
		return FALSE;
	}

	// Mark all pages in data list as not having been written out
	for(node=m_head;node!=NULL;node=node->next)
		node->flags &= ~PAGE_WRITTEN_FLAG;

	OpenFilterLogFiles();

	// Read in every page from the source table file, but only write
	// out a page to the new table file if the same page name (and type)
	// are found in the current page data list
	pages_written=0;
	while (!done)
	{
		if (cfeof (infile))
		{
			done=1;
			continue;
		}
		pagetype=cf_ReadByte (infile);
		len=cf_ReadInt (infile);

		switch (pagetype)
		{
			case PAGETYPE_TEXTURE:
				// Read it in, write it out.
				mng_ReadNewTexturePage (infile,&texpage);
				if((node=FindNode(texpage.tex_struct.name,pagetype))!=NULL) {
					WriteToRemainingLog(texpage.tex_struct.name,pagetype);
					mng_WriteNewTexturePage (outfile,&texpage);
					node->flags |= PAGE_WRITTEN_FLAG;
					pages_written++;
				} else {
					WriteToRemovedLog(texpage.tex_struct.name,pagetype);
				}
				break;
			case PAGETYPE_POWERUP:
				// Read it in, write it out.
				//@@mng_ReadPowerPage (infile,&powpage);		
				//@@mng_WritePowerPage (outfile,&powpage);
				if(m_Dlg) m_Dlg->MessageBox("A Powerup page was encountered in the source table file","Powerup Page Encountered"); 
				break;
			case PAGETYPE_DOOR:
				// Read it in, write it out.
				mng_ReadNewDoorPage (infile,&doorpage);		
				if((node=FindNode(doorpage.door_struct.name,pagetype))!=NULL) {
					WriteToRemainingLog(doorpage.door_struct.name,pagetype);
					mng_WriteNewDoorPage (outfile,&doorpage);
					node->flags |= PAGE_WRITTEN_FLAG;
					pages_written++;
				} else {
					WriteToRemovedLog(doorpage.door_struct.name,pagetype);
				}
				break;
			case PAGETYPE_ROBOT:
				// Read it in, write it out.
				//@@mng_ReadRobotPage (infile,&robotpage);		
				//@@mng_WriteRobotPage (outfile,&robotpage);
				if(m_Dlg) m_Dlg->MessageBox("A Robot page was encountered in the source table file","Robot Page Encountered"); 
				break;
			case PAGETYPE_GENERIC:
				// Read it in, write it out.
				mng_ReadNewGenericPage (infile,&genericpage);		
				if((node=FindNode(genericpage.objinfo_struct.name,pagetype))!=NULL) {	
					WriteToRemainingLog(genericpage.objinfo_struct.name,pagetype);
					mng_WriteNewGenericPage (outfile,&genericpage);
					node->flags |= PAGE_WRITTEN_FLAG;
					pages_written++;
				} else {
					WriteToRemovedLog(genericpage.objinfo_struct.name,pagetype);
				}
				if (genericpage.objinfo_struct.description!=NULL)
				{
					mem_free (genericpage.objinfo_struct.description);
					genericpage.objinfo_struct.description = NULL;
				}
				break;
			case PAGETYPE_GAMEFILE:
				// Read it in, write it out.
				mng_ReadNewGamefilePage (infile,&gamefilepage);
				node=NULL;
				strcpy(ext,"");
				if((node=FindNode(gamefilepage.gamefile_struct.name,pagetype))!=NULL || ALWAYS_WRITE_GAMEFILE_PAGES) {
					//if(GetFileExt(gamefilepage.gamefile_struct.name,ext) && stricmp(ext,".d3l")!=0) {
						WriteToRemainingLog(gamefilepage.gamefile_struct.name,pagetype);
						mng_WriteNewGamefilePage (outfile,&gamefilepage);
						if(node) node->flags |= PAGE_WRITTEN_FLAG;
						pages_written++;
					//} else {
					//	WriteToRemovedLog(gamefilepage.gamefile_struct.name,pagetype);
					//}
				} else {
					WriteToRemovedLog(gamefilepage.gamefile_struct.name,pagetype);
				}
				break;
			case PAGETYPE_SOUND:
				// Read it in, write it out.
				mng_ReadNewSoundPage (infile,&soundpage);		
				if((node=FindNode(soundpage.sound_struct.name,pagetype))!=NULL) {
					WriteToRemainingLog(soundpage.sound_struct.name,pagetype);
					mng_WriteNewSoundPage (outfile,&soundpage);
					node->flags |= PAGE_WRITTEN_FLAG;
					pages_written++;
				} else {
					WriteToRemovedLog(soundpage.sound_struct.name,pagetype);
				}
				break;
			case PAGETYPE_MEGACELL:
				// Read it in, write it out.
				mng_ReadNewMegacellPage (infile,&megacellpage);		
				if((node=FindNode(megacellpage.megacell_struct.name,pagetype))!=NULL) {
					WriteToRemainingLog(megacellpage.megacell_struct.name,pagetype);
					mng_WriteNewMegacellPage (outfile,&megacellpage);
					node->flags |= PAGE_WRITTEN_FLAG;
					pages_written++;
				} else {
					WriteToRemovedLog(megacellpage.megacell_struct.name,pagetype);
				}
				break;
			case PAGETYPE_SHIP:
				// Read it in, write it out.
				mng_ReadNewShipPage (infile,&shippage);		
				if((node=FindNode(shippage.ship_struct.name,pagetype))!=NULL) {
					WriteToRemainingLog(shippage.ship_struct.name,pagetype);
					mng_WriteNewShipPage (outfile,&shippage);
					node->flags |= PAGE_WRITTEN_FLAG;
					pages_written++;
				} else {
					WriteToRemovedLog(shippage.ship_struct.name,pagetype);
				}
				break;
			case PAGETYPE_WEAPON:
				// Read it in, write it out.
				mng_ReadNewWeaponPage (infile,&weaponpage);		
				if((node=FindNode(weaponpage.weapon_struct.name,pagetype))!=NULL) {
					WriteToRemainingLog(weaponpage.weapon_struct.name,pagetype);
					mng_WriteNewWeaponPage (outfile,&weaponpage);
					node->flags |= PAGE_WRITTEN_FLAG;
					pages_written++;
				} else {
					WriteToRemovedLog(weaponpage.weapon_struct.name,pagetype);
				}
				break;
			case PAGETYPE_UNKNOWN:
				WriteToRemainingLog("Unknown",pagetype);
				mng_WriteUnknownPage (outfile);
				break;
			default:
				Int3();	// unrecognized pagetype
				break;
		}
	}

	CloseFilterLogFiles();

	cfclose (infile);
	cfclose (outfile);

	if(m_StatusText) m_StatusText->Format("New Table File Created (%d pages)",pages_written);
	if(m_Dlg) m_Dlg->UpdateData(FALSE);
	DeferMessages();

	// Create message summary (if some pages weren't written)
	unwritten_page_found=FALSE;
	summary_msg="The following pages were not written out (they were not found in the source table file):\n\n";
	for(node=m_head;node!=NULL;node=node->next) {
		if(node->flags & PAGE_WRITTEN_FLAG) continue;

		// This node was not written out!
		unwritten_page_found=TRUE;
		CString temp_str;
		temp_str.Format("%s, ",node->name);
		summary_msg+=temp_str;
	}

	// Display message summary (if some pages weren't written)
	if(unwritten_page_found)
		if(m_Dlg) m_Dlg->MessageBox(summary_msg,"Not All Pages Were Written");

	// Mark all pages in data list as not having been written out
	for(node=m_head;node!=NULL;node=node->next)
		node->flags &= ~PAGE_WRITTEN_FLAG;

	return TRUE;  // successful!		
}


// Inserts data from the given node into the dialog list control
bool PageDataList::AddToListCtrl(int index,PageDataNode *node)
{
	int pos;
	CString type_text;

	CWaitCursor wc;

	// Make sure this is worth doing
	if(node==NULL) return FALSE;
	if(m_PageDataListCtrl==NULL) return FALSE;

	// Add the data to the list control
	if((pos=m_PageDataListCtrl->InsertItem(0,node->name))==-1) return FALSE;

	switch(node->type) {
		case PAGETYPE_UNKNOWN:
			type_text="Unknown";
			break;
		case PAGETYPE_TEXTURE:
			type_text="Texture";
			break;
		case PAGETYPE_WEAPON:
			type_text="Weapon";
			break;
		case PAGETYPE_ROBOT:
			type_text="Robot";
			break;
		case PAGETYPE_POWERUP:
			type_text="Powerup";
			break;
		case PAGETYPE_DOOR:
			type_text="Door";
			break;
		case PAGETYPE_SHIP:
			type_text="Ship";
			break;
		case PAGETYPE_SOUND:
			type_text="Sound";
			break;
		case PAGETYPE_MEGACELL:
			type_text="Megacell";
			break;
		case PAGETYPE_GAMEFILE:
			type_text="Gamefile";
			break;
		case PAGETYPE_GENERIC:
			type_text="Generic";
			break;
	}

	m_PageDataListCtrl->SetItem(pos,PAGE_TYPE_COLUMN,LVIF_TEXT,type_text.GetBuffer(0),0,0,0,0);

	return TRUE;
}


// Removes all selected items from the list control
bool PageDataList::RemoveSelFromListCtrl(void)
{
	int j, num_items;
	LV_ITEM item_info;

	char name[PAGENAME_LEN+1];
	char type[_MAX_PATH+1];

	CWaitCursor wc;

	if(m_PageDataListCtrl==NULL) return FALSE;

	// How many items in the control list?
	num_items=m_PageDataListCtrl->GetItemCount();
	
	j=0;
	while(j<num_items) {

		// Grab the info for item #j
		item_info.iItem=j;
		item_info.iSubItem=0;
		item_info.mask= LVIF_TEXT | LVIF_STATE;
		item_info.stateMask=LVIS_SELECTED;
		item_info.pszText=name;
		item_info.cchTextMax=PSFILENAME_LEN+1;
		m_PageDataListCtrl->GetItem(&item_info);

		// Is item at index j selected?		
		if(item_info.state & LVIS_SELECTED) {	// yes, delete it
			
			// Get Directory name for item #j
			item_info.iSubItem=PAGE_TYPE_COLUMN;
			item_info.pszText=type;
			item_info.cchTextMax=_MAX_PATH;
			m_PageDataListCtrl->GetItem(&item_info);

			int page_type;
			if(stricmp(type,"Unknown")==0) page_type=PAGETYPE_UNKNOWN;
			else if(stricmp(type,"Texture")==0) page_type=PAGETYPE_TEXTURE;
			else if(stricmp(type,"Weapon")==0) page_type=PAGETYPE_WEAPON;
			else if(stricmp(type,"Robot")==0) page_type=PAGETYPE_ROBOT;
			else if(stricmp(type,"Powerup")==0) page_type=PAGETYPE_POWERUP;
			else if(stricmp(type,"Door")==0) page_type=PAGETYPE_DOOR;
			else if(stricmp(type,"Ship")==0) page_type=PAGETYPE_SHIP;
			else if(stricmp(type,"Sound")==0) page_type=PAGETYPE_SOUND;
			else if(stricmp(type,"Megacell")==0) page_type=PAGETYPE_MEGACELL;
			else if(stricmp(type,"Gamefile")==0) page_type=PAGETYPE_GAMEFILE;
			else page_type=PAGETYPE_GENERIC;

			// Search for and remove corresponding entry from Document Library
			PageDataNode *found_node;
			found_node=FindNode(name,page_type);
			if(found_node!=NULL) {
				RemoveFromList(found_node);
				wc.Restore();
			}
			else {
				if(m_Dlg) m_Dlg->MessageBox("The list control does not match the data list.","List Mismatch Error!");
				wc.Restore();
			}

			// Delete entry/item from control list
			m_PageDataListCtrl->DeleteItem(j);
			num_items=m_PageDataListCtrl->GetItemCount();

		}
		else
			j++;
	}

	return TRUE;
}

// Displays the modified confirmation prompt if appropriate
int PageDataList::ModifiedPrompt(void)
{
	if(m_Dlg==NULL) return(IDNO);

	// If current file has been modified, check with user first...
	if(!m_DataListModified) return(IDYES);
	if(m_Dlg->MessageBox("Changes to data list will be lost. Proceed?",
		          "Page Data List Has Been Modified",
				  MB_YESNO)==IDYES) return(IDYES);
	return(IDNO);
}


// Sets the title bar string
void PageDataList::SetTitleString(void)
{
	if(m_Dlg==NULL) return;

	char title[1024];
	char loaded_file[1024];

	if(!m_DataListFilename.IsEmpty()) 
		sprintf(loaded_file," - [%s%s]", m_DataListFilename.GetBuffer(0),
			(m_DataListModified) ? "*":"");
	else
		sprintf(loaded_file,"");

	sprintf(title,"%s%s", TITLE_NAME, loaded_file); 
	m_Dlg->SetWindowText(title);
}


// Adds a page specified by the "Add Page" button
bool PageDataList::AddPageFromUser(char *page_name, uint8_t page_type)
{
	int id;
	switch(page_type) {
		case PAGETYPE_GENERIC:
			id=FindObjectIDName (page_name);
			if (id==-1)
				return FALSE;
			AddGenericPage (id,ADD_PAGES);
			break;

		case PAGETYPE_TEXTURE:
			id=FindTextureName (page_name);
			if (id==-1)
				return FALSE;
			AddTexturePage (id,ADD_PAGES);
			break;

		case PAGETYPE_SOUND:
			id=FindSoundName (page_name);
			if (id==-1)
				return FALSE;
			AddSoundPage (id,ADD_PAGES);
			break;

		case PAGETYPE_DOOR:
			id=FindDoorName (page_name);
			if (id==-1)
				return FALSE;
			AddDoorPage (id,ADD_PAGES);
			break;

		case PAGETYPE_SHIP:
			id=FindShipName (page_name);
			if (id==-1)
				return FALSE;
			AddShipPage (id,ADD_PAGES);
			break;

		case PAGETYPE_WEAPON:
			id=FindWeaponName (page_name);
			if (id==-1)
				return FALSE;
			AddWeaponPage (id,ADD_PAGES);
			break;

		default:
			return FALSE;
	}

	return TRUE;
}


// Returns the pagetype for a given string (PAGETYPE_UNKNOWN if not valid)
uint8_t PageDataList::GetPageTypeFromString(char *text)
{
	int type;

	if(text==NULL) return(PAGETYPE_UNKNOWN);

	type=PAGETYPE_UNKNOWN;
	if(strcmp(text,"PAGETYPE_SOUND")==0)
		type=PAGETYPE_SOUND;
	else if(strcmp(text,"PAGETYPE_GENERIC")==0)
		type=PAGETYPE_GENERIC;
	else if(strcmp(text,"PAGETYPE_WEAPON")==0)
		type=PAGETYPE_WEAPON;
	else if(strcmp(text,"PAGETYPE_GAMEFILE")==0)
		type=PAGETYPE_GAMEFILE;
	else if(strcmp(text,"PAGETYPE_TEXTURE")==0)
		type=PAGETYPE_TEXTURE;
	else if(strcmp(text,"PAGETYPE_DOOR")==0)
		type=PAGETYPE_DOOR;
	else if(strcmp(text,"PAGETYPE_SHIP")==0)
		type=PAGETYPE_SHIP;

	return(type);
}


// Processes a page read in from text file
bool PageDataList::ProcessPageFromFile(char *page_name, uint8_t page_type, int process_type)
{
	char ext[12];
	int id;
	switch(page_type) {
		case PAGETYPE_GENERIC:
			id=FindObjectIDName (page_name);
			if (id==-1)
				return FALSE;
			AddGenericPage (id,process_type);
			break;

		case PAGETYPE_TEXTURE:
			id=FindTextureName (page_name);
			if (id==-1)
				return FALSE;
			AddTexturePage (id,process_type);
			break;

		case PAGETYPE_SOUND:
			id=FindSoundName (page_name);
			if (id==-1)
				return FALSE;
			AddSoundPage (id,process_type);
			break;

		case PAGETYPE_DOOR:
			id=FindDoorName (page_name);
			if (id==-1)
				return FALSE;
			AddDoorPage (id,process_type);
			break;

		case PAGETYPE_SHIP:
			id=FindShipName (page_name);
			if (id==-1)
				return FALSE;
			AddShipPage (id,process_type);
			break;

		case PAGETYPE_WEAPON:
			id=FindWeaponName (page_name);
			if (id==-1)
				return FALSE;
			AddWeaponPage (id,process_type);
			break;

		case PAGETYPE_GAMEFILE:
			GetFileExt(page_name,ext);
			
			//if (stricmp(ext,".d3l")) 
			//{
				ProcessPage(page_name,PAGETYPE_GAMEFILE,process_type);
			//}

			if (!stricmp(ext,".oof")) 
			{
				int pid=LoadPolyModel (page_name,0);
				if (pid!=-1)
				{
					PageInPolymodel (pid);
					poly_model *pm=&Poly_models[pid];
					for (int t=0;t<pm->n_textures;t++)
						AddTexturePage (pm->textures[t],process_type);
				}
				else
				{
					mprintf(0,"Couldn't find polymodel %s!\n",page_name);
					Int3();
				}
			}
			break;

		default:
			return FALSE;
	}

	return TRUE;
}


// Scans one of Jeff's TableParse output files and processes
// all of the pages contained within
bool PageDataList::ProcessPagesInTextFile(char *filename, int process_type)
{
	CFILE *infile;
	char linebuf[2048];
	char *line;
	int linenum;

	char *pagename;
	uint8_t pagetype;
	char *pagetype_text;

	CString pages_not_added;
	int num_pages_not_added;

	// Try to open the file for loading
	infile=cfopen (filename,"rt");
	if (!infile)
	{
		CString msg;
		msg.Format("Unable to open \"%s\"!",filename);
		m_Dlg->MessageBox(msg,"TableParse Output File Error!",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;
	}

	linenum=0;
	pages_not_added="The following pages were not processed successfully:\n\n";
	num_pages_not_added=0;

	// Read in and parse each line of the file
	while (!cfeof(infile)) {

		// Clear the buffer
		strcpy(linebuf,"");

		// Read in a line from the file
		cf_ReadString(linebuf, sizeof(linebuf), infile);
		linenum++;

		// Remove whitespace padding at start and end of line
		RemoveTrailingWhitespace(linebuf);
		line=SkipInitialWhitespace(linebuf);

		// If the line is empty, grab a new one
		if(strlen(line)==0) continue;

		// Grab the page type
		line=strtok(line," \t\n");
		if(line==NULL) continue;
		pagetype=GetPageTypeFromString(line);
		if(pagetype==PAGETYPE_UNKNOWN) {
			CString msg;
			msg.Format("Found invalid page type: %s, in %s, line %d",line,filename,linenum);
			m_Dlg->MessageBox(msg,"Invalid Page Type",MB_OK|MB_ICONEXCLAMATION);
			continue;
		}
		pagetype_text=line;

		// Grab the page name
		pagename=strtok(NULL,"");
		if(pagename==NULL || strlen(pagename)==0) {
			CString msg;
			msg.Format("No page name found in %s, line %d",filename,linenum);
			m_Dlg->MessageBox(msg,"Invalid Page Name",MB_OK|MB_ICONEXCLAMATION);
			continue;
		}

		// remove quotes
		if(pagename[0]=='"') {
			char *end_of_name;

			pagename++;
			end_of_name=strchr(pagename,'"');
			if(end_of_name!=NULL)
				(*end_of_name)='\0';
		}

		// Make sure we still have a name
		if(strlen(pagename)==0) {
			CString msg;
			msg.Format("No page name found in %s, line %d",filename,linenum);
			m_Dlg->MessageBox(msg,"Invalid Page Name",MB_OK|MB_ICONEXCLAMATION);
			continue;
		}

		// Try and add the page
		if(!ProcessPageFromFile(pagename,pagetype,process_type)) {
			CString msg;
			msg.Format("%s(%s), ",pagename,pagetype_text);
			if(num_pages_not_added < 30)
				pages_not_added+=msg;
			else if(num_pages_not_added == 30)
				pages_not_added+="and even more...";
			num_pages_not_added++;
		}
	}

	cfclose(infile);
/*
	if(num_pages_not_added>0) {
		m_Dlg->MessageBox(pages_not_added,"Not All Pages From File Were Processed",MB_OK|MB_ICONEXCLAMATION);
	}
*/
	return TRUE;
}


// Adds all appropriate gamefiles to the list
bool PageDataList::AddAllGamefilePages(void)
{
	int i;
	char ext[12];

	for(i=0;i<MAX_GAMEFILES;i++) {
		if(	Gamefiles[i].used != 0 && 
			GetFileExt(Gamefiles[i].name,ext) && 
			stricmp(ext,".d3l")!=0) {
			AddToList(Gamefiles[i].name,PAGETYPE_GAMEFILE,ADDED_FOR_LEVEL_FLAG);
		}
	}
	return TRUE;
}


// Processes any waiting messages
#define MAX_MESSAGES 10	// max messages to process during a message deferral
void DeferMessages(void) 
{
	MSG msg;
	for (	int MsgCount = MAX_MESSAGES;
			MsgCount && PeekMessage( &msg, NULL, 0, 0, PM_REMOVE );
			MsgCount--) {
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}
}

// Log file handles
CFILE *remaining_log_file=NULL;
CFILE *removed_log_file=NULL;

// Opens the Filter Log Files
void OpenFilterLogFiles(void)
{
	char fullpath[_MAX_PATH+1];

	ddio_MakePath(fullpath,LocalD3Dir,"Remaining Pages.txt",NULL);
	remaining_log_file=cfopen (fullpath,"wt");
	if (remaining_log_file==NULL)
	{
		mprintf(0,"Couldn't open \"Remaining Pages\" log file!\n");
		return;
	}

	ddio_MakePath(fullpath,LocalD3Dir,"Removed Pages.txt",NULL);
	removed_log_file=cfopen (fullpath,"wt");
	if (removed_log_file==NULL)
	{
		mprintf(0,"Couldn't open \"Removed Pages\" log file!\n");
		return;
	}
}

// Returns the pagetype string for a given type
void GetPageTypeString(int type, CString &type_text)
{
	switch(type) {
		case PAGETYPE_TEXTURE:
			type_text="Texture ";
			break;
		case PAGETYPE_WEAPON:
			type_text="Weapon  ";
			break;
		case PAGETYPE_ROBOT:
			type_text="Robot   ";
			break;
		case PAGETYPE_POWERUP:
			type_text="Powerup ";
			break;
		case PAGETYPE_DOOR:
			type_text="Door    ";
			break;
		case PAGETYPE_SHIP:
			type_text="Ship    ";
			break;
		case PAGETYPE_SOUND:
			type_text="Sound   ";
			break;
		case PAGETYPE_MEGACELL:
			type_text="Megacell";
			break;
		case PAGETYPE_GAMEFILE:
			type_text="Gamefile";
			break;
		case PAGETYPE_GENERIC:
			type_text="Generic ";
			break;
		default:
			type_text="Unknown ";
			break;
	}
}

// Writes a page to the "remaining pages" log file
void WriteToRemainingLog(char *name, int type)
{
	if(remaining_log_file!=NULL) {
		CString line, type_string;
		GetPageTypeString(type,type_string);
		line.Format("%s\t%s",type_string,name);
		cf_WriteString(remaining_log_file,line.GetBuffer(0));
	}
}

// Writes a page to the "removed pages" log file
void WriteToRemovedLog(char *name, int type)
{
	if(removed_log_file!=NULL) {
		CString line, type_string;
		GetPageTypeString(type,type_string);
		line.Format("%s\t%s",type_string,name);
		cf_WriteString(removed_log_file,line.GetBuffer(0));
	}
}

// Closes the Filter Log Files
void CloseFilterLogFiles(void)
{
	if(remaining_log_file!=NULL) cfclose(remaining_log_file);
	if(removed_log_file!=NULL) cfclose(removed_log_file);
}
