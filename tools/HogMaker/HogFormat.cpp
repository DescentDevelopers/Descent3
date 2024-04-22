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

#include <algorithm>
#include <fstream>
#include <iterator>

namespace D3 {

std::ostream &operator<<(std::ostream &output, const HogHeader &header) {
  std::copy(header.tag.begin(), header.tag.end(), std::ostream_iterator<char>(output));
  bin_write(output, header.nfiles);
  bin_write(output, header.file_data_offset);
  std::copy(header.reserved.begin(), header.reserved.end(), std::ostream_iterator<char>(output));

  return output;
}

std::ostream &operator<<(std::ostream &output, const HogFileEntry &entry) {
  std::copy(entry.name.begin(), entry.name.end(), std::ostream_iterator<char>(output));
  bin_write(output, entry.flags);
  bin_write(output, entry.len);
  bin_write(output, entry.timestamp);
  return output;
}

std::ostream &operator<<(std::ostream &output, const HogFormat &format) {
  output << format.m_header;
  for (const HogFileEntry &entry : format.m_file_entries) {
    output << entry;
  }

  return output;
}

} // namespace D3
