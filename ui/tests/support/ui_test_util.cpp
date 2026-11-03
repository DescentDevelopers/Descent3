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

#include "ui_test_util.h"

int Idle(UIWindow &wnd) {
  UI_input = {};
  return wnd.Process();
}

int Press(UIWindow &wnd, int key) {
  UI_input = {};
  UI_input.key = key;
  UI_input.key_status = UIKEY_CLICKED;
  int result = wnd.Process();
  UI_input = {};
  return result;
}

void Type(UIWindow &wnd, const char *text) {
  for (const char *c = text; *c; ++c) {
    int key = ddio_AsciiToKey(*c);
    ASSERT_NE(key, 0) << "no key for character '" << *c << "'";
    Press(wnd, key);
  }
}

void CreateGadgets(UIWindow &wnd, PlainGadget *g, int count, const int *flags) {
  for (int i = 0; i < count; i++)
    g[i].Create(&wnd, 100 + i, 10, 10 + 30 * i, 100, 20, flags ? flags[i] : 0);
}

void CreateEdit(UIEdit &edit, UIWindow &wnd, int id, int flags) { edit.Create(&wnd, id, 10, 10, 200, 20, flags); }

std::string EditText(UIEdit &edit) {
  char buf[1100];
  edit.GetText(buf, sizeof(buf));
  return buf;
}

void CreateListBox(UIListBox &lb, UIWindow &wnd, int id, int flags) { lb.Create(&wnd, id, 10, 10, 200, 100, flags); }

UITextItem *ListBoxItems::Add(const std::string &text) {
  storage_.emplace_back(text.c_str());
  lb_.AddItem(&storage_.back());
  return &storage_.back();
}

namespace {

std::string TextOf(const UIItem *item) { return item ? static_cast<const UITextItem *>(item)->GetBuffer() : "<null>"; }

} // namespace

std::string ItemText(UIListBox &lb, int index) { return TextOf(lb.GetItem(index)); }

std::string SelectedText(UIListBox &lb) { return ItemText(lb, lb.GetSelectedIndex()); }

std::vector<std::string> ByIndex(UIListBox &lb) {
  std::vector<std::string> out;
  for (int i = 0; i < lb.GetNumItems(); i++)
    out.push_back(ItemText(lb, i));
  return out;
}

std::vector<std::string> DisplayOrder(UIWindow &wnd, UIListBox &lb) {
  std::vector<std::string> out;
  int n = lb.GetNumItems();
  for (int i = 0; i < n; i++)
    Press(wnd, KEY_UP);
  for (int i = 0; i < n; i++) {
    out.push_back(SelectedText(lb));
    Press(wnd, KEY_DOWN);
  }
  return out;
}
