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