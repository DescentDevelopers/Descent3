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

 * $Logfile: /DescentIII/Main/hogedit/HogEditDoc.cpp $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:57:56 $
 * $Author: kevinb $
 *
 * 
 *
 * $Log: not supported by cvs2svn $
 * 
 * 10    5/05/99 12:53p Nate
 * Added support for multiple file extraction
 * 
 * 9     10/30/98 11:15a Nate
 * Added support for modification of hog files.
 * 
 * 8     10/09/98 4:50p Nate
 * 
 * 7     9/17/98 4:29p Nate
 * Added Import Directory option.
 * 
 * 6     8/14/98 4:38p Nate
 * Fixed a few minor bugs and added better error reporting
 * 
 * 5     8/14/98 1:01p Nate
 * Added better error reporting for the HogEditor
 * 
 * 4     7/22/98 2:38p Nate
 * Added Modified File prompt when exiting
 * 
 * 3     7/20/98 3:35p Nate
 * Added more Editing functionality
 * 
 * 2     7/15/98 12:31p Nate
 * Initial version
 *
 * $NoKeywords: $
 */

// HogEditDoc.cpp : implementation of the CHogEditDoc class
//

#include "stdafx.h"
#include <afxtempl.h>
#include <sys/stat.h>
#include "HogEdit.h"

#include "hogfile.h"
#include "HogEditDoc.h"
#include <io.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


bool DocModified;			// is document modified.


/////////////////////////////////////////////////////////////////////////////
// CHogEditDoc

IMPLEMENT_DYNCREATE(CHogEditDoc, CDocument)

BEGIN_MESSAGE_MAP(CHogEditDoc, CDocument)
	//{{AFX_MSG_MAP(CHogEditDoc)
	ON_UPDATE_COMMAND_UI(ID_ACTION_INSERT, OnUpdateActionInsert)
	ON_UPDATE_COMMAND_UI(ID_ACTION_CREATE, OnUpdateActionCreate)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSave)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_AS, OnUpdateFileSaveAs)
	ON_UPDATE_COMMAND_UI(ID_ACTION_UPDATE, OnUpdateActionUpdate)
	ON_UPDATE_COMMAND_UI(ID_ACTION_IMPORT, OnUpdateActionImport)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHogEditDoc construction/destruction

CHogEditDoc::CHogEditDoc()
{
	// TODO: add one-time construction code here

}

CHogEditDoc::~CHogEditDoc()
{
}

BOOL CHogEditDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)
	NewDocument();

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CHogEditDoc serialization

void CHogEditDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CHogEditDoc diagnostics

#ifdef _DEBUG
void CHogEditDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CHogEditDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CHogEditDoc commands


//////////////////////////////////////////////////////////////////////////////
//	These functions perform most of the meaty tasks of this interface.


//	creates a new (and empty) hog library.
void CHogEditDoc::NewDocument()
{
	// Initialize the hog library structure
	Library.filename[0]='\0';
	Library.flags=0;
	Library.num_entries=0;

	//	clear out all lists
	Library.filelist.RemoveAll();	

	//	mark as not modified.
	DocModified = FALSE;
	m_StaticHog = FALSE;
	m_DocumentName = "Untitled.rib";
	m_NoNameChosen = TRUE;
}


//	loads a RIB file or HOG file.
int CHogEditDoc::LoadDocument(CString& name)
{
	char *ascii_name;
	char ext[_MAX_EXT];
	int res;

	// Get ascii string from name
	ascii_name=name.GetBuffer(0);

	//	extract file extension from name
	_splitpath(ascii_name, NULL, NULL, NULL, ext);

	//	load either .rib or .hog file
        if (stricmp(ext, ".rib") == 0) {
		res=LoadRib(ascii_name);
		if(res) m_StaticHog = false;
	}
	else {
		res=LoadHog(ascii_name);
		if(res) m_StaticHog = true;
	}

	//	Set the new document name
	if(res) m_DocumentName = name;

	return(res);
}

