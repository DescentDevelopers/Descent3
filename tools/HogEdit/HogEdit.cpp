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
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <algorithm>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <string>
#include <stdexcept>
#include <functional>
#include <iterator>
#include <list>

// Project
#include "version.h"
#include "HogFormat.h"
#include "string_hash.h"

// TODO: To our descendants from the future: remove it when code will support C++20
// NOTE: time_t is not of a defined size. recently it has been switched to a 64-bit value
template <typename TP> uint32_t file_time_to_uint32_t(TP tp) {
  auto sctp = std::chrono::duration_cast<std::chrono::system_clock::duration>(tp - TP::clock::now()) +
              std::chrono::system_clock::now();
  return std::chrono::system_clock::to_time_t(sctp);
}

D3::HogFormat::hog_iter find_file_entry(const D3::HogFormat &format, const std::string &name) {
  auto fn = [&name](const auto &hfe) { return D3::lowercase(name) == D3::lowercase(D3::arr_to_str(hfe.name)); };
  if (auto it = std::find_if(std::begin(format), std::end(format), fn); it != std::end(format))
    return it;
  return std::end(format);
}

std::list<std::filesystem::path> validate_files(const std::filesystem::path &base_path,
                                                std::list<std::filesystem::path> filepaths) {
  bool printed_base_dir = false;
  std::list<std::filesystem::path> valid_files;
  for (auto &filepath : filepaths) {
    if (!std::filesystem::exists(base_path / filepath)) {
      if (!printed_base_dir) {
        printed_base_dir = true;
        std::cout << "File paths relative to " << std::filesystem::absolute(base_path) << std::endl;
      }
      std::cout << "File " << filepath << " does not exist! Skipping... " << std::endl;
    } else {
      if (!std::filesystem::is_regular_file(base_path / filepath))
        throw std::runtime_error(filepath.string() + " is not a regular file.");

      if (filepath.string().size() > 36)
        throw std::runtime_error("Length of filepath \"" + filepath.string() + "\" exceeds 36 bytes.");

      valid_files.emplace_back(filepath);
    }
  }
  return valid_files;
}

