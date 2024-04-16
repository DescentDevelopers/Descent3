/*
* $Logfile: /DescentIII/main/viseffect_external.h $
* $Revision: 6 $
* $Date: 3/21/00 9:58a $
* $Author: Matt $
*
* <insert description of file here>
*
* $Log: /DescentIII/main/viseffect_external.h $
 * 
 * 6     3/21/00 9:58a Matt
 * Changed to Mac-only the code that sets a variable number of vis effects
 * based on texture quality.
 * 
 * 5     3/20/00 2:27p Matt
 * Merge of Duane's post-1.3 changes.
 * Moved max_vis_effects (the variable, not the constant) from viseffect.h
 * to viseffect_external.h.
 * 
 * 4     10/21/99 9:30p Jeff
 * B.A. Macintosh code merge
 * 
 * 3     5/02/99 1:37a Jason
 * added moving object lighting viseffects
 * 
 * 2     4/30/99 7:37p Jeff
 * created viseffect_external.h
*
* $NoKeywords: $
*/


#ifndef __VISEFFECT_EXTERNAL_H_
#define __VISEFFECT_EXTERNAL_H_


#include "pstypes.h"
#include "pserror.h"
#include "vecmat.h"

#ifdef MACINTOSH
#define MAX_VIS_EFFECTS	4096 //DAJ utb 5000
#else
#define MAX_VIS_EFFECTS	5000
#endif

// types
#define VIS_NONE			0
#define VIS_FIREBALL		1

// Flags
#define VF_USES_LIFELEFT		1
#define VF_WINDSHIELD_EFFECT	2
#define VF_DEAD					4
#define VF_PLANAR					8
#define VF_REVERSE				16
#define VF_EXPAND					32
#define VF_ATTACHED				64
#define VF_NO_Z_ADJUST			128
#define VF_LINK_TO_VIEWER		256	// Always link into the room that the viewer is in

extern ushort max_vis_effects;

struct object;

typedef struct
{
	int obj_handle;
	int dest_objhandle;

	ushort modelnum;
	ushort vertnum;
	ushort end_vertnum;

	ubyte subnum,subnum2;	
} vis_attach_info;

typedef struct
{
	ubyte width;
	ubyte height;
	ubyte texture;			
} axis_billboard_info;


typedef struct
{
	vector pos;
	vector end_pos;	
	vector velocity;
	float mass;
	float drag;
	float size;
	float lifeleft;
	float lifetime;
	float creation_time;

	int roomnum;
	
	int phys_flags;
	
	short custom_handle;
	ushort lighting_color;

	ushort flags;

	short next;
	short prev;

	vis_attach_info attach_info;
	axis_billboard_info billboard_info;

	ubyte movement_type;
	ubyte type,id;
} vis_effect;

#endif