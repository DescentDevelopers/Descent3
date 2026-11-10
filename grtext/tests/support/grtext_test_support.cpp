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

#include "grtext_test_support.h"

#include <algorithm>

#include "bitmap.h"
#include "cfile.h"
#include "grtext.h"
#include "d3_test/byte_writer.h"
#include "d3_test/game.h"
#include "d3_test/renderer.h"

namespace {

constexpr int16_t kFlagColor = 1, kFlagProportional = 2, kFlagKerned = 4, kFlagFFI2 = 32;

} // namespace

std::vector<uint8_t> FontFileBytes(const TestFont &font) {
  d3_test::ByteWriter out;
  out.U32(0xfeedbaba).I16(font.width).I16(font.height);
  int16_t flags = (font.color ? kFlagColor : 0) | (font.widths.empty() ? 0 : kFlagProportional) |
                  (font.kerning.empty() ? 0 : kFlagKerned) | (font.ffi2 ? kFlagFFI2 : 0);
  out.I16(flags);
  out.I16(0); // baseline
  out.U8(font.min_ascii).U8(font.max_ascii);
  out.FixedText("test font", 32);

  if (font.ffi2)
    out.I16(font.tracking).Zeros(62);
  for (uint8_t w : font.widths)
    out.I16(w);
  if (!font.kerning.empty()) {
    out.I16(static_cast<int16_t>(font.kerning.size()));
    for (const auto &k : font.kerning)
      out.U8(k.first).U8(k.second).U8(static_cast<uint8_t>(k.delta));
  }

  std::vector<uint8_t> pixels;
  for (int i = 0; i < font.NumGlyphs(); i++) {
    int w = font.GlyphWidth(font.min_ascii + i);
    size_t size = font.color ? size_t(w) * font.height * 2 : size_t((w + 7) / 8) * font.height;
    std::vector<uint8_t> glyph(size, 0);
    if (i < static_cast<int>(font.glyph_data.size()) && !font.glyph_data[i].empty()) {
      EXPECT_EQ(font.glyph_data[i].size(), size) << "bad test glyph data for glyph " << i;
      std::copy_n(font.glyph_data[i].begin(), std::min(size, font.glyph_data[i].size()), glyph.begin());
    }
    pixels.insert(pixels.end(), glyph.begin(), glyph.end());
  }
  out.U32(static_cast<uint32_t>(pixels.size())).Bytes(pixels);
  return out.bytes();
}

uint16_t BitmapPixel(int bm_handle, int x, int y) {
  const bms_bitmap &bm = GameBitmaps[bm_handle];
  EXPECT_TRUE(bm.used);
  EXPECT_TRUE(x >= 0 && x < bm.width && y >= 0 && y < bm.height);
  return bm.data16[y * bm.width + x];
}

void EnsureGrtextInitialized() {
  static bool initialized = false;
  if (!initialized) {
    grtext_Init();
    initialized = true;
  }
}

void GrtextTest::SetUp() {
  // grfont_Load keeps the name it is given in a 32 character buffer, so fonts are loaded by bare file name.
  cf_SetSearchPath(dir_.path());
  d3_test::EnsureBitmaps();
  EnsureGrtextInitialized();
  grfont_Reset();
  grtext_Reset();
  grtext_SetParameters(kLeft, kTop, kRight, kBottom, kTabSpaces);
  d3_test::ResetRenderer();
}

void GrtextTest::TearDown() {
  grtext_Reset();
  grfont_Reset();
  d3_test::ResetRenderer();
  cf_ClearAllSearchPaths();
}

std::filesystem::path GrtextTest::WriteFont(const TestFont &font) {
  return dir_.Write("font" + std::to_string(font_counter_++) + ".fnt", FontFileBytes(font));
}

int GrtextTest::LoadFont(const TestFont &font) {
  std::filesystem::path path = WriteFont(font);
  int handle = grfont_Load(path.filename().u8string().c_str());
  // grfont_Load's failure value is indistinguishable from handle 0, so check that the font really arrived.
  EXPECT_EQ(grfont_GetHeight(handle), font.height) << "font " << path << " did not load";
  return handle;
}
