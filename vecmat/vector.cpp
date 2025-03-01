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

 * $Logfile: /DescentIII/Main/vecmat/vector.cpp $
 * $Revision: 26 $
 * $Date: 4/19/00 5:28p $
 * $Author: Matt $
 *
 *  Vector/Matrix functions
 *
 * $Log: /DescentIII/Main/vecmat/vector.cpp $
 *
 * 26    4/19/00 5:28p Matt
 * From Duane for 1.4
 * Mac-only optimizations
 *
 * 25    5/10/99 3:18a Matt
 * Fixed extract angles from matrix, which was totally bogus when the
 * forward vector was straight up or down.
 *
 * 24    4/21/99 11:06a Kevin
 * new ps_rand and ps_srand to replace rand & srand
 *
 * 23    2/19/99 4:26p Jason
 * more work on Katmai support
 *
 * 22    2/16/99 12:36a Kevin
 * Fixes for release builds of OEM V3 and KAtmai
 *
 * 21    1/11/99 4:45p Jason
 * added first pass at katmai support
 *
 * 20    1/01/99 4:10p Chris
 * Added some const parameters, improved ray cast object collide/rejection
 * code
 *
 * 19    6/15/98 7:00a Chris
 * Added vm_SinCos().
 *
 * 18    6/03/98 6:50p Chris
 * Fixed some infinity bugs
 *
 * 17    6/03/98 6:42p Chris
 * Added multipoint collision detection an Assert on invalid (infinite
 * endpoint).
 *
 * 16    5/25/98 3:45p Jason
 * added vm_GetCentroidFast
 *
 * 15    3/12/98 7:30p Chris
 * Added ObjSetOrient
 *
 * 14    2/08/98 6:01p Matt
 * Added functions to multiply by a transposed matrix, and simplified some
 * other code a bit.
 *
 * 13    2/06/98 10:57a Matt
 * Made vm_VectorToMatrix() take any one or two vectors, & not require
 * forward vec.
 * Also, made the uvec and rvec parameters default to NULL if not
 * specified.
 *
 * 12    2/02/98 8:17p Chris
 * Added a != operator and a Zero_vector constant
 *
 * 11    1/20/98 4:04p Matt
 * Made vm_GetNormalizedDir() and vm_GetNormalizeDirFast() return the
 * distance between the two input points.
 *
 * 10    1/13/98 1:30p Jason
 * changed vm_GetCentroid to also return the size of the area
 *
 * 9     11/04/97 6:21p Chris
 * Allowed other files to use the vm_DeltaAngVecNorm function
 *
 * 8     10/25/97 7:15p Jason
 * implemented vm_ComputeBoundingSphere
 *
 * 7     10/14/97 4:35p Samir
 * Added vm_MakeRandomVector.
 *
 * 6     9/23/97 2:26p Matt
 * Made vm_GetNormal() return the magnitude of the normal (before it was
 * normalized)
 *
 * 5     8/28/97 4:56p Jason
 * implemented vm_GetCentroid
 *
 * 4     8/21/97 7:09p Matt
 * Made  vm_VectorAngleToMatrix() work when forward vector was straight up
 * or down
 *
 * 3     8/18/97 4:45p Matt
 * Added vm_VectorAngleToMatrix() to vecmat library, and removed copy from
 * HCurves.cpp
 *
 * 2     7/17/97 3:56p Matt
 * Added vm_Orthogonalize()
 *
 * 25    5/20/97 5:52p Jason
 * tweaked a couple of things with magnitude division
 *
 * 24    4/18/97 2:14p Samir
 * Added vm_DeltaAngVec.
 *
 * 23    2/28/97 3:33 PM Jeremy
 * put pserror.h in "" instead of <> since it's one of
 *
 * 22    2/27/97 1:40p Chris
 * Added a function to compute the determinate --
 * BTW on the last rev. I moved all inline functions
 * to the header.  (So they will be inlined)
 *
 * 21    2/26/97 7:33p Chris
 *
 * 20    2/20/97 11:41a Chris
 * Added a negate unary operator for vectors
 *
 * 19    2/12/97 5:28p Jason
 * implemented ExtractAnglesFromMatrix function
 *
 * 18    2/11/97 6:49p Matt
 * Added vm_VectorToMatrix()
 * Made vm_NormalizeVector() return the old vector mag
 * Fixed bug in inline version of crossprod
 *
 * 17    2/07/97 5:38p Matt
 * Moved fixed-point math funcs to fix.lib
 *
 * $NoKeywords: $
 */

