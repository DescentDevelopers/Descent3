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

// rad_scan

#include "vecmat.h"
#include "3d.h"
#include "radiosity.h"
#include "hemicube.h"
#include "gr.h"
#include "d3edit.h"
#include "ddio.h"
#include "mem.h"

#include <stdlib.h>
#include <algorithm>

#define TOP_FACE 0
#define LEFT_FACE 1
#define RIGHT_FACE 2
#define FRONT_FACE 3
#define BACK_FACE 4

float Hemicube_view_zoom = 1.0;

int rad_Drawing = 0;

g3Point Element_points[100];
rad_element *rad_MaxElement;

rad_hemicube rad_Hemicube;

extern void ShowRadView();
int Show_rad_progress = 0;
int Cracks_this_frame, Cracks_this_side;

float Highest_top_delta, Highest_side_delta;

#define PI 3.141592654

// Calculates delta form factors
void CalculateDeltaFormFactors() {
  int i, j;         // Loop indices
  float da;         // Cell area
  float dx, dy, dz; // Cell dimensions
  float r, x, y, z; // Cell co-ordinates
  float val;

  // Initialize cell dimensions and area
  Highest_top_delta = -1.0f;
  Highest_side_delta = -1.0f;
  dx = dy = dz = 2.0 / (float)rad_Hemicube.ff_res;
  da = 4.0 / ((float)rad_Hemicube.ff_res * (float)rad_Hemicube.ff_res);

  // Calculate top face delta form factors
  x = dx / 2.0;
  for (i = 0; i < rad_Hemicube.grid_dim; i++) {
    y = dy / 2.0;
    for (j = 0; j < rad_Hemicube.grid_dim; j++) {
      r = x * x + y * y + 1.0;
      val = (float)(da / (PI * r * r));
      rad_Hemicube.top_array[j * rad_Hemicube.grid_dim + i] = val;

      if (val > Highest_top_delta)
        Highest_top_delta = val;

      y += dy;
    }
    x += dx;
  }

  // Calculate side face delta form factors
  x = dx / 2.0;
  for (i = 0; i < rad_Hemicube.grid_dim; i++) {
    z = dz / 2.0;
    for (j = 0; j < rad_Hemicube.grid_dim; j++) {
      r = x * x + z * z + 1.0;
      val = (float)(z * da / (PI * r * r));
      rad_Hemicube.side_array[j * rad_Hemicube.grid_dim + i] = val;

      if (val > Highest_side_delta)
        Highest_side_delta = val;

      z += dz;
    }
    x += dx;
  }
}

