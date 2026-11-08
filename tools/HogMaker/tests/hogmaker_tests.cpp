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
#include <cstring>
#include <filesystem>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "cfile.h"
#include "d3_test/files.h"
#include "hogfile.h"
#include "HogFormat.h"
#include "support/hogmaker_test_support.h"

namespace fs = std::filesystem;

namespace {

using namespace hogmaker_test;

class HogMakerTest : public ::testing::Test {
protected:
  d3_test::TempDir tmp;

  void TearDown() override { cf_ClearBaseDirectories(); }
};

} // namespace

TEST_F(HogMakerTest, EmptyArchiveIsExactlyAHeaderTheGameCanParse) {
  D3::HogFormat hog;
  std::ostringstream ss;
  ss << hog;
  // The game seeks to HOG_HDR_SIZE to find the file table, so the header must
  // occupy exactly that many bytes.
  ASSERT_EQ(ss.str().size(), static_cast<size_t>(HOG_HDR_SIZE));

  fs::path path = tmp.Write("empty.hog", ss.str());
  RawHog raw;
  ASSERT_TRUE(ReadRawHog(path, raw));
  EXPECT_EQ(std::memcmp(raw.header.magic, HOG_TAG_STR, HOG_TAG_LEN), 0);
  EXPECT_EQ(raw.header.nfiles, 0u);
  EXPECT_EQ(raw.header.file_data_offset, static_cast<uint32_t>(HOG_HDR_SIZE));
}

TEST_F(HogMakerTest, FileTableRoundTripsThroughGameReader) {
  // Multi-byte values with distinct bytes catch field-order and endianness mistakes.
  D3::HogFormat hog;
  hog.AddEntry(D3::HogFileEntry("alpha.txt", 0x01020304u, 0x0A0B0C0Du, 0x11223344u));
  hog.AddEntry(D3::HogFileEntry("beta.dat", 0u, 0u, 0u));
  hog.AddEntry(D3::HogFileEntry("gamma.ogf", 0xFFFFFFFFu, 0x80000001u, 0x7FFFFFFFu));
  EXPECT_EQ(hog.Count(), 3u);

  std::ostringstream ss;
  ss << hog;
  fs::path path = tmp.Write("table.hog", ss.str());

  RawHog raw;
  ASSERT_TRUE(ReadRawHog(path, raw));
  EXPECT_EQ(std::memcmp(raw.header.magic, HOG_TAG_STR, HOG_TAG_LEN), 0);
  ASSERT_EQ(raw.header.nfiles, 3u);
  // File data is appended directly after the table, so the advertised data
  // offset must be exactly where the serialized header + table ends.
  EXPECT_EQ(raw.header.file_data_offset, ss.str().size());

  ASSERT_EQ(raw.entries.size(), 3u);
  EXPECT_STREQ(raw.entries[0].name, "alpha.txt");
  EXPECT_EQ(raw.entries[0].flags, 0x01020304u);
  EXPECT_EQ(raw.entries[0].len, 0x0A0B0C0Du);
  EXPECT_EQ(raw.entries[0].timestamp, 0x11223344u);
  EXPECT_STREQ(raw.entries[1].name, "beta.dat");
  EXPECT_EQ(raw.entries[1].flags, 0u);
  EXPECT_EQ(raw.entries[1].len, 0u);
  EXPECT_EQ(raw.entries[1].timestamp, 0u);
  EXPECT_STREQ(raw.entries[2].name, "gamma.ogf");
  EXPECT_EQ(raw.entries[2].flags, 0xFFFFFFFFu);
  EXPECT_EQ(raw.entries[2].len, 0x80000001u);
  EXPECT_EQ(raw.entries[2].timestamp, 0x7FFFFFFFu);
}

TEST_F(HogMakerTest, SortEntriesOrdersNamesCaseInsensitively) {
  // The game binary-searches the table with stricmp, so entries must be in
  // case-insensitive order. '.' (0x2E) < '_' (0x5F) < letters, and a name
  // sorts before any longer name it is a prefix of.
  D3::HogFormat hog;
  hog.AddEntry(D3::HogFileEntry("b.txt", 0, 1));
  hog.AddEntry(D3::HogFileEntry("C.txt", 0, 2));
  hog.AddEntry(D3::HogFileEntry("a_b.txt", 0, 3));
  hog.AddEntry(D3::HogFileEntry("A.txt", 0, 4));
  hog.AddEntry(D3::HogFileEntry("ab", 0, 5));
  hog.AddEntry(D3::HogFileEntry("AB.txt", 0, 6));
  hog.SortEntries();

  std::ostringstream ss;
  ss << hog;
  fs::path path = tmp.Write("sorted.hog", ss.str());
  RawHog raw;
  ASSERT_TRUE(ReadRawHog(path, raw));

  const std::vector<std::pair<std::string, uint32_t>> expected = {
      {"A.txt", 4}, {"a_b.txt", 3}, {"ab", 5}, {"AB.txt", 6}, {"b.txt", 1}, {"C.txt", 2},
  };
  ASSERT_EQ(raw.entries.size(), expected.size());
  for (size_t i = 0; i < expected.size(); i++) {
    EXPECT_EQ(std::string(raw.entries[i].name), expected[i].first) << "position " << i;
    EXPECT_EQ(raw.entries[i].len, expected[i].second) << "position " << i;
  }
}

