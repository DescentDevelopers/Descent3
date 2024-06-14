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

* $Logfile: /DescentIII/Main/Editor/DallasUtilities.cpp $
* $Revision: 1.1.1.1 $
* $Date: 2003-08-26 03:57:37 $
* $Author: kevinb $
*
* Code for DALLAS related utility functions
*
* $Log: not supported by cvs2svn $
 * 
 * 6     6/09/99 7:06p Jeff
 * stub functions added.  #ifdef NEWEDITORs added, files changed, to get
 * Dallas integrated into new editor
 * 
 * 5     2/19/99 5:35p Nate
 * Added new types and events
 * 
 * 4     12/16/98 8:45p Nate
 * Added loading of Actions
 * 
 * 3     12/15/98 7:48p Nate
 * Fixed minor buffering problem
 * 
 * 2     12/14/98 12:45p Nate
 * Initial Version
*
* $NoKeywords: $
*/

#include "stdafx.h"

#include <string.h>
#include <ctype.h>

#include "pserror.h"
#include "cfile.h"
#include "mem.h"
#include "mono.h"
#include "psglob.h"
#include "manage.h"
#include "gamefile.h"

#include "DallasUtilities.h"


// Removes any whitespace padding from the end of a string
void RemoveTrailingWhitespace(char *s)
{
	int last_char_pos;

	last_char_pos=strlen(s)-1;
	while(last_char_pos>=0 && isspace(s[last_char_pos])) {
		s[last_char_pos]='\0';
		last_char_pos--;
	}
}

// Returns a pointer to the first non-whitespace char in given string
char *SkipInitialWhitespace(char *s)
{
	while((*s)!='\0' && isspace(*s)) 
		s++;

	return(s);
}


class tFindInManage
{
public:
	tFindInManage() {searching = false;}

	bool searching;
	bool in_Gamefiles;
	int curr_index;
	char glob_string[PAGENAME_LEN];
};
tFindInManage FindInManageData;



bool FindManageFirst(char *buffer,char *wildcard)
{
#ifndef NEWEDITOR
	if(FindInManageData.searching)
		FindManageClose();

	FindInManageData.searching = true;
	strcpy(FindInManageData.glob_string,wildcard);
	FindInManageData.curr_index = 0;
	FindInManageData.in_Gamefiles = true;

	return FindManageNext(buffer);
#else
	return false;
#endif
}

bool FindManageNext(char *buffer)
{
#ifndef NEWEDITOR
	if(!FindInManageData.searching)
		return false;

	if(FindInManageData.in_Gamefiles)
	{

		for(;FindInManageData.curr_index<MAX_GAMEFILES;FindInManageData.curr_index++)
		{
			if(Gamefiles[FindInManageData.curr_index].used && 
				PSGlobMatch(FindInManageData.glob_string,Gamefiles[FindInManageData.curr_index].name,false,false))
			{
				//match
				strcpy(buffer,Gamefiles[FindInManageData.curr_index].name);
				FindInManageData.curr_index++;
				return true;
			}
		}

		FindInManageData.curr_index = 0;
		FindInManageData.in_Gamefiles = false;
	}

	for(;FindInManageData.curr_index<MAX_TRACKLOCKS;FindInManageData.curr_index++)
	{
		if(GlobalTrackLocks[FindInManageData.curr_index].used && GlobalTrackLocks[FindInManageData.curr_index].pagetype==PAGETYPE_GAMEFILE && 
			PSGlobMatch(FindInManageData.glob_string,GlobalTrackLocks[FindInManageData.curr_index].name,false,false))
		{
			//match
			strcpy(buffer,GlobalTrackLocks[FindInManageData.curr_index].name);
			FindInManageData.curr_index++;
			return true;
		}
	}

	return false;
#else
	return false;
#endif
}

void FindManageClose(void)
{
#ifndef NEWEDITOR
	FindInManageData.searching = false;
#endif
}

bool GamefileExists(char *name)
{
	bool found;
	char buffer[PAGENAME_LEN+1];

	found=FindManageFirst(buffer,name);
	FindManageClose();

	return(found);
}

