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
#include "ddgrWin32DX.h"

struct DxPixelFormat {
  int bits;
  uint32_t rmask, gmask, bmask;
};

inline constexpr DxPixelFormat kX8R8G8B8 = {32, 0x00ff0000, 0x0000ff00, 0x000000ff};

class DxSurface {
public:
  DxSurface(int w, int h, const DxPixelFormat &format = kX8R8G8B8);
  ~DxSurface();
  DxSurface(const DxSurface &) = delete;
  DxSurface &operator=(const DxSurface &) = delete;

  bool valid() const { return m_dx.lpdds != nullptr; }
  ddgr_surface *get() { return &m_surface; }

  uint32_t get(int x, int y);
  void set(int x, int y, uint32_t value);
  void fill(uint32_t value);

private:
  DxPixelFormat m_format;
  LPDIRECTDRAW m_dd = nullptr;
  tDXSurface m_dx{};
  ddgr_surface m_surface{};
};
