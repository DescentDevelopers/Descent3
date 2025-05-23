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

 * $Logfile: /DescentIII/Main/fix/fix.cpp $
 * $Revision: 6 $
 * $Date: 4/22/99 8:29p $
 * $Author: Kevin $
 *
 *  Fixed-point math functions, including trig & float conversions
 *
 * $Log: /DescentIII/Main/fix/fix.cpp $
 *
 * 6     4/22/99 8:29p Kevin
 * made psrand.h come after stdlib.h
 *
 * 5     4/21/99 11:05a Kevin
 * new ps_rand and ps_srand to replace rand & srand
 *
 * 4     8/26/97 10:15a Jason
 * corrected the FixCeil function
 *
 * 3     8/22/97 12:34p Jason
 * added FixCeil and FixFloor
 *
 * 2     8/21/97 5:57p Samir
 * Added initialization of random number generator.
 *
 * 7     2/27/97 4:47 PM Jeremy
 * moved inline assembly language functions processor for
 * fixmul/fixdiv/fixmuldiv into fixmac.h and fixwin32.h
 *
 * 6     2/12/97 5:35p Jason
 * fixed yet another dumb syntax error...will I ever learn?
 *
 * 5     2/12/97 5:34p Jason
 * fixed dumb syntax error
 *
 * 4     2/12/97 5:27p Jason
 * implemented asin,acos,atan2
 *
 * 3     2/10/97 1:57p Jason
 * changed FloatToFix back to what it originally was
 *
 * 2     2/07/97 5:44p Matt
 * Moved fixed-point math funcs from vecmat to fix
 * Changed trig functions to use small table like D2
 *
 * $NoKeywords: $
 */

#include <cmath>

#include "fix.h"

// Tables for trig functions
scalar sincos_table[321]; // 256 entries + 64 sin-only + 1 for interpolation
angle asin_table[257];   // 1 quadrants worth, +1 for interpolation
angle acos_table[257];

// Generate the data for the trig tables
void InitMathTables() {
  int i;
  scalar rad, s, c;

  for (i = 0; i < 321; i++) {
    rad = (scalar)((double)i / 256.0 * 2 * PI);
    sincos_table[i] = std::sin(rad);
  }

  for (i = 0; i < 256; i++) {

    s = std::asin((scalar)i / 256.0f);
    c = std::acos((scalar)i / 256.0f);

    s = (s / (PI * 2));
    c = (c / (PI * 2));

    asin_table[i] = FloatToFix(s);
    acos_table[i] = FloatToFix(c);
  }

  asin_table[256] = asin_table[255];
  acos_table[256] = acos_table[255];
}

// Returns the sine of the given angle.  Linearly interpolates between two entries in a 256-entry table
scalar FixSin(angle a) {
  int i, f;
  scalar s0, s1;

  i = (a >> 8) & 0xff;
  f = a & 0xff;

  s0 = sincos_table[i];
  s1 = sincos_table[i + 1];
  return (scalar)(s0 + ((s1 - s0) * (double)f / 256.0));
}

// Returns the cosine of the given angle.  Linearly interpolates between two entries in a 256-entry table
scalar FixCos(angle a) {
  int i, f;
  scalar c0, c1;

  i = (a >> 8) & 0xff;
  f = a & 0xff;

  c0 = sincos_table[i + 64];
  c1 = sincos_table[i + 64 + 1];
  return (scalar)(c0 + ((c1 - c0) * (double)f / 256.0));
}

// compute inverse sine
angle FixAsin(scalar v) {
  fix vv;
  int i, f, aa;

  vv = FloatToFix(std::fabs(v));

  if (vv >= F1_0) // check for out of range
    return 0x4000;

  i = (vv >> 8) & 0xff;
  f = vv & 0xff;

  aa = asin_table[i];
  aa = aa + (((asin_table[i + 1] - aa) * f) >> 8);

  if (v < 0)
    aa = F1_0 - aa;

  return aa;
}

// compute inverse cosine
angle FixAcos(scalar v) {
  fix vv;
  int i, f, aa;

  vv = FloatToFix(std::fabs(v));

  if (vv >= F1_0) // check for out of range
    return 0;

  i = (vv >> 8) & 0xff;
  f = vv & 0xff;

  aa = acos_table[i];
  aa = aa + (((acos_table[i + 1] - aa) * f) >> 8);

  if (v < 0)
    aa = 0x8000 - aa;

  return aa;
}

// given cos & sin of an angle, return that angle.
// parms need not be normalized, that is, the ratio of the parms cos/sin must
// equal the ratio of the actual cos & sin for the result angle, but the parms
// need not be the actual cos & sin.
// NOTE: this is different from the standard C atan2, since it is left-handed.
angle FixAtan2(scalar cos, scalar sin) {
  scalar q, m;
  angle t;

  // find smaller of two

  q = (sin * sin) + (cos * cos);

  m = std::sqrt(q);

  if (m == 0)
    return 0;

  if (std::fabs(sin) < std::fabs(cos)) {
    // sin is smaller, use arcsin
    t = FixAsin(sin / m);
    if (cos < 0)
      t = 0x8000 - t;

    return t;
  } else {
    t = FixAcos(cos / m);
    if (sin < 0)
      t = F1_0 - t;

    return t;
  }
}
