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
#include <array>
#include <numeric>
#include <algorithm>
#include <limits>

static constexpr inline bool VM_ISPOW2(uintmax_t x) { return (x && (!(x & (x-1)))); }

template<typename T>
static constexpr inline int VM_BIT_SCAN_REVERSE_CONST(const T n) noexcept {
    if (n == 0) return -1;
	if (VM_ISPOW2(n)) return n;
    T a = n, b = 0, j = std::numeric_limits<T>::digits, k = 0;
    do {
        j >>= 1;
        k = (T)1 << j;
        if (a >= k) {
            a >>= j;
            b += j;
        }
    } while (j > 0);
    return int(b);
}

template<typename T>
static constexpr inline T VM_BIT_CEIL(T x) noexcept { return T(1) << (VM_BIT_SCAN_REVERSE_CONST((T)x-1) + 1); };

template<typename T>
static constexpr inline T VM_BIT_FLOOR(T x) noexcept { return x == 0 || VM_ISPOW2(x) ? x : ((VM_BIT_FLOOR(x >> 1)) << 1); }

#ifdef _MSVC_LANG
#define VM_ALIGN(n) __declspec(align(n))
#else
#define VM_ALIGN(n) __attribute__((aligned(n)))
#endif

enum class align
{
	none     = 0 << 0,
	scalar   = 1 << 1,
	vector   = 2 << 1,
	matrix   = 3 << 1,
	adaptive = 4 << 1,
};

