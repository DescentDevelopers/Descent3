/*
 * Descent 3
 * Copyright (C) 2026 Descent Developers
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

#include <cmath>
#include <random>
#include <vector>

#include <gtest/gtest.h>

#include "osiris_vector.h"

#include "support/dallas_vector_helpers.h"

namespace {

using namespace dallas_fakes;

class FixTables : public ::testing::Environment {
public:
  void SetUp() override { InitMathTables(); }
};
const auto *const g_fix_env = ::testing::AddGlobalTestEnvironment(new FixTables);

// Angles are 16-bit: 0x10000 == 360 degrees.
constexpr int kDeg90 = 0x4000;
constexpr double kPi = 3.14159265358979323846;

const matrix kIdentity{V(1, 0, 0), V(0, 1, 0), V(0, 0, 1)};
const matrix kGeneralA{V(1, 2, 3), V(-4, 5, 0.5f), V(7, -8, 9)};
const matrix kGeneralB{V(0.5f, -1, 2), V(3, 0, -2), V(1, 1, 1)};

TEST(DallasVector, NormalizeReturnsMagnitudeAndUnitVector) {
  vector v = V(3, 4, 12);
  EXPECT_NEAR(vm_VectorNormalize(&v), 13.0f, kEps);
  ExpectVecNear(v, V(3.0f / 13, 4.0f / 13, 12.0f / 13));
}

TEST(DallasVector, NormalizeZeroVectorReturnsZeroWithoutNaN) {
  vector v = V(0, 0, 0);
  EXPECT_EQ(vm_VectorNormalize(&v), 0.0f);
  EXPECT_TRUE(IsFinite(v));

  vector f = V(0, 0, 0);
  EXPECT_EQ(vm_VectorNormalizeFast(&f), 0.0f);
  EXPECT_TRUE(IsFinite(f));
}

TEST(DallasVector, FastMagnitudeIgnoresComponentOrderAndSign) {
  const float c[3] = {1.0f, 2.5f, 7.0f};
  const int perms[6][3] = {{0, 1, 2}, {0, 2, 1}, {1, 0, 2}, {1, 2, 0}, {2, 0, 1}, {2, 1, 0}};
  vector ref = V(c[0], c[1], c[2]);
  const float expected = vm_GetMagnitudeFast(&ref);
  for (const auto &p : perms) {
    for (int signs = 0; signs < 8; signs++) {
      vector v = V((signs & 1 ? -1 : 1) * c[p[0]], (signs & 2 ? -1 : 1) * c[p[1]], (signs & 4 ? -1 : 1) * c[p[2]]);
      EXPECT_FLOAT_EQ(vm_GetMagnitudeFast(&v), expected) << v.x() << "," << v.y() << "," << v.z();
    }
  }
}

TEST(DallasVector, FastMagnitudeApproximatesMagnitude) {
  std::mt19937 rng(1234);
  std::uniform_real_distribution<float> d(-100.0f, 100.0f);
  for (int i = 0; i < 1000; i++) {
    vector v = V(d(rng), d(rng), d(rng));
    const float exact = Len(v);
    EXPECT_NEAR(vm_GetMagnitudeFast(&v), exact, 0.10f * exact);

    vector n = v;
    const float mag = vm_VectorNormalizeFast(&n);
    EXPECT_NEAR(mag, exact, 0.10f * exact);
    ExpectVecNear(Cross(n, v) / exact, V(0, 0, 0), 1e-5f);
    EXPECT_GT(Dot(n, v), 0.0f);
  }
}

TEST(DallasVector, NormalizedDirPointsFromStartToEnd) {
  const vector start = V(1, 1, 1);
  const vector end = V(1, 5, 4);
  vector dir;
  EXPECT_NEAR(vm_GetNormalizedDir(&dir, &end, &start), 5.0f, kEps);
  ExpectVecNear(dir, V(0, 0.8f, 0.6f));
}

TEST(DallasVector, DeterminantOfKnownMatrix) {
  // Cofactor expansion along the first row: 2*(3*4 - 2*1) - (-1)*(1*4 - 2*0) + 0 = 24.
  const matrix m{V(2, -1, 0), V(1, 3, 2), V(0, 1, 4)};
  EXPECT_NEAR(calc_det_value(&m), 24.0f, kEps);
  // Swapping two rows negates the determinant.
  const matrix swapped{m.uvec, m.rvec, m.fvec};
  EXPECT_NEAR(calc_det_value(&swapped), -24.0f, kEps);
  EXPECT_NEAR(calc_det_value(&kIdentity), 1.0f, kEps);
}

TEST(DallasVector, TransposeSwapsRowsAndColumns) {
  matrix t = kGeneralA;
  vm_TransposeMatrix(&t);
  const matrix expected{V(1, -4, 7), V(2, 5, -8), V(3, 0.5f, 9)};
  ExpectMatNear(t, expected, 0.0f);
}

TEST(DallasVector, MatrixOperatorsAgreeWithMatrixMul) {
  matrix expected;
  vm_MatrixMul(&expected, &kGeneralA, &kGeneralB);
  ExpectMatNear(kGeneralA * kGeneralB, expected);
  matrix m = kGeneralA;
  m *= kGeneralB;
  ExpectMatNear(m, expected);
}

TEST(DallasVector, MatrixMulComposesTransforms) {
  const vector v = V(0.3f, -2.0f, 5.0f);
  matrix ab;
  vm_MatrixMul(&ab, &kGeneralA, &kGeneralB);

  vector by_a, by_a_then_b, by_ab;
  vm_MatrixMulVector(&by_a, &v, &kGeneralA);
  vm_MatrixMulVector(&by_a_then_b, &by_a, &kGeneralB);
  vm_MatrixMulVector(&by_ab, &v, &ab);
  ExpectVecNear(by_ab, by_a_then_b, 1e-3f);
}

TEST(DallasVector, MatrixMulTMatrixMultipliesByTranspose) {
  matrix bt = kGeneralB;
  vm_TransposeMatrix(&bt);
  matrix expected, actual;
  vm_MatrixMul(&expected, &kGeneralA, &bt);
  vm_MatrixMulTMatrix(&actual, &kGeneralA, &kGeneralB);
  ExpectMatNear(actual, expected, 1e-4f);
}

TEST(DallasVector, RotationTimesItsTransposeIsIdentity) {
  const matrix m = SomeRotation();
  ExpectRotation(m, 1e-5f);
  matrix r;
  vm_MatrixMulTMatrix(&r, &m, &m);
  ExpectMatNear(r, kIdentity, 1e-4f);
}

TEST(DallasVector, VectorMulTMatrixUndoesMatrixMulVector) {
  const matrix m = SomeRotation();
  const vector v = V(4, -5, 6);
  vector rotated, back;
  vm_MatrixMulVector(&rotated, &v, &m);
  EXPECT_NEAR(Len(rotated), Len(v), 1e-4f);
  vm_VectorMulTMatrix(&back, &rotated, &m);
  ExpectVecNear(back, v, 1e-4f);
}

TEST(DallasVector, AnglesToMatrixOfZeroIsIdentity) {
  matrix m;
  vm_AnglesToMatrix(&m, 0, 0, 0);
  ExpectMatNear(m, kIdentity);
}

// Pitch is kept away from +-90 degrees, where heading and bank are not separable. Tolerance accounts
// for the 256-entry trig tables in fix.cpp (~0.3 degree).
TEST(DallasVector, AnglesMatrixRoundTrip) {
  const angle pitches[] = {0, 0x0800, 0x2000, 0x3000, 0xD000, 0xF800};
  const angle headings[] = {0, 0x1000, 0x4000, 0x7000, 0x9000, 0xC000, 0xFF00};
  const angle banks[] = {0, 0x0400, 0x4000, 0x8000, 0xB000};
  for (angle p : pitches) {
    for (angle h : headings) {
      for (angle b : banks) {
        matrix m;
        vm_AnglesToMatrix(&m, p, h, b);
        ExpectRotation(m, 1e-3f);
        angvec a;
        vm_ExtractAnglesFromMatrix(&a, &m);
        EXPECT_LE(std::abs(AngleDiff(a.p(), p)), 0x40) << std::hex << p << " " << h << " " << b;
        EXPECT_LE(std::abs(AngleDiff(a.h(), h)), 0x40) << std::hex << p << " " << h << " " << b;
        EXPECT_LE(std::abs(AngleDiff(a.b(), b)), 0x40) << std::hex << p << " " << h << " " << b;
      }
    }
  }
}

TEST(DallasVector, VectorToMatrixFromForwardOnly) {
  vector f = V(0, 0, 5);
  matrix m;
  vm_VectorToMatrix(&m, &f);
  ExpectMatNear(m, kIdentity);

  const vector dirs[] = {V(1, 2, 3), V(-4, 0.5f, -1), V(0, 1, 0), V(0, -3, 0), V(1, 0, 0), V(0.001f, 10, 0)};
  for (vector d : dirs) {
    vector in = d;
    vm_VectorToMatrix(&m, &in);
    SCOPED_TRACE(testing::Message() << d.x() << "," << d.y() << "," << d.z());
    ExpectRotation(m);
    ExpectVecNear(m.fvec, d / Len(d), 1e-5f);
  }
}

struct TwoVectorCase {
  vector primary;
  vector secondary;
};

const TwoVectorCase kTwoVectorCases[] = {
    {V(0, 0, 1), V(0, 1, 0)},
    {V(1, 2, 3), V(0, 1, 0)},
    {V(-3, 1, 0.5f), V(1, 1, 1)},
    {V(0, 1, 0), V(1, 0, 0.2f)},
};

TEST(DallasVector, VectorToMatrixFromForwardAndUp) {
  for (const auto &c : kTwoVectorCases) {
    vector f = c.primary, u = c.secondary;
    matrix m;
    vm_VectorToMatrix(&m, &f, &u, nullptr);
    ExpectRotation(m);
    ExpectVecNear(m.fvec, c.primary / Len(c.primary));
    EXPECT_GT(Dot(m.uvec, c.secondary), 0.0f);
    EXPECT_NEAR(Dot(m.rvec, c.secondary), 0.0f, 1e-4f) << "up vector should lie in the forward/up plane";
  }
}

TEST(DallasVector, VectorToMatrixFromForwardAndRight) {
  for (const auto &c : kTwoVectorCases) {
    vector f = c.primary, r = c.secondary;
    matrix m;
    vm_VectorToMatrix(&m, &f, nullptr, &r);
    ExpectRotation(m);
    ExpectVecNear(m.fvec, c.primary / Len(c.primary));
    EXPECT_GT(Dot(m.rvec, c.secondary), 0.0f);
    EXPECT_NEAR(Dot(m.uvec, c.secondary), 0.0f, 1e-4f) << "right vector should lie in the forward/right plane";
  }
}

TEST(DallasVector, VectorToMatrixFromUpOnlyOrRightOnly) {
  for (const auto &c : kTwoVectorCases) {
    vector u = c.primary;
    matrix m;
    vm_VectorToMatrix(&m, nullptr, &u, nullptr);
    ExpectRotation(m);
    ExpectVecNear(m.uvec, c.primary / Len(c.primary));

    vector r = c.primary;
    vm_VectorToMatrix(&m, nullptr, nullptr, &r);
    ExpectRotation(m);
    ExpectVecNear(m.rvec, c.primary / Len(c.primary));
  }
  vector up = V(0, 1, 0), right = V(1, 0, 0);
  matrix m;
  vm_VectorToMatrix(&m, nullptr, &up, &right);
  ExpectMatNear(m, kIdentity);
}

TEST(DallasVector, VectorAngleToMatrixBanksAroundForward) {
  vector f = V(2, -1, 2);
  f /= Len(f);

  matrix base, from_vec;
  vm_VectorAngleToMatrix(&base, &f, 0);
  vm_VectorToMatrix(&from_vec, &f);
  ExpectRotation(base);
  ExpectVecNear(base.fvec, f, 1e-5f);
  ExpectMatNear(base, from_vec, 1e-5f);

  for (angle a : {0x1000, 0x4000, 0x8000, 0xC000}) {
    matrix m;
    vm_VectorAngleToMatrix(&m, &f, a);
    ExpectRotation(m);
    ExpectVecNear(m.fvec, f, 1e-5f);
    const double rad = a / 65536.0 * 2 * kPi;
    EXPECT_NEAR(Dot(m.rvec, base.rvec), std::cos(rad), 1e-3) << std::hex << a;
  }
}

TEST(DallasVector, OrthogonalizeFixesSkewedMatrix) {
  matrix m{V(1.0f, 0.1f, 0.0f), V(0.2f, 0.9f, 0.1f), V(0.05f, -0.1f, 2.0f)};
  const vector f_in = m.fvec;
  const vector u_in = m.uvec;
  vm_Orthogonalize(&m);
  ExpectRotation(m);
  ExpectVecNear(m.fvec, f_in / Len(f_in));
  EXPECT_GT(Dot(m.uvec, u_in), 0.0f);
}

TEST(DallasVector, NormalOfTriangle) {
  // Right triangle with legs 3 and 4 in the plane z = 2: area 6.
  const vector a = V(0, 0, 2), b = V(3, 0, 2), c = V(0, 4, 2);
  vector n;
  const float mag = vm_GetNormal(&n, &a, &b, &c);
  EXPECT_NEAR(mag, 12.0f, kEps) << "returns |perp|, which is twice the triangle area";
  EXPECT_NEAR(Len(n), 1.0f, kEps);
  EXPECT_NEAR(std::fabs(n.z()), 1.0f, kEps);

  vector n_rev;
  vm_GetNormal(&n_rev, &c, &b, &a);
  ExpectVecNear(n_rev, -n);
}

TEST(DallasVector, NormalOfDegenerateTriangle) {
  const vector a = V(0, 0, 0), b = V(1, 1, 1), c = V(3, 3, 3);
  vector n;
  EXPECT_EQ(vm_GetNormal(&n, &a, &b, &c), 0.0f);
  EXPECT_TRUE(IsFinite(n));
}

TEST(DallasVector, CentroidOfTriangleIsVertexAverage) {
  const vector tri[3] = {V(0, 0, 1), V(6, 0, 1), V(0, 3, 1)};
  vector c;
  EXPECT_NEAR(vm_GetCentroid(&c, tri, 3), 9.0f, kEps);
  ExpectVecNear(c, V(2, 1, 1));
}

TEST(DallasVector, CentroidOfQuadIsAreaWeighted) {
  // Trapezoid (0,0) (4,0) (2,2) (0,2) at z = 5. By the shoelace formula: area 6,
  // centroid (14/9, 8/9) -- not the vertex average (1.5, 1).
  const vector quad[4] = {V(0, 0, 5), V(4, 0, 5), V(2, 2, 5), V(0, 2, 5)};
  vector c;
  EXPECT_NEAR(vm_GetCentroid(&c, quad, 4), 6.0f, 1e-4f);
  ExpectVecNear(c, V(14.0f / 9, 8.0f / 9, 5), 1e-4f);
}

TEST(DallasVector, CentroidOfRegularPolygonIsCenter) {
  constexpr int kSides = 7;
  const vector center = V(10, -3, 4);
  std::vector<vector> poly;
  for (int i = 0; i < kSides; i++) {
    const double t = 2 * kPi * i / kSides;
    poly.push_back(center + V(2 * std::cos(t), 0, 2 * std::sin(t)));
  }
  vector c;
  const float area = vm_GetCentroid(&c, poly.data(), kSides);
  // Area of a regular n-gon with circumradius R: n/2 * R^2 * sin(2*pi/n).
  EXPECT_NEAR(area, kSides / 2.0 * 4 * std::sin(2 * kPi / kSides), 1e-4);
  ExpectVecNear(c, center, 1e-4f);
}

TEST(DallasVector, BoundingSphereOfTwoPoints) {
  const vector pts[2] = {V(1, 2, 3), V(1, 2, 13)};
  vector center;
  EXPECT_NEAR(vm_ComputeBoundingSphere(&center, pts, 2), 5.0f, kEps);
  ExpectVecNear(center, V(1, 2, 8));
}

TEST(DallasVector, BoundingSphereOfPointsOnASphere) {
  // Including the axis extremes makes this sphere the minimal one, so it should be found exactly.
  const vector center = V(5, 5, 5);
  std::vector<vector> pts = {center + V(3, 0, 0), center + V(-3, 0, 0), center + V(0, 3, 0),
                             center + V(0, -3, 0), center + V(0, 0, 3), center + V(0, 0, -3)};
  std::mt19937 rng(42);
  std::normal_distribution<float> d;
  for (int i = 0; i < 50; i++) {
    vector v = V(d(rng), d(rng), d(rng));
    pts.push_back(center + v * (3.0f / Len(v)));
  }
  vector c;
  EXPECT_NEAR(vm_ComputeBoundingSphere(&c, pts.data(), static_cast<int>(pts.size())), 3.0f, 1e-3f);
  ExpectVecNear(c, center, 1e-3f);
}

TEST(DallasVector, BoundingSphereContainsAllPoints) {
  std::mt19937 rng(7);
  std::uniform_real_distribution<float> d(-50.0f, 50.0f);
  for (int trial = 0; trial < 20; trial++) {
    std::vector<vector> pts;
    const int n = 3 + trial * 5;
    for (int i = 0; i < n; i++)
      pts.push_back(V(d(rng), d(rng) * 0.3f, d(rng) + 20.0f));
    vector c;
    const float rad = vm_ComputeBoundingSphere(&c, pts.data(), n);
    for (const auto &p : pts)
      EXPECT_LE(Len(p - c), rad * (1 + 1e-5f) + 1e-4f) << "trial " << trial;
  }
}

TEST(DallasVector, DeltaAngleBetweenVectors) {
  const vector x = V(1, 0, 0), y = V(0, 3, 0), xy = V(2, 2, 0);
  // Inputs need not be normalized. Tolerance covers the trig-table interpolation in fix.cpp.
  EXPECT_LE(std::abs(AngleDiff(vm_DeltaAngVec(&x, &y, nullptr), kDeg90)), 0x10);
  EXPECT_LE(std::abs(AngleDiff(vm_DeltaAngVec(&x, &xy, nullptr), kDeg90 / 2)), 0x10);
  EXPECT_LE(std::abs(AngleDiff(vm_DeltaAngVec(&x, &x, nullptr), 0)), 0x10);
}

TEST(DallasVector, DeltaAngleOfOppositeVectorsIsHalfTurn) {
  const vector x = V(1, 0, 0), nx = V(-5, 0, 0);
  EXPECT_LE(std::abs(AngleDiff(vm_DeltaAngVec(&x, &nx, nullptr), 2 * kDeg90)), 0x10);
}

// The angle is positive when v0 x v1 points along fvec.
TEST(DallasVector, DeltaAngleIsSignedAroundForwardVector) {
  const vector x = V(1, 0, 0), y = V(0, 1, 0);
  const vector z = Cross(x, y);
  const vector neg_z = -z;
  EXPECT_LE(std::abs(AngleDiff(vm_DeltaAngVec(&x, &y, &z), kDeg90)), 0x10);
  EXPECT_LE(std::abs(AngleDiff(vm_DeltaAngVec(&y, &x, &z), -kDeg90)), 0x10);
  EXPECT_LE(std::abs(AngleDiff(vm_DeltaAngVec(&x, &y, &neg_z), -kDeg90)), 0x10);
}

} // namespace
