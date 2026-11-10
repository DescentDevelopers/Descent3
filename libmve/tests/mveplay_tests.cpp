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

// Sound is disabled (MVE_sndInit(false)), or routed to SDL's dummy driver, so no audio device is needed, and the
// movies have no timer segment, so playback never waits.

#include <cstdint>
#include <cstring>
#include <vector>

#include <SDL3/SDL_hints.h>
#include <SDL3/SDL_init.h>
#include <gtest/gtest.h>

#include "d3_test/byte_writer.h"
#include "d3_test/game_clock.h"
#include "mvelib.h"
#include "support/mveplay_test_util.h"

using namespace mvetest;

namespace {

class MvePlayTest : public ::testing::Test {
protected:
  void SetUp() override {
    d3_test::EnsureChronoTimer();
    TrackingAllocator::Reset();
    g_frames.clear();
    g_palettes.clear();
    MVE_memCallbacks(TrackingAllocator::Alloc, TrackingAllocator::Free);
    MVE_ioCallbacks(MemRead);
    MVE_sfCallbacks(ShowFrame);
    MVE_palCallbacks(SetPalette);
    MVE_sndInit(false);
  }
  void TearDown() override {
    EXPECT_EQ(TrackingAllocator::BadFrees(), 0);
    EXPECT_EQ(TrackingAllocator::Live().size(), 0u) << "movie memory not released by MVE_rmEndMovie";
    TrackingAllocator::Reset();
  }