#include <cmath>
#include <cstring>

#include "vecmat.h"
#include "mono.h"
#include "pserror.h"
#include "psrand.h"

const vector Zero_vector = vector::ne();
const matrix Identity_matrix = IDENTITY_MATRIX;

void vm_AverageVector(vector *a, int num) {
  // Averages a vector.  ie divides each component of vector a by num
  ASSERT(num != 0);
  a->x = a->x / (scalar)num;
  a->y = a->y / (scalar)num;
  a->z = a->z / (scalar)num;
}

void vm_AddVectors(vector *result, vector *a, vector *b) {
  // Adds two vectors.  Either source can equal dest

  result->x = a->x + b->x;
  result->y = a->y + b->y;
  result->z = a->z + b->z;
}

void vm_SubVectors(vector *result, const vector *a, const vector *b) {
  // Subtracts second vector from first.  Either source can equal dest

  result->x = a->x - b->x;
  result->y = a->y - b->y;
  result->z = a->z - b->z;
}

scalar vm_VectorDistance(const vector *a, const vector *b) {
  // Given two vectors, returns the distance between them

  vector dest;
  scalar dist;

  vm_SubVectors(&dest, a, b);
  dist = vm_GetMagnitude(&dest);
  return dist;
}
scalar vm_VectorDistanceQuick(vector *a, vector *b) {
  // Given two vectors, returns the distance between them

  vector dest;
  scalar dist;

  vm_SubVectors(&dest, a, b);
  dist = vm_GetMagnitudeFast(&dest);
  return dist;
}

// Calculates the perpendicular vector given three points
// Parms:	n - the computed perp vector (filled in)
//			v0,v1,v2 - three clockwise vertices
void vm_GetPerp(vector *n, vector *a, vector *b, vector *c) {
  // Given 3 vertices, return the surface normal in n
  // IMPORTANT: B must be the 'corner' vertex

  vector x, y;

  vm_SubVectors(&x, b, a);
  vm_SubVectors(&y, c, b);

  vm_CrossProduct(n, &x, &y);
}

// Calculates the (normalized) surface normal give three points
// Parms:	n - the computed surface normal (filled in)
//			v0,v1,v2 - three clockwise vertices
// Returns the magnitude of the normal before it was normalized.
// The bigger this value, the better the normal.
scalar vm_GetNormal(vector *n, vector *v0, vector *v1, vector *v2) {
  vm_GetPerp(n, v0, v1, v2);

  return vm_NormalizeVector(n);
}

// Does a simple dot product calculation
scalar vm_DotProduct(const vector *u, const vector *v) { return (u->x * v->x) + (u->y * v->y) + (u->z * v->z); }

// Scales all components of vector v by value s and stores result in vector d
// dest can equal source
void vm_ScaleVector(vector *d, vector *v, scalar s) {
  d->x = (v->x * s);
  d->y = (v->y * s);
  d->z = (v->z * s);
}

void vm_ScaleAddVector(vector *d, vector *p, vector *v, scalar s) {
  // Scales all components of vector v by value s
  // adds the result to p and stores result in vector d
  // dest can equal source

  d->x = p->x + (v->x * s);
  d->y = p->y + (v->y * s);
  d->z = p->z + (v->z * s);
}

void vm_DivVector(vector *dest, vector *src, scalar n) {
  // Divides a vector into n portions
  // Dest can equal src

  ASSERT(n != 0);
  dest->x = src->x / n;
  dest->y = src->y / n;
  dest->z = src->z / n;
}

void vm_CrossProduct(vector *dest, vector *u, vector *v) {
  // Computes a cross product between u and v, returns the result
  //	in Normal.  Dest cannot equal source.

  dest->x = (u->y * v->z) - (u->z * v->y);
  dest->y = (u->z * v->x) - (u->x * v->z);
  dest->z = (u->x * v->y) - (u->y * v->x);
}

