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

#include <gtest/gtest.h>

#include "support/ui_test_util.h"

namespace {

std::vector<int> g_callback_values;

constexpr int kComboId = 11;

class UIComboBoxTest : public UIWindowTest {
protected:
  UIComboBox combo;
  UITextItem a{"a"}, b{"b"}, c{"c"};

  void SetUp() override {
    UIWindowTest::SetUp();
    g_callback_values.clear();
    combo.Create(&wnd, kComboId, 10, 10, 200, 20);
    combo.AddItem(&a);
    combo.AddItem(&b);
    combo.AddItem(&c);
  }
};

TEST_F(UIComboBoxTest, ItemsKeepInsertionOrder) {
  EXPECT_EQ(combo.GetItem(0), &a);
  EXPECT_EQ(combo.GetItem(1), &b);
  EXPECT_EQ(combo.GetItem(2), &c);
  EXPECT_EQ(combo.GetItem(3), nullptr);
}

TEST_F(UIComboBoxTest, RemoveItemClosesTheGap) {
  combo.RemoveItem(&b);
  EXPECT_EQ(combo.GetItem(0), &a);
  EXPECT_EQ(combo.GetItem(1), &c);
  EXPECT_EQ(combo.GetItem(2), nullptr);
}

TEST_F(UIComboBoxTest, ArrowKeysStepThroughItemsAndStopAtTheEnds) {
  combo.SetSelectChangeCallback([](int index) { g_callback_values.push_back(index); });
  ASSERT_EQ(combo.GetSelectedIndex(), 0);

  Press(KEY_UP); // already at the top
  Press(KEY_DOWN);
  Press(KEY_DOWN);
  EXPECT_EQ(combo.GetSelectedIndex(), 2);
  Press(KEY_DOWN); // already at the bottom
  EXPECT_EQ(combo.GetSelectedIndex(), 2);
  Press(KEY_UP);
  EXPECT_EQ(combo.GetSelectedIndex(), 1);

  EXPECT_EQ(g_callback_values, (std::vector<int>{1, 2, 1}));
}

TEST_F(UIComboBoxTest, SelectItemSelectsByPointer) {
  combo.SelectItem(&c);
  EXPECT_EQ(combo.GetSelectedIndex(), 2);
}

TEST_F(UIComboBoxTest, OutOfRangeSelectionIsIgnored) {
  combo.SetSelectedIndex(1);
  combo.SetSelectedIndex(3);
  EXPECT_EQ(combo.GetSelectedIndex(), 1);
}

// UISlider::GetPos() is documented (ui.h) as ranging over 0 .. GetRange()-1, and SetPos() clamps
// to that range.

constexpr int kSliderId = 12;

class UISliderTest : public UIWindowTest {
protected:
  UISlider slider;

  void SetUp() override {
    UIWindowTest::SetUp();
    g_callback_values.clear();
    slider.Create(&wnd, kSliderId, 10, 10, 200, 20);
    slider.SetRange(5);
    slider.SetSelectChangeCallback([](int pos) { g_callback_values.push_back(pos); });
  }
};

TEST_F(UISliderTest, ArrowKeysMoveOneStepAndReportThePosition) {
  EXPECT_EQ(Press(KEY_RIGHT), kSliderId);
  EXPECT_EQ(Press(KEY_RIGHT), kSliderId);
  EXPECT_EQ(slider.GetPos(), 2);
  EXPECT_EQ(Press(KEY_LEFT), kSliderId);
  EXPECT_EQ(slider.GetPos(), 1);
  EXPECT_EQ(g_callback_values, (std::vector<int>{1, 2, 1}));
}

TEST_F(UISliderTest, LeftArrowStopsAtZero) {
  EXPECT_EQ(Press(KEY_LEFT), -1);
  EXPECT_EQ(slider.GetPos(), 0);
  EXPECT_TRUE(g_callback_values.empty());
}

TEST_F(UISliderTest, RightArrowStopsAtTheLastPosition) {
  slider.SetPos(4);
  g_callback_values.clear();
  Press(KEY_RIGHT);
  EXPECT_EQ(slider.GetPos(), 4);
  EXPECT_TRUE(g_callback_values.empty());
}

} // namespace
