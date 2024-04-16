/*
 * $Logfile: /DescentIII/main/TelComGoals.h $
 * $Revision: 2 $
 * $Date: 4/14/99 3:57a $
 * $Author: Jeff $
 *
 * TelCom Goal Status screen
 *
 * $Log: /DescentIII/main/TelComGoals.h $
 *
 * 2     4/14/99 3:57a Jeff
 * fixed case mismatch in #includes
 *
 * 1     2/20/99 4:31p Jeff
 *
 * $NoKeywords: $
 */

#ifndef __TCGOALS_H_
#define __TCGOALS_H_
#include "TelCom.h"

// This is the function called by TelCom
//  return true if TelCom should exit to TelCom Main Menu
//  return false if you should exit out of TelCom completly
bool TelComGoalStatus(tTelComInfo *tcs);

#endif