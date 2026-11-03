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
#include <cstring>
#include <filesystem>
#include <limits>
#include <set>
#include <string>

#include <gtest/gtest.h>

#include "cfile.h"
#include "d3_test/files.h"
#include "support/cfile_test_utils.h"

using cfile_test::CFileFixture;
using cfile_test::DoubleBits;
using cfile_test::FloatBits;

class CFileIOTest : public CFileFixture {};

TEST_F(CFileIOTest, NumericValuesRoundTrip) {
  const auto file = dir_ / "numbers.bin";

  const int32_t ints[] = {0, -1, 0x12345678, std::numeric_limits<int32_t>::min(), std::numeric_limits<int32_t>::max()};
  const int16_t shorts[] = {0, -1, 0x1234, std::numeric_limits<int16_t>::min(), std::numeric_limits<int16_t>::max()};
  const int8_t bytes[] = {0, -1, 0x7f, -128};
  const float floats[] = {0.0f, -0.0f, 1.5f, -123.456f, 1e-30f, std::numeric_limits<float>::infinity()};
  const double doubles[] = {0.0, -2.75, 1e300, 3.141592653589793};

  CFILE *out = cfopen(file, "wb");
  ASSERT_NE(out, nullptr);
  for (auto v : ints)
    cf_WriteInt(out, v);
  for (auto v : shorts)
    cf_WriteShort(out, v);
  for (auto v : bytes)
    cf_WriteByte(out, v);
  for (auto v : floats)
    cf_WriteFloat(out, v);
  for (auto v : doubles)
    cf_WriteDouble(out, v);
  cfclose(out);

  CFILE *in = cfopen(file, "rb");
  ASSERT_NE(in, nullptr);
  for (auto v : ints)
    EXPECT_EQ(cf_ReadInt(in), v);
  for (auto v : shorts)
    EXPECT_EQ(cf_ReadShort(in), v);
  for (auto v : bytes)
    EXPECT_EQ(cf_ReadByte(in), v);
  // Serialization must be bit-exact (this also distinguishes 0.0 from -0.0).
  for (auto v : floats)
    EXPECT_EQ(FloatBits(cf_ReadFloat(in)), FloatBits(v));
  for (auto v : doubles)
    EXPECT_EQ(DoubleBits(cf_ReadDouble(in)), DoubleBits(v));
  EXPECT_TRUE(cfeof(in));
  cfclose(in);
}

TEST_F(CFileIOTest, NumericValuesAreStoredLittleEndian) {
  const auto file = dir_ / "layout.bin";
  CFILE *out = cfopen(file, "wb");
  ASSERT_NE(out, nullptr);
  cf_WriteInt(out, 0x11223344);
  cf_WriteShort(out, 0x5566);
  cf_WriteFloat(out, 1.0f); // IEEE-754 single: 0x3F800000
  cf_WriteDouble(out, 1.0); // IEEE-754 double: 0x3FF0000000000000
  cfclose(out);

  const std::string expected("\x44\x33\x22\x11"
                             "\x66\x55"
                             "\x00\x00\x80\x3f"
                             "\x00\x00\x00\x00\x00\x00\xf0\x3f",
                             18);
  EXPECT_EQ(d3_test::ReadFileText(file), expected);
}

TEST_F(CFileIOTest, BigEndianReadsDecodeMostSignificantByteFirst) {
  const auto file = dir_ / "be.bin";
  d3_test::WriteFile(file, std::string("\x11\x22\x33\x44\x80\x01", 6));

  CFILE *in = cfopen(file, "rb");
  ASSERT_NE(in, nullptr);
  EXPECT_EQ(cf_ReadInt(in, false), 0x11223344);
  EXPECT_EQ(cf_ReadShort(in, false), static_cast<int16_t>(0x8001));
  cfclose(in);
}

