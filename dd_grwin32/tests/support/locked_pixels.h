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

#include "ddgr.h"

class LockedPixels {
public:
  explicit LockedPixels(ddgr_surface *sf);
  ~LockedPixels();

  bool valid() const { return m_data != nullptr; }
  int rowsize() const { return m_rowsize; }

  uint32_t get(int x, int y) const;

  void set(int x, int y, uint32_t value);

  void fill(uint32_t value);

private:
  ddgr_surface *m_sf;
  uint8_t *m_data = nullptr;
  int m_rowsize = 0;
};
