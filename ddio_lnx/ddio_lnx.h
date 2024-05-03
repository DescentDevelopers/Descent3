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
 *
 * $NoKeywords: $
 */

#ifndef DDIO_LNX_H
#define DDIO_LNX_H

#include "pstypes.h"

class oeLnxApplication;
extern oeLnxApplication *Lnx_app_obj;

/*
typedef struct dinput_data {
        oeWin32Application *app;
        HWND hwnd;
        LPDIRECTINPUT lpdi;

        LPDIRECTINPUTDEVICE lpkeydev;
        HHOOK hkeyhook;
        BOOL key_acquired;
        bool preemptive;

        LPDIRECTINPUTDEVICE lpmsedev;
        HHOOK hmsehook;
        BOOL mse_acquired;
        int num_mse_buttons;
        int num_mse_axes;
} dinput_data;

extern dinput_data DInputData;
extern bool DDIO_init;
extern bool DDIO_preemptive;


//	Internal functions
//		thread handlers
//			-keyboard

bool ddio_JoyHandler();
void ddio_DebugMessage(unsigned err, char *fmt, ...);
float ddio_TickToSeconds(unsigned int ticks);

void ddio_KeyHandler(MSG *msg);
void ddio_MouseHandler(MSG *msg);

#define DDIO_MESSAGE(_m) ddio_DebugMessage _m

#define MAKE_DDIO_TIME(_ms) ddio_TickToSeconds(_ms)

*/
#endif
