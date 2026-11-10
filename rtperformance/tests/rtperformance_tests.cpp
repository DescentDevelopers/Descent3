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

// rtp_Init() is deliberately never called: it queries the real clock frequency (making seconds conversions
// unpredictable) and registers an atexit handler. Instead each test sets Runtime_performance_clockfreq to a known
// value so tick counts convert to exactly known seconds.

#include <cstring>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "d3_test/files.h"
#include "d3_test/game_clock.h"
#include "rtperformance.h"

#include "support/rtp_log.h"

#ifdef USE_RTP

namespace {

constexpr INT64 kTicksPerSecond = 1000;

class RtpLogTest : public ::testing::Test {
protected:
  void SetUp() override {
    d3_test::EnsureChronoTimer();
    std::string dir_str = dir_.path().string();
    ASSERT_LT(dir_str.size(), static_cast<size_t>(TABLE_NAME_LEN)); // the size manage.cpp gives LocalD3Dir
    strcpy(LocalD3Dir, dir_str.c_str());

    saved_clockfreq_ = Runtime_performance_clockfreq;
    Runtime_performance_clockfreq = kTicksPerSecond;
  }

  void TearDown() override {
    // Leave the logger idle so nothing later in the process writes into the (deleted) temp directory.
    Runtime_performance_enabled = 0;
    Runtime_performance_clockfreq = saved_clockfreq_;
    LocalD3Dir[0] = '\0';
  }

  d3_test::TempDir dir_;
  std::vector<std::string> header_;
  INT64 saved_clockfreq_ = 0;
};

TEST_F(RtpLogTest, RecordedFrameValuesAppearInLogWithTimesInSeconds) {
  rtp_StartLog();

  RTP_RECORDVALUE(renderframe_time, 2500); // ticks -> 2.5 s
  RTP_RECORDVALUE(aiframeall_time, 500);   // ticks -> 0.5 s
  RTP_RECORDVALUE(frame_time, 0.03f);      // already seconds
  RTP_RECORDVALUE(texture_uploads, 7);
  RTP_INCRVALUE(polys_drawn, 150);
  RTP_INCRVALUE(polys_drawn, 60);
  RTP_DECRVALUE(polys_drawn, 10);
  rtp_RecordFrame();

  RTP_RECORDVALUE(multiframe_time, 1000); // ticks -> 1.0 s
  rtp_RecordFrame();

  rtp_StopLog();

  std::vector<std::string> rows = ReadLogRows(dir_.path(), header_);
  ASSERT_EQ(rows.size(), 2u);

  EXPECT_NEAR(Column(header_, rows[0], "RenderFrameTime"), 2.5, 1e-6);
  EXPECT_NEAR(Column(header_, rows[0], "AIFrameAllTime"), 0.5, 1e-6);
  EXPECT_NEAR(Column(header_, rows[0], "FrameTime"), 0.03, 1e-6);
  EXPECT_EQ(Column(header_, rows[0], "REN:NumTexturesUploaded"), 7);
  EXPECT_EQ(Column(header_, rows[0], "REN:PolysDrawn"), 200);
  EXPECT_EQ(Column(header_, rows[0], "MultiFrameTime"), 0);

  EXPECT_NEAR(Column(header_, rows[1], "MultiFrameTime"), 1.0, 1e-6);
  EXPECT_EQ(Column(header_, rows[1], "RenderFrameTime"), 0);
  EXPECT_EQ(Column(header_, rows[1], "AIFrameAllTime"), 0);
  EXPECT_EQ(Column(header_, rows[1], "FrameTime"), 0);
  EXPECT_EQ(Column(header_, rows[1], "REN:PolysDrawn"), 0);

  EXPECT_EQ(Column(header_, rows[1], "FrameNum") - Column(header_, rows[0], "FrameNum"), 1);
}

TEST_F(RtpLogTest, StartingANewLogDiscardsFramesFromThePreviousOne) {
  rtp_StartLog();
  for (int i = 0; i < 5; i++) {
    RTP_RECORDVALUE(polys_drawn, 111);
    rtp_RecordFrame();
  }
  rtp_StopLog();

  rtp_StartLog();
  RTP_RECORDVALUE(polys_drawn, 1);
  rtp_RecordFrame();
  RTP_RECORDVALUE(polys_drawn, 2);
  rtp_RecordFrame();
  rtp_StopLog();

  std::vector<std::string> rows = ReadLogRows(dir_.path(), header_);
  ASSERT_EQ(rows.size(), 2u);
  EXPECT_EQ(Column(header_, rows[0], "REN:PolysDrawn"), 1);
  EXPECT_EQ(Column(header_, rows[1], "REN:PolysDrawn"), 2);
}

TEST_F(RtpLogTest, FramesWhilePausedAreNotLogged) {
  rtp_StartLog();
  RTP_RECORDVALUE(polys_drawn, 1);
  rtp_RecordFrame();

  rtp_PauseLog();
  for (int i = 0; i < 3; i++) {
    RTP_RECORDVALUE(polys_drawn, 99);
    rtp_RecordFrame();
  }

  rtp_ResumeLog();
  RTP_RECORDVALUE(polys_drawn, 2);
  rtp_RecordFrame();
  rtp_StopLog();

  std::vector<std::string> rows = ReadLogRows(dir_.path(), header_);
  ASSERT_EQ(rows.size(), 2u);
  EXPECT_EQ(Column(header_, rows[0], "REN:PolysDrawn"), 1);
  EXPECT_EQ(Column(header_, rows[1], "REN:PolysDrawn"), 2);
  // The frame counter keeps running while paused ("Update the frame counter to the next frame, always").
  EXPECT_EQ(Column(header_, rows[1], "FrameNum") - Column(header_, rows[0], "FrameNum"), 4);
}

TEST_F(RtpLogTest, FullBufferIsFlushedToDiskAndLoggingStops) {
  rtp_StartLog();

  int frames = 0;
  while (Runtime_performance_enabled && frames < 1000000) {
    RTP_RECORDVALUE(polys_drawn, frames + 1);
    rtp_RecordFrame();
    frames++;
  }
  ASSERT_FALSE(Runtime_performance_enabled) << "logging never stopped by itself";

  RTP_RECORDVALUE(polys_drawn, -1);
  rtp_RecordFrame();
  EXPECT_FALSE(Runtime_performance_enabled);

  std::vector<std::string> rows = ReadLogRows(dir_.path(), header_);
  ASSERT_EQ(rows.size(), static_cast<size_t>(frames));
  EXPECT_EQ(Column(header_, rows.front(), "REN:PolysDrawn"), 1);
  EXPECT_EQ(Column(header_, rows.back(), "REN:PolysDrawn"), frames);
}

} // namespace

#else

TEST(Rtperformance, NotCompiledIntoThisBuild) {
  GTEST_SKIP() << "run-time profiling (USE_RTP) is disabled in this configuration";
}

#endif