  MemStream stream_;
};

TEST_F(MvePlayTest, PrepRejectsAnInvalidFile) {
  stream_.bytes = Bytes(64, 0);
  EXPECT_EQ(PrepMovie(stream_, -1, -1), nullptr);
}

TEST_F(MvePlayTest, PlaysAFrameThenReportsEndOfFile) {
  stream_.bytes = OneFrameMovie();
  auto movie = PrepMovie(stream_, -1, -1);
  ASSERT_NE(movie, nullptr);

  MVE_videoSpec spec{};
  MVE_getVideoSpec(&spec);
  EXPECT_EQ(spec.screenWidth, 64);
  EXPECT_EQ(spec.screenHeight, 48);
  EXPECT_EQ(spec.width, 16);
  EXPECT_EQ(spec.height, 8);
  EXPECT_EQ(spec.truecolor, 0);

  EXPECT_EQ(MVE_rmStepMovie(movie.get()), 0);
  ASSERT_EQ(g_frames.size(), 1u);
  const ShownFrame &f = g_frames[0];
  EXPECT_EQ(f.bufw, 16u);
  EXPECT_EQ(f.bufh, 8u);
  EXPECT_EQ(f.sx, 0u);
  EXPECT_EQ(f.sy, 0u);
  EXPECT_EQ(f.w, 16u);
  EXPECT_EQ(f.h, 8u);
  EXPECT_EQ(f.hicolor, 0u);
  EXPECT_EQ(f.pixels, Solid8(0x11, 0x22));

  EXPECT_EQ(MVE_rmStepMovie(movie.get()), MVE_ERR_EOF);
  EXPECT_EQ(g_frames.size(), 1u);
}

TEST_F(MvePlayTest, NegativePositionCentersTheMovieOnTheScreen) {
  stream_.bytes = OneFrameMovie();
  auto movie = PrepMovie(stream_, -1, -1);
  ASSERT_NE(movie, nullptr);
  ASSERT_EQ(MVE_rmStepMovie(movie.get()), 0);
  ASSERT_EQ(g_frames.size(), 1u);
  EXPECT_EQ(g_frames[0].dstx, (64u - 16u) / 2);
  EXPECT_EQ(g_frames[0].dsty, (48u - 8u) / 2);
}

TEST_F(MvePlayTest, ExplicitPositionIsPassedThrough) {
  stream_.bytes = OneFrameMovie();
  auto movie = PrepMovie(stream_, 5, 7);
  ASSERT_NE(movie, nullptr);
  ASSERT_EQ(MVE_rmStepMovie(movie.get()), 0);
  ASSERT_EQ(g_frames.size(), 1u);
  EXPECT_EQ(g_frames[0].dstx, 5u);
  EXPECT_EQ(g_frames[0].dsty, 7u);
}

TEST_F(MvePlayTest, PaletteUpdatesReachTheCallbackIndexedByEntry) {
  Bytes rgb = {1, 2, 3, 4, 5, 6};
  Bytes palette = d3_test::ByteWriter().U16(16).U16(2).Bytes(rgb).bytes(); // start entry, count, then the entries
  stream_.bytes = MveBuilder()
                      .Chunk({VideoMode(16, 8), VideoBuffers8(2, 1), {kSetPalette, 0, palette}, {kEndOfChunk, 0, {}}})
                      .Chunk({{kEndOfStream, 0, {}}})
                      .bytes();
  auto movie = PrepMovie(stream_, 0, 0);
  ASSERT_NE(movie, nullptr);

  ASSERT_EQ(g_palettes.size(), 1u);
  EXPECT_EQ(g_palettes[0].start, 16u);
  EXPECT_EQ(g_palettes[0].count, 2u);
  EXPECT_EQ(g_palettes[0].rgb, rgb);
}

TEST_F(MvePlayTest, SwapFlagMakesThePreviousFrameAvailableToTheNextOne) {
  // Frame 2 sets the swap flag, so opcode 0x0 (copy from the previous frame) in its left block must reproduce frame
  // 1's left block.
  stream_.bytes =
      MveBuilder()
          .Chunk({VideoMode(16, 8), VideoBuffers8(2, 1), {kEndOfChunk, 0, {}}})
          .Chunk(
              {{kSetDecodingMap, 0, {0xee}}, VideoData(0, {0x11, 0x22}), {kDisplayVideo, 0, {}}, {kEndOfChunk, 0, {}}})
          .Chunk({{kSetDecodingMap, 0, {0xe0}}, VideoData(1, {0x33}), {kDisplayVideo, 0, {}}, {kEndOfChunk, 0, {}}})
          .Chunk({{kEndOfStream, 0, {}}})
          .bytes();
  auto movie = PrepMovie(stream_, 0, 0);
  ASSERT_NE(movie, nullptr);

  ASSERT_EQ(MVE_rmStepMovie(movie.get()), 0);
  ASSERT_EQ(MVE_rmStepMovie(movie.get()), 0);
  ASSERT_EQ(g_frames.size(), 2u);
  EXPECT_EQ(g_frames[0].pixels, Solid8(0x11, 0x22));
  EXPECT_EQ(g_frames[1].pixels, Solid8(0x11, 0x33));
  EXPECT_EQ(MVE_rmStepMovie(movie.get()), MVE_ERR_EOF);
}

TEST_F(MvePlayTest, TruecolorMovieDecodesSixteenBitFrames) {
  Bytes stream = LE16s({6, 0x1234, 0x5678}); // LE16 offset to the (empty) motion stream, then two pixels
  stream_.bytes =
      MveBuilder()
          .Chunk({VideoMode(16, 8), VideoBuffers16(2, 1), {kEndOfChunk, 0, {}}})
          .Chunk({{kSetDecodingMap, 0, {0xee}}, VideoData(0, stream), {kDisplayVideo, 0, {}}, {kEndOfChunk, 0, {}}})
          .Chunk({{kEndOfStream, 0, {}}})
          .bytes();
  auto movie = PrepMovie(stream_, 0, 0);
  ASSERT_NE(movie, nullptr);

  MVE_videoSpec spec{};
  MVE_getVideoSpec(&spec);
  EXPECT_NE(spec.truecolor, 0);

  ASSERT_EQ(MVE_rmStepMovie(movie.get()), 0);
  ASSERT_EQ(g_frames.size(), 1u);
  EXPECT_NE(g_frames[0].hicolor, 0u);
  ASSERT_EQ(g_frames[0].pixels.size(), 16u * 8u * 2u);
  std::vector<uint16_t> px(16 * 8);
  memcpy(px.data(), g_frames[0].pixels.data(), g_frames[0].pixels.size());
  for (int y = 0; y < 8; y++)
    for (int x = 0; x < 16; x++)
      EXPECT_EQ(px[y * 16 + x], x < 8 ? 0x1234 : 0x5678) << "(" << x << "," << y << ")";
}

TEST_F(MvePlayTest, MoviesCanBePlayedOneAfterAnother) {
  // The player keeps its state in globals.
  stream_.bytes = OneFrameMovie();
  auto movie = PrepMovie(stream_, -1, -1);
  ASSERT_NE(movie, nullptr);
  ASSERT_EQ(MVE_rmStepMovie(movie.get()), 0);
  movie.reset();

  stream_ = MemStream{MveBuilder()
                          .Chunk({VideoMode(32, 16), VideoBuffers8(4, 2), {kEndOfChunk, 0, {}}})
                          .Chunk({{kSetDecodingMap, 0, {0xee, 0xee, 0xee, 0xee}},
                                  VideoData(0, {1, 2, 3, 4, 5, 6, 7, 8}),
                                  {kDisplayVideo, 0, {}},
                                  {kEndOfChunk, 0, {}}})
                          .Chunk({{kEndOfStream, 0, {}}})
                          .bytes()};
  movie = PrepMovie(stream_, 0, 0);
  ASSERT_NE(movie, nullptr);
  MVE_videoSpec spec{};
  MVE_getVideoSpec(&spec);
  EXPECT_EQ(spec.screenWidth, 32);
  EXPECT_EQ(spec.width, 32);
  EXPECT_EQ(spec.height, 16);

  ASSERT_EQ(MVE_rmStepMovie(movie.get()), 0);
  ASSERT_EQ(g_frames.size(), 2u);
  const ShownFrame &f = g_frames[1];
  ASSERT_EQ(f.bufw, 32u);
  ASSERT_EQ(f.bufh, 16u);
  // Bottom-right block is the eighth solid color.
  EXPECT_EQ(f.pixels[15 * 32 + 31], 8);
  EXPECT_EQ(f.pixels[0], 1);
}

class MvePlayAudioTest : public MvePlayTest {
protected:
  void SetUp() override {
    MvePlayTest::SetUp();
    SDL_SetHint(SDL_HINT_AUDIO_DRIVER, "dummy");
    ASSERT_TRUE(SDL_InitSubSystem(SDL_INIT_AUDIO)) << SDL_GetError();
    MVE_sndInit(true);
  }
  void TearDown() override {
    MVE_sndInit(false);
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
    MvePlayTest::TearDown();
  }
};

} // namespace