// Calculates the form factors for a hemicube
void CalculateFormFactorsHemiCube() {
  g3Point *pointlist[100];
  int i, t, k, j, en_limit, en;
  int ff_index = 0;
  int self;
  rad_element *dest_element;

  for (i = 0; i < rad_NumElements; i++)
    rad_FormFactors[i] = 0.0f;

  // Shoot from patch (faster) or element?
  if (Shoot_from_patch)
    en_limit = 1;
  else
    en_limit = rad_MaxSurface->xresolution * rad_MaxSurface->yresolution;
  for (en = 0; en < en_limit; en++) {
    if (Shoot_from_patch) {
      SetSurfaceView(rad_MaxSurface);
    } else {
      rad_MaxElement = &rad_MaxSurface->elements[en];
      if (rad_MaxElement->flags & EF_IGNORE)
        continue;

      SetElementView(rad_MaxElement);
    }

    Cracks_this_frame = 0;

    for (i = 0; i < 5; i++) {
      ClearHemicubeGrid();
      UpdateView(i);
      StartHemicubeDrawing();

      ff_index = 0;

      for (t = 0; t < rad_NumSurfaces; t++) {
        int ignore = 0;
        rad_surface *surf = &rad_Surfaces[t];

        if (surf == rad_MaxSurface)
          ignore = 1;
        if (surf->surface_type == ST_PORTAL)
          ignore = 1;

        for (j = 0; j < surf->xresolution * surf->yresolution; j++, ff_index++) {
          if (ignore)
            continue;

          rad_element *ep = &surf->elements[j];

          if (ep->flags & EF_IGNORE)
            continue;

          for (k = 0; k < ep->num_verts; k++) {
            vector vec = ep->verts[k];
            g3_RotatePoint(&Element_points[k], &vec);
            Element_points[k].p3_flags = 0;
          }

          if (g3_CheckNormalFacing(&ep->verts[0], &surf->normal)) {
            for (k = 0; k < ep->num_verts; k++) {
              g3Point *p = &Element_points[k];
              pointlist[k] = p;
            }

            DrawRadiosityPoly(ep->num_verts, pointlist, ff_index);
          }
        }
      }

      SumDeltas(rad_FormFactors, i);
      EndHemicubeDrawing(i);
    }
  }

  // Now extract the results
  for (ff_index = 0, i = 0; i < rad_NumSurfaces; i++) {
    rad_surface *surf = &rad_Surfaces[i];

    // Check for self surface
    if (rad_MaxSurface == surf)
      self = 1;
    else
      self = 0;

    for (t = 0; t < surf->xresolution * surf->yresolution; t++, ff_index++) {
      if (self)
        continue;

      dest_element = &surf->elements[t];

      if (rad_FormFactors[ff_index] > 0.0) {
        spectra delta;
        float reflect_factor = surf->reflectivity;

        // Compute reciprocal form factor
        float rff;

        if (rad_MaxSurface->surface_type == ST_SATELLITE)
          rff = std::min(rad_FormFactors[ff_index], 1.0f);
        else
          rff = (float)std::min(rad_FormFactors[ff_index] * rad_MaxSurface->area / dest_element->area, 1.0f);

        // Get shooting patch unsent exitance
        spectra shoot = rad_MaxSurface->exitance;

        // Calculate delta exitance
        delta.r = shoot.r * reflect_factor * rff;
        delta.g = shoot.g * reflect_factor * rff;
        delta.b = shoot.b * reflect_factor * rff;

        // Update element exitance
        dest_element->exitance.r += delta.r;
        dest_element->exitance.g += delta.g;
        dest_element->exitance.b += delta.b;

        // Update patch unsent exitance

        surf->exitance.r += ((dest_element->area / surf->area) * delta.r);
        surf->exitance.g += ((dest_element->area / surf->area) * delta.g);
        surf->exitance.b += ((dest_element->area / surf->area) * delta.b);
      }
    }
  }
}

void InitHemicube(int resolution) {
  // Make sure resolution is even
  ASSERT(resolution % 2 == 0);

  rad_Drawing = 1;

  rad_Hemicube.ff_res = resolution;
  rad_Hemicube.grid_dim = resolution / 2;

  rad_Hemicube.id_grid = (int *)mem_malloc(rad_Hemicube.ff_res * rad_Hemicube.ff_res * sizeof(int));
  ASSERT(rad_Hemicube.id_grid != NULL);
  rad_Hemicube.depth_grid = (float *)mem_malloc(rad_Hemicube.ff_res * rad_Hemicube.ff_res * sizeof(float));
  ASSERT(rad_Hemicube.depth_grid != NULL);

  rad_Hemicube.top_array = (float *)mem_malloc(rad_Hemicube.grid_dim * rad_Hemicube.grid_dim * sizeof(float));
  ASSERT(rad_Hemicube.top_array != NULL);
  rad_Hemicube.side_array = (float *)mem_malloc(rad_Hemicube.grid_dim * rad_Hemicube.grid_dim * sizeof(float));
  ASSERT(rad_Hemicube.side_array != NULL);

  CalculateDeltaFormFactors();

  // Get a surface to draw to
  rad_Hemicube.drawing_surface.create(rad_Hemicube.ff_res, rad_Hemicube.ff_res, BPP_16);
  rad_Hemicube.vport = new grViewport(&rad_Hemicube.drawing_surface);
}

