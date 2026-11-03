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

#include <gtest/gtest.h>

#include "fix.h"

// Angles in this library are 16-bit binary angles: 0x10000 units is one full
// revolution, so 0x4000 is 90 degrees, 0x8000 is 180 and 0xC000 is 270 (-90).

namespace {

constexpr double kTwoPi = 6.283185307179586476925;
constexpr double kUnitsPerRev = 65536.0;

double RadToUnits(double rad) { return rad / kTwoPi * kUnitsPerRev; }
double DegToUnits(double deg) { return deg / 360.0 * kUnitsPerRev; }
double UnitsToRad(double units) { return units / kUnitsPerRev * kTwoPi; }

double AngleError(angle actual, double expected_units) {
  double d = std::fmod(static_cast<double>(actual) - expected_units, kUnitsPerRev);
  if (d >= kUnitsPerRev / 2)
    d -= kUnitsPerRev;
  if (d < -kUnitsPerRev / 2)
    d += kUnitsPerRev;
  return d;
}

// The trig functions linearly interpolate a 256-entry-per-revolution table.
// The worst-case linear interpolation error of sin with step h = 2*pi/256 is
// h^2/8 ~= 7.5e-5, so 1e-4 is a bound a correct implementation always meets.
constexpr double kSinCosTol = 1e-4;

// 0.1 degree is comfortably above the table and fixed-point quantization
// error (a few units).
const double kAngleTol = DegToUnits(0.1);

class FixTrig : public ::testing::Test {
protected:
  static void SetUpTestSuite() { InitMathTables(); }
};

} // namespace

TEST_F(FixTrig, SinMatchesStdSinForEveryAngle) {
  for (int a = 0; a < 0x10000; a++) {
    double expected = std::sin(UnitsToRad(a));
    ASSERT_NEAR(FixSin(static_cast<angle>(a)), expected, kSinCosTol) << "angle 0x" << std::hex << a;
  }
}

TEST_F(FixTrig, CosMatchesStdCosForEveryAngle) {
  for (int a = 0; a < 0x10000; a++) {
    double expected = std::cos(UnitsToRad(a));
    ASSERT_NEAR(FixCos(static_cast<angle>(a)), expected, kSinCosTol) << "angle 0x" << std::hex << a;
  }
}

TEST_F(FixTrig, AsinMatchesStdAsinInsideDomain) {
  for (int k = -1980; k <= 1980; k++) {
    float v = static_cast<float>(k / 2000.0); // [-0.99, 0.99]
    double expected = RadToUnits(std::asin(static_cast<double>(v)));
    ASSERT_NEAR(AngleError(FixAsin(v), expected), 0.0, kAngleTol) << "v = " << v;
  }
}

TEST_F(FixTrig, AcosMatchesStdAcosInsideDomain) {
  for (int k = -1980; k <= 1980; k++) {
    float v = static_cast<float>(k / 2000.0); // [-0.99, 0.99]
    double expected = RadToUnits(std::acos(static_cast<double>(v)));
    ASSERT_NEAR(AngleError(FixAcos(v), expected), 0.0, kAngleTol) << "v = " << v;
  }
}

TEST_F(FixTrig, AsinClampsAtPositiveOne) {
  for (float v : {1.0f, 1.001f, 2.0f}) {
    EXPECT_NEAR(AngleError(FixAsin(v), 0x4000), 0.0, kAngleTol) << "v = " << v;
  }
}

TEST_F(FixTrig, AcosClampsAtPositiveOne) {
  for (float v : {1.0f, 1.001f, 2.0f}) {
    EXPECT_NEAR(AngleError(FixAcos(v), 0), 0.0, kAngleTol) << "v = " << v;
  }
}

// FixAtan2 takes (cos, sin) -- note the argument order, opposite to std::atan2.
TEST_F(FixTrig, Atan2AxesAndArgumentOrder) {
  EXPECT_NEAR(AngleError(FixAtan2(1.0f, 0.0f), 0x0000), 0.0, kAngleTol);
  EXPECT_NEAR(AngleError(FixAtan2(0.0f, 1.0f), 0x4000), 0.0, kAngleTol);
  EXPECT_NEAR(AngleError(FixAtan2(-1.0f, 0.0f), 0x8000), 0.0, kAngleTol);
  EXPECT_NEAR(AngleError(FixAtan2(0.0f, -1.0f), 0xC000), 0.0, kAngleTol);
}

// The exact diagonals are where the implementation switches between its asin
// and acos branches.
TEST_F(FixTrig, Atan2RecoversDirectionOfUnnormalizedVector) {
  for (double mag : {1e-3, 1.0, 1e4}) {
    for (int deg = 0; deg < 360; deg++) {
      double rad = deg * kTwoPi / 360.0;
      float c = static_cast<float>(mag * std::cos(rad));
      float s = static_cast<float>(mag * std::sin(rad));
      ASSERT_NEAR(AngleError(FixAtan2(c, s), DegToUnits(deg)), 0.0, kAngleTol)
          << "deg = " << deg << ", mag = " << mag;
    }
  }
}

TEST_F(FixTrig, Atan2OfZeroVectorIsZero) { EXPECT_EQ(FixAtan2(0.0f, 0.0f), 0); }

TEST_F(FixTrig, Atan2InvertsFixSinFixCos) {
  for (int a = 0; a < 0x10000; a++) {
    angle in = static_cast<angle>(a);
    ASSERT_NEAR(AngleError(FixAtan2(FixCos(in), FixSin(in)), a), 0.0, kAngleTol) << "angle 0x" << std::hex << a;
  }
}
