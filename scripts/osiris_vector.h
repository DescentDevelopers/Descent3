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

#ifndef OSIRIS_VECTOR_H
#define OSIRIS_VECTOR_H

#include <math.h>
#include <time.h>

#include "vecmat_external.h"

const vector Zero_vector = {0.0f, 0.0f, 0.0f};

// Disable the "possible loss of data" warning
#pragma warning(disable : 4244)

// Angles are unsigned shorts
typedef unsigned short angle;

// The basic fixed-point type
typedef long fix;

#define PI 3.141592654

// Constants for converted between fix and float
#define FLOAT_SCALER 65536.0
#define FIX_SHIFT 16

// 1.0 in fixed-point
#define F1_0 (1 << FIX_SHIFT)

// Generate the data for the trig tables.  Must be called before trig functions
void InitMathTables();

// Returns the sine of the given angle.  Linearly interpolates between two entries in a 256-entry table
float FixSin(angle a);

// Returns the cosine of the given angle.  Linearly interpolates between two entries in a 256-entry table
float FixCos(angle a);

// Returns the sine of the given angle, but does no interpolation
float FixSinFast(angle a);

// Returns the cosine of the given angle, but does no interpolation
float FixCosFast(angle a);

#define Round(x) ((int)(x + 0.5))

fix FloatToFixFast(float num);

// Conversion macros
//??#define FloatToFix(num) Round((num) * FLOAT_SCALER)
#define FloatToFix(num) ((fix)((num) * FLOAT_SCALER))
#define IntToFix(num) ((num) << FIX_SHIFT)
#define ShortToFix(num) (((long)(num)) << FIX_SHIFT)
#define FixToFloat(num) (((float)(num)) / FLOAT_SCALER)
#define FixToInt(num) ((num) >> FIX_SHIFT)
#define FixToShort(num) ((short)((num) >> FIX_SHIFT))

// Tables for trig functions
float sincos_table[321]; // 256 entries + 64 sin-only + 1 for interpolation
angle asin_table[257];   // 1 quadrants worth, +1 for interpolation
angle acos_table[257];

// Generate the data for the trig tables
void InitMathTables() {
  int i;
  float rad, s, c;

  for (i = 0; i < 321; i++) {
    rad = (float)((double)i / 256.0 * 2 * PI);
    sincos_table[i] = (float)sin(rad);
  }

  for (i = 0; i < 256; i++) {

    s = asin((float)i / 256.0);
    c = acos((float)i / 256.0);

    s = (s / (PI * 2));
    c = (c / (PI * 2));

    asin_table[i] = FloatToFix(s);
    acos_table[i] = FloatToFix(c);
  }

  asin_table[256] = asin_table[255];
  acos_table[256] = acos_table[255];

  //	Initialize a random seed.
  srand(time(NULL));
}

// Returns the sine of the given angle.  Linearly interpolates between two entries in a 256-entry table
float FixSin(angle a) {
  int i, f;
  float s0, s1;

  i = (a >> 8) & 0xff;
  f = a & 0xff;

  s0 = sincos_table[i];
  s1 = sincos_table[i + 1];
  return (float)(s0 + ((s1 - s0) * (double)f / 256.0));
}

// Returns the cosine of the given angle.  Linearly interpolates between two entries in a 256-entry table
float FixCos(angle a) {
  int i, f;
  float c0, c1;

  i = (a >> 8) & 0xff;
  f = a & 0xff;

  c0 = sincos_table[i + 64];
  c1 = sincos_table[i + 64 + 1];
  return (float)(c0 + ((c1 - c0) * (double)f / 256.0));
}

// Returns the sine of the given angle, but does no interpolation
float FixSinFast(angle a) {
  int i;

  i = ((a + 0x80) >> 8) & 0xff;

  return sincos_table[i];
}

// Returns the cosine of the given angle, but does no interpolation
float FixCosFast(angle a) {
  int i;

  i = ((a + 0x80) >> 8) & 0xff;

  return sincos_table[i + 64];
}

