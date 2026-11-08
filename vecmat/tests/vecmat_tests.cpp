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

// Engine conventions used throughout:
//  - A matrix is three row vectors rvec/uvec/fvec (right, up, forward). For an
//    orientation they are orthonormal and right-handed in the sense that
//    rvec = uvec x fvec, i.e. det(rvec, uvec, fvec) = +1, as for the identity.
//  - vm_MatrixMulVector(v, m) gives v expressed in m's frame:
//    (v.rvec, v.uvec, v.fvec). vm_VectorMulTMatrix is the inverse mapping.
//  - Angles are 16-bit: 0x10000 is a full circle, 0x4000 is 90 degrees.
//    The trig tables are interpolated, so angle results get a few units of
//    slack and matrices built from angles get ~1e-3.

#include <cmath>
#include <cstdint>
#include <random>
#include <vector>

#include <gtest/gtest.h>

#include "fix.h"
#include "vecmat.h"

#include "support/vecmat_test_helpers.h"

using namespace vecmat_test;

namespace {

class MathTablesEnvironment : public ::testing::Environment {
public:
  void SetUp() override { InitMathTables(); }
};

const auto *const math_tables_env = ::testing::AddGlobalTestEnvironment(new MathTablesEnvironment);

constexpr float kTol = 1e-5f;     // pure floating-point arithmetic
constexpr float kTrigTol = 1e-3f; // anything that went through the trig tables
constexpr int kAngleTol = 8;      // in 1/65536ths of a circle (~0.04 degrees)

} // namespace

TEST(VecmatVector, CrossDotAndMagnitudeMatchHandComputedValues) {
  vector a{1, 2, 3}, b{4, 5, 6}, c;

  vm_CrossProduct(&c, &a, &b);
  EXPECT_TRUE(VecNear(c, vector{-3, 6, -3}, kTol));
  EXPECT_TRUE(VecNear(vm_Cross3Product(b, a), vector{3, -6, 3}, kTol));

  EXPECT_FLOAT_EQ(vm_DotProduct(&a, &b), 32.0f);

  vector d{2, -3, 6};
  EXPECT_FLOAT_EQ(vm_GetMagnitude(&d), 7.0f);

  vector p{1, 1, 1}, q{3, 4, 7};
  EXPECT_FLOAT_EQ(vm_VectorDistance(&p, &q), 7.0f);
}

TEST(VecmatVector, NormalizeReturnsOriginalMagnitudeAndYieldsUnitVector) {
  vector v{3, -4, 12};
  EXPECT_FLOAT_EQ(vm_NormalizeVector(&v), 13.0f);
  EXPECT_TRUE(VecNear(v, vector{3.0f / 13, -4.0f / 13, 12.0f / 13}, kTol));
}

TEST(VecmatVector, NormalizeZeroVectorReturnsZeroAndDoesNotProduceNaN) {
  // The engine historically sets the vector to a unit vector rather than dividing by zero.
  vector v{0, 0, 0};
  EXPECT_EQ(vm_NormalizeVector(&v), 0.0f);
  EXPECT_TRUE(std::isfinite(v.x()) && std::isfinite(v.y()) && std::isfinite(v.z()));
  EXPECT_NEAR(Len(v), 1.0f, kTol);
}

TEST(VecmatVector, GetNormalizedDirPointsFromStartToEndAndReturnsDistance) {
  // Note the argument order: (dest, end, start).
  vector start{1, 2, 3}, end{4, 6, 3}, dir;
  EXPECT_FLOAT_EQ(vm_GetNormalizedDir(&dir, &end, &start), 5.0f);
  EXPECT_TRUE(VecNear(dir, vector{0.6f, 0.8f, 0.0f}, kTol));

  EXPECT_EQ(vm_GetNormalizedDir(&dir, &start, &start), 0.0f);
}

