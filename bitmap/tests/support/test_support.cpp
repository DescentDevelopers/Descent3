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

#include "test_support.h"

#include <algorithm>
#include <cstring>

#include <SDL3/SDL_assert.h>

#include "cfile.h"
#include "d3_test/game.h"
#include "grdefs.h"

void BitmapLibTest::SetUp() { d3_test::EnsureBitmaps(); }

void BitmapLibTest::TearDown() {
  for (int handle : tracked_) {
    if (handle > 0 && handle < MAX_BITMAPS) {
      while (GameBitmaps[handle].used > 0)
        bm_FreeBitmap(handle);
    }
  }
  tracked_.clear();
  cf_ClearAllSearchPaths();
}

int BitmapLibTest::Track(int handle) {
  if (handle > 0)
    tracked_.insert(handle);
  return handle;
}

int BitmapLibTest::MakeBitmap(int w, int h, const std::vector<uint16_t> &pixels, int format, bool room_for_mips) {
  return Track(AllocBitmap(w, h, pixels, format, room_for_mips));
}

std::vector<uint16_t> BitmapLibTest::Pixels(int handle, int mip) {
  const uint16_t *data = bm_data(handle, mip);
  const int count = bm_w(handle, mip) * bm_h(handle, mip);
  return std::vector<uint16_t>(data, data + count);
}

bool IsTransparent1555(uint16_t pixel) { return (pixel & OPAQUE_FLAG) == 0; }

uint16_t ChunkPattern(int x, int y) { return static_cast<uint16_t>(OPAQUE_FLAG | ((x * 7 + y * 131) & 0x7FFF)); }

int AllocBitmap(int w, int h, const std::vector<uint16_t> &pixels, int format, bool room_for_mips) {
  EXPECT_EQ(pixels.size(), static_cast<size_t>(w * h));
  int handle = bm_AllocBitmap(w, h, room_for_mips ? (w * h * 2) / 3 : 0);
  EXPECT_GT(handle, 0);
  if (handle <= 0)
    return handle;
  GameBitmaps[handle].format = format;
  uint16_t *data = bm_data(handle, 0);
  std::copy(pixels.begin(), pixels.end(), data);
  return handle;
}

int SaveAndReload(const d3_test::TempDir &dir, int handle, int mipped) {
  std::filesystem::path path = dir / "roundtrip.ogf";
  EXPECT_EQ(bm_SaveFileBitmap(path, handle), 1);
  CFILE *in = cfopen(path, "rb");
  EXPECT_NE(in, nullptr);
  if (!in)
    return -1;
  int loaded = bm_AllocLoadBitmap(in, mipped);
  cfclose(in);
  return loaded;
}

std::filesystem::path WriteOgf(const d3_test::TempDir &dir, const std::string &filename, uint16_t fill) {
  return WriteOgfStoringName(dir, filename, filename, fill);
}

std::filesystem::path WriteOgfStoringName(const d3_test::TempDir &dir, const std::string &filename,
                                          const std::string &stored_name, uint16_t fill) {
  int src = AllocBitmap(2, 2, {fill, fill, fill, fill});
  strcpy(GameBitmaps[src].name, stored_name.c_str());
  std::filesystem::path path = dir / filename;
  EXPECT_EQ(bm_SaveFileBitmap(path, src), 1);
  bm_FreeBitmap(src);
  return path;
}

uint32_t ReadBe32(const std::vector<uint8_t> &bytes, size_t offset) {
  return (uint32_t(bytes[offset]) << 24) | (uint32_t(bytes[offset + 1]) << 16) | (uint32_t(bytes[offset + 2]) << 8) |
         uint32_t(bytes[offset + 3]);
}
