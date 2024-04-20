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
 * $Logfile: /Descent3/main/ddgr_mac/ddgr_mac.h $
 * $Revision: 1.1.1.1 $
 * $Date: 2003/08/26 03:56:53 $
 * $Author: kevinb $
 *
 * macintosh implementation of interface to ddgr library
 *
 * $Log: ddgr_mac.h,v $
 * Revision 1.1.1.1  2003/08/26 03:56:53  kevinb
 * initial 1.5 import
 *
 *
 * 5     5/19/97 2:52 PM Jeremy
 * added constructor/destructor to ddgr_mac_lib
 *
 * 4     5/11/97 8:02 PM Jeremy
 * removed use of m_displayAspectRatio
 *
 * 3     5/9/97 7:13 PM Jeremy
 * some bug fixes in initialization code
 *
 * 2     4/15/97 7:02 PM Jeremy
 * initial implementation of initialization and closing of ddgr mac
 * library.  also added c version of scale bitmap16
 *
 * 1     4/9/97 7:16 PM Jeremy
 * initial check in
 *
 * $NoKeywords: $
 */

#ifndef DDGR_MAC_H
#define DDGR_MAC_H

#include <string.h>

#include "ddgr.h"
#include "pserror.h"
#include "gameos.h"

class ddgr_mac_lib {
public:
  ddgr_mac_lib(void);
  ~ddgr_mac_lib(void);
  void SetWindowedMode(bool inWindowMode = true);
  bool IsWindowed(void);

  void SetInitted(bool inInitState = true);
  bool IsInitted(void);

  void SetDefaultColorKey(ddgr_color inColor);

private:
  bool m_initted;
  bool m_windowed_mode;
  ddgr_color m_color_key;
};

extern ddgr_mac_lib Mac_DDGR_Lib;

#endif