template<typename T, size_t N, enum align A = align::adaptive, size_t N_POW2 = VM_BIT_CEIL(N)>
struct alignas((N==N_POW2 && A != align::scalar) || A == align::vector ? alignof(T) * N_POW2 : alignof(T)) vec : std::array<T,N> {

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
constexpr static inline const vec<T,N,A> ne() { vec<T,N,A> dst = {}; return dst; }

template<enum align A_DST = align::adaptive>
constexpr inline const vec<T,3,A_DST> yzx() const { return vec<T,3,A_DST>{ y(), z(), x() }; }
template<enum align A_DST = align::adaptive>
constexpr inline const vec<T,3,A_DST> zxy() const { return vec<T,3,A_DST>{ z(), x(), y() }; }
template<enum align A_DST = align::adaptive>
constexpr inline const vec<T,3,A_DST> hbp() const { return vec<T,3,A_DST>{ h(), b(), p() }; }
template<enum align A_DST = align::adaptive>
constexpr inline const vec<T,3,A_DST> bph() const { return vec<T,3,A_DST>{ b(), p(), h() }; }

constexpr inline T sum() const                    { return std::accumulate((*this).cbegin(), (*this).cend(), (T)0); }
constexpr inline vec<T,3,A> operator-() const     { vec<T,N,A> neg = {}; neg.fill((T)-1); return (*this) * neg; }

/* arithmetic vector */
template<typename T_OTHER, size_t N_OTHER, enum align A_OTHER = align::adaptive, typename T_DST = decltype((T)1 + (T_OTHER)1)>
constexpr inline vec<T_DST,N,A> operator+(const vec<T_OTHER,N_OTHER,A_OTHER> other) const
{
	vec<T_DST,N,A> dst = {};
	std::transform((*this).cbegin(), (*this).cbegin() + std::min<size_t>(N_OTHER,N), other.cbegin(), dst.begin(), std::plus<>{});
	return dst;
}
template<typename T_OTHER, size_t N_OTHER, enum align A_OTHER = align::adaptive, typename T_DST = decltype((T)1 - (T_OTHER)1)>
constexpr inline vec<T_DST,N,A> operator-(const vec<T_OTHER,N_OTHER,A_OTHER> other) const
{
	vec<T_DST,N,A> dst = {};
	std::transform((*this).cbegin(), (*this).cbegin() + std::min<size_t>(N_OTHER,N), other.cbegin(), dst.begin(), std::minus<>{});
	return dst;
}
template<typename T_OTHER, size_t N_OTHER, enum align A_OTHER = align::adaptive, typename T_DST = decltype((T)1 * (T_OTHER)1)>
constexpr inline vec<T_DST,N,A> operator*(const vec<T_OTHER,N_OTHER,A_OTHER> other) const
{
	vec<T_DST,N,A> dst = {};
	std::transform((*this).cbegin(), (*this).cbegin() + std::min<size_t>(N_OTHER,N), other.cbegin(), dst.begin(), std::multiplies<>{});
	return dst;
}
template<typename T_OTHER, size_t N_OTHER, enum align A_OTHER = align::adaptive, typename T_DST = decltype((T)1 / (T_OTHER)1)>
constexpr inline vec<T_DST,N,A> operator/(const vec<T_OTHER,N_OTHER,A_OTHER> other) const
{
	vec<T_DST,N,A> dst = {};
	std::transform((*this).cbegin(), (*this).cbegin() + std::min<size_t>(N_OTHER,N), other.cbegin(), dst.begin(), [](const T& a, const T& b) { return (T)a*(T)1/b; });
	return dst;
}
/* arithmetic scalar */
template<typename T_OTHER, typename T_DST = decltype((T)1 + (T_OTHER)1)>
constexpr inline vec<T_DST,N,A> operator+(const T_OTHER other) const
{
	vec<T_DST,N,A> dst = {};
	dst.fill((T)other);
	std::transform((*this).cbegin(), (*this).cend(), dst.cbegin(), dst.begin(), std::plus<>{});
	return dst;
}
template<typename T_OTHER, typename T_DST = decltype((T)1 - (T_OTHER)1)>
constexpr inline vec<T_DST,N,A> operator-(const T_OTHER other) const
{
	vec<T_DST,N,A> dst = {};
	dst.fill((T)other);
	std::transform((*this).cbegin(), (*this).cend(), dst.cbegin(), dst.begin(), std::minus<>{});
	return dst;
}
template<typename T_OTHER, typename T_DST = decltype((T)1 * (T_OTHER)1)>
constexpr inline vec<T_DST,N,A> operator*(const T_OTHER other) const
{
	vec<T_DST,N,A> dst = {};
	dst.fill((T)other);
	std::transform((*this).cbegin(), (*this).cend(), dst.cbegin(), dst.begin(), std::multiplies<>{});
	return dst;
}
template<typename T_OTHER, typename T_DST = decltype((T)1 / (T_OTHER)1)>
constexpr inline vec<T_DST,N,A> operator/(const T_OTHER other) const
{
	vec<T_DST,N,A> dst = {};
	dst.fill((T)1/other);
	std::transform((*this).cbegin(), (*this).cend(), dst.cbegin(), dst.begin(), std::multiplies<>{});
	return dst;
}

/* arithmetic assign vector */
template<typename T_OTHER, size_t N_OTHER, enum align A_OTHER = align::adaptive>
constexpr inline vec<T,N,A> operator+=(const vec<T_OTHER,N_OTHER,A_OTHER> other)
{
	std::transform((*this).cbegin(), (*this).cbegin() + std::min<size_t>(N_OTHER,N), other.cbegin(), (*this).begin(), std::plus<>{});
	return (*this);
}
template<typename T_OTHER, size_t N_OTHER, enum align A_OTHER = align::adaptive>
constexpr inline vec<T,N,A> operator-=(const vec<T_OTHER,N_OTHER,A_OTHER> other)
{
	std::transform((*this).cbegin(), (*this).cbegin() + std::min<size_t>(N_OTHER,N), other.cbegin(), (*this).begin(), std::minus<>{});
	return (*this);
}
template<typename T_OTHER, size_t N_OTHER, enum align A_OTHER = align::adaptive>
constexpr inline vec<T,N,A> operator*=(const vec<T_OTHER,N_OTHER,A_OTHER> other)
{
	std::transform((*this).cbegin(), (*this).cbegin() + std::min<size_t>(N_OTHER,N), other.cbegin(), (*this).begin(), std::multiplies<>{});
	return (*this);
}
template<typename T_OTHER, size_t N_OTHER, enum align A_OTHER = align::adaptive>
constexpr inline vec<T,N,A> operator/=(const vec<T_OTHER,N_OTHER,A_OTHER> other)
{
	std::transform((*this).cbegin(), (*this).cbegin() + std::min<size_t>(N_OTHER,N), other.cbegin(), (*this).begin(), [](const T& a, const T& b) { return (T)a*(T)1/b; });
	return (*this);
}
/* arithmetic assign scalar */
template<typename T_OTHER>
constexpr inline vec<T,N,A> operator+=(const T_OTHER other)
{
	vec<T,N,A> dst = {};
	dst.fill((T)other);
	std::transform((*this).cbegin(), (*this).cend(), dst.cbegin(), (*this).begin(), std::plus<>{});
	return (*this);
}
template<typename T_OTHER>
constexpr inline vec<T,N,A> operator-=(const T_OTHER other)
{
	vec<T,N,A> dst = {};
	dst.fill((T)other);
	std::transform((*this).cbegin(), (*this).cend(), dst.cbegin(), (*this).begin(), std::minus<>{});
	return (*this);
}
template<typename T_OTHER>
constexpr inline vec<T,N,A> operator*=(const T_OTHER other)
{
	vec<T,N,A> dst = {};
	dst.fill((T)other);
	std::transform((*this).cbegin(), (*this).cend(), dst.cbegin(), (*this).begin(), std::multiplies<>{});
	return (*this);
}
template<typename T_OTHER>
constexpr inline vec<T,N,A> operator/=(const T_OTHER other)
{
	vec<T,N,A> dst = {};
	dst.fill((T)1/other);
	std::transform((*this).cbegin(), (*this).cend(), dst.cbegin(), (*this).begin(), std::multiplies<>{});
	return (*this);
}

/* commutative operators */
friend inline vec<T,N,A> operator+(const scalar s, vec<T,N,A> rhs) { return  rhs + s; }
friend inline vec<T,N,A> operator-(const scalar s, vec<T,N,A> rhs) { return -rhs + s; }
friend inline vec<T,N,A> operator*(const scalar s, vec<T,N,A> rhs) { return  rhs * s; }
friend inline vec<T,N,A> operator/(const scalar s, vec<T,N,A> rhs) { vec<T,N,A> tmp = {}; tmp.fill(s); return tmp/rhs; }

template<size_t N_A = 3, size_t N_B = 3, enum align A_A = align::adaptive, enum align A_B = align::adaptive>
constexpr static inline scalar dot(vec<T,N_A,A_A> a, vec<T,N_B,A_B> b) { return (a * b).sum(); }

template<size_t N_A = 3, size_t N_B = 3, enum align A_A = align::adaptive, enum align A_B = align::adaptive>
constexpr static inline vec<T,3> cross3(vec<T,N_A,A_A> a, vec<T,N_B,A_B> b)
{
  return vec<T,3,align::vector>{a.y()*b.z(), a.z()*b.x(), a.x()*b.y()}
       - vec<T,3,align::vector>{b.y()*a.z(), b.z()*a.x(), b.x()*a.y()};
}
constexpr inline scalar mag() const { return (scalar)sqrt(dot((*this),(*this))); }

template<size_t N_A = 3, size_t N_B = 3, enum align A_A = align::adaptive, enum align A_B = align::adaptive>
static constexpr inline scalar distance(vec<T,N_A,A_A> a, vec<T,N_B,A_B> b) { return (a - b).mag(); }


};

