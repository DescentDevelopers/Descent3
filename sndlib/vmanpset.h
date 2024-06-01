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

/*****************************************************************************/
/*									     				*/
/*									     				*/
/*      Abstract:                                                            	*/
/*                                                                           	*/
/*		DirectSound3D Voice Manager property set definitions.	     		*/
/*		April 24, 1998				     					*/
/*****************************************************************************/

#ifndef _VMANPSET_H_
#define _VMANPSET_H_

/******************************************************************************/
/*									     				*/
/*	G l o b a l   t y p e d e f s					     			*/
/*									     				*/
/******************************************************************************/

enum DSPROPERTY_VMANAGER {
  DSPROPERTY_VMANAGER_MODE = 0,
  DSPROPERTY_VMANAGER_PRIORITY,
  DSPROPERTY_VMANAGER_STATE
};

enum VmMode {
  DSPROPERTY_VMANAGER_MODE_DEFAULT = 0,
  DSPROPERTY_VMANAGER_MODE_AUTO,
  DSPROPERTY_VMANAGER_MODE_REPORT,
  DSPROPERTY_VMANAGER_MODE_USER
};

enum VmState {
  DSPROPERTY_VMANAGER_STATE_PLAYING3DHW = 0,
  DSPROPERTY_VMANAGER_STATE_SILENT,
  DSPROPERTY_VMANAGER_STATE_BUMPED,
  DSPROPERTY_VMANAGER_STATE_PLAYFAILED
};

#ifdef __cplusplus
extern "C" {
#endif

// {62A69BAE-DF9D-11d1-99A6-00C04FC99D46}
DEFINE_GUID(DSPROPSETID_VoiceManager, 0x62a69bae, 0xdf9d, 0x11d1, 0x99, 0xa6, 0x0, 0xc0, 0x4f, 0xc9, 0x9d, 0x46);

#ifdef __cplusplus
}
#endif

#endif
