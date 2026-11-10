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

#include "osf_writer.h"

#include <cstring>

#include <gtest/gtest.h>

#include "streamaudio.h"

namespace {

// Title buffer padded to the full title field, so the writer never reads past the caller's string.
struct Title {
  char text[OSF_HDR_TITLE_LEN] = {};
  explicit Title(const char *s) { std::strncpy(text, s, sizeof(text) - 1); }
};

} // namespace

void WriteOsf(const std::filesystem::path &path, const std::vector<uint8_t> &data, uint8_t comp, uint8_t format,
              uint32_t rate, uint32_t measure, const char *title) {
  OSFArchive osf;
  ASSERT_TRUE(osf.Open(path, true));
  size_t half = data.size() / 2;
  ASSERT_TRUE(osf.WriteBlock(const_cast<uint8_t *>(data.data()), static_cast<int>(half)));
  ASSERT_TRUE(osf.WriteBlock(const_cast<uint8_t *>(data.data()) + half, static_cast<int>(data.size() - half)));
  tOSFDigiHdr hdr;
  hdr.measure = measure;
  Title t(title);
  ASSERT_TRUE(osf.SaveHeader(OSF_DIGITAL_STRM, comp, format, rate, static_cast<uint32_t>(data.size()), &hdr, t.text));
  osf.Close();
}