// use this instead of:
// for:  (int)floor(x+0.5f) use FloatRound(x)
//       (int)ceil(x-0.5f)  use FloatRound(x)
//       (int)floor(x-0.5f) use FloatRound(x-1.0f)
//       (int)floor(x)      use FloatRound(x-0.5f)
// for values in the range -2048 to 2048

// Set a vector to {0,0,0}
int FloatRound(float x) {
  float nf;
  nf = x + 8390656.0f;
  return ((*((int *)&nf)) & 0x7FFFFF) - 2048;
}

// A fast way to convert floats to fix
fix FloatToFixFast(float x) {

  float nf;
  nf = x * 65536.0f + 8390656.0f;
  return ((*((int *)&nf)) & 0x7FFFFF) - 2048;
}

// Get rid of the "no return value" warnings in the next three functions
#pragma warning(disable : 4035)

// compute inverse sine
angle FixAsin(float v) {
  fix vv;
  int i, f, aa;

  vv = FloatToFix(fabs(v));

  if (vv >= F1_0) // check for out of range
    return 0x4000;

  i = (vv >> 8) & 0xff;
  f = vv & 0xff;

  aa = asin_table[i];
  aa = aa + (((asin_table[i + 1] - aa) * f) >> 8);

  if (v < 0)
    aa = F1_0 - aa;

  return aa;
}

// compute inverse cosine
angle FixAcos(float v) {
  fix vv;
  int i, f, aa;

  vv = FloatToFix(fabs(v));

  if (vv >= F1_0) // check for out of range
    return 0;

  i = (vv >> 8) & 0xff;
  f = vv & 0xff;

  aa = acos_table[i];
  aa = aa + (((acos_table[i + 1] - aa) * f) >> 8);

  if (v < 0)
    aa = 0x8000 - aa;

  return aa;
}

// given cos & sin of an angle, return that angle.
// parms need not be normalized, that is, the ratio of the parms cos/sin must
// equal the ratio of the actual cos & sin for the result angle, but the parms
// need not be the actual cos & sin.
// NOTE: this is different from the standard C atan2, since it is left-handed.
angle FixAtan2(float cos, float sin) {
  float q, m;
  angle t;

  // find smaller of two

  q = (sin * sin) + (cos * cos);

  m = sqrt(q);

  if (m == 0)
    return 0;

  if (fabs(sin) < fabs(cos)) {
    // sin is smaller, use arcsin
    t = FixAsin(sin / m);
    if (cos < 0)
      t = 0x8000 - t;

    return t;
  } else {
    t = FixAcos(cos / m);
    if (sin < 0)
      t = F1_0 - t;

    return t;
  }
}

// Does a ceiling operation on a fixed number
fix FixCeil(fix num) {
  int int_num;
  fix new_num;

  int_num = FixToInt(num);

  if (num & 0xFFFF) {
    new_num = IntToFix(int_num + 1);
    return new_num;
  }

  new_num = IntToFix(int_num);
  return (new_num);
}

// Floors a fixed number
fix FixFloor(fix num) {
  int int_num = FixToInt(num);

  return (IntToFix(int_num));
}

// use this instead of:
// for:  (int)floor(x+0.5f) use FloatRound(x)
//       (int)ceil(x-0.5f)  use FloatRound(x)
//       (int)floor(x-0.5f) use FloatRound(x-1.0f)
//       (int)floor(x)      use FloatRound(x-0.5f)
// for values in the range -2048 to 2048
int FloatRound(float x);

angle FixAtan2(float cos, float sin);
angle FixAsin(float v);
angle FixAcos(float v);

// Does a ceiling operation on a fixed number
fix FixCeil(fix num);

// Floors a fixed number
fix FixFloor(fix num);

// Used for debugging.  It is used in printf's so we do not have to write out the structure 3 times
// to print all the coordinates.
#define XYZ(v) (v)->x, (v)->y, (v)->z

extern const matrix Identity_matrix;

// Given a matrix, makes it an identity matrix
extern void vm_MakeIdentity(matrix *);

// Set a vector to {0,0,0}
extern void vm_MakeZero(vector *v);

// Set an angvec to {0,0,0}
extern void vm_MakeZero(angvec *a);

