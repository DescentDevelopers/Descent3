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

#ifndef DMFC_HOSTS_FILE_H
#define DMFC_HOSTS_FILE_H

#include <cstdint>
#include <filesystem>
#include <set>
#include <string>
#include <utility>

#include "d3_test/files.h"
#include "gamedll_header.h"

namespace dmfc_test {

// An (ip, mask) pair as ParseHostsFile stores it, both in network byte order.
using IpMask = std::pair<uint32_t, uint32_t>;

// The four octets a.b.c.d in network byte order.
uint32_t Octets(uint8_t a, uint8_t b, uint8_t c, uint8_t d);

// Parses a hosts file with ParseHostsFile and returns its entries, freeing the list it builds.
std::multiset<IpMask> ParseHostsPath(const std::filesystem::path &file);

// While alive, points DMFC's file and address hooks at the real cfile library and owns a temporary directory for
// hosts files. Restores the hooks and removes the directory on destruction.
class HostsFileSandbox {
public:
  HostsFileSandbox();
  ~HostsFileSandbox();
  HostsFileSandbox(const HostsFileSandbox &) = delete;
  HostsFileSandbox &operator=(const HostsFileSandbox &) = delete;

  const std::filesystem::path &dir() const { return m_dir.path(); }

  // Writes `contents` to a hosts file in the directory and parses it.
  std::multiset<IpMask> Parse(const std::string &contents) const;

private:
  d3_test::TempDir m_dir;
  OpenCFILE_fp m_saved_open;
  cfeof_fp m_saved_eof;
  cf_ReadString_fp m_saved_read;
  cfclose_fp m_saved_close;
  nw_GetHostAddressFromNumbers_fp m_saved_addr;
};

} // namespace dmfc_test

#endif
