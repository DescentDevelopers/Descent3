/*
 * $Logfile: /DescentIII/Main/mac/MACDATABASE.H $
 * $Revision: 1.1.1.1 $
 * $Date: 2003/08/26 03:58:15 $
 * $Author: kevinb $
 *
 *	Application Database for Win32 version
 *
 * $Log: MACDATABASE.H,v $
 * Revision 1.1.1.1  2003/08/26 03:58:15  kevinb
 * initial 1.5 import
 *
 * 
 * 3     4/12/00 7:09p Matt
 * From Duane for 1.4
 * 
 * 2     10/21/99 1:55p Kevin
 * Mac Merge!
 * 
 * 1     7/28/99 2:31p Kevin
 * Mac only stuff
 * 
 * 6     5/11/99 12:12a Ardussi
 * changes to compile on Mac
 * 
 * 5     4/24/99 5:47p Samir
 * added functions to set current win32 resource dll or exe.
 * 
 * 4     7/27/97 11:07p Matt
 * Added write_string() macro
 * 
 * 3     7/24/97 3:06p Matt
 * Added functions to read & write bools & variable-length integers, and
 * fixed a few small bugs.
 * 
 * 2     6/11/97 2:41p Samir
 * fixed class declaration.
 * 
 * 1     6/10/97 4:53p Samir
 * Database control for Win32 systems
 * 
 * $NoKeywords: $
 */
#ifndef MACDATABASE
#define MACDATABASE
#include <stdio.h>
typedef struct DBEntry
{
	char *label, *entry;
	struct DBEntry *next;
} DBEntry;
typedef struct DBRecord
{
	char name[64];
	DBEntry	*entries;
	struct DBRecord *next;
} DBRecord;
/* oeWin32AppDatabase
	to get info about the application from a managed database (or a custom info file)
	we get our information from the registry!
*/
class oeMacAppDatabase: public oeAppDatabase
{
	//unsigned hBaseKey;							// look up from this key.
	//unsigned hCurKey;							// current key for lookup
protected:
	//char m_Basepath[256];
	FILE *file;
	DBRecord *data;
	DBRecord *current;
	
	void DBReadIn(void);
	void DBWriteOut(void);
public:
	oeMacAppDatabase();
	oeMacAppDatabase(oeMacAppDatabase *parent);
	virtual ~oeMacAppDatabase();
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
// pass name of dll which contains desired language
// NULL library is the default resource DLL
bool mac_SetResourceDLL(const char *libname);
// returns a string from the current resource
bool mac_GetStringResource(int txt_id, char *buffer, int buflen);
#endif