// Normalize a vector.
// Returns:  the magnitude before normalization
scalar vm_NormalizeVector(vector *a) {
  scalar mag;

  mag = vm_GetMagnitude(a);

  if (mag > 0)
    *a /= mag;
  else {
    *a = Zero_vector;
    a->x = 1.0;
    mag = 0.0f;
  }

  return mag;
}

scalar vm_GetMagnitude(vector *a) {
  scalar f;

  f = (a->x * a->x) + (a->y * a->y) + (a->z * a->z);

  return (sqrt(f));
}

void vm_ClearMatrix(matrix *dest) { memset(dest, 0, sizeof(matrix)); }

void vm_MakeIdentity(matrix *dest) {
  memset(dest, 0, sizeof(matrix));
  dest->rvec.x = dest->uvec.y = dest->fvec.z = 1.0;
}
void vm_MakeInverseMatrix(matrix *dest) {
  memset((void *)dest, 0, sizeof(matrix));
  dest->rvec.x = dest->uvec.y = dest->fvec.z = -1.0;
}

void vm_TransposeMatrix(matrix *m) {
  // Transposes a matrix in place

  scalar t;

  t = m->uvec.x;
  m->uvec.x = m->rvec.y;
  m->rvec.y = t;
  t = m->fvec.x;
  m->fvec.x = m->rvec.z;
  m->rvec.z = t;
  t = m->fvec.y;
  m->fvec.y = m->uvec.z;
  m->uvec.z = t;
}

void vm_MatrixMulVector(vector *result, vector *v, matrix *m) {
  // Rotates a vector thru a matrix

  ASSERT(result != v);

  result->x = *v * m->rvec;
  result->y = *v * m->uvec;
  result->z = *v * m->fvec;
}

// Multiply a vector times the transpose of a matrix
void vm_VectorMulTMatrix(vector *result, vector *v, matrix *m) {
  ASSERT(result != v);

  result->x = vm_Dot3Vector(m->rvec.x, m->uvec.x, m->fvec.x, v);
  result->y = vm_Dot3Vector(m->rvec.y, m->uvec.y, m->fvec.y, v);
  result->z = vm_Dot3Vector(m->rvec.z, m->uvec.z, m->fvec.z, v);
}

void vm_MatrixMul(matrix *dest, matrix *src0, matrix *src1) {
  // For multiplying two 3x3 matrices together

  ASSERT((dest != src0) && (dest != src1));

  dest->rvec.x = vm_Dot3Vector(src0->rvec.x, src0->uvec.x, src0->fvec.x, &src1->rvec);
  dest->uvec.x = vm_Dot3Vector(src0->rvec.x, src0->uvec.x, src0->fvec.x, &src1->uvec);
  dest->fvec.x = vm_Dot3Vector(src0->rvec.x, src0->uvec.x, src0->fvec.x, &src1->fvec);

  dest->rvec.y = vm_Dot3Vector(src0->rvec.y, src0->uvec.y, src0->fvec.y, &src1->rvec);
  dest->uvec.y = vm_Dot3Vector(src0->rvec.y, src0->uvec.y, src0->fvec.y, &src1->uvec);
  dest->fvec.y = vm_Dot3Vector(src0->rvec.y, src0->uvec.y, src0->fvec.y, &src1->fvec);

  dest->rvec.z = vm_Dot3Vector(src0->rvec.z, src0->uvec.z, src0->fvec.z, &src1->rvec);
  dest->uvec.z = vm_Dot3Vector(src0->rvec.z, src0->uvec.z, src0->fvec.z, &src1->uvec);
  dest->fvec.z = vm_Dot3Vector(src0->rvec.z, src0->uvec.z, src0->fvec.z, &src1->fvec);
}