void CloseHemicube() {
  delete rad_Hemicube.vport;
  rad_Hemicube.drawing_surface.free();

  mem_free(rad_Hemicube.depth_grid);
  mem_free(rad_Hemicube.id_grid);
  mem_free(rad_Hemicube.side_array);
  mem_free(rad_Hemicube.top_array);

  rad_Drawing = 0;
}

void ClearHemicubeGrid() {
  int i, t;

  for (i = 0; i < rad_Hemicube.ff_res; i++) {
    for (t = 0; t < rad_Hemicube.ff_res; t++) {
      rad_Hemicube.depth_grid[i * rad_Hemicube.ff_res + t] = 999999.0f;
      rad_Hemicube.id_grid[i * rad_Hemicube.ff_res + t] = -1;
    }
  }
}

void SetElementView(rad_element *ep) {
  vector rv; // Random vector
  vector u, v, n;

  // Select random vector for hemicube orientation
  rv.x = ((rand() / RAND_MAX) * 2.0 - 1.0);
  rv.y = ((rand() / RAND_MAX) * 2.0 - 1.0);
  rv.z = ((rand() / RAND_MAX) * 2.0 - 1.0);

  n = rad_MaxSurface->normal; // Get patch normal

  do // Get valid u-axis vector
  {
    vm_CrossProduct(&u, &n, &rv);
  } while (vm_GetMagnitude(&u) < .0001);

  vm_NormalizeVector(&u);
  vm_CrossProduct(&v, &u, &n); // Determine v-axis

  rad_Hemicube.head_matrix.rvec = u;
  rad_Hemicube.head_matrix.uvec = v;
  rad_Hemicube.head_matrix.fvec = n;

  vm_VectorToMatrix(&rad_Hemicube.head_matrix, &n, NULL, NULL);

  rad_Hemicube.shooting_element = ep;
}

void SetSurfaceView(rad_surface *surf) {
  vector rv; // Random vector
  vector u, v, n;

  // Select random vector for hemicube orientation
  rv.x = ((rand() / RAND_MAX) * 2.0 - 1.0);
  rv.y = ((rand() / RAND_MAX) * 2.0 - 1.0);
  rv.z = ((rand() / RAND_MAX) * 2.0 - 1.0);

  n = rad_MaxSurface->normal; // Get patch normal

  do // Get valid u-axis vector
  {
    vm_CrossProduct(&u, &n, &rv);
  } while (vm_GetMagnitude(&u) < .0001);

  vm_NormalizeVector(&u);
  vm_CrossProduct(&v, &u, &n); // Determine v-axis

  rad_Hemicube.head_matrix.rvec = u;
  rad_Hemicube.head_matrix.uvec = v;
  rad_Hemicube.head_matrix.fvec = n;

  vm_VectorToMatrix(&rad_Hemicube.head_matrix, &n, NULL, NULL);

  rad_Hemicube.shooting_surface = surf;
}

// Build transformation matrix for our hemicube
void BuildTransform(vector *nu, vector *nv, vector *nn) {
  matrix *vm = &rad_Hemicube.view_matrix; // view matrix

  if (Shoot_from_patch)
    GetCenterOfSurface(rad_Hemicube.shooting_surface, &rad_Hemicube.view_position);
  else
    GetCenterOfElement(rad_Hemicube.shooting_element, &rad_Hemicube.view_position);

  rad_Hemicube.view_position += (rad_MaxSurface->normal / 16.0);

  vm->fvec = *nn;
  vm->uvec = *nv;
  vm->rvec = *nu;
}

void StartHemicubeDrawing() {
  StartEditorFrame(rad_Hemicube.vport, &rad_Hemicube.view_position, &rad_Hemicube.view_matrix, Hemicube_view_zoom);
}

