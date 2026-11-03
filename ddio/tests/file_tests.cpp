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

#include <chrono>
#include <cstdint>
#include <filesystem>
#include <regex>
#include <set>
#include <string>
#include <gtest/gtest.h>

#include "ddio.h"

#include "support/lock_file.h"
#include "d3_test/files.h"

namespace fs = std::filesystem;

namespace {

struct DdioFile : testing::Test {
  d3_test::TempDir temp;
  const fs::path dir = temp.path();
};

// No real process has this PID: Linux caps PIDs at 2^22, and Windows PIDs, multiples of 4, stay far smaller
constexpr int32_t kImpossiblePid = 0x7FFFFFFC;

} // namespace

TEST_F(DdioFile, LockCreateDeleteRoundTrip) {
  ASSERT_TRUE(ddio_CreateLockFile(dir));
  EXPECT_TRUE(fs::exists(dir / ".lock"));

  EXPECT_TRUE(ddio_DeleteLockFile(dir));
  EXPECT_FALSE(fs::exists(dir / ".lock"));
}

TEST_F(DdioFile, LockCanBeRetakenBySameProcess) {
  ASSERT_TRUE(ddio_CreateLockFile(dir));
  EXPECT_TRUE(ddio_CreateLockFile(dir));
  EXPECT_TRUE(ddio_DeleteLockFile(dir));
  EXPECT_FALSE(fs::exists(dir / ".lock"));
}

TEST_F(DdioFile, LockInMissingDirectoryFails) {
  EXPECT_FALSE(ddio_CreateLockFile(dir / "missing"));
  EXPECT_FALSE(fs::exists(dir / "missing"));
}

TEST_F(DdioFile, DeletingAbsentLockSucceeds) { EXPECT_TRUE(ddio_DeleteLockFile(dir)); }

TEST_F(DdioFile, ForeignFileNamedLockIsLeftAlone) {
  const std::string foreign = "not a lock file at all";
  temp.Write(".lock", foreign);

  EXPECT_FALSE(ddio_CreateLockFile(dir));
  EXPECT_FALSE(ddio_DeleteLockFile(dir));
  EXPECT_EQ(d3_test::ReadFileText(dir / ".lock"), foreign);
}

TEST_F(DdioFile, StaleLockIsReclaimed) {
  ASSERT_FALSE(ddio_CheckProcess(kImpossiblePid));
  WriteLockFile(dir, kImpossiblePid);

  EXPECT_TRUE(ddio_CreateLockFile(dir));
  EXPECT_TRUE(ddio_DeleteLockFile(dir));
  EXPECT_FALSE(fs::exists(dir / ".lock"));
}

TEST_F(DdioFile, LockHeldByLiveProcessIsRespected) {
  const int other = ParentPid();
  if (other <= 0 || other == ddio_GetPID() || !ddio_CheckProcess(other)) {
    GTEST_SKIP() << "no observable running process other than this one to own the lock";
  }
  WriteLockFile(dir, other);
  const std::string before = d3_test::ReadFileText(dir / ".lock");

  EXPECT_FALSE(ddio_CreateLockFile(dir));
  EXPECT_FALSE(ddio_DeleteLockFile(dir));
  EXPECT_EQ(d3_test::ReadFileText(dir / ".lock"), before);
}

TEST_F(DdioFile, ForeachFileVisitsMatchingRegularFilesOnly) {
  temp.Write("a.txt");
  temp.Write("b.txt");
  temp.Write("c.txt.bak");
  temp.Write("d.dat");
  fs::create_directory(dir / "e.txt");
  fs::create_directory(dir / "sub");
  temp.Write("sub/f.txt");

  std::set<fs::path> visited;
  ddio_DoForeachFile(dir, std::regex(R"(.*\.txt)"), [&](const fs::path &p) { visited.insert(p); });

  EXPECT_EQ(visited, (std::set<fs::path>{dir / "a.txt", dir / "b.txt"}));
}

TEST_F(DdioFile, ForeachFileInMissingDirectoryVisitsNothing) {
  int calls = 0;
  ddio_DoForeachFile(dir / "missing", std::regex(".*"), [&](const fs::path &) { calls++; });
  EXPECT_EQ(calls, 0);
}

TEST_F(DdioFile, TmpFileNameKeepsPrefix) {
  const fs::path name = ddio_GetTmpFileName(dir, "save_");
  ASSERT_FALSE(name.empty());
  EXPECT_EQ(name.filename().string().rfind("save_", 0), 0u) << name;
}

