/*
 * $Logfile: /DescentIII/main/lib/multi_save_settings.h $
 * $Revision: 3 $
 * $Date: 9/23/98 6:33p $
 * $Author: Kevin $
 *
 * Multiplayer settings save/load 
 *
 * $Log: /DescentIII/main/lib/multi_save_settings.h $
 * 
 * 3     9/23/98 6:33p Kevin
 * Fixed load settings
 * 
 * 2     9/23/98 2:55p Kevin
 * Saved multi config and changed UI to conform
 * 
 * 1     9/23/98 10:42a Kevin
 * 
 *
 * Initial Version
 *
 * $NoKeywords: $
 */

#ifndef _MULTI_SAVE_SETTINGS_HEADER
#define _MULTI_SAVE_SETTINGS_HEADER

#define MAX_MPS_LINE_LEN	200


int MultiSaveSettings(char *filename);
int MultiLoadSettings(char *filename);














#endif