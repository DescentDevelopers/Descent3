/*
* $Logfile: /DescentIII/Main/ddio_lnx/lnxkey_sdl.cpp $
* $Revision: 1.6 $
* $Date: 2001/02/07 09:18:02 $
* $Author: icculus $
*
* Linux Xwindows routines
*
* $Log: lnxkey_sdl.cpp,v $
* Revision 1.6  2001/02/07 09:18:02  icculus
* Fullscreen/windowed toggle keybinding attempts to toggle regardless of
* renderer, now. Otherwise, 2D surfaces (movies) wouldn't toggle. Glide uses
* can just risk it.
*
* Revision 1.5  2001/01/11 11:40:53  icculus
* Added CTRL-G and ALT-Enter keybindings.
*
* Revision 1.4  2000/09/22 19:01:49  icculus
* SDLK_WORLD* support.
*
* Revision 1.3  2000/06/29 09:52:45  hercules
* Don't poll for events twice per frame (already done in mouse driver)
*
* Revision 1.2  2000/06/24 01:15:15  icculus
* patched to compile.
*
* Revision 1.1  2000/04/27 11:16:01  icculus
* First (and hopefully last) commit.
*
* Revision 1.1.1.1  2000/04/18 00:00:33  icculus
* initial checkin
*
 * 
 * 6     8/19/99 3:45p Jeff
 * commented out mprintfs
 * 
 * 5     8/19/99 1:32p Jeff
 * fixed o key. Added scroll lock, caps lock and print screen keys.
 * 
 * 4     7/15/99 11:37a Jeff
 * moved autorepeat calls to lnxapp.cpp
 * 
 * 3     7/14/99 9:06p Jeff
 * added comment header
*
* $NoKeywords: $
*/

#include "SDL.h"
#include "pserror.h"
#include "mono.h"
#include "ddio.h"
#include "ddio_lnx.h"
#include "application.h"

#ifdef __PERMIT_GL_LOGGING
void DGL_EnableLogging( int enable );
extern bool __glLog;
#endif

extern bool ddio_mouseGrabbed;

extern volatile struct tLnxKeys
{
	union{
		int up_ticks;
		float up_time;
	};
	union{
		int down_ticks;
		float down_time;
	};
	bool status;
}LKeys[DDIO_MAX_KEYS];


int sdlkey_to_ddiocode[27] = { 0, KEY_A, KEY_B, KEY_C, KEY_D, KEY_E, KEY_F, KEY_G, KEY_H, KEY_I, KEY_J, KEY_K, KEY_L, KEY_M, KEY_N,
			KEY_O, KEY_P, KEY_Q, KEY_R, KEY_S, KEY_T, KEY_U, KEY_V, KEY_W, KEY_X, KEY_Y, KEY_Z
};