TEST_F(DdioFile, FileDiffComparesSizeAndModificationTime) {
  const auto when = fs::file_time_type::clock::now() - std::chrono::hours(48);
  const fs::path a = temp.Write("a", "same");
  const fs::path b = temp.Write("b", "SAME");
  const fs::path longer = temp.Write("longer", "same but longer");
  fs::last_write_time(a, when);
  fs::last_write_time(b, when);
  fs::last_write_time(longer, when);

  EXPECT_FALSE(ddio_FileDiff(a, b)) << "same size and time count as the same file";
  EXPECT_TRUE(ddio_FileDiff(a, longer));

  // A whole day apart: the Windows CRT's stat() goes through local time, so two times exactly an hour apart can read
  // the same around a daylight saving change
  fs::last_write_time(b, when + std::chrono::hours(24));
  EXPECT_TRUE(ddio_FileDiff(a, b));
}

TEST_F(DdioFile, CopyFileTimeMakesTimesMatch) {
  const fs::path src = temp.Write("src", "contents");
  const fs::path dest = temp.Write("dest", "CONTENTS");
  const auto src_time = fs::file_time_type::clock::now() - std::chrono::hours(48);
  fs::last_write_time(src, src_time);
  ASSERT_TRUE(ddio_FileDiff(dest, src));

  ddio_CopyFileTime(dest, src);

  // Some platforms only copy whole seconds
  const auto skew = fs::last_write_time(dest) - src_time;
  EXPECT_LT(std::chrono::abs(skew), std::chrono::seconds(2));
  EXPECT_FALSE(ddio_FileDiff(dest, src));
  EXPECT_EQ(d3_test::ReadFileText(dest), "CONTENTS");
}

TEST_F(DdioFile, DeleteFileReportsSuccess) {
  const fs::path p = temp.Write("doomed");
  EXPECT_NE(ddio_DeleteFile(p.string().c_str()), 0);
  EXPECT_FALSE(fs::exists(p));

  EXPECT_EQ(ddio_DeleteFile(p.string().c_str()), 0) << "deleting a file that is not there should fail";
}

namespace {
const std::string kSep(1, static_cast<char>(fs::path::preferred_separator));
}

TEST(DdioPath, MakePathJoinsWithSeparator) {
  char buf[256];
  ddio_MakePath(buf, "base", "data", "misc", "file.txt", nullptr);
  EXPECT_EQ(std::string(buf), "base" + kSep + "data" + kSep + "misc" + kSep + "file.txt");
}

TEST(DdioPath, MakePathDoesNotDoubleTrailingSeparator) {
  char buf[256];
  const std::string base = "base" + kSep;
  ddio_MakePath(buf, base.c_str(), "file.txt", nullptr);
  EXPECT_EQ(std::string(buf), "base" + kSep + "file.txt");
}

TEST(DdioPath, MakePathCanAppendInPlace) {
  char buf[256] = "base";
  ddio_MakePath(buf, buf, "sub", nullptr);
  EXPECT_EQ(std::string(buf), "base" + kSep + "sub");
}

TEST(DdioPath, SplitPathSeparatesNameAndExtension) {
  char path[256], name[256], ext[256];
  const std::string src = "base" + kSep + "sub" + kSep + "level.d3l";
  ddio_SplitPath(src.c_str(), path, name, ext);
  EXPECT_STREQ(name, "level");
  EXPECT_STREQ(ext, ".d3l");
}

TEST(DdioPath, SplitPathExtensionIsAfterLastDot) {
  char path[256], name[256], ext[256];
  const std::string src = "base" + kSep + "archive.tar.gz";
  ddio_SplitPath(src.c_str(), path, name, ext);
  EXPECT_STREQ(name, "archive.tar");
  EXPECT_STREQ(ext, ".gz");
}

TEST(DdioPath, SplitPathDotInDirectoryIsNotAnExtension) {
  char path[256], name[256], ext[256];
  const std::string src = "base.d" + kSep + "README";
  ddio_SplitPath(src.c_str(), path, name, ext);
  EXPECT_STREQ(name, "README");
  EXPECT_STREQ(ext, "");
}

TEST(DdioPath, SplitPathAcceptsNullOutputs) {
  char name[256];
  const std::string src = "base" + kSep + "level.d3l";
  ddio_SplitPath(src.c_str(), nullptr, name, nullptr);
  EXPECT_STREQ(name, "level");
}

TEST(DdioPath, SplitThenMakeRebuildsPath) {
  for (const std::string &src : {"base" + kSep + "level.d3l", "base" + kSep + "sub" + kSep + "level.d3l",
                                 "base" + kSep + "sub" + kSep + "noext"}) {
    char path[256], name[256], ext[256], rebuilt[256];
    ddio_SplitPath(src.c_str(), path, name, ext);
    const std::string file = std::string(name) + ext;
    ddio_MakePath(rebuilt, path, file.c_str(), nullptr);
    EXPECT_EQ(std::string(rebuilt), src);
  }
}
