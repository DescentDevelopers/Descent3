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

#include <filesystem>
#include <set>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "cfile.h"
#include "d3_test/files.h"
#include "support/cfile_test_utils.h"

using cfile_test::BuildHog;
using cfile_test::CFileFixture;
using cfile_test::HogEntry;
using cfile_test::ReadRest;
using cfile_test::Upper;

namespace {

// Enough entries that lookups exercise several levels of the binary search.
const std::vector<HogEntry> kSevenFiles = {
    {"alpha.txt", "first file"},   {"bravo.dat", std::string("\0\x01\x02\xff", 4)},
    {"charlie.txt", ""},           {"delta.ogf", "delta contents"},
    {"echo.txt", "echo"},          {"foxtrot.dat", "foxtrot!"},
    {"golf.txt", "last file in the hog"},
};

} // namespace

class CFileHogTest : public CFileFixture {};

TEST_F(CFileHogTest, EveryEntryIsFoundWithItsContents) {
  ASSERT_NE(OpenHog("test.hog", kSevenFiles), 0);

  for (const auto &entry : kSevenFiles) {
    SCOPED_TRACE(entry.name);
    CFILE *cf = cfopen(entry.name, "rb");
    ASSERT_NE(cf, nullptr);
    EXPECT_EQ(cfilelength(cf), entry.data.size());
    EXPECT_EQ(ReadRest(cf), entry.data);
    cfclose(cf);
  }
}

TEST_F(CFileHogTest, LookupIsCaseInsensitive) {
  ASSERT_NE(OpenHog("test.hog", kSevenFiles), 0);

  for (const auto &entry : kSevenFiles) {
    SCOPED_TRACE(entry.name);
    CFILE *cf = cfopen(Upper(entry.name), "rb");
    ASSERT_NE(cf, nullptr);
    EXPECT_EQ(ReadRest(cf), entry.data);
    cfclose(cf);
  }
}

TEST_F(CFileHogTest, NamesNotInTheHogAreNotFound) {
  ASSERT_NE(OpenHog("test.hog", kSevenFiles), 0);

  for (const char *name : {"aardvark.txt", "bravo.txt", "cobra.dat", "zulu.txt", "alpha"}) {
    SCOPED_TRACE(name);
    EXPECT_EQ(cfopen(name, "rb"), nullptr);
  }
}

TEST_F(CFileHogTest, SingleEntryHogLookup) {
  ASSERT_NE(OpenHog("one.hog", {{"only.txt", "solo"}}), 0);

  CFILE *cf = cfopen("only.txt", "rb");
  ASSERT_NE(cf, nullptr);
  EXPECT_EQ(ReadRest(cf), "solo");
  cfclose(cf);

  EXPECT_EQ(cfopen("before.txt", "rb"), nullptr);
  EXPECT_EQ(cfopen("zzz.txt", "rb"), nullptr);
}

TEST_F(CFileHogTest, ReadsStopAtTheEndOfTheEntry) {
  ASSERT_NE(OpenHog("test.hog", {{"a.bin", "0123456789"}, {"b.bin", "NEXTFILE"}}), 0);

  CFILE *cf = cfopen("a.bin", "rb");
  ASSERT_NE(cf, nullptr);
  char buf[64];
  // Binary cf_ReadString reads up to a NUL or EOF; there is no NUL in a.bin.
  EXPECT_EQ(cf_ReadString(buf, sizeof(buf), cf), 10);
  EXPECT_STREQ(buf, "0123456789");
  EXPECT_TRUE(cfeof(cf));
  EXPECT_EQ(cfgetc(cf), EOF);
  uint8_t byte;
  EXPECT_EQ(cf_ReadBytes(&byte, 1, cf), 0);
  cfclose(cf);
}

TEST_F(CFileHogTest, SeekTellAndRewindAreRelativeToTheEntry) {
  ASSERT_NE(OpenHog("test.hog", {{"a.bin", "0123456789"}, {"b.bin", "ABCDEFGHIJ"}, {"c.bin", "abcdefghij"}}), 0);

  CFILE *cf = cfopen("b.bin", "rb");
  cfile_test::ExpectSeekBehaviourOnAToJ(cf);
  if (cf)
    cfclose(cf);
}

