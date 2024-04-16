/*
* $Logfile: /DescentIII/Main/lib/psrand.h $
* $Revision: 2 $
* $Date: 4/21/99 11:05a $
* $Author: Kevin $
*
* Outrage random number generator code
*
* $Log: /DescentIII/Main/lib/psrand.h $
 * 
 * 2     4/21/99 11:05a Kevin
 * new ps_rand and ps_srand to replace rand & srand
 * 
 * 1     4/21/99 10:16a Kevin
*
* $NoKeywords: $
*/

#undef RAND_MAX

#define RAND_MAX	0x7fff

void ps_srand(unsigned int seed);

int ps_rand(void);