// vm_GetMagnitudeFast is a cheap approximation (largest + 3/8 middle + 3/16 smallest).
// On the unit sphere that linear form ranges over [1.5625/sqrt(3), |(1,3/8,3/16)|] =
// [0.902, 1.085], so any vector's estimate must be within ~10% of the true length.
TEST(VecmatVector, FastMagnitudeIsSymmetricAndWithinApproximationBounds) {
  const vector samples[] = {{1, 0, 0}, {0, -5, 0}, {1, 1, 0}, {1, 1, 1}, {1, 0.375f, 0.1875f},
                            {3, -4, 12}, {-7, 2, 0.5f}, {0.001f, -0.002f, 0.003f}};
  for (const vector &v : samples) {
    SCOPED_TRACE(::testing::Message() << "v = (" << v.x() << ", " << v.y() << ", " << v.z() << ")");
    const float exact = Len(v);
    const float fast = vm_GetMagnitudeFast(&v);
    EXPECT_GE(fast, 0.90f * exact);
    EXPECT_LE(fast, 1.09f * exact);

    const vector perms[] = {{v.x(), v.z(), v.y()},  {v.y(), v.x(), v.z()},  {v.y(), v.z(), v.x()},
                            {v.z(), v.x(), v.y()},  {v.z(), v.y(), v.x()},  {-v.x(), v.y(), -v.z()}};
    for (const vector &p : perms)
      EXPECT_FLOAT_EQ(vm_GetMagnitudeFast(&p), fast);
  }

  // Axis-aligned vectors have no error at all.
  vector axis{0, -5, 0};
  EXPECT_FLOAT_EQ(vm_GetMagnitudeFast(&axis), 5.0f);
}

TEST(VecmatVector, NormalizeFastKeepsDirectionAndReturnsFastMagnitude) {
  vector v{-7, 2, 0.5f};
  const float expected_mag = vm_GetMagnitudeFast(&v);
  const vector exact_dir = Unit(v);

  EXPECT_FLOAT_EQ(vm_NormalizeVectorFast(&v), expected_mag);
  EXPECT_TRUE(VecNear(Unit(v), exact_dir, kTol));
  EXPECT_NEAR(Len(v), 1.0f, 0.11f);

  vector zero{0, 0, 0};
  EXPECT_EQ(vm_NormalizeVectorFast(&zero), 0.0f);
  EXPECT_TRUE(VecNear(zero, vector{0, 0, 0}, 0.0f));
}

TEST(VecmatNormal, ClockwiseVerticesGiveNormalTowardViewer) {
  // A viewer at negative z looking down +z (x right, y up) sees a -> b -> c as clockwise:
  // up the left edge, then across to the right. Descent faces are clockwise from the
  // front, so the normal must point back at the viewer (-z).
  vector a{0, 0, 5}, b{0, 3, 5}, c{4, 3, 5}, n;

  // The returned value is the pre-normalization length: twice the triangle's area (legs 3 and 4).
  EXPECT_FLOAT_EQ(vm_GetNormal(&n, &a, &b, &c), 12.0f);
  EXPECT_TRUE(VecNear(n, vector{0, 0, -1}, kTol));

  vm_GetNormal(&n, &c, &b, &a);
  EXPECT_TRUE(VecNear(n, vector{0, 0, 1}, kTol));
}

TEST(VecmatNormal, DegenerateTriangleReportsZeroQuality) {
  vector a{1, 1, 1}, b{2, 2, 2}, c{4, 4, 4}, n;
  EXPECT_EQ(vm_GetNormal(&n, &a, &b, &c), 0.0f);
}

TEST(VecmatMatrix, IdentityAndInverseMatricesActOnVectors) {
  const vector v{1.5f, -2, 3};
  vector out;
  matrix m;

  vm_MakeIdentity(&m);
  vm_MatrixMulVector(&out, &v, &m);
  EXPECT_TRUE(VecNear(out, v, 0.0f));

  // vm_MakeInverseMatrix produces the negated identity: every axis is flipped.
  vm_MakeInverseMatrix(&m);
  vm_MatrixMulVector(&out, &v, &m);
  EXPECT_TRUE(VecNear(out, -v, 0.0f));
}

TEST(VecmatMatrix, TransposeSwapsRowsAndColumns) {
  const matrix m = GeneralMatrix();
  matrix t = m;
  vm_TransposeMatrix(&t);
  const matrix t2 = ~m;

  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      EXPECT_EQ(t.a2d[i][j], m.a2d[j][i]) << "i=" << i << " j=" << j;
      EXPECT_EQ(t2.a2d[i][j], m.a2d[j][i]) << "i=" << i << " j=" << j;
    }
  }
}

