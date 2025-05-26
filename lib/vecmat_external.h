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
 * into separate header files so that multiplayer dlls don't require major
 * game headers, just those new headers.  Side effect is a shorter build
 * time.  Also cleaned up some header file #includes that weren't needed.
 * This affected polymodel.h, object.h, player.h, vecmat.h, room.h,
 * manage.h and multi.h
 *
 * $NoKeywords: $
 */

#pragma once

#include "fix.h"

struct angvec {
using T = angle;
constexpr static const size_t N       = 3;
constexpr static const size_t PITCH   = 0;
constexpr static const size_t HEADING = 1;
constexpr static const size_t BANK    = 2;
union {
  T phb[N];
  struct { T iip, iih, iib; };
};
constexpr inline T &p() { return iip; }
constexpr inline T &h() { return iih; }
constexpr inline T &b() { return iib; }
constexpr inline const T &p() const { return iip; }
constexpr inline const T &h() const { return iih; }
constexpr inline const T &b() const { return iib; }
constexpr static inline angvec id(ssize_t i = -1)
{
  return angvec{
  ((i % N) == PITCH)   ? (T)1 : (T)0,
  ((i % N) == HEADING) ? (T)1 : (T)0,
  ((i % N) == BANK)    ? (T)1 : (T)0
  };
}
constexpr static inline angvec ne()
{
  return angvec{ (T)0, (T)0, (T)0 };
}
};

struct vector {
using T = scalar;
constexpr static const size_t N = 3;
constexpr static const size_t X = 0;
constexpr static const size_t Y = 1;
constexpr static const size_t Z = 2;
union {
  T xyz[N];
  struct { T iix, iiy, iiz; };
  struct { T r, g, b; };
};
constexpr inline T &x() { return iix; }
constexpr inline T &y() { return iiy; }
constexpr inline T &z() { return iiz; }
constexpr inline const T &x() const { return iix; }
constexpr inline const T &y() const { return iiy; }
constexpr inline const T &z() const { return iiz; }
constexpr static inline vector cross3(vector a, vector b)
{
  vector r;
  r.iix = a.y() * b.z() - b.y() * a.z();
  r.iiy = a.z() * b.x() - b.z() * a.x();
  r.iiz = a.x() * b.y() - b.x() * a.y();
  return r;
}

constexpr static inline const vector id(ssize_t i = -1)
{
  return vector{
  ((i % N) == X) ? (T)1 : (T)0,
  ((i % N) == Y) ? (T)1 : (T)0,
  ((i % N) == Z) ? (T)1 : (T)0
  };
}
constexpr static inline const vector ne()
{
  return vector{ (T)0, (T)0, (T)0 };
}
// Does a simple dot product calculation
static inline scalar dot(const vector &u, const vector &v) { return (u.x() * v.x()) + (u.y() * v.y()) + (u.z() * v.z()); }
inline scalar mag() const { return iix * iix + iiy * iiy + iiz * iiz; }
inline void fill(T x) { iix = iiy = iiz = x; }
};

using vector_array = vector;

struct alignas(sizeof(scalar) * 4) aligned_vector {
using T = scalar;
constexpr static const size_t N = 3;
constexpr static const size_t X = 0;
constexpr static const size_t Y = 1;
constexpr static const size_t Z = 2;
union {
  T xyz[N];
  struct { T x, y, z; };
  struct { T r, g, b; };
};
constexpr static inline const aligned_vector id(ssize_t i = -1)
{
  return aligned_vector{
  ((i % N) == X) ? (T)1 : (T)0,
  ((i % N) == Y) ? (T)1 : (T)0,
  ((i % N) == Z) ? (T)1 : (T)0,
  };
}
constexpr static inline const aligned_vector ne()
{
  return aligned_vector{ (scalar)0, (scalar)0, (scalar)0 };
}
};

using aligned_vector_array = aligned_vector;

struct alignas(sizeof(scalar) * 4) vector4 {
using T = scalar;
constexpr static const size_t N = 4;
constexpr static const size_t X = 0;
constexpr static const size_t Y = 1;
constexpr static const size_t Z = 2;
constexpr static const size_t W = 3;
union {
  T xyzw[N];
  struct { T iix, iiy, iiz; union { T iiw; T kat_pad; }; };
  struct { union { T r; T l; }; T g, b, a; };
  struct { T u,v; union { T u2; T s; }; union { T v2; T t; }; };
};
constexpr inline T &x() { return iix; }
constexpr inline T &y() { return iiy; }
constexpr inline T &z() { return iiz; }
constexpr inline T &w() { return iiw; }
constexpr inline const T &x() const { return iix; }
constexpr inline const T &y() const { return iiy; }
constexpr inline const T &z() const { return iiz; }
constexpr inline const T &w() const { return iiw; }

constexpr static inline const vector4 id(ssize_t i = -1)
{
  return vector4{
  ((i % N) == X) ? (T)1 : (T)0,
  ((i % N) == Y) ? (T)1 : (T)0,
  ((i % N) == Z) ? (T)1 : (T)0,
  ((i % N) == W) ? (T)1 : (T)0
  };
}
constexpr static inline const vector4 ne()
{
  return vector4{ (T)0, (T)0, (T)0, (T)0 };
}
};

