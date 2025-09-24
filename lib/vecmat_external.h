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
#include <algorithm>
#include <array>
#include <functional>
#include <limits>
#include <numeric>

#define BITOP_RUP01__(x) (             (x) | (             (x) >>  1))
#define BITOP_RUP02__(x) (BITOP_RUP01__(x) | (BITOP_RUP01__(x) >>  2))
#define BITOP_RUP04__(x) (BITOP_RUP02__(x) | (BITOP_RUP02__(x) >>  4))
#define BITOP_RUP08__(x) (BITOP_RUP04__(x) | (BITOP_RUP04__(x) >>  8))
#define BITOP_RUP16__(x) (BITOP_RUP08__(x) | (BITOP_RUP08__(x) >> 16))
#define BITOP_RUP32__(x) (BITOP_RUP16__(x) | (BITOP_RUP16__(x) >> 32))
#define BITOP_RUP64__(x) (BITOP_RUP32__(x) | (BITOP_RUP32__(x) >> 64))

constexpr static inline uintmax_t VM_BIT_CEIL (uintmax_t x) {
switch(std::numeric_limits<uintmax_t>::digits)
{
	case   8: return (BITOP_RUP04__(uint8_t (x) - 1) + 1);
	case  16: return (BITOP_RUP08__(uint16_t(x) - 1) + 1);
	default:
	case  32: return (BITOP_RUP16__(uint32_t(x) - 1) + 1);
	case  64: return (BITOP_RUP32__(uint64_t(x) - 1) + 1);
}
}

constexpr static inline bool      VM_ISPOW2   (uintmax_t x) { return x && (!(x & (x-1)));                                            }
constexpr static inline uintmax_t VM_BIT_FLOOR(uintmax_t x) { return ((x == 0) || VM_ISPOW2(x) ? x : ((VM_BIT_FLOOR(x >> 1)) << 1)); }

enum class align
{
	none     = 0 << 0,
	scalar   = 1 << 0,
	linear   = 1 << 1,
	matrix   = 1 << 2,
	adaptive = 1 << 3,
};