TEST(VecmatMatrix, MatrixMulVectorExpressesVectorInMatrixFrame) {
  const matrix m = GeneralMatrix();
  const vector v{1, -1, 2};
  vector out;
  vm_MatrixMulVector(&out, &v, &m);
  EXPECT_TRUE(VecNear(out, vector{1 - 2 + 6, 0 - 1 + 8, 5 - 6 + 0}, kTol));
  EXPECT_TRUE(VecNear(v * m, out, kTol));

  matrix o;
  vm_AnglesToMatrix(&o, 0x1234, 0x5678, 0x9abc);
  vm_MatrixMulVector(&out, &o.fvec, &o);
  EXPECT_TRUE(VecNear(out, vector{0, 0, 1}, kTrigTol));
  vm_MatrixMulVector(&out, &o.rvec, &o);
  EXPECT_TRUE(VecNear(out, vector{1, 0, 0}, kTrigTol));
}

TEST(VecmatMatrix, VectorMulTMatrixMultipliesByTranspose) {
  const matrix m = GeneralMatrix();
  const vector v{1, -1, 2};
  vector out;
  vm_VectorMulTMatrix(&out, &v, &m);
  // Sum of the rows weighted by v: 1*(1,2,3) - 1*(0,1,4) + 2*(5,6,0).
  EXPECT_TRUE(VecNear(out, vector{11, 13, -1}, kTol));

  matrix o;
  vm_AnglesToMatrix(&o, 0x1234, 0x5678, 0x9abc);
  vector local, world;
  vm_MatrixMulVector(&local, &v, &o);
  vm_VectorMulTMatrix(&world, &local, &o);
  EXPECT_TRUE(VecNear(world, v, kTrigTol * 4));
}

TEST(VecmatMatrix, MatrixProductAppliesFirstOperandThenSecond) {
  const matrix a = GeneralMatrix();
  const matrix b = OtherMatrix();
  const vector v{0.5f, -3, 2};

  matrix ab;
  vm_MatrixMul(&ab, &a, &b);
  EXPECT_TRUE(MatNear(a * b, ab, 0.0f));

  vector step1, step2, direct;
  vm_MatrixMulVector(&step1, &v, &a);
  vm_MatrixMulVector(&step2, &step1, &b);
  vm_MatrixMulVector(&direct, &v, &ab);
  EXPECT_TRUE(VecNear(direct, step2, kTol * 10));

  matrix acc = a;
  acc *= b;
  EXPECT_TRUE(MatNear(acc, ab, 0.0f));
}

TEST(VecmatMatrix, MatrixMulTMatrixMultipliesByTransposeOfSecondArgument) {
  const matrix a = GeneralMatrix();
  const matrix b = OtherMatrix();
  const matrix bt = ~b;

  matrix expected, actual;
  vm_MatrixMul(&expected, &a, &bt);
  vm_MatrixMulTMatrix(&actual, &a, &b);
  EXPECT_TRUE(MatNear(actual, expected, kTol));
}

TEST(VecmatMatrix, OrientationTimesItsTransposeIsIdentity) {
  matrix o;
  vm_AnglesToMatrix(&o, 0xe000, 0x3456, 0x2222);
  matrix ot = ~o, prod;
  vm_MatrixMul(&prod, &o, &ot);
  EXPECT_TRUE(MatNear(prod, Identity_matrix, kTrigTol));
}

TEST(VecmatMatrix, DeterminantMatchesHandComputedValues) {
  matrix diag{vector{2, 0, 0}, vector{0, 3, 0}, vector{0, 0, 4}};
  EXPECT_FLOAT_EQ(calc_det_value(&diag), 24.0f);

  matrix g = GeneralMatrix();
  EXPECT_FLOAT_EQ(calc_det_value(&g), 1.0f);

  matrix o = OtherMatrix();
  EXPECT_FLOAT_EQ(calc_det_value(&o), 3.0f);

  // Swapping two rows negates the determinant.
  matrix swapped{g.uvec, g.rvec, g.fvec};
  EXPECT_FLOAT_EQ(calc_det_value(&swapped), -1.0f);

  // det(A*B) = det(A) * det(B)
  matrix go = g * o;
  EXPECT_NEAR(calc_det_value(&go), 3.0f, kTol * 100);

  EXPECT_FLOAT_EQ(calc_det_value(&Identity_matrix), 1.0f);
}

