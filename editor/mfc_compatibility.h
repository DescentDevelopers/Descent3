/*
* Descent 3 
* Copyright (C) 2024 Descent Developers
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

/**
 * MFC is not 64bit compatible, so we have to re-define some macros,
 * changing pointer types that MFC expects to be 32bit to actual 32bit types 
 */

#include "stdafx.h"
#ifdef ON_WM_TIMER
#undef ON_WM_TIMER
#define ON_WM_TIMER() \
    { WM_TIMER, 0, 0, 0, AfxSig_v_up_v, \
        (AFX_PMSG)(AFX_PMSGW) \
        (static_cast< void (AFX_MSG_CALL CWnd::*)(UINT) > ( &ThisClass :: OnTimer)) },
#endif

#ifdef ON_REGISTERED_MESSAGE
#undef ON_REGISTERED_MESSAGE
#define ON_REGISTERED_MESSAGE(nMessageVariable, memberFxn) \
	{ 0xC000, 0, 0, 0, (UINT_PTR)(UINT*)(&nMessageVariable), \
		/*implied 'AfxSig_lwl'*/ \
		(AFX_PMSG)(AFX_PMSGW) \
		(static_cast< LONG (AFX_MSG_CALL CWnd::*)(WPARAM, LPARAM) > \
		(memberFxn)) },
#endif