template<typename T, size_t N, enum align A = align::adaptive, size_t N_POW2 = VM_BIT_CEIL(N), size_t T_S = std::max<size_t>(alignof(T), sizeof(T))>
struct alignas((((N == N_POW2) && (A != align::scalar)) || ((A == align::linear) || (A == align::matrix))) ? N_POW2 * T_S : T_S) vec : std::array<T,N> {

template<size_t N_DST = N, enum align A_DST = align::adaptive>
operator vec<T,N_DST,A_DST>() { return *reinterpret_cast<vec<T,N_DST,A_DST>*>(this); }

constexpr inline T& x() { return (*this)[0]; }
constexpr inline T& y() { static_assert(N >= 2); return (*this)[1]; }
constexpr inline T& z() { static_assert(N >= 3); return (*this)[2]; }
constexpr inline T& w() { static_assert(N >= 4); return (*this)[3]; }

constexpr inline const T& x() const { return (*this)[0]; }
constexpr inline const T& y() const { static_assert(N >= 2); return (*this)[1]; }
constexpr inline const T& z() const { static_assert(N >= 3); return (*this)[2]; }
constexpr inline const T& w() const { static_assert(N >= 4); return (*this)[3]; }

constexpr inline T& p() { return (*this)[0]; }
constexpr inline T& h() { static_assert(N >= 2); return (*this)[1]; }
constexpr inline T& b() { static_assert(N >= 3); return (*this)[2]; }

constexpr inline const T& p() const { return (*this)[0]; }
constexpr inline const T& h() const { static_assert(N >= 2); return (*this)[1]; }
constexpr inline const T& b() const { static_assert(N >= 3); return (*this)[2]; }

constexpr inline T&  u() { return (*this)[0]; }
constexpr inline T&  v() { static_assert(N >= 2); return (*this)[1]; }
constexpr inline T& u2() { static_assert(N >= 3); return (*this)[2]; }
constexpr inline T& v2() { static_assert(N >= 4); return (*this)[3]; }
constexpr inline T&  s() { static_assert(N >= 3); return (*this)[2]; }
constexpr inline T&  t() { static_assert(N >= 4); return (*this)[3]; }
constexpr inline const T&  u() const { return (*this)[0]; }
constexpr inline const T&  v() const { static_assert(N >= 2); return (*this)[1]; }
constexpr inline const T& u2() const { static_assert(N >= 3); return (*this)[2]; }
constexpr inline const T& v2() const { static_assert(N >= 4); return (*this)[3]; }
constexpr inline const T&  s() const { static_assert(N >= 3); return (*this)[2]; }
constexpr inline const T&  t() const { static_assert(N >= 4); return (*this)[3]; }

constexpr inline T& l() { return (*this)[0]; }
constexpr inline T& r() { return (*this)[0]; }
constexpr inline T& g() { static_assert(N >= 2); return (*this)[1]; }
constexpr inline T& a() { static_assert(N >= 4); return (*this)[3]; }
constexpr inline const T& l() const { return (*this)[0]; }
constexpr inline const T& r() const { return (*this)[0]; }
constexpr inline const T& g() const { static_assert(N >= 2); return (*this)[1]; }
constexpr inline const T& a() const { static_assert(N >= 4); return (*this)[3]; }

constexpr static inline const vec<T,N,A> id(ssize_t i = -1) { vec<T,N> dst = {}; if(i == -1) dst.fill(1); else dst[i % N] = (T)1; return dst; }

template<enum align A_DST = align::adaptive>
constexpr inline const vec<T,3,A_DST> yzx() const { return vec<T,3,A_DST>{ y(), z(), x() }; }
template<enum align A_DST = align::adaptive>
constexpr inline const vec<T,3,A_DST> zxy() const { return vec<T,3,A_DST>{ z(), x(), y() }; }
template<enum align A_DST = align::adaptive>
constexpr inline const vec<T,3,A_DST> hbp() const { return vec<T,3,A_DST>{ h(), b(), p() }; }
template<enum align A_DST = align::adaptive>
constexpr inline const vec<T,3,A_DST> bph() const { return vec<T,3,A_DST>{ b(), p(), h() }; }

constexpr inline T sum() const                    { return std::accumulate(this->cbegin(), this->cend(), T{}); }
constexpr inline vec<T,N,A> operator-() const
{
	vec<T,N,A> dst;
	std::transform(this->cbegin(), this->cbegin() + N, dst.begin(), std::negate<>{});
	return dst;
}

/* arithmetic vector */
template<typename T_OTHER, size_t N_OTHER, enum align A_OTHER = align::adaptive, typename T_DST = decltype((T)1 + (T_OTHER)1)>
constexpr inline vec<T_DST,N,A> operator+(const vec<T_OTHER,N_OTHER,A_OTHER> &other) const
{
	vec<T_DST,N,A> dst = {};
	std::transform(this->cbegin(), this->cbegin() + std::min<size_t>(N_OTHER,N), other.cbegin(), dst.begin(), std::plus<>{});
	return dst;
}
template<typename T_OTHER, size_t N_OTHER, enum align A_OTHER = align::adaptive, typename T_DST = decltype((T)1 - (T_OTHER)1)>
constexpr inline vec<T_DST,N,A> operator-(const vec<T_OTHER,N_OTHER,A_OTHER> &other) const
{
	vec<T_DST,N,A> dst = {};
	std::transform(this->cbegin(), this->cbegin() + std::min<size_t>(N_OTHER,N), other.cbegin(), dst.begin(), std::minus<>{});
	return dst;
}
template<typename T_OTHER, size_t N_OTHER, enum align A_OTHER = align::adaptive, typename T_DST = decltype((T)1 * (T_OTHER)1)>
constexpr inline vec<T_DST,N,A> operator*(const vec<T_OTHER,N_OTHER,A_OTHER> &other) const
{
	vec<T_DST,N,A> dst = {};
	std::transform(this->cbegin(), this->cbegin() + std::min<size_t>(N_OTHER,N), other.cbegin(), dst.begin(), std::multiplies<>{});
	return dst;
}
template<typename T_OTHER, size_t N_OTHER, enum align A_OTHER = align::adaptive, typename T_DST = decltype((T)1 / (T_OTHER)1)>
constexpr inline vec<T_DST,N,A> operator/(const vec<T_OTHER,N_OTHER,A_OTHER> &other) const
{
	vec<T_DST,N,A> dst = {};
	std::transform(this->cbegin(), this->cbegin() + std::min<size_t>(N_OTHER,N), other.cbegin(), dst.begin(), [](const T& a, const T& b) { return (T)a*(T)1/b; });
	return dst;
}
/* arithmetic scalar */
template<typename T_OTHER, typename T_DST = decltype((T)1 + (T_OTHER)1)>
constexpr inline vec<T_DST,N,A> operator+(const T_OTHER &other) const
{
	vec<T_DST,N,A> dst = {};
	dst.fill((T)other);
	std::transform(this->cbegin(), this->cend(), dst.cbegin(), dst.begin(), std::plus<>{});
	return dst;
}
template<typename T_OTHER, typename T_DST = decltype((T)1 - (T_OTHER)1)>
constexpr inline vec<T_DST,N,A> operator-(const T_OTHER &other) const
{
	vec<T_DST,N,A> dst = {};
	dst.fill((T)other);
	std::transform(this->cbegin(), this->cend(), dst.cbegin(), dst.begin(), std::minus<>{});
	return dst;
}
template<typename T_OTHER, typename T_DST = decltype((T)1 * (T_OTHER)1)>
constexpr inline vec<T_DST,N,A> operator*(const T_OTHER &other) const
{
	vec<T_DST,N,A> dst = {};
	dst.fill((T)other);
	std::transform(this->cbegin(), this->cend(), dst.cbegin(), dst.begin(), std::multiplies<>{});
	return dst;
}
template<typename T_OTHER, typename T_DST = decltype((T)1 / (T_OTHER)1)>
constexpr inline vec<T_DST,N,A> operator/(const T_OTHER &other) const
{
	vec<T_DST,N,A> dst = {};
	dst.fill((T)1/other);
	std::transform(this->cbegin(), this->cend(), dst.cbegin(), dst.begin(), std::multiplies<>{});
	return dst;
}

/* arithmetic assign vector */
template<typename T_OTHER, size_t N_OTHER, enum align A_OTHER = align::adaptive>
constexpr inline vec<T,N,A> operator+=(const vec<T_OTHER,N_OTHER,A_OTHER> &other)
{
	std::transform(this->cbegin(), this->cbegin() + std::min<size_t>(N_OTHER,N), other.cbegin(), this->begin(), std::plus<>{});
	return (*this);
}
template<typename T_OTHER, size_t N_OTHER, enum align A_OTHER = align::adaptive>
constexpr inline vec<T,N,A> operator-=(const vec<T_OTHER,N_OTHER,A_OTHER> &other)
{
	std::transform(this->cbegin(), this->cbegin() + std::min<size_t>(N_OTHER,N), other.cbegin(), this->begin(), std::minus<>{});
	return (*this);
}
template<typename T_OTHER, size_t N_OTHER, enum align A_OTHER = align::adaptive>
constexpr inline vec<T,N,A> operator*=(const vec<T_OTHER,N_OTHER,A_OTHER> &other)
{
	std::transform(this->cbegin(), this->cbegin() + std::min<size_t>(N_OTHER,N), other.cbegin(), this->begin(), std::multiplies<>{});
	return (*this);
}
template<typename T_OTHER, size_t N_OTHER, enum align A_OTHER = align::adaptive>
constexpr inline vec<T,N,A> operator/=(const vec<T_OTHER,N_OTHER,A_OTHER> &other)
{
	std::transform(this->cbegin(), this->cbegin() + std::min<size_t>(N_OTHER,N), other.cbegin(), this->begin(), [](const T& a, const T& b) { return (T)a*(T)1/b; });
	return (*this);
}
/* arithmetic assign scalar */
template<typename T_OTHER>
constexpr inline vec<T,N,A> operator+=(const T_OTHER &other)
{
	vec<T,N,A> dst = {};
	dst.fill((T)other);
	std::transform(this->cbegin(), this->cend(), dst.cbegin(), this->begin(), std::plus<>{});
	return (*this);
}
template<typename T_OTHER>
constexpr inline vec<T,N,A> operator-=(const T_OTHER &other)
{
	vec<T,N,A> dst = {};
	dst.fill((T)other);
	std::transform(this->cbegin(), this->cend(), dst.cbegin(), this->begin(), std::minus<>{});
	return (*this);
}
template<typename T_OTHER>
constexpr inline vec<T,N,A> operator*=(const T_OTHER &other)
{
	vec<T,N,A> dst = {};
	dst.fill((T)other);
	std::transform(this->cbegin(), this->cend(), dst.cbegin(), this->begin(), std::multiplies<>{});
	return (*this);
}
template<typename T_OTHER>
constexpr inline vec<T,N,A> operator/=(const T_OTHER &other)
{
	vec<T,N,A> dst = {};
	dst.fill((T)1/other);
	std::transform(this->cbegin(), this->cend(), dst.cbegin(), this->begin(), std::multiplies<>{});
	return (*this);
}

/* commutative operators */
friend inline vec<T,N,A> operator+(const scalar s, const vec<T,N,A> &rhs) { return  rhs + s; }
friend inline vec<T,N,A> operator-(const scalar s, const vec<T,N,A> &rhs) { return -rhs + s; }
friend inline vec<T,N,A> operator*(const scalar s, const vec<T,N,A> &rhs) { return  rhs * s; }
friend inline vec<T,N,A> operator/(const scalar s, const vec<T,N,A> &rhs) { vec<T,N,A> tmp = {}; tmp.fill(s); return tmp/rhs; }

template<size_t N_A = 3, size_t N_B = 3, enum align A_A = align::adaptive, enum align A_B = align::adaptive>
constexpr static inline scalar dot(const vec<T,N_A,A_A> &a, const vec<T,N_B,A_B> &b) { return (a * b).sum(); }

template<size_t N_A = 3, size_t N_B = 3, enum align A_A = align::adaptive, enum align A_B = align::adaptive>
constexpr static inline vec<T,3> cross3(const vec<T,N_A,A_A> &a, const vec<T,N_B,A_B> &b)
{
  return vec<T,3,align::linear>{a.y()*b.z(), a.z()*b.x(), a.x()*b.y()}
       - vec<T,3,align::linear>{b.y()*a.z(), b.z()*a.x(), b.x()*a.y()};
}
constexpr inline scalar mag() const { return (scalar)sqrt(dot((*this),(*this))); }

template<size_t N_A = 3, size_t N_B = 3, enum align A_A = align::adaptive, enum align A_B = align::adaptive>
static constexpr inline scalar distance(const vec<T,N_A,A_A> &a, const vec<T,N_B,A_B> &b) { return (a - b).mag(); }


};