namespace {
struct Pbh {
  angle p, h, b;
};

// Pitches strictly between -90 and +90 degrees, where the (p, h, b) representation of an
// orientation is unique.
const Pbh kRegularAngles[] = {
    {0x0000, 0x0000, 0x0000}, {0x1000, 0x0000, 0x0000}, {0x0000, 0x2000, 0x0000}, {0x0000, 0x0000, 0x3000},
    {0x1234, 0x5678, 0x9abc}, {0xd000, 0x7fff, 0x0100}, {0x3000, 0xe123, 0xc000}, {0xf800, 0x9000, 0x4000},
    {0x2a00, 0xbeef, 0x7000}, {0xc800, 0x0400, 0xffff},
};
} // namespace

TEST(VecmatAngles, ZeroAnglesGiveIdentity) {
  matrix m;
  vm_AnglesToMatrix(&m, 0, 0, 0);
  EXPECT_TRUE(MatNear(m, Identity_matrix, kTrigTol));
}

TEST(VecmatAngles, AnglesToMatrixProducesRightHandedOrthonormalMatrix) {
  for (const Pbh &a : kRegularAngles) {
    SCOPED_TRACE(::testing::Message() << std::hex << "p=" << a.p << " h=" << a.h << " b=" << a.b);
    matrix m;
    vm_AnglesToMatrix(&m, a.p, a.h, a.b);
    EXPECT_TRUE(IsOrientation(m, kTrigTol));
  }
}

TEST(VecmatAngles, ExtractAnglesInvertsAnglesToMatrix) {
  for (const Pbh &a : kRegularAngles) {
    SCOPED_TRACE(::testing::Message() << std::hex << "p=" << a.p << " h=" << a.h << " b=" << a.b);
    matrix m;
    vm_AnglesToMatrix(&m, a.p, a.h, a.b);
    angvec out;
    vm_ExtractAnglesFromMatrix(&out, &m);
    EXPECT_LE(std::abs(AngleDiff(out.p(), a.p)), kAngleTol);
    EXPECT_LE(std::abs(AngleDiff(out.h(), a.h)), kAngleTol);
    EXPECT_LE(std::abs(AngleDiff(out.b(), a.b)), kAngleTol);
  }
}

TEST(VecmatAngles, ExtractAnglesRoundTripsThroughMatrixForAnyPitch) {
  // Past +-90 degrees of pitch (and exactly at it) the angles are not unique, so compare
  // the orientation they describe rather than the angle values.
  const Pbh cases[] = {
      {0x4000, 0x1234, 0x0000}, {0x4000, 0x1234, 0x2345}, {0xc000, 0x9876, 0x5555}, {0xc000, 0x0000, 0xe000},
      {0x6000, 0x2000, 0x1000}, {0x9000, 0xd000, 0x8000}, {0x8000, 0x4321, 0x0042},
  };
  for (const Pbh &a : cases) {
    SCOPED_TRACE(::testing::Message() << std::hex << "p=" << a.p << " h=" << a.h << " b=" << a.b);
    matrix m, back;
    vm_AnglesToMatrix(&m, a.p, a.h, a.b);
    angvec out;
    vm_ExtractAnglesFromMatrix(&out, &m);
    vm_AnglesToMatrix(&back, out.p(), out.h(), out.b());
    EXPECT_TRUE(MatNear(back, m, kTrigTol));
  }
}

TEST(VecmatAngles, VectorAngleToMatrixMatchesAnglesToMatrix) {
  for (const Pbh &a : kRegularAngles) {
    SCOPED_TRACE(::testing::Message() << std::hex << "p=" << a.p << " h=" << a.h << " b=" << a.b);
    matrix expected, actual;
    vm_AnglesToMatrix(&expected, a.p, a.h, a.b);
    vector fvec = expected.fvec;
    vm_VectorAngleToMatrix(&actual, &fvec, a.b);
    EXPECT_TRUE(MatNear(actual, expected, kTrigTol));
  }
}

TEST(VecmatAngles, VectorAngleToMatrixHandlesStraightUpAndDown) {
  for (angle bank : {angle(0x0000), angle(0x2000), angle(0xa000)}) {
    SCOPED_TRACE(::testing::Message() << std::hex << "bank=" << bank);
    matrix actual, expected;

    vector down{0, -1, 0}; // pitch +90 (positive pitch is nose-down)
    vm_VectorAngleToMatrix(&actual, &down, bank);
    EXPECT_TRUE(IsOrientation(actual, kTrigTol));
    EXPECT_TRUE(VecNear(actual.fvec, down, kTrigTol));
    vm_AnglesToMatrix(&expected, 0x4000, 0, bank);
    EXPECT_TRUE(MatNear(actual, expected, kTrigTol));

    vector up{0, 1, 0}; // pitch -90
    vm_VectorAngleToMatrix(&actual, &up, bank);
    EXPECT_TRUE(IsOrientation(actual, kTrigTol));
    EXPECT_TRUE(VecNear(actual.fvec, up, kTrigTol));
    vm_AnglesToMatrix(&expected, 0xc000, 0, bank);
    EXPECT_TRUE(MatNear(actual, expected, kTrigTol));
  }
}