//	saves a RIB file
int CHogEditDoc::SaveDocument(CString& name)
{
	char *ascii_name;
	char ext[_MAX_EXT];
	int res;

	ascii_name=name.GetBuffer(0);

	//	name dialog
	_splitpath(ascii_name, NULL, NULL, NULL, ext);

	//	load either .rib or .hog file
        if (stricmp(ext, ".rib") == 0) {
		res=SaveRib(ascii_name);
		if(res) m_StaticHog = false;
	}
	else {
		res=SaveHog(ascii_name);
		if(res) m_StaticHog = true;
	}
	return(res);
}

//	loads a RIB file
int CHogEditDoc::LoadRib(const char *pathname)
{
	hog_library_entry entry;
	FILE *rib_fp;
	char tag[10];
	int j;

	// open the file
	rib_fp = fopen( pathname, "rb" );
	if ( rib_fp == NULL )		{
		OutputDebugString("Could not open file.\n");
		return false;
	}

	// write the tag
	if (!fread(tag, strlen(RIB_TAG_STR), 1, rib_fp ))	{
		fclose(rib_fp);
		return false;
	}

	// Make tag string null terminated
	tag[strlen(RIB_TAG_STR)]='\0';

	// Check if string has a valid tag
	if(strcmp(tag,RIB_TAG_STR)!=0) {
		OutputDebugString("File is not a RIB file!\n");
		fclose(rib_fp);
		return false;
	}

	// Read in the hog filename
	if(!fread(Library.filename,sizeof(char),_MAX_PATH,rib_fp)) {
		fclose(rib_fp);
		return false;
	}

	// Read in the hog file flags
	if(!fread(&Library.flags,sizeof(Library.flags),1,rib_fp)) {
		fclose(rib_fp);
		return false;
	}

	// Read the number of file entries
	if (!fread(&Library.num_entries, sizeof(Library.num_entries), 1, rib_fp ))	{
		fclose(rib_fp);
		return false;
	}

	// read the hogfile entries 
	for(j=0;j<Library.num_entries;j++) {
		if(!ReadHogLibEntry(rib_fp,&entry)) {
			fclose(rib_fp);
			return false;
		}
		Library.filelist.AddTail(entry);
	}

	fclose(rib_fp);

	char str[256];
	sprintf(str,"Read %d entries from %s.\n",Library.num_entries,pathname);
	OutputDebugString(str);
	
	return true;
}


//	loads a HOG file.
int CHogEditDoc::LoadHog(const char *pathname)
{
	int i;
	int table_pos;
	FILE *hog_fp;
	tHogHeader header;
	tHogFileEntry file_entry;
	hog_library_entry lib_entry; 
	char tag[10];
	int offset;

	// open the file
	hog_fp = fopen( pathname, "rb" );
	if ( hog_fp == NULL ) {
		return FALSE;
	}

	// read the tag
	if (!fread(tag, strlen(HOG_TAG_STR), 1, hog_fp ))	{
		fclose(hog_fp);
		return FALSE;
	}

	// Make tag string null terminated
	tag[strlen(HOG_TAG_STR)]='\0';

	// Check if string has a valid tag
	if(strcmp(tag,HOG_TAG_STR)!=0) {
		OutputDebugString("File is not a HOG file!\n");
		fclose(hog_fp);
		return FALSE;
	}

	// Get number of entries
	if (!fread(&header.nfiles,sizeof(header.nfiles),1,hog_fp))	{
		fclose(hog_fp);
		return FALSE;
	}
	Library.num_entries=header.nfiles;

	// Get the file data offset
	if (!fread(&header.file_data_offset,sizeof(header.file_data_offset),1,hog_fp))	{
		fclose(hog_fp);
		return FALSE;
	}
	offset=header.file_data_offset;

	// Calculate file position of index table
	table_pos = strlen(HOG_TAG_STR) + HOG_HDR_SIZE;

	// Scan ahead to start of entry table
	fseek(hog_fp,table_pos,SEEK_SET);

	// Read in the entries and store them in the Library list
	for (i = 0; i < Library.num_entries; i++)
	{
		if (!ReadHogEntry(hog_fp, &file_entry)) {
			fclose(hog_fp);
			return FALSE;
		}
		strcpy(lib_entry.path, "N/A");
		strcpy(lib_entry.name, file_entry.name);
		lib_entry.length=file_entry.len;
		lib_entry.timestamp=file_entry.timestamp;
		lib_entry.flags=file_entry.flags;
		lib_entry.offset=offset;

		offset+=lib_entry.length; // get offset for next file

		Library.filelist.AddTail(lib_entry);
	}

	//	cleanup
	fclose(hog_fp);	

	char str[256];
	sprintf(str,"Read %d entries from %s.\n",Library.num_entries,pathname);
	OutputDebugString(str);

	return TRUE;
}

