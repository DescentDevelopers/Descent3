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
