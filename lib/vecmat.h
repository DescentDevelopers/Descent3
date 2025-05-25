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

 * $Logfile: /DescentIII/Main/lib/vecmat.h $
 * $Revision: 21 $
 * $Date: 1/21/99 11:16p $
 * $Author: Jeff $
 *
 * Vector/Matrix routines
 *
 * $Log: /DescentIII/Main/lib/vecmat.h $
 *
 * 21    1/21/99 11:16p Jeff
 * pulled out some structs and defines from header files and moved them
 * into separate header files so that multiplayer dlls don't require major
 * game headers, just those new headers.  Side effect is a shorter build
 * time.  Also cleaned up some header file #includes that weren't needed.
 * This affected polymodel.h, object.h, player.h, vecmat.h, room.h,
 * manage.h and multi.h
 *
 * 20    1/11/99 4:45p Jason
 * added first pass at katmai support
 *
 * 19    1/01/99 4:10p Chris
 * Added some const parameters, improved ray cast object collide/rejection
 * code
 *
 * 18    5/25/98 3:45p Jason
 * added vm_GetCentroidFast
 *
 * 17    3/12/98 7:30p Chris
 * Added ObjSetOrient
 *
 * 16    2/08/98 6:01p Matt
 * Added functions to multiply by a transposed matrix, and simplified some
 * other code a bit.
 *
 * 15    2/06/98 11:02a Matt
 * Added missing include
 *
 * 14    2/06/98 10:43a Matt
 * Made vm_VectorToMatrix() take any one or two vectors, & not require
 * forward vec.
 * Also, made the uvec and rvec parameters default to NULL if not
 * specified.
 *
 * 13    2/02/98 8:17p Chris
 * Added a != operator and a Zero_vector constant
 *
 * 12    1/20/98 4:04p Matt
 * Made vm_GetNormalizedDir() and vm_GetNormalizeDirFast() return the
 * distance between the two input points.
 *
 * 11    1/13/98 1:30p Jason
 * changed vm_GetCentroid to also return the size of the area
 *
 * 10    11/04/97 6:21p Chris
 * Allowed other files to use the vm_DeltaAngVecNorm function
 *
 * 9     10/25/97 7:15p Jason
 * implemented vm_ComputeBoundingSphere
 *
 * 8     10/14/97 4:35p Samir
 * Added vm_MakeRandomVector.
 *
 * 7     9/23/97 2:26p Matt
 * Made vm_GetNormal() return the magnitude of the normal (before it was
 * normalized)
 *
 * 6     8/28/97 4:56p Jason
 * implemented vm_GetCentroid
 *
 * 5     8/18/97 6:39p Matt
 * Added vm_VectorAngleToMatrix()
 *
 * 4     8/04/97 12:36p Chris
 * Added an == operator for vectors
 *
 * 3     7/17/97 3:56p Matt
 * Added vm_Orthogonalize()
 *
 * 2     7/16/97 5:15p Chris
 * Moved the XYZ() macro to vecmat.h
 *
 * 1     6/23/97 9:25p Samir
 * added because source safe sucks
 *
 * 31    4/18/97 2:14p Samir
 * Added vm_DeltaAngVec.
 *
 * 30    2/27/97 6:16p Chris
 * Added the vector_array type
 *
 * 29    2/27/97 6:08 PM Jeremy
 * added prototypes for vm_MakeInverseMatrix and vm_SinCosToMatrix
 *
 * 28    2/27/97 5:23p Chris
 * Removed the remaining extern inline
 *
 * 27    2/27/97 4:56p Samir
 * took out ifndef MAC stuff
 *
 * 26    2/27/97 1:40p Chris
 * Added a function to compute the determinate --
 * BTW on the last rev. I moved all inline functions
 * to the header.  (So they will be inlined)
 *
 * 25    2/26/97 7:33p Chris
 *
 * 24    2/26/97 6:17 PM Jeremy
 * put #pragma warning inside #ifndef macintosh
 *
 * 23    2/20/97 11:41a Chris
 * Added a negate unary operator for vectors
 *
 * 22    2/12/97 5:28p Jason
 * implemented ExtractAnglesFromMatrix function
 *
 * 21    2/11/97 6:49p Matt
 * Added vm_VectorToMatrix()
 * Made vm_NormalizeVector() return the old vector mag
 * Fixed bug in inline version of crossprod
 *
 * 20    2/11/97 11:54a Jason
 *
 * 19    2/10/97 3:36p Matt
 * Fixed (added) IDENTITY_MATRIX define
 *
 * 18    2/07/97 5:38p Matt
 * Moved fixed-point math funcs to fix.lib
 *
 * $NoKeywords: $
 */

#ifndef VECMAT_H
#define VECMAT_H

#include <cmath>

// All structs, defines and inline functions are located in vecmat_external.h
// vecmat_external.h is where anything that can be used by DLLs should be.
#include "vecmat_external.h"

extern const vector Zero_vector;
extern const matrix Identity_matrix;

// Used for debugging.  It is used in printf's so we do not have to write out the structure 3 times
// to print all the coordinates.
#define XYZ(v) (v)->x, (v)->y, (v)->z

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
extern float vm_DotProduct(const vector *, const vector *);

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
extern float vm_NormalizeVector(vector *);

// Scales second arg vector by 3rd arg, placing result in first arg
extern void vm_ScaleVector(vector *, vector *, float);

// Scales all components of vector v by value s adds the result to p and stores result in vector d
extern void vm_ScaleAddVector(vector *d, vector *p, vector *v, float s);

// Divides second vector components by 3rd arg, placing result in first arg.  Useful for parametric lines
extern void vm_DivVector(vector *, vector *, const scalar);

// Same as NormalizeVector, but uses approximation
extern float vm_NormalizeVectorFast(vector *);

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
void vm_VectorToMatrix(matrix *m, vector *fvec, vector *uvec = nullptr, vector *rvec = nullptr);

// Computes a matrix from a vector and and angle of rotation around that vector
// Parameters:	m - filled in with the computed matrix
//					v - the forward vector of the new matrix
//					a - the angle of rotation around the forward vector
void vm_VectorAngleToMatrix(matrix *m, vector *v, angle a);

// Given an angle, places sin in 2nd arg, cos in 3rd.  Either can be null
extern void vm_SinCos(angle, scalar *, scalar *);

// Given x1,y1,x2,y2, returns the slope
extern scalar vm_GetSlope(scalar, scalar, scalar, scalar);

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
void vm_SinCosToMatrix(matrix *m, scalar sinp, scalar cosp, scalar sinb, scalar cosb, scalar sinh, scalar cosh);

// Gets the real center of a polygon
scalar vm_GetCentroid(vector *centroid, vector *src, int nv);

//	retrieves a random vector in values -RAND_MAX/2 to RAND_MAX/2
void vm_MakeRandomVector(vector *vec);

// Given a set of points, computes the minimum bounding sphere of those points
scalar vm_ComputeBoundingSphere(vector *center, vector *vecs, int num_verts);

// Gets the real center of a polygon, but uses fast magnitude calculation
// Returns the size of the passed in stuff
float vm_GetCentroidFast(vector *centroid, vector *src, int nv);

// Here are the C++ operator overloads -- they do as expected
extern matrix operator*(matrix src0, matrix src1);
extern matrix operator*=(matrix &src0, matrix src1);

#endif