TEST(VecmatAngles, DeltaAngVecMeasuresAngleBetweenVectors) {
  // Inputs need not be normalized.
  vector x{2, 0, 0}, y{0, 5, 0}, diag{3, 3, 0};
  EXPECT_LE(std::abs(AngleDiff(vm_DeltaAngVec(&x, &y, nullptr), 0x4000)), kAngleTol);
  EXPECT_LE(std::abs(AngleDiff(vm_DeltaAngVec(&x, &diag, nullptr), 0x2000)), kAngleTol);
  EXPECT_EQ(vm_DeltaAngVec(&x, &x, nullptr), 0);

  vector zf{0, 0, 1}, zb{0, 0, -1};
  const angle around_f = vm_DeltaAngVec(&x, &y, &zf);
  const angle around_b = vm_DeltaAngVec(&x, &y, &zb);
  EXPECT_LE(std::abs(AngleDiff(around_f, static_cast<angle>(-around_b))), 1);
  EXPECT_LE(std::abs(std::abs(AngleDiff(around_f, 0)) - 0x4000), kAngleTol);
  EXPECT_LE(std::abs(AngleDiff(vm_DeltaAngVec(&y, &x, &zf), around_b)), 1);
}

namespace {
const vector kDirections[] = {
    {0, 0, 1}, {0, 0, -1}, {1, 0, 0}, {-3, 0, 0}, {0, 1, 0}, {0, -7, 0}, {1, 2, 3}, {-0.2f, 0.9f, -0.1f}, {5, -1, -4},
};
} // namespace

TEST(VecmatVectorToMatrix, ForwardOnlyGivesOrientationWithThatForward) {
  for (const vector &f : kDirections) {
    SCOPED_TRACE(::testing::Message() << "f = (" << f.x() << ", " << f.y() << ", " << f.z() << ")");
    vector fv = f;
    matrix m;
    vm_VectorToMatrix(&m, &fv);
    EXPECT_TRUE(IsOrientation(m, kTol * 10));
    EXPECT_TRUE(VecNear(m.fvec, Unit(f), kTol));
  }
}

TEST(VecmatVectorToMatrix, ForwardAndUpGivesIdentityForCanonicalAxes) {
  vector f{0, 0, 2}, u{0, 3, 0};
  matrix m;
  vm_VectorToMatrix(&m, &f, &u);
  EXPECT_TRUE(MatNear(m, Identity_matrix, kTol));
}

TEST(VecmatVectorToMatrix, ForwardAndUpKeepsForwardAndTiltsUpTowardHint) {
  const vector f{1, 2, 3};
  const vector up_hints[] = {{0, 1, 0}, {0, 0, 1}, {-1, 1, 0.5f}, {1, 2, 2.9f}};
  for (const vector &u : up_hints) {
    SCOPED_TRACE(::testing::Message() << "u = (" << u.x() << ", " << u.y() << ", " << u.z() << ")");
    vector fv = f, uv = u;
    matrix m;
    vm_VectorToMatrix(&m, &fv, &uv);
    EXPECT_TRUE(IsOrientation(m, kTol * 10));
    EXPECT_TRUE(VecNear(m.fvec, Unit(f), kTol));
    // The up hint lies in the forward/up plane, on the up side.
    EXPECT_NEAR(Dot(m.rvec, u), 0.0f, kTol * 10);
    EXPECT_GT(Dot(m.uvec, u), 0.0f);
  }
}

TEST(VecmatVectorToMatrix, ForwardAndRightKeepsForwardAndTiltsRightTowardHint) {
  const vector f{-2, 1, 0.5f};
  const vector right_hints[] = {{1, 0, 0}, {0, 0, 1}, {0.3f, 1, -2}};
  for (const vector &r : right_hints) {
    SCOPED_TRACE(::testing::Message() << "r = (" << r.x() << ", " << r.y() << ", " << r.z() << ")");
    vector fv = f, rv = r;
    matrix m;
    vm_VectorToMatrix(&m, &fv, nullptr, &rv);
    EXPECT_TRUE(IsOrientation(m, kTol * 10));
    EXPECT_TRUE(VecNear(m.fvec, Unit(f), kTol));
    EXPECT_NEAR(Dot(m.uvec, r), 0.0f, kTol * 10);
    EXPECT_GT(Dot(m.rvec, r), 0.0f);
  }
}

