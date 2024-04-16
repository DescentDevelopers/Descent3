/*	internal header to 2dlib

*/

#ifndef LIB2D_H
#define LIB2D_H

#include "gr.h"
#include "renderer.h"

//	structures

typedef struct mem_bitmap {
	char *data;
	short bpp;
	int rowsize;
	ushort alloced:2;
	ushort flag:14; 
} mem_bitmap;

#define MEMFLAG_TRANSBLT	1


//	---------------------------------------------------------------------------
//	Line Drawing Functions

void gr_Line(gr_pen *pen, int x1, int y1, int x2, int y2);
void gr_HLine(gr_pen *pen, int x1, int x2, int y);
void gr_VLine(gr_pen *pen, int y1, int y2, int x);
void gr_Rect(gr_pen *pen, int l, int t, int r, int b);
void gr_FillRect(gr_pen *pen, int l, int t, int r, int b);


//	---------------------------------------------------------------------------
//	Memory surface functions

bool gr_mem_surf_Create(ddgr_surface *sf);
bool gr_mem_surf_Destroy(ddgr_surface *sf);

//	generic clearing functions
void gr_mem_surf_Clear(ddgr_surface *dsf, ddgr_color col, int l, int t, int w, int h);

//	non-scaling bitmap blt functions
bool gr_mem_surf_Blt(ddgr_surface *dsf, int dx, int dy,
					 ddgr_surface *ssf, int sx, int sy, int sw, int sh);

//	retrieves a pointer to surface memory.  allowed to lock one surface multiple times.
bool gr_mem_surf_Lock(ddgr_surface *sf, void **ptr, int *rowsize);
bool gr_mem_surf_Unlock(ddgr_surface *sf, void *ptr);

//	initializes a surface based of preinitialized objects by the user.
bool gr_mem_surf_Init(ddgr_surface *sf, char *data, int rowsize);

#endif
