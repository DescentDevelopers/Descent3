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

#include "aiscripts_test_support.h"

#include <cstdarg>

#include "cfile.h"
#include "osiris_import.h"

namespace {

void NoOpMprintf(int, const char *, ...) {}

void *OpenFile(const char *filename, const char *mode) { return cfopen(filename, mode); }
int ReadString(char *buf, size_t n, void *file) { return cf_ReadString(buf, n, static_cast<CFILE *>(file)); }
uint8_t IsEof(void *file) { return cfeof(static_cast<CFILE *>(file)) ? 1 : 0; }
void CloseFile(void *file) { cfclose(static_cast<CFILE *>(file)); }

} // namespace

namespace aiscripts_test {

void InstallEngineImports() {
  mprintf = NoOpMprintf;
  File_Open = OpenFile;
  File_ReadString = ReadString;
  File_eof = IsEof;
  File_Close = CloseFile;
}

} // namespace aiscripts_test