int surface_colors[9000];
void EndHemicubeDrawing(int face) {
  static int first = 1;

  EndEditorFrame();

  mprintf_at(2, 4, 0, "CTF=%d    ", Cracks_this_frame);
  mprintf_at(2, 5, 0, "CTS=%d    ", Cracks_this_side);

  if (Show_rad_progress) {
    int i, t;
    int key;

    if (first) {
      for (i = 0; i < 9000; i++) {
        int r = (rand() % 127) + 128;
        int g = (rand() % 127) + 128;
        int b = (rand() % 127) + 128;

        surface_colors[i] = GR_RGB(r, g, b);
      }

      first = 0;
    }

    while ((key = ddio_KeyInKey()) != 0)
      ;

    uint16_t surfval[90000];
    int ff_index = 0;

    for (i = 0; i < rad_NumSurfaces; i++) {
      rad_surface *surf = &rad_Surfaces[i];

      for (t = 0; t < surf->yresolution * surf->xresolution; t++, ff_index++) {
        surfval[ff_index] = i;
      }
    }

    for (i = 0; i < rad_Hemicube.ff_res; i++) {
      for (t = 0; t < rad_Hemicube.ff_res; t++) {
        int element_num = rad_Hemicube.id_grid[i * rad_Hemicube.ff_res + t];
        float factor;

        if (face == TOP_FACE)
          factor = GetTopFactor(i, t);
        else
          factor = GetSideFactor(i, t);

        if (element_num == -1)
          rad_Hemicube.id_grid[i * rad_Hemicube.ff_res + t] = -1;
        else {
          int surfnum = surfval[element_num];
          int color = surface_colors[surfnum];

          float norm;

          if (face == TOP_FACE)
            norm = factor / Highest_top_delta;
          else
            norm = factor / Highest_side_delta;

          int r = GR_COLOR_RED(color) * norm;
          int g = GR_COLOR_GREEN(color) * norm;
          int b = GR_COLOR_BLUE(color) * norm;

          rad_Hemicube.id_grid[i * rad_Hemicube.ff_res + t] = GR_RGB(r, g, b);
        }
      }
    }

    ShowRadView();

    while ((key = ddio_KeyInKey()) == 0) {
      ;
    }
  }
}

// Update hemicube view transformation matrix
void UpdateView(int face_id) {
  vector nu, nv, nn; // View space co-ordinates

  switch (face_id) // Exchange co-ordinates
  {
  case TOP_FACE:
    nu = rad_Hemicube.head_matrix.rvec;
    nv = rad_Hemicube.head_matrix.uvec;
    nn = rad_Hemicube.head_matrix.fvec;
    break;
  case FRONT_FACE:
    nu = rad_Hemicube.head_matrix.rvec;
    nv = rad_Hemicube.head_matrix.fvec;
    nn = rad_Hemicube.head_matrix.uvec * -1;
    break;
  case RIGHT_FACE:
    nu = rad_Hemicube.head_matrix.uvec;
    nv = rad_Hemicube.head_matrix.fvec;
    nn = rad_Hemicube.head_matrix.rvec;
    break;
  case BACK_FACE:
    nu = rad_Hemicube.head_matrix.rvec * -1;
    nv = rad_Hemicube.head_matrix.fvec;
    nn = (rad_Hemicube.head_matrix.uvec);
    break;
  case LEFT_FACE:
    nu = (rad_Hemicube.head_matrix.uvec * -1);
    nv = rad_Hemicube.head_matrix.fvec;
    nn = (rad_Hemicube.head_matrix.rvec * -1);
    break;
  default:
    Int3();
    break;
  }

  // Build new view transformation matrix
  BuildTransform(&nu, &nv, &nn);
}

