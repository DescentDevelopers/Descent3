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
 * $Logfile: /DescentIII/Main/D3Launch/OS_Config.cpp $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:56:51 $
 * $Author: kevinb $
 *
 * 
 *
 * $Log: not supported by cvs2svn $
 * 
 * 9     3/12/99 3:29p Nate
 * Moved application registry names to LaunchNames.h
 * 
 * 8     2/15/99 1:22p Kevin
 * Changes for GameGauge
 * 
 * 7     2/05/99 3:51p Nate
 * Added conditional compilation directives for OEM support
 * 
 * 6     9/23/98 11:21a Nate
 * Added "demo" conditional compilation
 * 
 * 5     9/22/98 3:34p Nate
 * Changed registry key names to the "new ones"
 * 
 * 4     9/21/98 11:58a Nate
 * The section name is now NULL, so all launcher registry values will be
 * written into the main Descent III key
 * 
 * 3     8/10/98 10:44a Nate
 * Added Language selection support
 * 
 * 2     8/05/98 4:52p Nate
 * Added registry reading and writing.
 * 
 * 1     8/05/98 3:31p Nate
 * Initial Version
 *
 * $NoKeywords: $
 */

#include "stdafx.h"
#include "PsTypes.h"
#include "D3Launch.h"

#include "LaunchNames.h"

// System Registry Functions

char *szCompanyName = REG_COMPANY_NAME;
char *szAppName = REG_APP_NAME;
char *szSectionName = NULL;	// Don't use a section name for the launcher

// Removes a value from to the INI file.  Passing
// name=NULL will delete the section.

void os_config_remove( char *section, char *name )
{
	HKEY hKey = NULL;
	DWORD dwDisposition;
	char keyname[1024];
	LONG lResult;

	if ( section )	{
		sprintf( keyname, "Software\\%s\\%s\\%s", szCompanyName, szAppName, section );
	} else {
		sprintf( keyname, "Software\\%s\\%s", szCompanyName, szAppName );
	}

	// remove the value
	if ( !name )	{
		if ( !section )	{
			////mprintf(( "Can't delete root key\n" ));
			goto Cleanup;
		}
		lResult = RegDeleteKey( HKEY_LOCAL_MACHINE, keyname );
		if ( lResult != ERROR_SUCCESS )	{
			////mprintf(( "Error removing registry key '%s'\n", name ));
			goto Cleanup;
		}
	} else	{
		lResult = RegCreateKeyEx( HKEY_LOCAL_MACHINE,						// Where to add it
												 keyname,								// name of key
												 NULL,									// DWORD reserved
												 "",										// Object class
												 REG_OPTION_NON_VOLATILE,			// Save to disk
												 KEY_ALL_ACCESS,						// Allows all changes
												 NULL,									// Default security attributes
												 &hKey,							// Location to store key
												 &dwDisposition );					// Location to store status of key

		if ( lResult != ERROR_SUCCESS )	{
			//mprintf(( "Error opening registry key '%s'\n", keyname ));
			goto Cleanup;
		}

		lResult = RegDeleteValue( hKey, name );
		if ( lResult != ERROR_SUCCESS )	{
			//mprintf(( "Error removing registry value '%s'\n", name ));
			goto Cleanup;
		}
	}

Cleanup:
	if ( hKey )
		RegCloseKey(hKey);
}


// Writes a string to the INI file.  If value is NULL,
// removes the string. Writing a NULL value to a NULL name will delete
// the section.
void os_config_write_string( char *section, char *name, char *value )
{
	HKEY hKey = NULL;
	DWORD dwDisposition;
	char keyname[1024];
	LONG lResult;


	if ( section )	{
		sprintf( keyname, "Software\\%s\\%s\\%s", szCompanyName, szAppName, section );
	} else {
		sprintf( keyname, "Software\\%s\\%s", szCompanyName, szAppName );
	}

	lResult = RegCreateKeyEx( HKEY_LOCAL_MACHINE,						// Where to add it
											 keyname,								// name of key
											 NULL,									// DWORD reserved
											 "",										// Object class
											 REG_OPTION_NON_VOLATILE,			// Save to disk
											 KEY_ALL_ACCESS,						// Allows all changes
											 NULL,									// Default security attributes
											 &hKey,							// Location to store key
											 &dwDisposition );					// Location to store status of key

	if ( lResult != ERROR_SUCCESS )	{
		//mprintf(( "Error opening registry key '%s'\n", keyname ));
		goto Cleanup;
	}

	if ( !name )	 {
		//mprintf(( "No variable name passed\n" ));
		goto Cleanup;
	}
		
	lResult = RegSetValueEx( hKey,									// Handle to key
									 name,											// The values name
									 NULL,											// DWORD reserved
									 REG_SZ,											// null terminated string
									 (CONST BYTE *)value,						// value to set
									 strlen(value) );								// How many bytes to set
																			
	if ( lResult != ERROR_SUCCESS )	{
		//mprintf(( "Error writing registry key '%s'\n", name ));
		goto Cleanup;
	}


Cleanup:
	if ( hKey )
		RegCloseKey(hKey);
}