// Rotates a vector thru a matrix
extern void vm_MatrixMulVector(vector *, vector *, matrix *);

// Multiply a vector times the transpose of a matrix
void vm_VectorMulTMatrix(vector *result, vector *v, matrix *m);

// Multiplies 2 3x3 matrixes, returning the result in first argument
extern void vm_MatrixMul(matrix *, matrix *, matrix *);

// Multiply a matrix times the transpose of a matrix
void vm_MatrixMulTMatrix(matrix *dest, matrix *src0, matrix *src1);

// Computes all math look up tables, must be called before any vector stuff is used
extern void vm_InitMathTables();

// Given a vector, returns the magnitude.  Uses sqrt so it's slow
extern float vm_GetMagnitude(vector *);

// Given a vector, returns an approximation of the magnitude
extern float vm_GetMagnitudeFast(vector *);

// Returns the dot product of the two given vectors
extern float vm_DotProduct(vector *, vector *);

// Returns a perpendicular vector to the two given vectors
extern void vm_CrossProduct(vector *, vector *, vector *);

// Returns the difference between two vectors
extern void vm_SubVectors(vector *, const vector *, const vector *);

// Returns adds two vectors, returns result in first arg
extern void vm_AddVectors(vector *, vector *, vector *);

// Inits vector to 0,0,0
extern void vm_CenterVector(vector *);

// Given a vector, divides second arg by vector components
extern void vm_AverageVector(vector *, int);

// Normalizes a vector
// Returns the magnitude before normalization
extern float vm_VectorNormalize(vector *);

// Scales second arg vector by 3rd arg, placing result in first arg
extern void vm_ScaleVector(vector *, vector *, float);

// Scales all components of vector v by value s adds the result to p and stores result in vector d
extern void vm_ScaleAddVector(vector *d, vector *p, vector *v, float s);

// Divides second vector components by 3rd arg, placing result in first arg.  Useful for parametric lines
extern void vm_DivVector(vector *, vector *, float);

// Same as VectorNormalize, but uses approximation
extern float vm_VectorNormalizeFast(vector *);

// Clears a matrix to zero
extern void vm_ClearMatrix(matrix *);

// Transposes a matrix in place
extern void vm_TransposeMatrix(matrix *);

// Given 3 angles (p,h,b), makes a rotation matrix out of them
extern void vm_AnglesToMatrix(matrix *, angle p, angle h, angle b);

// Ensure that a matrix is orthogonal
void vm_Orthogonalize(matrix *m);

// Compute a matrix from one or two vectors.  At least one and at most two vectors must/can be specified.
// Parameters:	m - filled in with the orienation matrix
//					fvec,uvec,rvec - pointers to vectors that determine the matrix.
//						One or two of these must be specified, with the other(s) set to NULL.
void vm_VectorToMatrix(matrix *m, vector *fvec, vector *uvec = NULL, vector *rvec = NULL);

// Computes a matrix from a vector and and angle of rotation around that vector
// Parameters:	m - filled in with the computed matrix
//					v - the forward vector of the new matrix
//					a - the angle of rotation around the forward vector
void vm_VectorAngleToMatrix(matrix *m, vector *v, angle a);

// Given an angle, places sin in 2nd arg, cos in 3rd.  Either can be null
extern void vm_SinCos(angle, float *, float *);

// Given x1,y1,x2,y2, returns the slope
extern float vm_GetSlope(float, float, float, float);

// Calculates the perpendicular vector given three points
// Parms:	n - the computed perp vector (filled in)
//			v0,v1,v2 - three clockwise vertices
void vm_GetPerp(vector *n, vector *a, vector *b, vector *c);

// Calculates the (normalized) surface normal give three points
// Parms:	n - the computed surface normal (filled in)
//			v0,v1,v2 - three clockwise vertices
// Returns the magnitude of the normal before it was normalized.
// The bigger this value, the better the normal.
float vm_GetNormal(vector *n, vector *v0, vector *v1, vector *v2);

#define vm_GetSurfaceNormal vm_GetNormal

