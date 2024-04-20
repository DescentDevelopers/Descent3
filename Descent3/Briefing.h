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
*/

/*
 * $Logfile: /DescentIII/Main/Briefing.h $
 * $Revision: 11 $
 * $Date: 4/14/99 3:56a $
 * $Author: Jeff $
 *
 * Header for briefing system
 *
 * $Log: /DescentIII/Main/Briefing.h $
 *
 * 11    4/14/99 3:56a Jeff
 * fixed case mismatch in #includes
 *
 * 10    8/27/98 2:51p Jeff
 * New TelCom finally checked in
 *
 * 9     4/26/98 2:53a Jeff
 * Made changes needed for new Effect driver system
 *
 * 8     4/22/98 7:44p Jeff
 * added flashing buttons
 *
 * 7     4/15/98 6:28p Jeff
 * Got parsing working for most of the keywords
 *
 * 6     1/20/98 6:04p Jeff
 * Added support for fading fonts
 *
 * 5     1/20/98 12:08p Jeff
 *
 * 4     1/19/98 5:37p Jeff
 * Got briefing up to par, and even better than before, added timer so
 * scroll and type fonts are time based.
 *
 * 3     1/16/98 2:40p Jeff
 * Adjusted so everything is displayed in correct spots, took out
 * DefaultControl timer, added support for non fading bitmaps
 *
 * 2     11/17/97 3:56p Matt
 * Added the briefing system code
 *
 * 1     11/12/97 10:44a Matt
 *
 * $NoKeywords: $
 */

#ifndef __BRIEFING_H_
#define __BRIEFING_H_

#include "TelCom.h"

bool ParseBriefing(char *filename, tTelComInfo *tcs);

#endif