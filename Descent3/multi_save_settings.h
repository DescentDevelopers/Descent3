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
 * $Logfile: /DescentIII/main/lib/multi_save_settings.h $
 * $Revision: 3 $
 * $Date: 9/23/98 6:33p $
 * $Author: Kevin $
 *
 * Multiplayer settings save/load
 *
 * $Log: /DescentIII/main/lib/multi_save_settings.h $
 *
 * 3     9/23/98 6:33p Kevin
 * Fixed load settings
 *
 * 2     9/23/98 2:55p Kevin
 * Saved multi config and changed UI to conform
 *
 * 1     9/23/98 10:42a Kevin
 *
 *
 * Initial Version
 *
 * $NoKeywords: $
 */

#ifndef _MULTI_SAVE_SETTINGS_HEADER
#define _MULTI_SAVE_SETTINGS_HEADER

#define MAX_MPS_LINE_LEN 200

int MultiSaveSettings(char *filename);
int MultiLoadSettings(char *filename);

#endif