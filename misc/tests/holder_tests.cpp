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

// The moved-from holder passes the value-initialised handle to Deleter; that is fine for GL
// (deleting name 0 is a no-op), so these tests only count releases of real handles.

#include <utility>
#include <vector>

#include <gtest/gtest.h>

#include "holder.h"

#include "support/release_recorder.h"

namespace {

using namespace release_recorder;

using Holder = MoveOnlyHolder<int, RecordRelease>;

class MoveOnlyHolderTest : public ::testing::Test {
protected:
  void SetUp() override { g_released.clear(); }
  void TearDown() override { g_released.clear(); }
};

} // namespace

TEST_F(MoveOnlyHolderTest, ReleasesHandleOnceWhenDestroyed) {
  {
    Holder h(17);
    EXPECT_EQ(static_cast<int>(h), 17);
    EXPECT_EQ(ReleaseCount(17), 0) << "handle released while still owned";
  }
  EXPECT_EQ(ReleaseCount(17), 1);
}

TEST_F(MoveOnlyHolderTest, MoveTransfersOwnershipWithoutDoubleRelease) {
  {
    Holder source(42);
    {
      Holder target(std::move(source));
      EXPECT_EQ(static_cast<int>(target), 42);
    }
    EXPECT_EQ(ReleaseCount(42), 1);
  }
  EXPECT_EQ(ReleaseCount(42), 1);
}

TEST_F(MoveOnlyHolderTest, HandlesSurviveContainerReallocation) {
  {
    std::vector<Holder> holders;
    for (int i = 1; i <= 50; ++i)
      holders.emplace_back(i);
    for (int i = 1; i <= 50; ++i)
      EXPECT_EQ(static_cast<int>(holders[i - 1]), i);
    for (int i = 1; i <= 50; ++i)
      EXPECT_EQ(ReleaseCount(i), 0) << i;
  }
  for (int i = 1; i <= 50; ++i)
    EXPECT_EQ(ReleaseCount(i), 1) << i;
}