using vector                = vec<scalar,3>;
using vector_array          = vector;
using aligned_vector        = vec<scalar,3,align::linear>;
using aligned_vector_array  = aligned_vector;

using vector4               = vec<scalar,4>;
using vector4_array         = vector4;
using aligned_vector4       = vec<scalar,4,align::linear>; 
using aligned_vector4_array = aligned_vector4;

using angvec                = vec<angle,3>;
using angvec_array          = angvec;
using aligned_angvec        = vec<angle,3,align::linear>;
using aligned_angvec_array  = aligned_angvec;

// Set an angvec to {0,0,0}
static inline void vm_MakeZero(angvec *a) { *a = angvec{}; }
// zero's out a vector
static inline void vm_MakeZero(vector *v) { *v = vector{}; }

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
  return matrix{ vector{}, vector{}, vector{} };
}
constexpr inline void set_col(size_t i, const vector v)
{
	a2d[0][i % 3] = v[0];
	a2d[1][i % 3] = v[1];
	a2d[2][i % 3] = v[2];
}
constexpr inline vector col(size_t i) const
{
	return vector{ a2d[0][i % 3], a2d[1][i % 3], a2d[2][i % 3] };
}
constexpr inline matrix transposed() const
{
	return matrix{ col(0), col(1), col(2) };
}
constexpr inline scalar det() const
{
	scalar dst = scalar(0);
	for(size_t i = 0; i < 3; i++)
		dst += a2d[0][i] * (a2d[1][(i+1) % 3] * a2d[2][(i+2) % 3]
		                  - a2d[1][(i+2) % 3] * a2d[2][(i+1) % 3]);
  	return dst;
}
};

