/*
 * $Logfile: /DescentIII/Main/editor/rad_cast.h $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:57:38 $
 * $Author: kevinb $
 *
 * Header for rad_cast.cpp
 *
 * $Log: not supported by cvs2svn $
 * 
 * 3     2/04/98 6:23p Matt
 * Changed object room number to indicate a terrain cell via a flag.  Got
 * rid of the object flag which used to indicate terrain.
 * 
 */

#ifndef RAD_CAST_H
#define RAD_CAST_H

#include "radiosity.h"

// Returns 1 if a src vector can hit dest vector unobstructed, else 0
int ShootRayFromPoint (vector *src,vector *dest,rad_surface *src_surf,rad_surface *dest_surf);

// Calculates the form factors using a raycasting method
void CalculateFormFactorsRaycast ();

#endif