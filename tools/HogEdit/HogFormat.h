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

#include <array>
#include <cstdint>
#include <filesystem>
#include <vector>

#include "IOOps.h"

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

inline std::array<char, 56> array_with(char val) {
  std::array<char, 56> arr;
  arr.fill(val);
  return arr;
}

struct HogHeader {
  std::array<char, 4> tag = {'H', 'O', 'G', '2'}; // "HOG2"
  uint32_t nfiles = 0;                            // number of files in header
  uint32_t file_data_offset = 68;                 // offset in file to filedata.
  std::array<char, 56> reserved = array_with(-1); // filled with 0xff
};

struct HogFileEntry {
  std::array<char, 36> name = {}; // file name (36 char max)
  uint32_t flags = 0;              // extra info
  uint32_t len = 0;                // length of file
  uint32_t timestamp = 0;          // time of file.
};

class HogFormat {
private:
  HogHeader m_header;
  std::vector<HogFileEntry> m_file_entries;

public:
  void AddEntry(HogFileEntry &entry) {
    m_file_entries.push_back(entry);
    m_header.nfiles++;
    m_header.file_data_offset += 48;
  };

  friend std::ostream &operator<<(std::ostream &output, const HogFormat &format);
};

} // namespace D3