// Multiply a matrix times the transpose of a matrix
void vm_MatrixMulTMatrix(matrix *dest, matrix *src0, matrix *src1) {
  // For multiplying two 3x3 matrices together

  ASSERT((dest != src0) && (dest != src1));

  dest->rvec.x = src0->rvec.x * src1->rvec.x + src0->uvec.x * src1->uvec.x + src0->fvec.x * src1->fvec.x;
  dest->uvec.x = src0->rvec.x * src1->rvec.y + src0->uvec.x * src1->uvec.y + src0->fvec.x * src1->fvec.y;
  dest->fvec.x = src0->rvec.x * src1->rvec.z + src0->uvec.x * src1->uvec.z + src0->fvec.x * src1->fvec.z;

  dest->rvec.y = src0->rvec.y * src1->rvec.x + src0->uvec.y * src1->uvec.x + src0->fvec.y * src1->fvec.x;
  dest->uvec.y = src0->rvec.y * src1->rvec.y + src0->uvec.y * src1->uvec.y + src0->fvec.y * src1->fvec.y;
  dest->fvec.y = src0->rvec.y * src1->rvec.z + src0->uvec.y * src1->uvec.z + src0->fvec.y * src1->fvec.z;

  dest->rvec.z = src0->rvec.z * src1->rvec.x + src0->uvec.z * src1->uvec.x + src0->fvec.z * src1->fvec.x;
  dest->uvec.z = src0->rvec.z * src1->rvec.y + src0->uvec.z * src1->uvec.y + src0->fvec.z * src1->fvec.y;
  dest->fvec.z = src0->rvec.z * src1->rvec.z + src0->uvec.z * src1->uvec.z + src0->fvec.z * src1->fvec.z;
}

matrix operator*(matrix src0, matrix src1) {
  // For multiplying two 3x3 matrices together
  matrix dest;

  dest.rvec.x = vm_Dot3Vector(src0.rvec.x, src0.uvec.x, src0.fvec.x, &src1.rvec);
  dest.uvec.x = vm_Dot3Vector(src0.rvec.x, src0.uvec.x, src0.fvec.x, &src1.uvec);
  dest.fvec.x = vm_Dot3Vector(src0.rvec.x, src0.uvec.x, src0.fvec.x, &src1.fvec);

  dest.rvec.y = vm_Dot3Vector(src0.rvec.y, src0.uvec.y, src0.fvec.y, &src1.rvec);
  dest.uvec.y = vm_Dot3Vector(src0.rvec.y, src0.uvec.y, src0.fvec.y, &src1.uvec);
  dest.fvec.y = vm_Dot3Vector(src0.rvec.y, src0.uvec.y, src0.fvec.y, &src1.fvec);

  dest.rvec.z = vm_Dot3Vector(src0.rvec.z, src0.uvec.z, src0.fvec.z, &src1.rvec);
  dest.uvec.z = vm_Dot3Vector(src0.rvec.z, src0.uvec.z, src0.fvec.z, &src1.uvec);
  dest.fvec.z = vm_Dot3Vector(src0.rvec.z, src0.uvec.z, src0.fvec.z, &src1.fvec);

  return dest;
}

matrix operator*=(matrix &src0, matrix src1) { return (src0 = src0 * src1); }

// Computes a normalized direction vector between two points
// Parameters:	dest - filled in with the normalized direction vector
//					start,end - the start and end points used to calculate the vector
// Returns:		the distance between the two input points
scalar vm_GetNormalizedDir(vector *dest, vector *end, vector *start) {
  vm_SubVectors(dest, end, start);
  return vm_NormalizeVector(dest);
}

// Returns a normalized direction vector between two points
// Just like vm_GetNormalizedDir(), but uses sloppier magnitude, less precise
// Parameters:	dest - filled in with the normalized direction vector
//					start,end - the start and end points used to calculate the vector
// Returns:		the distance between the two input points
scalar vm_GetNormalizedDirFast(vector *dest, vector *end, vector *start) {
  vm_SubVectors(dest, end, start);
  return vm_NormalizeVectorFast(dest);
}

