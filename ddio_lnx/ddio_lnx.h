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
float ddio_TickToSeconds(unsigned long ticks);

void ddio_KeyHandler(MSG *msg);
void ddio_MouseHandler(MSG *msg);

#define DDIO_MESSAGE(_m) ddio_DebugMessage _m

#define MAKE_DDIO_TIME(_ms) ddio_TickToSeconds(_ms)

*/
#endif
