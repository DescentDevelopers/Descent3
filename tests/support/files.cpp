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


#include "d3_test/files.h"

#include <atomic>
#include <fstream>
#include <iterator>
#include <random>
#include <system_error>

#include <gtest/gtest.h>

#ifdef WIN32
#include <process.h>
#define getpid _getpid
#else
#include <unistd.h>
#endif

namespace d3_test {

TempDir::TempDir() {
  static std::atomic<unsigned> counter{0};
  const std::filesystem::path base = std::filesystem::temp_directory_path();
  std::random_device random;
  while (true) {
    path_ = base / ("d3test-" + std::to_string(getpid()) + "-" + std::to_string(counter++) + "-" +
                    std::to_string(random() % 100000));
    std::error_code ec;
    if (std::filesystem::create_directory(path_, ec)) {
      created_ = true;
      return;
    }
    if (ec) {
      ADD_FAILURE() << "Cannot create a temporary directory " << path_ << ": " << ec.message();
      return;
    }
  }
}

TempDir::~TempDir() {
  if (!created_)
    return;
  std::error_code ec;
  std::filesystem::remove_all(path_, ec);
}

std::filesystem::path TempDir::Write(const std::filesystem::path &name, std::string_view contents) const {
  const std::filesystem::path file = path_ / name;
  std::error_code ec;
  std::filesystem::create_directories(file.parent_path(), ec);
  EXPECT_TRUE(WriteFile(file, contents)) << "Cannot write " << file;
  return file;
}

std::filesystem::path TempDir::Write(const std::filesystem::path &name, const std::vector<uint8_t> &contents) const {
  return Write(name, std::string_view(reinterpret_cast<const char *>(contents.data()), contents.size()));
}

std::vector<uint8_t> ReadFileBytes(const std::filesystem::path &path) {
  std::ifstream in(path, std::ios::binary);
  return std::vector<uint8_t>(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
}

std::string ReadFileText(const std::filesystem::path &path) {
  std::ifstream in(path, std::ios::binary);
  return std::string(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
}

bool WriteFile(const std::filesystem::path &path, std::string_view contents) {
  std::ofstream out(path, std::ios::binary | std::ios::trunc);
  out.write(contents.data(), static_cast<std::streamsize>(contents.size()));
  return static_cast<bool>(out);
}

bool WriteFile(const std::filesystem::path &path, const std::vector<uint8_t> &contents) {
  return WriteFile(path, std::string_view(reinterpret_cast<const char *>(contents.data()), contents.size()));
}

} // namespace d3_test
