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

#include "image_loading.h"

#include <gtest/gtest.h>

#include "bitmap.h"
#include "iff.h"

int LoadFromBytes(const d3_test::TempDir &dir, const std::vector<uint8_t> &bytes, ImageLoader load) {
  std::filesystem::path path = dir.Write("image.bin", bytes);
  CFILE *in = cfopen(path, "rb");
  EXPECT_NE(in, nullptr);
  if (!in)
    return -1;
  int handle = load(in);
  cfclose(in);
  return handle;
}

std::set<int> UsedSlots() {
  std::set<int> used;
  for (int i = 0; i < MAX_BITMAPS; i++)
    if (GameBitmaps[i].used)
      used.insert(i);
  return used;
}

int LoadTga1555(CFILE *in) {
  char name[BITMAP_NAME_LEN] = {};
  return bm_tga_alloc_file(in, name, BITMAP_FORMAT_1555);
}

int LoadTga4444(CFILE *in) {
  char name[BITMAP_NAME_LEN] = {};
  return bm_tga_alloc_file(in, name, BITMAP_FORMAT_4444);
}
