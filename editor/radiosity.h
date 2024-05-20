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

 * $Logfile: /DescentIII/Main/editor/radiosity.h $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:57:38 $
 * $Author: kevinb $
 *
 * Header for radiosity.cpp
 *
 * $Log: not supported by cvs2svn $
 * 
 * 33    3/10/99 7:12p Jason
 * added smooth specular shading for curved surfaces
 * 
 * 32    8/10/98 11:25a Jason
 * added better specular lighting
 * 
 * 31    6/02/98 6:04p Jason
 * added specular lightmaps
 * 
 * 30    5/22/98 3:28p Jason
 * added specular lighting
 * 
 * 29    5/20/98 5:44p Jason
 * incremental checkin for bumpmapping
 * 
 * 28    5/19/98 2:36p Jason
 * improved volume light casting
 * 
 * 27    5/15/98 5:41p Jason
 * implemented volume lighting system
 * 
 * 26    3/11/98 5:14p Jason
 * incremental lighting checkin
 * 
 * 25    3/06/98 3:23p Jason
 * added lighting from satellites to indoor rooms
 * 
 * 24    2/05/98 4:20p Matt
 * Fixed terrain lighting broken in roomnum/cellnum change
 * 
 * 23    2/04/98 6:23p Matt
 * Changed object room number to indicate a terrain cell via a flag.  Got
 * rid of the object flag which used to indicate terrain.
 * 
 */

#ifndef RADIOSITY_H
#define RADIOSITY_H

#include "pserror.h"
#include "pstypes.h"
#include "3d.h"
#include "gr.h"

// Shooting methods
#define SM_HEMICUBE						0
#define SM_RAYCAST						1
// This method makes a switch from RAYCAST to HEMICUBE after all satellites have
// been used for lightsources
#define SM_SWITCH_AFTER_SATELLITES	2

#define MAX_VOLUME_ELEMENTS		500


struct spectra
{
	float r,g,b;
};


// element flags
#define EF_IGNORE		1
#define EF_SMALL		2	// Don't blend this one into the lightmap - it will corrupt!

struct rad_element
{
	vector *verts;
	spectra exitance;
	float area;
	uint8_t num_verts;
	uint8_t flags;  // see above
};

#define VEF_REVERSE_SHOOT	1

struct volume_element
{
	spectra color;
	vector pos;	
	uint8_t flags;
};

#define ST_ROOM				0		// This is a room surface
#define ST_TERRAIN			1		// This is a terrain surface
#define ST_SATELLITE			2		// This is a sun on the terrain
#define ST_PORTAL				3		// This surface is a portal, don't consider it
#define ST_ROOM_OBJECT		4		// This surface is part of an object
#define ST_TERRAIN_OBJECT	5		// This surface is part of terrain object
#define ST_EXTERNAL_ROOM	6		// This surface is a room outside

#define SF_TOUCHES_TERRAIN	1
#define SF_LIGHTSOURCE	2

struct rad_surface
{
	float area;
	spectra emittance;		
	spectra exitance; 
	float reflectivity;		// how much light bounces off 
	uint8_t xresolution;			// how many elements (resolution x resolution) for this face
	uint8_t yresolution;
	rad_element *elements;	// list of elements for this surface
	vector normal;				// normal of this surface
	vector *verts;
	
	uint8_t surface_type;		// See ST_ types above
	
	int	facenum;				// facenumber of room
	int	roomnum;				// The roomnumber or terrain segment number
	uint8_t num_verts;
	uint8_t flags;

	float surface_area,element_area;

	uint8_t x1,y1,x2,y2;		// Where in the lightmap our bounds are
};

struct rad_point
{
	vector pos;
	uint8_t code;

};

extern float *Room_strongest_value[][4];

extern int Ignore_terrain;
extern int Ignore_satellites;

extern float Ignore_limit;
extern float rad_TotalFlux;
extern float rad_Convergence;

extern int	rad_StepCount;
extern int	rad_MaxStep;
extern int	rad_DoneCalculating;

extern float	rad_TotalUnsent;

extern rad_surface	*rad_MaxSurface;

extern int	rad_NumSurfaces;
extern int	rad_NumElements;

extern float *rad_FormFactors;
extern rad_surface	*rad_Surfaces;
extern volume_element *Volume_elements[];

extern int UseVolumeLights;
extern int Calculate_specular_lighting;

extern int Shoot_from_patch;

// Tells radiosity renderer to do volume lighting
extern int Do_volume_lighting;


int DoRadiosityRun (int method,rad_surface *light_surfaces,int count);
// Sets up our radiosity run
void InitRadiosityRun ();

// Initializes the exitances for all surfaces
void InitExitance ();

// Gets the spectral emittance for a surface
void GetEmittance (rad_surface *surf,spectra *dest);

// Sets all the elements of a surface to their initial exitance values
void SetExitanceForSurface (rad_surface *surf);


// Find the surface we want to shoot from
void UpdateUnsentValues ();

// Does one iteration of ray-casting radiosity
int DoRadiosityIteration ();

// Finds the world coordinate center of a element
void GetCenterOfElement (rad_element *ep,vector *dest);

// Finds the world coordinate center of a surface
void GetCenterOfSurface (rad_surface *ep,vector *dest);

// Returns 1 if a src vector can hit dest vector unobstructed
int ShootRayFromPoint (vector *src,vector *dest,rad_surface *src_surf,rad_surface *dest_surf);

// Shoots a ray from the center of the max surface to center of every other element
// Also updates the exitances of elements that get hit

void ShootMaxSurface ();

// Does all the radiosity calculations until done
void CalculateRadiosity ();

// returns the amount of unsent flux from a surface
float GetUnsentFlux (rad_surface *surface);

// Shuts down the radiosity stuff, freeing memory, etc
void CloseRadiosityRun ();

// Initalizes memory for form factors
void SetupFormFactors ();

// Calculates the area of the surfaces and elements in our environment
void CalculateArea ();

// Counts the total number of elements we have to work with
void CountElements ();

// Returns max color
float GetMaxColor (spectra *sp);


#endif