void DrawRadiosityPoly(int nv, g3Point **pointlist, int id) {
  int i;
  g3Codes cc;
  bool was_clipped = 0;
  int triangulate = 1;

  ASSERT(id >= 0 && id <= rad_NumElements);

  if (triangulate) {
    if (nv > 3) {
      g3Point *tripoints[3];

      tripoints[0] = pointlist[0];
      tripoints[2] = pointlist[1];

      for (i = 0; i < nv - 2; i++) {
        tripoints[1] = tripoints[2];
        tripoints[2] = pointlist[2 + i];
        DrawRadiosityPoly(3, tripoints, id);
      }

      return;
    }
  }

  // Initialize
  cc.cc_or = 0;
  cc.cc_and = 0xff;

  // Get codes for this polygon, and copy uvls into points
  for (i = 0; i < nv; i++) {
    uint8_t c;

    c = pointlist[i]->p3_codes;

    cc.cc_and &= c;
    cc.cc_or |= c;
  }

  // All points off grid?
  if (cc.cc_and)
    return;

  // One or more point off screen, so clip
  if (cc.cc_or) {

    // Clip the polygon, getting pointer to new buffer
    pointlist = g3_ClipPolygon(pointlist, &nv, &cc);

    // Flag as clipped so temp points will be freed
    was_clipped = 1;

    // Check for polygon clipped away, or clip otherwise failed
    if ((nv == 0) || (cc.cc_or & CC_BEHIND) || cc.cc_and)
      goto free_points;
  }

  // Make list of 2d coords
  for (i = 0; i < nv; i++) {
    g3Point *p = pointlist[i];
    g3_ProjectPoint(p);
  }

  // Draw!
  ScanRadiosityPoly(pointlist, nv, id);

free_points:;

  // If was clipped, free temp points
  if (was_clipped)
    g3_FreeTempPoints(pointlist, nv);
}

/*void GetVertexOrdering (hemicube_point *t, int nv, int *vlt, int *vlb, int *vrt, int *vrb,int *bottom_y_ind)
{
        int	i;
        float	min_y,max_y;
        int	min_y_ind;
        int	original_vrt;
        float	min_x;

        // Scan all vertices, set min_y_ind to vertex with smallest y coordinate.
        min_y = t[0].sy;
        max_y = min_y;
        min_y_ind = 0;
        min_x = t[0].sx;
        *bottom_y_ind = 0;

        for (i=1; i<nv; i++)
        {
                if (t[i].sy < min_y)
                {
                        min_y = t[i].sy;
                        min_y_ind = i;
                        min_x = t[i].sx;
                }
                else if (t[i].sy == min_y)
                {
                        if (t[i].sx < min_x)
                        {
                                min_y_ind = i;
                                min_x = t[i].sx;
                        }
                }
                if (t[i].sy > max_y)
                {
                        max_y = t[i].sy;
                        *bottom_y_ind = i;
                }
        }


        // Set "vertex left top", etc. based on vertex with topmost y coordinate
        *vlt = min_y_ind;
        *vrt = *vlt;
        *vlb = PrevIndex(*vlt,nv);
        *vrb = NextIndex(*vrt,nv);

        // If right edge is horizontal, then advance along polygon bound until it no longer is or until all
        // vertices have been examined.
        // (Left edge cannot be horizontal, because *vlt is set to leftmost point with highest y coordinate.)

        original_vrt = *vrt;

        while (t[*vrt].sy == t[*vrb].sy)
        {
                if (NextIndex(*vrt,nv) == original_vrt)
                        break;

                *vrt = NextIndex(*vrt,nv);
                *vrb = NextIndex(*vrt,nv);
        }
}*/