inline ubyte sdlkeycode_to_keycode(unsigned int sdlkeycode)
{
// unceremoniously taken from Heretic source code with a few modifications.
//  (by Outrage. Not Loki. We know better.  :)  --ryan.)
    int rc = sdlkeycode;

	switch (rc)
	{
		case SDLK_DELETE:		rc = KEY_DELETE;        break;
		case SDLK_INSERT:		rc = KEY_INSERT;        break;
      	case SDLK_PAGEUP:	    rc = KEY_PAGEUP;        break;
      	case SDLK_PAGEDOWN:	    rc = KEY_PAGEDOWN;      break;
      	case SDLK_HOME:		    rc = KEY_HOME;          break;
      	case SDLK_END:		    rc = KEY_END;           break;
      	case SDLK_LEFT:		    rc = KEY_LEFT;			break;
      	case SDLK_RIGHT:		rc = KEY_RIGHT;			break;
      	case SDLK_DOWN:		    rc = KEY_DOWN;			break;
      	case SDLK_UP:			rc = KEY_UP;			break;
      	case SDLK_ESCAPE:		rc = KEY_ESC;			break;
      	case SDLK_RETURN:		rc = KEY_ENTER;			break;
      	case SDLK_TAB:		    rc = KEY_TAB;			break;
      	case SDLK_F1:			rc = KEY_F1;			break;
      	case SDLK_F2:			rc = KEY_F2;			break;
      	case SDLK_F3:			rc = KEY_F3;			break;
      	case SDLK_F4:			rc = KEY_F4;			break;
      	case SDLK_F5:			rc = KEY_F5;			break;
      	case SDLK_F6:			rc = KEY_F6;			break;
      	case SDLK_F7:			rc = KEY_F7;			break;
      	case SDLK_F8:			rc = KEY_F8;			break;
      	case SDLK_F9:			rc = KEY_F9;			break;
      	case SDLK_F10:		    rc = KEY_F10;			break;
      	case SDLK_F11:		    rc = KEY_F11;			break;
      	case SDLK_F12:		    rc = KEY_F12;			break;
      	case SDLK_BACKSPACE:	rc = KEY_BACKSP;	    break;
      	case SDLK_PAUSE:		rc = KEY_PAUSE;			break;
      	case SDLK_KP_PLUS:		rc = KEY_PADPLUS;		break;
       	case SDLK_KP_MINUS:     rc = KEY_PADMINUS;		break;
       	case SDLK_KP_ENTER:	    rc = KEY_PADENTER;		break;
       	case SDLK_KP_PERIOD:	rc = KEY_PADPERIOD;		break;
       	case SDLK_KP_DIVIDE:	rc = KEY_PADDIVIDE;		break;
       	case SDLK_KP_MULTIPLY:  rc = KEY_PADMULTIPLY;	break;
       	case SDLK_KP0: 		    rc = KEY_PAD0;			break;
       	case SDLK_KP1: 	    	rc = KEY_PAD1;			break;
       	case SDLK_KP2:   		rc = KEY_PAD2;			break;
       	case SDLK_KP3: 		    rc = KEY_PAD3;			break;
       	case SDLK_KP4: 	    	rc = KEY_PAD4;			break;
       	case SDLK_KP5:   		rc = KEY_PAD5;			break;
       	case SDLK_KP6: 		    rc = KEY_PAD6;			break;
       	case SDLK_KP7: 	    	rc = KEY_PAD7;			break;
       	case SDLK_KP8:  		rc = KEY_PAD8;			break;
       	case SDLK_KP9: 		    rc = KEY_PAD9;			break;
       	case SDLK_SPACE:		rc = KEY_SPACEBAR;	    break;
       	case SDLK_1:			rc = KEY_1; 			break;
       	case SDLK_2:			rc = KEY_2; 			break;
       	case SDLK_3:			rc = KEY_3; 			break;
       	case SDLK_4:			rc = KEY_4; 			break;
       	case SDLK_5:			rc = KEY_5; 			break;
       	case SDLK_6:			rc = KEY_6; 			break;
       	case SDLK_7:			rc = KEY_7; 			break;
       	case SDLK_8:			rc = KEY_8; 			break;
       	case SDLK_9:			rc = KEY_9; 			break;
       	case SDLK_0:			rc = KEY_0; 			break;

// modifier keys
		case SDLK_LSHIFT:	    rc = KEY_LSHIFT;		break;
		case SDLK_RSHIFT:   	rc = KEY_RSHIFT;		break;
		case SDLK_LCTRL:	    rc = KEY_LCTRL;			break;
		case SDLK_RCTRL:	    rc = KEY_RCTRL;			break;
      	case SDLK_LALT:
      	case SDLK_LMETA:	    rc = KEY_LALT;			break;
      	case SDLK_RALT:
      	case SDLK_RMETA:   	    rc = KEY_RALT;			break;
       	
// since we look at unmodified keycodes, we don't have to worry about shifted states, I think.
		case SDLK_EQUALS:		rc = KEY_EQUAL;			break;
       	case SDLK_MINUS:		rc = KEY_MINUS;			break;
       	case SDLK_LEFTBRACKET:	rc = KEY_LBRACKET;	    break;
       	case SDLK_RIGHTBRACKET:	rc = KEY_RBRACKET;	    break;
       	case SDLK_BACKSLASH:	rc = KEY_BACKSLASH;		break;
       	case SDLK_BACKQUOTE:	rc = KEY_LAPOSTRO;		break;
       	case SDLK_QUOTE:        rc = KEY_RAPOSTRO;		break;
       	case SDLK_SEMICOLON:	rc = KEY_SEMICOL;		break;
       	case SDLK_PERIOD:		rc = KEY_PERIOD;		break;
       	case SDLK_SLASH:		rc = KEY_SLASH;			break;
       	case SDLK_COMMA:		rc = KEY_COMMA;			break;
		case SDLK_NUMLOCK:	    rc = KEY_NUMLOCK;		break;
		case SDLK_PRINT:		rc = KEY_PRINT_SCREEN;	break;
		case SDLK_SCROLLOCK:    rc = KEY_SCROLLOCK;	    break;
		case SDLK_CAPSLOCK:     rc = KEY_CAPSLOCK;		break;

// world keys. Ugh.
        case SDLK_WORLD_0:      rc = KEY_WORLD0;        break;
        case SDLK_WORLD_1:      rc = KEY_WORLD1;        break;
        case SDLK_WORLD_2:      rc = KEY_WORLD2;        break;
        case SDLK_WORLD_3:      rc = KEY_WORLD3;        break;
        case SDLK_WORLD_4:      rc = KEY_WORLD4;        break;
        case SDLK_WORLD_5:      rc = KEY_WORLD5;        break;
        case SDLK_WORLD_6:      rc = KEY_WORLD6;        break;
        case SDLK_WORLD_7:      rc = KEY_WORLD7;        break;
        case SDLK_WORLD_8:      rc = KEY_WORLD8;        break;
        case SDLK_WORLD_9:      rc = KEY_WORLD9;        break;
        case SDLK_WORLD_10:     rc = KEY_WORLD10;       break;
        case SDLK_WORLD_11:     rc = KEY_WORLD11;       break;
        case SDLK_WORLD_12:     rc = KEY_WORLD12;       break;
        case SDLK_WORLD_13:     rc = KEY_WORLD13;       break;
        case SDLK_WORLD_14:     rc = KEY_WORLD14;       break;
        case SDLK_WORLD_15:     rc = KEY_WORLD15;       break;
        case SDLK_WORLD_16:     rc = KEY_WORLD16;       break;
        case SDLK_WORLD_17:     rc = KEY_WORLD17;       break;
        case SDLK_WORLD_18:     rc = KEY_WORLD18;       break;
        case SDLK_WORLD_19:     rc = KEY_WORLD19;       break;
        case SDLK_WORLD_20:     rc = KEY_WORLD20;       break;
        case SDLK_WORLD_21:     rc = KEY_WORLD21;       break;
        case SDLK_WORLD_22:     rc = KEY_WORLD22;       break;
        case SDLK_WORLD_23:     rc = KEY_WORLD23;       break;
        case SDLK_WORLD_24:     rc = KEY_WORLD24;       break;
        case SDLK_WORLD_25:     rc = KEY_WORLD25;       break;
        case SDLK_WORLD_26:     rc = KEY_WORLD26;       break;
        case SDLK_WORLD_27:     rc = KEY_WORLD27;       break;
        case SDLK_WORLD_28:     rc = KEY_WORLD28;       break;
        case SDLK_WORLD_29:     rc = KEY_WORLD29;       break;
        case SDLK_WORLD_30:     rc = KEY_WORLD30;       break;
        case SDLK_WORLD_31:     rc = KEY_WORLD31;       break;
        case SDLK_WORLD_32:     rc = KEY_WORLD32;       break;
        case SDLK_WORLD_33:     rc = KEY_WORLD33;       break;
        case SDLK_WORLD_34:     rc = KEY_WORLD34;       break;
        case SDLK_WORLD_35:     rc = KEY_WORLD35;       break;
        case SDLK_WORLD_36:     rc = KEY_WORLD36;       break;
        case SDLK_WORLD_37:     rc = KEY_WORLD37;       break;
        case SDLK_WORLD_38:     rc = KEY_WORLD38;       break;
        case SDLK_WORLD_39:     rc = KEY_WORLD39;       break;
        case SDLK_WORLD_40:     rc = KEY_WORLD40;       break;
        case SDLK_WORLD_41:     rc = KEY_WORLD41;       break;
        case SDLK_WORLD_42:     rc = KEY_WORLD42;       break;
        case SDLK_WORLD_43:     rc = KEY_WORLD43;       break;
        case SDLK_WORLD_44:     rc = KEY_WORLD44;       break;
        case SDLK_WORLD_45:     rc = KEY_WORLD45;       break;
        case SDLK_WORLD_46:     rc = KEY_WORLD46;       break;
        case SDLK_WORLD_47:     rc = KEY_WORLD47;       break;
        case SDLK_WORLD_48:     rc = KEY_WORLD48;       break;
        case SDLK_WORLD_49:     rc = KEY_WORLD49;       break;
        case SDLK_WORLD_50:     rc = KEY_WORLD50;       break;
        case SDLK_WORLD_51:     rc = KEY_WORLD51;       break;
        case SDLK_WORLD_52:     rc = KEY_WORLD52;       break;
        case SDLK_WORLD_53:     rc = KEY_WORLD53;       break;
        case SDLK_WORLD_54:     rc = KEY_WORLD54;       break;
        case SDLK_WORLD_55:     rc = KEY_WORLD55;       break;
        case SDLK_WORLD_56:     rc = KEY_WORLD56;       break;
        case SDLK_WORLD_57:     rc = KEY_WORLD57;       break;
        case SDLK_WORLD_58:     rc = KEY_WORLD58;       break;
        case SDLK_WORLD_59:     rc = KEY_WORLD59;       break;
        case SDLK_WORLD_60:     rc = KEY_WORLD60;       break;
        case SDLK_WORLD_61:     rc = KEY_WORLD61;       break;
        case SDLK_WORLD_62:     rc = KEY_WORLD62;       break;
        case SDLK_WORLD_63:     rc = KEY_WORLD63;       break;
        case SDLK_WORLD_64:     rc = KEY_WORLD64;       break;
        case SDLK_WORLD_65:     rc = KEY_WORLD65;       break;
        case SDLK_WORLD_66:     rc = KEY_WORLD66;       break;
        case SDLK_WORLD_67:     rc = KEY_WORLD67;       break;
        case SDLK_WORLD_68:     rc = KEY_WORLD68;       break;
        case SDLK_WORLD_69:     rc = KEY_WORLD69;       break;
        case SDLK_WORLD_70:     rc = KEY_WORLD70;       break;
        case SDLK_WORLD_71:     rc = KEY_WORLD71;       break;
        case SDLK_WORLD_72:     rc = KEY_WORLD72;       break;
        case SDLK_WORLD_73:     rc = KEY_WORLD73;       break;
        case SDLK_WORLD_74:     rc = KEY_WORLD74;       break;
        case SDLK_WORLD_75:     rc = KEY_WORLD75;       break;
        case SDLK_WORLD_76:     rc = KEY_WORLD76;       break;
        case SDLK_WORLD_77:     rc = KEY_WORLD77;       break;
        case SDLK_WORLD_78:     rc = KEY_WORLD78;       break;
        case SDLK_WORLD_79:     rc = KEY_WORLD79;       break;
        case SDLK_WORLD_80:     rc = KEY_WORLD80;       break;
        case SDLK_WORLD_81:     rc = KEY_WORLD81;       break;
        case SDLK_WORLD_82:     rc = KEY_WORLD82;       break;
        case SDLK_WORLD_83:     rc = KEY_WORLD83;       break;
        case SDLK_WORLD_84:     rc = KEY_WORLD84;       break;
        case SDLK_WORLD_85:     rc = KEY_WORLD85;       break;
        case SDLK_WORLD_86:     rc = KEY_WORLD86;       break;
        case SDLK_WORLD_87:     rc = KEY_WORLD87;       break;
        case SDLK_WORLD_88:     rc = KEY_WORLD88;       break;
        case SDLK_WORLD_89:     rc = KEY_WORLD89;       break;
        case SDLK_WORLD_90:     rc = KEY_WORLD90;       break;
        case SDLK_WORLD_91:     rc = KEY_WORLD91;       break;
        case SDLK_WORLD_92:     rc = KEY_WORLD92;       break;
        case SDLK_WORLD_93:     rc = KEY_WORLD93;       break;
        case SDLK_WORLD_94:     rc = KEY_WORLD94;       break;
        case SDLK_WORLD_95:     rc = KEY_WORLD95;       break;

// convert 'a' - 'z' to 0-27, and then convert to ddio format.
	default:
		if (rc >= SDLK_a && rc <= SDLK_z) {
			rc = (rc - SDLK_a)+1;
		}
		else {
			rc = 0;
		}
		ASSERT(rc >= 0 && rc <=26);
		rc = sdlkey_to_ddiocode[rc];
		break;
    }

    return (ubyte)rc;
}


