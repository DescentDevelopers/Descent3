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

#include <filesystem>
#include <string>

#include <SDL3/SDL_assert.h>
#include <gtest/gtest.h>

#include "d3_test/files.h"
#include "polymodel.h"
#include "vecmat.h"

#include "pof_writer.h"

namespace model_test {

constexpr float kTol = 1e-3f;

#define EXPECT_VEC_NEAR(v, ex, ey, ez)                                                                                 \
  do {                                                                                                                 \
    EXPECT_NEAR((v).x(), (ex), ::model_test::kTol);                                                                    \
    EXPECT_NEAR((v).y(), (ey), ::model_test::kTol);                                                                    \
    EXPECT_NEAR((v).z(), (ez), ::model_test::kTol);                                                                    \
  } while (0)

// A temporary directory that model files are written to and loaded from. Removed on destruction.
class ModelDir {
public:
  const std::filesystem::path &path() const { return m_dir.path(); }

  std::filesystem::path Save(const pof::Writer &w, const std::string &filename) const;
  // Saves the model and loads it with LoadPolyModel, returning its handle (-1 on failure).
  int Load(const pof::Writer &w, const std::string &filename = "model.oof") const;
  // Like Load, but fails the test and returns null if the model doesn't load.
  poly_model *LoadOrFail(const pof::Writer &w, const std::string &filename = "model.oof") const;

private:
  d3_test::TempDir m_dir;
};

// Counts SDL assertions (ASSERT, Int3) while alive, ignoring them instead of breaking.
class AssertionCounter {
public:
  AssertionCounter();
  ~AssertionCounter();
  AssertionCounter(const AssertionCounter &) = delete;
  AssertionCounter &operator=(const AssertionCounter &) = delete;

  int count() const { return m_count; }

private:
  static SDL_AssertState SDLCALL OnAssertion(const SDL_AssertData *, void *userdata);

  int m_count = 0;
};

// Where point `p` of subobject `sub` ends up for a model placed at the origin with no rotation.
vector ModelPoint(poly_model *pm, int sub, vector p, float *normalized_time);

// Frees every loaded model, regardless of its reference count.
void FreeAllPolyModels();

} // namespace model_test
