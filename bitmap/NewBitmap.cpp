/*
 * Descent 3
 * Copyright (C) 2024 Descent Developers
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

#include "NewBitmap.h"
#include "pserror.h"

#include <stb_image_write.h>

NewBitmap::NewBitmap(uint32_t w, uint32_t h, PixelDataFormat format, bool flippedY)
    : _w(w), _h(h), _format(format), _flippedY(flippedY), _data(allocateData(w, h, format)) {
  ASSERT(_data.get());
}

std::unique_ptr<uint8_t[]> NewBitmap::allocateData(uint32_t w, uint32_t h, PixelDataFormat format) {
  switch (format) {
  case PixelDataFormat::RGBA32:
    return std::make_unique<uint8_t[]>(w * h * 4);
  default:
    return nullptr;
  }
}

uint32_t NewBitmap::getNumComponents() const {
  switch (_format) {
  case PixelDataFormat::RGBA32:
    return 4;
  default:
    return 0;
  }
}

uint32_t NewBitmap::getStride() const {
  switch (_format) {
  case PixelDataFormat::RGBA32:
    return _w * 4;
  default:
    return 0;
  }
}

bool NewBitmap::saveAsPNG(const char *filePath) {
  // TODO: Support more formats? stb_write supports at least RGB and RGBA
  ASSERT(_format == PixelDataFormat::RGBA32);

  stbi_flip_vertically_on_write(_flippedY);
  return stbi_write_png(filePath, _w, _h, getNumComponents(), _data.get(), getStride()) == 0;
}
