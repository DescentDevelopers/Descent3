/*
 * $Logfile: /DescentIII/Main/mac/macdatabase.cpp $
 * $Revision: 1.1.1.1 $
 * $Date: 2003/08/26 03:58:15 $
 * $Author: kevinb $
 * 
 * Mac Database objects functions
 *
 * $Log: macdatabase.cpp,v $
 * Revision 1.1.1.1  2003/08/26 03:58:15  kevinb
 * initial 1.5 import
 *
 * 
 * 3     10/21/99 1:55p Kevin
 * Mac Merge!
 * 
 * 2     7/28/99 2:51p Kevin
 * 
 * 3     5/15/97 2:34 PM Jeremy
 * made database return user name or default user name if no user name in
 * database
 * 
 * 2     5/15/97 1:50 AM Jeremy
 * correct and more complete implementation of macOSDatabase object which
 * uses the mac resource manager to store entries in the application
 * preferences file (used like the windows registry file)
 * 
 * 1     4/11/97 4:12 PM Jeremy
 * initial checkin
 * 
 * $NoKeywords: $
 */

// ANSI Headers
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Macintosh Headers
#include <Files.h>
#include <Processes.h>
#include <Folders.h>
#include <Script.h>
#include <Resources.h>
#include <Errors.h>

// MacPlayLib Headers
#include "PString.h"
#include "File Utils.h"

// Descent 3 Headers
#include "mono.h"
#include "pserror.h"
#include "gameos.h"

//#define ASSERT(x)
//#define mprintf(x)
//#define _MAX_FNAME 32
//#define _MAX_DIR 32
//typedef unsigned long ulong;
//#include <Fonts.h>
//#include <Windows.h>
//#include <Menus.h>
//#include <TextEdit.h>
//#include <Dialogs.h>

// ----------------------------------------------------------------------------
//	Internal Data Types
// ----------------------------------------------------------------------------
class CDatabaseErr
{
	public:
	CDatabaseErr(char* inErrStr = NULL, OSErr inMacErr = noErr)
	{
		mErrStr = inErrStr;
		mMacErr = inMacErr;
	}
	
	char* mErrStr;
	OSErr mMacErr;
};


// ----------------------------------------------------------------------------
//	Functions
// ----------------------------------------------------------------------------

osMacDatabase::osMacDatabase(void)
{
	mprintf((0, "Creating mac database object\n"));
	
	mInitted 		= false;

	mAppSignature 	= '????';

	memset(mPrefsFileName, 0, sizeof(mPrefsFileName));	
	memset(mPrefsFolderName, 0, sizeof(mPrefsFolderName));	
	mPrefsFileType 	= '????';
	memset(&mPrefsFileSpec, 0, sizeof(mPrefsFileSpec));
	mPrefsFileRefNum = -1;
}

osMacDatabase::~osMacDatabase(void)
{
	mprintf((0, "Destroying mac database object\n"));
}

bool
osMacDatabase::init(void)
{
	bool 				success = false;
	OSErr	 			err = noErr;
	ProcessSerialNumber	myPSN;
	ProcessInfoRec		myInfo;	

	// Get information about the current process (this app)
	err = GetCurrentProcess(&myPSN);
	
	memset(&myInfo, 0, sizeof(myInfo));
	myInfo.processInfoLength = sizeof(myInfo);
	
	err = GetProcessInformation(&myPSN, &myInfo);
	if (!err)
	{
		bool prefsSuccess  = false;

		mAppSignature = myInfo.processSignature;

		prefsSuccess = FillOutPrefsInfo();
		
		if (prefsSuccess)
		{
			InitPrefsFile();
			success = true;
		}
	}
	
	mInitted = true;
	
	return mInitted;
}

//	read either an integer or string from the current record
bool
osMacDatabase::read(const char *label, char *entry, int *entrylen)
{
	return ReadDataFromResourceFork(label, entry, entrylen);
}

bool
osMacDatabase::read(const char *label, int *entry)
{
	int intSize = sizeof(int);
	
	return ReadDataFromResourceFork(label, entry, &intSize);
}

