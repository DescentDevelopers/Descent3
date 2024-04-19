/*
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
typedef enum
{
	RENDERER_SOFTWARE_8BIT,
	RENDERER_SOFTWARE_16BIT,
	RENDERER_OPENGL,
	RENDERER_DIRECT3D,
	RENDERER_GLIDE,
	RENDERER_NONE
} RendererType;

// The structure for a 3D device
typedef struct card3d {
	char name[1024];
	RendererType renderer_type;
} card3d;

extern card3d Cards[MAX_CARDS];
extern int Num_cards;

// Fills in Cards and Num_cards
// If really_detect is 0, just find the software only one.
void detect_3dcards(int detect_direct3d, int detect_glide, int detect_opengl);
char *GetFullName(card3d *card);
void shutdown_glide(void);

#endif //_3D_DETECT_H
