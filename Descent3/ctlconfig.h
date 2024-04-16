/*
 * $Logfile: /DescentIII/main/ctlconfig.h $
 * $Revision: 7 $
 * $Date: 4/30/99 12:12p $
 * $Author: Samir $
 * 
 * <insert description of file here>
 *
 * $Log: /DescentIII/main/ctlconfig.h $
 * 
 * 7     4/30/99 12:12p Samir
 * added strings for key ramping, other buttons in general config menu.
 * 
 * 6     4/22/99 3:43p Kevin
 * Training missions show controls on screen
 * 
 * 5     2/16/99 12:07p Samir
 * redid controller config with new ui.
 * 
 * 4     10/18/98 7:24p Samir
 * added a reset to defaults options for key and joystick config.
 * 
 * 3     2/15/98 7:06p Samir
 * Keyboard configuration system done.  Not joystick.
 * 
 * 2     2/10/98 3:45p Samir
 * Added list of configuration items.
 * 
 * 1     2/10/98 11:49a Samir
 * Initial revision.
 *
 * $NoKeywords: $
 */

#ifndef CTLCONFIG_H
#define CTLCONFIG_H

#define CTLCONFIG_KEYBOARD			0
#define CTLCONFIG_CONTROLLER		1
#define CTLCONFIG_WPNSEL			2

typedef struct t_cfg_element
{
	short fn_id;									// -1 = group start
	short text;										// text string id.
	short x;
	short y;											// location (for groups only)
}
t_cfg_element;

extern t_cfg_element Cfg_key_elements[];
extern t_cfg_element Cfg_joy_elements[];

int CtlFindBinding(int controlid,bool keyboard);
//	configures controllers.
void CtlConfig(int mode);

// opens the settings dialogs.
void joystick_settings_dialog();
void key_settings_dialog();

#endif