/*
* $Logfile: /DescentIII/Main/misc/psrand.cpp $
* $Revision: 2 $
* $Date: 4/21/99 11:05a $
* $Author: Kevin $
*
* Outrage random number generator code
*
* $Log: /DescentIII/Main/misc/psrand.cpp $
 * 
 * 2     4/21/99 11:05a Kevin
 * new ps_rand and ps_srand to replace rand & srand
 * 
 * 1     4/21/99 10:16a Kevin
*
* $NoKeywords: $
*/

#include "psrand.h"

static long ps_holdrand = 1L;

//These are adapted from the C runtime lib. Pretty simple.

void ps_srand(unsigned int seed)
{
	ps_holdrand = (long)seed;
}

int ps_rand(void)
{
	return(((ps_holdrand = ps_holdrand * 214013L + 2531011L) >> 16) & 0x7fff);
}