// same as previous function except we don't use the application name to build up the keyname
void os_config_write_string2( char *section, char *name, char *value )
{
	HKEY hKey = NULL;
	DWORD dwDisposition;
	char keyname[1024];
	LONG lResult;


	if ( section )	{
		sprintf( keyname, "Software\\%s\\%s", szCompanyName, section );
	} else {
		sprintf( keyname, "Software\\%s", szCompanyName );
	}

	lResult = RegCreateKeyEx( HKEY_LOCAL_MACHINE,						// Where to add it
											 keyname,								// name of key
											 NULL,									// DWORD reserved
											 "",										// Object class
											 REG_OPTION_NON_VOLATILE,			// Save to disk
											 KEY_ALL_ACCESS,						// Allows all changes
											 NULL,									// Default security attributes
											 &hKey,							// Location to store key
											 &dwDisposition );					// Location to store status of key

	if ( lResult != ERROR_SUCCESS )	{
		//mprintf(( "Error opening registry key '%s'\n", keyname ));
		goto Cleanup;
	}

	if ( !name )	 {
		//mprintf(( "No variable name passed\n" ));
		goto Cleanup;
	}
		
	lResult = RegSetValueEx( hKey,									// Handle to key
									 name,											// The values name
									 NULL,											// DWORD reserved
									 REG_SZ,											// null terminated string
									 (CONST BYTE *)value,						// value to set
									 strlen(value) );								// How many bytes to set
																			
	if ( lResult != ERROR_SUCCESS )	{
		//mprintf(( "Error writing registry key '%s'\n", name ));
		goto Cleanup;
	}


Cleanup:
	if ( hKey )
		RegCloseKey(hKey);
}

// Writes an unsigned int to the INI file.  
void os_config_write_uint( char *section, char *name, uint value )
{
	HKEY hKey = NULL;
	DWORD dwDisposition;
	char keyname[1024];
	LONG lResult;


	if ( section )	{
		sprintf( keyname, "Software\\%s\\%s\\%s", szCompanyName, szAppName, section );
	} else {
		sprintf( keyname, "Software\\%s\\%s", szCompanyName, szAppName );
	}

	lResult = RegCreateKeyEx( HKEY_LOCAL_MACHINE,						// Where to add it
											 keyname,								// name of key
											 NULL,									// DWORD reserved
											 "",										// Object class
											 REG_OPTION_NON_VOLATILE,			// Save to disk
											 KEY_ALL_ACCESS,						// Allows all changes
											 NULL,									// Default security attributes
											 &hKey,							// Location to store key
											 &dwDisposition );					// Location to store status of key

	if ( lResult != ERROR_SUCCESS )	{
		//mprintf(( "Error opening registry key '%s'\n", keyname ));
		goto Cleanup;
	}

	if ( !name )	 {
		//mprintf(( "No variable name passed\n" ));
		goto Cleanup;
	}
		
	lResult = RegSetValueEx( hKey,									// Handle to key
									 name,											// The values name
									 NULL,											// DWORD reserved
									 REG_DWORD,										// null terminated string
									 (CONST BYTE *)&value,						// value to set
									 4 );								// How many bytes to set
																				
	if ( lResult != ERROR_SUCCESS )	{
		//mprintf(( "Error writing registry key '%s'\n", name ));
		goto Cleanup;
	}

Cleanup:
	if ( hKey )
		RegCloseKey(hKey);

}




// Reads a string from the INI file.  If default is passed,
// and the string isn't found, returns ptr to default otherwise
// returns NULL;    Copy the return value somewhere before
// calling os_read_string again, because it might reuse the
// same buffer.

static char tmp_string_data[1024];

char * os_config_read_string( char *section, char *name, char *default_value )
{
	HKEY hKey = NULL;
	DWORD dwType, dwLen;
	char keyname[1024];
	LONG lResult;


	if ( section )	{
		sprintf( keyname, "Software\\%s\\%s\\%s", szCompanyName, szAppName, section );
	} else {
		sprintf( keyname, "Software\\%s\\%s", szCompanyName, szAppName );
	}

	lResult = RegOpenKeyEx( HKEY_LOCAL_MACHINE,							// Where it is
											 keyname,								// name of key
											 NULL,									// DWORD reserved
											 KEY_QUERY_VALUE,						// Allows all changes
											 &hKey );								// Location to store key

	if ( lResult != ERROR_SUCCESS )	{
		//mprintf(( "Error opening registry key '%s'\n", keyname ));
		goto Cleanup;
	}

	if ( !name )	 {
		//mprintf(( "No variable name passed\n" ));
		goto Cleanup;
	}

	dwLen = 1024;
	lResult = RegQueryValueEx( hKey,									// Handle to key
									 name,											// The values name
									 NULL,											// DWORD reserved
	                         &dwType,										// What kind it is
									 (ubyte *)&tmp_string_data,						// value to set
									 &dwLen );								// How many bytes to set
																				
	if ( lResult != ERROR_SUCCESS )	{
		//mprintf(( "Error reading registry key '%s'\n", name ));
		goto Cleanup;
	}

	default_value = tmp_string_data;

Cleanup:
	if ( hKey )
		RegCloseKey(hKey);

	return default_value;
}


