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

#include "3d.h"
#include "HardwareInternal.h"
#include "renderer.h"

#define round(v) ((int)(((v) + 0.5f)))

// draws a line. takes two points.  returns true if drew
void g3_DrawLine(ddgr_color color, g3Point *p0, g3Point *p1) {
  uint8_t codes_or;
  bool was_clipped = 0;

  if (p0->p3_codes & p1->p3_codes)
    return;

  codes_or = p0->p3_codes | p1->p3_codes;

  if (codes_or) {
    ClipLine(&p0, &p1, codes_or);
    was_clipped = 1;
  }

  if (!(p0->p3_flags & PF_PROJECTED)) {
    g3_ProjectPoint(p0);
  }

  if (!(p1->p3_flags & PF_PROJECTED)) {
    g3_ProjectPoint(p1);
  }

  rend_SetFlatColor(color);
  rend_DrawLine(round(p0->p3_sx), round(p0->p3_sy), round(p1->p3_sx), round(p1->p3_sy));

  // If was clipped, free temp points
  if (was_clipped) {
    if (p0->p3_flags & PF_TEMP_POINT) {
      FreeTempPoint(p0);
    }

    if (p1->p3_flags & PF_TEMP_POINT) {
      FreeTempPoint(p1);
    }

    // Make sure all temp points have been freed
    CheckTempPoints();
  }
}

// draws a line based on the current setting of render states. takes two points.  returns true if drew
void g3_DrawSpecialLine(g3Point *p0, g3Point *p1) {
  uint8_t codes_or;
  bool was_clipped = 0;

  if (p0->p3_codes & p1->p3_codes)
    return;

  codes_or = p0->p3_codes | p1->p3_codes;

  if (codes_or) {

    ClipLine(&p0, &p1, codes_or);

    was_clipped = 1;
  }

  if (!(p0->p3_flags & PF_PROJECTED))
    g3_ProjectPoint(p0);

  if (!(p1->p3_flags & PF_PROJECTED))
    g3_ProjectPoint(p1);

  rend_DrawSpecialLine(p0, p1);

  // If was clipped, free temp points
  if (was_clipped) {
    if (p0->p3_flags & PF_TEMP_POINT)
      FreeTempPoint(p0);

    if (p1->p3_flags & PF_TEMP_POINT)
      FreeTempPoint(p1);

    // Make sure all temp points have been freed
    CheckTempPoints();
  }
}

// returns true if a plane is facing the viewer. takes the unrotated surface
// normal of the plane, and a point on it.  The normal need not be normalized
bool g3_CheckNormalFacing(vector *v, vector *norm) {
  vector tempv;

  tempv = View_position - *v;

  return ((tempv * *norm) > 0);
}

bool DoFacingCheck(vector *norm, g3Point **vertlist, vector *p) {
  if (norm) {
    // have normal
    ASSERT(norm->x || norm->y || norm->z);
    return g3_CheckNormalFacing(p, norm);
  } else {
    // normal not specified, so must compute
    vector tempv;
    // get three points (rotated) and compute normal
    vm_GetPerp(&tempv, &vertlist[0]->p3_vec, &vertlist[1]->p3_vec, &vertlist[2]->p3_vec);
    return ((tempv * vertlist[1]->p3_vec) < 0);
  }
}

// like g3_DrawPoly(), but checks to see if facing.  If surface normal is
// NULL, this routine must compute it, which will be slow.  It is better to
// pre-compute the normal, and pass it to this function.  When the normal
// is passed, this function works like g3_CheckNormalFacing() plus
// g3_DrawPoly().
void g3_CheckAndDrawPoly(int nv, g3Point **pointlist, int bm, vector *norm, vector *pnt) {
  if (DoFacingCheck(norm, pointlist, pnt))
    g3_DrawPoly(nv, pointlist, bm);
}

int Triangulate_test = 0;

