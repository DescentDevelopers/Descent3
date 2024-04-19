/*
 * $Source: /home/kevin/cvsstuff/descent3/descent3/Main/FontEditor/args.cpp,v $
 * $Revision: 1.1.1.1 $
 * $Author: kevinb $
 * $Date: 2003-08-26 03:57:45 $
 *
 * Transferred from Descent 3
 *
 * $Log: not supported by cvs2svn $
 * 
 * 2     4/17/99 4:05p Samir
 * complete font editor.
 * 
 */

#include "args.h"
#include "mono.h"
#include <string.h>

static int TotalArgs=0;
char GameArgs[MAX_ARGS][MAX_CHARS_PER_ARG];

// Gathers all arguments
void GatherArgs (char *str)
{
	int i,t,curarg=1;
	int len=strlen(str);
	bool gotquote = false;

	for (t=0,i=0;i<len;i++)
	{
		if(t==0)
		{
			if(str[i]=='\"')
			{
				gotquote = true;
				continue;
			}
		}
		if(gotquote)
		{
			if (str[i]!='\"' && str[i]!=0)
				GameArgs[curarg][t++]=str[i];
			else
			{
				GameArgs[curarg][t]=0;
				t=0;
				mprintf ((0,"Arg %d is %s\n",curarg,GameArgs[curarg]));
				gotquote = false;
				curarg++;
			}
		}
		else
		{
			if (str[i]!=' ' && str[i]!=0)
				GameArgs[curarg][t++]=str[i];
			else
			{
				GameArgs[curarg][t]=0;
				t=0;
				mprintf ((0,"Arg %d is %s\n",curarg,GameArgs[curarg]));
				curarg++;
			}
		}
	}

	GameArgs[curarg][t]=0;
	curarg++;
	TotalArgs=curarg;

// clear out argument list.
	for(;curarg < MAX_ARGS; curarg++)
		GameArgs[curarg][0] = 0;
}
 
// Returns index of argument sought, or 0 if not found
int FindArg (char *which)
 {
  int i;
  
  for (i=1;i<=TotalArgs;i++)
   if (stricmp (which,GameArgs[i])==0)
     return (i);

  return (0);
 }
