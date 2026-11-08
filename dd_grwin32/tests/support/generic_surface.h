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

#include "ddgr.h"

ddgr_surface MakeSurface(int w, int h, int bpp, int type, int flags, const char *name);

class GenericSurface {
public:
  GenericSurface(int w, int h, int bpp);
  ~GenericSurface();
  GenericSurface(const GenericSurface &) = delete;
  GenericSurface &operator=(const GenericSurface &) = delete;

  ddgr_surface *get() { return &m_surface; }

private:
  ddgr_surface m_surface;
};
