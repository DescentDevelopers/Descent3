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

#ifndef BITMAP_TESTS_IMAGE_LOADING_H
#define BITMAP_TESTS_IMAGE_LOADING_H

#include <cstdint>
#include <set>
#include <vector>

#include "cfile.h"
#include "d3_test/files.h"

// Loads a bitmap from an open file and returns its handle or -1, as the bm_*_alloc_file functions do.
using ImageLoader = int (*)(CFILE *);

// Writes `bytes` to dir/image.bin and loads it with `load`; returns the handle (owned by the caller) or -1.
int LoadFromBytes(const d3_test::TempDir &dir, const std::vector<uint8_t> &bytes, ImageLoader load);

// The GameBitmaps slots currently in use.
std::set<int> UsedSlots();

// bm_tga_alloc_file into BITMAP_FORMAT_1555.
int LoadTga1555(CFILE *in);

// bm_tga_alloc_file into BITMAP_FORMAT_4444.
int LoadTga4444(CFILE *in);

#endif
