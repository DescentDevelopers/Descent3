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

#include <string>

#include <gtest/gtest.h>

#include "support/ui_test_util.h"

namespace {

constexpr int kEditId = 7;

class UIEditTest : public UIWindowTest {
protected:
  UIEdit edit;
};

TEST_F(UIEditTest, TypedCharactersAreAppended) {
  CreateEdit(edit, wnd, kEditId);
  Type("Hello 42");
  EXPECT_EQ(EditText(edit), "Hello 42");
}

TEST_F(UIEditTest, TypingInsertsAtTheCursor) {
  CreateEdit(edit, wnd, kEditId);
  Type("ac");
  Press(KEY_LEFT);
  Type("b");
  EXPECT_EQ(EditText(edit), "abc");

  Type("X");
  EXPECT_EQ(EditText(edit), "abXc");
}

TEST_F(UIEditTest, SetTextPutsTheCursorAtTheEnd) {
  CreateEdit(edit, wnd, kEditId);
  edit.SetText("hello");
  Type("!");
  EXPECT_EQ(EditText(edit), "hello!");
}

TEST_F(UIEditTest, BackspaceRemovesTheCharacterBeforeTheCursor) {
  CreateEdit(edit, wnd, kEditId);
  Type("abc");
  Press(KEY_LEFT);
  Press(KEY_BACKSP);
  EXPECT_EQ(EditText(edit), "ac");

  Type("X");
  EXPECT_EQ(EditText(edit), "aXc");
}

TEST_F(UIEditTest, BackspaceAtTheStartDoesNothing) {
  CreateEdit(edit, wnd, kEditId);
  Type("ab");
  Press(KEY_LEFT);
  Press(KEY_LEFT);
  Press(KEY_BACKSP);
  EXPECT_EQ(EditText(edit), "ab");

  Press(KEY_LEFT);
  Type("X");
  EXPECT_EQ(EditText(edit), "Xab");
}

TEST_F(UIEditTest, DeleteRemovesTheCharacterAtTheCursor) {
  CreateEdit(edit, wnd, kEditId);
  Type("abc");
  Press(KEY_LEFT);
  Press(KEY_LEFT);
  Press(KEY_DELETE);
  EXPECT_EQ(EditText(edit), "ac");
  Type("X");
  EXPECT_EQ(EditText(edit), "aXc");
}

TEST_F(UIEditTest, DeleteAtTheEndDoesNothing) {
  CreateEdit(edit, wnd, kEditId);
  Type("abc");
  Press(KEY_DELETE);
  EXPECT_EQ(EditText(edit), "abc");
}

TEST_F(UIEditTest, RightArrowStopsAtTheEndOfTheText) {
  CreateEdit(edit, wnd, kEditId);
  Type("ab");
  Press(KEY_LEFT);
  Press(KEY_LEFT);
  Press(KEY_RIGHT);
  Type("X");
  EXPECT_EQ(EditText(edit), "aXb");

  Press(KEY_RIGHT);
  Press(KEY_RIGHT);
  Press(KEY_RIGHT);
  Type("Y");
  EXPECT_EQ(EditText(edit), "aXbY");
}

TEST_F(UIEditTest, InputStopsAtTheBufferLength) {
  CreateEdit(edit, wnd, kEditId);
  edit.SetBufferLen(4);
  Type("abcdef");
  EXPECT_EQ(EditText(edit), "abcd");

  Press(KEY_LEFT);
  Type("X");
  EXPECT_EQ(EditText(edit), "abcd");
}

TEST_F(UIEditTest, ShrinkingTheBufferTruncatesTheText) {
  CreateEdit(edit, wnd, kEditId);
  edit.SetText("hello");
  edit.SetBufferLen(3);
  EXPECT_EQ(EditText(edit), "hel");
}

TEST_F(UIEditTest, GetTextTruncatesToTheCallersBuffer) {
  CreateEdit(edit, wnd, kEditId);
  edit.SetText("hello");
  char buf[4] = {'x', 'x', 'x', 'x'};
  edit.GetText(buf, sizeof(buf));
  EXPECT_STREQ(buf, "hel");
}

TEST_F(UIEditTest, NumbersOnlyBoxIgnoresNonDigitKeys) {
  CreateEdit(edit, wnd, kEditId, UIED_NUMBERS);
  Type("1a2 b3");
  EXPECT_EQ(EditText(edit), "123");
}

TEST_F(UIEditTest, NumbersOnlyBoxKeepsOnlyTheLeadingNumberOfSetText) {
  CreateEdit(edit, wnd, kEditId, UIED_NUMBERS);
  edit.SetText("42abc");
  EXPECT_EQ(EditText(edit), "42");
}

TEST_F(UIEditTest, EnterSelectsTheBox) {
  CreateEdit(edit, wnd, kEditId);
  Type("ab");
  EXPECT_EQ(Press(KEY_ENTER), kEditId);
  EXPECT_EQ(EditText(edit), "ab");
}

TEST_F(UIEditTest, AutoSelectFiresWhenTheBufferBecomesFull) {
  CreateEdit(edit, wnd, kEditId, UIED_AUTOSELECT);
  edit.SetBufferLen(3);
  EXPECT_EQ(Press(ddio_AsciiToKey('a')), -1);
  EXPECT_EQ(Press(ddio_AsciiToKey('b')), -1);
  EXPECT_EQ(Press(ddio_AsciiToKey('c')), kEditId);
  EXPECT_EQ(EditText(edit), "abc");
}

} // namespace