using vector                = vec<scalar,3>;
using vector_array          = vector;
using aligned_vector        = vec<scalar,3,align::vector>;
using aligned_vector_array  = aligned_vector;

using vector4               = vec<scalar,4>;
using vector4_array         = vector4;
using aligned_vector4       = vec<scalar,4,align::vector>; 
using aligned_vector4_array = aligned_vector4;

using angvec                = vec<angle,3>;
using angvec_array          = angvec;
using aligned_angvec        = vec<angle,3,align::vector>;
using aligned_angvec_array  = aligned_angvec;

// Set an angvec to {0,0,0}
static inline void vm_MakeZero(angvec *a) { *a = angvec::ne(); }
// zero's out a vector
static inline void vm_MakeZero(vector *v) { *v = vector::ne(); }

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
  return matrix{ {{vector::id(0), vector::id(1), vector::id(2)}} };
}
constexpr static inline const matrix ne()
{
  return matrix{ {{vector::ne(), vector::ne(), vector::ne()}} };
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
  return matrix4{ {{vector4::id(0), vector4::id(1), vector4::id(2), vector4::id(3)}} };
}
constexpr static inline const matrix4 ne()
{
  return matrix4{ {{vector4::ne(), vector4::ne(), vector4::ne(), vector4::ne()}} };
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

inline scalar vm_Dot3Product(const vector a, const vector b) { return vector::dot(a,b); }
static inline scalar vm_Dot3Vector(scalar x, scalar y, scalar z, vector *v) { return vector::dot(aligned_vector{x,y,z}, *(aligned_vector*)v); }

inline vector vm_Cross3Product(vector u, vector v) {
  return vector::cross3(u,v);
}

// Scalar division
static inline matrix operator/=(matrix &src, float n) { return (src = src / n); }

#define vm_GetSurfaceNormal vm_GetNormal

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

// Apply a matrix to a vector
static inline vector operator*(vector v, matrix m) {
  return { vector::dot(v,m.rvec), vector::dot(v,m.uvec), vector::dot(v,m.fvec) };
}
