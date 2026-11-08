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

#include <deque>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "d3_test/game.h"
#include "d3_test/game_clock.h"
#include "UIlib.h"
#include "ddio.h"

// Runs one frame of the window with no input; returns what Process() returned.
int Idle(UIWindow &wnd);

// Runs one frame with the key clicked; returns what Process() returned.
int Press(UIWindow &wnd, int key);

// Presses the key for each character of text in turn.
void Type(UIWindow &wnd, const char *text);

// Keys are sent as "clicked" (pressed and released in the same frame), which releases the
// key-repeat trap every time, so results never depend on the real clock.
class UIWindowTest : public ::testing::Test {
protected:
  UIWindow wnd;

  void SetUp() override {
    d3_test::EnsureMem();
    d3_test::EnsureChronoTimer();
    UI_input = {};
    UI_output = {};
    wnd.Create(0, 0, 640, 480);
  }

  // Destroys all gadgets while the test fixture's gadget members are still alive.
  void TearDown() override {
    wnd.Destroy();
    UI_input = {};
  }

  int Idle() { return ::Idle(wnd); }
  int Press(int key) { return ::Press(wnd, key); }
  void Type(const char *text) { ::Type(wnd, text); }
};

class PlainGadget : public UIGadget {
protected:
  void OnDraw() override {}
};

// Creates count gadgets g[0..count) in wnd, stacked vertically, with ids 100, 101, ... and the given flags.
void CreateGadgets(UIWindow &wnd, PlainGadget *g, int count, const int *flags = nullptr);

// Creates edit as a 200x20 box in wnd.
void CreateEdit(UIEdit &edit, UIWindow &wnd, int id, int flags = 0);

// The edit box's whole text.
std::string EditText(UIEdit &edit);

// Creates lb as a 200x100 list box in wnd.
void CreateListBox(UIListBox &lb, UIWindow &wnd, int id, int flags = 0);

// Owns the text items added to a list box, at stable addresses, for as long as the list box uses them.
class ListBoxItems {
public:
  explicit ListBoxItems(UIListBox &lb) : lb_(lb) {}
  ListBoxItems(const ListBoxItems &) = delete;
  ListBoxItems &operator=(const ListBoxItems &) = delete;

  // Adds a new text item to the list box and returns it.
  UITextItem *Add(const std::string &text);

private:
  UIListBox &lb_;
  std::deque<UITextItem> storage_;
};

// The text of the item at the caller-facing index.
std::string ItemText(UIListBox &lb, int index);

// The text of the selected item.
std::string SelectedText(UIListBox &lb);

// The item texts in caller-facing (insertion) order.
std::vector<std::string> ByIndex(UIListBox &lb);

// The item texts in displayed order, read by walking the selection with the arrow keys; changes the selection.
std::vector<std::string> DisplayOrder(UIWindow &wnd, UIListBox &lb);
