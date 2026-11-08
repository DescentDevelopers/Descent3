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
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "unzip.h"
#include "d3_test/byte_writer.h"
#include "d3_test/files.h"
#include "support/zip_builder.h"
#include "support/zip_reader.h"

namespace {

class UnzipTest : public ::testing::Test {
protected:
  void TearDown() override {
    zip_.CloseZip(); // release the file handle so the directory can be removed on Windows
  }

  d3_test::TempDir dir_;
  ZIP zip_;
};

TEST_F(UnzipTest, OpenFailsForMissingFile) { EXPECT_FALSE(zip_.OpenZip((dir_ / "does_not_exist.zip").string().c_str())); }

TEST_F(UnzipTest, OpenFailsForEmptyFile) { EXPECT_FALSE(zip_.OpenZip(dir_.Write("empty.zip").string().c_str())); }

TEST_F(UnzipTest, OpenFailsForFileShorterThanEndRecord) {
  EXPECT_FALSE(zip_.OpenZip(dir_.Write("tiny.zip", ToBytes("PK\x05\x06")).string().c_str()));
}

TEST_F(UnzipTest, OpenFailsForNonZipData) {
  // Larger than the reader's initial tail window so the whole-file search path is exercised too.
  EXPECT_FALSE(zip_.OpenZip(dir_.Write("garbage.zip", TextBytes(5000)).string().c_str()));
}

TEST_F(UnzipTest, OpenFailsWhileAlreadyOpenAndSucceedsAgainAfterClose) {
  const std::string first = dir_.Write("first.zip", BuildZip({Stored("one.txt", ToBytes("1"))}).bytes).string();
  const std::string second = dir_.Write("second.zip", BuildZip({Stored("two.txt", ToBytes("22"))}).bytes).string();

  ASSERT_TRUE(zip_.OpenZip(first.c_str()));
  EXPECT_FALSE(zip_.OpenZip(second.c_str()));
  NextEntry(zip_, "one.txt");

  zip_.CloseZip();
  ASSERT_TRUE(zip_.OpenZip(second.c_str()));
  NextEntry(zip_, "two.txt");
}

TEST_F(UnzipTest, ArchiveCommentLongerThanInitialSearchWindowIsSkipped) {
  const Bytes data = ToBytes("payload after a long archive comment");
  const std::string comment(5000, 'c');
  OpenArchive(zip_, dir_, BuildZip({Stored("file.txt", data)}, comment));

  zipentry *ent = NextEntry(zip_, "file.txt");
  ASSERT_NE(ent, nullptr);
  int rc = 0;
  EXPECT_EQ(AsBytes(ReadIntoMemory(zip_, ent, &rc)), data);
  EXPECT_EQ(rc, 0);
}

TEST_F(UnzipTest, EnumeratesEntriesWithCentralDirectoryMetadata) {
  const Bytes a = ToBytes("alpha");
  const Bytes b = TextBytes(3000);
  OpenArchive(zip_, dir_, BuildZip({Stored("a.txt", a), Deflated("dir/sub/b.bin", b), Stored("empty", {})}));

  zipentry *ent = NextEntry(zip_, "a.txt");
  ASSERT_NE(ent, nullptr);
  EXPECT_EQ(ent->compression_method, kMethodStored);
  EXPECT_EQ(ent->uncompressed_size, a.size());
  EXPECT_EQ(ent->compressed_size, a.size());
  EXPECT_EQ(ent->crc32, Crc(a));

  ent = NextEntry(zip_, "dir/sub/b.bin");
  ASSERT_NE(ent, nullptr);
  EXPECT_EQ(ent->compression_method, kMethodDeflate);
  EXPECT_EQ(ent->uncompressed_size, b.size());
  EXPECT_EQ(ent->compressed_size, RawDeflate(b).size());
  EXPECT_EQ(ent->crc32, Crc(b));

  ent = NextEntry(zip_, "empty");
  ASSERT_NE(ent, nullptr);
  EXPECT_EQ(ent->uncompressed_size, 0u);

  EXPECT_EQ(zip_.ReadNextZipEntry(), nullptr);
  EXPECT_EQ(zip_.ReadNextZipEntry(), nullptr);
}

TEST_F(UnzipTest, RewindRestartsEnumerationFromFirstEntry) {
  OpenArchive(zip_, dir_, BuildZip({Stored("first", ToBytes("1")), Stored("second", ToBytes("2"))}));

  NextEntry(zip_, "first");
  NextEntry(zip_, "second");
  ASSERT_EQ(zip_.ReadNextZipEntry(), nullptr);

  zip_.Rewind();
  NextEntry(zip_, "first");
  zip_.Rewind();
  NextEntry(zip_, "first");
  NextEntry(zip_, "second");
}

TEST_F(UnzipTest, CentralExtraFieldsAndCommentsDoNotDisturbEnumeration) {
  Entry first = Stored("first.txt", ToBytes("first"));
  first.central_extra = Bytes(36, 0xEE);
  first.comment = "a per-file comment";
  Entry second = Stored("second.txt", ToBytes("second"));
  second.central_extra = Bytes(5, 0x11);
  OpenArchive(zip_, dir_, BuildZip({first, second, Stored("third.txt", ToBytes("third"))}));

  NextEntry(zip_, "first.txt");
  NextEntry(zip_, "second.txt");
  NextEntry(zip_, "third.txt");
  EXPECT_EQ(zip_.ReadNextZipEntry(), nullptr);
}

TEST_F(UnzipTest, EntryWhoseNameOverrunsCentralDirectoryIsRejected) {
  Archive ar = BuildZip({Stored("name.txt", ToBytes("x"))});
  // Patch the central directory's file-name-length field (offset 28 in the record) to run past its end.
  ar.bytes[ar.cd_offset + 28] = 0xFF;
  ar.bytes[ar.cd_offset + 29] = 0x00;
  OpenArchive(zip_, dir_, ar);

  EXPECT_EQ(zip_.ReadNextZipEntry(), nullptr);
}

TEST_F(UnzipTest, ReadFileReturnsStoredContents) {
  const Bytes data = RandomBytes(5000, 7);
  OpenArchive(zip_, dir_, BuildZip({Stored("padding.txt", ToBytes("leading entry")), Stored("data.bin", data)}));

  zip_.ReadNextZipEntry();
  zipentry *ent = NextEntry(zip_, "data.bin");
  ASSERT_NE(ent, nullptr);
  int rc = -100;
  EXPECT_EQ(AsBytes(ReadIntoMemory(zip_, ent, &rc)), data);
  EXPECT_EQ(rc, 0);
}

TEST_F(UnzipTest, ExtractFileWritesStoredContents) {
  const Bytes data = RandomBytes(5000, 8);
  OpenArchive(zip_, dir_, BuildZip({Stored("padding.txt", ToBytes("leading entry")), Stored("data.bin", data)}));

  zip_.ReadNextZipEntry();
  zipentry *ent = NextEntry(zip_, "data.bin");
  ASSERT_NE(ent, nullptr);
  const std::string out = (dir_ / "out.bin").string();
  EXPECT_EQ(zip_.ExtractFile(ent, out.c_str()), 0);
  EXPECT_EQ(d3_test::ReadFileBytes(out), data);
}

// Sizes straddle the reader's 1 KiB / 16 KiB internal chunk sizes. Random data deflates to a stream larger than
// the uncompressed data, spanning several input chunks.
struct DeflateCase {
  size_t size;
  bool random;
};

class UnzipDeflateRoundTrip : public UnzipTest, public ::testing::WithParamInterface<DeflateCase> {};

TEST_P(UnzipDeflateRoundTrip, ReadFileInflatesToOriginal) {
  const Bytes data = SampleBytes(GetParam().size, GetParam().random);
  OpenArchive(zip_, dir_, BuildZip({Stored("padding.txt", ToBytes("leading entry")), Deflated("data.bin", data)}));
  zip_.ReadNextZipEntry();
  zipentry *ent = NextEntry(zip_, "data.bin");
  ASSERT_NE(ent, nullptr);

  int rc = -100;
  const Bytes got = AsBytes(ReadIntoMemory(zip_, ent, &rc));
  EXPECT_EQ(rc, 0);
  EXPECT_TRUE(got == data);
}

TEST_P(UnzipDeflateRoundTrip, ExtractFileInflatesToOriginal) {
  const Bytes data = SampleBytes(GetParam().size, GetParam().random);
  OpenArchive(zip_, dir_, BuildZip({Stored("padding.txt", ToBytes("leading entry")), Deflated("data.bin", data)}));
  zip_.ReadNextZipEntry();
  zipentry *ent = NextEntry(zip_, "data.bin");
  ASSERT_NE(ent, nullptr);

  const std::string out = (dir_ / "out.bin").string();
  EXPECT_EQ(zip_.ExtractFile(ent, out.c_str()), 0);
  EXPECT_TRUE(d3_test::ReadFileBytes(out) == data);
}

INSTANTIATE_TEST_SUITE_P(Sizes, UnzipDeflateRoundTrip,
                         ::testing::Values(DeflateCase{1, false}, DeflateCase{1023, false}, DeflateCase{1024, false},
                                           DeflateCase{1025, false}, DeflateCase{4096, false},
                                           DeflateCase{100000, false}, DeflateCase{1, true},
                                           DeflateCase{1023, true}, DeflateCase{1024, true},
                                           DeflateCase{1025, true}, DeflateCase{16384, true},
                                           DeflateCase{40000, true}),
                         [](const ::testing::TestParamInfo<DeflateCase> &info) {
                           return std::string(info.param.random ? "Random" : "Text") +
                                  std::to_string(info.param.size);
                         });

TEST_F(UnzipTest, DataIsLocatedUsingLocalHeaderExtraFieldLength) {
  // The local header's extra field may differ from the central directory's (e.g. Info-ZIP timestamps or
  // alignment padding).
  const Bytes stored = ToBytes("stored contents");
  const Bytes deflated = TextBytes(2000);
  Entry s = Stored("s.txt", stored);
  s.local_extra = Bytes(28, 0xAB);
  Entry d = Deflated("d.txt", deflated);
  d.local_extra = Bytes(9, 0xCD);
  d.central_extra = Bytes(4, 0x01);
  OpenArchive(zip_, dir_, BuildZip({s, d}));

  zipentry *ent = NextEntry(zip_, "s.txt");
  ASSERT_NE(ent, nullptr);
  int rc = -100;
  EXPECT_EQ(AsBytes(ReadIntoMemory(zip_, ent, &rc)), stored);
  EXPECT_EQ(rc, 0);

  ent = NextEntry(zip_, "d.txt");
  ASSERT_NE(ent, nullptr);
  const std::string out = (dir_ / "d.out").string();
  EXPECT_EQ(zip_.ExtractFile(ent, out.c_str()), 0);
  EXPECT_EQ(d3_test::ReadFileBytes(out), deflated);
}

TEST_F(UnzipTest, EntriesWithDataDescriptorUseCentralDirectorySizes) {
  const Bytes stored = ToBytes("streamed stored");
  const Bytes deflated = TextBytes(3000);
  Entry s = Stored("s.txt", stored);
  s.data_descriptor = true;
  Entry d = Deflated("d.txt", deflated);
  d.data_descriptor = true;
  OpenArchive(zip_, dir_, BuildZip({s, d}));

  zipentry *ent = NextEntry(zip_, "s.txt");
  ASSERT_NE(ent, nullptr);
  int rc = -100;
  EXPECT_EQ(AsBytes(ReadIntoMemory(zip_, ent, &rc)), stored);
  EXPECT_EQ(rc, 0);

  ent = NextEntry(zip_, "d.txt");
  ASSERT_NE(ent, nullptr);
  EXPECT_EQ(AsBytes(ReadIntoMemory(zip_, ent, &rc)), deflated);
  EXPECT_EQ(rc, 0);
}

TEST_F(UnzipTest, ExtractFileOfEmptyStoredEntryCreatesEmptyFile) {
  OpenArchive(zip_, dir_, BuildZip({Stored("empty.txt", {})}));
  zipentry *ent = NextEntry(zip_, "empty.txt");
  ASSERT_NE(ent, nullptr);

  const std::string out = (dir_ / "empty.out").string();
  EXPECT_EQ(zip_.ExtractFile(ent, out.c_str()), 0);
  EXPECT_TRUE(std::filesystem::exists(out));
  EXPECT_EQ(std::filesystem::file_size(out), 0u);
}

TEST_F(UnzipTest, ReadFileOfEmptyDeflatedEntrySucceeds) {
  OpenArchive(zip_, dir_, BuildZip({Deflated("empty.txt", {})}));
  zipentry *ent = NextEntry(zip_, "empty.txt");
  ASSERT_NE(ent, nullptr);

  int rc = -100;
  ReadIntoMemory(zip_, ent, &rc);
  EXPECT_EQ(rc, 0);
}

TEST_F(UnzipTest, OperationsFailWhenNoArchiveIsOpen) {
  zipentry ent{};
  ent.compression_method = kMethodStored;
  char buf[4];
  const std::string out = (dir_ / "never.out").string();

  EXPECT_EQ(zip_.ReadNextZipEntry(), nullptr);
  EXPECT_EQ(zip_.ReadFile(&ent, buf), -1);
  EXPECT_EQ(zip_.ExtractFile(&ent, out.c_str()), -1);
  EXPECT_FALSE(std::filesystem::exists(out));

  OpenArchive(zip_, dir_, BuildZip({Stored("a", ToBytes("abc"))}));
  zipentry *real = zip_.ReadNextZipEntry();
  ASSERT_NE(real, nullptr);
  zipentry copy = *real;
  zip_.CloseZip();
  copy.name = nullptr; // owned by the (now closed) archive

  EXPECT_EQ(zip_.ReadNextZipEntry(), nullptr);
  EXPECT_EQ(zip_.ReadFile(&copy, buf), -1);
  EXPECT_EQ(zip_.ExtractFile(&copy, out.c_str()), -1);
}

TEST_F(UnzipTest, UnsupportedCompressionMethodIsReported) {
  Entry e = Stored("bzip2.bin", ToBytes("not really bzip2"));
  e.method = 12; // BZIP2
  OpenArchive(zip_, dir_, BuildZip({e}));
  zipentry *ent = NextEntry(zip_, "bzip2.bin");
  ASSERT_NE(ent, nullptr);

  int rc = 0;
  ReadIntoMemory(zip_, ent, &rc);
  EXPECT_EQ(rc, -7);
  EXPECT_EQ(zip_.ExtractFile(ent, (dir_ / "out.bin").string().c_str()), -7);
}

TEST_F(UnzipTest, StoredEntryWithMismatchedSizesIsReportedCorrupt) {
  Entry e = Stored("bad.bin", ToBytes("0123456789"));
  e.override_sizes = true;
  e.central_compressed_size = 10;
  e.central_uncompressed_size = 12;
  OpenArchive(zip_, dir_, BuildZip({e}));
  zipentry *ent = NextEntry(zip_, "bad.bin");
  ASSERT_NE(ent, nullptr);

  int rc = 0;
  ReadIntoMemory(zip_, ent, &rc);
  EXPECT_EQ(rc, -2);
  EXPECT_EQ(zip_.ExtractFile(ent, (dir_ / "out.bin").string().c_str()), -2);
}

TEST_F(UnzipTest, DeflatedEntryNeedingNewerVersionIsReported) {
  Entry e = Deflated("zip64.bin", TextBytes(100));
  e.version_needed = 45; // ZIP64 extensions; only version 2.0 (20) features are supported
  OpenArchive(zip_, dir_, BuildZip({e}));
  zipentry *ent = NextEntry(zip_, "zip64.bin");
  ASSERT_NE(ent, nullptr);

  int rc = 0;
  ReadIntoMemory(zip_, ent, &rc);
  EXPECT_EQ(rc, -3);
  EXPECT_EQ(zip_.ExtractFile(ent, (dir_ / "out.bin").string().c_str()), -3);
}

TEST_F(UnzipTest, CorruptDeflateStreamIsReportedAsInflateError) {
  Entry e = Deflated("bad.bin", TextBytes(500));
  // BFINAL=1 with the reserved block type 11: invalid per RFC 1951.
  e.payload = Bytes(64, 0xFF);
  e.payload_set = true;
  OpenArchive(zip_, dir_, BuildZip({e}));
  zipentry *ent = NextEntry(zip_, "bad.bin");
  ASSERT_NE(ent, nullptr);

  int rc = 0;
  ReadIntoMemory(zip_, ent, &rc);
  EXPECT_EQ(rc, -6);
  EXPECT_EQ(zip_.ExtractFile(ent, (dir_ / "out.bin").string().c_str()), -6);
}

TEST_F(UnzipTest, TruncatedDeflateStreamIsReportedAsInflateError) {
  const Bytes data = RandomBytes(3000, 5);
  Entry e = Deflated("short.bin", data);
  e.payload = RawDeflate(data);
  e.payload.resize(e.payload.size() / 2);
  e.payload_set = true;
  OpenArchive(zip_, dir_, BuildZip({e}));
  zipentry *ent = NextEntry(zip_, "short.bin");
  ASSERT_NE(ent, nullptr);

  int rc = 0;
  ReadIntoMemory(zip_, ent, &rc);
  EXPECT_EQ(rc, -6);
  EXPECT_EQ(zip_.ExtractFile(ent, (dir_ / "out.bin").string().c_str()), -6);
}

TEST_F(UnzipTest, LocalHeaderOffsetPastEndOfFileIsReportedCorrupt) {
  Entry s = Stored("s.bin", ToBytes("stored"));
  s.override_offset = true;
  s.central_local_offset = 0x00100000;
  Entry d = Deflated("d.bin", TextBytes(100));
  d.override_offset = true;
  d.central_local_offset = 0x00100000;
  OpenArchive(zip_, dir_, BuildZip({s, d}));

  for (const char *name : {"s.bin", "d.bin"}) {
    SCOPED_TRACE(name);
    zipentry *ent = NextEntry(zip_, name);
    ASSERT_NE(ent, nullptr);
    int rc = 0;
    ReadIntoMemory(zip_, ent, &rc);
    EXPECT_EQ(rc, -2);
    EXPECT_EQ(zip_.ExtractFile(ent, (dir_ / "out.bin").string().c_str()), -2);
  }
}

TEST_F(UnzipTest, ExtractFileReportsUnopenableOutput) {
  OpenArchive(zip_, dir_, BuildZip({Stored("a.txt", ToBytes("abc"))}));
  zipentry *ent = NextEntry(zip_, "a.txt");
  ASSERT_NE(ent, nullptr);

  EXPECT_EQ(zip_.ExtractFile(ent, (dir_ / "no_such_dir/out.txt").string().c_str()), -8);
}

TEST(CompareZipFileName, MatchesBaseNameOfArchiveEntryCaseInsensitively) {
  EXPECT_TRUE(CompareZipFileName("mission.mn3", "mission.mn3"));
  EXPECT_TRUE(CompareZipFileName("missions/sub/Mission.MN3", "mission.mn3"));
  EXPECT_TRUE(CompareZipFileName("MISSION.mn3", "Mission.Mn3"));
}

TEST(CompareZipFileName, RejectsDifferentOrPartialNames) {
  EXPECT_FALSE(CompareZipFileName("mission.mn3", "other.mn3"));
  EXPECT_FALSE(CompareZipFileName("mission.mn3", "mission.mn"));
  EXPECT_FALSE(CompareZipFileName("mission.mn", "mission.mn3"));
  EXPECT_FALSE(CompareZipFileName("dir/mission.mn3", "dir"));
  EXPECT_FALSE(CompareZipFileName("mission.mn3/", "mission.mn3"));
}

} // namespace