int sdlKeyFilter(const SDL_Event *event)
{
    ubyte kc = 0;

    if ((event->type != SDL_KEYUP) && (event->type != SDL_KEYDOWN))
        return(1);

   	switch(event->key.state)
   	{
   		case SDL_PRESSED:
   			kc = sdlkeycode_to_keycode(event->key.keysym.sym);
            if (event->key.keysym.mod & KMOD_CTRL)
            {
                switch (kc)
                {
                    case KEY_G:   // toggle grabbed input.
                        if ( ddio_mouseGrabbed )
                        {
                            SDL_WM_GrabInput(SDL_GRAB_OFF);
                            ddio_mouseGrabbed = false;
                        } // if
                        else
                        {
                            SDL_WM_GrabInput(SDL_GRAB_ON);
                            ddio_mouseGrabbed = true;
                        } // else
                        return(0);

                    #ifdef __PERMIT_GL_LOGGING
                    case KEY_INSERT:
                        if (__glLog == false)
                        {
                            DGL_EnableLogging(1);
                            __glLog = true;
                            mprintf((0, "OpenGL: Logging enabled."));
                        } // if
                        return(0);

                    case KEY_DELETE:
                        if (__glLog == true)
                        {
                            DGL_EnableLogging(0);
                            __glLog = false;
                            mprintf((0, "OpenGL: Logging disabled."));
                        } // if
                        return(0);
                    #endif
                } // switch
            } // if

            else if (event->key.keysym.mod & KMOD_ALT)
            {
                if ((kc == KEY_ENTER) || (kc == KEY_PADENTER))
                {
                    SDL_WM_ToggleFullScreen(SDL_GetVideoSurface());
                    return(0);
                } // if
            } // else if

   			LKeys[kc].down_time = timer_GetTime();
   			LKeys[kc].status = true;
   			ddio_UpdateKeyState(kc, true);
   			break;

   		case SDL_RELEASED:
   			kc = sdlkeycode_to_keycode(event->key.keysym.sym);
   			if (LKeys[kc].status)
            {
   				LKeys[kc].up_time = timer_GetTime();
   				LKeys[kc].status = false;
   				ddio_UpdateKeyState(kc, false);
   			} // if
   			break;
   	} // switch

    return(0);
} // sdlKeyFilter