//	saves a HOG file.
int CHogEditDoc::SaveHog(const char *pathname)
{
	return FALSE;
}

// saves a RIB file
int CHogEditDoc::SaveRib(const char *pathname)
{
	FILE *rib_fp;
	POSITION pos;
	hog_library_entry entry;


	// create new file
	rib_fp = fopen( pathname, "wb" );
	if ( rib_fp == NULL ) {
		OutputDebugString("Could not open file.\n");
		return FALSE;
	}

	// write the tag
	if (!fwrite(RIB_TAG_STR, strlen(RIB_TAG_STR), 1, rib_fp ))	{
		OutputDebugString("Could not write header tag.\n");
		fclose(rib_fp);
		return FALSE;
	}

	// write out the hog filename
	if(!fwrite(Library.filename,sizeof(char),_MAX_PATH,rib_fp)) {
		fclose(rib_fp);
		return false;
	}

	// write out the hog file flags
	if(!fwrite(&Library.flags,sizeof(Library.flags),1,rib_fp)) {
		fclose(rib_fp);
		return false;
	}

	// write out the number of entries
	Library.num_entries=Library.filelist.GetCount();
	if(!fwrite(&Library.num_entries,sizeof(Library.num_entries),1,rib_fp)) {
		fclose(rib_fp);
		return false;
	}

	// write out the hogfile entries 
	pos = Library.filelist.GetHeadPosition();
	while (pos)
	{
		entry = Library.filelist.GetNext(pos);
		if(!WriteHogLibEntry(rib_fp,&entry)) {
			fclose(rib_fp);
			return false;
		}
	}

	fclose(rib_fp);

	char str[256];
	sprintf(str,"Writing %d entries to %s...\n",Library.num_entries,pathname);
	OutputDebugString(str);
	
	return true;
}

// Fills given entry structure with given file's data, and adds entry to Library list
int CHogEditDoc::AddFile(const char *pathname, hog_library_entry *entry)
{	
	struct _stat filestat;
	char temp_filename[_MAX_PATH];
	char test_filename[_MAX_PATH];
	char filename[PSFILENAME_LEN+1];
	char ext[_MAX_EXT];
	unsigned length;
  int32_t timestamp;
	POSITION pos;
	char path[_MAX_PATH];
	char drive[_MAX_PATH];
	char newpath[_MAX_PATH];
	hog_library_entry temp_entry;

	// Get file info
	if (_stat(pathname, &filestat) != 0) {
		return ADDFILE_STAT_ERROR;
	}

	// Get just the filename
	_splitpath(pathname, NULL, NULL, temp_filename, ext);


	// Check if filename is too big
	sprintf(test_filename, "%s%s", temp_filename, ext);
	if( strlen(test_filename) > PSFILENAME_LEN )
		return ADDFILE_LONG_FNAME_ERROR;

	sprintf(filename, "%s%s", temp_filename, ext);

	length=filestat.st_size;
	timestamp=filestat.st_mtime;

	_splitpath(pathname, drive, path, NULL, NULL);
	sprintf(newpath, "%s%s", drive, path);
	strcpy(path, filename);

	// See if this entry is already in the library
	pos = Library.filelist.GetHeadPosition();
	while (pos)
	{
		temp_entry = Library.filelist.GetNext(pos);
                if (!stricmp(temp_entry.name, filename) /*&& !stricmp(temp_entry.path, newpath)*/)
			return ADDFILE_DUP_FILE_ERROR;
	}

	//	this is a new entry, so set it up and add it to the list!
	strcpy(entry->path, newpath); 
	strcpy(entry->name, filename);
	entry->length = length;
	entry->timestamp = timestamp;
	entry->offset = -1;  // Entry was not read in from a hogfile

	Library.filelist.AddTail(*entry);
	DocModified=true;

	return ADDFILE_OK;
}


