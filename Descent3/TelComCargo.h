/*
 * $Logfile: /DescentIII/main/TelComCargo.h $
 * $Revision: 4 $
 * $Date: 4/14/99 3:57a $
 * $Author: Jeff $
 *
 * TelCom Cargo Header
 *
 * $Log: /DescentIII/main/TelComCargo.h $
 *
 * 4     4/14/99 3:57a Jeff
 * fixed case mismatch in #includes
 *
 * 3     8/27/98 2:51p Jeff
 * New TelCom finally checked in
 *
 * 2     7/08/98 8:06p Jeff
 * Initial creation of TelComCargo
 *
 * $NoKeywords: $
 */

#ifndef __TCCARGO_H_
#define __TCCARGO_H_
#include "TelCom.h"

// This is the function called by TelCom
//  return true if TelCom should exit to TelCom Main Menu
//  return false if you should exit out of TelCom completly
bool TelComCargo(tTelComInfo *tcs);

#endif