bool ddio_sdl_InternalKeyInit(ddio_init_info *init_info)
{
        //reset key list
	for(int i=0;i<DDIO_MAX_KEYS; i++)
	{
		LKeys[i].down_ticks = 0;
  		LKeys[i].up_ticks = 0;
  		LKeys[i].status = false;
  	}

    SDL_EventState(SDL_KEYDOWN, SDL_ENABLE);
    SDL_EventState(SDL_KEYUP, SDL_ENABLE);
	return true;
}

void ddio_sdl_InternalKeyClose()
{
}

bool ddio_sdl_InternalKeyState(ubyte key)
{
	return LKeys[key].status;
}

void ddio_sdl_InternalKeySuspend()
{
}


void ddio_sdl_InternalKeyResume()
{
}

float ddio_sdl_InternalKeyDownTime(ubyte key)
{
	float down_time = 0.0f;
	if(LKeys[key].status)
	{
		float timer = timer_GetTime();
		down_time = timer - LKeys[key].down_time;
		LKeys[key].down_time = timer;
	}else
	{
		down_time = LKeys[key].up_time - LKeys[key].down_time;
		LKeys[key].down_time = LKeys[key].up_time = 0.0f;
	}

	return down_time;
}

void ddio_sdl_InternalResetKey(ubyte key)
{
	LKeys[key].down_time = 0.0f;
	LKeys[key].up_time = 0.0f;
	LKeys[key].status = false;
}


void ddio_sdl_InternalKeyFrame(void)
{
    // Sam 6/28 - This is already done in the mouse handling code
    //SDL_PumpEvents();
}

