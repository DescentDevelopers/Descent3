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

#include "polymodel_harness.h"

#include <SDL3/SDL_assert.h>

namespace model_test {

std::filesystem::path ModelDir::Save(const pof::Writer &w, const std::string &filename) const {
  return m_dir.Write(filename, w.bytes());
}

int ModelDir::Load(const pof::Writer &w, const std::string &filename) const {
  return LoadPolyModel(Save(w, filename), 0);
}

poly_model *ModelDir::LoadOrFail(const pof::Writer &w, const std::string &filename) const {
  int handle = Load(w, filename);
  EXPECT_GE(handle, 0);
  return handle >= 0 ? &Poly_models[handle] : nullptr;
}

AssertionCounter::AssertionCounter() { SDL_SetAssertionHandler(&AssertionCounter::OnAssertion, this); }

SDL_AssertState SDLCALL AssertionCounter::OnAssertion(const SDL_AssertData *, void *userdata) {
  static_cast<AssertionCounter *>(userdata)->m_count++;
  return SDL_ASSERTION_IGNORE;
}

AssertionCounter::~AssertionCounter() { SDL_SetAssertionHandler(nullptr, nullptr); }

vector ModelPoint(poly_model *pm, int sub, vector p, float *normalized_time) {
  vector dest{};
  vector wpos{};
  matrix orient = IDENTITY_MATRIX;
  GetPolyModelPointInWorld(&dest, pm, &wpos, &orient, sub, normalized_time, &p);
  return dest;
}

void FreeAllPolyModels() {
  for (int i = 0; i < MAX_POLY_MODELS; i++) {
    if (Poly_models[i].used > 0) {
      Poly_models[i].used = 1;
      FreePolyModel(i);
    }
  }
}

} // namespace model_test