// Gets the distances (magnitude) between two vectors. Slow.
extern float vm_VectorDistance(const vector *a, const vector *b);

// Gets the approx distances (magnitude) between two vectors. Faster.
extern float vm_VectorDistanceQuick(vector *a, vector *b);

// Computes a normalized direction vector between two points
// Parameters:	dest - filled in with the normalized direction vector
//					start,end - the start and end points used to calculate the vector
// Returns:		the distance between the two input points
float vm_GetNormalizedDir(vector *dest, vector *end, vector *start);

// Returns a normalized direction vector between two points
// Uses sloppier magnitude, less precise
float vm_GetNormalizedDirFast(vector *dest, vector *end, vector *start);

// extract angles from a matrix
angvec *vm_ExtractAnglesFromMatrix(angvec *a, matrix *m);

//	returns the angle between two vectors and a forward vector
angle vm_DeltaAngVec(vector *v0, vector *v1, vector *fvec);

//	returns the angle between two normalized vectors and a forward vector
angle vm_DeltaAngVecNorm(vector *v0, vector *v1, vector *fvec);

// Computes the distance from a point to a plane.
// Parms:	checkp - the point to check
// Parms:	norm - the (normalized) surface normal of the plane
//				planep - a point on the plane
// Returns:	The signed distance from the plane; negative dist is on the back of the plane
float vm_DistToPlane(vector *checkp, vector *norm, vector *planep);

// returns the value of a determinant
float calc_det_value(matrix *det);

void vm_MakeInverseMatrix(matrix *dest);
void vm_SinCosToMatrix(matrix *m, float sinp, float cosp, float sinb, float cosb, float sinh, float cosh);

// Gets the real center of a polygon
float vm_GetCentroid(vector *centroid, vector *src, int nv);

//	retrieves a random vector in values -RAND_MAX/2 to RAND_MAX/2
void vm_MakeRandomVector(vector *vec);

// Given a set of points, computes the minimum bounding sphere of those points
float vm_ComputeBoundingSphere(vector *center, vector *vecs, int num_verts);

// Gets the real center of a polygon, but uses fast magnitude calculation
// Returns the size of the passed in stuff
float vm_GetCentroidFast(vector *centroid, vector *src, int nv);

// Here are the C++ operator overloads -- they do as expected
extern matrix operator*(matrix src0, matrix src1);
extern matrix operator*=(matrix &src0, matrix src1);

