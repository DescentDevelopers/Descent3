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

 * $Logfile: /DescentIII/Main/lib/vecmat_external.h $
 * $Revision: 3 $
 * $Date: 2/19/99 4:26p $
 * $Author: Jason $
 *
 * Contains any header information that can/should be exported to DLLs
 *
 * $Log: /DescentIII/Main/lib/vecmat_external.h $
 *
 * 3     2/19/99 4:26p Jason
 * more work on Katmai support
 *
 * 2     1/21/99 11:15p Jeff
 * pulled out some structs and defines from header files and moved them
 * into seperate header files so that multiplayer dlls don't require major
 * game headers, just those new headers.  Side effect is a shorter build
 * time.  Also cleaned up some header file #includes that weren't needed.
 * This affected polymodel.h, object.h, player.h, vecmat.h, room.h,
 * manage.h and multi.h
 *
 * $NoKeywords: $
 */

#ifndef VECMAT_EXTERNAL_H
#define VECMAT_EXTERNAL_H

#include <cstdint>

// Angles are unsigned shorts
typedef uint16_t angle; // make sure this matches up with fix.h

struct angvec {
  angle p, h, b;
};

#define IDENTITY_MATRIX                                                                                                \
  {                                                                                                                    \
    {1.0, 0, 0}, {0, 1.0, 0}, { 0, 0, 1.0 }                                                                            \
  }

struct vector {
  float x, y, z;
};

struct vector4 {
  float x, y, z, kat_pad;
};

struct vector_array {
  float xyz[3];
};

struct matrix {
  vector rvec, uvec, fvec;
};

struct matrix4 {
  vector4 rvec, uvec, fvec;
};

// Zero's out a vector
static inline void vm_MakeZero(vector *v) { v->x = v->y = v->z = 0; }

// Set an angvec to {0,0,0}
static inline void vm_MakeZero(angvec *a) { a->p = a->h = a->b = 0; }

// Checks for equality
static inline bool operator==(vector a, vector b) {
  bool equality = false;
  // Adds two vectors.

  if (a.x == b.x && a.y == b.y && a.z == b.z)
    equality = true;

  return equality;
}

// Checks for inequality
static inline bool operator!=(vector a, vector b) {
  bool equality = true;
  // Adds two vectors.

  if (a.x == b.x && a.y == b.y && a.z == b.z)
    equality = false;

  return equality;
}

// Adds 2 vectors
static inline vector operator+(vector a, vector b) {
  // Adds two vectors.

  a.x += b.x;
  a.y += b.y;
  a.z += b.z;

  return a;
}

// Adds 2 vectors
static inline vector operator+=(vector &a, vector b) { return (a = a + b); }

// Adds 2 matrices
static inline matrix operator+(matrix a, matrix b) {
  // Adds two 3x3 matrixs.

  a.rvec += b.rvec;
  a.uvec += b.uvec;
  a.fvec += b.fvec;

  return a;
}

// Adds 2 matrices
static inline matrix operator+=(matrix &a, matrix b) { return (a = a + b); }

// Subtracts 2 vectors
static inline vector operator-(vector a, vector b) {
  // subtracts two vectors

  a.x -= b.x;
  a.y -= b.y;
  a.z -= b.z;

  return a;
}

// Subtracts 2 vectors
static inline vector operator-=(vector &a, vector b) { return (a = a - b); }

// Subtracts 2 matrices
static inline matrix operator-(matrix a, matrix b) {
  // subtracts two 3x3 matrices

  a.rvec = a.rvec - b.rvec;
  a.uvec = a.uvec - b.uvec;
  a.fvec = a.fvec - b.fvec;

  return a;
}

// Subtracts 2 matrices
static inline matrix operator-=(matrix &a, matrix b) { return (a = a - b); }

// Does a simple dot product calculation
static inline float operator*(vector u, vector v) { return (u.x * v.x) + (u.y * v.y) + (u.z * v.z); }

// Scalar multiplication
static inline vector operator*(vector v, float s) {
  v.x *= s;
  v.y *= s;
  v.z *= s;

  return v;
}

// Scalar multiplication
static inline vector operator*=(vector &v, float s) { return (v = v * s); }

// Scalar multiplication
static inline vector operator*(float s, vector v) { return v * s; }

// Scalar multiplication
static inline matrix operator*(float s, matrix m) {
  m.fvec = m.fvec * s;
  m.uvec = m.uvec * s;
  m.rvec = m.rvec * s;

  return m;
}

// Scalar multiplication
static inline matrix operator*(matrix m, float s) { return s * m; }

// Scalar multiplication
static inline matrix operator*=(matrix &m, float s) { return (m = m * s); }

// Scalar division
static inline vector operator/(vector src, float n) {
  src.x /= n;
  src.y /= n;
  src.z /= n;

  return src;
}

// Scalar division
static inline vector operator/=(vector &src, float n) { return (src = src / n); }

// Scalar division
static inline matrix operator/(matrix src, float n) {
  src.fvec = src.fvec / n;
  src.rvec = src.rvec / n;
  src.uvec = src.uvec / n;

  return src;
}

// Scalar division
static inline matrix operator/=(matrix &src, float n) { return (src = src / n); }

// Computes a cross product between u and v, returns the result
//	in Normal.
static inline vector operator^(vector u, vector v) {
  vector dest;

  dest.x = (u.y * v.z) - (u.z * v.y);
  dest.y = (u.z * v.x) - (u.x * v.z);
  dest.z = (u.x * v.y) - (u.y * v.x);

  return dest;
}

// Matrix transpose
static inline matrix operator~(matrix m) {
  float t;

  t = m.uvec.x;
  m.uvec.x = m.rvec.y;
  m.rvec.y = t;
  t = m.fvec.x;
  m.fvec.x = m.rvec.z;
  m.rvec.z = t;
  t = m.fvec.y;
  m.fvec.y = m.uvec.z;
  m.uvec.z = t;

  return m;
}

// Negate vector
static inline vector operator-(vector a) {
  a.x *= -1;
  a.y *= -1;
  a.z *= -1;

  return a;
}

// Apply a matrix to a vector
static inline vector operator*(vector v, matrix m) {
  vector result;

  result.x = v * m.rvec;
  result.y = v * m.uvec;
  result.z = v * m.fvec;

  return result;
}

static inline float vm_Dot3Vector(float x, float y, float z, vector *v) { return (x * v->x) + (y * v->y) + (z * v->z); }

#define vm_GetSurfaceNormal vm_GetNormal

#endif
