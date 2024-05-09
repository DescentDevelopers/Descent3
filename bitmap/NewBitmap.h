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

#ifndef DESCENT3_NEWBITMAP_H
#define DESCENT3_NEWBITMAP_H

#include <cstdint>
#include <memory>

enum class PixelDataFormat {
  Unknown,
  RGBA32
};

class NewBitmap {
private:
  uint32_t _w, _h;
  PixelDataFormat _format;
  bool _flippedY;
  std::unique_ptr<uint8_t[]> _data;

  static std::unique_ptr<uint8_t[]> allocateData(uint32_t w, uint32_t h, PixelDataFormat format);

  [[nodiscard]] uint32_t getStride() const;

public:
  /// Creates a new bitmap of size "w * h" and the specified format. Will allocate enough memory to store it.
  NewBitmap(uint32_t w, uint32_t h, PixelDataFormat format, bool flippedY = false);

  void getSize(uint32_t &w, uint32_t &h) const {
    w = _w;
    h = _h;
  }

  /// @returns raw pointer to image data
  [[nodiscard]] uint8_t *getData() const { return _data.get(); }

  /// @returns number of components for the bitmap format (i.e. 4 for "RGBA")
  [[nodiscard]] uint32_t getNumComponents() const;

  /// @returns true on success, false otherwise
  bool saveAsPNG(const char* filePath);
};

#endif // DESCENT3_NEWBITMAP_H
