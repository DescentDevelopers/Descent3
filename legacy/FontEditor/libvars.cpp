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
 * $Source: /home/kevin/cvsstuff/descent3/descent3/Main/FontEditor/libvars.cpp,v $
 * $Revision: 1.1.1.1 $
 * $Author: kevinb $
 * $Date: 2003-08-26 03:57:45 $
 *
 * variables accessed by libraries into main execute file.
 *
 * $Log: not supported by cvs2svn $
 * 
 * 2     4/17/99 4:05p Samir
 * complete font editor.
 * 
 */

#include "appdatabase.h"

int paged_in_num=0;
int paged_in_count=0;
class oeAppDatabase *Database=NULL;
bool Force_one_texture = false;