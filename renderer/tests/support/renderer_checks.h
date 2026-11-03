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

#pragma once

#include "3d.h"

// Clip codes of a polygon's vertices, combined the way g3_ClipPolygon expects.
g3Codes CodesOf(g3Point **pts, int n);

// Index of `target` in `list`, or -1 if it is not there.
int IndexOf(g3Point **list, int n, const g3Point *target);

// Expects the rotated position of `p` to be (x, y, z), within 1e-4.
void ExpectPos(const g3Point *p, float x, float y, float z);

// Expects `p` to carry UVs equal to (u, v), within 1e-4.
void ExpectUV(const g3Point *p, float u, float v);

// Expects `actual` to match `expected` in rotated position (within 1e-3) and clip codes.
void ExpectSameRotation(const g3Point &actual, const g3Point &expected);
