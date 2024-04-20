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
 * $Logfile: $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:57:38 $
 * $Author: kevinb $
 *
 * Header for read3ds.cpp
 *
 * $Log: not supported by cvs2svn $
 *
 * $NoKeywords: $
 */

#ifndef READ3DS_H
#define READ3DS_H


#include "cfile.h"
#include "room.h"

// Opens and reads a 3dsmax file for our rooms.  Reads this file into the passed room 
// structure
int Read3DSMaxFile(char *filename);

// Converts the 3dsmax coordinate space into our left-handed coordinate space
void ConvertHandiness( vector * v );

// Parses a chunk of a 3dsmax file - this function calls itself
void Parse3DSMaxChunk (CFILE *fp, int size);

#endif
