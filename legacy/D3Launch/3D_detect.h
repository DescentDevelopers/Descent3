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

 * $Logfile: /DescentIII/Main/D3Launch/3D_detect.h $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:56:51 $
 * $Author: kevinb $
 *
 * Code to detect what 3d cards you have
 *
 * $Log: not supported by cvs2svn $
 * 
 * 6     10/15/98 7:30p Nate
 * 
 * 5     10/12/98 7:13p Nate
 * Fixed several bugs.
 * 
 * 4     9/01/98 7:15p Nate
 * Major Revision #2
 * 
 * 3     8/05/98 11:54a Nate
 * Initial Version
 * 
 * 2     7/27/98 11:46a Nate
 * Added D3Launch
 * 
 * 1     7/27/98 11:45a Nate
 * 
 * 2     3/07/98 2:55p John
 * Added tabs for different devices in setup
 * 
 * 1     2/18/98 4:07p John
 *
 * $NoKeywords: $
 */


#ifndef _3D_DETECT_H
#define _3D_DETECT_H

#include "PsTypes.h"

#define MAX_CARDS 16

// The list of rendering device types
enum RendererType
{
	RENDERER_SOFTWARE_8BIT,
	RENDERER_SOFTWARE_16BIT,
	RENDERER_OPENGL,
	RENDERER_DIRECT3D,
	RENDERER_GLIDE,
	RENDERER_NONE
};

// The structure for a 3D device
struct card3d {
	char name[1024];
	RendererType renderer_type;
};

extern card3d Cards[MAX_CARDS];
extern int Num_cards;

// Fills in Cards and Num_cards
// If really_detect is 0, just find the software only one.
void detect_3dcards(int detect_direct3d, int detect_glide, int detect_opengl);
char *GetFullName(card3d *card);
void shutdown_glide(void);

#endif //_3D_DETECT_H
