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

#pragma once

#include <cstdint>
#include <filesystem>
#include <memory>
#include <vector>

#include <gtest/gtest.h>

#include "cfile.h"
#include "manage_page_builders.h"

// Defined in manage.cpp, not in a header.
void mng_WriteNewUnknownPage(CFILE *outfile);

namespace manage_page_io {

// Writes `page` to a new file at `file` with `writer`.
template <typename Page>
void WritePageFile(const std::filesystem::path &file, void (*writer)(CFILE *, Page *), Page *page) {
  CFILE *out = cfopen(file, "wb");
  ASSERT_NE(out, nullptr);
  writer(out, page);
  cfclose(out);
}

// Writes `in` to `file` with `writer`, checks the page header, and reads the body back with `reader`. Returns
// nullptr if the file could not be written or opened.
template <typename Page>
std::unique_ptr<Page> RoundTrip(const std::filesystem::path &file, Page *in, void (*writer)(CFILE *, Page *),
                                int (*reader)(CFILE *, Page *), uint8_t expected_type) {
  WritePageFile(file, writer, in);
  if (::testing::Test::HasFatalFailure())
    return nullptr;
  const auto file_size = static_cast<int>(std::filesystem::file_size(file));

  CFILE *f = cfopen(file, "rb");
  EXPECT_NE(f, nullptr);
  if (!f)
    return nullptr;
  const uint8_t pagetype = static_cast<uint8_t>(cf_ReadByte(f));
  const int len = cf_ReadInt(f);
  EXPECT_EQ(pagetype, expected_type);
  EXPECT_EQ(len, file_size - 1) << "page length must cover the length field and the body";

  auto page = std::make_unique<Page>();
  EXPECT_EQ(reader(f, page.get()), 1);
  EXPECT_EQ(cftell(f), file_size) << "reader must consume exactly the page body";
  cfclose(f);
  return page;
}

// Writes every page of `s`, plus an unknown page, to `out` and returns the page types in the order written.
std::vector<uint8_t> WriteAllPages(CFILE *out, manage_page_builders::PageSet &s);

} // namespace manage_page_io
