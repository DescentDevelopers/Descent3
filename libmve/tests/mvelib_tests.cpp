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

#include <cstdio>
#include <filesystem>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "d3_test/byte_writer.h"
#include "d3_test/files.h"
#include "mvelib.h"
#include "support/mvelib_test_util.h"

using namespace mvetest;

namespace {

class MveLibTest : public ::testing::Test {
protected:
  void SetUp() override {
    TrackingAllocator::Reset();
    mve_alloc = TrackingAllocator::Alloc;
    mve_free = TrackingAllocator::Free;
    mve_read = MemRead;
  }
  void TearDown() override {
    EXPECT_EQ(TrackingAllocator::BadFrees(), 0) << "libmve freed memory it did not allocate";
    TrackingAllocator::Reset();
  }
};

TEST_F(MveLibTest, OpenAcceptsValidHeader) {
  MemStream s{MveBuilder().Chunk({{1, 0, {}}}).bytes()};
  MVEFILE *f = mvefile_open(&s);
  ASSERT_NE(f, nullptr);
  mvefile_close(f);
  EXPECT_EQ(LiveAllocations(), 0u);
}

TEST_F(MveLibTest, OpenRejectsEmptyStream) {
  MemStream s;
  EXPECT_EQ(mvefile_open(&s), nullptr);
  EXPECT_EQ(LiveAllocations(), 0u);
}

TEST_F(MveLibTest, OpenRejectsTruncatedHeader) {
  Bytes full = MveBuilder().bytes();
  MemStream s{Bytes(full.begin(), full.begin() + 20)};
  EXPECT_EQ(mvefile_open(&s), nullptr);
  EXPECT_EQ(LiveAllocations(), 0u);
}

TEST_F(MveLibTest, OpenRejectsCorruptSignature) {
  MveBuilder b;
  b.Chunk({{1, 0, {}}});
  b.bytes()[0] = 'X';
  MemStream s{b.bytes()};
  EXPECT_EQ(mvefile_open(&s), nullptr);
  EXPECT_EQ(LiveAllocations(), 0u);
}

TEST_F(MveLibTest, OpenRejectsWrongHeaderConstants) {
  // Each of the three 16-bit constants after the signature must match.
  for (size_t offset : {20u, 22u, 24u}) {
    SCOPED_TRACE(offset);
    MveBuilder b;
    b.Chunk({{1, 0, {}}});
    b.bytes()[offset] ^= 0x01;
    MemStream s{b.bytes()};
    EXPECT_EQ(mvefile_open(&s), nullptr);
  }
  EXPECT_EQ(LiveAllocations(), 0u);
}

TEST_F(MveLibTest, SegmentsAreIteratedInOrderWithTheirTypeAndPayload) {
  Bytes a = Payload(5, 0x10);
  Bytes c = Payload(300, 0x30);
  MemStream s{MveBuilder().Chunk({{0x0A, 1, a}, {0x05, 2, {}}, {0x11, 0, c}}).bytes()};
  MVEFILE *f = mvefile_open(&s);
  ASSERT_NE(f, nullptr);

  EXPECT_EQ(mvefile_get_next_segment_major(f), 0x0A);
  EXPECT_EQ(mvefile_get_next_segment_minor(f), 1);
  EXPECT_EQ(mvefile_get_next_segment_size(f), 5);
  EXPECT_EQ(SegmentData(f), a);
  mvefile_advance_segment(f);

  EXPECT_EQ(mvefile_get_next_segment_major(f), 0x05);
  EXPECT_EQ(mvefile_get_next_segment_minor(f), 2);
  EXPECT_EQ(mvefile_get_next_segment_size(f), 0);
  mvefile_advance_segment(f);

  EXPECT_EQ(mvefile_get_next_segment_major(f), 0x11);
  EXPECT_EQ(mvefile_get_next_segment_minor(f), 0);
  EXPECT_EQ(mvefile_get_next_segment_size(f), 300);
  EXPECT_EQ(SegmentData(f), c);
  mvefile_advance_segment(f);

  // End of chunk: the documented sentinels.
  EXPECT_EQ(mvefile_get_next_segment_major(f), 0xff);
  EXPECT_EQ(mvefile_get_next_segment_minor(f), 0xff);
  EXPECT_EQ(mvefile_get_next_segment_size(f), -1);
  EXPECT_EQ(mvefile_get_next_segment(f), nullptr);

  mvefile_advance_segment(f);
  EXPECT_EQ(mvefile_get_next_segment_major(f), 0xff);

  mvefile_close(f);
  EXPECT_EQ(LiveAllocations(), 0u);
}

TEST_F(MveLibTest, FetchNextChunkMovesThroughChunksAndReportsEndOfStream) {
  MemStream s{MveBuilder().Chunk({{2, 0, {0xAA}}}).Chunk({{3, 0, {0xBB, 0xCC}}}).bytes()};
  MVEFILE *f = mvefile_open(&s);
  ASSERT_NE(f, nullptr);

  // The first chunk is available right after opening.
  EXPECT_EQ(mvefile_get_next_segment_major(f), 2);
  EXPECT_EQ(SegmentData(f), Bytes({0xAA}));

  ASSERT_EQ(mvefile_fetch_next_chunk(f), 1);
  EXPECT_EQ(mvefile_get_next_segment_major(f), 3);
  EXPECT_EQ(SegmentData(f), Bytes({0xBB, 0xCC}));

  EXPECT_EQ(mvefile_fetch_next_chunk(f), 0);

  mvefile_close(f);
}

TEST_F(MveLibTest, ChunkLargerThanTheInitialBufferIsReadIntact) {
  // Chunks grow the read buffer on demand; data must survive the growth, and a later smaller chunk must still be read
  // correctly into the larger buffer.
  Bytes big1 = Payload(5000, 0x01);
  Bytes big2 = Payload(20000, 0x55);
  Bytes small = Payload(3, 0x99);
  MemStream s{MveBuilder().Chunk({{1, 0, big1}, {2, 0, big2}}).Chunk({{3, 0, small}}).bytes()};
  MVEFILE *f = mvefile_open(&s);
  ASSERT_NE(f, nullptr);

  EXPECT_EQ(SegmentData(f), big1);
  mvefile_advance_segment(f);
  EXPECT_EQ(SegmentData(f), big2);

  ASSERT_EQ(mvefile_fetch_next_chunk(f), 1);
  EXPECT_EQ(mvefile_get_next_segment_major(f), 3);
  EXPECT_EQ(SegmentData(f), small);

  mvefile_close(f);
  EXPECT_EQ(LiveAllocations(), 0u);
}

TEST_F(MveLibTest, TrailingBytesTooShortForASegmentHeaderEndTheChunk) {
  Bytes body = EncodeSegment({4, 0, {0x01, 0x02}});
  body.push_back(0x00); // 3 stray bytes: less than a 4-byte segment header
  body.push_back(0x00);
  body.push_back(0x09);
  MemStream s{MveBuilder().RawChunk(body).bytes()};
  MVEFILE *f = mvefile_open(&s);
  ASSERT_NE(f, nullptr);

  EXPECT_EQ(mvefile_get_next_segment_major(f), 4);
  mvefile_advance_segment(f);
  EXPECT_EQ(mvefile_get_next_segment_major(f), 0xff);
  EXPECT_EQ(mvefile_get_next_segment_size(f), -1);
  EXPECT_EQ(mvefile_get_next_segment(f), nullptr);

  mvefile_close(f);
}

TEST_F(MveLibTest, TruncatedChunkIsReportedAsEndOfStream) {
  MveBuilder b;
  b.Chunk({{1, 0, {0x01}}});
  // Second chunk claims 100 bytes but only 10 follow.
  b.Raw(d3_test::ByteWriter().U16(100).U16(0));
  b.Raw(Bytes(10, 0xEE));
  MemStream s{b.bytes()};
  MVEFILE *f = mvefile_open(&s);
  ASSERT_NE(f, nullptr);

  EXPECT_EQ(mvefile_fetch_next_chunk(f), 0);

  mvefile_close(f);
}

class MveStreamTest : public MveLibTest {
protected:
  void SetUp() override {
    MveLibTest::SetUp();
    g_calls.clear();
    g_stop_on_major = -1;
  }
};

TEST_F(MveStreamTest, DispatchesSegmentsToRegisteredHandlersInOrder) {
  int ctx = 0;
  MemStream s{MveBuilder()
                  .Chunk({{2, 7, {0x01, 0x02}}, {3, 0, {0x03}}, {5, 1, {}}, {2, 0, {0x04}}})
                  .Chunk({{2, 0, {0x05}}})
                  .bytes()};
  MVESTREAM *m = mve_open(&s);
  ASSERT_NE(m, nullptr);
  mve_set_handler(m, 2, RecordingHandler);
  mve_set_handler(m, 5, RecordingHandler);
  mve_set_handler_context(m, &ctx);

  // Not the last chunk, so playback reports more to come.
  EXPECT_EQ(mve_play_next_chunk(m), 1);

  // Segment type 3 has no handler and is skipped.
  ASSERT_EQ(g_calls.size(), 3u);
  EXPECT_EQ(g_calls[0].major, 2);
  EXPECT_EQ(g_calls[0].minor, 7);
  EXPECT_EQ(g_calls[0].data, Bytes({0x01, 0x02}));
  EXPECT_EQ(g_calls[0].context, &ctx);
  EXPECT_EQ(g_calls[1].major, 5);
  EXPECT_EQ(g_calls[1].minor, 1);
  EXPECT_TRUE(g_calls[1].data.empty());
  EXPECT_EQ(g_calls[2].major, 2);
  EXPECT_EQ(g_calls[2].data, Bytes({0x04}));

  g_calls.clear();
  EXPECT_EQ(mve_play_next_chunk(m), 0);
  ASSERT_EQ(g_calls.size(), 1u);
  EXPECT_EQ(g_calls[0].data, Bytes({0x05}));

  mve_close(m);
  EXPECT_EQ(LiveAllocations(), 0u);
}

TEST_F(MveStreamTest, HandlerReturningZeroStopsPlayback) {
  MemStream s{MveBuilder().Chunk({{1, 0, {}}, {2, 0, {}}, {3, 0, {}}}).Chunk({{1, 0, {}}}).bytes()};
  MVESTREAM *m = mve_open(&s);
  ASSERT_NE(m, nullptr);
  for (unsigned char major : {1, 2, 3})
    mve_set_handler(m, major, RecordingHandler);
  g_stop_on_major = 2;

  EXPECT_EQ(mve_play_next_chunk(m), 0);
  ASSERT_EQ(g_calls.size(), 2u);
  EXPECT_EQ(g_calls[0].major, 1);
  EXPECT_EQ(g_calls[1].major, 2);

  mve_close(m);
}

TEST_F(MveStreamTest, OutOfRangeSegmentTypesAreNeverDispatched) {
  // Handlers exist only for segment types 0-31.
  MemStream s{MveBuilder().Chunk({{40, 0, {0x01}}, {31, 0, {0x02}}}).bytes()};
  MVESTREAM *m = mve_open(&s);
  ASSERT_NE(m, nullptr);
  mve_set_handler(m, 40, RecordingHandler);
  mve_set_handler(m, 31, RecordingHandler);

  mve_play_next_chunk(m);
  ASSERT_EQ(g_calls.size(), 1u);
  EXPECT_EQ(g_calls[0].major, 31);

  mve_close(m);
}

class MveResetTest : public MveStreamTest {
protected:
  void SetUp() override {
    MveStreamTest::SetUp();
    mve_read = FileRead;
    const std::filesystem::path path =
        dir_.Write("reset.mve", MveBuilder().Chunk({{2, 0, {0xAA}}}).Chunk({{3, 0, {0xBB}}}).bytes());
    file_ = fopen(path.string().c_str(), "rb");
    ASSERT_NE(file_, nullptr);
  }
  void TearDown() override {
    if (file_)
      fclose(file_);
    MveStreamTest::TearDown();
  }

  d3_test::TempDir dir_;
  FILE *file_ = nullptr;
};

TEST_F(MveResetTest, ResetRestartsPlaybackFromTheFirstChunk) {
  MVESTREAM *m = mve_open(file_);
  ASSERT_NE(m, nullptr);
  mve_set_handler(m, 2, RecordingHandler);
  mve_set_handler(m, 3, RecordingHandler);

  EXPECT_EQ(mve_play_next_chunk(m), 1);
  EXPECT_EQ(mve_play_next_chunk(m), 0);

  mve_reset(m);
  g_calls.clear();
  EXPECT_EQ(mve_play_next_chunk(m), 1);
  ASSERT_EQ(g_calls.size(), 1u);
  EXPECT_EQ(g_calls[0].major, 2);
  EXPECT_EQ(g_calls[0].data, Bytes({0xAA}));

  mve_close(m);
}

} // namespace