using vector4_array = vector4;
using aligned_vector4 = vector4;
using aligned_vector4_array = aligned_vector4;

struct matrix {
constexpr static const size_t RIGHT_HAND = 0;
constexpr static const size_t UP         = 1;
constexpr static const size_t FORWARD    = 2;
union {
struct {
  vector rvec;
  vector uvec;
  vector fvec;
};
  scalar a2d[3][3];
  scalar a1d[9];
};
constexpr static inline const matrix id()
{
  return matrix{ vector::id(0), vector::id(1), vector::id(2) };
}
constexpr static inline const matrix ne()
{
  return matrix{ vector::ne(), vector::ne(), vector::ne() };
}
};

constexpr matrix IDENTITY_MATRIX = matrix::id();

struct alignas(alignof(vector4) * 4) matrix4 {
constexpr static const size_t RIGHT_HAND = 0;
constexpr static const size_t UP         = 1;
constexpr static const size_t FORWARD    = 2;
constexpr static const size_t POSITION   = 3;
union {
struct {
  vector4 rvec;
  vector4 uvec;
  vector4 fvec;
  vector4 pos;
};
  scalar a2d[4][4];
  scalar a1d[16];
};
constexpr static inline const matrix4 id()
{
  return matrix4{ vector4::id(0), vector4::id(1), vector4::id(2), vector4::id(3) };
}
constexpr static inline const matrix4 ne()
{
  return matrix4{ vector4::ne(), vector4::ne(), vector4::ne(), vector4::ne() };
}
};

// Zero's out a vector
static inline void vm_MakeZero(vector *v) { *v = vector::ne(); }

// Set an angvec to {0,0,0}
static inline void vm_MakeZero(angvec *a) { *a = angvec::ne(); }

// Checks for equality
static inline bool operator==(vector a, vector b) {
  bool equality = false;
  // Adds two vectors.

  if (a.x() == b.x() && a.y() == b.y() && a.z() == b.z())
    equality = true;

  return equality;
}

// Checks for inequality
static inline bool operator!=(vector a, vector b) {
  bool equality = true;
  // Adds two vectors.

  if (a.x() == b.x() && a.y() == b.y() && a.z() == b.z())
    equality = false;

  return equality;
}

// Adds 2 vectors
static inline vector operator+(vector a, vector b) {
  // Adds two vectors.

  a.x() += b.x();
  a.y() += b.y();
  a.z() += b.z();

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

  a.x() -= b.x();
  a.y() -= b.y();
  a.z() -= b.z();

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

// Scalar multiplication
static inline vector operator*(vector v, float s) {
  v.x() *= s;
  v.y() *= s;
  v.z() *= s;

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
  src.x() /= n;
  src.y() /= n;
  src.z() /= n;

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

  dest.x() = (u.y() * v.z()) - (u.z() * v.y());
  dest.y() = (u.z() * v.x()) - (u.x() * v.z());
  dest.z() = (u.x() * v.y()) - (u.y() * v.x());

  return dest;
}

// Matrix transpose
static inline matrix operator~(matrix m) {
  float t;

  t = m.uvec.x();
  m.uvec.x() = m.rvec.y();
  m.rvec.y() = t;
  t = m.fvec.x();
  m.fvec.x() = m.rvec.z();
  m.rvec.z() = t;
  t = m.fvec.y();
  m.fvec.y() = m.uvec.z();
  m.uvec.z() = t;

  return m;
}

// Negate vector
static inline vector operator-(vector a) {
  a.x() *= -1;
  a.y() *= -1;
  a.z() *= -1;

  return a;
}

// Apply a matrix to a vector
static inline vector operator*(vector v, matrix m) {
  vector result;

  result.x() = vector::dot(v, m.rvec);
  result.y() = vector::dot(v, m.uvec);
  result.z() = vector::dot(v, m.fvec);

  return result;
}

inline scalar vm_Dot3Product(const vector a, const vector b) { return (a.x() * b.x()) + (a.y() * b.y()) + (a.z() * b.z()); }
static inline float vm_Dot3Vector(float x, float y, float z, vector *v) { return (x * v->x()) + (y * v->y()) + (z * v->z()); }
inline vector vm_Cross3Product(vector u, vector v) {
  return vector::cross3(u,v);
}

#define vm_GetSurfaceNormal vm_GetNormal
