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
 * $Source: /home/kevin/cvsstuff/descent3/descent3/Main/FontEditor/Args.h,v $
 * $Revision: 1.1.1.1 $
 * $Author: kevinb $
 * $Date: 2003-08-26 03:57:45 $
 *
 * Args header transferred from D3.
 *
 * $Log: not supported by cvs2svn $
 * 
 * 2     5/10/99 12:24p Samir
 * added comment header
 * 
 */

#ifndef ARGS_H
#define ARGS_H

#define MAX_ARGS			15
#define MAX_CHARS_PER_ARG	100

extern char GameArgs[MAX_ARGS][MAX_CHARS_PER_ARG];

// Gathers all arguments
void GatherArgs (char *str);

// Returns index of argument sought, or 0 if not found
int FindArg (char *which);
#endif