// Checks to see if file has been modified, and updates entry accordingly
int CHogEditDoc::UpdatedFileCheck(hog_library_entry *entry)
{
	struct _stat filestat;
	char full_name[_MAX_PATH];

	sprintf(full_name,"%s%s",entry->path,entry->name);

	// Get file info
	if (_stat(full_name, &filestat) != 0) 
		return FILE_IS_GONE;

	// has file changed?
	if(entry->timestamp == filestat.st_mtime)
		return FILE_IS_SAME;

	// update entries
	entry->length=filestat.st_size;
	entry->timestamp=filestat.st_mtime;

	return FILE_HAS_CHANGED;
}


// Loads a Hog library entry structure
bool CHogEditDoc::ReadHogLibEntry(FILE *fp, hog_library_entry *entry)
{
	int res=0;
	res = fread(entry->path, sizeof(char), _MAX_PATH, fp);
	res = fread(entry->name, sizeof(char), PSFILENAME_LEN+1, fp);
	res = fread(&entry->flags, sizeof(entry->flags), 1, fp);
	res = fread(&entry->length, sizeof(entry->length), 1, fp);
	res = fread(&entry->timestamp, sizeof(entry->timestamp), 1, fp);

	entry->offset = -1;  // Entry was not read in from a hogfile
	
	if (res) 
		return TRUE;
	else
		return FALSE;
}


// Saves a Hog library entry structure
bool CHogEditDoc::WriteHogLibEntry(FILE *fp, hog_library_entry *entry)
{
	int res=0;
	res = fwrite(entry->path, sizeof(char), _MAX_PATH, fp);
	res = fwrite(entry->name, sizeof(char), PSFILENAME_LEN+1, fp);
	res = fwrite(&entry->flags, sizeof(entry->flags), 1, fp);
	res = fwrite(&entry->length, sizeof(entry->length), 1, fp);
	res = fwrite(&entry->timestamp, sizeof(entry->timestamp), 1, fp);
	
	if (res) 
		return TRUE;
	else
		return FALSE;
}

// Allocates and fills the hog library filenames list
bool CHogEditDoc::CreateFilenameList(char ***filenames)
{
	char full_name[_MAX_PATH];
	POSITION pos;
	hog_library_entry temp_entry;
	int j;

	// Get the number of filenames
	Library.num_entries=Library.filelist.GetCount();

	// Allocate the list of filename pointers
	*filenames=new (char (*[Library.num_entries]));
	if(*filenames==NULL) return FALSE;

	// Allocate and store each filename in the Library
	j=0;
	pos=Library.filelist.GetHeadPosition();
	while (pos!=NULL)
	{
		temp_entry = Library.filelist.GetNext(pos);
		
		// Only include the path if it's added from a rib file
		if(temp_entry.offset==-1)
			sprintf(full_name,"%s%s",temp_entry.path,temp_entry.name);
		else
			sprintf(full_name,"%s",temp_entry.name);

		(*filenames)[j]=new char[strlen(full_name)+1];
		if((*filenames)[j]==NULL) {OutputDebugString("Outta memory!\n");return FALSE;}

		strcpy((*filenames)[j],full_name);

		/*
		sprintf(full_name,"%d - %s\n",j,(*filenames)[j]);
		OutputDebugString(full_name);
		*/
		j++;
	}
	return TRUE;
}

