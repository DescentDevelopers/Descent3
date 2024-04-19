/*
 * $Logfile: /DescentIII/Main/D3Launch/OS_Config.h $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:56:51 $
 * $Author: kevinb $
 *
 * 
 *
 * $Log: not supported by cvs2svn $
 * 
 * 4     9/09/98 12:24p Nate
 * Added os_config_read_string_ex() prototype
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

// Header for System Registry functions

#ifndef _OS_CONFIG_H
#define _OS_CONFIG_H

#include "PsTypes.h"

void os_config_write_string( char *section, char *name, char *value );
char *os_config_read_string( char *section, char *name, char *default_value );
uint  os_config_read_uint( char *section, char *name, uint default_value );
void os_config_write_uint( char *section, char *name, uint value );
char *os_config_read_string_ex( char *keyname, char *name, char *default_value );

extern char *szSectionName;

#endif //_OS_CONFIG_H