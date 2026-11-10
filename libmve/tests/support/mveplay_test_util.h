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

#ifndef LIBMVE_TESTS_MVEPLAY_TEST_UTIL_H_
#define LIBMVE_TESTS_MVEPLAY_TEST_UTIL_H_

#include <cstdint>
#include <initializer_list>
#include <memory>
#include <vector>

#include "d3_test/byte_writer.h"
#include "mve_test_util.h"
#include "mvelib.h"

namespace mvetest {

// See the MVE_OPCODE_* list in mveplay.cpp.
constexpr uint8_t kEndOfStream = 0x00;
constexpr uint8_t kEndOfChunk = 0x01;
constexpr uint8_t kInitAudioBuffers = 0x03;
constexpr uint8_t kInitVideoBuffers = 0x05;
constexpr uint8_t kDisplayVideo = 0x07;
constexpr uint8_t kAudioSilence = 0x09;
constexpr uint8_t kInitVideoMode = 0x0A;
constexpr uint8_t kSetPalette = 0x0C;
constexpr uint8_t kSetDecodingMap = 0x0F;
constexpr uint8_t kVideoData = 0x11;

inline Bytes LE16s(std::initializer_list<uint16_t> values) {
  d3_test::ByteWriter out;
  for (uint16_t v : values)
    out.U16(v);
  return out.bytes();
}

inline Segment VideoMode(uint16_t screen_w, uint16_t screen_h) {
  return {kInitVideoMode, 0, LE16s({screen_w, screen_h, 0})};
}

inline Segment VideoBuffers8(uint16_t w_blocks, uint16_t h_blocks) {
  return {kInitVideoBuffers, 0, LE16s({w_blocks, h_blocks})};
}
inline Segment VideoBuffers16(uint16_t w_blocks, uint16_t h_blocks) {
  return {kInitVideoBuffers, 2, LE16s({w_blocks, h_blocks, 1, 1})}; // count = 1, truecolor = 1
}

inline Segment AudioBuffersMono8(uint16_t sample_rate) {
  return {kInitAudioBuffers, 0, LE16s({0, 0, sample_rate, 0, 0})}; // flags = 0, then a 32-bit buffer size
}

/// Stream mask 1 selects the channel the player decodes; the last field is the length of silence in bytes.
inline Segment AudioSilence(uint16_t bytes) { return {kAudioSilence, 0, LE16s({0, 1, bytes})}; }

/// The last header field is the flags; bit 0 swaps the frame buffers first.
inline Segment VideoData(uint16_t flags, const Bytes &stream) {
  return {kVideoData, 0, d3_test::ByteWriter().Bytes(LE16s({0, 0, 0, 0, 0, 0, flags})).Bytes(stream).bytes()};
}

struct ShownFrame {
  Bytes pixels;
  unsigned bufw, bufh, sx, sy, w, h, dstx, dsty, hicolor;
};
inline std::vector<ShownFrame> g_frames;

inline void ShowFrame(unsigned char *buffer, unsigned int bufw, unsigned int bufh, unsigned int sx, unsigned int sy,
                      unsigned int w, unsigned int h, unsigned int dstx, unsigned int dsty, unsigned int hicolor) {
  size_t bytes = size_t(bufw) * bufh * (hicolor ? 2 : 1);
  g_frames.push_back({Bytes(buffer, buffer + bytes), bufw, bufh, sx, sy, w, h, dstx, dsty, hicolor});
}

struct PaletteUpdate {
  unsigned start, count;
  Bytes rgb;
};
inline std::vector<PaletteUpdate> g_palettes;

inline void SetPalette(const unsigned char *p, unsigned int start, unsigned int count) {
  // Descent3's callback (d3movie.cpp) indexes the buffer by palette entry: entry i is at p[3 * i].
  const unsigned char *first = p + 3 * start;
  g_palettes.push_back({start, count, Bytes(first, first + 3 * count)});
}

/// A 16x8 palettized movie with one frame: left block solid 0x11, right block solid 0x22.
inline Bytes OneFrameMovie() {
  return MveBuilder()
      .Chunk({VideoMode(64, 48), VideoBuffers8(2, 1), {kEndOfChunk, 0, {}}})
      .Chunk({{kSetDecodingMap, 0, {0xee}}, VideoData(0, {0x11, 0x22}), {kDisplayVideo, 0, {}}, {kEndOfChunk, 0, {}}})
      .Chunk({{kEndOfStream, 0, {}}})
      .bytes();
}

inline Bytes Solid8(uint8_t left, uint8_t right) {
  Bytes px(16 * 8);
  for (int y = 0; y < 8; y++)
    for (int x = 0; x < 16; x++)
      px[y * 16 + x] = x < 8 ? left : right;
  return px;
}

struct MovieEnder {
  void operator()(MVESTREAM *movie) const { MVE_rmEndMovie(movie); }
};
using MovieHandle = std::unique_ptr<MVESTREAM, MovieEnder>;

inline MovieHandle PrepMovie(MemStream &stream, int x, int y) {
  return MovieHandle(MVE_rmPrepMovie(&stream, x, y, 0));
}

} // namespace mvetest

#endif // LIBMVE_TESTS_MVEPLAY_TEST_UTIL_H_
