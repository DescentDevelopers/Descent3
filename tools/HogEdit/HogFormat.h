/*
 * Descent 3
 * Copyright (C) 2024 Descent Developers
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

// System
#include <cstdint>
#include <cstring>
#include <array>
#include <filesystem>
#include <list>
#include <string>
#include <algorithm>

namespace D3 {

/*
HOG FILE FORMAT v2.0

HOG_TAG_STR		[strlen()]
NFILES			[int32]
HDRINFO			[HOG_HDR_SIZE]
FILE_TABLE		[sizeof(FILE_ENTRY) * NFILES]
FILE 0			[filelen(FILE 0)]
FILE 1			[filelen(FILE 1)]
...
FILE NFILES-1		[filelen(NFILES -1)]
*/

template <std::size_t len> static inline std::string arr_to_str(const std::array<char, len> &arr) {
  return std::string(std::data(arr), strnlen(std::data(arr), std::size(arr)));
}

static inline std::string lowercase(std::string s) {
  std::transform(std::begin(s), std::end(s), std::begin(s), [](unsigned char c) { return std::tolower(c); });
  return s;
}

static inline std::array<char, 56> array_with(char val) {
  std::array<char, 56> arr;
  arr.fill(val);
  return arr;
}

struct /* [[gnu::packed]] */ HogHeader {
  std::array<char, 4> magic = {'H', 'O', 'G', '2'};          // "HOG2"
  uint32_t entry_count = 0;                                  // number of file entries in header
  uint32_t file_data_offset = sizeof(HogHeader);             // offset in file to filedata.
  std::array<char, 56> reserved = array_with(-1 /*'\xff'*/); // filled with 0xff
};
static_assert(sizeof(HogHeader) == 68, "HogHeader is the incorrect size. 68 bytes was expected.");

struct /* [[gnu::packed] ]*/ HogFileEntry {
  std::array<char, 36> name = {}; // file name (36 char max)
  uint32_t flags = 0;             // extra info
  uint32_t len = 0;               // length of file
  uint32_t timestamp = 0;         // time of file.
};
static_assert(sizeof(HogFileEntry) == 48, "HogFileEntry is the incorrect size. 48 bytes was expected.");

class HogFormat {
private:
  HogHeader m_header;
  std::list<HogFileEntry> m_file_entries;

public:
  using hog_iter = std::list<HogFileEntry>::const_iterator;
  hog_iter begin(void) const { return std::cbegin(m_file_entries); }
  hog_iter end(void) const { return std::cend(m_file_entries); }

  void AddEntry(HogFileEntry &entry) {
    m_file_entries.push_back(entry);
    m_header.entry_count++;
    m_header.file_data_offset += 48;

    m_file_entries.sort(
        [](HogFileEntry &a, HogFileEntry &b) { return lowercase(arr_to_str(a.name)) < lowercase(arr_to_str(b.name)); });
  }

  void RemoveEntry(hog_iter entry_iter) {
    m_file_entries.erase(entry_iter);
    m_header.entry_count--;
    m_header.file_data_offset -= 48;
  }

  std::size_t GetFileOffset(const hog_iter entry_iter) {
    std::size_t offset = m_header.file_data_offset;
    for (hog_iter pos = begin(); pos != entry_iter && pos != end(); pos++)
      offset += pos->len;

    return offset;
  }

  friend std::ostream &operator<<(std::ostream &output, const HogFormat &format);
  friend std::istream &operator>>(std::istream &input, HogFormat &format);
};

} // namespace D3
