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

#include "dx_surface.h"

#include <cstring>

#include <gtest/gtest.h>

#include "generic_surface.h"

DxSurface::DxSurface(int w, int h, const DxPixelFormat &format)
    : m_format(format), m_surface(MakeSurface(w, h, format.bits, SURFTYPE_GENERIC, 0, "dx")) {
  EXPECT_EQ(DirectDrawCreate(nullptr, &m_dd, nullptr), DD_OK);
  if (!m_dd)
    return;
  EXPECT_EQ(m_dd->SetCooperativeLevel(nullptr, DDSCL_NORMAL), DD_OK);

  DDSURFACEDESC ddsd;
  std::memset(&ddsd, 0, sizeof(ddsd));
  ddsd.dwSize = sizeof(ddsd);
  ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | DDSD_PIXELFORMAT;
  ddsd.dwWidth = w;
  ddsd.dwHeight = h;
  ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
  ddsd.ddpfPixelFormat.dwSize = sizeof(ddsd.ddpfPixelFormat);
  ddsd.ddpfPixelFormat.dwFlags = DDPF_RGB;
  ddsd.ddpfPixelFormat.dwRGBBitCount = format.bits;
  ddsd.ddpfPixelFormat.dwRBitMask = format.rmask;
  ddsd.ddpfPixelFormat.dwGBitMask = format.gmask;
  ddsd.ddpfPixelFormat.dwBBitMask = format.bmask;
  EXPECT_EQ(m_dd->CreateSurface(&ddsd, &m_dx.lpdds, nullptr), DD_OK);

  m_surface.obj = &m_dx;
}

DxSurface::~DxSurface() {
  if (m_dx.lpdds)
    m_dx.lpdds->Release();
  if (m_dd)
    m_dd->Release();
}

uint32_t DxSurface::get(int x, int y) {
  void *ptr = nullptr;
  int rowsize = 0;
  EXPECT_TRUE(ddgr_dx_surf_Lock(&m_surface, &ptr, &rowsize));
  uint32_t v = 0;
  if (ptr) {
    std::memcpy(&v, static_cast<uint8_t *>(ptr) + y * rowsize + x * (m_format.bits / 8), m_format.bits / 8);
    ddgr_dx_surf_Unlock(&m_surface, ptr);
  }
  return v & (m_format.rmask | m_format.gmask | m_format.bmask);
}

void DxSurface::set(int x, int y, uint32_t value) {
  void *ptr = nullptr;
  int rowsize = 0;
  EXPECT_TRUE(ddgr_dx_surf_Lock(&m_surface, &ptr, &rowsize));
  if (!ptr)
    return;
  std::memcpy(static_cast<uint8_t *>(ptr) + y * rowsize + x * (m_format.bits / 8), &value, m_format.bits / 8);
  ddgr_dx_surf_Unlock(&m_surface, ptr);
}

void DxSurface::fill(uint32_t value) {
  void *ptr = nullptr;
  int rowsize = 0;
  EXPECT_TRUE(ddgr_dx_surf_Lock(&m_surface, &ptr, &rowsize));
  if (!ptr)
    return;
  for (int y = 0; y < m_surface.h; y++)
    for (int x = 0; x < m_surface.w; x++)
      std::memcpy(static_cast<uint8_t *>(ptr) + y * rowsize + x * (m_format.bits / 8), &value, m_format.bits / 8);
  ddgr_dx_surf_Unlock(&m_surface, ptr);
}
