/*
 * $Logfile: /DescentIII/Main/lib/win/win32database.h $
 * $Revision: 6 $
 * $Date: 5/11/99 12:12a $
 * $Author: Ardussi $
 *
 *	Application Database for Win32 version
 *
 * $Log: /DescentIII/Main/lib/win/win32database.h $
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

#ifndef WIN32DATABASE
#define WIN32DATABASE


/* oeWin32AppDatabase
	to get info about the application from a managed database (or a custom info file)
	we get our information from the registry!
*/

class oeWin32AppDatabase: public oeAppDatabase
{
	unsigned hBaseKey;							// look up from this key.
	unsigned hCurKey;							// current key for lookup

protected:
	char m_Basepath[256];

public:
	oeWin32AppDatabase();
	oeWin32AppDatabase(oeWin32AppDatabase *parent);
	virtual ~oeWin32AppDatabase();

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
bool win32_SetResourceDLL(const char *libname);

// returns a string from the current resource
bool win32_GetStringResource(int txt_id, char *buffer, int buflen);


#endif