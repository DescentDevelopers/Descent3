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

// UIListBox keeps two orders: the caller-facing index of an item is the order in which it was
// added, while the displayed order is sorted by text unless UILB_NOSORT is given. The arrow keys
// walk the displayed order.

#include <algorithm>
#include <cstdio>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "support/ui_test_util.h"

namespace {

constexpr int kListId = 3;

class UIListBoxTest : public UIWindowTest {
protected:
  UIListBox lb;
  ListBoxItems items{lb};
};

TEST_F(UIListBoxTest, SortedListIndexesByInsertionButDisplaysAlphabetically) {
  CreateListBox(lb, wnd, kListId);
  items.Add("delta");
  items.Add("Alpha");
  items.Add("charlie");
  items.Add("bravo");

  EXPECT_EQ(ByIndex(lb), (std::vector<std::string>{"delta", "Alpha", "charlie", "bravo"}));
  EXPECT_EQ(DisplayOrder(wnd, lb), (std::vector<std::string>{"Alpha", "bravo", "charlie", "delta"}));
}

TEST_F(UIListBoxTest, SortIsCaseInsensitiveByDefault) {
  CreateListBox(lb, wnd, kListId);
  items.Add("Banana");
  items.Add("apple");
  EXPECT_EQ(DisplayOrder(wnd, lb), (std::vector<std::string>{"apple", "Banana"}));
}

TEST_F(UIListBoxTest, CaseSensitiveFlagSortsByCharacterCode) {
  CreateListBox(lb, wnd, kListId, UILB_CASESENSITIVE);
  items.Add("apple");
  items.Add("Banana");
  EXPECT_EQ(DisplayOrder(wnd, lb), (std::vector<std::string>{"Banana", "apple"}));
}

TEST_F(UIListBoxTest, NoSortListDisplaysInInsertionOrder) {
  CreateListBox(lb, wnd, kListId, UILB_NOSORT);
  items.Add("delta");
  items.Add("Alpha");
  items.Add("charlie");
  EXPECT_EQ(ByIndex(lb), (std::vector<std::string>{"delta", "Alpha", "charlie"}));
  EXPECT_EQ(DisplayOrder(wnd, lb), (std::vector<std::string>{"delta", "Alpha", "charlie"}));
}

// The item storage grows in fixed-size steps and the sorted insert has separate code paths for
// "needs to grow" and "has room", so use enough items to cross several growth steps.
TEST_F(UIListBoxTest, ManyItemsKeepBothOrdersConsistent) {
  CreateListBox(lb, wnd, kListId);
  constexpr int kCount = 67;
  std::vector<std::string> inserted;
  for (int i = 0; i < kCount; i++) {
    char buf[16];
    snprintf(buf, sizeof(buf), "item%02d", (i * 29) % kCount); // a fixed permutation of 0..kCount-1
    inserted.push_back(buf);
    items.Add(buf);
  }
  ASSERT_EQ(lb.GetNumItems(), kCount);

  std::vector<std::string> sorted = inserted;
  std::sort(sorted.begin(), sorted.end());
  EXPECT_EQ(ByIndex(lb), inserted);
  EXPECT_EQ(DisplayOrder(wnd, lb), sorted);
}

TEST_F(UIListBoxTest, ManyItemsInNoSortList) {
  CreateListBox(lb, wnd, kListId, UILB_NOSORT);
  constexpr int kCount = 45;
  std::vector<std::string> inserted;
  for (int i = 0; i < kCount; i++) {
    char buf[16];
    snprintf(buf, sizeof(buf), "item%02d", (i * 7) % kCount);
    inserted.push_back(buf);
    items.Add(buf);
  }
  EXPECT_EQ(ByIndex(lb), inserted);
  EXPECT_EQ(DisplayOrder(wnd, lb), inserted);
}

TEST_F(UIListBoxTest, InterleavedAddAndRemoveMatchesModel) {
  CreateListBox(lb, wnd, kListId);
  std::vector<std::string> model; // caller-facing order
  std::vector<UITextItem *> handles;

  auto check = [&](const char *step) {
    SCOPED_TRACE(step);
    std::vector<std::string> sorted = model;
    std::sort(sorted.begin(), sorted.end());
    ASSERT_EQ(lb.GetNumItems(), (int)model.size());
    EXPECT_EQ(ByIndex(lb), model);
    EXPECT_EQ(DisplayOrder(wnd, lb), sorted);
  };
  auto add = [&](const std::string &text) {
    handles.push_back(items.Add(text));
    model.push_back(text);
  };
  auto remove = [&](const std::string &text) {
    auto it = std::find(model.begin(), model.end(), text);
    ASSERT_NE(it, model.end());
    size_t pos = it - model.begin();
    lb.RemoveItem(handles[pos]);
    model.erase(model.begin() + pos);
    handles.erase(handles.begin() + pos);
  };

  for (const char *t : {"m", "c", "x", "a", "q"})
    add(t);
  check("initial adds");

  remove("c"); // middle of both orders
  check("remove c");
  remove("q"); // last by index
  check("remove q");
  remove("m"); // first by index
  check("remove m");

  for (const char *t : {"b", "z", "n", "c", "y"})
    add(t);
  check("re-adds");

  // Push past the first storage growth step after removals have happened.
  for (int i = 0; i < 25; i++) {
    char buf[8];
    snprintf(buf, sizeof(buf), "k%02d", (i * 11) % 25);
    add(buf);
  }
  check("growth");

  remove("a");
  remove("k07");
  remove("z");
  check("final removes");
}

TEST_F(UIListBoxTest, RemovingAnUnknownItemChangesNothing) {
  CreateListBox(lb, wnd, kListId);
  items.Add("a");
  items.Add("b");
  UITextItem stranger("a");
  lb.RemoveItem(&stranger);
  EXPECT_EQ(ByIndex(lb), (std::vector<std::string>{"a", "b"}));
}

TEST_F(UIListBoxTest, GetItemOutOfRangeReturnsNull) {
  CreateListBox(lb, wnd, kListId);
  EXPECT_EQ(lb.GetItem(0), nullptr);
  items.Add("a");
  items.Add("b");
  EXPECT_EQ(lb.GetItem(-1), nullptr);
  EXPECT_EQ(lb.GetItem(2), nullptr);
}

TEST_F(UIListBoxTest, RemoveAllEmptiesAndTheListIsReusable) {
  CreateListBox(lb, wnd, kListId);
  items.Add("a");
  items.Add("b");
  lb.RemoveAll();
  EXPECT_EQ(lb.GetNumItems(), 0);
  EXPECT_EQ(lb.GetItem(0), nullptr);

  items.Add("z");
  items.Add("y");
  EXPECT_EQ(ByIndex(lb), (std::vector<std::string>{"z", "y"}));
  EXPECT_EQ(DisplayOrder(wnd, lb), (std::vector<std::string>{"y", "z"}));
}

TEST_F(UIListBoxTest, SetSelectedIndexUsesCallerFacingIndex) {
  CreateListBox(lb, wnd, kListId);
  items.Add("m");
  items.Add("a");
  items.Add("z");
  lb.SetSelectedIndex(0);
  EXPECT_EQ(SelectedText(lb), "m");
  lb.SetSelectedIndex(2);
  EXPECT_EQ(SelectedText(lb), "z");
  EXPECT_EQ(lb.GetSelectedIndex(), 2);
}

TEST_F(UIListBoxTest, SelectItemSelectsByPointer) {
  CreateListBox(lb, wnd, kListId);
  items.Add("m");
  UITextItem *a = items.Add("a");
  items.Add("z");
  lb.SelectItem(a);
  EXPECT_EQ(lb.GetSelectedIndex(), 1);
}

TEST_F(UIListBoxTest, OutOfRangeSelectionIsIgnored) {
  CreateListBox(lb, wnd, kListId);
  items.Add("m");
  items.Add("a");
  lb.SetSelectedIndex(1);
  lb.SetSelectedIndex(2);
  lb.SetSelectedIndex(-1);
  EXPECT_EQ(SelectedText(lb), "a");
}

TEST_F(UIListBoxTest, SelectionStaysOnItemWhenOthersAreInsertedAroundIt) {
  CreateListBox(lb, wnd, kListId);
  items.Add("m");
  lb.SetSelectedIndex(0);
  items.Add("a"); // displayed before the selection
  items.Add("z"); // displayed after it
  items.Add("b"); // before again
  EXPECT_EQ(SelectedText(lb), "m");
}

TEST_F(UIListBoxTest, SelectionStaysOnItemWhenOthersAreRemoved) {
  CreateListBox(lb, wnd, kListId);
  UITextItem *a = items.Add("a");
  items.Add("m");
  UITextItem *z = items.Add("z");
  UITextItem *b = items.Add("b");
  lb.SetSelectedIndex(1);
  ASSERT_EQ(SelectedText(lb), "m");

  lb.RemoveItem(a); // displayed before the selection, and earlier by index
  EXPECT_EQ(SelectedText(lb), "m");
  lb.RemoveItem(z); // displayed after it
  EXPECT_EQ(SelectedText(lb), "m");
  lb.RemoveItem(b); // displayed before, later by index
  EXPECT_EQ(SelectedText(lb), "m");
}

TEST_F(UIListBoxTest, RemovingTheSelectedItemLeavesAValidSelection) {
  CreateListBox(lb, wnd, kListId);
  items.Add("a");
  UITextItem *m = items.Add("m");
  items.Add("z");
  lb.SetSelectedIndex(1);
  lb.RemoveItem(m);

  int sel = lb.GetSelectedIndex();
  ASSERT_GE(sel, 0);
  ASSERT_LT(sel, lb.GetNumItems());
  EXPECT_NE(SelectedText(lb), "m");
}

std::vector<int> g_reported_indices;

TEST_F(UIListBoxTest, KeyboardSelectionChangeReportsCallerFacingIndex) {
  CreateListBox(lb, wnd, kListId);
  items.Add("c");         // index 0
  items.Add("a");         // index 1
  items.Add("b");         // index 2
  lb.SetSelectedIndex(1); // "a", top of the display

  g_reported_indices.clear();
  lb.SetSelectChangeCallback([](int index) { g_reported_indices.push_back(index); });
  Press(KEY_DOWN);
  Press(KEY_DOWN);
  EXPECT_EQ(g_reported_indices, (std::vector<int>{2, 0})); // "b" then "c"
  EXPECT_EQ(SelectedText(lb), "c");

  Press(KEY_DOWN);
  EXPECT_EQ(g_reported_indices.size(), 2u);
  EXPECT_EQ(SelectedText(lb), "c");
}

TEST_F(UIListBoxTest, SelectChangeCallbackWithUserDataGetsGadgetId) {
  CreateListBox(lb, wnd, kListId);
  items.Add("a");
  items.Add("b");

  struct Log {
    std::vector<int> ids;
  } log;
  lb.SetSelectChangeCallback([](int id, void *ptr) { static_cast<Log *>(ptr)->ids.push_back(id); }, &log);
  lb.SetSelectedIndex(1);
  Press(KEY_UP);
  EXPECT_EQ(log.ids, (std::vector<int>{kListId, kListId}));
}

} // namespace