//	read either an integer or string from the current record
bool
osMacDatabase::ReadDataFromResourceFork(const char* label, void* entry, int* entrylen)
{
	ASSERT(label);
	ASSERT(entry);
	ASSERT(entrylen);

	short	saveResFile = CurResFile();
	OSErr	err = noErr;
	bool	success = false;
	
	try
	{
		//¥ Start by terminating the entry in case anything goes wrong
		((char*) entry)[0] = 0;
		
		//¥ Open the resource file (must be closed later)
		mPrefsFileRefNum = FSpOpenResFile(&mPrefsFileSpec, fsRdWrPerm);
		if (mPrefsFileRefNum == -1)
		{
			err = ResError();
			
			if (err)
			{
				throw (CDatabaseErr("An error occurred opening the prefs file resource fork.", err));
			}
		}
		
		//¥ Switch to the prefs file's resource fork
		UseResFile(mPrefsFileRefNum);
		err = ResError();
		if (err)
		{
			throw (CDatabaseErr("Could not switch to database resource file.", err));
		}
		
		//¥ Convert the name string from a c to a pascal style string
		Str255	labelPStr = "\p";
		CPstrcpy(labelPStr, (char*) label);

		//¥ See if this resource exists in the prefs file database
		Handle	dataHandle = nil;
		dataHandle = Get1NamedResource(kMacDatabaseResourceType, labelPStr);
		err = ResError();
		if (err == resNotFound)
		{
			mprintf((0, "Could not find label %s  in the database.\n", label));
			throw (CDatabaseErr("Entry does not exist.", err));
		}
		else if (err)
		{
			mprintf((0, "LABEL: %s\n", label));
			throw (CDatabaseErr("Error getting label %s from the database.", err));
		}
		
		ASSERT(dataHandle);		
		
		//¥ Lock the handle down in memory
		HLock(dataHandle);
		
		//¥ Copy the data to the buffer passed in by the database user
		//   Note! Only copies up to as many bytes as specified by entrylen initially, returns the 
		//		   actual size of the data in entrylen on completion 
		int	dataSize = GetHandleSize(dataHandle);
		*entrylen = (*entrylen < dataSize) ? *entrylen : dataSize;
		BlockMove(*dataHandle, entry, *entrylen);

		//¥ Unlock the memory handle
		HUnlock(dataHandle);
				
		success = true;
	}
	catch (CDatabaseErr databaseErr)
	{
		mprintf((0, "An error occurred reading from the mac resource database: %d\n", databaseErr.mMacErr));
		mprintf((0, databaseErr.mErrStr));
		mprintf((0, "\n"));
		success = false;
	}

	CloseResFile(mPrefsFileRefNum);
	mPrefsFileRefNum = -1;
	
	//¥ Restore the original resource file
	UseResFile(saveResFile);

	return success;
}

//	write either an integer or string to a record.
bool
osMacDatabase::write(const char *label, char *entry, int entrylen)
{
	return WriteDataToResourceFork(label, entry, entrylen);
}

bool
osMacDatabase::write(const char *label, int *entry)
{
	return WriteDataToResourceFork(label, entry, sizeof(int));
}