void vm_AverageVector(vector *a, int num) {
  // Averages a vector.  ie divides each component of vector a by num
  // assert (num!=0);

  a->x = a->x / (float)num;
  a->y = a->y / (float)num;
  a->z = a->z / (float)num;
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

float vm_VectorDistance(const vector *a, const vector *b) {
  // Given two vectors, returns the distance between them

  vector dest;
  float dist;

  vm_SubVectors(&dest, a, b);
  dist = vm_GetMagnitude(&dest);
  return dist;
}
float vm_VectorDistanceQuick(vector *a, vector *b) {
  // Given two vectors, returns the distance between them

  vector dest;
  float dist;

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
float vm_GetNormal(vector *n, vector *v0, vector *v1, vector *v2) {
  vm_GetPerp(n, v0, v1, v2);

  return vm_VectorNormalize(n);
}

// Does a simple dot product calculation
float vm_DotProduct(vector *u, vector *v) { return (u->x * v->x) + (u->y * v->y) + (u->z * v->z); }

// Scales all components of vector v by value s and stores result in vector d
// dest can equal source
void vm_ScaleVector(vector *d, vector *v, float s) {
  d->x = (v->x * s);
  d->y = (v->y * s);
  d->z = (v->z * s);
}

void vm_ScaleAddVector(vector *d, vector *p, vector *v, float s) {
  // Scales all components of vector v by value s
  // adds the result to p and stores result in vector d
  // dest can equal source

  d->x = p->x + (v->x * s);
  d->y = p->y + (v->y * s);
  d->z = p->z + (v->z * s);
}

void vm_DivVector(vector *dest, vector *src, float n) {
  // Divides a vector into n portions
  // Dest can equal src

  // assert (n!=0);

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
float vm_VectorNormalize(vector *a) {
  float mag;

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

float vm_GetMagnitude(vector *a) {
  float f;

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

  float t;

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

  // assert(result != v);

  result->x = *v * m->rvec;
  result->y = *v * m->uvec;
  result->z = *v * m->fvec;
}

// Multiply a vector times the transpose of a matrix
void vm_VectorMulTMatrix(vector *result, vector *v, matrix *m) {
  // assert(result != v);

  result->x = vm_Dot3Vector(m->rvec.x, m->uvec.x, m->fvec.x, v);
  result->y = vm_Dot3Vector(m->rvec.y, m->uvec.y, m->fvec.y, v);
  result->z = vm_Dot3Vector(m->rvec.z, m->uvec.z, m->fvec.z, v);
}

void vm_MatrixMul(matrix *dest, matrix *src0, matrix *src1) {
  // For multiplying two 3x3 matrices together

  // assert((dest != src0) && (dest != src1));

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

  // assert((dest != src0) && (dest != src1));

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
float vm_GetNormalizedDir(vector *dest, vector *end, vector *start) {
  vm_SubVectors(dest, end, start);
  return vm_VectorNormalize(dest);
}

// Returns a normalized direction vector between two points
// Just like vm_GetNormalizedDir(), but uses sloppier magnitude, less precise
// Parameters:	dest - filled in with the normalized direction vector
//					start,end - the start and end points used to calculate the vector
// Returns:		the distance between the two input points
float vm_GetNormalizedDirFast(vector *dest, vector *end, vector *start) {
  vm_SubVectors(dest, end, start);
  return vm_VectorNormalizeFast(dest);
}

float vm_GetMagnitudeFast(vector *v) {
  float a, b, c, bc;

  a = fabs(v->x);
  b = fabs(v->y);
  c = fabs(v->z);

  if (a < b) {
    float t = a;
    a = b;
    b = t;
  }

  if (b < c) {
    float t = b;
    b = c;
    c = t;

    if (a < b) {
      float t = a;
      a = b;
      b = t;
    }
  }

  bc = (b / 4) + (c / 8);

  return a + bc + (bc / 2);
}

// Normalize a vector using an approximation of the magnitude
// Returns:  the magnitude before normalization
float vm_VectorNormalizeFast(vector *a) {
  float mag;

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
float vm_DistToPlane(vector *checkp, vector *norm, vector *planep) {
  vector t;

  t = *checkp - *planep;

  return t * *norm;
}

float vm_GetSlope(float x1, float y1, float x2, float y2) {
  // returns the slope of a line
  float r;

  if (y2 - y1 == 0)
    return (0.0);

  r = (x2 - x1) / (y2 - y1);
  return (r);
}

void vm_SinCosToMatrix(matrix *m, float sinp, float cosp, float sinb, float cosb, float sinh, float cosh) {
  float sbsh, cbch, cbsh, sbch;

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
  float sinp, cosp, sinb, cosb, sinh, cosh;

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
  float sinb, cosb, sinp, cosp, sinh, cosh;

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
  if (vm_VectorNormalize(&m->fvec) == 0) {
    return;
  }

  // Generate right vector from forward and up vectors
  m->rvec = m->uvec ^ m->fvec;

  // Normaize new right vector
  if (vm_VectorNormalize(&m->rvec) == 0) {
    vm_VectorToMatrix(m, &m->fvec, NULL, NULL); // error, so generate from forward vector only
    return;
  }

  // Recompute up vector, in case it wasn't entirely perpendiclar
  m->uvec = m->fvec ^ m->rvec;
}

// do the math for vm_VectorToMatrix()
void DoVectorToMatrix(matrix *m, vector *fvec, vector *uvec, vector *rvec) {
  vector *xvec = &m->rvec, *yvec = &m->uvec, *zvec = &m->fvec;

  // ASSERT(fvec != NULL);

  *zvec = *fvec;
  if (vm_VectorNormalize(zvec) == 0) {
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

        vm_VectorNormalize(xvec);

        *yvec = *zvec ^ *xvec;
      }

    } else { // use right vec

      *xvec = *rvec;
      if (vm_VectorNormalize(xvec) == 0)
        goto bad_vector2;

      *yvec = *zvec ^ *xvec;

      // normalize new perpendicular vector
      if (vm_VectorNormalize(yvec) == 0)
        goto bad_vector2;

      // now recompute right vector, in case it wasn't entirely perpendiclar
      *xvec = *yvec ^ *zvec;
    }
  } else { // use up vec

    *yvec = *uvec;
    if (vm_VectorNormalize(yvec) == 0)
      goto bad_vector2;

    *xvec = *yvec ^ *zvec;

    // normalize new perpendicular vector
    if (vm_VectorNormalize(xvec) == 0)
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
      // ASSERT(rvec);
      DoVectorToMatrix(&tmatrix, rvec, NULL, NULL);
      m->fvec = -tmatrix.rvec;
      m->uvec = tmatrix.uvec;
      m->rvec = tmatrix.fvec;
      return;
    }
  } else {
    // ASSERT(! (uvec && rvec));		//can only have 1 or 2 vectors specified
    DoVectorToMatrix(m, fvec, uvec, rvec);
  }
}

void vm_SinCos(unsigned short a, float *s, float *c) {
  if (s)
    *s = FixSin(a);
  if (c)
    *c = FixCos(a);
}

// extract angles from a matrix
angvec *vm_ExtractAnglesFromMatrix(angvec *a, matrix *m) {
  float sinh, cosh, cosp;

  if (m->fvec.x == 0 && m->fvec.z == 0) // zero head
    a->h = 0;
  else
    a->h = FixAtan2(m->fvec.z, m->fvec.x);

  sinh = FixSin(a->h);
  cosh = FixCos(a->h);

  if (fabs(sinh) > fabs(cosh)) // sine is larger, so use it
    cosp = (m->fvec.x / sinh);
  else // cosine is larger, so use it
    cosp = (m->fvec.z / cosh);

  if (cosp == 0 && m->fvec.y == 0)
    a->p = 0;
  else
    a->p = FixAtan2(cosp, -m->fvec.y);

  if (cosp == 0) // the cosine of pitch is zero.  we're pitched straight up. say no bank

    a->b = 0;

  else {
    float sinb, cosb;

    sinb = (m->rvec.y / cosp);
    cosb = (m->uvec.y / cosp);

    if (sinb == 0 && cosb == 0)
      a->b = 0;
    else
      a->b = FixAtan2(cosb, sinb);
  }

  return a;
}

// returns the value of a determinant
float calc_det_value(matrix *det) {
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

  vm_VectorNormalize(&t0);
  vm_VectorNormalize(&t1);

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
float vm_GetCentroid(vector *centroid, vector *src, int nv) {
  // ASSERT (nv>2);
  vector normal;
  float area, total_area;
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
float vm_GetCentroidFast(vector *centroid, vector *src, int nv) {
  // ASSERT (nv>2);
  vector normal;
  float area, total_area;
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
  vec->x = rand() - RAND_MAX / 2;
  vec->y = rand() - RAND_MAX / 2;
  vec->z = rand() - RAND_MAX / 2;
}

// Given a set of points, computes the minimum bounding sphere of those points
float vm_ComputeBoundingSphere(vector *center, vector *vecs, int num_verts) {
  // This algorithm is from Graphics Gems I.  There's a better algorithm in Graphics Gems III that
  // we should probably implement sometime.

  vector *min_x, *max_x, *min_y, *max_y, *min_z, *max_z, *vp;
  float dx, dy, dz;
  float rad, rad2;
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
    float t2;

    delta = *vp - *center;
    t2 = delta.x * delta.x + delta.y * delta.y + delta.z * delta.z;

    // If point outside, make the sphere bigger
    if (t2 > rad2) {
      float t;

      t = sqrt(t2);
      rad = (rad + t) / 2;
      rad2 = rad * rad;
      *center += delta * (t - rad) / t;
    }
  }

  // We're done
  return rad;
}

#endif