TEST_F(CFileIOTest, BinaryStringsRoundTripIncludingEmpty) {
  const auto file = dir_ / "strings.bin";
  CFILE *out = cfopen(file, "wb");
  ASSERT_NE(out, nullptr);
  // Return value counts the terminator.
  EXPECT_EQ(cf_WriteString(out, "hello"), 6);
  EXPECT_EQ(cf_WriteString(out, ""), 1);
  EXPECT_EQ(cf_WriteString(out, "world"), 6);
  cfclose(out);

  EXPECT_EQ(d3_test::ReadFileText(file), std::string("hello\0\0world\0", 13));

  CFILE *in = cfopen(file, "rb");
  ASSERT_NE(in, nullptr);
  char buf[32];
  EXPECT_EQ(cf_ReadString(buf, sizeof(buf), in), 5);
  EXPECT_STREQ(buf, "hello");
  EXPECT_EQ(cf_ReadString(buf, sizeof(buf), in), 0);
  EXPECT_STREQ(buf, "");
  EXPECT_EQ(cf_ReadString(buf, sizeof(buf), in), 5);
  EXPECT_STREQ(buf, "world");
  EXPECT_TRUE(cfeof(in));
  cfclose(in);
}

TEST_F(CFileIOTest, ReadStringTruncatesToBufferButConsumesWholeString) {
  const auto file = dir_ / "long.bin";
  d3_test::WriteFile(file, std::string("abcdefghij\0next\0", 16));

  CFILE *in = cfopen(file, "rb");
  ASSERT_NE(in, nullptr);
  char buf[16];
  std::memset(buf, 'X', sizeof(buf));
  EXPECT_EQ(cf_ReadString(buf, 4, in), 10);
  EXPECT_STREQ(buf, "abc");
  EXPECT_EQ(buf[4], 'X');

  EXPECT_EQ(cf_ReadString(buf, sizeof(buf), in), 4);
  EXPECT_STREQ(buf, "next");
  cfclose(in);
}

TEST_F(CFileIOTest, ReadStringAtEndOfFileReturnsEmptyWithoutThrowing) {
  const auto file = dir_ / "short.bin";
  d3_test::WriteFile(file, "tail");

  CFILE *in = cfopen(file, "rb");
  ASSERT_NE(in, nullptr);
  char buf[16];
  EXPECT_EQ(cf_ReadString(buf, sizeof(buf), in), 4);
  EXPECT_STREQ(buf, "tail");
  EXPECT_NO_THROW(EXPECT_EQ(cf_ReadString(buf, sizeof(buf), in), 0));
  EXPECT_STREQ(buf, "");
  cfclose(in);
}

TEST_F(CFileIOTest, TextModeNormalisesLineEndings) {
  const auto file = dir_ / "lines.txt";
  d3_test::WriteFile(file, "one\r\ntwo\rthree\nfour\r\n\r\nlast");

  CFILE *in = cfopen(file, "rt");
  ASSERT_NE(in, nullptr);
  char buf[32];
  const char *expected[] = {"one", "two", "three", "four", "", "last"};
  for (const char *line : expected) {
    EXPECT_EQ(cf_ReadString(buf, sizeof(buf), in), static_cast<int>(std::strlen(line)));
    EXPECT_STREQ(buf, line);
  }
  EXPECT_TRUE(cfeof(in));
  cfclose(in);
}

TEST_F(CFileIOTest, TextModeStringsRoundTrip) {
  const auto file = dir_ / "roundtrip.txt";
  CFILE *out = cfopen(file, "wt");
  ASSERT_NE(out, nullptr);
  cf_WriteString(out, "first line");
  cf_WriteString(out, "");
  cf_WriteString(out, "third line");
  cfclose(out);

  CFILE *in = cfopen(file, "rt");
  ASSERT_NE(in, nullptr);
  char buf[32];
  cf_ReadString(buf, sizeof(buf), in);
  EXPECT_STREQ(buf, "first line");
  cf_ReadString(buf, sizeof(buf), in);
  EXPECT_STREQ(buf, "");
  cf_ReadString(buf, sizeof(buf), in);
  EXPECT_STREQ(buf, "third line");
  EXPECT_TRUE(cfeof(in));
  cfclose(in);
}