TEST_F(CFileHogTest, NumericReadsFromAnEntry) {
  // 0x11223344 little-endian, then -2 as a little-endian int16.
  ASSERT_NE(OpenHog("test.hog", {{"pad.bin", "xyz"}, {"num.bin", std::string("\x44\x33\x22\x11\xfe\xff", 6)}}), 0);

  CFILE *cf = cfopen("num.bin", "rb");
  ASSERT_NE(cf, nullptr);
  EXPECT_EQ(cf_ReadInt(cf), 0x11223344);
  EXPECT_EQ(cf_ReadShort(cf), -2);
  EXPECT_TRUE(cfeof(cf));
  cfclose(cf);
}

TEST_F(CFileHogTest, TextModeEntryNormalisesLineEndings) {
  ASSERT_NE(OpenHog("test.hog", {{"lines.txt", "one\r\ntwo\nthree"}, {"z.bin", "\nZZZ"}}), 0);

  CFILE *cf = cfopen("lines.txt", "rt");
  ASSERT_NE(cf, nullptr);
  char buf[32];
  for (const char *line : {"one", "two", "three"}) {
    cf_ReadString(buf, sizeof(buf), cf);
    EXPECT_STREQ(buf, line);
  }
  EXPECT_TRUE(cfeof(cf));
  cfclose(cf);
}

TEST_F(CFileHogTest, ExistReportsLibraryFiles) {
  ASSERT_NE(OpenHog("test.hog", kSevenFiles), 0);
  dir_.Write("disk.txt", "x");

  EXPECT_EQ(cfexist("delta.ogf"), CFES_IN_LIBRARY);
  EXPECT_EQ(cfexist(dir_ / "disk.txt"), CFES_ON_DISK);
}

TEST_F(CFileHogTest, CrcOfEntryMatchesStandardCrc32) {
  ASSERT_NE(OpenHog("test.hog", {{"a.txt", "padding"}, {"check.txt", "123456789"}}), 0);

  CFILE *cf = cfopen("check.txt", "rb");
  ASSERT_NE(cf, nullptr);
  EXPECT_EQ(cf_CalculateFileCRC(cf), 0xCBF43926u); // standard CRC-32 check value
  cfclose(cf);
  EXPECT_EQ(cf_GetfileCRC("check.txt"), 0xCBF43926u);
}

TEST_F(CFileHogTest, CopyFileExtractsAnEntry) {
  ASSERT_NE(OpenHog("test.hog", kSevenFiles), 0);

  const auto dst = dir_ / "extracted.dat";
  EXPECT_TRUE(cf_CopyFile(dst, "bravo.dat"));
  EXPECT_EQ(d3_test::ReadFileText(dst), std::string("\0\x01\x02\xff", 4));
}

TEST_F(CFileHogTest, MostRecentlyOpenedLibraryWins) {
  int first = OpenHog("first.hog", {{"shared.txt", "from first"}, {"only_first.txt", "1"}});
  int second = OpenHog("second.hog", {{"shared.txt", "from second"}});
  ASSERT_NE(first, 0);
  ASSERT_NE(second, 0);
  EXPECT_NE(first, second);

  CFILE *cf = cfopen("shared.txt", "rb");
  ASSERT_NE(cf, nullptr);
  EXPECT_EQ(ReadRest(cf), "from second");
  cfclose(cf);

  cf = cfopen("only_first.txt", "rb");
  ASSERT_NE(cf, nullptr);
  EXPECT_EQ(ReadRest(cf), "1");
  cfclose(cf);

  CloseHog(second);
  cf = cfopen("shared.txt", "rb");
  ASSERT_NE(cf, nullptr);
  EXPECT_EQ(ReadRest(cf), "from first");
  cfclose(cf);
}

TEST_F(CFileHogTest, OpenFileInLibraryUsesOnlyTheGivenLibrary) {
  int first = OpenHog("first.hog", {{"shared.txt", "from first"}, {"only_first.txt", "1"}});
  int second = OpenHog("second.hog", {{"shared.txt", "from second"}});
  ASSERT_NE(first, 0);
  ASSERT_NE(second, 0);

  CFILE *cf = cf_OpenFileInLibrary("shared.txt", first);
  ASSERT_NE(cf, nullptr);
  EXPECT_EQ(ReadRest(cf), "from first");
  cfclose(cf);

  EXPECT_EQ(cf_OpenFileInLibrary("only_first.txt", second), nullptr);
  EXPECT_EQ(cf_OpenFileInLibrary("shared.txt", 0), nullptr);
  EXPECT_EQ(cf_OpenFileInLibrary("shared.txt", first + second + 1000), nullptr);
}