// Comparison function for qsort
int compare( const void *arg1, const void *arg2 )
{	
	char filename1[_MAX_PATH];
	char filename2[_MAX_PATH];
	char ext1[_MAX_EXT];
	char ext2[_MAX_EXT];

	// Isolate just the filenames (w/extensions) for the compare
	_splitpath(* (char **)arg1, NULL, NULL, filename1, ext1);
	_splitpath(* (char **)arg2, NULL, NULL, filename2, ext2);
	strcat(filename1,ext1);
	strcat(filename2,ext2);

	// Do a case-insensitive, asending order comparison
	return _stricmp(filename1, filename2);
}

// Quicksort the list of filenames
void CHogEditDoc::SortFilenameList(char **filenames)
{
	int count;

	count=Library.num_entries;
	qsort((void *)filenames,(size_t)count,sizeof(char *),compare);
}

// Deallocates the hog library filenames list (Library.num_entries must be valid!)
bool CHogEditDoc::DeleteFilenameList(char **filenames)
{
	int j;

	// Delete each filename
	for(j=0;j<Library.num_entries;j++)
		delete[] filenames[j];

	// Delete the list of pointers
	delete[] filenames;

	return TRUE;
}

// Return estimate (in bytes) of the final Hog File Size
uint32_t CHogEditDoc::CalcHogFileSize(void)
{
	uint32_t total;
	POSITION pos;
	hog_library_entry temp_entry;

	total=0;

	// Add header and file table sizes
	total += strlen(HOG_TAG_STR);
	total += HOG_HDR_SIZE;
	total +=(sizeof(tHogFileEntry) * Library.filelist.GetCount());

	// Add file lengths
	pos=Library.filelist.GetHeadPosition();
	while (pos!=NULL)
	{
		temp_entry = Library.filelist.GetNext(pos);
		total += temp_entry.length;
	}

	return(total);
}

// Extracts a file from the current hog file
bool CHogEditDoc::ExtractFile(int file_pos, unsigned file_len, char *out_fname)
{
	FILE *hog_fp, *out_fp;

	// If current file isn't a hog file, get outta here
	if(!m_StaticHog) return FALSE;

	// Open input (hog) file
	hog_fp = fopen( m_DocumentName, "rb" );
	if ( hog_fp == NULL ) {
		return FALSE;
	}

	// Open output file
	out_fp = fopen( out_fname, "wb" );
	if ( out_fp == NULL ) {
		fclose(hog_fp);
		return FALSE;
	}

	// Seek to correct place in hog file
	if(fseek(hog_fp,file_pos,SEEK_SET)!=0) {
		fclose(hog_fp);
		fclose(out_fp);
		return FALSE;
	}

	// Copy out the file
	if(!FileCopy(out_fp,hog_fp,file_len)) {
		fclose(hog_fp);
		fclose(out_fp);
		return FALSE;
	}
	
	// Close em up
	fclose(out_fp);
	fclose(hog_fp);

	return(TRUE);
}

