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

#ifndef HEMICUBE_H
#define HEMICUBE_H

#include "3d.h"
#include "radiosity.h"
#include "gr.h"

struct rad_hemicube
{
	matrix view_matrix;
	matrix head_matrix;
	vector view_position;
	rad_element *shooting_element;
	rad_surface *shooting_surface;
	
	grHardwareSurface drawing_surface;
	grViewport *vport;
	int *id_grid;
	float *depth_grid;

	float *top_array;
	float *side_array;

	int ff_res;				// Form factor resolution
	int grid_dim;			// Hemicube top/side array dimension

};

struct hemicube_point
{
	float sx,sy;
	float z;
};


extern rad_hemicube rad_Hemicube;
extern int Hemicube_resolution;

// Calculates the form factors for a hemicube
void CalculateFormFactorsHemiCube ();

// Initializes the hemicube
void InitHemicube (int resolution);

void CloseHemicube ();

// Clears the Zbuffer hemicube grid
void ClearHemicubeGrid ();

// Sets the elements we're shooting from
void SetElementView(rad_element *ep);

// Sets the surface we're shooting from
void SetSurfaceView(rad_surface *ep);

// Build transformation matrix for our hemicube
void BuildTransform(vector *nu,vector *nv,vector *nn);

// Gets our view system ready for drawing
void StartHemicubeDrawing ();

// Releases the view system
void EndHemicubeDrawing (int);

// Update hemicube view transformation matrix
void UpdateView( int face_id );

// Actually draws the projected polygon onto a hemicube face	
void ScanRadiosityPoly (g3Point **pl,int nv,int element_id);
void DrawRadiosityPoly(int nv,g3Point **pointlist,int id);
void GetVertexOrdering (hemicube_point *t, int nv, int *vlt, int *vlb, int *vrt, int *vrb,int *bottom_y_ind);
int PrevIndex(int val,int modulus);
int NextIndex(int val,int modulus);

// Sums the delta form factors
void SumDeltas (float *ff_array,int face_id);

float GetTopFactor( int row, int col );

// Get side face cell form factor
float GetSideFactor( int row, int col );



#endif
