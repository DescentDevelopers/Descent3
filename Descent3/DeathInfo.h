/*
* Descent 3 
* Copyright (C) 2024 Parallax Software
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.

--- HISTORICAL COMMENTS FOLLOW ---

 * $Logfile: /DescentIII/Main/DeathInfo.h $
 * $Revision: 6 $
 * $Date: 4/02/99 2:46p $
 * $Author: Matt $
 *
 * Info for object deaths
 *
 * $Log: /DescentIII/Main/DeathInfo.h $
 *
 * 6     4/02/99 2:46p Matt
 * Moved flags from deathinfo to deathinfo_external, because the arhive
 * builder only copies the latter into the archive.
 *
 * 5     4/02/99 11:23a Matt
 * Made KillObject not take a death_info struct, but rather the death info
 * as individual parameters.  Moved death_info into objinfo.h, since it's
 * only used for generic objects.  Took out fade-away death hack, now that
 * fade-away deaths can be explicitely set.
 *
 * 4     2/28/99 11:30p Chris
 * FindObjOfType and OSIRIS controllable deaths
 *
 * 3     2/25/99 11:01a Matt
 * Added new explosion system.
 *
 * 2     1/14/99 8:21p Matt
 * Added dialog for defining object deaths
 *
 * 1     1/14/99 8:13p Matt
 *
 */

#ifndef _DEATHINFO_H
#define _DEATHINFO_H

// Get the death delay type
#define DEATH_DELAY(f) (f & DF_DELAY_MASK)

// Get the explosion size
#define DEATH_EXPL_SIZE(f) (f & DF_EXPL_SIZE_MASK)

// Include the actual flags
#include "deathinfo_external.h"

#endif