scalar vm_GetMagnitudeFast(vector *v) {
  scalar a, b, c, bc;

  a = fabs(v->x);
  b = fabs(v->y);
  c = fabs(v->z);

  if (a < b) {
    scalar t = a;
    a = b;
    b = t;
  }

  if (b < c) {
    scalar t = b;
    b = c;
    c = t;

    if (a < b) {
      scalar t = a;
      a = b;
      b = t;
    }
  }

  bc = (b / 4) + (c / 8);

  return a + bc + (bc / 2);
}

// Normalize a vector using an approximation of the magnitude
// Returns:  the magnitude before normalization
scalar vm_NormalizeVectorFast(vector *a) {
  scalar mag;

  mag = vm_GetMagnitudeFast(a);

  if (mag == 0.0) {
    a->x = a->y = a->z = 0.0;
    return 0;
  }

  a->x = (a->x / mag);
  a->y = (a->y / mag);
  a->z = (a->z / mag);

  return mag;
}

// Computes the distance from a point to a plane.
// Parms:	checkp - the point to check
// Parms:	norm - the (normalized) surface normal of the plane
//				planep - a point on the plane
// Returns:	The signed distance from the plane; negative dist is on the back of the plane
scalar vm_DistToPlane(vector *checkp, vector *norm, vector *planep) {
  vector t;

  t = *checkp - *planep;

  return t * *norm;
}

scalar vm_GetSlope(scalar x1, scalar y1, scalar x2, scalar y2) {
  // returns the slope of a line
  scalar r;

  if (y2 - y1 == 0)
    return (0.0);

  r = (x2 - x1) / (y2 - y1);
  return (r);
}

void vm_SinCosToMatrix(matrix *m, scalar sinp, scalar cosp, scalar sinb, scalar cosb, scalar sinh, scalar cosh) {
  scalar sbsh, cbch, cbsh, sbch;

  sbsh = (sinb * sinh);
  cbch = (cosb * cosh);
  cbsh = (cosb * sinh);
  sbch = (sinb * cosh);

  m->rvec.x = cbch + (sinp * sbsh); // m1
  m->uvec.z = sbsh + (sinp * cbch); // m8

  m->uvec.x = (sinp * cbsh) - sbch; // m2
  m->rvec.z = (sinp * sbch) - cbsh; // m7

  m->fvec.x = (sinh * cosp); // m3
  m->rvec.y = (sinb * cosp); // m4
  m->uvec.y = (cosb * cosp); // m5
  m->fvec.z = (cosh * cosp); // m9

  m->fvec.y = -sinp; // m6
}

void vm_AnglesToMatrix(matrix *m, angle p, angle h, angle b) {
  scalar sinp, cosp, sinb, cosb, sinh, cosh;

  sinp = FixSin(p);
  cosp = FixCos(p);
  sinb = FixSin(b);
  cosb = FixCos(b);
  sinh = FixSin(h);
  cosh = FixCos(h);

  vm_SinCosToMatrix(m, sinp, cosp, sinb, cosb, sinh, cosh);
}

// Computes a matrix from a vector and and angle of rotation around that vector
// Parameters:	m - filled in with the computed matrix
//					v - the forward vector of the new matrix
//					a - the angle of rotation around the forward vector
void vm_VectorAngleToMatrix(matrix *m, vector *v, angle a) {
  scalar sinb, cosb, sinp, cosp, sinh, cosh;

  sinb = FixSin(a);
  cosb = FixCos(a);

  sinp = -v->y;
  cosp = sqrt(1.0 - (sinp * sinp));

  if (cosp != 0.0) {
    sinh = v->x / cosp;
    cosh = v->z / cosp;
  } else {
    sinh = 0;
    cosh = 1.0;
  }

  vm_SinCosToMatrix(m, sinp, cosp, sinb, cosb, sinh, cosh);
}

// Ensure that a matrix is orthogonal
void vm_Orthogonalize(matrix *m) {
  // Normalize forward vector
  if (vm_NormalizeVector(&m->fvec) == 0) {
    Int3(); // forward vec should not be zero-length
    return;
  }

  // Generate right vector from forward and up vectors
  m->rvec = m->uvec ^ m->fvec;

  // Normaize new right vector
  if (vm_NormalizeVector(&m->rvec) == 0) {
    vm_VectorToMatrix(m, &m->fvec, NULL, NULL); // error, so generate from forward vector only
    return;
  }

  // Recompute up vector, in case it wasn't entirely perpendiclar
  m->uvec = m->fvec ^ m->rvec;
}