bool
osMacDatabase::WriteDataToResourceFork(const char* label, void* entry, int entrylen)
{
	short	saveResFile = CurResFile();
	OSErr	err = noErr;
	bool	success = false;
	
	try
	{
		//¥ Open the resource file (must be closed later in destructor)
		mPrefsFileRefNum = FSpOpenResFile(&mPrefsFileSpec, fsRdWrPerm);
		if (mPrefsFileRefNum == -1)
		{
			err = ResError();
			
			if (err)
			{
				throw (CDatabaseErr("An error occurred opening the prefs file resource fork.", err));
			}
		}
		
		//¥ Switch to the prefs file's resource fork
		UseResFile(mPrefsFileRefNum);
		err = ResError();
		if (err)
		{
			throw (CDatabaseErr("Could not switch to database resource file.", err));
		}
		
		//¥ Convert the name string from a c to a pascal style string
		Str255	labelPStr = "\p";
		CPstrcpy(labelPStr, (char*) label);

		//¥ See if this resource already exists in the prefs file database
		Handle	oldHandle = nil;
		oldHandle = Get1NamedResource(kMacDatabaseResourceType, labelPStr);
		err = ResError();
		if (err && (err != resNotFound))
		{
			throw (CDatabaseErr("Could not check if database entry already exists.", err));
		}
		
		if (oldHandle != nil)	// If this resource exists, then delete it
		{
			//¥ Now delete the old resource
			RemoveResource(oldHandle);
			err = ResError();
			if (err)
			{
				throw (CDatabaseErr("Could not get delete previously existing database entry", err));
			}
			
			//¥ Write the changes to disk
			UpdateResFile(mPrefsFileRefNum);
			err = ResError();
			if (err)
			{
				throw (CDatabaseErr("Could add write mac database resource to disk.", err));
			}
		}

		//¥ Create a handle to store the entry in
		Handle	entryH = nil;
		entryH = NewHandleClear(entrylen);
		if (!entryH)
		{
			throw (CDatabaseErr("Could not allocate memory for new database entry", nilHandleErr));
		}
		
		//¥ Lock the handle down in memory
		HLock(entryH);
		
		//¥ Copy the new entry data to a handle to be added as a named resource
		BlockMove(entry, *entryH, entrylen);

		//¥ Get a unique ID for this resource
		short entryResID = 0;
		entryResID = Unique1ID(kMacDatabaseResourceType);	
	
		//¥ Add the resource to the database
		AddResource(entryH, kMacDatabaseResourceType, entryResID, labelPStr);
		err = ResError();
		if (err)
		{
			throw (CDatabaseErr("Could not add entry to mac database resource file.", err));
		}
		
		//¥ Write the changes to disk
		UpdateResFile(mPrefsFileRefNum);
		err = ResError();
		if (err)
		{
			throw (CDatabaseErr("Could add write mac database resource to disk.", err));
		}
		
		//¥ Unlock the memory handle
		HUnlock(entryH);

		//¥ Dispose of the memory handle
		DisposeHandle(entryH);
		
		success = true;
	}
	catch (CDatabaseErr databaseErr)
	{
		mprintf((0, "An error occurred writing to the mac resource database: %d\n", databaseErr.mMacErr));
		mprintf((0, databaseErr.mErrStr));
		mprintf((0, "\n"));
		success = false;
	}

	CloseResFile(mPrefsFileRefNum);
	mPrefsFileRefNum = -1;
	
	//¥ Restore the original resource file
	UseResFile(saveResFile);

	return success;
}
	


// get the current user's name from the os
void
osMacDatabase::get_user_name(char* buffer, ulong* size)
{
	bool success = false;
	
	success = read("user name", buffer, (int*) size);
	
	if (!success)
	{
		mprintf((0, "Error reading user name from data base!\n"));
		mprintf((0, "Using default name\n"));

		char	defaultName[100] = "MacUser";
		strncpy(buffer, defaultName, *size);
		*size = strlen(defaultName) + 1;
	}
}

//	creates an empty classification or structure where you can store information
bool
osMacDatabase::create_record(const char *pathname)
{
	bool result = false;
	
	
	return result;
}

//	set current database focus to a particular record
bool
osMacDatabase::lookup_record(const char *pathname)
{
	bool result = false;
	
	return result;
}