// draw a polygon
// Parameters:	nv - the number of verts in the poly
//					pointlist - a pointer to a list of pointers to points
//					bm - the bitmap handle if texturing.  ignored if flat shading
// Returns 0 if clipped away
int g3_DrawPoly(int nv, g3Point **pointlist, int bm, int map_type, g3Codes *clip_codes) {
  rend_DrawPolygon3D(bm, pointlist, nv, map_type);
  return 1;

  /*
  int i;
  g3Codes cc;
  bool was_clipped=0;

  if( Triangulate_test && (nv > 3) )
  {
          g3Point *tripoints[3];
          int sum=0;

          for (i=0;i<nv-2;i++)
          {
                  tripoints[0] = pointlist[0];
                  tripoints[1] = pointlist[i+1];
                  tripoints[2] = pointlist[i+2];
                  sum += g3_DrawPoly( 3, tripoints, bm, map_type );
          }

          return sum;
  }

  //Initialize or just used the ones passed in
  if( clip_codes )
  {
          cc = *clip_codes;
  }
  else
  {
          cc.cc_or  = 0;
          cc.cc_and = 0xff;

          //Get codes for this polygon, and copy uvls into points
          for( i = 0; i < nv; ++i )
          {
                  uint8_t c = pointlist[i]->p3_codes;
                  cc.cc_and &= c;
                  cc.cc_or  |= c;
          }
  }

  //All points off screen?
  if( cc.cc_and )
          return 0;

  //One or more point off screen, so clip
  if( cc.cc_or )
  {
          //Clip the polygon, getting pointer to new buffer
          pointlist = g3_ClipPolygon( pointlist, &nv, &cc );

          //Flag as clipped so temp points will be freed
          was_clipped = 1;

          //Check for polygon clipped away, or clip otherwise failed
          if( (nv==0) || (cc.cc_or&CC_BEHIND) || cc.cc_and )
                  goto free_points;
  }

  //Make list of 2d coords (& check for overflow)
  for( i = 0; i < nv; ++i )
  {
          g3Point *p = pointlist[i];

          //Project if needed
          if( !(p->p3_flags&PF_PROJECTED) )
          {
                  g3_ProjectPoint(p);
          }
  }

  //Draw!
  rend_DrawPolygon3D( bm, pointlist, nv, map_type );

free_points:;

  //If was clipped, free temp points
  if( was_clipped )
  {
          g3_FreeTempPoints( pointlist, nv );
  }

  return 1;
  */
}

// draw a sortof sphere - i.e., the 2d radius is proportional to the 3d
// radius, but not to the distance from the eye
void g3_DrawSphere(ddgr_color color, g3Point *pnt, float rad) {
  if (!(pnt->p3_codes & CC_BEHIND)) {
    if (!(pnt->p3_flags & PF_PROJECTED))
      g3_ProjectPoint(pnt);

    rend_FillCircle(color, pnt->p3_sx, pnt->p3_sy, (rad * Matrix_scale.x * Window_w2 / pnt->p3_z));
  }
}

// draws a bitmap with the specified 3d width & height
// If offsets  are not -1, then the blitter draws not from the upper left hand
// corner of the bitmap, but from size*offsetx,size*offsety
// See Jason for explaination
void g3_DrawBitmap(vector *pos, float width, float height, int bm, int color) {
  // get the view orientation
  matrix viewOrient;
  g3_GetUnscaledMatrix(&viewOrient);

  // break down the color into components
  float r, g, b;
  if (color != -1) {
    float scale = 1.0f / 255.0f;
    r = GR_COLOR_RED(color) * scale;
    g = GR_COLOR_GREEN(color) * scale;
    b = GR_COLOR_BLUE(color) * scale;
  }

  // calculate the four corners
  g3Point corners[4], *pts[4];
  int i;
  for (i = 0; i < 4; ++i) {
    pts[i] = &corners[i];

    // calculate the offset for this corner
    float cornerScaleU = ((i & 1) ^ ((i & 2) >> 1)) ? 1.0f : -1.0f;
    float cornerScaleV = (i & 2) ? 1.0f : -1.0f;

    // find the point (parallel to the view frame)
    vector cornerPos = *pos + (viewOrient.uvec * (height * -cornerScaleV)) + (viewOrient.rvec * (width * cornerScaleU));
    corners[i].p3_codes = 0;
    g3_RotatePoint(pts[i], &cornerPos);

    // setup the flags, UVs and colors
    corners[i].p3_flags |= PF_UV;
    corners[i].p3_uvl.u = (cornerScaleU * 0.5f) + 0.5f;
    corners[i].p3_uvl.v = (cornerScaleV * 0.5f) + 0.5f;
    if (color == -1) {
      corners[i].p3_flags |= PF_L;
      corners[i].p3_uvl.l = 1.0f;
    } else {
      corners[i].p3_flags |= PF_RGBA;
      corners[i].p3_uvl.r = r;
      corners[i].p3_uvl.g = g;
      corners[i].p3_uvl.b = b;
    }
    corners[i].p3_uvl.a = 1.0f;
  }

  rend_SetTextureType(TT_LINEAR);
  rend_DrawPolygon3D(bm, pts, 4);
}

