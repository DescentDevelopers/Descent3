/*
 * $Logfile: /DescentIII/main/screens.h $
 * $Revision: 3 $
 * $Date: 4/07/99 12:31p $
 * $Author: Matt $
 *
 *
 *
 * $Log: /DescentIII/main/screens.h $
 *
 * 3     4/07/99 12:31p Matt
 * Added code for failed missions.
 *
 * 2     4/18/98 2:08a Samir
 * update.
 *
 * 1     4/14/98 8:19p Samir
 * initial revision.
 *
 * $NoKeywords: $
 */

#ifndef SCREENS_H
#define SCREENS_H

//	returns true if contine, returns false if player escaped out.
// Parameter:	success - true if the level was completed ok, false if failed
bool PostLevelResults(bool success);

#endif