// do the math for vm_VectorToMatrix()
void DoVectorToMatrix(matrix *m, vector *fvec, vector *uvec, vector *rvec) {
  vector *xvec = &m->rvec, *yvec = &m->uvec, *zvec = &m->fvec;

  ASSERT(fvec != NULL);

  *zvec = *fvec;
  if (vm_NormalizeVector(zvec) == 0) {
    Int3(); // forward vec should not be zero-length
    return;
  }

  if (uvec == NULL) {

    if (rvec == NULL) { // just forward vec

    bad_vector2:;

      if (zvec->x == 0 && zvec->z == 0) { // forward vec is straight up or down

        m->rvec.x = 1.0;
        m->uvec.z = (zvec->y < 0) ? 1.0 : -1.0;

        m->rvec.y = m->rvec.z = m->uvec.x = m->uvec.y = 0;
      } else { // not straight up or down

        xvec->x = zvec->z;
        xvec->y = 0;
        xvec->z = -zvec->x;

        vm_NormalizeVector(xvec);

        *yvec = *zvec ^ *xvec;
      }

    } else { // use right vec

      *xvec = *rvec;
      if (vm_NormalizeVector(xvec) == 0)
        goto bad_vector2;

      *yvec = *zvec ^ *xvec;

      // normalize new perpendicular vector
      if (vm_NormalizeVector(yvec) == 0)
        goto bad_vector2;

      // now recompute right vector, in case it wasn't entirely perpendiclar
      *xvec = *yvec ^ *zvec;
    }
  } else { // use up vec

    *yvec = *uvec;
    if (vm_NormalizeVector(yvec) == 0)
      goto bad_vector2;

    *xvec = *yvec ^ *zvec;

    // normalize new perpendicular vector
    if (vm_NormalizeVector(xvec) == 0)
      goto bad_vector2;

    // now recompute up vector, in case it wasn't entirely perpendiclar
    *yvec = *zvec ^ *xvec;
  }
}

// Compute a matrix from one or two vectors.  At least one and at most two vectors must/can be specified.
// Parameters:	m - filled in with the orienation matrix
//					fvec,uvec,rvec - pointers to vectors that determine the matrix.
//						One or two of these must be specified, with the other(s) set to NULL.
void vm_VectorToMatrix(matrix *m, vector *fvec, vector *uvec, vector *rvec) {
  if (!fvec) { // no forward vector.  Use up and/or right vectors.
    matrix tmatrix;

    if (uvec) { // got up vector. use up and, if specified, right vectors.
      DoVectorToMatrix(&tmatrix, uvec, NULL, rvec);
      m->fvec = -tmatrix.uvec;
      m->uvec = tmatrix.fvec;
      m->rvec = tmatrix.rvec;
      return;
    } else { // no up vector.  Use right vector only.
      ASSERT(rvec);
      DoVectorToMatrix(&tmatrix, rvec, NULL, NULL);
      m->fvec = -tmatrix.rvec;
      m->uvec = tmatrix.uvec;
      m->rvec = tmatrix.fvec;
      return;
    }
  } else {
    ASSERT(!(uvec && rvec)); // can only have 1 or 2 vectors specified
    DoVectorToMatrix(m, fvec, uvec, rvec);
  }
}

void vm_SinCos(uint16_t a, scalar *s, scalar *c) {
  if (s)
    *s = FixSin(a);
  if (c)
    *c = FixCos(a);
}

#define EPSILON 0.00001
#define IS_ZERO(x) (fabs(x) < EPSILON)

