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


#pragma once

#include <cstdint>
#include <filesystem>
#include <string>
#include <string_view>
#include <vector>

namespace d3_test {

// A new, empty directory under the system temporary directory, removed with everything in it on destruction. Its
// name includes the process id, because ctest runs the tests of one executable in parallel processes.
class TempDir {
public:
  TempDir();
  ~TempDir();
  TempDir(const TempDir &) = delete;
  TempDir &operator=(const TempDir &) = delete;

  const std::filesystem::path &path() const { return path_; }
  // The path of `name` inside the directory.
  std::filesystem::path operator/(const std::filesystem::path &name) const { return path_ / name; }

  // Writes `contents` to `name` inside the directory, creating any subdirectories, and returns the file's path.
  std::filesystem::path Write(const std::filesystem::path &name, std::string_view contents = {}) const;
  std::filesystem::path Write(const std::filesystem::path &name, const std::vector<uint8_t> &contents) const;

private:
  std::filesystem::path path_;
  // Whether the constructor created the directory, so the destructor never removes something it didn't make.
  bool created_ = false;
};

// The whole contents of a file; empty if it cannot be read.
std::vector<uint8_t> ReadFileBytes(const std::filesystem::path &path);
std::string ReadFileText(const std::filesystem::path &path);

// Replaces the file's contents. Returns false if it cannot be written.
bool WriteFile(const std::filesystem::path &path, std::string_view contents);
bool WriteFile(const std::filesystem::path &path, const std::vector<uint8_t> &contents);

} // namespace d3_test
