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

#include "hosts_file.h"

#include <cstdlib>
#include <cstring>

#include "cfile.h"
#include "dmfc_fakes.h"
#include "dmfc_internals.h"

namespace dmfc_test {

uint32_t Octets(uint8_t a, uint8_t b, uint8_t c, uint8_t d) {
  uint8_t bytes[4] = {a, b, c, d};
  uint32_t result;
  memcpy(&result, bytes, 4);
  return result;
}

std::multiset<IpMask> ParseHostsPath(const std::filesystem::path &file) {
  std::string name = file.string();
  tHostsNode *root = nullptr;
  ParseHostsFile(name.data(), &root);

  std::multiset<IpMask> result;
  while (root) {
    result.insert({root->ip, root->mask});
    tHostsNode *next = root->next;
    free(root);
    root = next;
  }
  return result;
}

HostsFileSandbox::HostsFileSandbox()
    : m_saved_open(DLLOpenCFILE), m_saved_eof(DLLcfeof), m_saved_read(DLLcf_ReadString), m_saved_close(DLLcfclose),
      m_saved_addr(DLLnw_GetHostAddressFromNumbers) {
  DLLOpenCFILE = TestOpenCFILE;
  DLLcfeof = cfeof;
  DLLcf_ReadString = cf_ReadString;
  DLLcfclose = cfclose;
  DLLnw_GetHostAddressFromNumbers = TestHostAddressFromNumbers;
}

HostsFileSandbox::~HostsFileSandbox() {
  DLLOpenCFILE = m_saved_open;
  DLLcfeof = m_saved_eof;
  DLLcf_ReadString = m_saved_read;
  DLLcfclose = m_saved_close;
  DLLnw_GetHostAddressFromNumbers = m_saved_addr;
}

std::multiset<IpMask> HostsFileSandbox::Parse(const std::string &contents) const {
  return ParseHostsPath(m_dir.Write("hosts.allow", contents));
}

} // namespace dmfc_test