TEST_F(CFileIOTest, SeekTellAndRewindOnDiskFile) {
  const auto file = dir_ / "atoj.bin";
  d3_test::WriteFile(file, "ABCDEFGHIJ");

  CFILE *in = cfopen(file, "rb");
  cfile_test::ExpectSeekBehaviourOnAToJ(in);
  if (in)
    cfclose(in);
}

TEST_F(CFileIOTest, ReadingAtEndOfFileReturnsZeroBytes) {
  const auto file = dir_ / "four.bin";
  d3_test::WriteFile(file, "1234");

  CFILE *in = cfopen(file, "rb");
  ASSERT_NE(in, nullptr);
  uint8_t buf[4];
  EXPECT_EQ(cf_ReadBytes(buf, 4, in), 4);
  EXPECT_TRUE(cfeof(in));
  EXPECT_EQ(cf_ReadBytes(buf, 1, in), 0);
  EXPECT_EQ(cftell(in), 4);
  cfclose(in);
}

TEST_F(CFileIOTest, TellTracksBytesWrittenInBinaryMode) {
  const auto file = dir_ / "written.bin";
  CFILE *out = cfopen(file, "wb");
  ASSERT_NE(out, nullptr);
  cf_WriteInt(out, 1);
  EXPECT_EQ(cftell(out), 4);
  cf_WriteShort(out, 2);
  EXPECT_EQ(cftell(out), 6);
  cf_WriteByte(out, 3);
  EXPECT_EQ(cftell(out), 7);
  cf_WriteString(out, "ab");
  EXPECT_EQ(cftell(out), 10);
  cf_WriteFloat(out, 4.0f);
  EXPECT_EQ(cftell(out), 14);
  cf_WriteDouble(out, 5.0);
  EXPECT_EQ(cftell(out), 22);
  const uint8_t raw[3] = {7, 8, 9};
  EXPECT_EQ(cf_WriteBytes(raw, 3, out), 3);
  EXPECT_EQ(cftell(out), 25);
  cfclose(out);

  EXPECT_EQ(std::filesystem::file_size(file), 25u);
}

TEST_F(CFileIOTest, CopyFileCopiesContentsAcrossChunkBoundaries) {
  // Larger than the internal copy buffer so more than one chunk is needed;
  // include NUL and high bytes.
  std::string contents;
  for (int i = 0; i < 12001; i++)
    contents.push_back(static_cast<char>(i % 256));
  const auto src = dir_ / "src.bin";
  const auto dst = dir_ / "dst.bin";
  d3_test::WriteFile(src, contents);

  EXPECT_TRUE(cf_CopyFile(dst, src));
  EXPECT_EQ(d3_test::ReadFileText(dst), contents);
}

TEST_F(CFileIOTest, CopyFileOfEmptyFileCreatesEmptyFile) {
  const auto src = dir_ / "empty.bin";
  const auto dst = dir_ / "empty_copy.bin";
  d3_test::WriteFile(src, "");

  EXPECT_TRUE(cf_CopyFile(dst, src));
  ASSERT_TRUE(std::filesystem::exists(dst));
  EXPECT_EQ(std::filesystem::file_size(dst), 0u);
}

TEST_F(CFileIOTest, CopyFileFailsForMissingSourceWithoutCreatingDest) {
  const auto dst = dir_ / "never.bin";
  EXPECT_FALSE(cf_CopyFile(dst, dir_ / "does-not-exist.bin"));
  EXPECT_FALSE(std::filesystem::exists(dst));
}

TEST_F(CFileIOTest, FileCrcMatchesStandardCrc32) {
  // The standard CRC-32 check value for "123456789" is 0xCBF43926.
  dir_.Write("check.txt", "123456789");
  EXPECT_EQ(cf_GetfileCRC(dir_ / "check.txt"), 0xCBF43926u);

  dir_.Write("empty.bin", "");
  EXPECT_EQ(cf_GetfileCRC(dir_ / "empty.bin"), 0u);

  // Spans several internal read buffers. Expected value from
  // Python: zlib.crc32(bytes(i % 251 for i in range(12000)))
  std::string big;
  for (int i = 0; i < 12000; i++)
    big.push_back(static_cast<char>(i % 251));
  dir_.Write("big.bin", big);
  EXPECT_EQ(cf_GetfileCRC(dir_ / "big.bin"), 0x9CCC6324u);
}

