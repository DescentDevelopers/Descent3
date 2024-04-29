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

// System
#include <algorithm>
#include <fstream>
#include <iterator>
#include <stdexcept>

// Project
#include "HogFormat.h"
#include "IOOps.h"
#include "byteswap.h"

namespace D3 {

std::ostream &operator<<(std::ostream &output, const HogHeader &header) {
  std::copy(std::begin(header.magic), std::end(header.magic), std::ostream_iterator<char>(output));
  bin_write(output, header.entry_count);
  bin_write(output, header.file_data_offset);
  bin_write(output, header.reserved);

  return output;
}

std::istream &operator>>(std::istream &input, HogHeader &header) {
  input.read(std::data(header.magic), std::size(header.magic));
  if (header.magic != std::array<char, 4>{'H', 'O', 'G', '2'})
    throw std::invalid_argument("HOG2 magic number not found. Not a hog file.");

  bin_read(input, header.entry_count);
  bin_read(input, header.file_data_offset);
  bin_read(input, header.reserved);

  return input;
}

std::ostream &operator<<(std::ostream &output, const HogFileEntry &entry) {
  std::copy(std::begin(entry.name), std::end(entry.name), std::ostream_iterator<char>(output));
  bin_write(output, entry.flags);
  bin_write(output, entry.len);
  bin_write(output, entry.timestamp);

  return output;
}

std::istream &operator>>(std::istream &input, HogFileEntry &entry) {
  input.read(std::data(entry.name), std::size(entry.name));
  bin_read(input, entry.flags);
  bin_read(input, entry.len);
  bin_read(input, entry.timestamp);

  return input;
}

std::ostream &operator<<(std::ostream &output, const HogFormat &format) {
  output << format.m_header;
  for (const HogFileEntry &entry : format.m_file_entries) {
    output << entry;
  }

  return output;
}

std::istream &operator>>(std::istream &input, HogFormat &format) {
  input >> format.m_header;
  format.m_file_entries.resize(format.m_header.entry_count);
  for (HogFileEntry &entry : format.m_file_entries) {
    input >> entry;
  }

  return input;
}

} // namespace D3
