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

#include "room.h"

// Functions defined in editor/Erooms.cpp that no header declares.

// Returns the index of an earlier face with the same vertices and facing as facenum, or -1.
int CheckForDuplicateFace(room *rp, int facenum);
// Removes vertices no face uses; returns how many were removed.
int DeleteUnusedRoomVerts(room *rp);
// Welds coincident vertices; returns how many were removed.
int RemoveDuplicatePoints(room *rp);
// Drops repeated consecutive corners from faces; returns how many were removed.
int RemoveDuplicateFacePoints(room *rp);
