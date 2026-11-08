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

#ifndef CFILE_TEST_UTILS_H
#define CFILE_TEST_UTILS_H

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "cfile.h"
#include "d3_test/byte_writer.h"
#include "d3_test/files.h"

namespace cfile_test {

// Returns s with every character upper-cased.
inline std::string Upper(std::string s) {
  for (auto &c : s)
    c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
  return s;
}

// Returns the IEEE-754 bit pattern of f.
inline uint32_t FloatBits(float f) {
  uint32_t u;
  std::memcpy(&u, &f, sizeof(u));
  return u;
}

// Returns the IEEE-754 bit pattern of d.
inline uint64_t DoubleBits(double d) {
  uint64_t u;
  std::memcpy(&u, &d, sizeof(u));
  return u;
}

inline std::string ReadRest(CFILE *cf) {
  std::string out;
  int c;
  while ((c = cfgetc(cf)) != EOF)
    out.push_back(static_cast<char>(c));
  return out;
}

struct HogEntry {
  std::string name;
  std::string data;
};

// Builds a HOG2 archive, the container format of Descent 3's .hog files:
//   "HOG2", uint32 file count, uint32 offset of the first file's data,
//   padding up to a 68-byte header, then one 48-byte record per file
//   (36-byte NUL-padded name, uint32 flags, uint32 length, uint32 timestamp),
//   then the file contents back to back in record order. All integers are
//   little-endian. Records are sorted by case-insensitive name, as the HOG
//   tools write them (lookups binary-search the table).
// The layout matches the checked-in TestDir/test.hog.
inline std::vector<uint8_t> BuildHog(std::vector<HogEntry> entries) {
  constexpr size_t kHeaderSize = 68;
  constexpr size_t kRecordSize = 48;
  constexpr size_t kNameSize = 36;

  auto lower = [](std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return s;
  };
  std::sort(entries.begin(), entries.end(),
            [&](const HogEntry &a, const HogEntry &b) { return lower(a.name) < lower(b.name); });

  d3_test::ByteWriter out;
  out.Text("HOG2");
  out.U32(static_cast<uint32_t>(entries.size()));
  out.U32(static_cast<uint32_t>(kHeaderSize + kRecordSize * entries.size()));
  out.Fill(kHeaderSize - out.size(), 0xff);
  for (const auto &e : entries) {
    out.FixedText(e.name, kNameSize);
    out.U32(0);                                    // flags
    out.U32(static_cast<uint32_t>(e.data.size())); // length
    out.U32(0);                                    // timestamp
  }
  for (const auto &e : entries)
    out.Text(e.data);
  return out.bytes();
}

class CFileFixture : public ::testing::Test {
protected:
  void SetUp() override {
    saved_base_directories_ = Base_directories;
    cf_ClearBaseDirectories();
    cf_ClearAllSearchPaths();
    cf_AddBaseDirectory(dir_.path());
  }

  void TearDown() override {
    for (int handle : open_libraries_)
      cf_CloseLibrary(handle);
    cf_ClearAllSearchPaths();
    Base_directories = saved_base_directories_;
  }

  int OpenHog(const std::string &hog_name, const std::vector<HogEntry> &entries) {
    dir_.Write(hog_name, BuildHog(entries));
    int handle = cf_OpenLibrary(hog_name);
    if (handle != 0)
      open_libraries_.push_back(handle);
    return handle;
  }

  void CloseHog(int handle) {
    cf_CloseLibrary(handle);
    open_libraries_.erase(std::remove(open_libraries_.begin(), open_libraries_.end(), handle), open_libraries_.end());
  }

  d3_test::TempDir dir_;

private:
  std::vector<std::filesystem::path> saved_base_directories_;
  std::vector<int> open_libraries_;
};

inline void ExpectSeekBehaviourOnAToJ(CFILE *cf) {
  ASSERT_NE(cf, nullptr);
  ASSERT_EQ(cfilelength(cf), 10u);

  EXPECT_EQ(cftell(cf), 0);
  EXPECT_EQ(cfseek(cf, 2, SEEK_SET), 0);
  EXPECT_EQ(cftell(cf), 2);
  EXPECT_EQ(cfgetc(cf), 'C');
  EXPECT_EQ(cftell(cf), 3);

  EXPECT_EQ(cfseek(cf, 3, SEEK_CUR), 0);
  EXPECT_EQ(cftell(cf), 6);
  EXPECT_EQ(cfgetc(cf), 'G');

  EXPECT_EQ(cfseek(cf, -1, SEEK_END), 0);
  EXPECT_EQ(cftell(cf), 9);
  EXPECT_FALSE(cfeof(cf));
  EXPECT_EQ(cfgetc(cf), 'J');
  EXPECT_TRUE(cfeof(cf));
  EXPECT_EQ(cfgetc(cf), EOF);

  cf_Rewind(cf);
  EXPECT_EQ(cftell(cf), 0);
  EXPECT_FALSE(cfeof(cf));
  EXPECT_EQ(cfgetc(cf), 'A');
}

} // namespace cfile_test

#endif