void GetVertexOrdering(hemicube_point *t, int nv, int *vlt, int *vlb, int *vrt, int *vrb, float *top_y, float *bottom_y,
                       int *left_edge_dir) {
  int i;
  float min_y, max_y;
  int min_index_l, min_index_r, max_index;

  // Scan all vertices, set min_y_ind to vertex with smallest y coordinate.

  *bottom_y = 0;
  *top_y = 0;

  min_index_l = max_index = 0;
  max_y = min_y = t[0].sy;
  for (i = 1; i < nv; i++) {
    if (t[i].sy < min_y)
      min_y = t[min_index_l = i].sy;
    else if (t[i].sy > max_y)
      max_y = t[max_index = i].sy;
  }
  if (min_y == max_y)
    return;

  // Scan in ascending order to find the last top-edge point */
  min_index_r = min_index_l;
  while (t[min_index_r].sy == min_y)
    min_index_r = NextIndex(min_index_r, nv);
  min_index_r = PrevIndex(min_index_r, nv);

  // Now scan in descending order to find the first top-edge point
  while (t[min_index_l].sy == min_y)
    min_index_l = PrevIndex(min_index_l, nv);
  min_index_l = NextIndex(min_index_l, nv);

  *left_edge_dir = -1;
  if (t[min_index_l].sx != t[min_index_r].sx) {
    // If the top is flat, just see which of the ends is leftmost
    if (t[min_index_l].sx > t[min_index_r].sx) {
      *left_edge_dir = 1;

      int temp = min_index_l;
      min_index_l = min_index_r;
      min_index_r = temp;
    }
  } else {
    // Point to the downward end of the first line of each of the
    // two edges down from the top
    int next_index = min_index_r;
    next_index = NextIndex(next_index, nv);
    int prev_index = min_index_r;
    prev_index = PrevIndex(prev_index, nv);

    /* Calculate X and Y lengths from the top vertex to the end of
       the first line down each edge; use those to compare slopes
       and see which line is leftmost */
    float deltaXN = t[next_index].sx - t[min_index_l].sx;
    float deltaYN = t[next_index].sy - t[min_index_l].sy;
    float deltaXP = t[prev_index].sx - t[min_index_l].sx;
    float deltaYP = t[prev_index].sy - t[min_index_l].sy;
    if (((deltaXN * deltaYP) - (deltaYN * deltaXP)) < 0) {
      *left_edge_dir = 1;

      int temp = min_index_l;
      min_index_l = min_index_r;
      min_index_r = temp;
    }
  }

  *bottom_y = max_y;
  *top_y = min_y;

  *vlt = min_index_l;
  *vrt = min_index_r;

  if (*left_edge_dir == -1) {
    *vlb = PrevIndex(min_index_l, nv);
    *vrb = NextIndex(min_index_r, nv);
  } else {
    *vrb = PrevIndex(min_index_r, nv);
    *vlb = NextIndex(min_index_l, nv);
  }
}

//	Returns number preceding val modulo modulus.
//	prevmod(3,4) = 2
//	prevmod(0,4) = 3
int PrevIndex(int val, int modulus) {
  if (val > 0)
    return val - 1;
  else
    return modulus - 1;
}

//	Returns number succeeding val modulo modulus.
//	succmod(3,4) = 0
//	succmod(0,4) = 1
int NextIndex(int val, int modulus) {
  if (val < modulus - 1)
    return val + 1;
  else
    return 0;
}

void ScanRadiosityPoly(g3Point **pl, int nv, int element_id) {
  int i, left_edge_dir;

  float Delta_right_x, Right_x, Delta_left_x, Left_x;
  float Left_z, Right_z, Delta_left_z, Delta_right_z;

  float height, left_height, right_height;
  int vlt, vlb, vrt, vrb, desty;
  float top_y, bottom_y, next_break_left, next_break_right, y;

  hemicube_point cp[100];

  int destptr;

  ASSERT(element_id >= 0 && element_id <= rad_NumElements);

  for (i = 0; i < nv; i++) {
    g3Point p;
    p = *pl[i];

    cp[i].sx = p.p3_sx;
    cp[i].sy = p.p3_sy;
    cp[i].z = 1.0 / (float)p.p3_vec.z;
  }

  // Determine top and bottom y coords.
  GetVertexOrdering(cp, nv, &vlt, &vlb, &vrt, &vrb, &top_y, &bottom_y, &left_edge_dir);

  height = bottom_y - top_y;
  if (height < 1.0)
    return;

#include "radscan_leftedge.h"
#include "radscan_rightedge.h"

  // Set the destptr to equal the first row to draw to
  destptr = ((int)top_y * rad_Hemicube.ff_res);
  desty = (int)top_y;

  for (y = top_y; y < bottom_y; y++) {
    if (y >= next_break_left) {
      do {
        vlt = vlb;

        if (left_edge_dir == -1)
          vlb = PrevIndex(vlb, nv);
        else
          vlb = NextIndex(vlb, nv);
      } while (y == cp[vlb].sy);

#include "radscan_leftedge.h"
    }

    if (y >= next_break_right) {
      do {
        vrt = vrb;
        if (left_edge_dir == -1)
          vrb = NextIndex(vrb, nv);
        else
          vrb = PrevIndex(vrb, nv);

      } while (y == cp[vrb].sy);
#include "radscan_rightedge.h"
    }

    // Draw a scanline

    float sl, sr;
    float lz, rz;

    sl = Left_x;
    sr = Right_x;

    lz = Left_z;
    rz = Right_z;

    int x1 = sl;
    int width = (sr - x1) + 1;

    if (desty < 0 || desty >= rad_Hemicube.ff_res)
      goto UpdateExtents;

    if (x1 < 0 || x1 >= rad_Hemicube.ff_res)
      goto UpdateExtents;
    if ((x1 + width) > rad_Hemicube.ff_res) {
      width = (rad_Hemicube.ff_res - x1);
    }

    if (width > 0) {
      float z = lz;
      float dz = (rz - z) / width;

      // Enter scan line
      for (int x = x1; x < x1 + width; x++) {
        float realz = 1.0 / z;
        // Check element visibility
        if (realz <= rad_Hemicube.depth_grid[destptr + x]) {
          // Update Z-buffer
          rad_Hemicube.depth_grid[destptr + x] = realz;

          //	Set polygon identifier
          rad_Hemicube.id_grid[destptr + x] = element_id;
        }
        // Update element pseudodepth
        z += dz;
      }
    }

  UpdateExtents:

    destptr += rad_Hemicube.ff_res;
    desty++;

    Left_x += Delta_left_x;
    Right_x += Delta_right_x;
    Right_z += Delta_right_z;
    Left_z += Delta_left_z;
  }
}

