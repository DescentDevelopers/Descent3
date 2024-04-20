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

/*
 * $Logfile: /DescentIII/Main/hogmaker/hog.cpp $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:57:56 $
 * $Author: kevinb $
 *
 * 
 *
 * $Log: not supported by cvs2svn $
 * 
 * 2     3/31/98 6:13p Samir
 * new hogfile format.
 * 
 * 1     3/31/98 5:15p Samir
 * new hog file read and write module.
 *
 * $NoKeywords: $
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <io.h>
#include <sys/stat.h>

#include "hogfile.h"
#include "pstypes.h"
#include "macros.h"


/*	HOG FILE FORMAT v2.0 
		
		HOG_TAG_STR			[strlen()]
		NFILES				[int32]
		HDRINFO				[HOG_HDR_SIZE]
		FILE_TABLE			[sizeof(FILE_ENTRY) * NFILES]
		FILE 0				[filelen(FILE 0)]
		FILE 1				[filelen(FILE 1)]
		.
		.
		.
		FILE NFILES-1		[filelen(NFILES -1)]
*/



////////////////////////////////////////////////////////////////////////

bool CopyFile(FILE *ofp,FILE *ifp,int length)
{
	#define BUFFER_SIZE			(1024*1024)			//	1 meg

	char *buffer;

	buffer = (char *)malloc(BUFFER_SIZE);
	if (!buffer) 
		return false;

	while (length) 
	{
		size_t n,read_len;

		read_len = min(length,(int)BUFFER_SIZE);

		n = fread( buffer, 1, read_len, ifp );
		if ( n != read_len )	{
			free(buffer);
			return false;
		}

		if (fwrite( buffer, 1, read_len, ofp) != read_len )	{
			free(buffer);
			return false;
		}

		length -= read_len;
	}

	free(buffer);

	return true;
}


bool ReadHogHeader(FILE *fp, tHogHeader *header)
{
	int res=0;
	res = fread(&header->nfiles, sizeof(header->nfiles), 1, fp);
	res = fwrite(&header->file_data_offset, sizeof(header->file_data_offset), 1, fp);
	
	if (res) 
		return true;
	else
		return false;
}


bool ReadHogEntry(FILE *fp, tHogFileEntry *entry)
{
	int res=0;
	res = fread(entry->name, strlen(entry->name), 1, fp);
	res = fread(&entry->flags, sizeof(entry->flags), 1, fp);
	res = fread(&entry->len, sizeof(entry->len), 1, fp);
	res = fread(&entry->timestamp, sizeof(entry->timestamp), 1, fp);
	
	if (res) 
		return true;
	else
		return false;
}


inline bool WRITE_FILE_ENTRY(FILE *fp, tHogFileEntry *entry)
{
	int res=0;
	res = fwrite(entry->name, strlen(entry->name), 1, fp);
	res = fwrite(&entry->flags, sizeof(entry->flags), 1, fp);
	res = fwrite(&entry->len, sizeof(entry->len), 1, fp);
	res = fwrite(&entry->timestamp, sizeof(entry->timestamp), 1, fp);
	
	if (res) 
		return true;
	else
		return false;
}



////////////////////////////////////////////////////////////////////////
//	create new hog file

#define HOGMAKER_ERROR		0
#define HOGMAKER_OK			1
#define HOGMAKER_MEMORY		2
#define HOGMAKER_OUTFILE	3
#define HOGMAKER_INFILE		4
#define HOGMAKER_COPY		5

int NewHogFile(const char *hogname, int nfiles, const char **filenames)
{
	unsigned i;
	int table_pos;
	FILE *hog_fp;
	tHogHeader header;
	tHogFileEntry *table;
	char ext[_MAX_EXT];

//	allocate file table
	if (nfiles <= 0) 
		return HOGMAKER_ERROR;

	table = new tHogFileEntry[nfiles];
	if (!table) 
		return HOGMAKER_MEMORY;

//	create new file
	hog_fp = fopen( hogname, "wb" );
	if ( hog_fp == NULL )		{
		delete[] table;
		return HOGMAKER_OUTFILE;
	}

//write the tag
	if (!fwrite(HOG_TAG_STR, strlen(HOG_TAG_STR), 1, hog_fp ))	{
		delete[] table;
		fclose(hog_fp);
		return HOGMAKER_OUTFILE;
	}

//write number of files
	ubyte filler = 0xff;
	header.nfiles = (unsigned)nfiles;
	header.file_data_offset = strlen(HOG_TAG_STR) + HOG_HDR_SIZE + (sizeof(tHogFileEntry) * header.nfiles);

	if (!fwrite(&header.nfiles,sizeof(header.nfiles),1,hog_fp))	{
		delete[] table;
		fclose(hog_fp);
		return HOGMAKER_OUTFILE;
	}
	if (!fwrite(&header.file_data_offset,sizeof(header.file_data_offset),1,hog_fp))	{
		delete[] table;
		fclose(hog_fp);
		return HOGMAKER_OUTFILE;
	}
	if (!fwrite(&filler,sizeof(ubyte),HOG_HDR_SIZE-sizeof(tHogHeader),hog_fp))	{
		delete[] table;
		fclose(hog_fp);
		return HOGMAKER_OUTFILE;
	}

//save file position of index table and write out dummy table
	table_pos = strlen(HOG_TAG_STR) + HOG_HDR_SIZE;

	memset(&table[0], 0, sizeof(table[0]));

	for (i = 0; i < header.nfiles; i++)
	{
		if (!WRITE_FILE_ENTRY(hog_fp, &table[0])) {
			delete[] table;
			fclose(hog_fp);
			return HOGMAKER_OUTFILE;
		}
	}

//write files (& build index)
	for (i=0;i<header.nfiles;i++) 
	{
		FILE *ifp;
		struct _stat mystat;

		ifp = fopen(filenames[i],"rb");
		if ( ifp == NULL )	{
			delete[] table;
			fclose(hog_fp);
			return HOGMAKER_INFILE;
		}

		_splitpath(filenames[i],NULL,NULL,table[i].name,ext);
		_fstat(fileno(ifp), &mystat);

		strcat(table[i].name,ext);
		table[i].flags = 0;
		table[i].len = _filelength(_fileno(ifp));
		table[i].timestamp = mystat.st_mtime;

		if (!CopyFile(hog_fp,ifp,table[i].len)) {
			delete[] table;
			fclose(hog_fp);
			return HOGMAKER_COPY;
		}

		fclose(ifp);
	}

//now write the real index
	fseek(hog_fp,table_pos,SEEK_SET);

	for (i = 0; i < header.nfiles; i++)
	{
		if (!WRITE_FILE_ENTRY(hog_fp, &table[i])) {
			delete[] table;
			fclose(hog_fp);
			return HOGMAKER_OUTFILE;
		}
	}

//	cleanup
	fclose( hog_fp );	
	delete[] table;

	return HOGMAKER_OK;
}



/*	CopyFile
		used to copy one file to another.
*/

