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