static char tmp_string_data_ex[1024];

char * os_config_read_string_ex( char *keyname, char *name, char *default_value )
{
	HKEY hKey = NULL;
	DWORD dwType, dwLen;
	LONG lResult;

	lResult = RegOpenKeyEx( HKEY_LOCAL_MACHINE,							// Where it is
											 keyname,								// name of key
											 NULL,									// DWORD reserved
											 KEY_QUERY_VALUE,						// Allows all changes
											 &hKey );								// Location to store key

	if ( lResult != ERROR_SUCCESS )	{
		//mprintf(( "Error opening registry key '%s'\n", keyname ));
		goto Cleanup;
	}

	if ( !name )	 {
		//mprintf(( "No variable name passed\n" ));
		goto Cleanup;
	}

	dwLen = 1024;
	lResult = RegQueryValueEx( hKey,									// Handle to key
									 name,											// The values name
									 NULL,											// DWORD reserved
	                         &dwType,										// What kind it is
									 (ubyte *)&tmp_string_data_ex,						// value to set
									 &dwLen );								// How many bytes to set
																				
	if ( lResult != ERROR_SUCCESS )	{
		//mprintf(( "Error reading registry key '%s'\n", name ));
		goto Cleanup;
	}

	default_value = tmp_string_data_ex;

Cleanup:
	if ( hKey )
		RegCloseKey(hKey);

	return default_value;
}

char * os_config_read_string2( char *section, char *name, char *default_value )
{
	HKEY hKey = NULL;
	DWORD dwType, dwLen;
	char keyname[1024];
	LONG lResult;


	if ( section )	{
		sprintf( keyname, "Software\\%s\\%s", szCompanyName, section );
	} else {
		sprintf( keyname, "Software\\%s", szCompanyName );
	}

	lResult = RegOpenKeyEx( HKEY_LOCAL_MACHINE,							// Where it is
											 keyname,								// name of key
											 NULL,									// DWORD reserved
											 KEY_QUERY_VALUE,						// Allows all changes
											 &hKey );								// Location to store key

	if ( lResult != ERROR_SUCCESS )	{
		//mprintf(( "Error opening registry key '%s'\n", keyname ));
		goto Cleanup;
	}

	if ( !name )	 {
		//mprintf(( "No variable name passed\n" ));
		goto Cleanup;
	}

	dwLen = 1024;
	lResult = RegQueryValueEx( hKey,									// Handle to key
									 name,											// The values name
									 NULL,											// DWORD reserved
	                         &dwType,										// What kind it is
									 (ubyte *)&tmp_string_data,						// value to set
									 &dwLen );								// How many bytes to set
																				
	if ( lResult != ERROR_SUCCESS )	{
		//mprintf(( "Error reading registry key '%s'\n", name ));
		goto Cleanup;
	}

	default_value = tmp_string_data;

Cleanup:
	if ( hKey )
		RegCloseKey(hKey);

	return default_value;
}


// Reads a string from the INI file.  Default_value must 
// be passed, and if 'name' isn't found, then returns default_value
uint  os_config_read_uint( char *section, char *name, uint default_value )
{
	HKEY hKey = NULL;
	DWORD dwType, dwLen;
	char keyname[1024];
	LONG lResult;
	uint tmp_val;


	if ( section )	{
		sprintf( keyname, "Software\\%s\\%s\\%s", szCompanyName, szAppName, section );
	} else {
		sprintf( keyname, "Software\\%s\\%s", szCompanyName, szAppName );
	}

	lResult = RegOpenKeyEx( HKEY_LOCAL_MACHINE,							// Where it is
											 keyname,								// name of key
											 NULL,									// DWORD reserved
											 KEY_QUERY_VALUE,						// Allows all changes
											 &hKey );								// Location to store key

	if ( lResult != ERROR_SUCCESS )	{
		//mprintf(( "Error opening registry key '%s'\n", keyname ));
		goto Cleanup;
	}

	if ( !name )	 {
		//mprintf(( "No variable name passed\n" ));
		goto Cleanup;
	}

	dwLen = 4;
	lResult = RegQueryValueEx( hKey,									// Handle to key
									 name,											// The values name
									 NULL,											// DWORD reserved
	                         &dwType,										// What kind it is
									 (ubyte *)&tmp_val,						// value to set
									 &dwLen );								// How many bytes to set
																				
	if ( lResult != ERROR_SUCCESS )	{
		//mprintf(( "Error reading registry key '%s'\n", name ));
		goto Cleanup;
	}

	default_value = tmp_val;

Cleanup:
	if ( hKey )
		RegCloseKey(hKey);

	return default_value;
}