TEST_F(CFileIOTest, FileCrcOfMissingFileIsAllOnes) {
  EXPECT_EQ(cf_GetfileCRC(dir_ / "missing.bin"), 0xFFFFFFFFu);
}

TEST_F(CFileIOTest, ExistReportsDiskFilesAndMissingFiles) {
  dir_.Write("here.txt", "x");
  EXPECT_EQ(cfexist(dir_ / "here.txt"), CFES_ON_DISK);
  EXPECT_EQ(cfexist(dir_ / "not-here.txt"), CFES_NOT_FOUND);
}

TEST_F(CFileIOTest, LaterBaseDirectoriesTakePriority) {
  d3_test::TempDir low;
  d3_test::TempDir high;
  low.Write("shared.txt", "low");
  high.Write("shared.txt", "high");
  low.Write("only_low.txt", "low");

  cf_ClearBaseDirectories();
  cf_AddBaseDirectory(low.path());
  cf_AddBaseDirectory(high.path());

  std::error_code ec;
  EXPECT_TRUE(std::filesystem::equivalent(cf_LocatePath("shared.txt"), high.path() / "shared.txt", ec));
  EXPECT_TRUE(std::filesystem::equivalent(cf_LocatePath("only_low.txt"), low.path() / "only_low.txt", ec));
  EXPECT_TRUE(cf_LocatePath("nowhere.txt").empty());

  auto all = cf_LocateMultiplePaths("shared.txt");
  ASSERT_EQ(all.size(), 2u);
  std::set<std::string> contents = {d3_test::ReadFileText(all[0]), d3_test::ReadFileText(all[1])};
  EXPECT_EQ(contents, (std::set<std::string>{"low", "high"}));
}

TEST_F(CFileIOTest, AddBaseDirectoryIgnoresMissingDirectoriesAndFiles) {
  const auto before = Base_directories;
  dir_.Write("plain_file", "x");
  cf_AddBaseDirectory(dir_ / "no-such-dir");
  cf_AddBaseDirectory(dir_ / "plain_file");
  EXPECT_EQ(Base_directories, before);
}

TEST_F(CFileIOTest, OpenRelativePathWithDirectoryResolvesAgainstBaseDirectory) {
  std::filesystem::create_directory(dir_ / "sub");
  d3_test::WriteFile(dir_ / "sub" / "data.bin", "payload");

  CFILE *in = cfopen(std::filesystem::path("sub") / "data.bin", "rb");
  ASSERT_NE(in, nullptr);
  EXPECT_EQ(cfilelength(in), 7u);
  EXPECT_EQ(cfile_test::ReadRest(in), "payload");
  cfclose(in);

  EXPECT_EQ(cfopen(std::filesystem::path("sub") / "missing.bin", "rb"), nullptr);
}

TEST_F(CFileIOTest, ExtensionRestrictedSearchPathOnlyServesThoseExtensions) {
  d3_test::TempDir scripts;
  d3_test::TempDir general;
  scripts.Write("level.msg", "msg");
  scripts.Write("notes.txt", "hidden");
  general.Write("readme.txt", "general");

  EXPECT_FALSE(cf_SetSearchPath(dir_ / "no-such-dir"));
  ASSERT_TRUE(cf_SetSearchPath(scripts.path(), {".msg"}));
  ASSERT_TRUE(cf_SetSearchPath(general.path()));

  CFILE *msg = cfopen("level.msg", "rb");
  ASSERT_NE(msg, nullptr);
  EXPECT_EQ(cfile_test::ReadRest(msg), "msg");
  cfclose(msg);

  CFILE *readme = cfopen("readme.txt", "rb");
  ASSERT_NE(readme, nullptr);
  EXPECT_EQ(cfile_test::ReadRest(readme), "general");
  cfclose(readme);

  EXPECT_EQ(cfopen("notes.txt", "rb"), nullptr);

  cf_ClearAllSearchPaths();
  EXPECT_EQ(cfopen("readme.txt", "rb"), nullptr);
}