TEST(VecmatVectorToMatrix, UpOnlyGivesOrientationWithThatUp) {
  for (const vector &u : kDirections) {
    SCOPED_TRACE(::testing::Message() << "u = (" << u.x() << ", " << u.y() << ", " << u.z() << ")");
    vector uv = u;
    matrix m;
    vm_VectorToMatrix(&m, nullptr, &uv);
    EXPECT_TRUE(IsOrientation(m, kTol * 10));
    EXPECT_TRUE(VecNear(m.uvec, Unit(u), kTol));
  }
}

TEST(VecmatVectorToMatrix, RightOnlyGivesOrientationWithThatRight) {
  for (const vector &r : kDirections) {
    SCOPED_TRACE(::testing::Message() << "r = (" << r.x() << ", " << r.y() << ", " << r.z() << ")");
    vector rv = r;
    matrix m;
    vm_VectorToMatrix(&m, nullptr, nullptr, &rv);
    EXPECT_TRUE(IsOrientation(m, kTol * 10));
    EXPECT_TRUE(VecNear(m.rvec, Unit(r), kTol));
  }
}

TEST(VecmatVectorToMatrix, UpAndRightKeepsUpAndTiltsRightTowardHint) {
  vector u{0.5f, 2, -1}, r{1, 0, 0.2f};
  matrix m;
  vm_VectorToMatrix(&m, nullptr, &u, &r);
  EXPECT_TRUE(IsOrientation(m, kTol * 10));
  EXPECT_TRUE(VecNear(m.uvec, Unit(vector{0.5f, 2, -1}), kTol));
  EXPECT_NEAR(Dot(m.fvec, vector{1, 0, 0.2f}), 0.0f, kTol * 10);
  EXPECT_GT(Dot(m.rvec, vector{1, 0, 0.2f}), 0.0f);
}

TEST(VecmatOrthogonalize, RepairsSkewedMatrixKeepingForwardAndUpSide) {
  matrix m{vector{5, 5, 5}, vector{0.1f, 2, 0.7f}, vector{0, 0.2f, 3}};
  const vector old_f = m.fvec, old_u = m.uvec;
  vm_Orthogonalize(&m);
  EXPECT_TRUE(IsOrientation(m, kTol * 10));
  EXPECT_TRUE(VecNear(m.fvec, Unit(old_f), kTol));
  // New up stays in the plane of forward and the old up, on the same side.
  EXPECT_NEAR(Dot(m.rvec, old_u), 0.0f, kTol * 10);
  EXPECT_GT(Dot(m.uvec, old_u), 0.0f);
}

TEST(VecmatOrthogonalize, LeavesValidOrientationUnchanged) {
  matrix m;
  vm_AnglesToMatrix(&m, 0x1234, 0x5678, 0x9abc);
  // Make it exactly orthonormal first; the trig tables leave ~1e-4 of slop.
  vm_Orthogonalize(&m);
  const matrix before = m;
  vm_Orthogonalize(&m);
  EXPECT_TRUE(MatNear(m, before, kTol));
}

TEST(VecmatOrthogonalize, UpParallelToForwardStillGivesValidOrientation) {
  matrix m{vector{0, 0, 0}, vector{0, 0, 2}, vector{0, 0, 1}};
  vm_Orthogonalize(&m);
  EXPECT_TRUE(IsOrientation(m, kTol * 10));
  EXPECT_TRUE(VecNear(m.fvec, vector{0, 0, 1}, kTol));
}

namespace {
// An isosceles trapezoid in the z=0 plane: bases 4 (y=0) and 2 (y=2), height 2.
// Area = (4 + 2) / 2 * 2 = 6. Centroid: x = 2 by symmetry, y = h/3 * (b1 + 2*b2) / (b1 + b2)
// = 2/3 * 8/6 = 8/9.
const vector kTrapezoid[] = {{0, 0, 0}, {4, 0, 0}, {3, 2, 0}, {1, 2, 0}};
constexpr float kTrapezoidArea = 6.0f;
const vector kTrapezoidCentroid{2.0f, 8.0f / 9.0f, 0.0f};
} // namespace

