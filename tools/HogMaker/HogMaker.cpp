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

#include "d3_version.h"
#include "HogFormat.h"

// TODO: To our descendants from the future: remove it when code will support C++20
template <typename TP> std::time_t to_time_t(TP tp) {
  auto sctp = std::chrono::duration_cast<std::chrono::system_clock::duration>(tp - TP::clock::now()) +
              std::chrono::system_clock::now();
  return std::chrono::system_clock::to_time_t(sctp);
}

// Find requested file in search paths
std::filesystem::path resolve_path(const std::vector<std::filesystem::path> &search_path,
                                   const std::filesystem::path &file) {
  for (const auto &i : search_path) {
    if (is_regular_file((i / file))) {
      return (i / file);
    }
  }
  // Return empty path
  return std::filesystem::path{};
}

int main(int argc, char *argv[]) {
  std::vector<std::filesystem::path> input_files;

  if (argc < 3) {
    std::cout << "HogMaker v" << D3_MAJORVER << "." << D3_MINORVER << "." << D3_BUILD << "-g" << D3_GIT_HASH << "\n"
              << "Usage:\n"
              << "  " << argv[0] << " <hogfile.hog> <inputfile.txt> [search_path]\n"
              << std::endl;
    return 1;
  }

  std::filesystem::path hog_filename{argv[1]};
  std::filesystem::path input_filename{argv[2]};
  // Default search path
  std::vector<std::filesystem::path> search_paths{input_filename.parent_path()};
  // Additional search paths
  for (int i = 3; i < argc; i++) {
    if (is_directory(std::filesystem::path(argv[i]))) {
      search_paths.emplace_back(argv[i]);
    }
  }

  if (!is_regular_file(input_filename)) {
    std::cout << input_filename << " is not a regular file!" << std::endl;
    return 1;
  }

  D3::HogFormat hog_table;

  try {
    create_directories(absolute(hog_filename).parent_path());
    std::ifstream input(input_filename, std::ios_base::in);
    for (std::string line; std::getline(input, line);) {
      auto resolved_file = resolve_path(search_paths, line);
      if (resolved_file.empty()) {
        std::cout << "Warning! File " << line << " from " << input_filename << " not found! Skipping..." << std::endl;
      } else if (line.size() > 36) {
        std::cout << "Warning! Length of name of file " << line << " is more than 36 symbols! Skipping..." << std::endl;
      } else {
        hog_table.AddEntry(D3::HogFileEntry(resolved_file));
      }
    }

  } catch (std::exception &e) {
    std::cout << "Exception: " << e.what() << std::endl;
    return 1;
  }

  hog_table.SortEntries();

  std::cout << "Creating " << absolute(hog_filename) << "..." << std::endl;
  std::fstream ofs(hog_filename, std::ios_base::out | std::ios_base::binary);

  ofs << hog_table;
  for (const auto& i : hog_table.GetEntries()) {
    std::cout << "Adding " << i.GetName() << "... ";
    try {
      std::fstream ifs(i.GetRealPath(), std::ios_base::in | std::ios_base::binary);
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
