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

#include "rtp_log.h"

#include <fstream>

#include <gtest/gtest.h>

std::vector<std::string> Split(const std::string &line, const char *delimiters) {
  std::vector<std::string> fields;
  size_t start = 0;
  while (true) {
    size_t end = line.find_first_of(delimiters, start);
    fields.push_back(line.substr(start, end - start));
    if (end == std::string::npos)
      break;
    start = end + 1;
  }
  return fields;
}

std::vector<std::string> ReadLogRows(const std::filesystem::path &dir, std::vector<std::string> &header) {
  const std::filesystem::path log_path = dir / "D3Performance.txt";
  std::vector<std::string> rows;
  std::ifstream in(log_path);
  EXPECT_TRUE(in.is_open()) << "no log written to " << log_path;
  std::string line;
  header.clear();
  if (std::getline(in, line))
    header = Split(line, ",");
  while (std::getline(in, line)) {
    if (!line.empty())
      rows.push_back(line);
  }
  return rows;
}

double Column(const std::vector<std::string> &header, const std::string &row, const std::string &name) {
  size_t index = 0;
  while (index < header.size() && header[index] != name)
    index++;
  if (index == header.size()) {
    ADD_FAILURE() << "no column named " << name;
    return 0;
  }
  std::vector<std::string> fields = Split(row, ",");
  if (index >= fields.size()) {
    ADD_FAILURE() << "row has no field for column " << name << ": " << row;
    return 0;
  }
  return std::stod(fields[index]);
}
