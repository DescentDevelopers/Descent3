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
#include <iostream>
#include <algorithm>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <string>

#include "HogFormat.h"

// TODO: To our descendants from the future: remove it when code will support C++20
template <typename TP> std::time_t to_time_t(TP tp) {
  auto sctp = std::chrono::duration_cast<std::chrono::system_clock::duration>(tp - TP::clock::now()) +
              std::chrono::system_clock::now();
  return std::chrono::system_clock::to_time_t(sctp);
}

std::string str_tolower(std::string s) {
  std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return std::tolower(c); });
  return s;
}

struct {
  bool operator()(const std::filesystem::path &a, const std::filesystem::path &b) const {
    return (str_tolower(a.u8string()) < str_tolower(b.u8string()));
  }
} customLess;

int main(int argc, char *argv[]) {
  std::vector<std::filesystem::path> input_files;

  if (argc != 3) {
    std::cout << "Usage:\n"
              << "  " << argv[0] << " <hogfile.hog> <inputfile.txt>\n"
              << std::endl;
    return 1;
  }

  std::filesystem::path hog_filename{argv[1]};
  std::filesystem::path input_filename{argv[2]};
  if (!is_regular_file(input_filename)) {
    std::cout << input_filename << " is not a regular file!" << std::endl;
    return 1;
  }

  try {
    create_directories(absolute(hog_filename).parent_path());
    std::ifstream input(input_filename, std::ios_base::in);
    auto parent_input_path = input_filename.parent_path();
    for (std::string line; std::getline(input, line);) {
      if (!is_regular_file(parent_input_path / line)) {
        std::cout << "Warning! File " << line << " from " << input_filename << " does not exists! Skipping..."
                  << std::endl;
      } else if (std::filesystem::path(line).filename().string().size() > 36) {
        std::cout << "Warning! Length of name of file " << line << " is more than 36 symbols! Skipping..." << std::endl;
      } else {
        input_files.emplace_back(parent_input_path / line);
      }
    }

  } catch (std::exception &e) {
    std::cout << "Exception: " << e.what() << std::endl;
    return 1;
  }
  std::sort(input_files.begin(), input_files.end(), customLess);

  D3::HogFormat hog_table;

  for (const auto &i : input_files) {
    try {
      D3::HogFileEntry entry;
      entry.name = i.filename();
      entry.flags = 0;
      entry.len = (uint32_t)file_size(i);
      entry.timestamp = (uint32_t)to_time_t(last_write_time(i));

      hog_table.AddEntry(entry);

    } catch (std::exception &e) {
      std::cout << "Exception: " << e.what() << std::endl;
      return 1;
    }
  }

  std::cout << "Creating " << hog_filename << "..." << std::endl;
  std::fstream ofs(hog_filename, std::ios_base::out | std::ios_base::binary);

  ofs << hog_table;
  for (const auto &i : input_files) {
    std::cout << "Adding " << i << "... ";
    try {
      std::fstream ifs(i, std::ios_base::in | std::ios_base::binary);
      std::copy((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>(),
                std::ostreambuf_iterator<char>(ofs));
    } catch (std::exception &e) {
      std::cout << "Exception: " << e.what() << std::endl;
      return 1;
    }
    std::cout << "[ok]" << std::endl;
  }
  std::cout << "Done!" << std::endl;

  return 0;
}
