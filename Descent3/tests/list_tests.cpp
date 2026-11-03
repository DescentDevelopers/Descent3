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

#include <vector>

#include <SDL3/SDL_assert.h>
#include <gtest/gtest.h>

#include "d3_test/game_assertions.h"
#include "list.h"
#include "support/list_walk.h"

namespace {

class ListTest : public ::testing::Test {
protected:
  void TearDown() override { DestroyList(&list); }

  // Some list operations Int3() on misuse that the tests exercise on purpose.
  d3_test::ExpectGameAssertions expected_assertions;

  listnode *list = nullptr;
  int a = 0, b = 0, c = 0, d = 0;
};

TEST_F(ListTest, EmptyList) {
  EXPECT_EQ(CountListItems(&list), 0);
  EXPECT_EQ(GetListItem(&list, 0), nullptr);
  EXPECT_EQ(GetListItemIndex(&list, &a), -1);
}

TEST_F(ListTest, AddAppendsInOrder) {
  ASSERT_EQ(AddListItem(&list, &a), 1);
  ASSERT_EQ(AddListItem(&list, &b), 1);
  ASSERT_EQ(AddListItem(&list, &c), 1);

  EXPECT_EQ(CountListItems(&list), 3);
  EXPECT_EQ(Forward(list), (std::vector<void *>{&a, &b, &c}));
  EXPECT_EQ(Backward(list), (std::vector<void *>{&c, &b, &a}));
  EXPECT_EQ(list->prev, nullptr);
}

TEST_F(ListTest, GetListItemAndIndexAreInverse) {
  AddListItem(&list, &a);
  AddListItem(&list, &b);
  AddListItem(&list, &c);

  for (int i = 0; i < 3; i++)
    EXPECT_EQ(GetListItemIndex(&list, GetListItem(&list, i)), i);

  EXPECT_EQ(GetListItem(&list, 3), nullptr);
  EXPECT_EQ(GetListItemIndex(&list, &d), -1);
}

TEST_F(ListTest, RemoveHeadKeepsLinksConsistent) {
  AddListItem(&list, &a);
  AddListItem(&list, &b);
  AddListItem(&list, &c);

  EXPECT_EQ(RemoveListItem(&list, &a), 1);

  EXPECT_EQ(Forward(list), (std::vector<void *>{&b, &c}));
  EXPECT_EQ(Backward(list), (std::vector<void *>{&c, &b}));
  EXPECT_EQ(list->prev, nullptr);
}

TEST_F(ListTest, RemoveMiddleKeepsLinksConsistent) {
  AddListItem(&list, &a);
  AddListItem(&list, &b);
  AddListItem(&list, &c);

  EXPECT_EQ(RemoveListItem(&list, &b), 1);

  EXPECT_EQ(Forward(list), (std::vector<void *>{&a, &c}));
  EXPECT_EQ(Backward(list), (std::vector<void *>{&c, &a}));
}

TEST_F(ListTest, RemoveTailKeepsLinksConsistent) {
  AddListItem(&list, &a);
  AddListItem(&list, &b);
  AddListItem(&list, &c);

  EXPECT_EQ(RemoveListItem(&list, &c), 1);

  EXPECT_EQ(Forward(list), (std::vector<void *>{&a, &b}));
  EXPECT_EQ(Backward(list), (std::vector<void *>{&b, &a}));
}

TEST_F(ListTest, RemovingOnlyItemEmptiesList) {
  AddListItem(&list, &a);

  EXPECT_EQ(RemoveListItem(&list, &a), 1);

  EXPECT_EQ(list, nullptr);
  EXPECT_EQ(CountListItems(&list), 0);
}

TEST_F(ListTest, RemoveMissingItemFailsAndLeavesListUnchanged) {
  AddListItem(&list, &a);
  AddListItem(&list, &b);

  EXPECT_EQ(RemoveListItem(&list, &c), 0);

  EXPECT_EQ(Forward(list), (std::vector<void *>{&a, &b}));
}

TEST_F(ListTest, ListCanBeRebuiltAfterRemovingEverything) {
  AddListItem(&list, &a);
  AddListItem(&list, &b);
  RemoveListItem(&list, &b);
  RemoveListItem(&list, &a);
  ASSERT_EQ(list, nullptr);

  AddListItem(&list, &c);
  AddListItem(&list, &a);

  EXPECT_EQ(Forward(list), (std::vector<void *>{&c, &a}));
  EXPECT_EQ(Backward(list), (std::vector<void *>{&a, &c}));
}

TEST_F(ListTest, AddRejectsDuplicateOfNonTailItem) {
  AddListItem(&list, &a);
  AddListItem(&list, &b);

  EXPECT_EQ(AddListItem(&list, &a), 0);
  EXPECT_EQ(Forward(list), (std::vector<void *>{&a, &b}));
}

} // namespace