namespace D3 {
std::filesystem::path archive_path; // path of hog archive
std::filesystem::path base_path;
std::list<std::filesystem::path> file_path_list;

void add_files_to_header(D3::HogFormat &format, const std::list<std::filesystem::path> &path_list) {
  for (const auto &i : path_list) {
    D3::HogFileEntry entry;
    auto name = i.u8string();
    std::copy(name.begin(), name.end(), entry.name.begin());
    entry.flags = 0;
    entry.len = static_cast<uint32_t>(std::filesystem::file_size(base_path / i));
    entry.timestamp = static_cast<uint32_t>(file_time_to_uint32_t(std::filesystem::last_write_time(base_path / i)));

    format.AddEntry(entry);
  }
}

void amend_hog(D3::HogFormat &old_header, D3::HogFormat &new_header) {
  std::ifstream old_hog;
  std::ofstream new_hog;
  auto tmp_archive = archive_path;
  tmp_archive += ".tmp";
  bool modify_mode = std::distance(std::begin(old_header), std::end(old_header));

  if (modify_mode) {
    old_hog = std::ifstream(archive_path, std::ios_base::in | std::ios_base::binary);
    new_hog = std::ofstream(tmp_archive, std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);
  } else
    new_hog = std::ofstream(archive_path, std::ios_base::out | std::ios_base::binary);

  new_hog << new_header;

  for (const D3::HogFileEntry &entry : new_header) {
    if (auto it = find_file_entry(old_header, arr_to_str(entry.name)); it != std::end(old_header)) {
      std::cout << "Copying " << arr_to_str(entry.name) << "... ";
      auto old_offset = old_header.GetFileOffset(it);
      old_hog.seekg(old_offset);
      auto length = it->len;
      std::copy_n(std::istreambuf_iterator<char>(old_hog), length, std::ostreambuf_iterator<char>(new_hog));
      std::cout << "[ok]" << std::endl;
    } else {
      std::cout << "Adding " << arr_to_str(entry.name) << "... ";
      auto p = base_path / arr_to_str(entry.name);
      std::ifstream input_file(base_path / arr_to_str(entry.name), std::ios_base::in | std::ios_base::binary);
      std::copy(std::istreambuf_iterator<char>(input_file), std::istreambuf_iterator<char>(),
                std::ostreambuf_iterator<char>(new_hog));
      std::cout << "[ok]" << std::endl;
    }
  }

  old_hog.close();
  new_hog.close();

  if (modify_mode)
    std::filesystem::rename(tmp_archive, archive_path);
}

void create(void) {
  file_path_list = validate_files(base_path, file_path_list);

  D3::HogFormat hog_header, dummy_header;
  add_files_to_header(hog_header, file_path_list);

  std::cout << "Creating " << archive_path << "..." << std::endl;
  std::filesystem::create_directories(std::filesystem::absolute(archive_path).parent_path());
  amend_hog(dummy_header, hog_header);
}

void add(void) {
  file_path_list = validate_files(base_path, file_path_list);

  std::ifstream ifs(archive_path, std::ios_base::in | std::ios_base::binary);
  D3::HogFormat old_header, new_header;
  ifs >> old_header;
  ifs.close();
  new_header = old_header;

  add_files_to_header(new_header, file_path_list);
  amend_hog(old_header, new_header);
}

void remove(void) {
  std::ifstream ifs(archive_path, std::ios_base::in | std::ios_base::binary);
  D3::HogFormat old_header, new_header;
  ifs >> old_header;
  ifs.close();
  new_header = old_header;

  for (auto &file_path : file_path_list)
    if (auto it = find_file_entry(new_header, file_path.string()); it != std::end(new_header))
      new_header.RemoveEntry(it);

  amend_hog(old_header, new_header);
}

void list(void) {
  D3::HogFormat hog_table;

  std::ifstream ifs(archive_path, std::ios_base::in | std::ios_base::binary);
  ifs >> hog_table;
  for (const D3::HogFileEntry &entry : hog_table) {
    std::copy(std::begin(entry.name), std::end(entry.name), std::ostream_iterator<char>(std::cout));
    std::cout << std::endl;
  }
}

void extract(void) {
  D3::HogFormat hog_header;
  std::ifstream ifs(archive_path, std::ios_base::in | std::ios_base::binary);
  ifs >> hog_header;

  auto extractor = [&hog_header, &ifs](auto it) {
    auto name = arr_to_str(it->name);
    std::cout << "Extracting " << name << std::endl;
    std::filesystem::create_directories(std::filesystem::absolute(base_path / name).parent_path());
    std::ofstream ofs(base_path / name, std::ios_base::out | std::ios_base::binary);

    ifs.seekg(hog_header.GetFileOffset(it));
    std::copy_n((std::istreambuf_iterator<char>(ifs)), it->len, std::ostreambuf_iterator<char>(ofs));
  };

  if (file_path_list.empty()) {
    for (auto it = std::begin(hog_header); it != std::end(hog_header); it++)
      extractor(it);
  } else {
    for (const auto &i : file_path_list) {
      if (auto it = find_file_entry(hog_header, i.string()); it != std::end(hog_header))
        extractor(it);
      else
        std::cout << "File " << i << " is not in the hog file. Skipping..." << std::endl;
    }
  }
}
} // namespace D3

static inline void get_hog_path(int &parsed_parguments, const int argc, char *const argv[]) {
  if (parsed_parguments++ < argc)
    D3::archive_path = argv[parsed_parguments];
  else
    throw std::invalid_argument("No hog file specified.");
}

static inline void get_base_path(int &parsed_parguments, const int argc, char *const argv[]) {
  if (parsed_parguments++ < argc)
    D3::base_path = argv[parsed_parguments];
  else
    throw std::invalid_argument("Missing directory argument");
}

