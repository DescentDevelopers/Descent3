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

#include "hogmaker_test_support.h"

#include <cctype>
#include <cstdio>
#include <cstdlib>

#include <gtest/gtest.h>

#include "cfile.h"

#ifndef _WIN32
#include <sys/wait.h>
#endif

namespace hogmaker_test {

bool CaseInsensitiveLess(const char *a, const char *b) {
  for (;; a++, b++) {
    int ca = std::tolower(static_cast<unsigned char>(*a));
    int cb = std::tolower(static_cast<unsigned char>(*b));
    if (ca != cb || ca == 0)
      return ca < cb;
  }
}

bool ReadRawHog(const std::filesystem::path &path, RawHog &out) {
  FILE *fp = fopen(path.u8string().c_str(), "rb");
  if (!fp)
    return false;
  bool ok = ReadHogHeader(fp, &out.header) && fseek(fp, HOG_HDR_SIZE, SEEK_SET) == 0;
  for (uint32_t i = 0; ok && i < out.header.nfiles; i++) {
    tHogFileEntry entry{};
    ok = ReadHogEntry(fp, &entry);
    if (ok)
      out.entries.push_back(entry);
  }
  fclose(fp);
  return ok;
}

int RunHogMaker(const std::vector<std::filesystem::path> &args, const std::filesystem::path &log) {
  std::string cmd = "\"" + std::string(HOGMAKER_EXE) + "\"";
  for (const auto &arg : args)
    cmd += " \"" + arg.u8string() + "\"";
  cmd += " > \"" + log.u8string() + "\" 2>&1";
#ifdef _WIN32
  // cmd.exe strips the outermost pair of quotes when the line has more than two.
  cmd = "\"" + cmd + "\"";
#endif
  int rc = std::system(cmd.c_str());
#ifndef _WIN32
  rc = WIFEXITED(rc) ? WEXITSTATUS(rc) : -1;
#endif
  return rc;
}

std::map<std::string, std::string> ReadThroughGameLoader(const std::filesystem::path &hog,
                                                         const std::vector<std::string> &names) {
  std::map<std::string, std::string> result;
  // cf_OpenLibrary ASSERTs on unsorted tables (an SDL assertion dialog in
  // debug builds), so report that as a test failure instead of loading.
  RawHog raw;
  EXPECT_TRUE(ReadRawHog(hog, raw)) << hog;
  for (size_t i = 1; i < raw.entries.size(); i++) {
    if (CaseInsensitiveLess(raw.entries[i].name, raw.entries[i - 1].name)) {
      ADD_FAILURE() << "file table not in case-insensitive order: " << raw.entries[i - 1].name << " before "
                    << raw.entries[i].name;
      return result;
    }
  }
  cf_ClearBaseDirectories();
  cf_AddBaseDirectory(hog.parent_path());
  int handle = cf_OpenLibrary(hog.filename());
  EXPECT_NE(handle, 0) << "cf_OpenLibrary failed for " << hog;
  if (handle == 0)
    return result;
  for (const auto &name : names) {
    CFILE *cfp = cf_OpenFileInLibrary(name, handle);
    if (!cfp)
      continue;
    std::string data(cfilelength(cfp), '\0');
    if (!data.empty())
      cf_ReadBytes(reinterpret_cast<uint8_t *>(data.data()), static_cast<int>(data.size()), cfp);
    cfclose(cfp);
    result[name] = data;
  }
  cf_CloseLibrary(handle);
  return result;
}

std::string AllByteValues() {
  std::string s;
  for (int i = 0; i < 256; i++)
    s.push_back(static_cast<char>(i));
  return s;
}

} // namespace hogmaker_test
