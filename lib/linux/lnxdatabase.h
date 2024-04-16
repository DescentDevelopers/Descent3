/*
 *	Application Database for Linux version
 * 
 * $NoKeywords: $
 */

#ifndef LINUXDATABASE
#define LINUXDATABASE

#include <stdio.h>
#include <stdlib.h>

class CRegistry;

#include "Macros.h"

/* oeLnxAppDatabase
       to get info about the application from a managed database (or a custom info file)
*/

class oeLnxAppDatabase: public oeAppDatabase
{

protected:
	CRegistry *database;
public:
	oeLnxAppDatabase();
	oeLnxAppDatabase(oeLnxAppDatabase *parent);
	virtual ~oeLnxAppDatabase();
	CRegistry *GetSystemRegistry();

//	creates an empty classification or structure where you can store information
	virtual bool create_record(const char *pathname);

//	set current database focus to a particular record
	virtual bool lookup_record(const char *pathname);

//	read either an integer or string from the current record
	virtual bool read(const char *label, char *entry, int *entrylen);
	virtual bool read(const char *label, void *entry, int wordsize);	 //read a variable-size int
	virtual bool read(const char *label, bool *entry);

//	write either an integer or string to a record.
	virtual bool write(const char *label, const char *entry, int entrylen);
	virtual bool write(const char *label, int entry);
	
// get the current user's name.
	virtual void get_user_name(char* buffer, ulong* size);
};

//Handy macro to read an int without having to specify the wordsize
#define read_int(label,varp) read(label,varp,sizeof(*varp))

//Macro to write a string
#define write_string(label,varp) write(label,varp,strlen(varp))

#endif

