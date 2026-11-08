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

#include "dallas_fake_engine.h"

namespace dallas_fakes {

MemStream *S(void *f) { return static_cast<MemStream *>(f); }
void FakeWriteInt(int i, void *f) { S(f)->put<int32_t>(i); }
void FakeWriteFloat(float v, void *f) { S(f)->put<float>(v); }
void FakeWriteByte(int8_t b, void *f) { S(f)->put<int8_t>(b); }
int FakeReadInt(void *f) { return S(f)->get<int32_t>(); }
float FakeReadFloat(void *f) { return S(f)->get<float>(); }
int8_t FakeReadByte(void *f) { return S(f)->get<int8_t>(); }

msafe_struct g_world_pos;
msafe_struct g_moved_to;
bool g_move_called;

void FakeGetValue(int type, msafe_struct *ms) {
  if (type == MSAFE_OBJECT_WORLD_POSITION) {
    ms->pos = g_world_pos.pos;
    ms->orient = g_world_pos.orient;
    ms->roomnum = g_world_pos.roomnum;
  }
}
void FakeCallFunction(int type, msafe_struct *ms) {
  if (type == MSAFE_OBJECT_WORLD_POSITION) {
    g_move_called = true;
    g_moved_to.pos = ms->pos;
    g_moved_to.orient = ms->orient;
    g_moved_to.roomnum = ms->roomnum;
  }
}

} // namespace dallas_fakes
