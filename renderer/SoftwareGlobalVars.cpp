#include "RendererConfig.h"
#ifdef USE_SOFTWARE_TNL

#include "3d.h"
#include "SoftwareInternal.h"
#include <float.h>

vector	View_position;
float	View_zoom;

float	Far_clip_z=FLT_MAX; 		// set to a really really far distance initially

ubyte	Clip_custom=0;
float	Clip_plane_distance=0;
vector	Clip_plane;

matrix	Unscaled_matrix;		//before scaling
matrix	View_matrix;

vector	Matrix_scale;			//how the matrix is scaled, window_scale * zoom

int		Window_width;			//the actual width
int		Window_height;			//the actual height

float	Window_w2;				//width/2
float	Window_h2;				//height/2

float gTransformViewPort[4][4];
float gTransformProjection[4][4];
float gTransformModelView[4][4];
float gTransformFull[4][4];

#endif