// The library shares one FILE* between its open entries when it can.
TEST_F(CFileHogTest, InterleavedReadsFromTheSameLibrary) {
  ASSERT_NE(OpenHog("test.hog", {{"a.bin", "aaaa1111"}, {"b.bin", "bbbb2222"}}), 0);

  CFILE *a = cfopen("a.bin", "rb");
  CFILE *b = cfopen("b.bin", "rb");
  ASSERT_NE(a, nullptr);
  ASSERT_NE(b, nullptr);
  uint8_t buf[4];
  ASSERT_EQ(cf_ReadBytes(buf, 4, a), 4);
  EXPECT_EQ(std::string(buf, buf + 4), "aaaa");
  ASSERT_EQ(cf_ReadBytes(buf, 4, b), 4);
  EXPECT_EQ(std::string(buf, buf + 4), "bbbb");
  ASSERT_EQ(cf_ReadBytes(buf, 4, a), 4);
  EXPECT_EQ(std::string(buf, buf + 4), "1111");
  ASSERT_EQ(cf_ReadBytes(buf, 4, b), 4);
  EXPECT_EQ(std::string(buf, buf + 4), "2222");
  cfclose(a);
  cfclose(b);

  CFILE *again = cfopen("b.bin", "rb");
  ASSERT_NE(again, nullptr);
  EXPECT_EQ(ReadRest(again), "bbbb2222");
  cfclose(again);
}

TEST_F(CFileHogTest, ClosedLibraryIsNoLongerSearched) {
  int handle = OpenHog("test.hog", kSevenFiles);
  ASSERT_NE(handle, 0);
  CloseHog(handle);

  EXPECT_EQ(cfopen("alpha.txt", "rb"), nullptr);
  EXPECT_EQ(cf_OpenFileInLibrary("alpha.txt", handle), nullptr);
}

TEST_F(CFileHogTest, SearchPathOverridesLibrary) {
  ASSERT_NE(OpenHog("test.hog", kSevenFiles), 0);
  d3_test::TempDir override_dir;
  override_dir.Write("echo.txt", "overridden");
  ASSERT_TRUE(cf_SetSearchPath(override_dir.path()));

  CFILE *cf = cfopen("echo.txt", "rb");
  ASSERT_NE(cf, nullptr);
  EXPECT_EQ(ReadRest(cf), "overridden");
  cfclose(cf);
  EXPECT_EQ(cfexist("echo.txt"), CFES_ON_DISK);
}

TEST_F(CFileHogTest, ForeachFileFiltersByExtensionCaseInsensitively) {
  int handle = OpenHog("test.hog", kSevenFiles);
  ASSERT_NE(handle, 0);

  std::set<std::string> seen;
  int count = cf_DoForeachFileInLibrary(handle, ".TXT", [&](const std::filesystem::path &p) { seen.insert(p.u8string()); });
  EXPECT_EQ(count, 4);
  EXPECT_EQ(seen, (std::set<std::string>{"alpha.txt", "charlie.txt", "echo.txt", "golf.txt"}));

  EXPECT_EQ(cf_DoForeachFileInLibrary(handle, ".wav", [](const std::filesystem::path &) {}), 0);
  EXPECT_EQ(cf_DoForeachFileInLibrary(handle + 1000, ".txt", [](const std::filesystem::path &) {}), 0);
}

TEST_F(CFileHogTest, OpenLibraryRejectsMissingAndNonHogFiles) {
  EXPECT_EQ(cf_OpenLibrary("missing.hog"), 0);

  dir_.Write("bogus.hog", "this is not a hog file at all, just some text padding it out");
  EXPECT_EQ(cf_OpenLibrary("bogus.hog"), 0);

  dir_.Write("empty_file.hog", "");
  EXPECT_EQ(cf_OpenLibrary("empty_file.hog"), 0);
}

