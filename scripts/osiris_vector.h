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

#include "fix.h"
#include "vecmat_external.h"

const vector Zero_vector = vector::ne();

// Disable the "possible loss of data" warning
#pragma warning(disable : 4244)

// Used for debugging.  It is used in printf's so we do not have to write out the structure 3 times
// to print all the coordinates.
#define XYZ(v) (v)->x(), (v)->y(), (v)->z()
#define HPB(v) (v)->h(), (v)->p(), (v)->b()

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

// Given a vector, returns the magnitude.  Uses sqrt so it's slow
extern scalar vm_GetMagnitude(const vector *);

// Given a vector, returns an approximation of the magnitude
extern scalar vm_GetMagnitudeFast(const vector *);

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
extern scalar vm_VectorDistance(const vector *a, const vector *b);

// Gets the approx distances (magnitude) between two vectors. Faster.
extern scalar vm_VectorDistanceQuick(const vector *a, const vector *b);

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

#endif
