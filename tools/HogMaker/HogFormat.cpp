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

#include <algorithm>
#include <fstream>
#include <iterator>

#include "HogFormat.h"
#include "IOOps.h"

namespace D3 {

// TODO: To our descendants from the future: remove it when code will support C++20
template <typename TP> std::time_t to_time_t(TP tp) {
  auto sctp = std::chrono::duration_cast<std::chrono::system_clock::duration>(tp - TP::clock::now()) +
              std::chrono::system_clock::now();
  return std::chrono::system_clock::to_time_t(sctp);
}

HogFileEntry::HogFileEntry(const std::filesystem::path &input) {
  m_real_path = input;
  m_flags = 0;
  m_len = (uint32_t)file_size(input);
  m_timestamp = 0;
  auto name = input.filename().u8string();
  std::copy(name.begin(), name.end(), m_name.begin());
}

HogFileEntry::HogFileEntry(std::string name, uint32_t flags, uint32_t len, uint32_t timestamp) {
  std::copy(name.begin(), name.end(), m_name.begin());
  m_flags = flags;
  m_len = len;
  m_timestamp = timestamp;
}

void HogFileEntry::SetRealPath(const std::filesystem::path &input) {
  m_real_path = input;
  m_flags = 0;
  m_len = (uint32_t)file_size(input);
  m_timestamp = (uint32_t)to_time_t(last_write_time(input));
  auto name = input.filename().u8string();
  std::copy(name.begin(), name.end(), m_name.begin());
}

std::array<char, 36> char_tolower(std::array<char, 36> s) {
  std::transform(s.begin(), s.end(), s.begin(), [](uint8_t c) { return std::tolower(c); });
  return s;
}

std::ostream &operator<<(std::ostream &output, const HogHeader &header) {
  std::copy(header.tag.begin(), header.tag.end(), std::ostream_iterator<char>(output));
  bin_write(output, header.nfiles);
  bin_write(output, header.file_data_offset);
  std::copy(header.reserved.begin(), header.reserved.end(), std::ostream_iterator<char>(output));

  return output;
}

std::ostream &operator<<(std::ostream &output, const HogFileEntry &entry) {
  std::copy(entry.m_name.begin(), entry.m_name.end(), std::ostream_iterator<char>(output));
  bin_write(output, entry.m_flags);
  bin_write(output, entry.m_len);
  bin_write(output, entry.m_timestamp);

  return output;
}

std::ostream &operator<<(std::ostream &output, const HogFormat &format) {
  output << format.m_header;
  for (const HogFileEntry &entry : format.m_file_entries) {
    output << entry;
  }

  return output;
}

void HogFormat::SortEntries() {
  auto customLess = [](const HogFileEntry &a, const HogFileEntry &b) {
    return (char_tolower(a.m_name) < char_tolower(b.m_name));
  };

  std::sort(m_file_entries.begin(), m_file_entries.end(), customLess);
}

} // namespace D3