// Draws a bitmap that has been rotated about its center.  Angle of rotation is passed as 'rot_angle'
void g3_DrawRotatedBitmap(vector *pos, angle rot_angle, float width, float height, int bm, int color) {
  // get the view orientation
  matrix viewOrient;
  g3_GetUnscaledMatrix(&viewOrient);

  matrix rot_matrix;
  vm_AnglesToMatrix(&rot_matrix, 0, 0, rot_angle);

  float w = width;
  float h = height;

  vector rot_vectors[4];
  rot_vectors[0].x = -w;
  rot_vectors[0].y = h;

  rot_vectors[1].x = w;
  rot_vectors[1].y = h;

  rot_vectors[2].x = w;
  rot_vectors[2].y = -h;

  rot_vectors[3].x = -w;
  rot_vectors[3].y = -h;

  g3Point rot_points[8], *pntlist[8];
  int i;
  for (i = 0; i < 4; ++i) {
    vector offset;
    rot_vectors[i].z = 0.0f;
    vm_MatrixMulVector(&offset, &rot_vectors[i], &rot_matrix);

    vector cornerPos = *pos + (viewOrient.uvec * offset.y) + (viewOrient.rvec * offset.x);
    rot_points[i].p3_codes = 0;
    g3_RotatePoint(&rot_points[i], &cornerPos);

    rot_points[i].p3_flags |= PF_UV | PF_RGBA;
    rot_points[i].p3_l = 1.0f;
    rot_points[i].p3_uvl.u = ((i & 1) ^ ((i & 2) >> 1)) ? 1.0f : 0.0f;
    rot_points[i].p3_uvl.v = (i & 2) ? 1.0f : 0.0f;

    pntlist[i] = &rot_points[i];
  }

  // And draw!!
  rend_SetTextureType(TT_LINEAR);

  if (color != -1) {
    rend_SetLighting(LS_FLAT_GOURAUD);
    rend_SetFlatColor(color);
  }

  g3_DrawPoly(4, pntlist, bm);
}

// Draws a bitmap on a specific plane.  Also does rotation.  Angle of rotation is passed as 'rot_angle'
void g3_DrawPlanarRotatedBitmap(vector *pos, vector *norm, angle rot_angle, float width, float height, int bm) {
  matrix rot_matrix;
  vm_VectorToMatrix(&rot_matrix, norm, NULL, NULL);
  vm_TransposeMatrix(&rot_matrix);

  matrix twist_matrix;
  vm_AnglesToMatrix(&twist_matrix, 0, 0, rot_angle);

  float w = width;
  float h = height;

  vector rot_vectors[4];
  rot_vectors[0] = (twist_matrix.rvec * -w);
  rot_vectors[0] += (twist_matrix.uvec * h);

  rot_vectors[1] = (twist_matrix.rvec * w);
  rot_vectors[1] += (twist_matrix.uvec * h);

  rot_vectors[2] = (twist_matrix.rvec * w);
  rot_vectors[2] -= (twist_matrix.uvec * h);

  rot_vectors[3] = (twist_matrix.rvec * -w);
  rot_vectors[3] -= (twist_matrix.uvec * h);

  int i;
  for (i = 0; i < 4; ++i) {
    vector temp_vec = rot_vectors[i];
    vm_MatrixMulVector(&rot_vectors[i], &temp_vec, &rot_matrix);
  }

  g3Point rot_points[8], *pntlist[8];
  for (i = 0; i < 4; ++i) {
    rot_vectors[i] += *pos;

    g3_RotatePoint(&rot_points[i], &rot_vectors[i]);
    rot_points[i].p3_flags |= PF_UV | PF_L;
    rot_points[i].p3_l = 1.0f;

    pntlist[i] = &rot_points[i];
  }

  rot_points[0].p3_u = 0.0f;
  rot_points[0].p3_v = 0.0f;

  rot_points[1].p3_u = 1.0f;
  rot_points[1].p3_v = 0.0f;

  rot_points[2].p3_u = 1.0f;
  rot_points[2].p3_v = 1.0f;

  rot_points[3].p3_u = 0.0f;
  rot_points[3].p3_v = 1.0f;

  // And draw!!
  rend_SetTextureType(TT_LINEAR);
  g3_DrawPoly(4, pntlist, bm);
}

// Draw a wireframe box aligned with the screen.  Used for the editor.
// Parameters:	color - the color to draw the lines
//					pnt - the center point
//					rad - specifies the width/2 & height/2 of the box
void g3_DrawBox(ddgr_color color, g3Point *pnt, float rad) {
  if (!(pnt->p3_codes & CC_BEHIND)) {
    if (!(pnt->p3_flags & PF_PROJECTED))
      g3_ProjectPoint(pnt);

    float w, h;

    w = rad * Matrix_scale.x * Window_w2 / pnt->p3_z;
    h = rad * Matrix_scale.y * Window_h2 / pnt->p3_z;

    rend_DrawLine(round(pnt->p3_sx - w), round(pnt->p3_sy - h), round(pnt->p3_sx + w), round(pnt->p3_sy - h));
    rend_DrawLine(round(pnt->p3_sx + w), round(pnt->p3_sy - h), round(pnt->p3_sx + w), round(pnt->p3_sy + h));
    rend_DrawLine(round(pnt->p3_sx + w), round(pnt->p3_sy + h), round(pnt->p3_sx - w), round(pnt->p3_sy + h));
    rend_DrawLine(round(pnt->p3_sx - w), round(pnt->p3_sy + h), round(pnt->p3_sx - w), round(pnt->p3_sy - h));
  }
}

// Sets the triangulation test to on or off
void g3_SetTriangulationTest(int state) { Triangulate_test = state; }
