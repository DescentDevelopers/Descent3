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