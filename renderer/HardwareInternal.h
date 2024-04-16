#ifndef __HARDWARE_INTERNAL_H__
#define __HARDWARE_INTERNAL_H__

#include "pserror.h"

#define MAX_POINTS_IN_POLY 100

void FreeTempPoint(g3Point *p);
void InitFreePoints(void);
void ClipLine(g3Point **p0,g3Point **p1,ubyte codes_or);

//Verify that all the temp points are free, and free them it they are not.
#ifdef _DEBUG
void CheckTempPoints();
#else
#define CheckTempPoints()
#endif


extern int Window_width,Window_height;		//the actual integer width & height
extern float Window_w2,Window_h2;	  		//width,height/2
extern float View_zoom;
extern vector View_position,Matrix_scale;
extern matrix View_matrix,Unscaled_matrix;
extern float Far_clip_z;

// For custom clipping plane
extern ubyte Clip_custom;
extern float Clip_plane_distance;
extern vector Clip_plane;

extern float gTransformViewPort[4][4];
extern float gTransformProjection[4][4];
extern float gTransformModelView[4][4];
extern float gTransformFull[4][4];
void g3_UpdateFullTransform();
void g3_ForceTransformRefresh(void);

void rend_TransformSetToPassthru(void);
void rend_TransformSetViewport( int lx, int ty, int width, int height );
void rend_TransformSetProjection( float trans[4][4] );
void rend_TransformSetModelView( float trans[4][4] );

#endif
