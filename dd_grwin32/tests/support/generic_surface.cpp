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

#include "generic_surface.h"

#include <cstdint>
#include <cstring>

#include <gtest/gtest.h>

ddgr_surface MakeSurface(int w, int h, int bpp, int type, int flags, const char *name) {
  ddgr_surface sf{};
  sf.w = w;
  sf.h = h;
  sf.bpp = bpp;
  sf.type = static_cast<uint16_t>(type);
  sf.flags = static_cast<uint16_t>(flags);
  std::strncpy(sf.name, name, SURF_NAMELEN - 1);
  return sf;
}

GenericSurface::GenericSurface(int w, int h, int bpp)
    : m_surface(MakeSurface(w, h, bpp, SURFTYPE_GENERIC, 0, "generic")) {
  EXPECT_TRUE(ddgr_surf_Create(&m_surface));
}

GenericSurface::~GenericSurface() { ddgr_surf_Destroy(&m_surface); }
