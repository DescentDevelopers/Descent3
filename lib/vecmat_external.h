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

#ifdef _MSVC_LANG
#define VM_ALIGN(n) __declspec(align(n))
#else
#define VM_ALIGN(n) __attribute__((aligned(n)))
#endif

struct angvec {
using T = angle;
constexpr static const size_t N       = 3;
constexpr static const size_t PITCH   = 0;
constexpr static const size_t HEADING = 1;
constexpr static const size_t BANK    = 2;
union {
  T phb[N];
  struct { T p, h, b; };
};
constexpr static inline angvec id(ssize_t i = -1)
{
  return angvec{
  ((i % N) == PITCH)   ? (T)1 : (T)0,
  ((i % N) == HEADING) ? (T)1 : (T)0,
  ((i % N) == BANK)    ? (T)1 : (T)0
  };
}
constexpr static inline angvec ne()       { return angvec{ (T)0, (T)0, (T)0 }; }
constexpr inline const angvec hbp() const { return angvec{ h, b, p }; }
constexpr inline const angvec bph() const { return angvec{ b, p, h }; }
constexpr inline T sum() const            { return p + h + b; }
constexpr inline angvec operator-()      { return angvec{(T)-p, (T)-h, (T)-b }; }
constexpr inline angvec operator*(angvec other) { return angvec{(T)(p * other.p),(T)(h * other.h),(T)(b * other.b)}; }
};
// Set an angvec to {0,0,0}
static inline void vm_MakeZero(angvec *a) { *a = angvec::ne(); }

struct vector {
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
constexpr static inline const vector id(ssize_t i = -1)
{
  return vector{
  ((i % N) == X) ? (T)1 : (T)0,
  ((i % N) == Y) ? (T)1 : (T)0,
  ((i % N) == Z) ? (T)1 : (T)0
  };
}
constexpr static inline const vector ne() { return vector{ (T)0, (T)0, (T)0 }; }
constexpr inline const vector yzx() const { return vector{ y, z, x }; }
constexpr inline const vector zxy() const { return vector{ z, x, y }; }
constexpr inline T sum() const            { return x + y + z; }
constexpr inline vector operator-()       { return vector{(T)-x, (T)-y, (T)-z }; }

constexpr inline vector operator+(const vector other) const { return { x + other.x, y + other.y, z + other.z }; }
constexpr inline vector operator-(const vector other) const { return { x - other.x, y - other.y, z - other.z }; }
constexpr inline vector operator*(const vector other) const { return { x * other.x, y * other.y, z * other.z }; }
constexpr inline vector operator/(const vector other) const { return { x / other.x, y / other.y, z / other.z }; }

constexpr inline vector& operator+=(const vector other) { return ((*this) = (*this) + other); }
constexpr inline vector& operator-=(const vector other) { return ((*this) = (*this) - other); }
constexpr inline vector& operator*=(const vector other) { return ((*this) = (*this) * other); }
constexpr inline vector& operator/=(const vector other) { return ((*this) = (*this) / other); }

constexpr inline vector operator+(const scalar s) const { return (*this) + vector{ s, s, s }; }
constexpr inline vector operator-(const scalar s) const { return (*this) - vector{ s, s, s }; }
constexpr inline vector operator*(const scalar s) const { return (*this) * vector{ s, s, s }; }
constexpr inline vector operator/(const scalar s) const { return (*this) / vector{ s, s, s }; }

constexpr inline vector& operator+=(const scalar s) { return ((*this) = (*this) + s); }
constexpr inline vector& operator-=(const scalar s) { return ((*this) = (*this) - s); }
constexpr inline vector& operator*=(const scalar s) { return ((*this) = (*this) * s); }
constexpr inline vector& operator/=(const scalar s) { return ((*this) = (*this) / s); }

friend inline vector operator+(const scalar s, vector rhs) { return  rhs + s; }
friend inline vector operator-(const scalar s, vector rhs) { return -rhs + s; }
friend inline vector operator*(const scalar s, vector rhs) { return  rhs * s; }
friend inline vector operator/(const scalar s, vector rhs) { return (vector{s,s,s}/rhs); }

constexpr inline bool operator==(const vector& other) { return x == other.x && y == other.y && z == other.z; }
constexpr inline bool operator!=(const vector& other) { return !((*this) == other); }
constexpr inline bool operator<(const vector& other) { return x < other.x || y < other.y || z < other.z; }
};

// zero's out a vector
static inline void vm_MakeZero(vector *v) { *v = vector::ne(); }

using vector_array = vector;
typedef VM_ALIGN(alignof(scalar) * 4) vector aligned_vector;
using aligned_vector_array = aligned_vector;

struct alignas(alignof(scalar) * 4) vector4 {
using T = scalar;
constexpr static const size_t N = 4;
constexpr static const size_t X = 0;
constexpr static const size_t Y = 1;
constexpr static const size_t Z = 2;
constexpr static const size_t W = 3;
union {
  T xyzw[N];
  struct { T x, y, z; union { T w; T kat_pad; }; };
  struct { union { T r; T l; }; T g, b, a; };
  struct { T u,v; union { T u2; T s; }; union { T v2; T t; }; };
};
constexpr static inline const vector4 id(ssize_t i = -1)
{
  return vector4{
  ((i % N) == X) ? (T)1 : (T)0,
  ((i % N) == Y) ? (T)1 : (T)0,
  ((i % N) == Z) ? (T)1 : (T)0,
  ((i % N) == W) ? (T)1 : (T)0
  };
}
constexpr static inline const vector4 ne() { return vector4{ (T)0, (T)0, (T)0, (T)0 }; }
constexpr inline operator aligned_vector() { return *(aligned_vector*)this; }
constexpr inline operator const aligned_vector() const { return *(const aligned_vector*)this; }
constexpr inline T sum() const             { return x + y + z + w; }
constexpr inline vector4 operator-()      { return vector4{(T)-x, (T)-y, (T)-z, (T)-w}; }
constexpr inline vector4 operator*(vector4 other)  { return vector4{x * other.x,y * other.y,z * other.z,w * other.w}; }
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
//static inline float operator*(vector u, vector v) { return (u.x * v.x) + (u.y * v.y) + (u.z * v.z); }

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
static inline matrix operator/(matrix src, float n) {
  src.fvec = src.fvec / n;
  src.rvec = src.rvec / n;
  src.uvec = src.uvec / n;

  return src;
}

inline scalar vm_Dot3Product(const vector a, const vector b) { return (aligned_vector{a} * aligned_vector{b}).sum(); }
static inline scalar vm_Dot3Vector(scalar x, scalar y, scalar z, vector *v) { return vm_Dot3Product(aligned_vector{x,y,z}, *(aligned_vector*)v); }
inline vector vm_Cross3Product(vector u, vector v) {
  return aligned_vector{u.y*v.z, u.z*v.x, u.x*v.y}
       - aligned_vector{v.y*u.z, v.z*u.x, v.x*u.y};
}

// Scalar division
static inline matrix operator/=(matrix &src, float n) { return (src = src / n); }

#define vm_GetSurfaceNormal vm_GetNormal

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

// Apply a matrix to a vector
static inline vector operator*(vector v, matrix m) {
  vector result;

  result.x = vm_Dot3Product(v,m.rvec);
  result.y = vm_Dot3Product(v,m.uvec);
  result.z = vm_Dot3Product(v,m.fvec);

  return result;
}