static inline void get_file_path_list(int &parsed_parguments, const int argc, char *const argv[]) {
  if (parsed_parguments++ < argc) {
    std::filesystem::path txt_path = argv[parsed_parguments];
    if (D3::base_path.empty())
      D3::base_path = txt_path.parent_path();

    std::ifstream input(txt_path, std::ios_base::in);
    for (std::string line; std::getline(input, line);)
      D3::file_path_list.push_back(line);
  } else
    throw std::invalid_argument("Missing filename argument");
}

void print_usage(int exit_value) {
  std::cout << "HogEdit v" << HOGEDIT_MAJORVER << "." << HOGEDIT_MINORVER << "." << HOGEDIT_BUILD << "-g"
            << HOGEDIT_GIT_HASH << std::endl
            << R"(Usage:
  hogedit <command> hog_archive [options] [files]

  commands:
    --create      Create a new hog file that will contain the specified files.
                  * The directory option specifies the base directory of the specified files.
                  * Will overwrite an existing hog archive.

    --add         Add one or more files to an existing hog file.
                  * The directory option specifies the base directory of the specified files.

    --delete      Delete one or more files from a hog file.
                  * Filenames are those listed in the hog file.

    --list        Print a list of filenames of files contained in the hog file.

    --extract     Extract the specified list of files from the hog file.
                  * If no filenames are specified then all files will be extracted.
                  * The directory option specifies extraction location.

  options:
    --directory   Specifies the directory for the command to use.
                  * If not specified then it is implied to be the current directory.

    --files-from  Load the list of files from a text file instead the command line.
                  * Format must be one filepath per line.
                  * If the directory option is not specified then the filepaths will be relative
                    to the path of this file.
)";
  std::exit(exit_value);
}

int main(int argc, char *argv[]) {

  if (argc < 3)
    print_usage(EXIT_FAILURE);

  std::function<void(void)> hog_operation = nullptr;
  try {
    bool parsing_files = false;
    for (int parsed_parguments = 1; parsed_parguments < argc; parsed_parguments++) {
      switch (hash(argv[parsed_parguments], std::strlen(argv[parsed_parguments]))) {
      case "--create"_hash:
        hog_operation = D3::create;
        get_hog_path(parsed_parguments, argc, argv);
        break;
      case "--add"_hash:
        hog_operation = D3::add;
        get_hog_path(parsed_parguments, argc, argv);
        break;
      case "--delete"_hash:
        hog_operation = D3::remove;
        get_hog_path(parsed_parguments, argc, argv);
        break;
      case "--list"_hash:
        hog_operation = D3::list;
        get_hog_path(parsed_parguments, argc, argv);
        break;
      case "--extract"_hash:
        hog_operation = D3::extract;
        get_hog_path(parsed_parguments, argc, argv);
        break;
      case "--directory"_hash:
        get_base_path(parsed_parguments, argc, argv);
        parsing_files = false; // files may have already been specified
        break;
      case "--files-from"_hash:
        get_file_path_list(parsed_parguments, argc, argv);
        parsing_files = false; // files may have already been specified
        break;
      default:
        if (hog_operation == nullptr ||                      // if no hog_operation was specified
            (!parsing_files && !D3::file_path_list.empty())) // if files were already specified
          throw std::invalid_argument(argv[parsed_parguments]);

        parsing_files = true; // now specifying files
        D3::file_path_list.push_back(argv[parsed_parguments]);
        break;
      }
    }
  } catch (std::invalid_argument &e) {
    std::cerr << "Invalid program argument: " << e.what() << std::endl;
    print_usage(EXIT_FAILURE);
  }

  try {
    if (D3::base_path.empty())
      D3::base_path = std::filesystem::current_path();
    hog_operation();
    std::cout << "Done!" << std::endl;
  } catch (std::exception &e) {
    std::cerr << "Fatal: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
