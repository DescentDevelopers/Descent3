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

 * $Logfile: /DescentIII/Main/lnxfix.h $
 * $Revision: 1.2 $
 * $Date: 2004/02/09 04:14:49 $
 * $Author: kevinb $
 *
 * File operations not covered properly in ANSI C
 *
 * $Log: lnxfix.h,v $
 * Revision 1.2  2004/02/09 04:14:49  kevinb
 * Added newlines to all headers to reduce number of warnings printed
 *
 * Made some small changes to get everything compiling.
 *
 * All Ready to merge the 1.5 tree.
 *
 * Revision 1.1.1.1  2000/04/18 00:00:32  icculus
 * initial checkin
 *
 *
 * 2     4/15/99 1:39a Jeff
 * changes for linux compile
 *
 * 1     4/14/99 7:51p Jeff
 *
 *
 * $NoKeywords: $
 */

#ifndef LINUX_FIX_H_
#define LINUX_FIX_H_
#if defined(__LINUX__)

#define stricmp(x, y) strcasecmp(x, y)
#define _vsnprintf(a, b, c, d) vsnprintf(a, b, c, d)

extern bool Dedicated_server;
#endif
#endif