bool
osMacDatabase::FillOutPrefsInfo(void)
{
	ProcessSerialNumber	thePSN;
	ProcessInfoRec		thePIR;
	FSSpec				appSpec;
	OSErr				theErr	= noErr;
	bool				success = false;
	
	//¥ Get information about the current process (this app)
	theErr = GetCurrentProcess(&thePSN);
	thePIR.processName = nil;
	thePIR.processInfoLength = sizeof(ProcessInfoRec);
	thePIR.processAppSpec = &appSpec;
	
	theErr = GetProcessInformation(&thePSN, &thePIR);
	if (!theErr)
	{
		//¥ Make sure the constructed name will not overflow the maximum file/folder length
		Str31	fileTag 		= "\p Prefs";
		Str31	folderTag 		= "\p Preferences";
		int 	fileTagLength 	= (int) fileTag[0];
		int 	folderTagLength = (int) folderTag[0];

		Str255	tempAppName		="\p";
		int tempAppNameLength 	= 0;
		
		
		
		//¥ Shorten the app name length until it will fit into the file name
		Pstrcpy(tempAppName, appSpec.name);		
		tempAppNameLength = tempAppName[0];
		while (fileTagLength + tempAppNameLength >= _MAX_FNAME)
		{
			tempAppNameLength--;
		}
		tempAppName[0] = tempAppNameLength;
		
		//¥ Construct the Prefs File Name
		Pstrcpy(mPrefsFileName, tempAppName);
		Pstrcat(mPrefsFileName, fileTag);
		
		
		
		
		//¥ Shorten the app name length until it will fit into the folder name
		Pstrcpy(tempAppName, appSpec.name);		
		tempAppNameLength = tempAppName[0];
		while (folderTagLength + tempAppNameLength >= _MAX_DIR)
		{
			tempAppNameLength--;
		}
		tempAppName[0] = tempAppNameLength;

		//¥ Construct the Prefs Folder Name
		Pstrcpy(mPrefsFolderName, tempAppName);
		Pstrcat(mPrefsFolderName, folderTag);

		success = true;
	}
	
	return success;
}

bool
osMacDatabase::InitPrefsFile(void)
{
	long	newDirID 	= 0;
	short	prefVRefNum	= 0;
	long	prefDirID	= 0;
	OSErr	theErr 		= noErr;
	bool	success 	= false;
		
	try
	{
		//¥	Find the System "Preferences" folder
		theErr = FindFolder(kOnSystemDisk, kPreferencesFolderType, kCreateFolder, &prefVRefNum, &prefDirID);
		
		//¥	Find (or make) the app prefs folder in the Preferences folder
		mPrefsFileSpec.vRefNum 	= prefVRefNum;
		mPrefsFileSpec.parID	= prefDirID;
		Pstrcpy(mPrefsFileSpec.name, mPrefsFolderName);
		
		theErr = NormalizeFolderSpec(&mPrefsFileSpec);
		if (theErr)
		{
			if (theErr != fnfErr)
			{
				// A Real Error Occurred
				throw (CDatabaseErr("An Error occurred getting at the preferences folder", theErr));
			}
			
			//¥	Create the app prefs folder in the Preferences folder
			theErr = FSpDirCreate(&mPrefsFileSpec, smCurrentScript, &newDirID);
			
			if (theErr)
			{
				throw (CDatabaseErr("Error creating preferences folder", theErr));
			}

			mPrefsFileSpec.parID = newDirID;
		}

		//¥	At this point we should have a valid FSSpec for items inside the app prefs folder
		Pstrcpy(mPrefsFileSpec.name, mPrefsFileName);

		//¥	If we make it here OK, create Preferences file if necessary
		theErr = FSpCreate(&mPrefsFileSpec, mAppSignature, mPrefsFileType, smCurrentScript);

		//¥ If there was no error, then file did not already exist,
		//   and we must create the resource map in the file
		if (!theErr) 
		{
			FSpCreateResFile(&mPrefsFileSpec, mAppSignature, mPrefsFileType, smCurrentScript);
			theErr = ResError();
			
			if (theErr)
			{
				throw (CDatabaseErr("An error occurred creating the prefs file resource map.", theErr));
			}
		}
		else
		{
			if (theErr != dupFNErr)	// If the error is not "duplicate file error" then there is a problem
			{
				// We have a real error
				throw (CDatabaseErr("An error occurred creating the prefs file.", theErr));
			}
		}
		
		success = true;
	}
	catch (CDatabaseErr databaseErr)
	{
		mprintf((0, "ERROR in osMacDatabase::InitPrefsFile: %d\n", databaseErr.mMacErr));
		mprintf((0, databaseErr.mErrStr));
		success = false;
	}
	
	return success;
}

