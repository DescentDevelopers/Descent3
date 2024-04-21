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

#include "HogFormat.h"
#include "IOOps.h"

#include <fstream>
#include <cstring>

namespace D3 {

std::ostream &operator<<(std::ostream &output, HogHeader &header) {
  output.write((char *)&header.tag[0], 4);
  bin_write(output, header.nfiles);
  bin_write(output, header.file_data_offset);
  output.write((char *)&header.reserved[0], 56);

  return output;
}

std::ostream &operator<<(std::ostream &output, HogFileEntry &entry) {
  // Prevent buffer overflow
  char outBytes[36] = {0};
  auto tmp = entry.name.c_str();
  // FIXME: better conversion under Windows
  strcpy(outBytes, (const char *)&tmp[0]);
  output.write(outBytes, 36);
  bin_write(output, entry.flags);
  bin_write(output, entry.len);
  bin_write(output, entry.timestamp);
  return output;
}

std::ostream &operator<<(std::ostream &output, HogFormat &format) {
  output << format.m_header;
  for (auto i : format.m_file_entries) {
    output << i;
  }

  return output;
}

} // namespace D3
