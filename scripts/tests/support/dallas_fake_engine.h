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

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <vector>

#include "osiris_common.h"

namespace dallas_fakes {

struct MemStream {
  std::vector<uint8_t> data;
  size_t read_pos = 0;

  template <typename T> void put(T v) {
    const auto *p = reinterpret_cast<const uint8_t *>(&v);
    data.insert(data.end(), p, p + sizeof(T));
  }
  template <typename T> T get() {
    T v{};
    if (read_pos + sizeof(T) <= data.size())
      std::memcpy(&v, data.data() + read_pos, sizeof(T));
    read_pos += sizeof(T);
    return v;
  }
};

void FakeWriteInt(int i, void *f);
void FakeWriteFloat(float v, void *f);
void FakeWriteByte(int8_t b, void *f);
int FakeReadInt(void *f);
float FakeReadFloat(void *f);
int8_t FakeReadByte(void *f);

extern msafe_struct g_world_pos;
extern msafe_struct g_moved_to;
extern bool g_move_called;

void FakeGetValue(int type, msafe_struct *ms);
void FakeCallFunction(int type, msafe_struct *ms);

} // namespace dallas_fakes
