/*
 * $Logfile: /DescentIII/main/soar.cpp $
 * $Revision: 4 $
 * $Date: 4/16/99 12:33a $
 * $Author: Matt $
 *
 *
 *
 * $Log: /DescentIII/main/soar.cpp $
 *
 * 4     4/16/99 12:33a Matt
 * Disable Soar on non-Windows systems.
 *
 */

#include "soar.h"

#ifdef SOAR_ENABLED

bool Soar_active = false;

#endif // ifdef SOAR_ENABLED