// Opens and returns a pointer to the file (matching the given entry name)
// within the current hog file, returns NULL if file is not in the hog
FILE *CHogEditDoc::FindFileInHog(char *hog_fname,tHogFileEntry *table_entry)
{
	FILE *hog_fp;
	POSITION pos;
	hog_library_entry entry;
	bool found_in_hog;

	// If current file isn't a hog, get outta here
	if(!m_StaticHog) return(NULL);

	// Search the current entry list for the given filename
	pos = Library.filelist.GetHeadPosition();
	found_in_hog=FALSE;
	while (pos!=NULL)
	{
		entry = Library.filelist.GetNext(pos);
                if (!stricmp(entry.name, table_entry->name)) {
			found_in_hog=TRUE;
			break;
		}
	}

	// If the filename wasn't in the list, get outta here
	if(!found_in_hog) return(NULL);

	// If the found file is actually an "external" file, get outta here
	if(entry.offset == -1) return(NULL);

	// fill in the entry data for this file
	table_entry->flags = entry.flags;
	table_entry->len = entry.length;
	table_entry->timestamp = entry.timestamp;

	// open the source hog file
	hog_fp = fopen( hog_fname, "rb" );
	if ( hog_fp == NULL ) {
		return(NULL);
	}

	// Seek to correct place in hog file
	if(fseek(hog_fp,entry.offset,SEEK_SET)!=0) {
		fclose(hog_fp);
		return(NULL);
	}

	// Return pointer to file's position within the hog
	return(hog_fp);
}


