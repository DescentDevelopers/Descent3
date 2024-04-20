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

/////////////////////////////////////////////////////////////////////////////
//
// VerifyA3D.h : header file for Aureal A3D verification handling.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __VERIFYA3D_H
#define __VERIFYA3D_H

#include <windows.h>

#ifdef __cplusplus
extern "C"
{
#endif

// Public verification function.
// Return TRUE if Aureal, FALSE if clone, -1 if check could not succeed.
extern int WINAPI VerifyAurealA3D(void);

#ifdef __cplusplus
}
#endif

#endif


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
