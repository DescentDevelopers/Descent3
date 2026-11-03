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

#include "zip_reader.h"

#include <gtest/gtest.h>

void OpenArchive(ZIP &zip, const d3_test::TempDir &dir, const Archive &ar) {
  ASSERT_TRUE(zip.OpenZip(dir.Write("archive.zip", ar.bytes).string().c_str()));
}

zipentry *NextEntry(ZIP &zip, const std::string &expected_name) {
  zipentry *ent = zip.ReadNextZipEntry();
  EXPECT_NE(ent, nullptr) << "expected entry " << expected_name;
  if (ent)
    EXPECT_STREQ(ent->name, expected_name.c_str());
  return ent;
}

std::vector<char> ReadIntoMemory(ZIP &zip, zipentry *ent, int *result) {
  // One spare byte so a zero-length entry still has a valid, non-null buffer.
  std::vector<char> buf(ent->uncompressed_size + 1, '\x5a');
  *result = zip.ReadFile(ent, buf.data());
  buf.resize(ent->uncompressed_size);
  return buf;
}
