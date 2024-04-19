/*
 * Header for moveworld.cpp
 *
 */

#ifndef _MOVEWORLD_H
#define _MOVEWORLD_H

#include "vecmat.h"

//Read the mouse and move the wireframe view
void MoveWorld();

//Reset the orienation, position, & distance of the wireframe view
void ResetWireframeView();

//Reset the view radius of the wireframe view
void ResetWireframeViewRad();

//Set the wireframe view target to a specific location
//Does not reset the distance or orientation
void SetWireframeView(vector *pos);

//Structure to describe the wireframe view
typedef struct {
	matrix	orient;
	vector	target;
	float		dist;
	float		rad;
} wireframe_view;

//The current wireframe view
extern wireframe_view *Wireframe_view;

//Wireframe views for the mine & room views
extern wireframe_view Wireframe_view_mine,Wireframe_view_room;

#endif	//ifndef _MOVEWORLD_H