TEST(VecmatCentroid, TriangleCentroidIsVertexAverage) {
  const vector tri[] = {{1, 1, 1}, {7, 1, 1}, {1, 5, 1}};
  vector c;
  EXPECT_FLOAT_EQ(vm_GetCentroid(&c, tri, 3), 12.0f); // legs 6 and 4
  EXPECT_TRUE(VecNear(c, vector{3, 7.0f / 3, 1}, kTol));
}

TEST(VecmatCentroid, TrapezoidCentroidAndArea) {
  vector c;
  EXPECT_NEAR(vm_GetCentroid(&c, kTrapezoid, 4), kTrapezoidArea, kTol);
  EXPECT_TRUE(VecNear(c, kTrapezoidCentroid, kTol));
}

TEST(VecmatCentroid, CentroidFollowsRigidMotionOfPolygon) {
  vector moved[4];
  for (int i = 0; i < 4; i++)
    moved[i] = Tilt(kTrapezoid[i]);

  vector c;
  EXPECT_NEAR(vm_GetCentroid(&c, moved, 4), kTrapezoidArea, kTol * 10);
  EXPECT_TRUE(VecNear(c, Tilt(kTrapezoidCentroid), kTol * 10));
}

TEST(VecmatCentroid, FastCentroidOfPlanarPolygonIsExactAndAreaApproximate) {
  // All fan triangles of a planar polygon share a normal direction, so the approximate
  // magnitude errs by the same factor on each and the area-weighted centroid is unaffected.
  // The area carries the fast-magnitude error (within ~10%).
  vector moved[4];
  for (int i = 0; i < 4; i++)
    moved[i] = Tilt(kTrapezoid[i]);

  vector c;
  const float area = vm_GetCentroidFast(&c, moved, 4);
  EXPECT_TRUE(VecNear(c, Tilt(kTrapezoidCentroid), kTol * 10));
  EXPECT_GE(area, 0.90f * kTrapezoidArea);
  EXPECT_LE(area, 1.09f * kTrapezoidArea);
}

TEST(VecmatBoundingSphere, SinglePointHasZeroRadius) {
  const vector pts[] = {{3, -2, 8}};
  vector center;
  EXPECT_EQ(vm_ComputeBoundingSphere(&center, pts, 1), 0.0f);
  EXPECT_TRUE(VecNear(center, pts[0], 0.0f));
}

TEST(VecmatBoundingSphere, TwoPointsGiveSphereOnTheirMidpoint) {
  const vector pts[] = {{1, 2, 3}, {3, 6, 7}};
  vector center;
  EXPECT_NEAR(vm_ComputeBoundingSphere(&center, pts, 2), 3.0f, kTol); // distance is 6
  EXPECT_TRUE(VecNear(center, vector{2, 4, 5}, kTol));
}

TEST(VecmatBoundingSphere, SymmetricPointsGiveExactSphere) {
  const vector c{10, -5, 2};
  const float r = 4;
  const vector pts[] = {c + vector{r, 0, 0}, c + vector{0, -r, 0}, c + vector{0, 0, r},
                        c + vector{-r, 0, 0}, c + vector{0, r, 0},  c + vector{0, 0, -r}};
  vector center;
  EXPECT_NEAR(vm_ComputeBoundingSphere(&center, pts, 6), r, kTol * 10);
  EXPECT_TRUE(VecNear(center, c, kTol * 10));
}

TEST(VecmatBoundingSphere, SphereContainsEveryPoint) {
  // The algorithm is approximate (not the minimal sphere).
  std::mt19937 rng(12345);
  std::uniform_real_distribution<float> coord(-100.0f, 100.0f);
  for (int trial = 0; trial < 20; trial++) {
    SCOPED_TRACE(::testing::Message() << "trial " << trial);
    std::vector<vector> pts(3 + trial * 7);
    for (vector &p : pts) {
      p = vector{coord(rng), coord(rng), coord(rng)};
      if (trial % 2) // also try flattened, elongated clouds
        p.y() *= 0.05f;
    }

    vector center;
    const float rad = vm_ComputeBoundingSphere(&center, pts.data(), static_cast<int>(pts.size()));
    for (const vector &p : pts)
      EXPECT_LE(Len(p - center), rad * (1 + 1e-5f) + 1e-4f);
  }
}