float GetTopFactor(int row, int col) {
  if (row >= rad_Hemicube.grid_dim)
    row -= rad_Hemicube.grid_dim;
  else
    row = rad_Hemicube.grid_dim - row - 1;

  if (col >= rad_Hemicube.grid_dim)
    col -= rad_Hemicube.grid_dim;
  else
    col = rad_Hemicube.grid_dim - col - 1;

  return rad_Hemicube.top_array[row * rad_Hemicube.grid_dim + col];
}

// Get side face cell form factor
float GetSideFactor(int row, int col) {
  if (col >= rad_Hemicube.grid_dim)
    col -= rad_Hemicube.grid_dim;
  else
    col = rad_Hemicube.grid_dim - col - 1;

  if (row >= rad_Hemicube.grid_dim)
    return 0.0f;
  else
    row = rad_Hemicube.grid_dim - row - 1;

  return rad_Hemicube.side_array[(row * rad_Hemicube.grid_dim) + col];
}

// Sums the delta form factors
void SumDeltas(float *ff_array, int face_id) {
  int poly_id;  // Polygon identifier
  int row, col; // Face cell indices

  Cracks_this_side = 0;

  if (face_id == TOP_FACE) {
    // Scan entire face buffer
    for (row = 0; row < rad_Hemicube.ff_res; row++) {
      for (col = 0; col < rad_Hemicube.ff_res; col++) {
        poly_id = rad_Hemicube.id_grid[row * rad_Hemicube.ff_res + col];
        if (poly_id != -1) {
          if (Shoot_from_patch)
            ff_array[poly_id] += (GetTopFactor(row, col));
          else
            ff_array[poly_id] += (GetTopFactor(row, col) * (rad_MaxElement->area / rad_MaxSurface->area));
        } else {
          Cracks_this_frame++;
          Cracks_this_side++;
        }
      }
    }
  } else {
    // Scan upper half of face buffer only
    for (row = 0; row < rad_Hemicube.grid_dim; row++) {
      for (col = 0; col < rad_Hemicube.ff_res; col++) {
        poly_id = rad_Hemicube.id_grid[row * rad_Hemicube.ff_res + col];
        if (poly_id != -1) {
          if (Shoot_from_patch)
            ff_array[poly_id] += GetSideFactor(row, col);
          else
            ff_array[poly_id] += (GetSideFactor(row, col) * (rad_MaxElement->area / rad_MaxSurface->area));
        } else {
          Cracks_this_frame++;
          Cracks_this_side++;
        }
      }
    }
  }
}
