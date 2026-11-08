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

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <string>

#include <gtest/gtest.h>
#include <SDL3/SDL_assert.h>

#include "application.h"
#include "ddgr.h"
#include "pserror.h"

#include "support/generic_surface.h"
#include "support/locked_pixels.h"

namespace {

class DdgrGdiTest : public ::testing::Test {
protected:
  void SetUp() override {
    // A windowless application object: the GDI subsystem only reads its window handle.
    tWin32AppInfo info{};
    m_app = std::make_unique<oeWin32Application>(&info);

    char subsystem[] = "GDI32";
    ASSERT_TRUE(ddgr_Init(m_app.get(), subsystem, false));

    // As in the editor, a back-buffered video surface is set up before any other surface. It is
    // what gives the subsystem the device context used for offscreen drawing.
    m_screen = MakeSurface(64, 48, BPP_16, SURFTYPE_VIDEOSCREEN, SURFFLAG_BACKBUFFER, "screen");
    ASSERT_TRUE(ddgr_surf_InitVideo(&m_screen));
    m_screen_open = true;
  }

  void TearDown() override {
    if (m_screen_open)
      ddgr_surf_CloseVideo(&m_screen);
    ddgr_Close();
    m_app.reset();
  }

  std::unique_ptr<oeWin32Application> m_app;
  ddgr_surface m_screen{};
  bool m_screen_open = false;
};

const uint32_t kSentinel16 = 0x1234; // a valid 555 value (top bit clear) that no test clears to
const uint32_t kSentinel32 = 0x00a5a5a5;

class DdgrGdiBltTest : public DdgrGdiTest, public ::testing::WithParamInterface<int> {};

// Odd surface widths make a wrong reported row stride show up as misplaced pixels.
TEST_P(DdgrGdiBltTest, BltCopiesSubRectToOffsetExactly) {
  const int bpp = GetParam();
  const int SW = 7, SH = 5, DW = 9, DH = 8;
  const int sx = 2, sy = 1, bw = 4, bh = 3, dx = 3, dy = 4;
  const uint32_t sentinel = bpp == BPP_16 ? kSentinel16 : kSentinel32;

  GenericSurface src(SW, SH, bpp);
  GenericSurface dst(DW, DH, bpp);

  // Every source pixel gets a distinct value, different from the sentinel and valid in 555.
  auto pattern = [](int x, int y) { return static_cast<uint32_t>(0x0100 + y * 16 + x); };
  {
    LockedPixels px(src.get());
    ASSERT_TRUE(px.valid());
    for (int y = 0; y < SH; y++)
      for (int x = 0; x < SW; x++)
        px.set(x, y, pattern(x, y));
  }
  {
    LockedPixels px(dst.get());
    ASSERT_TRUE(px.valid());
    px.fill(sentinel);
  }

  EXPECT_TRUE(ddgr_surf_Blt(dst.get(), dx, dy, src.get(), sx, sy, bw, bh));

  LockedPixels px(dst.get());
  ASSERT_TRUE(px.valid());
  for (int y = 0; y < DH; y++) {
    for (int x = 0; x < DW; x++) {
      const bool inside = x >= dx && x < dx + bw && y >= dy && y < dy + bh;
      const uint32_t expected = inside ? pattern(x - dx + sx, y - dy + sy) : sentinel;
      EXPECT_EQ(px.get(x, y), expected) << "at (" << x << "," << y << ")";
    }
  }
}

INSTANTIATE_TEST_SUITE_P(Depths, DdgrGdiBltTest, ::testing::Values(BPP_16, BPP_32),
                         [](const ::testing::TestParamInfo<int> &info) {
                           return info.param == BPP_16 ? std::string("16bpp") : std::string("32bpp");
                         });

} // namespace
