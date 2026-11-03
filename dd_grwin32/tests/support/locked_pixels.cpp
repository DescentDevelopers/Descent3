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

#include "locked_pixels.h"

#include <cstring>

#include <gtest/gtest.h>

namespace {

uint32_t BytesPerPixel(int bpp) { return bpp == BPP_16 ? 2 : 4; }

} // namespace

LockedPixels::LockedPixels(ddgr_surface *sf) : m_sf(sf) {
  void *ptr = nullptr;
  EXPECT_TRUE(ddgr_surf_Lock(sf, &ptr, &m_rowsize));
  m_data = static_cast<uint8_t *>(ptr);
}

LockedPixels::~LockedPixels() { ddgr_surf_Unlock(m_sf, m_data); }

uint32_t LockedPixels::get(int x, int y) const {
  const uint8_t *p = m_data + y * m_rowsize + x * BytesPerPixel(m_sf->bpp);
  if (m_sf->bpp == BPP_16) {
    uint16_t v;
    std::memcpy(&v, p, sizeof(v));
    return v;
  }
  uint32_t v;
  std::memcpy(&v, p, sizeof(v));
  return v & 0x00ffffff; // the top byte of a 32bpp pixel is unused
}

void LockedPixels::set(int x, int y, uint32_t value) {
  uint8_t *p = m_data + y * m_rowsize + x * BytesPerPixel(m_sf->bpp);
  if (m_sf->bpp == BPP_16) {
    uint16_t v = static_cast<uint16_t>(value);
    std::memcpy(p, &v, sizeof(v));
  } else {
    std::memcpy(p, &value, sizeof(value));
  }
}

void LockedPixels::fill(uint32_t value) {
  for (int y = 0; y < m_sf->h; y++)
    for (int x = 0; x < m_sf->w; x++)
      set(x, y, value);
}