// extract angles from a matrix
angvec *vm_ExtractAnglesFromMatrix(angvec *a, matrix *m) {
  scalar sinh, cosh, cosp, sinb, cosb;

  // Deal with straight up or straight down
  if (IS_ZERO(m->fvec.x) && IS_ZERO(m->fvec.z)) {
    a->p = (m->fvec.y > 0) ? 0xc000 : 0x4000;
    a->b = 0.0;
    a->h = FixAtan2(m->rvec.x, -m->rvec.z);
    return a;
  }

  a->h = FixAtan2(m->fvec.z, m->fvec.x);

  sinh = FixSin(a->h);
  cosh = FixCos(a->h);

  if (fabs(sinh) > fabs(cosh)) // sine is larger, so use it
    cosp = (m->fvec.x / sinh);
  else // cosine is larger, so use it
    cosp = (m->fvec.z / cosh);
  a->p = FixAtan2(cosp, -m->fvec.y);

  sinb = (m->rvec.y / cosp);
  cosb = (m->uvec.y / cosp);
  a->b = FixAtan2(cosb, sinb);

  return a;
}

// returns the value of a determinant
scalar calc_det_value(matrix *det) {
  return det->rvec.x * det->uvec.y * det->fvec.z - det->rvec.x * det->uvec.z * det->fvec.y -
         det->rvec.y * det->uvec.x * det->fvec.z + det->rvec.y * det->uvec.z * det->fvec.x +
         det->rvec.z * det->uvec.x * det->fvec.y - det->rvec.z * det->uvec.y * det->fvec.x;
}

// computes the delta angle between two vectors.
// vectors need not be normalized. if they are, call vm_vec_delta_ang_norm()
// the forward vector (third parameter) can be NULL, in which case the absolute
// value of the angle in returned.  Otherwise the angle around that vector is
// returned.

angle vm_DeltaAngVec(vector *v0, vector *v1, vector *fvec) {
  vector t0, t1;

  t0 = *v0;
  t1 = *v1;

  vm_NormalizeVector(&t0);
  vm_NormalizeVector(&t1);

  return vm_DeltaAngVecNorm(&t0, &t1, fvec);
}

// computes the delta angle between two normalized vectors.
angle vm_DeltaAngVecNorm(vector *v0, vector *v1, vector *fvec) {
  angle a;

  a = FixAcos(vm_DotProduct(v0, v1));

  if (fvec) {
    vector t;

    vm_CrossProduct(&t, v0, v1);
    if (vm_DotProduct(&t, fvec) < 0)
      a = -a;
  }

  return a;
}

// Gets the real center of a polygon
// Returns the size of the passed in stuff
scalar vm_GetCentroid(vector *centroid, vector *src, int nv) {
  ASSERT(nv > 2);
  vector normal;
  scalar area, total_area;
  int i;
  vector tmp_center;

  vm_MakeZero(centroid);

  // First figure out the total area of this polygon
  vm_GetPerp(&normal, &src[0], &src[1], &src[2]);
  total_area = (vm_GetMagnitude(&normal) / 2);

  for (i = 2; i < nv - 1; i++) {
    vm_GetPerp(&normal, &src[0], &src[i], &src[i + 1]);
    area = (vm_GetMagnitude(&normal) / 2);
    total_area += area;
  }

  // Now figure out how much weight each triangle represents to the overall
  // polygon
  vm_GetPerp(&normal, &src[0], &src[1], &src[2]);
  area = (vm_GetMagnitude(&normal) / 2);

  // Get the center of the first polygon
  vm_MakeZero(&tmp_center);
  for (i = 0; i < 3; i++) {
    tmp_center += src[i];
  }
  tmp_center /= 3;

  *centroid += (tmp_center * (area / total_area));

  // Now do the same for the rest
  for (i = 2; i < nv - 1; i++) {
    vm_GetPerp(&normal, &src[0], &src[i], &src[i + 1]);
    area = (vm_GetMagnitude(&normal) / 2);

    vm_MakeZero(&tmp_center);

    tmp_center += src[0];
    tmp_center += src[i];
    tmp_center += src[i + 1];

    tmp_center /= 3;

    *centroid += (tmp_center * (area / total_area));
  }

  return total_area;
}

