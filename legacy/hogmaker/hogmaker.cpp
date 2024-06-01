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

#include <dos.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <ctype.h>

//#include "mvlfile.h"		//for old D2 movie libraries
#include "hogfile.h"			//new D3 hogs

/*
#define BUFFER_SIZE		(1024*1024*5)	//5 MB

struct library_entry {
	char	name[LIB_FILENAME_LEN];	//just the filename part
	int	offset;						//offset into library file
	int	length;						//length of this file
  int32_t	timestamp;					//time and date of file
	int	flags;						//misc flags
};

struct library {
	int nfiles;
	library_entry *table;
	FILE *fp;
};

library *open_hogfile(char *hogname)
{
	FILE *fp;
	int id;
	int i,offset,t;
	library *hogfile;
	index_entry *index;

	fp = fopen( hogname, "rb" );
	if ( fp == NULL )
		return NULL;

	fread( &id, 4, 1, fp );
	if ( strncmp( (char *) &id, LIBRARY_TAG, strlen(LIBRARY_TAG) ) ) {
		printf("Invalid library file <%s>\n",hogname);
		exit(11);
	}

	hogfile = malloc(sizeof(*hogfile));

	hogfile->fp = fp;

	fread(&hogfile->nfiles,4,1,hogfile->fp);		//get number of files

	hogfile->table = malloc(sizeof(library_entry)*hogfile->nfiles);

	offset = 4+4+hogfile->nfiles*(13+4);	//id + nfiles + nfiles * (filename + size)

	//allocate index
	index = malloc(sizeof(index_entry) * hogfile->nfiles);

	//read in index table
	t = fread( index, sizeof(*index), hogfile->nfiles, hogfile->fp );
	if ( t != hogfile->nfiles )	{		//eof here is ok
		fclose(fp);
		return 0;	//CF_BAD_LIB;
	}

	//copy disk index to our internal structure
	for (i=0;i<hogfile->nfiles;i++) {

		strcpy(hogfile->table[i].name, index[i].name);
		hogfile->table[i].length = index[i].len;
		hogfile->table[i].offset = offset;
		offset += hogfile->table[i].length;

		#if LIB_HAS_DATE
			hogfile->table[i].timestamp = index[i].timestamp;
		#else
			hogfile->table[i].timestamp  = 0;
		#endif

		#if LIB_HAS_FLAGS
			hogfile->table[i].flags = index[i].flags;
		#else
			hogfile->table[i].flags = 0;
		#endif
	}

	return hogfile;
}

void close_hogfile(library *hogfile)
{
	fclose(hogfile->fp);
	free(hogfile->table);
	free(hogfile);
}

copy_file(FILE *ofp,FILE *ifp,int length)
{
	char *buffer;

	buffer = malloc(BUFFER_SIZE);
	if (!buffer) {
		printf("Error allocating buffer\n");
		exit(10);
	}

	while (length) {
		int n,read_len;

		read_len = min(length,BUFFER_SIZE);

		n = fread( buffer, 1, read_len, ifp );
		if ( n != read_len )	{
			printf( "Error reading file\n");
			free(buffer);
			return 0;
		}

		if (fwrite( buffer, 1, read_len, ofp) != read_len )	{
			printf("Error writing file\n");
			free(buffer);
			return 0;
		}

		length -= read_len;
	}

	free(buffer);

	return 1;
}

//returns 1 if extracted ok
int extract_file_by_index(library *hogfile,int index)
{
	int length;
	FILE *ofp;

	ofp = fopen( hogfile->table[index].name, "wb" );
	if ( ofp == NULL )	{
		printf( "Error opening '%s'\n", hogfile->table[index].name );
		return 0;
	}

	length = hogfile->table[index].length;

	fseek(hogfile->fp,hogfile->table[index].offset,SEEK_SET);

	copy_file(ofp,hogfile->fp,hogfile->table[index].length);

	fclose(ofp);

	#ifdef LIB_HAS_DATE
	ofp = fopen(hogfile->table[index].name, "r+b");
	_dos_setftime(fileno(ofp),(hogfile->table[index].timestamp>>16),(hogfile->table[index].timestamp&0xFFFF));
	fclose(ofp);
	#endif

	return 1;
}

extract_file_by_name(char *hogname,char *filename)
{
	library *hogfile;
	int i;

	hogfile = open_hogfile(hogname);

	if (!hogfile) {
		printf("Error opening library <%s>\n",hogname);
		exit(7);
	}

	for (i=0;i<hogfile->nfiles;i++)
		if (!stricmp(filename,hogfile->table[i].name)) {
			int ok;
			printf("  Extracting %s\n",hogfile->table[i].name);
			ok = extract_file_by_index(hogfile,i);
			if (!ok) {
				printf("Error extracting <%s> from <%s>\n",hogfile->table[i].name,hogname);
				exit(1);
			}
			break;
		}

	if (i==hogfile->nfiles)
		printf("Error: couldn't find file <%s> in <%s>\n",filename,hogname);

	close_hogfile(hogfile);
}

extract_all_files(char *hogname)
{
	library *hogfile;
	int i;

	hogfile = open_hogfile(hogname);

	if (!hogfile) {
		printf("Error opening library <%s>\n",hogname);
		exit(7);
	}

	for (i=0;i<hogfile->nfiles;i++) {
		int ok;

		printf("  Extracting %s\n",hogfile->table[i].name);
		ok = extract_file_by_index(hogfile,i);

		if (!ok) {
			printf("Error extracting <%s> from <%s>\n",hogfile->table[i].name,hogname);
			exit(1);
		}
	}

	close_hogfile(hogfile);
}

#define YEAR(t)	(((t & 0xFE000000) >> 25) + 80)
#define MONTH(t)	 ((t & 0x01E00000) >> 21)
#define DAY(t)		 ((t & 0x001F0000) >> 16)
#define HOUR(t)	 ((t & 0x0000F800) >> 11)
#define MINUTE(t)	 ((t & 0x000007E0) >> 5)
#define SECOND(t)	 ((t & 0x0000001F) << 1)

list_files(char *hogname)
{
	library *hogfile;
	int i;

	hogfile = open_hogfile(hogname);

	if (!hogfile) {
		printf("Error opening library <%s>\n",hogname);
		exit(7);
	}

	printf("Contents of %s:\n",hogname);

	printf(" Name            Size   Date/Time            Flags\n");
	printf(" --------------  -----  ------------------   -------\n");

	for (i=0;i<hogfile->nfiles;i++) {
		printf(" %-12s  %7d",hogfile->table[i].name,hogfile->table[i].length);

		if (hogfile->table[i].timestamp) {
      int32_t t = hogfile->table[i].timestamp;
			printf("  %2d/%02d/%02d  %2d:%02d:%02d",MONTH(t),DAY(t),YEAR(t),HOUR(t),MINUTE(t),SECOND(t));
		}
		else
			printf("                    ");

		printf("  %8x\n",hogfile->table[i].flags);
	}

	close_hogfile(hogfile);
}

#define REMOVE_EOL(s)		remove_char((s),'\n')
#define REMOVE_COMMENTS(s)	remove_char((s),';')
#define REMOVE_DOTS(s)  	remove_char((s),'.')

void remove_char( char * s, char c )
{
	char *p;
	p = strchr(s,c);
	if (p) *p = '\0';
}

#define MAX_LINE_LEN	160

int parse_listfile(char *listfile_name,char ***filenames_ptr)
{
	FILE *fp;
	char	inputline[MAX_LINE_LEN+1];
	int count;
	char **filenames;

	fp = fopen( listfile_name, "rt" );

	if (!fp) {
		printf("Error: can't open file list <%s>\n",listfile_name);
		exit(3);
	}

	count = 0;

	while (fgets(inputline, MAX_LINE_LEN, fp )) {
		char	filename[MAX_LINE_LEN+1];
		REMOVE_EOL(inputline);
		REMOVE_COMMENTS(inputline);
		if (strlen(inputline) > 0) {		
			sscanf( inputline, " %s ", filename );
			if ( strlen( filename ) > 0 )
				count++;
		}
	}

	fseek(fp,0,SEEK_SET);

	filenames = malloc(sizeof(*filenames) * count);

	count = 0;

	while (fgets(inputline, MAX_LINE_LEN, fp )) {
		char	filename[MAX_LINE_LEN+1];
		REMOVE_EOL(inputline);
		REMOVE_COMMENTS(inputline);
		if (strlen(inputline) > 0) {		
			sscanf( inputline, " %s ", filename );
			if ( strlen( filename ) > 0 ) {
				filenames[count] = malloc(strlen(filename)+1);
				strcpy(filenames[count],filename);
				count++;
			}
		}
	}
	
	fclose(fp);

	*filenames_ptr = filenames;

	return count;
}

create_new_library(char *hogname, int nfiles, char **filenames)
{
	int i,table_pos;
	FILE *hog_fp;
	index_entry *index;
	char ext[_MAX_EXT];

	index = malloc(sizeof(index_entry) * nfiles);
	if (!index) {
		printf("Error allocating index table\n");
		exit(9);
	}

	//open the new file
	hog_fp = fopen( hogname, "wb" );
	if ( hog_fp == NULL )		{
		printf( "Error opening file <%s>\n",hogname);
		exit(1);
	}

	//write the tag
	if (fwrite( LIBRARY_TAG, strlen(LIBRARY_TAG), 1, hog_fp )!=1)	{
		printf("Error writing tag tp file <%s>\n",hogname);
		fclose( hog_fp );	
		exit(1);
	}

	//write number of files
	if (fwrite(&nfiles,sizeof(int),1,hog_fp)!=1)	{
		printf("Error writing count to file <%s>\n",hogname);
		fclose( hog_fp );	
		exit(1);
	}

	//save file position of index table
	table_pos = ftell(hog_fp);

	//write dummy index
	if (fwrite(index,sizeof(*index),nfiles,hog_fp)!=nfiles)	{
		printf("Error writing index to file <%s>\n",hogname);
		fclose( hog_fp );	
		exit(1);
	}

	//write files (& build index)
	for (i=0;i<nfiles;i++) {
		FILE *ifp;
	   uint16_t date,time;

		ifp = fopen(filenames[i],"rb");
		if ( ifp == NULL )	{
			printf( "Error opening input file <%s>\n",filenames[i]);
			fclose(hog_fp);
			exit(10);
		}

		_splitpath(filenames[i],NULL,NULL,index[i].name,ext);
		strcat(index[i].name,ext);
		index[i].len = filelength(fileno(ifp));

		#ifdef LIB_HAS_DATE
		_dos_getftime(fileno(ifp), &date,&time);
		index[i].timestamp = (date << 16) + time;
		#endif

		#ifdef LIB_HAS_FLAGS
		index[i].flags = 0;
		#endif

		copy_file(hog_fp,ifp,index[i].len);
	}

	//now write the real index
	fseek(hog_fp,table_pos,SEEK_SET);
	if (fwrite(index,sizeof(*index),nfiles,hog_fp)!=nfiles)	{
		printf("Error writing index to file <%s>\n",hogname);
		fclose( hog_fp );	
		exit(1);
	}

	fclose( hog_fp );	
}

add_files(char *hogname, int argc, char *argv[])
{
	library *hogfile;
	int nfiles,i;
	char **filenames;

	hogfile = open_hogfile(hogname);

	if (hogfile) {
		printf("Sorry, cannot add files to existing library <%s>\n",hogname);
		exit(4);
	}

	//hog doesn't exist, so create new one

	if (argc != 1) {
		printf("Error: input for add files must be a listfile\n");
		exit(2);
	}
 
	nfiles = parse_listfile(argv[0],&filenames);
	
	create_new_library(hogname,nfiles,filenames);

	//now free filename list
	for (i=0;i<nfiles;i++)
		free(filenames[i]);
	free(filenames);
}


show_help()
{
	printf( "Usage: mvlutil command hogfile [file1 [file2] [...]]\n" );
	printf( "Valid commands:\n");
	printf( "  a  add files\n");
	printf( "  l  list files\n");
	printf( "  x  extract files\n");
}
*/
int main(int argc, char * argv[] )	
{
/*	char command;
	char *hogname;

	if ( argc < 3 )	{
		show_help();
		exit(1);
	}

	hogname = argv[2];
	command = tolower(argv[1][0]);

	if (command == 'a') {
		add_files(hogname,argc-3,argv+3);
	}
	else if (command == 'l') {
		list_files(hogname);
	}
	else if (command == 'x') {
		if (argv[1][1])
			extract_file_by_name(hogname,&argv[1][1]);
		else
			extract_all_files(hogname);
	}
	else {
		show_help();
		exit(2);
	}
*/
	return 0;
}