TEST_F(HogMakerTest, BuildsArchiveThatTheGameLoaderCanReadBack) {
  // Contents chosen to expose text-mode I/O (CR/LF, NUL, 0x1A), zero-length
  // entries (offset arithmetic) and table/data misalignment (distinct sizes).
  const std::map<std::string, std::string> files = {
      {"Zeta.bin", AllByteValues()},
      {"alpha.txt", "line one\r\nline two\n\x1a trailing"},
      {"empty.txt", ""},
      {"Beta_2.dat", std::string(1000, 'B')},
      {"beta.dat", "b"},
      {"MIXED.Case", std::string("\0\0\0x", 4)},
  };
  fs::path src = tmp / "src";
  std::string list;
  for (const auto &[name, content] : files) {
    tmp.Write("src/" + name, content);
    list += name + "\n";
  }
  tmp.Write("src/list.txt", list);

  // Output directory does not exist yet: HogMaker is expected to create it.
  fs::path hog = tmp / "out" / "nested" / "test.hog";
  fs::path log = tmp / "log.txt";
  ASSERT_EQ(RunHogMaker({hog, src / "list.txt"}, log), 0) << d3_test::ReadFileText(log);
  ASSERT_TRUE(fs::is_regular_file(hog)) << d3_test::ReadFileText(log);

  RawHog raw;
  ASSERT_TRUE(ReadRawHog(hog, raw));
  EXPECT_EQ(raw.header.nfiles, files.size());

  std::vector<std::string> names;
  for (const auto &kv : files)
    names.push_back(kv.first);
  auto read_back = ReadThroughGameLoader(hog, names);
  for (const auto &[name, content] : files) {
    ASSERT_EQ(read_back.count(name), 1u) << name << " not found by the game loader";
    EXPECT_EQ(read_back.at(name), content) << "content mismatch for " << name;
  }

  size_t total = 0;
  for (const auto &kv : files)
    total += kv.second.size();
  EXPECT_EQ(fs::file_size(hog), raw.header.file_data_offset + total);
}

TEST_F(HogMakerTest, MissingListedFilesAreSkipped) {
  fs::path src = tmp / "src";
  tmp.Write("src/one.txt", "1");
  tmp.Write("src/two.txt", "22");
  tmp.Write("src/list.txt", "one.txt\nmissing.txt\ntwo.txt\n");

  fs::path hog = tmp / "test.hog";
  fs::path log = tmp / "log.txt";
  ASSERT_EQ(RunHogMaker({hog, src / "list.txt"}, log), 0) << d3_test::ReadFileText(log);

  RawHog raw;
  ASSERT_TRUE(ReadRawHog(hog, raw));
  EXPECT_EQ(raw.header.nfiles, 2u);
  auto read_back = ReadThroughGameLoader(hog, {"one.txt", "two.txt", "missing.txt"});
  EXPECT_EQ(read_back["one.txt"], "1");
  EXPECT_EQ(read_back["two.txt"], "22");
  EXPECT_EQ(read_back.count("missing.txt"), 0u);
}

TEST_F(HogMakerTest, SearchPathsAreConsultedInOrderAfterListDirectory) {
  fs::path src = tmp / "src";
  fs::path extra1 = tmp / "extra1";
  fs::path extra2 = tmp / "extra2";
  tmp.Write("src/shared.txt", "from list dir");
  tmp.Write("extra1/shared.txt", "from extra1");
  tmp.Write("extra1/second.txt", "from extra1");
  tmp.Write("extra2/second.txt", "from extra2");
  tmp.Write("extra2/third.txt", "from extra2");
  tmp.Write("src/list.txt", "second.txt\nshared.txt\nthird.txt\n");

  fs::path hog = tmp / "test.hog";
  fs::path log = tmp / "log.txt";
  ASSERT_EQ(RunHogMaker({hog, src / "list.txt", extra1, extra2}, log), 0) << d3_test::ReadFileText(log);

  auto read_back = ReadThroughGameLoader(hog, {"shared.txt", "second.txt", "third.txt"});
  EXPECT_EQ(read_back["shared.txt"], "from list dir");
  EXPECT_EQ(read_back["second.txt"], "from extra1");
  EXPECT_EQ(read_back["third.txt"], "from extra2");
}

TEST_F(HogMakerTest, OverlongFileNameBehindADirectoryIsRejected) {
  const std::string name36 = std::string(32, 'b') + ".txt";
  ASSERT_EQ(name36.size(), 36u);

  fs::path src = tmp / "src";
  tmp.Write("src/d/" + name36, "36");
  tmp.Write("src/d/ok.txt", "ok");
  tmp.Write("src/list.txt", "d/" + name36 + "\nd/ok.txt\n");

  fs::path hog = tmp / "test.hog";
  fs::path log = tmp / "log.txt";
  ASSERT_EQ(RunHogMaker({hog, src / "list.txt"}, log), 0) << d3_test::ReadFileText(log);

  RawHog raw;
  ASSERT_TRUE(ReadRawHog(hog, raw));
  ASSERT_EQ(raw.entries.size(), 1u);
  EXPECT_STREQ(raw.entries[0].name, "ok.txt");
}

TEST_F(HogMakerTest, FailsWhenListFileDoesNotExist) {
  fs::path hog = tmp / "test.hog";
  fs::path log = tmp / "log.txt";
  EXPECT_NE(RunHogMaker({hog, tmp / "no_such_list.txt"}, log), 0) << d3_test::ReadFileText(log);
  EXPECT_FALSE(fs::exists(hog));
}