// Gets the real center of a polygon, but uses fast magnitude calculation
// Returns the size of the passed in stuff
scalar vm_GetCentroidFast(vector *centroid, vector *src, int nv) {
  ASSERT(nv > 2);
  vector normal;
  scalar area, total_area;
  int i;
  vector tmp_center;

  vm_MakeZero(centroid);

  // First figure out the total area of this polygon
  vm_GetPerp(&normal, &src[0], &src[1], &src[2]);
  total_area = (vm_GetMagnitudeFast(&normal) / 2);

  for (i = 2; i < nv - 1; i++) {
    vm_GetPerp(&normal, &src[0], &src[i], &src[i + 1]);
    area = (vm_GetMagnitudeFast(&normal) / 2);
    total_area += area;
  }

  // Now figure out how much weight each triangle represents to the overall
  // polygon
  vm_GetPerp(&normal, &src[0], &src[1], &src[2]);
  area = (vm_GetMagnitudeFast(&normal) / 2);

  // Get the center of the first polygon
  vm_MakeZero(&tmp_center);
  for (i = 0; i < 3; i++) {
    tmp_center += src[i];
  }
  tmp_center /= 3;

  *centroid += (tmp_center * (area / total_area));

  // Now do the same for the rest
  for (i = 2; i < nv - 1; i++) {
    vm_GetPerp(&normal, &src[0], &src[i], &src[i + 1]);
    area = (vm_GetMagnitudeFast(&normal) / 2);

    vm_MakeZero(&tmp_center);

    tmp_center += src[0];
    tmp_center += src[i];
    tmp_center += src[i + 1];

    tmp_center /= 3;

    *centroid += (tmp_center * (area / total_area));
  }

  return total_area;
}

//	creates a completely random, non-normalized vector with a range of values from -1023 to +1024 values)
void vm_MakeRandomVector(vector *vec) {
  vec->x = ps_rand() - D3_RAND_MAX / 2;
  vec->y = ps_rand() - D3_RAND_MAX / 2;
  vec->z = ps_rand() - D3_RAND_MAX / 2;
}

// Given a set of points, computes the minimum bounding sphere of those points
scalar vm_ComputeBoundingSphere(vector *center, vector *vecs, int num_verts) {
  // This algorithm is from Graphics Gems I.  There's a better algorithm in Graphics Gems III that
  // we should probably implement sometime.

  vector *min_x, *max_x, *min_y, *max_y, *min_z, *max_z, *vp;
  scalar dx, dy, dz;
  scalar rad, rad2;
  int i;

  // Initialize min, max vars
  min_x = max_x = min_y = max_y = min_z = max_z = &vecs[0];

  // First, find the points with the min & max x,y, & z coordinates
  for (i = 0, vp = vecs; i < num_verts; i++, vp++) {

    if (vp->x < min_x->x)
      min_x = vp;

    if (vp->x > max_x->x)
      max_x = vp;

    if (vp->y < min_y->y)
      min_y = vp;

    if (vp->y > max_y->y)
      max_y = vp;

    if (vp->z < min_z->z)
      min_z = vp;

    if (vp->z > max_z->z)
      max_z = vp;
  }

  // Calculate initial sphere

  dx = vm_VectorDistance(min_x, max_x);
  dy = vm_VectorDistance(min_y, max_y);
  dz = vm_VectorDistance(min_z, max_z);

  if (dx > dy)
    if (dx > dz) {
      *center = (*min_x + *max_x) / 2;
      rad = dx / 2;
    } else {
      *center = (*min_z + *max_z) / 2;
      rad = dz / 2;
    }
  else if (dy > dz) {
    *center = (*min_y + *max_y) / 2;
    rad = dy / 2;
  } else {
    *center = (*min_z + *max_z) / 2;
    rad = dz / 2;
  }

  // Go through all points and look for ones that don't fit
  rad2 = rad * rad;
  for (i = 0, vp = vecs; i < num_verts; i++, vp++) {
    vector delta;
    scalar t2;

    delta = *vp - *center;
    t2 = delta.x * delta.x + delta.y * delta.y + delta.z * delta.z;

    // If point outside, make the sphere bigger
    if (t2 > rad2) {
      scalar t;

      t = sqrt(t2);
      rad = (rad + t) / 2;
      rad2 = rad * rad;
      *center += delta * (t - rad) / t;
    }
  }

  // We're done
  return rad;
}
