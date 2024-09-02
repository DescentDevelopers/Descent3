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

#ifndef LIBMVE_SOUND_INTERFACE_H_
#define LIBMVE_SOUND_INTERFACE_H_

#include <cstdint>

namespace D3 {

/// Abstract class for sound device.
class ISoundDevice {
protected:
  bool m_is_compressed = false;

public:
  virtual ~ISoundDevice() = default;

  /// Play stream
  virtual void Play() {};
  /// Stop stream
  virtual void Stop() {};
  /// Lock buffer for various atomic operations
  virtual void Lock() {};
  /// Unlock buffer
  virtual void Unlock() {};

  /// Check if encoded sound is compressed
  [[nodiscard]] bool IsCompressed() const { return m_is_compressed; };

};

}
#endif // LIBMVE_SOUND_INTERFACE_H_