// This is a function to specifically handle the creation of a new hog
// file from the currently loaded (and presumedly altered) hog file.
// This one also takes a pointer to a function that will perform
// progress updates (for the user)
#define TEMP_HOG_FILENAME "TempHogFile.thf"
int CHogEditDoc::CreateNewHogFromCurrentHog(char *src_hog_fname, char *target_hog_fname, 
											int nfiles, const char **filenames, 
											void(* UpdateFunction)(char *))
{
	unsigned i;
	int table_pos;
	FILE *hog_fp;
	tHogHeader header;
	tHogFileEntry *table;
	char ext[_MAX_EXT];
	char dest_hog_fname[_MAX_PATH+1];

	// If the source and target hog filenames are the same,
	// then make the destination a temporary file
	if(!stricmp(src_hog_fname,target_hog_fname))
		strcpy(dest_hog_fname,TEMP_HOG_FILENAME);
	else
		strcpy(dest_hog_fname,target_hog_fname);

	hogerr_filename[0]='\0';

	//	allocate file table
	if (nfiles <= 0) 
		return HOGMAKER_ERROR;

	table = new tHogFileEntry[nfiles];
	if (!table) 
		return HOGMAKER_MEMORY;

	//	create new file
	hog_fp = fopen( dest_hog_fname, "wb" );
	if ( hog_fp == NULL )		{
		delete[] table;
		strcpy(hogerr_filename,dest_hog_fname);
		return HOGMAKER_OPENOUTFILE;
	}

	//write the tag
	if (!fwrite(HOG_TAG_STR, strlen(HOG_TAG_STR), 1, hog_fp ))	{
		delete[] table;
		fclose(hog_fp);
		strcpy(hogerr_filename,dest_hog_fname);
		return HOGMAKER_OUTFILE;
	}

	//write number of files
	uint8_t filler = 0xff;
	header.nfiles = (unsigned)nfiles;
	header.file_data_offset = strlen(HOG_TAG_STR) + HOG_HDR_SIZE + (sizeof(tHogFileEntry) * header.nfiles);

	if (!fwrite(&header.nfiles,sizeof(header.nfiles),1,hog_fp))	{
		delete[] table;
		fclose(hog_fp);
		strcpy(hogerr_filename,dest_hog_fname);
		return HOGMAKER_OUTFILE;
	}
	if (!fwrite(&header.file_data_offset,sizeof(header.file_data_offset),1,hog_fp))	{
		delete[] table;
		fclose(hog_fp);
		strcpy(hogerr_filename,dest_hog_fname);
		return HOGMAKER_OUTFILE;
	}

	// write out filler
	for(i=0; i < HOG_HDR_SIZE-sizeof(tHogHeader); i++)
		if (!fwrite(&filler,sizeof(uint8_t),1,hog_fp))	{
			delete[] table;
			fclose(hog_fp);
			strcpy(hogerr_filename,dest_hog_fname);
			return HOGMAKER_OUTFILE;
		}

	// save file position of index table and write out dummy table
	table_pos = strlen(HOG_TAG_STR) + HOG_HDR_SIZE;

	memset(&table[0], 0, sizeof(table[0]));

	for (i = 0; i < header.nfiles; i++)
	{
		if (!WRITE_FILE_ENTRY(hog_fp, &table[0])) {
			delete[] table;
			fclose(hog_fp);
			strcpy(hogerr_filename,dest_hog_fname);
			return HOGMAKER_OUTFILE;
		}
	}

	// write files (& build index)
	for (i=0;i<header.nfiles;i++) 
	{
		FILE *ifp;
		struct _stat mystat;

		// Get JUST the name of the file (with extension)
		_splitpath(filenames[i],NULL,NULL,table[i].name,ext);
		strcat(table[i].name,ext);

		// See if the file is in the hog, and, if it is, get the file position
		// it starts at
		ifp=FindFileInHog(src_hog_fname,&table[i]);

		// If file isn't in the hog, then process it as an "external" file
		if(ifp == NULL) {	
			ifp = fopen(filenames[i],"rb");
			if ( ifp == NULL )	{
				delete[] table;
				fclose(hog_fp);
				strcpy(hogerr_filename,filenames[i]);
				return HOGMAKER_INFILE;
			}

			_fstat(fileno(ifp), &mystat);

			table[i].flags = 0;
			table[i].len = _filelength(_fileno(ifp));
			table[i].timestamp = mystat.st_mtime;
		}

		if (!FileCopy(hog_fp,ifp,table[i].len)) {
			delete[] table;
			fclose(hog_fp);
			fclose(ifp);
			strcpy(hogerr_filename,filenames[i]);
			return HOGMAKER_COPY;
		}

		fclose(ifp);

		// Setup the update message and send it
		char msg[256];
		int ipct = int(100.0*(double(i)/double(header.nfiles)));
		sprintf(msg,"Creating Hog File... (%d%% done)",ipct);
		UpdateFunction(msg);
	}

	// now write the real index
	fseek(hog_fp,table_pos,SEEK_SET);

	for (i = 0; i < header.nfiles; i++)
	{
		if (!WRITE_FILE_ENTRY(hog_fp, &table[i])) {
			delete[] table;
			fclose(hog_fp);
			strcpy(hogerr_filename,dest_hog_fname);
			return HOGMAKER_OUTFILE;
		}
	}

	// cleanup
	fclose( hog_fp );	
	delete[] table;

	// Setup the update message and send it
	char msg[256];
	sprintf(msg,"Done Creating Hog File.");
	UpdateFunction(msg);

	// If new hog was written to temp file, delete the original and
	// rename the temp to be the original
	if(!stricmp(dest_hog_fname,TEMP_HOG_FILENAME)) {
		DeleteFile(target_hog_fname);
		rename(TEMP_HOG_FILENAME,target_hog_fname);
	}

	return HOGMAKER_OK;
}


// Update menu items
void CHogEditDoc::OnUpdateActionInsert(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	/*pCmdUI->Enable(!m_StaticHog);*/
}

void CHogEditDoc::OnUpdateActionCreate(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(!m_StaticHog && Library.filelist.GetCount()>0);
}

void CHogEditDoc::OnUpdateFileSave(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(/*!m_StaticHog &&*/ DocModified);	
}

void CHogEditDoc::OnUpdateFileSaveAs(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	/*pCmdUI->Enable(!m_StaticHog);*/	
}

void CHogEditDoc::OnUpdateActionUpdate(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(!m_StaticHog && Library.filelist.GetCount()>0);
}

void CHogEditDoc::OnUpdateActionImport(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	//pCmdUI->Enable(!m_StaticHog);	
}