constexpr matrix IDENTITY_MATRIX = { vector::id(0), vector::id(1), vector::id(2) };

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
  return matrix4{ vector4{}, vector4{}, vector4{}, vector4{} };
}
constexpr inline void set_col(size_t i, const vector4 v)
{
	a2d[0][i % 4] = v[0];
	a2d[1][i % 4] = v[1];
	a2d[2][i % 4] = v[2];
	a2d[3][i % 4] = v[3];
}
constexpr inline vector4 col(size_t i) const
{
	return vector4{ a2d[0][i % 4], a2d[1][i % 4], a2d[2][i % 4], a2d[2][i % 4] };
}
constexpr inline matrix4 transposed() const
{
	return matrix4{ col(0), col(1), col(2), col(3) };
}
};

// Adds 2 matrices
static inline matrix operator+(matrix a, const matrix &b) {
  // Adds two 3x3 matrixs.

  a.rvec += b.rvec;
  a.uvec += b.uvec;
  a.fvec += b.fvec;

  return a;
}

// Adds 2 matrices
static inline matrix operator+=(matrix &a, const matrix &b) { return (a = a + b); }
// Subtracts 2 matrices
static inline matrix operator-(matrix a, const matrix &b) {
  // subtracts two 3x3 matrices

  a.rvec = a.rvec - b.rvec;
  a.uvec = a.uvec - b.uvec;
  a.fvec = a.fvec - b.fvec;

  return a;
}

// Subtracts 2 matrices
static inline matrix operator-=(matrix &a, const matrix &b) { return (a = a - b); }

// Does a simple dot product calculation
//static inline float operator*(const vector &u, const vector &v) { return (u.x * v.x) + (u.y * v.y) + (u.z * v.z); }

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

inline scalar vm_Dot3Product(const vector &a, const vector &b) { return vector::dot(a,b); }
static inline scalar vm_Dot3Vector(scalar x, scalar y, scalar z, const vector *v) { return vector::dot(aligned_vector{x,y,z}, *v); }

inline vector vm_Cross3Product(const vector &u, const vector &v) {
  return vector::cross3(u,v);
}

// Scalar division
static inline matrix operator/=(matrix &src, float n) { return (src = src / n); }

#define vm_GetSurfaceNormal vm_GetNormal

// Matrix transpose
static inline matrix operator~(matrix m) {
  return m.transposed();
}

// Apply a matrix to a vector
static inline vector operator*(const vector &v, const matrix &m) {
  return { vector::dot(v,m.rvec), vector::dot(v,m.uvec), vector::dot(v,m.fvec) };
}
